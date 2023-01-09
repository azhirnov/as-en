// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "base/Utils/Version.h"
# include "base/CompileTime/StringToID.h"
# include "graphics/Public/DeviceProperties.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Device
	//

	class VDevice : public VulkanDeviceFn, public Noncopyable
	{
	// types
	public:
		using InstanceVersion	= TVersion2< "VkInstance"_StringToID >;
		using DeviceVersion		= TVersion2< "VkDevice"_StringToID >;
		using SpirvVersion		= TVersion2< "SPIRV"_StringToID >;
		
		#define VKFEATS_STRUCT
		#include "vulkan_loader/vk_features.h"
		#undef  VKFEATS_STRUCT

		// contains all available resource usage & options and memory types
		struct ResourceFlags
		{
			EBufferUsage	bufferUsage		= Default;
			EBufferOpt		bufferOptions	= Default;

			EImageUsage		imageUsage		= Default;
			EImageOpt		imageOptions	= Default;

			EnumBitSet<EDescriptorType>	descrTypes;

			FixedSet<EMemoryType, 8>	memTypes;
		};


	protected:
		using Queues_t			= FixedArray< VQueue, VConfig::MaxQueues >;
		using QueueTypes_t		= StaticArray< VQueuePtr, uint(EQueueType::_Count) >;
		
		using ExtensionName_t	= FixedString<VK_MAX_EXTENSION_NAME_SIZE>;
		using ExtensionSet_t	= FlatHashSet< ExtensionName_t >;
		
		static constexpr uint	MaxQueueFamilies = 32;
		using QueueFamilyProperties_t	= FixedArray< VkQueueFamilyProperties, MaxQueueFamilies >;
		using QueueCount_t				= FixedArray< uint, MaxQueueFamilies >;


	// variables
	protected:
		VkDevice				_vkLogicalDevice		= Default;
		
		EQueueMask				_queueMask				= Default;
		QueueTypes_t			_queueTypes				= {};
		Queues_t				_queues;

		VkPhysicalDevice		_vkPhysicalDevice		= Default;
		VkInstance				_vkInstance				= Default;
		InstanceVersion			_vkInstanceVersion;
		DeviceVersion			_vkDeviceVersion;
		SpirvVersion			_spirvVersion;
		
		DeviceProperties		_devProps;		// platform independent
		VExtensions				_extensions;

		VulkanDeviceFnTable		_deviceFnTable;
		
		VProperties				_properties;
		ResourceFlags			_resFlags;
		
		ExtensionSet_t			_instanceExtensions;
		ExtensionSet_t			_deviceExtensions;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		VDevice ();
		~VDevice ();
		
		ND_ VExtensions const&		GetExtensions ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _extensions; }
		ND_ VProperties const&		GetProperties ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _properties; }
		ND_ ResourceFlags const&	GetResourceFlags ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _resFlags; }
		ND_ DeviceProperties const&	GetDeviceProperties ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _devProps; }

		ND_ VkDevice				GetVkDevice ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkLogicalDevice; }
		ND_ VkPhysicalDevice		GetVkPhysicalDevice ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkPhysicalDevice; }
		ND_ VkInstance				GetVkInstance ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkInstance; }
		ND_ InstanceVersion			GetInstanceVersion ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkInstanceVersion; }
		ND_ DeviceVersion			GetDeviceVersion ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkDeviceVersion; }
		ND_ SpirvVersion			GetSpirvVersion ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _spirvVersion; }
		ND_ Version2				GetVkVersion ()				C_NE___	{ return GetDeviceVersion().Cast<0>(); }
		ND_ ArrayView<VQueue>		GetQueues ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _queues; }
		ND_ VQueuePtr				GetQueue (EQueueType type)	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint(type) < _queueTypes.size() ? _queueTypes[uint(type)] : null; }
		ND_ EQueueMask				GetAvailableQueues ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _queueMask; }

		ND_ bool					IsInitialized ()			C_NE___	{ return GetVkDevice() != Default; }

		// check extensions
		ND_ bool  HasInstanceExtension (StringView name)		C_NE___;
		ND_ bool  HasDeviceExtension (StringView name)			C_NE___;
		
			bool  SetObjectName (ulong id, NtStringView name, VkObjectType type)							C_NE___;

			void  GetQueueFamilies (EQueueMask mask, OUT VQueueFamilyIndices_t &)							C_NE___;
		
		ND_ bool  GetMemoryTypeIndex (uint memoryTypeBits, VkMemoryPropertyFlagBits includeFlags, VkMemoryPropertyFlagBits optFlags,
									  VkMemoryPropertyFlagBits excludeFlags, OUT uint &memoryTypeIndex)		C_NE___;
		ND_ bool  GetMemoryTypeIndex (uint memoryTypeBits, EMemoryType memType, OUT uint &memoryTypeIndex)	C_NE___;
		
		ND_ bool  CheckConstantLimits ()						C_NE___;
		ND_ bool  CheckExtensions ()							C_NE___;

			void  InitFeatureSet (OUT FeatureSet &)				C_NE___;
	};



	//
	// Vulkan Device Initializer
	//

	class VDeviceInitializer final : public VDevice
	{
	// types
	public:
		struct QueueCreateInfo
		{
		// variables
			VkQueueFlagBits				includeFlags	= Zero;
			VkQueueFlagBits				excludeFlags	= Zero;
			float						priority		= 0.0f;
			VkQueueGlobalPriorityKHR	globalPriority	= VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_KHR;
			FixedString<64>				debugName;

		// methods
			QueueCreateInfo () __NE___ {}
			
			explicit QueueCreateInfo (VkQueueFlagBits			includeFlags,
									  VkQueueFlagBits			excludeFlags	= Zero,
									  float						priority		= 0.0f,
									  VkQueueGlobalPriorityKHR	globalPriority	= VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_KHR,
									  StringView				name			= {}) __NE___ :
				includeFlags{includeFlags}, excludeFlags{excludeFlags},
				priority{priority}, globalPriority{globalPriority},
				debugName{name}
			{}
		};
		
		struct ObjectDbgInfo
		{
			StringView		type;
			StringView		name;
			ulong			handle;
		};

		struct DebugReport
		{
			ArrayView<ObjectDbgInfo>	objects;
			StringView					message;
			bool						isError		= false;
		};
		using DebugReport_t = Function< void (const DebugReport &) >;


	// variable
	private:
		VkDebugReportCallbackEXT	_debugReportCallback	= Default;
		VkDebugUtilsMessengerEXT	_debugUtilsMessenger	= Default;
		DebugReport_t				_callback;
		
		Mutex						_guard;
		bool						_breakOnValidationError	= true;
		bool						_enableInfoLog			= false;
		Array<ObjectDbgInfo>		_tempObjectDbgInfos;
		String						_tempString;

		bool						_isCopy					= false;


	// methods
	public:
		explicit VDeviceInitializer (bool enableInfoLog = false)									__NE___;
		~VDeviceInitializer ()																		__NE___;

		ND_ InstanceVersion  GetMaxInstanceVersion ()												C_NE___;

		ND_ bool  CreateInstance (NtStringView appName, NtStringView engineName, ArrayView<const char*> instanceLayers,
								  ArrayView<const char*> instanceExtensions = {}, InstanceVersion version = {1,2}, uint appVer = 0, uint engineVer = 0) __NE___;
		//ND_ bool  SetInstance (VkInstance value, InstanceVersion version = {1,2}, ArrayView<const char*> instanceExtensions = {}, ArrayView<const char*> instanceLayers = {});

			bool  DestroyInstance ()																__NE___;

		ND_ bool  ChooseDevice (StringView deviceName)												__NE___;
		ND_ bool  ChooseHighPerformanceDevice ()													__NE___;
		ND_ bool  SetPhysicalDevice (VkPhysicalDevice value)										__NE___;

		ND_ bool  CreateDefaultQueue ()																__NE___;
		ND_ bool  CreateDefaultQueues (EQueueMask required, EQueueMask optional = Default)			__NE___;
		ND_ bool  CreateQueues (ArrayView<QueueCreateInfo> queues)									__NE___;
		
		ND_ bool  CreateLogicalDevice (ArrayView<const char*> extensions = {},
									   const FeatureSet* fsToDeviceFeatures = null)					__NE___;
			//bool  SetLogicalDevice (VkDevice value, ArrayView<const char*> extensions = {})		__NE___;
			bool  DestroyLogicalDevice ()															__NE___;
		
			bool  CreateDebugCallback (VkDebugUtilsMessageSeverityFlagsEXT severity, DebugReport_t &&callback = Default)	__NE___;
			void  DestroyDebugCallback ()															__NE___;

		ND_ bool  Init (const VDeviceInitializer &otherDev)											__NE___;
		ND_ bool  Init (const GraphicsCreateInfo &ci, ArrayView<const char*> instanceExtensions)	__NE___;

		ND_ static ArrayView<const char*>	GetRecomendedInstanceLayers ()							__NE___;

		ND_ VulkanDeviceFnTable &			EditDeviceFnTable ()									__NE___	{ DRC_EXLOCK( _drCheck ); return _deviceFnTable; }


	private:
		#define VKFEATS_FN_DECL
		#include "vulkan_loader/vk_features.h"
		#undef  VKFEATS_FN_DECL
		
		ND_ bool  _CreateInstance (const char* appName, const char* engineName, ArrayView<const char*> instanceLayers,
								   ArrayView<const char*> instanceExtensions, InstanceVersion version, uint appVer, uint engineVer) __Th___;

		void  _ValidateInstanceVersion (VkInstance instance, ArrayView<const char*> layers, INOUT uint &version)		C_Th___;
		void  _ValidateInstanceLayers (INOUT Array<const char*> &layers)												C_Th___;
		void  _ValidateInstanceExtensions (INOUT Array<const char*> &ext)												C_Th___;
		bool  _ChooseHighPerformanceDevice ()																			__Th___;
		bool  _CreateLogicalDevice (ArrayView<const char*> extensions, const FeatureSet* fsToDeviceFeatures)			__Th___;
		void  _ValidateDeviceExtensions (VkPhysicalDevice physDev, INOUT Array<const char*> &ext)						C_Th___;
		void  _ValidateSpirvVersion (OUT SpirvVersion &ver)																C_NE___;
		void  _UpdateDeviceVersion (VkPhysicalDevice physicalDevice, OUT DeviceVersion &devVersion)						C_NE___;
		void  _SetResourceFlags (OUT ResourceFlags &)																	C_NE___;
		void  _OnCreateInstance (ArrayView<const char*> instanceExtensions, ArrayView<const char*> instanceLayers)		__Th___;

		void  _LogInstance (ArrayView<const char*> instanceLayers)														C_Th___;
		void  _LogPhysicalDevices ()																					C_NE___;
		void  _LogLogicalDevice ()																						C_Th___;
		void  _LogResourceFlags ()																						C_Th___;

		void  _InitQueues (ArrayView<VkQueueFamilyProperties> props, INOUT Queues_t &queues, INOUT QueueTypes_t &qtypes)C_NE___;
		void  _ValidateQueueStages (INOUT Queues_t &queues)																C_NE___;
		
		ND_ bool  _InitFeaturesAndPropertiesByFeatureSet (const FeatureSet &fs)											__NE___;

		// new debug api
		VKAPI_ATTR static VkBool32 VKAPI_CALL
			_DebugUtilsCallback (VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
								 VkDebugUtilsMessageTypeFlagsEXT				messageTypes,
								 const VkDebugUtilsMessengerCallbackDataEXT*	pCallbackData,
								 void*											pUserData) __NE___;
		
		// old debug api for mobile devices
		VKAPI_ATTR static VkBool32 VKAPI_CALL
			_DebugReportCallback (VkDebugReportFlagsEXT			flags,
								  VkDebugReportObjectTypeEXT	objectType,
								  ulong							object,
								  usize							/*location*/,
								  int							/*messageCode*/,
								  const char*					/*pLayerPrefix*/,
								  const char*					pMessage,
								  void*							pUserData) __NE___;

		void  _DebugReport (const DebugReport &) __Th___;
	};
	
	static constexpr VkDebugUtilsMessageSeverityFlagsEXT	DefaultDebugMessageSeverity =	//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
																							//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
																							VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
																							VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
