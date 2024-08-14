// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Profiler.pch.h"

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/ImColumnHistoryDiagram.h"
# include "profiler/ImGui/ImTaskRangeHorDiagram.h"
# include "profiler/ImGui/ImLineGraph.h"
# include "profiler/ImGui/ImLineGraphTable.h"
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
	using AE::Threading::Synchronized;

	using AE::Networking::EChannel;
	using AE::Networking::CSMessageGroupID;
	using AE::Networking::CSMessagePtr;


	//
	// Profiler Utils
	//

	class ProfilerUtils
	{
	// types
	public:
		using TimePoint_t	= Clock::TimePoint_t;


		class MsgProducer final : public Networking::IAsyncCSMessageProducer
		{
		public:
			MsgProducer () __NE___ : Networking::IAsyncCSMessageProducer{ Tag<Threading::LfLinearAllocator< usize{1_Mb}, usize{8_b}, 4 >>{} } {}

			EnumSet<EChannel>  GetChannels ()					C_NE_OV	{ return {EChannel::Reliable}; }
		};


		class MsgConsumer final : public Networking::ICSMessageConsumer
		{
		public:
			CSMessageGroupID  GetGroupID ()							C_NE_OV	{ return CSMessageGroup::Debug; }
			void  Consume (ChunkList<const CSMessagePtr> msgList)	__NE_OV;
		};


	// variables
	private:
		const Clock		_timer;


	// methods
	public:
		explicit ProfilerUtils (TimePoint_t startTime) :
			_timer{ startTime }
		{}

		ND_ usize			CurrentThreadID ()	const	{ return ThreadUtils::GetIntID(); }
		ND_ uint			CoreIndex ()		const	{ return ThreadUtils::GetCoreIndex(); }

		ND_ secondsf		CurrentTime ()		const	{ return _timer.TimeSince<secondsf>(); }
		ND_ nanosecondsd	CurrentTimeNano ()	const	{ return _timer.TimeSince<nanosecondsd>(); }
	};


} // AE::Profiler
