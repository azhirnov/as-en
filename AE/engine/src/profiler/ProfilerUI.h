// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Profiling.md)
*/

#pragma once

#include "profiler/Impl/GraphicsProfiler.h"
#include "profiler/Impl/MemoryProfiler.h"
#include "profiler/Impl/TaskProfiler.h"
#include "profiler/Impl/HwpcProfiler.h"

namespace AE::Profiler
{

	//
	// Profiler UI
	//

	class ProfilerUI final
	{
	// types
	private:
		using MsgProducer	= ProfilerUtils::MsgProducer;
		using MsgConsumer	= ProfilerUtils::MsgConsumer;


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

		RC<MsgProducer>			_msgProducer;
		RC<MsgConsumer>			_msgConsumer;
	  #endif


	// methods
	public:
		ProfilerUI () {}
		~ProfilerUI ();

		ND_ bool  Initialize (Ptr<Networking::ClientServerBase>);
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
