// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe:  no

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Profiling.md)
*/

#pragma once

#include "profiler/Impl/ProfilerUtils.h"
#include "profiler/Impl/LogWindow.h"

namespace AE::Profiler
{
	class TaskProfiler;
	class GraphicsProfiler;
	class MemoryProfiler;
	class HwpcProfiler;


	//
	// Profiler UI
	//

	class ProfilerUI final
	{
	// types
	private:
		using MsgProducer	= ProfilerUtils::MsgProducer;
		using MsgConsumer	= ProfilerUtils::MsgConsumer;

	public:
		enum class EMask
		{
			Unknown			= 0,
			Task			= 1 << 0,	// deprecated, use Tracy profiler
			Graphics		= 1 << 1,
			HardwareCPU		= 1 << 2,
			HardwareGPU		= 1 << 3,	// PowerVR GPU requires this flag for GraphicsProfiler
			OSMetrics		= 1 << 4,
			Memory			= 1 << 5,
			_Last,
			All				= CT_AllBitMask2<EMask>,
			_BITOPS_		= 0
		};

		static const EMask	c_DefaultMask = EMask::All & ~EMask::Task;


	// variables
	private:
	  #ifndef AE_CFG_RELEASE
		Atomic<bool>			_enabled	{false};
		Timer					_timer;
		uint					_frameCount	= 0;

		RC<TaskProfiler>		_task;
		RC<GraphicsProfiler>	_graphics;
		RC<MemoryProfiler>		_memory;
		Unique<HwpcProfiler>	_hwpcProf;

		LogWindow::WeakPtr		_logWindow;

		RC<MsgProducer>			_msgProducer;
		RC<MsgConsumer>			_msgConsumer;
	  #endif


	// methods
	public:
		ProfilerUI ();
		~ProfilerUI ();

		ND_ bool  Initialize (Ptr<Networking::ClientServerBase>, EMask mask = c_DefaultMask);
		ND_ bool  InitLogWindow ();
			void  Deinitialize ();

		ND_ bool  IsInitialized () const;

			void  Enable (bool enabled);

		#ifdef AE_ENABLE_IMGUI
			void  DrawImGUI ();
		#endif

			void  Draw (Graphics::Canvas &canvas);


	private:
		void  _Update ();
		void  _SampleGraphicsCounters ();
	};


} // AE::Profiler
