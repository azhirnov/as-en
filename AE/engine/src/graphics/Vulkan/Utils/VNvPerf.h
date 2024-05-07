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
	// NV Perf for Vulkan
	//

	class VNvPerf
	{
		friend class VDevice;
		friend class VDeviceInitializer;

	// types
	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;
		Library			_module;


	// methods
	public:
		~VNvPerf ()																		__NE___;

		ND_ bool  IsLoaded ()															C_NE___	{ return bool{_module}; }
		ND_ bool  IsInitialized ()														C_NE___;

		ND_	StringView  GetDeviceName ()												C_NE___;
		ND_	StringView  GetChipName ()													C_NE___;


	  // Profiler //
			bool  SetStableClockState (bool stable)										C_NE___;


	  // Metrics //
		//	bool  InitializeMetrics ()													__NE___;


	private:
		VNvPerf ()																		__NE___;

		ND_ bool  Load ()																__NE___;
		ND_ bool  Initialize (const VDevice &)											__NE___;
			void  Deinitialize ()														__NE___;

			bool  GetInstanceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___;
			bool  GetDeviceExtensions (const VDevice &, INOUT Array<const char*> &)		__Th___;

		ND_ bool  _Initialize (const VDevice &)											__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
