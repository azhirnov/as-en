// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "VulkanExtEmulation.h"

#include "base/CompileTime/Math.h"
#include "base/Containers/FixedMap.h"
#include "base/Containers/FixedTupleArray.h"
#include "base/Containers/FixedArray.h"
#include "base/Utils/HandleTmpl.h"
#include "base/Math/Color.h"
#include "base/Math/Random.h"

#include "threading/Primitives/DataRaceCheck.h"
#include "threading/Primitives/SpinLock.h"

#include "graphics/Public/Common.h"
#include "graphics/Vulkan/VulkanCheckError.h"

#ifdef AE_RELEASE
#	define ENABLE_DEBUG_CLEAR	0

#elif defined(AE_PLATFORM_ANDROID)
	// a lot of bugs on Android devices
#	define ENABLE_DEBUG_CLEAR	0
#else
#	define ENABLE_DEBUG_CLEAR	1
#endif

namespace AE::Graphics
{
namespace
{
	using namespace AE::Threading;

	enum class Extension : uint
	{
		Multiview			= 1 << 0,	// VK_KHR_multiview				or 1.1
		RenderPass2			= 1 << 1,	// VK_KHR_create_renderpass2	or 1.2	// requires Multiview
		Synchronization2	= 1 << 2,	// VK_KHR_synchronization2
		LoadStoreOpNone		= 1 << 3,	// VK_KHR_load_store_op_none | VK_EXT_load_store_op_none | VK_KHR_dynamic_rendering | VK_QCOM_render_pass_store_ops
		DebugClear			= 1 << 4,	// clear if don't care in render pass or if used undefined layout
		DebugMarker			= 1 << 5,
		_Last,
		All					= ((_Last-1) << 1) - 1,
		Unknown				= 0,
	};
	AE_BIT_OPERATORS( Extension );

	enum class SyncScope
	{
		First,
		Second
	};

	enum class EFeatures : uint
	{
		Tessellation	= 1 << 0,
		Geometry		= 1 << 1,
		Mesh			= 1 << 2,
		Task			= 1 << 3,
		Timeline		= 1 << 4,
		Unknown			= 0,
	};
	AE_BIT_OPERATORS( EFeatures );

	using SemaphoreID	= HandleTmpl< 32, 32, 0x654145 >;
	StaticAssert( sizeof(SemaphoreID) == sizeof(VkSemaphore) );

	using VkEE_Allocator_t = SmallLinearAllocator<>;



	//
	// Vulkan Emulator
	//
	class VulkanEmulation : public VulkanDeviceFn
	{
	// types
	public:
		struct PhysDevInfo
		{
			Extension	enabledExt	= Default;		// extensions which is not supported by device and will be emulated
			bool		initialized	= false;
			Version2	version;
		};
		using PhysDevInfoMap_t	= FixedMap< VkPhysicalDevice, PhysDevInfo, 8 >;

		using FnNameToPtrMap_t	= HashMap< StringView, PFN_vkVoidFunction >;


	// variables
	public:
		VkDevice										currentDevice						= Default;
		Extension										devEnabledExt						= Default;	// extensions which is not supported by device and will be emulated
		EFeatures										devFeatures							= Default;
		Version2										devVersion;
		VkPhysicalDeviceMultiviewFeatures				devMultiviewFeats					= {};
		VkPhysicalDeviceTimelineSemaphoreFeatures		devTimelineSemFeats					= {};
		VkPhysicalDeviceSynchronization2Features		devSync2Feats						= {};

		// device functions
		PFN_vkCmdPipelineBarrier						origin_vkCmdPipelineBarrier			= null;
		PFN_vkCmdPipelineBarrier2						origin_vkCmdPipelineBarrier2		= null;
		PFN_vkCreateRenderPass2							origin_vkCreateRenderPass2			= null;
		PFN_vkCreateRenderPass							origin_vkCreateRenderPass			= null;
		PFN_vkDestroyRenderPass							origin_vkDestroyRenderPass			= null;
		PFN_vkCmdBeginRenderPass2						origin_vkCmdBeginRenderPass2		= null;
		PFN_vkCmdNextSubpass2							origin_vkCmdNextSubpass2			= null;
		PFN_vkCmdEndRenderPass2							origin_vkCmdEndRenderPass2			= null;
		PFN_vkCreateImage								origin_vkCreateImage				= null;
		PFN_vkCmdClearColorImage						origin_vkCmdClearColorImage			= null;
		PFN_vkCmdClearDepthStencilImage					origin_vkCmdClearDepthStencilImage	= null;
		PFN_vkEndCommandBuffer							origin_vkEndCommandBuffer			= null;
		PFN_vkCreateFramebuffer							origin_vkCreateFramebuffer			= null;
		PFN_vkDestroyFramebuffer						origin_vkDestroyFramebuffer			= null;
		PFN_vkCreateImageView							origin_vkCreateImageView			= null;
		PFN_vkDestroyImageView							origin_vkDestroyImageView			= null;
		PFN_vkCmdWriteTimestamp							origin_vkCmdWriteTimestamp			= null;

		PhysDevInfoMap_t								physDeviceInfo;
		Version2										instanceVer;

		// instance functions
		PFN_vkGetInstanceProcAddr						origin_vkGetInstanceProcAddr				= null;
		PFN_vkGetDeviceProcAddr							origin_vkGetDeviceProcAddr					= null;
		PFN_vkEnumerateDeviceExtensionProperties		origin_vkEnumerateDeviceExtensionProperties	= null;
		PFN_vkCreateDevice								origin_vkCreateDevice						= null;
		PFN_vkDestroyDevice								origin_vkDestroyDevice						= null;
		PFN_vkGetPhysicalDeviceFeatures2				origin_vkGetPhysicalDeviceFeatures2			= null;
		PFN_vkGetPhysicalDeviceProperties2				origin_vkGetPhysicalDeviceProperties2		= null;

		FnNameToPtrMap_t					fnNameToPtr;

		struct DebugClear;
		Unique< DebugClear >				dbgClear;

		DRC_ONLY(
			RWDataRaceCheck					drCheck;
		)


	// methods
	public:
		explicit VulkanEmulation (VulkanDeviceFnTable* fnTable)								__NE___;
		~VulkanEmulation ()																	__NE___;

		void  InitDeviceInfo (VkPhysicalDevice physicalDevice, INOUT PhysDevInfo& info)		__NE___;

		ND_ VkPipelineStageFlags	ConvertVkPipelineStageFlags2 (VkPipelineStageFlags2 inStages, SyncScope scope)	C_NE___;
		ND_ VkAccessFlags			ConvertVkAccessFlags2 (VkAccessFlags2 inAccess, VkPipelineStageFlags2 inStages)	C_NE___;

		ND_ static VulkanEmulation&  Get ()													__NE___
		{
			static std::aligned_storage_t< sizeof(VulkanEmulation), alignof(VulkanEmulation) >	emulator;
			return *Cast<VulkanEmulation>( &emulator );
		}
	};
//-----------------------------------------------------------------------------


#	include "RenderPass2.h"
#	include "LoadStoreOpNone.h"
#	include "Synchronization2.h"
#	include "DebugClear.h"


	VulkanEmulation::VulkanEmulation (VulkanDeviceFnTable* fnTable) __NE___ : VulkanDeviceFn{fnTable} {}

	VulkanEmulation::~VulkanEmulation () __NE___ {}

/*
=================================================
	InitDeviceInfo
=================================================
*/
	void  VulkanEmulation::InitDeviceInfo (VkPhysicalDevice physicalDevice, INOUT PhysDevInfo& info) __NE___
	{
		ASSERT( not info.initialized );
		info.initialized = true;

		VkPhysicalDeviceProperties	props;
		vkGetPhysicalDeviceProperties( physicalDevice, OUT &props );

		const Version2	dev_ver = {VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion)};
		const Version2	vk_ver	= Min( dev_ver, instanceVer );

		uint	prop_count = 0;
		if ( origin_vkEnumerateDeviceExtensionProperties( physicalDevice, null, OUT &prop_count, null ) == VK_SUCCESS )
		{
			Array< VkExtensionProperties >	ext_props;
			NOTHROW_ERRV( ext_props.resize( prop_count ));

			VK_CHECK( origin_vkEnumerateDeviceExtensionProperties( physicalDevice, null, OUT &prop_count, OUT ext_props.data() ));

			Extension	exist_ext = Default;
			for (uint i = 0; i < prop_count and exist_ext != Extension::All; ++i)
			{
				StringView	ext_name {ext_props[i].extensionName};

				if ( ext_name == VK_KHR_MULTIVIEW_EXTENSION_NAME )
					exist_ext |= Extension::Multiview;

				if ( ext_name == VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME )
					exist_ext |= Extension::RenderPass2;

				if ( ext_name == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME )
					exist_ext |= Extension::Synchronization2;

				if ( ext_name == VK_KHR_LOAD_STORE_OP_NONE_EXTENSION_NAME )
					exist_ext |= Extension::LoadStoreOpNone;

				if ( ext_name == VK_EXT_DEBUG_MARKER_EXTENSION_NAME )
					exist_ext |= Extension::DebugMarker;
			}

			if ( vk_ver >= Version2{1,1} )
				exist_ext |= Extension::Multiview;

			if ( vk_ver >= Version2{1,2} )
				exist_ext |= Extension::RenderPass2;

			if ( vk_ver >= Version2{1,3} )
				exist_ext |= Extension::Synchronization2;

		  #if not ENABLE_DEBUG_CLEAR
			exist_ext |= Extension::DebugClear;		// disable
		  #endif

			info.enabledExt = ~exist_ext & Extension::All;
			info.version	= vk_ver;
		}
	}

/*
=================================================
	Wrap_vkEnumerateDeviceExtensionProperties
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkEnumerateDeviceExtensionProperties (VkPhysicalDevice physicalDevice, const char* pLayerName, INOUT uint* pPropertyCount, OUT VkExtensionProperties* pProperties)
	{
		auto&	emulator = VulkanEmulation::Get();

		if ( pLayerName != null )
		{
			DRC_SHAREDLOCK( emulator.drCheck );
			return emulator.origin_vkEnumerateDeviceExtensionProperties( physicalDevice, pLayerName, INOUT pPropertyCount, OUT pProperties );
		}

		Extension	enabled_ext;
		{
			DRC_EXLOCK( emulator.drCheck );
			auto&	info = emulator.physDeviceInfo.emplace( physicalDevice, VulkanEmulation::PhysDevInfo{} ).first->second;

			if_unlikely( not info.initialized )
				emulator.InitDeviceInfo( physicalDevice, INOUT info );

			enabled_ext = info.enabledExt;
		}

		uint&		count		= *pPropertyCount;
		const uint	capacity	= count;
		VkResult	result		= emulator.origin_vkEnumerateDeviceExtensionProperties( physicalDevice, pLayerName, INOUT pPropertyCount, OUT pProperties );

		// remove extensions which depends on emulated extensions (only for tests, actually they are not present)
		#if 1
		if ( pProperties != null )
		{
			FlatHashSet<StringView>		sync2_deps = {
				VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
				VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME,
				VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
				VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
				VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME,
				VK_ARM_RENDER_PASS_STRIPED_EXTENSION_NAME,
				VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME,
				VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME,
				VK_NV_OPTICAL_FLOW_EXTENSION_NAME,
			//	VK_AMDX_SHADER_ENQUEUE_EXTENSION_NAME,
				// second level deps:
				VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
				VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,
			//	VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME,
				VK_KHR_VIDEO_ENCODE_H264_EXTENSION_NAME,
				VK_KHR_VIDEO_ENCODE_H265_EXTENSION_NAME,
			};
			for (uint i = 0; i < count; ++i)
			{
				if ( sync2_deps.contains( StringView{pProperties[i].extensionName} ))
				{
					pProperties[i] = pProperties[count-1];
					--count;
					--i;
				}
			}
		}
		#endif

		// add emulated extensions
		if ( result == VK_SUCCESS and enabled_ext != Default )
		{
			for (auto ext_bit : BitfieldIterate( enabled_ext ))
			{
				const uint	idx	= count++;

				switch_enum( ext_bit )
				{
					case Extension::Multiview :
					{
						if ( pProperties != null )
						{
							if ( idx < capacity )
							{
								CopyCString( OUT pProperties[idx].extensionName, VK_KHR_MULTIVIEW_EXTENSION_NAME );
								pProperties[idx].specVersion = VK_KHR_MULTIVIEW_SPEC_VERSION;
							}
							else
								result = VK_INCOMPLETE;
						}
						break;
					}
					case Extension::RenderPass2 :
					{
						if ( pProperties != null )
						{
							if ( idx < capacity )
							{
								CopyCString( OUT pProperties[idx].extensionName, VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME );
								pProperties[idx].specVersion = VK_KHR_CREATE_RENDERPASS_2_SPEC_VERSION;
							}
							else
								result = VK_INCOMPLETE;
						}
						break;
					}
					case Extension::Synchronization2 :
					{
						if ( pProperties != null )
						{
							if ( idx < capacity )
							{
								CopyCString( OUT pProperties[idx].extensionName, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME );
								pProperties[idx].specVersion = VK_KHR_SYNCHRONIZATION_2_SPEC_VERSION;
							}
							else
								result = VK_INCOMPLETE;
						}
						break;
					}
					case Extension::LoadStoreOpNone :
					{
						if ( pProperties != null )
						{
							if ( idx < capacity )
							{
								CopyCString( OUT pProperties[idx].extensionName, VK_KHR_LOAD_STORE_OP_NONE_EXTENSION_NAME );
								pProperties[idx].specVersion = VK_KHR_LOAD_STORE_OP_NONE_SPEC_VERSION;
							}
							else
								result = VK_INCOMPLETE;
						}
						break;
					}
					case Extension::DebugMarker :
					{
						if ( pProperties != null )
						{
							if ( idx < capacity )
							{
								CopyCString( OUT pProperties[idx].extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME );
								pProperties[idx].specVersion = VK_EXT_DEBUG_MARKER_SPEC_VERSION;
							}
							else
								result = VK_INCOMPLETE;
						}
						break;
					}

					case Extension::DebugClear :
					case Extension::_Last :
					case Extension::All :
					case Extension::Unknown :
					default :
						break;
				}
				switch_end
			}
		}
		return result;
	}

/*
=================================================
	Wrap_vkDebugMarkerSetObjectNameEXT
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkDebugMarkerSetObjectNameEXT (VkDevice, const VkDebugMarkerObjectNameInfoEXT *)
	{
		return VK_SUCCESS;
	}

/*
=================================================
	Wrap_vkCreateDevice
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateDevice (VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_EXLOCK( emulator.drCheck );

		NonNull( pCreateInfo );
		ASSERT( pCreateInfo->sType == VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO );

		auto	iter = emulator.physDeviceInfo.find( physicalDevice );
		if ( iter != emulator.physDeviceInfo.end() )
		{
			emulator.devEnabledExt	= iter->second.enabledExt;
			emulator.devVersion		= iter->second.version;
		}

		VkPhysicalDeviceFeatures	dev_feat = {};
		vkGetPhysicalDeviceFeatures( physicalDevice, OUT &dev_feat );

		if ( dev_feat.tessellationShader )
			emulator.devFeatures |= EFeatures::Tessellation;

		if ( dev_feat.geometryShader )
			emulator.devFeatures |= EFeatures::Geometry;

		for (auto const* const* next = Cast<VkBaseInStructure*>(&pCreateInfo->pNext); *next != null;)
		{
			// remove emulated features
			if ( AllBits( emulator.devEnabledExt, Extension::Multiview ) and
				 (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES )
			{
				auto&	feats = *Cast<VkPhysicalDeviceMultiviewFeatures>(*next);
				emulator.devMultiviewFeats = feats;
				const_cast<const VkBaseInStructure*&>(*next) = (*next)->pNext;
				AE_LOG_DBG( "Enable Vulkan Multiview (RenderPass2) emulation" );
				continue;
			}
			if ( AllBits( emulator.devEnabledExt, Extension::Synchronization2 ) and
				 (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES )
			{
				auto&	feats = *Cast<VkPhysicalDeviceSynchronization2Features>(*next);
				emulator.devSync2Feats = feats;
				const_cast<const VkBaseInStructure*&>(*next) = (*next)->pNext;
				AE_LOG_DBG( "Enable Vulkan Synchronization2 emulation" );
				continue;
			}

			// enable features
			if ( (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT )
			{
				auto&	feats = *Cast<VkPhysicalDeviceMeshShaderFeaturesEXT>(*next);
				if ( feats.meshShader )
					emulator.devFeatures |= EFeatures::Mesh;
				if ( feats.taskShader )
					emulator.devFeatures |= EFeatures::Task;
			}
			if ( (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES )
			{
				auto&	feats = *Cast<VkPhysicalDeviceTimelineSemaphoreFeatures>(*next);
				if ( feats.timelineSemaphore )
					emulator.devFeatures |= EFeatures::Timeline;
			}

			next = &(*next)->pNext;
		}

		VkDeviceCreateInfo		dev_ci = *pCreateInfo;

		Array< const char* >	extensions;
		if ( dev_ci.ppEnabledExtensionNames != null and dev_ci.enabledExtensionCount > 0 )
		{
			NOTHROW_ERR( extensions.assign( dev_ci.ppEnabledExtensionNames, dev_ci.ppEnabledExtensionNames + dev_ci.enabledExtensionCount ),
					   VK_RESULT_MAX_ENUM );

			for (usize i = 0; i < extensions.size(); ++i)
			{
				StringView	ext_name {extensions[i]};

				if ( (AllBits( emulator.devEnabledExt, Extension::Multiview )		  and ext_name == VK_KHR_MULTIVIEW_EXTENSION_NAME)			 or
					 (AllBits( emulator.devEnabledExt, Extension::RenderPass2 )		  and ext_name == VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME) or
					 (AllBits( emulator.devEnabledExt, Extension::Synchronization2 )  and ext_name == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)	 or
					 (AllBits( emulator.devEnabledExt, Extension::LoadStoreOpNone )	  and ext_name == VK_KHR_LOAD_STORE_OP_NONE_EXTENSION_NAME)	 or
					 (AllBits( emulator.devEnabledExt, Extension::DebugMarker )		  and ext_name == VK_EXT_DEBUG_MARKER_EXTENSION_NAME) )
				{
					extensions.erase( extensions.begin() + i );
					--i;
				}
			}

			dev_ci.enabledExtensionCount	= uint(extensions.size());
			dev_ci.ppEnabledExtensionNames	= extensions.size() ? extensions.data() : null;
		}

		VkResult	result = emulator.origin_vkCreateDevice( physicalDevice, &dev_ci, pAllocator, OUT pDevice );
		if ( result != VK_SUCCESS )
			return result;

		emulator.currentDevice = *pDevice;

		emulator.origin_vkCmdPipelineBarrier  = BitCast<PFN_vkCmdPipelineBarrier >(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCmdPipelineBarrier" ));
		emulator.origin_vkCmdPipelineBarrier2 = emulator.devVersion >= Version2{1,3} ?
			BitCast<PFN_vkCmdPipelineBarrier2>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCmdPipelineBarrier2" )) :
			BitCast<PFN_vkCmdPipelineBarrier2>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCmdPipelineBarrier2KHR" ));

		const bool	use_rp2_core				= emulator.devVersion >= Version2{1,2};
		emulator.origin_vkCreateRenderPass		= BitCast<PFN_vkCreateRenderPass>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCreateRenderPass" ));
		emulator.origin_vkDestroyRenderPass		= BitCast<PFN_vkDestroyRenderPass>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkDestroyRenderPass" ));
		emulator.origin_vkCreateRenderPass2		= BitCast<PFN_vkCreateRenderPass2>(emulator.origin_vkGetDeviceProcAddr( *pDevice, use_rp2_core ? "vkCreateRenderPass2" : "vkCreateRenderPass2KHR" ));
		emulator.origin_vkCmdBeginRenderPass2	= BitCast<PFN_vkCmdBeginRenderPass2>(emulator.origin_vkGetDeviceProcAddr( *pDevice, use_rp2_core ? "vkCmdBeginRenderPass2" : "vkCmdBeginRenderPass2KHR" ));
		emulator.origin_vkCmdNextSubpass2		= BitCast<PFN_vkCmdNextSubpass2>(emulator.origin_vkGetDeviceProcAddr( *pDevice, use_rp2_core ? "vkCmdNextSubpass2" : "vkCmdNextSubpass2KHR" ));
		emulator.origin_vkCmdEndRenderPass2		= BitCast<PFN_vkCmdEndRenderPass2>(emulator.origin_vkGetDeviceProcAddr( *pDevice, use_rp2_core ? "vkCmdEndRenderPass2" : "vkCmdEndRenderPass2KHR" ));

		emulator.origin_vkCreateImage			= BitCast<PFN_vkCreateImage>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCreateImage" ));

		emulator.origin_vkCmdClearColorImage		= BitCast<PFN_vkCmdClearColorImage>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCmdClearColorImage" ));
		emulator.origin_vkCmdClearDepthStencilImage	= BitCast<PFN_vkCmdClearDepthStencilImage>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCmdClearDepthStencilImage" ));
		emulator.origin_vkEndCommandBuffer			= BitCast<PFN_vkEndCommandBuffer>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkEndCommandBuffer" ));

		emulator.origin_vkCreateFramebuffer			= BitCast<PFN_vkCreateFramebuffer>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCreateFramebuffer" ));
		emulator.origin_vkDestroyFramebuffer		= BitCast<PFN_vkDestroyFramebuffer>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkDestroyFramebuffer" ));
		emulator.origin_vkCreateImageView			= BitCast<PFN_vkCreateImageView>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCreateImageView" ));
		emulator.origin_vkDestroyImageView			= BitCast<PFN_vkDestroyImageView>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkDestroyImageView" ));

		emulator.origin_vkCmdWriteTimestamp		= BitCast<PFN_vkCmdWriteTimestamp>(emulator.origin_vkGetDeviceProcAddr( *pDevice, "vkCmdWriteTimestamp" ));

		emulator.fnNameToPtr.clear();

		// same reverse order as for '#include "feature"'

		if ( AllBits( emulator.devEnabledExt, Extension::DebugMarker ))
		{
			emulator.fnNameToPtr.emplace( "vkDebugMarkerSetObjectNameEXT",	BitCast<PFN_vkVoidFunction>( &Wrap_vkDebugMarkerSetObjectNameEXT ));
		}

		if ( AllBits( emulator.devEnabledExt, Extension::DebugClear ))
		{
			CHECK( emulator.origin_vkCmdPipelineBarrier != null );
			CHECK( emulator.origin_vkDestroyRenderPass != null );
			CHECK( emulator.origin_vkCreateImage != null );
			CHECK( emulator.origin_vkCmdClearColorImage != null );
			CHECK( emulator.origin_vkCmdClearDepthStencilImage != null );
			CHECK( emulator.origin_vkEndCommandBuffer != null );
			CHECK( emulator.origin_vkCreateFramebuffer != null and emulator.origin_vkDestroyFramebuffer != null );
			CHECK( emulator.origin_vkCreateImageView != null and emulator.origin_vkDestroyImageView != null );

			emulator.dbgClear.reset( new VulkanEmulation::DebugClear{} );

			emulator.fnNameToPtr.emplace( "vkCmdBeginRenderPass2KHR",	BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdBeginRenderPass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdBeginRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdBeginRenderPass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdEndRenderPass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdEndRenderPass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdEndRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdEndRenderPass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdNextSubpass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdNextSubpass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdNextSubpass2",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdNextSubpass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdPipelineBarrier2KHR",	BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdPipelineBarrier2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdPipelineBarrier2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdPipelineBarrier2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCmdPipelineBarrier",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdPipelineBarrier_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkDestroyRenderPass",		BitCast<PFN_vkVoidFunction>( &Wrap_vkDestroyRenderPass_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCreateImage",				BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateImage_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkEndCommandBuffer",			BitCast<PFN_vkVoidFunction>( &Wrap_vkEndCommandBuffer_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCreateFramebuffer",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateFramebuffer_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkDestroyFramebuffer",		BitCast<PFN_vkVoidFunction>( &Wrap_vkDestroyFramebuffer_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkCreateImageView",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateImageView_DbgClear ));
			emulator.fnNameToPtr.emplace( "vkDestroyImageView",			BitCast<PFN_vkVoidFunction>( &Wrap_vkDestroyImageView_DbgClear ));
		}

		if ( AllBits( emulator.devEnabledExt, Extension::Synchronization2 ))
		{
			emulator.fnNameToPtr.emplace( "vkCmdPipelineBarrier2KHR",	BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdPipelineBarrier2 ));
			emulator.fnNameToPtr.emplace( "vkCmdPipelineBarrier2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdPipelineBarrier2 ));
			emulator.fnNameToPtr.emplace( "vkCmdResetEvent2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdResetEvent2 ));
			emulator.fnNameToPtr.emplace( "vkCmdResetEvent2",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdResetEvent2 ));
			emulator.fnNameToPtr.emplace( "vkCmdSetEvent2KHR",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdSetEvent2 ));
			emulator.fnNameToPtr.emplace( "vkCmdSetEvent2",				BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdSetEvent2 ));
			emulator.fnNameToPtr.emplace( "vkCmdWaitEvents2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdWaitEvents2 ));
			emulator.fnNameToPtr.emplace( "vkCmdWaitEvents2",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdWaitEvents2 ));
			emulator.fnNameToPtr.emplace( "vkCmdWriteTimestamp",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdWriteTimestamp ));
			emulator.fnNameToPtr.emplace( "vkCmdWriteTimestamp2KHR",	BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdWriteTimestamp2 ));
			emulator.fnNameToPtr.emplace( "vkCmdWriteTimestamp2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdWriteTimestamp2 ));
			emulator.fnNameToPtr.emplace( "vkQueueSubmit2KHR",			BitCast<PFN_vkVoidFunction>( &Wrap_vkQueueSubmit2 ));
			emulator.fnNameToPtr.emplace( "vkQueueSubmit2",				BitCast<PFN_vkVoidFunction>( &Wrap_vkQueueSubmit2 ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2_Sync2 ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2_Sync2 ));
		}

		if ( AllBits( emulator.devEnabledExt, Extension::LoadStoreOpNone ))
		{
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2_OpNone ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2_OpNone ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass_OpNone ));
		}

		if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
		{
			emulator.fnNameToPtr.emplace( "vkCmdBeginRenderPass2KHR",	BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdBeginRenderPass2 ));
			emulator.fnNameToPtr.emplace( "vkCmdBeginRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdBeginRenderPass2 ));
			emulator.fnNameToPtr.emplace( "vkCmdEndRenderPass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdEndRenderPass2 ));
			emulator.fnNameToPtr.emplace( "vkCmdEndRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdEndRenderPass2 ));
			emulator.fnNameToPtr.emplace( "vkCmdNextSubpass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdNextSubpass2 ));
			emulator.fnNameToPtr.emplace( "vkCmdNextSubpass2",			BitCast<PFN_vkVoidFunction>( &Wrap_vkCmdNextSubpass2 ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2KHR",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2 ));
			emulator.fnNameToPtr.emplace( "vkCreateRenderPass2",		BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateRenderPass2 ));
		}

		return result;
	}

/*
=================================================
	Wrap_vkDestroyDevice
=================================================
*/
	VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyDevice (VkDevice device, const VkAllocationCallbacks* pAllocator)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_EXLOCK( emulator.drCheck );

		emulator.origin_vkDestroyDevice( device, pAllocator );

		CHECK( emulator.currentDevice == device );
		emulator.currentDevice		= Default;
		emulator.devEnabledExt		= Default;
		emulator.devFeatures		= Default;
		emulator.devVersion			= {};
		emulator.devMultiviewFeats	= {};
		emulator.devTimelineSemFeats= {};
		emulator.devSync2Feats		= {};

		emulator.fnNameToPtr.clear();
		emulator.dbgClear.reset();

		//if ( emulator.tls )
		//	emulator.tls->Release( emulator );

		//emulator.tls.reset();
	}

/*
=================================================
	Wrap_vkGetPhysicalDeviceFeatures2
=================================================
*/
	VKAPI_ATTR void VKAPI_CALL Wrap_vkGetPhysicalDeviceFeatures2 (VkPhysicalDevice physicalDevice, OUT VkPhysicalDeviceFeatures2* pFeatures)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		Extension	enabled_ext = Default;
		{
			auto	iter = emulator.physDeviceInfo.find( physicalDevice );
			if ( iter != emulator.physDeviceInfo.end() )
				enabled_ext	= iter->second.enabledExt;
		}

		for (auto** next = Cast<VkBaseOutStructure*>(&pFeatures->pNext); *next != null;)
		{
			if ( AllBits( enabled_ext, Extension::Multiview ) and (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES )
			{
				auto&	feats = *Cast<VkPhysicalDeviceMultiviewFeatures>(*next);
				feats.multiview						= VK_FALSE;
				feats.multiviewGeometryShader		= VK_FALSE;
				feats.multiviewTessellationShader	= VK_FALSE;
				*next = (*next)->pNext;
				continue;
			}
			if ( AllBits( enabled_ext, Extension::Synchronization2 ) and (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES )
			{
				auto&	feats = *Cast<VkPhysicalDeviceSynchronization2Features>(*next);
				feats.synchronization2 = VK_TRUE;
				*next = (*next)->pNext;
				continue;
			}

			next = &(*next)->pNext;
		}

		if ( emulator.origin_vkGetPhysicalDeviceFeatures2 != null )
			emulator.origin_vkGetPhysicalDeviceFeatures2( physicalDevice, pFeatures );
	}

/*
=================================================
	Wrap_vkGetPhysicalDeviceProperties2
=================================================
*/
	VKAPI_ATTR void VKAPI_CALL Wrap_vkGetPhysicalDeviceProperties2 (VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		Extension	enabled_ext = Default;
		{
			auto	iter = emulator.physDeviceInfo.find( physicalDevice );
			if ( iter != emulator.physDeviceInfo.end() )
				enabled_ext	= iter->second.enabledExt;
		}

		for (auto** next = Cast<VkBaseOutStructure*>(&pProperties->pNext); *next != null;)
		{
			if ( AllBits( enabled_ext, Extension::Multiview ) and (*next)->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES )
			{
				auto&	props = *Cast<VkPhysicalDeviceMultiviewProperties>(*next);
				props.maxMultiviewViewCount		= 0;
				props.maxMultiviewInstanceIndex	= 0;
				*next = (*next)->pNext;
				continue;
			}

			next = &(*next)->pNext;
		}

		if ( emulator.origin_vkGetPhysicalDeviceProperties2 != null )
			emulator.origin_vkGetPhysicalDeviceProperties2( physicalDevice, pProperties );
	}

/*
=================================================
	Wrap_vkGetInstanceProcAddr
=================================================
*/
	VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL Wrap_vkGetInstanceProcAddr (VkInstance instance, const char* pName)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		const StringView	name{pName};

		if ( name == "vkEnumerateDeviceExtensionProperties" )
			return BitCast<PFN_vkVoidFunction>( &Wrap_vkEnumerateDeviceExtensionProperties );

		if ( name == "vkCreateDevice" )
			return BitCast<PFN_vkVoidFunction>( &Wrap_vkCreateDevice );

		if ( name == "vkDestroyDevice" )
			return BitCast<PFN_vkVoidFunction>( &Wrap_vkDestroyDevice );

		if ( name == "vkGetPhysicalDeviceFeatures2" or name == "vkGetPhysicalDeviceFeatures2KHR" )
			return BitCast<PFN_vkVoidFunction>( &Wrap_vkGetPhysicalDeviceFeatures2 );

		if ( name == "vkGetPhysicalDeviceProperties2" or name == "vkGetPhysicalDeviceProperties2KHR" )
			return BitCast<PFN_vkVoidFunction>( &Wrap_vkGetPhysicalDeviceProperties2 );

		return emulator.origin_vkGetInstanceProcAddr( instance, pName );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	Wrap_vkGetDeviceProcAddr
=================================================
*/
	VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL Wrap_vkGetDeviceProcAddr (VkDevice device, const char* pName)
	{
		CHECK_ERR( pName != null );

		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		auto	it = emulator.fnNameToPtr.find( StringView{ pName });
		if ( it != emulator.fnNameToPtr.end() )
			return it->second;

		return emulator.origin_vkGetDeviceProcAddr( device, pName );
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VulkanExtEmulation::VulkanExtEmulation (VulkanDeviceFnTable* fnTable) __NE___
	{
		auto&	emulator = VulkanEmulation::Get();
		PlacementNew<VulkanEmulation>( OUT &emulator, fnTable );
	}

/*
=================================================
	destructor
=================================================
*/
	VulkanExtEmulation::~VulkanExtEmulation () __NE___
	{
		VulkanEmulation::Get().~VulkanEmulation();
	}

/*
=================================================
	OnInitialize
=================================================
*/
	void  VulkanExtEmulation::OnInitialize () __NE___
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_EXLOCK( emulator.drCheck );

		emulator.origin_vkGetInstanceProcAddr	= _var_vkGetInstanceProcAddr;
		_var_vkGetInstanceProcAddr				= &Wrap_vkGetInstanceProcAddr;
	}

/*
=================================================
	OnLoadInstance
=================================================
*/
	void  VulkanExtEmulation::OnLoadInstance (VkInstance instance, Version2 instVer) __NE___
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_EXLOCK( emulator.drCheck );

		emulator.physDeviceInfo.clear();
		emulator.fnNameToPtr.clear();

		const bool	is11 = instVer >= Version2{1,1};

		emulator.instanceVer				= instVer;
		emulator.origin_vkGetDeviceProcAddr	= _var_vkGetDeviceProcAddr;
		_var_vkGetDeviceProcAddr			= &Wrap_vkGetDeviceProcAddr;

		emulator.origin_vkEnumerateDeviceExtensionProperties =
			BitCast<PFN_vkEnumerateDeviceExtensionProperties>( emulator.origin_vkGetInstanceProcAddr( instance, "vkEnumerateDeviceExtensionProperties" ));
		emulator.origin_vkCreateDevice =
			BitCast<PFN_vkCreateDevice>( emulator.origin_vkGetInstanceProcAddr( instance, "vkCreateDevice" ));
		emulator.origin_vkDestroyDevice =
			BitCast<PFN_vkDestroyDevice>( emulator.origin_vkGetInstanceProcAddr( instance, "vkDestroyDevice" ));
		emulator.origin_vkGetPhysicalDeviceFeatures2 =
			BitCast<PFN_vkGetPhysicalDeviceFeatures2>( emulator.origin_vkGetInstanceProcAddr( instance, is11 ? "vkGetPhysicalDeviceFeatures2" : "vkGetPhysicalDeviceFeatures2KHR" ));
		emulator.origin_vkGetPhysicalDeviceProperties2 =
			BitCast<PFN_vkGetPhysicalDeviceProperties2>( emulator.origin_vkGetInstanceProcAddr( instance, is11 ? "vkGetPhysicalDeviceProperties2" : "vkGetPhysicalDeviceProperties2KHR" ));
	}

/*
=================================================
	OnUnload
=================================================
*/
	void  VulkanExtEmulation::OnUnload () __NE___
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_EXLOCK( emulator.drCheck );

		_var_vkGetInstanceProcAddr = emulator.origin_vkGetInstanceProcAddr;
	}

} // AE::Graphics
