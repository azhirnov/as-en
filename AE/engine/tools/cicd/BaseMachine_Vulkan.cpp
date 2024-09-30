// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN) and (defined(CICD_BUILD_MACHINE) or defined(CICD_TEST_MACHINE))

# ifndef VK_NO_PROTOTYPES
#	define VK_NO_PROTOTYPES
# endif

# define VK_ENABLE_BETA_EXTENSIONS

# include "base/Defines/StdInclude.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	include <vulkan/vulkan.h>
#	pragma warning (pop)
# else
#	include <vulkan/vulkan.h>
# endif

# include "cicd/BaseMachine.h"
# include "graphics/Private/EnumUtils.h"

namespace AE::CICD
{

/*
=================================================
	_GetGPUInfo
=================================================
*/
	bool  BaseMachine::_GetGPUInfo (INOUT MachineInfo &info)
	{
		Library		vk_lib;

	  #if defined(AE_PLATFORM_WINDOWS)
		Unused( vk_lib.Load( "vulkan-1.dll" ));

	  #elif defined(AE_PLATFORM_APPLE)
		Unused( vk_lib.Load( "/usr/local/lib/libvulkan.1.dylib" ));

	  #elif defined(AE_PLATFORM_UNIX_BASED)
		Unused( vk_lib.Load( "libvulkan.so" ));

	  #else
	  #	error not implemented
	  #endif

		if ( not vk_lib )
			return false;

		PFN_vkGetInstanceProcAddr			fnGetInstanceProcAddr			= null;
		PFN_vkCreateInstance				fnCreateInstance				= null;
		PFN_vkDestroyInstance				fnDestroyInstance				= null;
		PFN_vkEnumeratePhysicalDevices		fnEnumeratePhysicalDevices		= null;
		PFN_vkGetPhysicalDeviceProperties	fnGetPhysicalDeviceProperties	= null;

		if ( not vk_lib.GetProcAddr( "vkGetInstanceProcAddr",			OUT fnGetInstanceProcAddr )			or
			 not vk_lib.GetProcAddr( "vkCreateInstance",				OUT fnCreateInstance )				or
			 not vk_lib.GetProcAddr( "vkDestroyInstance",				OUT fnDestroyInstance )				or
			 not vk_lib.GetProcAddr( "vkEnumeratePhysicalDevices",		OUT fnEnumeratePhysicalDevices )	or
			 not vk_lib.GetProcAddr( "vkGetPhysicalDeviceProperties",	OUT fnGetPhysicalDeviceProperties ))
			return false;

		VkApplicationInfo		app_info = {};
		app_info.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.apiVersion			= VK_API_VERSION_1_0;
		app_info.pApplicationName	= "temp";
		app_info.pEngineName		= "temp";

		VkInstanceCreateInfo	instance_ci = {};
		instance_ci.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_ci.pApplicationInfo		= &app_info;

		#ifdef AE_PLATFORM_APPLE
		const char*	extension_names[]		= { VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME };
		instance_ci.ppEnabledExtensionNames	= extension_names;
		instance_ci.enabledExtensionCount	= uint(CountOf( extension_names ));
		instance_ci.flags					= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif

		VkInstance	vk_instance;
		if ( auto err = fnCreateInstance( &instance_ci, null, OUT &vk_instance ); err != VK_SUCCESS )
			return false;

		FixedArray< VkPhysicalDevice, 4 >	devices;

		uint	count = 0;
		fnEnumeratePhysicalDevices( vk_instance, OUT &count, null );

		if ( count == 0 )
		{
			fnDestroyInstance( vk_instance, null );
			return false;
		}

		info.gapi = EGraphicsAPI::Vulkan;

		devices.resize( count );
		count = uint(devices.size());
		fnEnumeratePhysicalDevices( vk_instance, OUT &count, OUT devices.data() );
		devices.resize( Min( count, devices.size() ));

		for (auto& dev : devices)
		{
			VkPhysicalDeviceProperties	prop = {};
			fnGetPhysicalDeviceProperties( dev, OUT &prop );

			GPUInfo&	dev_info	= info.gpuDevices.emplace_back();
			dev_info.gapiVersion	= Version2{VK_VERSION_MAJOR(prop.apiVersion), VK_VERSION_MINOR(prop.apiVersion)};
			dev_info.name			= prop.deviceName;
			dev_info.device			= Graphics::GetEGraphicsDeviceByID( prop.deviceID );

			if ( dev_info.device == Default )
				dev_info.device = Graphics::GetEGraphicsDeviceByName( dev_info.name );
		}

		fnDestroyInstance( vk_instance, null );
		return true;
	}

} // AE::CICD

#endif // AE_ENABLE_VULKAN
