// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for NSight Perf SDK
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// NVidia Performance Profiler for Vulkan
	//

	class VNvPerfProfiler
	{
		friend class VDevice;
		friend class VDeviceInitializer;

	// types
	public:
		struct HWInfo
		{
			String		deviceName;
			String		chipName;
		};

	private:
		struct Impl;


	// variables
	private:
		Library			_module;
		Unique<Impl>	_impl;


	// methods
	public:
		~VNvPerfProfiler ()																__NE___;

		ND_ bool  IsLoaded ()															C_NE___	{ return bool{_module}; }
		ND_ bool  IsInitialized ()														C_NE___;

		ND_ HWInfo	GetHWInfo ()														C_NE___;


	  // Profiler //
			bool  SetStableClockState (bool stable)										C_NE___;


	  // Metrics //
			bool  InitializeMetrics ()													__NE___;


	private:
		VNvPerfProfiler ()																__NE___;

		ND_ bool  Load ()																__NE___;
		ND_ bool  Initialize (const VDevice &)											__NE___;
			void  Deinitialize ()														__NE___;

			bool  GetInstanceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___;
			bool  GetDeviceExtensions (const VDevice &, INOUT Array<const char*> &)		__Th___;

		ND_ bool  _Initialize (const VDevice &)											__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
