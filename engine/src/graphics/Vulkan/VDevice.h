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
		Extensions				_extensions;

		VulkanDeviceFnTable		_deviceFnTable;
		
		Properties				_properties;
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
		
		ND_ Extensions const&		GetExtensions ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _extensions; }
		ND_ Properties const&		GetProperties ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _properties; }
		ND_ ResourceFlags const&	GetResourceFlags ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _resFlags; }
		ND_ DeviceProperties const&	GetDeviceProperties ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _devProps; }

		ND_ VkDevice				GetVkDevice ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _vkLogicalDevice; }
		ND_ VkPhysicalDevice		GetVkPhysicalDevice ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _vkPhysicalDevice; }
		ND_ VkInstance				GetVkInstance ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkInstance; }
		ND_ InstanceVersion			GetInstanceVersion ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _vkInstanceVersion; }
		ND_ DeviceVersion			GetDeviceVersion ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkDeviceVersion; }
		ND_ SpirvVersion			GetSpirvVersion ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _spirvVersion; }
		ND_ Version2				GetVkVersion ()				const	{ return GetDeviceVersion().Cast<0>(); }
		ND_ ArrayView<VQueue>		GetQueues ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _queues; }
		ND_ VQueuePtr				GetQueue (EQueueType type)	const	{ DRC_SHAREDLOCK( _drCheck );  return uint(type) < _queueTypes.size() ? _queueTypes[uint(type)] : null; }
		ND_ EQueueMask				GetAvailableQueues ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _queueMask; }

		ND_ bool					IsInitialized ()			const	{ return GetVkDevice() != Default; }

		// check extensions
		ND_ bool  HasInstanceExtension (StringView name) const;
		ND_ bool  HasDeviceExtension (StringView name) const;
		
		bool  SetObjectName (ulong id, NtStringView name, VkObjectType type) const;

		void  GetQueueFamilies (EQueueMask mask, OUT VQueueFamilyIndices_t &) const;
		
		bool  GetMemoryTypeIndex (uint memoryTypeBits, VkMemoryPropertyFlagBits includeFlags, VkMemoryPropertyFlagBits optFlags,
								  VkMemoryPropertyFlagBits excludeFlags, OUT uint &memoryTypeIndex) const;
		bool  GetMemoryTypeIndex (uint memoryTypeBits, EMemoryType memType, OUT uint &memoryTypeIndex) const;
		
		bool  CheckConstantLimits () const;
		bool  CheckExtensions () const;

		void  InitFeatureSet (OUT FeatureSet &) const;
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
			VkQueueGlobalPriorityEXT	globalPriority	= VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT;
			FixedString<64>				debugName;

		// methods
			QueueCreateInfo () {}
			
			explicit QueueCreateInfo (VkQueueFlagBits			includeFlags,
									  VkQueueFlagBits			excludeFlags	= Zero,
									  float						priority		= 0.0f,
									  VkQueueGlobalPriorityEXT	globalPriority	= VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT,
									  StringView				name			= {}) :
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

	private:
		static constexpr uint	MaxQueueFamilies = 32;
		using QueueFamilyProperties_t	= FixedArray< VkQueueFamilyProperties, MaxQueueFamilies >;
		using QueueCount_t				= FixedArray< uint, MaxQueueFamilies >;


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
		explicit VDeviceInitializer (bool enableInfoLog = false);
		~VDeviceInitializer ();

		ND_ InstanceVersion  GetMaxInstanceVersion () const;

		ND_ bool  CreateInstance (NtStringView appName, NtStringView engineName, ArrayView<const char*> instanceLayers,
								  ArrayView<const char*> instanceExtensions = {}, InstanceVersion version = {1,2}, uint appVer = 0, uint engineVer = 0);
		//ND_ bool  SetInstance (VkInstance value, InstanceVersion version = {1,2}, ArrayView<const char*> instanceExtensions = {}, ArrayView<const char*> instanceLayers = {});

			bool  DestroyInstance ();

		ND_ bool  ChooseDevice (StringView deviceName);
		ND_ bool  ChooseHighPerformanceDevice ();
		ND_ bool  SetPhysicalDevice (VkPhysicalDevice value);

		ND_ bool  CreateDefaultQueue ();
		ND_ bool  CreateDefaultQueues (EQueueMask required, EQueueMask optional = Default);
		ND_ bool  CreateQueues (ArrayView<QueueCreateInfo> queues);
		
		ND_ bool  CreateLogicalDevice (ArrayView<const char*> extensions = {});
			//bool  SetLogicalDevice (VkDevice value, ArrayView<const char*> extensions = {});
			bool  DestroyLogicalDevice ();
		
			bool  CreateDebugCallback (VkDebugUtilsMessageSeverityFlagsEXT severity, DebugReport_t &&callback = Default);
			void  DestroyDebugCallback ();

		ND_ bool  Init (const VDeviceInitializer &otherDev);
		ND_ bool  Init (const GraphicsCreateInfo &ci, ArrayView<const char*> instanceExtensions);

		ND_ static ArrayView<const char*>	GetRecomendedInstanceLayers ();

		ND_ VulkanDeviceFnTable &			EditDeviceFnTable ()			{ DRC_EXLOCK( _drCheck ); return _deviceFnTable; }


	private:
		#define VKFEATS_FN_DECL
		#include "vulkan_loader/vk_features.h"
		#undef  VKFEATS_FN_DECL

		void  _ValidateInstanceVersion (VkInstance instance, ArrayView<const char*> layers, INOUT uint &version) const;
		void  _ValidateInstanceLayers (INOUT Array<const char*> &layers) const;
		void  _ValidateInstanceExtensions (INOUT Array<const char*> &ext) const;
		void  _ValidateDeviceExtensions (INOUT Array<const char*> &ext) const;
		void  _ValidateSpirvVersion (OUT SpirvVersion &ver) const;
		void  _UpdateDeviceVersion ();
		void  _SetResourceFlags (OUT ResourceFlags &) const;
		void  _OnCreateInstance (ArrayView<const char*> instanceExtensions, ArrayView<const char*> instanceLayers);
		void  _InitDeviceProperties (OUT DeviceProperties &props) const;

		void  _LogInstance (ArrayView<const char*> instanceLayers) const;
		void  _LogPhysicalDevices () const;
		void  _LogLogicalDevice () const;
		void  _LogResourceFlags () const;

		void  _InitQueues (ArrayView<VkQueueFamilyProperties> props, INOUT Queues_t &queues, INOUT QueueTypes_t &qtypes) const;
		void  _ValidateQueueStages (INOUT Queues_t &queues) const;
		
		VKAPI_ATTR static VkBool32 VKAPI_CALL
			_DebugUtilsCallback (VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
								 VkDebugUtilsMessageTypeFlagsEXT				messageTypes,
								 const VkDebugUtilsMessengerCallbackDataEXT*	pCallbackData,
								 void*											pUserData);
		
		VKAPI_ATTR static VkBool32 VKAPI_CALL
			_DebugReportCallback (VkDebugReportFlagsEXT			flags,
								  VkDebugReportObjectTypeEXT	objectType,
								  ulong							object,
								  size_t						/*location*/,
								  int32_t						/*messageCode*/,
								  const char*					/*pLayerPrefix*/,
								  const char*					pMessage,
								  void*							pUserData);

		void  _DebugReport (const DebugReport &);
	};
	
	static constexpr VkDebugUtilsMessageSeverityFlagsEXT	DefaultDebugMessageSeverity =	//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
																							//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
																							VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
																							VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
