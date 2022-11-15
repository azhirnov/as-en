// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Platforms/ThreadUtils.h"
#include "graphics_hl/Canvas/Canvas.h"
#include "graphics_hl/Resources/RasterFont.h"

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/ImColumnHistoryDiagram.h"
# include "profiler/ImGui/ImTaskRangeHorDiagram.h"
#endif

namespace AE::Profiler
{
	using namespace AE::Base;

	using AE::Graphics::GraphicsPipelineID;
	using AE::Graphics::DescriptorSetID;
	using AE::Graphics::Canvas;
	using AE::Graphics::RasterFont;
	using AE::Graphics::IDrawContext;

	using AE::Threading::Atomic;
	using AE::Threading::FAtomic;
	using AE::Threading::SharedMutex;
	using AE::Threading::Mutex;



	//
	// Profiler Utils
	//

	class ProfilerUtils
	{
	// types
	public:
		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;


	// variables
	private:
		const TimePoint_t	_startTime;


	// methods
	public:
		explicit ProfilerUtils (TimePoint_t startTime) :
			_startTime{ startTime }
		{}

		ND_ usize			CurrentThreadID ()	const	{ return ThreadUtils::GetIntID(); }
		ND_ uint			CoreIndex ()		const	{ return ThreadUtils::GetCoreIndex(); }

		ND_ secondsf		CurrentTime ()		const	{ return std::chrono::duration_cast<secondsf>( TimePoint_t::clock::now() - _startTime ); }
		ND_ nanosecondsd	CurrentTimeNano ()	const	{ return std::chrono::duration_cast<nanosecondsd>( TimePoint_t::clock::now() - _startTime ); }
	};


} // AE::Profiler