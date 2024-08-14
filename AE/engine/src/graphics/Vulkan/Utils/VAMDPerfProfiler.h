// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for AMD GPU Perf SDK.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// AMD GPU Performance Profiler for Vulkan
	//

	class VAMDPerfProfiler
	{
		friend class VDevice;
		friend class VDeviceInitializer;

	// types
	public:
		enum class EHwGeneration : ubyte
		{
			Unknown,

			GCN1,
			GCN2,
			GCN3_GCN4,
			GCN5,

			RDNA1,
			RDNA2,
			RDNA3,

			CDNA1,
			CDNA2,
			CDNA3,
		};

		struct HWInfo
		{
			String			deviceName;
			uint			deviceId		= 0;
			uint			revisionId		= 0;
			EHwGeneration	gen				= Default;
		};

	private:
		struct Impl;


	// variables
	private:
		Library			_module;
		Unique<Impl>	_impl;


	// methods
	public:
		~VAMDPerfProfiler ()															__NE___;

		ND_ bool  IsLoaded ()															C_NE___	{ return bool{_module}; }
		ND_ bool  IsInitialized ()														C_NE___;


	private:
		VAMDPerfProfiler ()																__NE___;

		ND_ bool  Load ()																__NE___;
		ND_ bool  Initialize (const VDevice &, EDeviceFlags flags)						__NE___;
			void  Deinitialize ()														__NE___;

			bool  GetInstanceExtensions (const VDevice &, INOUT Array<const char*> &)	__Th___;
			bool  GetDeviceExtensions (const VDevice &, INOUT Array<const char*> &)		__Th___;

	private:
		ND_ bool  _Initialize (const VDevice &, EDeviceFlags flags)						__NE___;

			void  _PrintCounters ();
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
