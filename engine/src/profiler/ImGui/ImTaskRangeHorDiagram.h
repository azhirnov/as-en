// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Should be used for single task per thread
	with dependencies between threads to detect stalls,
	like a task execution on the CPU.

	| [aaaa]    [oooooo] | - thread1
	|   [eeeeee] [rrr]   | - thread2
	|  [cc] [ddddd]      | - thread3
*/

#pragma once

#ifdef AE_ENABLE_IMGUI

# include "profiler/ImGui/Common.h"

namespace AE::Profiler
{

	//
	// ImGui Task Range Horizontal Diagram
	//

	class ImTaskRangeHorDiagram
	{
	// types
	private:
		struct Task
		{
			String		name;
			RGBA8u		color;
			uint		threadIdx;
			double		begin;
			double		end;
		};

		struct ThreadInfo
		{
			StringView	name;
			RGBA8u		color;
		};

		using UniqueThread_t	= FlatHashMap< usize, uint >;		// thread ID to index
		using ThreadInfoMap_t	= Array< ThreadInfo >;
		using TaskArray_t		= Array< Task >;

		struct FrameData
		{
			TaskArray_t			tasks;

			UniqueThread_t		threads;
			ThreadInfoMap_t		threadInfos;
			Array<uint>			sortedThreads;

			double				min		= 0.0;
			double				max		= 0.0;

			void  Clear ();
		};

		using FrameHistory_t = StaticArray< FrameData, 2 >;


	// variables
	private:
		FrameHistory_t		_frames;

		uint				_frameIdx	: 1;
		uint				_maxThreads	= 0;

		
	// methods
	public:
		ImTaskRangeHorDiagram () : _frameIdx{0} {}

		void  Draw (INOUT RectF &region);

		void  Begin ();
		void  Add (StringView name, RGBA8u color, double begin, double end, usize threadId, StringView threadName);
		void  End ();
	};


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
