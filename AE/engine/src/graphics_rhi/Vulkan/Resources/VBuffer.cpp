// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VBuffer.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/Utils/NextChain.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VBuffer::~VBuffer () __NE___
	{
		ASSERT( _buffer == Default );
		ASSERT( _memoryId == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VBuffer::Create (ResourceManager &resMngr, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( _buffer == Default );
		CHECK_ERR( _memoryId == Default );
		CHECK_ERR( desc.size > 0 );
		CHECK_ERR( desc.usage != Default );

		_desc = desc;
		_desc.Validate();
		GRES_CHECK( IsSupported( resMngr, _desc ));

		auto&	dev = resMngr.GetDevice();

		VkBufferUsageFlags2CreateInfoKHR	flags2_ci = {};
		flags2_ci.sType	= VK_STRUCTURE_TYPE_BUFFER_USAGE_FLAGS_2_CREATE_INFO;
		flags2_ci.usage	= VEnumCast( _desc.usage );

		// create buffer
		VkBufferCreateInfo	info = {};
		VNextChain			p_next {info};
		info.sType	= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags	= 0;
		info.usage	= VkBufferUsageFlags( flags2_ci.usage );
		info.size	= VkDeviceSize{ _desc.size };

		if ( dev.GetVExtensions().maintenance5 ){
			p_next.Add( flags2_ci );
		}else{
			CHECK_ERR_MSG( flags2_ci.usage == info.usage,
				"Some buffer usage flags requires 'maintenance5' extension" );
		}

		if_unlikely( EMemoryType_IsNonCoherent( desc.memType ))
			info.size = AlignUp( info.size, dev.GetDeviceProperties().res.minNonCoherentAtomSize );

		VQueueFamilyIndices_t	queue_family_indices;

		// setup sharing mode
		if ( _desc.queues != Default )
		{
			dev.GetQueueFamilies( _desc.queues, OUT queue_family_indices );

			info.sharingMode			= VK_SHARING_MODE_CONCURRENT;
			info.pQueueFamilyIndices	= queue_family_indices.data();
			info.queueFamilyIndexCount	= uint(queue_family_indices.size());
		}

		// reset to exclusive mode
		if ( info.queueFamilyIndexCount <= 1 )
		{
			info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
			info.pQueueFamilyIndices	= null;
			info.queueFamilyIndexCount	= 0;
		}

		VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &info, null, OUT &_buffer ));

		_memoryId = resMngr.CreateMemoryObj( _buffer, _desc, RVRef(allocator), dbgName );
		CHECK_ERR( _memoryId );

		dev.SetObjectName( _buffer, dbgName, VK_OBJECT_TYPE_BUFFER );

		CHECK_ERR( _InitDeviceAddress( dev ));

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VBuffer::Create (ResourceManager &resMngr, const VulkanBufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( _buffer == Default );
		CHECK_ERR( _memoryId == Default );
		CHECK_ERR( desc.buffer != Default );
		CHECK_ERR( desc.usage != Zero );
		ASSERT( desc.memFlags != Zero );

		EVideoBufferUsage	vb_usage;

		_buffer			= desc.buffer;
		_desc.size		= desc.size;
		_desc.memType	= AEEnumCast( desc.memFlags, not desc.canBeDestroyed );
		_desc.queues	= desc.queues;

		CHECK( AEEnumCast( desc.usage, OUT _desc.usage, OUT vb_usage ));

		if ( AnyBits( _desc.usage, EBufferUsage_RequireDeviceLocal ))
			GRES_CHECK( AllBits( _desc.memType, EMemoryType::DeviceLocal ));	// specified memory type is not valid

		_desc.Validate();
		GRES_CHECK( IsSupported( resMngr, _desc ));

		if ( desc.allocMemory )
		{
			CHECK_ERR( allocator );
			_memoryId = resMngr.CreateMemoryObj( _buffer, _desc, RVRef(allocator), dbgName );
			CHECK_ERR( _memoryId );
		}

		const auto&		dev = resMngr.GetDevice();
		dev.SetObjectName( _buffer, dbgName, VK_OBJECT_TYPE_BUFFER );

		CHECK_ERR( _InitDeviceAddress( dev ));

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	_InitDeviceAddress
=================================================
*/
	bool  VBuffer::_InitDeviceAddress (const VDevice &dev) __NE___
	{
		if ( AnyBits( _desc.usage, EBufferUsage_RequireDevAddress ))
		{
			VkBufferDeviceAddressInfo	addr_info = {};
			addr_info.sType		= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			addr_info.buffer	= _buffer;

			_address = BitCast<DeviceAddress>( dev.vkGetBufferDeviceAddressKHR( dev.GetVkDevice(), &addr_info ));
			CHECK_ERR( _address != Default );

			GFX_DBG_ONLY(
				Bytes	align = GetMemoryAlignment( dev, _desc );
				CHECK( IsMultipleOf( BitCast<ulong>(_address), align ));
			)
		}
		return true;
	}

/*
=================================================
	Destroy
----
	Immediately destroy buffer and free memory,
	is it safe because 'ResourceManager' will delay destruction by N frames.
	For more info see 'ResourceManager::ReleaseExpiredResourcesTask'.
=================================================
*/
	void  VBuffer::Destroy (ResourceManager &resMngr) __NE___
	{
		const bool	is_internal = NoBits( _desc.memType, EMemoryType::_External );
		auto&		dev			= resMngr.GetDevice();

		if ( is_internal and _buffer != Default )
			dev.vkDestroyBuffer( dev.GetVkDevice(), _buffer, null );

		resMngr.ImmediatelyRelease( INOUT _memoryId );

		_memoryId	= Default;
		_buffer		= Default;
		_address	= Default;
		_desc		= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetNativeDescription
=================================================
*/
	VulkanBufferDesc  VBuffer::GetNativeDescription () C_NE___
	{
		VulkanBufferDesc	desc;
		desc.buffer			= _buffer;
		desc.usage			= VEnumCast( _desc.usage );
		desc.size			= _desc.size;
		desc.queues			= _desc.queues;
		desc.memFlags		= VEnumCast( _desc.memType );
		desc.canBeDestroyed	= NoBits( _desc.memType, EMemoryType::_External );
		return desc;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VBuffer::IsSupported (const ResourceManager &resMngr, const BufferDesc &desc) __NE___
	{
		return Buffer_IsSupported( resMngr, desc );
		/*
			case EBufferOpt::SparseResidency :			// checked in Buffer_IsSupported()
			case EBufferOpt::SparseAliased :			// checked in Buffer_IsSupported()
			case EBufferOpt::VertexPplnStore :			// checked in FeatureSet
			case EBufferOpt::FragmentPplnStore :		// checked in FeatureSet
			case EBufferOpt::StorageTexelAtomic :		// checked in buffer view
			case EBufferOpt::SparseResidencyAliased :	// checked in Buffer_IsSupported()
		*/
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VBuffer::IsSupported (const ResourceManager &resMngr, const BufferDesc &desc, const BufferViewDesc &view) __NE___
	{
		StaticAssert( uint(EBufferUsage::All) == 0xFFFF );
		StaticAssert( uint(EBufferOpt::All) == 0x1F );

		if_unlikely( not BufferView_IsSupported( resMngr, desc, view ))
			return false;

		const auto&		dev		= resMngr.GetDevice();
		const auto&		props	= dev.GetDeviceProperties();

		VkFormatProperties	fmt_props = {};
		vkGetPhysicalDeviceFormatProperties( dev.GetVkPhysicalDevice(), VEnumCast( view.format ), OUT &fmt_props );
		// TODO: VK_KHR_format_feature_flags2

		const VkFormatFeatureFlags	available_flags	= fmt_props.bufferFeatures;
		VkFormatFeatureFlags		required_flags	= 0;

		if ( AllBits( desc.usage, EBufferUsage::UniformTexel ))
			required_flags |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

		if ( AllBits( desc.usage, EBufferUsage::StorageTexel ))
		{
			required_flags |= VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

			if ( AllBits( desc.options, EBufferOpt::StorageTexelAtomic ))
				required_flags |= VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
		}

		if_unlikely( not AllBits( available_flags, required_flags ))
			return false;

		if_unlikely( AllBits( desc.usage, EBufferUsage::UniformTexel )						and
					 not IsMultipleOf( view.offset, props.res.minUniformTexelBufferOffsetAlign ))
			return false;

		if_unlikely( AllBits( desc.usage, EBufferUsage::StorageTexel )						and
					 not IsMultipleOf( view.offset, props.res.minStorageTexelBufferOffsetAlign ))
			return false;

		return true;
	}

/*
=================================================
	GetMemoryAlignment
=================================================
*/
	Bytes  VBuffer::GetMemoryAlignment (const VDevice &dev, const BufferDesc &desc) __NE___
	{
		const auto&		props		= dev.GetDeviceProperties();
		const auto&		rt_props	= dev.GetVProperties().rayTracingPipelineProps;	// TODO: use DeviceProperties::RayTracingProperties
		Bytes			align		= 4_b;

		if ( EMemoryType_IsNonCoherent( desc.memType ))
			align = Max( align, props.res.minNonCoherentAtomSize );

		for (auto t : BitfieldIterate( desc.usage ))
		{
			switch_enum( t )
			{
				case EBufferUsage::UniformTexel :
					align = Max( align, props.res.minUniformTexelBufferOffsetAlign );
					break;

				case EBufferUsage::StorageTexel :
					align = Max( align, props.res.minStorageTexelBufferOffsetAlign );
					break;

				case EBufferUsage::Uniform :
					align = Max( align, props.res.minUniformBufferOffsetAlign );
					break;

				case EBufferUsage::Storage :
				case EBufferUsage::ShaderAddress :		// TODO ?
					align = Max( align, props.res.minStorageBufferOffsetAlign );
					break;

				case EBufferUsage::ASBuild_ReadOnly :
					align = Max( align,
								 props.rayTracing.vertexDataAlign, props.rayTracing.indexDataAlign,
								 props.rayTracing.aabbDataAlign, props.rayTracing.transformDataAlign,
								 props.rayTracing.instanceDataAlign );
					break;

				case EBufferUsage::ASBuild_Scratch :
					align = Max( align, props.rayTracing.scratchBufferAlign );
					break;

				case EBufferUsage::ShaderBindingTable :
					align = Max( align, rt_props.shaderGroupBaseAlignment, rt_props.shaderGroupHandleAlignment );
					break;

				case EBufferUsage::MMBuild_ReadOnly :
					align = Max( align, 256_b );	// from specs
					break;

				case EBufferUsage::RTAS_Storage :
				case EBufferUsage::ICB_Preprocess :
					break;								// TODO ?

				case EBufferUsage::TransferSrc :
				case EBufferUsage::TransferDst :
				case EBufferUsage::Index :
				case EBufferUsage::Vertex :
				case EBufferUsage::Indirect :			break;

				case EBufferUsage::_Last :
				case EBufferUsage::All :
				case EBufferUsage::Transfer :
				case EBufferUsage::Unknown :
				default_unlikely :						DBG_WARNING( "unknown buffer usage" );	break;
			}
			switch_end
		}
		ASSERT( IsPowerOfTwo( align ));
		return align;
	}

/*
=================================================
	IsSupportedForVertex
=================================================
*/
	bool  VBuffer::IsSupportedForVertex (const ResourceManager &resMngr, EVertexType type) __NE___
	{
	#if 1
		return resMngr.GetFeatureSet().vertexFormats.contains( type );

	#else
		VkFormatProperties	props = {};
		vkGetPhysicalDeviceFormatProperties( resMngr.GetDevice().GetVkPhysicalDevice(), VEnumCast( type ), OUT &props );

		return (props.bufferFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
	#endif
	}

/*
=================================================
	IsSupportedForASVertex
=================================================
*/
	bool  VBuffer::IsSupportedForASVertex (const ResourceManager &resMngr, EVertexType type) __NE___
	{
	#if 1
		return resMngr.GetFeatureSet().accelStructVertexFormats.contains( type );

	#else
		const auto&	dev	= resMngr.GetDevice();

		if_unlikely( not dev.GetVProperties().accelerationStructureFeats.accelerationStructure )
			return false;

		VkFormatProperties	props = {};
		vkGetPhysicalDeviceFormatProperties( dev.GetVkPhysicalDevice(), VEnumCast( type ), OUT &props );

		return (props.bufferFeatures & VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR);
	#endif
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
