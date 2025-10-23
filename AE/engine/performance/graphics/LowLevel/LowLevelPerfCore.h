// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/GraphicsRHI.h"
#include "pch/VFS.h"

#include "profiler/Profilers/AdrenoProfiler.h"
#include "profiler/Profilers/MaliProfiler.h"
#include "profiler/Profilers/PowerVRProfiler.h"

namespace AE::Graphics
{

	//
	// Micro Benchmark Core
	//

	class LowLevelPerfCore
	{
	// types
	public:
		using BenchFn_t		= void (*) ();
		using FStorage_t	= RC<VFS::IVirtualFileStorage>;

		struct BenchResult
		{
			StringView		name;
			nanosecondsd	dtMin;
			nanosecondsd	dtMax;
			double			dtAvr	= 0.0;	// nanoseconds
		};

		using AllResults_t		= Array< BenchResult >;
		using ResultsPerType_t	= FlatHashMap< StringView, AllResults_t >;


	private:
		using Query				= Graphics::QueryManager::Query;

	  #if defined(AE_ENABLE_VULKAN)
		using GraphicsDevice	= Graphics::VDeviceInitializer;

	  #elif defined(AE_ENABLE_METAL)
		using GraphicsDevice	= Graphics::MDeviceInitializer;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using GraphicsDevice	= Graphics::RDeviceInitializer;

	  #else
	  #	error not implemented
	  #endif

		struct ComplexResult
		{
			StringView		name;
			StringView		lhs;	// name = lhs - rhs
			StringView		rhs;
			double			lScale;
			double			rScale;

			ComplexResult () {}
			ComplexResult (const Tuple<StringView, StringView, StringView, double, double> &t) :
				name{t.Get<0>()}, lhs{t.Get<1>()}, rhs{t.Get<2>()}, lScale{t.Get<3>()}, rScale{t.Get<4>()} {}
		};

		using ResultsMap_t		= FlatHashMap< StringView, usize >;
		using CustomPrintFn_t	= String (*)(const ResultsPerType_t &);


	// variables
	private:
		GraphicsDevice					_device;

		struct {
			Profiler::MaliProfiler			mali;
			Profiler::AdrenoProfiler		adreno;
			Profiler::PowerVRProfiler		pvr;
		}								_profilers;

		struct {
			uint2							dim;

			Strong<ImageID>					rt;
			Strong<ImageViewID>				rtView;

			Strong<ImageID>					img;
			Strong<ImageViewID>				imgView;

			Strong<BufferID>				hostBuf;

			Strong<PipelineCacheID>			pplnCache;
		}								_res;

		bool							_isHighPerf;
		ResultsPerType_t				_results;
		Array<ComplexResult>			_complex;


	// methods
	public:
		LowLevelPerfCore ();
		~LowLevelPerfCore ();

		ND_ bool  Run (FStorage_t assetStorage);

	private:
		ND_ bool  _InitGraphics (const GraphicsCreateInfo &);
			void  _DestroyGraphics ();

		ND_	bool  _InitResources (FStorage_t assetStorage);
			void  _DestroyResources ();

			void  _InitProfiler ();
			void  _DestroyProfiler ();

			void  _ResetProfilers () const;
			void  _ReadProfilers (OUT nanosecondsd* time, usize count) const;

			void  _SortResults ();
			void  _PrintResults (StringView name, CustomPrintFn_t fn = null) const;
			void  _Reset ();


	// InstructionBenchmark
	private:
			void  _InstructionBenchmark ();

			void  _IBenchmark (StringView rtechName, ArrayView<StringView> modeArr1, ArrayView<StringView> modeArr2,
								uint iterCount, ArrayView<StringView> types);

			void  _IBenchmarkFS (StringView rtechName, ArrayView<StringView> modeArr1, ArrayView<StringView> modeArr2,
								 uint iterCount, ArrayView<StringView> types);
			void  _IBenchmarkCS (StringView rtechName, ArrayView<StringView> modeArr1, ArrayView<StringView> modeArr2,
								 uint iterCount, ArrayView<StringView> types);

		template <typename PplnType>
		ND_ bool  _IBenchmarkFrame (PplnType ppln, DescriptorSetID ds, StringView mode, uint iterCount, OUT BenchResult &) const;

		ND_ RenderCoro  _IBenchmarkTask (GraphicsPipelineID ppln, DescriptorSetID ds, Query& q, uint qIndex, bool isLast) const;
		ND_ RenderCoro  _IBenchmarkTask (ComputePipelineID ppln, DescriptorSetID ds, Query& q, uint qIndex, bool isLast) const;


	// NaN
	private:
		void  _NaNTest ();
		bool  _NaNTest2 (ImageViewID view, GraphicsPipelineID ppln, Function<void (const ImageMemView &)> fn);
	};


} // AE::Graphics
