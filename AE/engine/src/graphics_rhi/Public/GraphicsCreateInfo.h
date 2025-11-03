// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/SwapchainDesc.h"

namespace AE::Graphics
{

	class IGfxMemAllocator;
	using GfxMemAllocatorPtr = RC< IGfxMemAllocator >;

	class IDescriptorAllocator;
	using DescriptorAllocatorPtr = RC< IDescriptorAllocator >;


	enum class EDeviceValidation : uint
	{
		Disabled				= 0,
		Enabled					= 1,			// Vulkan: StandardPreset

		// Vulkan presets
		//    requires: VExtensions::validationFlags (VK_EXT_validation_features)
		MinimalPreset			= 1 << 24,
		SynchronizationPreset	= 2 << 24,
		BestPracticesPreset		= 3 << 24,
		ShaderBasedPreset		= 4 << 24,	// validate access in shaders and indirect commands
		ShaderPrintfPreset		= 5 << 24,

		// Metal
	};


	enum class EDeviceFlags : uint
	{
		Unknown					= 0,

		// not supported in release config //
		SetStableClock			= 1 << 0,		// required for GPU profiling
		SetStableMemClock		= 1 << 1,		// required for GPU profiling

		EnablePerfCounters		= 1 << 2,
		EnableRenderDoc			= 1 << 3,		// allow to use RenderDoc API to trigger capture

		EnableMemoryReport		= 1 << 4,		// allow to set callback in 'Device::CreateMemoryReport()', may crash on some implementations

		_Last,
		All						= ((_Last - 1) << 1) - 1,
		_NvApiMask				= SetStableClock | EnablePerfCounters,
		_AmdApiMask				= SetStableClock | SetStableMemClock | EnablePerfCounters,
	};


	enum class EDriver : ubyte
	{
		Unknown	= 0,
		LavaPipe,		// software emulation from Mesa package

	  #ifdef AE_PLATFORM_LINUX
		_LinuxDrivers	= 0x10,
		RADV,			// open-source driver for AMD GPU from Mesa package
		AMDVLK,			// AMD open-source driver
		AMD_PRO,		// AMD proprietory driver
		ANV,			// open-source driver for Intel GPU from Mesa package
		IntelPro,		// Intel proprietory driver
		Nouveau,		// open-source driver for old NVIDIA GPUs from Mesa package
		NVK,			// open-source driver for new NVIDIA GPUs from Mesa package
		NVPro,			// NVIDIA proprietory driver
		VirtGPU,		// open-source driver for virtual GPU from Mesa package
		GFXStream,		// Vulkan virtualization from Google
	  #endif

		// TODO: other

		_Count
	};


	//
	// Graphics Create Info
	//
	struct GraphicsCreateInfo final
	{
		uint					maxFrames	= 2;

		// staging buffers //
		struct {
			// Static staging buffers allocated at engine start.
			Bytes32u				writeStaticSize			= 2_MiB;
			Bytes32u				readStaticSize			= 1_MiB;

			// Dynamic buffers will be allocated when needed and will be released after,
			// but total size can be limited here.
			//   expected FPS:           60
			//   PCI-E 3 x16 bandwidth:  16 GiB/s
			//   bandwidth per frame:   ~273 MiB
			Bytes					maxWriteDynamicSize		= 256_MiB;
			Bytes					maxReadDynamicSize		= 64_MiB;	// some GPUs has limited bandwidth for read access

			// Granularity of the dynamic staging buffers.
			Bytes					dynamicBlockSize		= 16_MiB;

			// Wait X frames before release dynamic buffer.
			uint					maxFramesToRelease		= 1 << 10;

			// Vertex & index buffer size for single frame.
			Bytes32u				vstreamSize				= 4_MiB;

			// Total size of staging memory is:
			//   (writeStaticSize * maxFrames) + (readStaticSize * (maxFrames+1)) + (vstreamSize * maxFrames) + (maxWriteDynamicSize + maxReadDynamicSize)
		}						staging;


		IGfxMemAllocator *		largeGfxAllocator		= null;
		IGfxMemAllocator *		defaultGfxAllocator		= null;
		IDescriptorAllocator *	defaultDescAllocator	= null;

		struct
		{
			StringView				appName;
			StringView				deviceName;		// keep empty for auto-detect
			EQueueMask				requiredQueues	= EQueueMask::Graphics;
			EQueueMask				optionalQueues	= Default;
			EDeviceValidation		validation		= EDeviceValidation::Enabled;
			EDeviceFlags			devFlags		= Default;
		}						device;

		bool					useRenderGraph	= false;

		SwapchainDesc			swapchain;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		Networking::IpAddress	deviceAddr;
		StringView				graphicsLibPath;
		bool					enableSyncLog	= false;
	  #endif
		StaticArray<EDriver, 8>	driverList = {};
	};

} // AE::Graphics
