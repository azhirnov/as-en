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
# include "base/Containers/FixedMap.h"
# include "base/Containers/FixedArray.h"
# include "profiler/ImGui/Common.h"

namespace AE::Profiler
{

	//
	// ImGui Task Range Horizontal Diagram
	//

	class ImTaskRangeHorDiagram final : public Noncopyable
	{
	// types
	private:
		enum class ThreadID : usize {};

		struct Task
		{
			String			name;
			RGBA8u			color;
			uint			threadIdx;
			nanosecondsd	begin;
			nanosecondsd	end;
		};

		struct ThreadInfo
		{
			StringView	name;
			RGBA8u		color;
		};

		static constexpr uint	MaxThreads = 64;

		using InfoIndex			= ubyte;
		using UniqueThread_t	= FixedMap< ThreadID, InfoIndex, MaxThreads >;		// thread ID to index
		using ThreadInfoMap_t	= FixedArray< ThreadInfo, MaxThreads >;
		using TaskArray_t		= Array< Task >;
		using IdxInTInfoArr_t	= FixedArray< InfoIndex, MaxThreads >;

		struct FrameData
		{
			TaskArray_t			tasks;

			UniqueThread_t		threads;
			ThreadInfoMap_t		threadInfos;

			// used to make thread position stable between frames
			IdxInTInfoArr_t		sortedThreads;

			nanosecondsd		min		{0.0};
			nanosecondsd		max		{0.0};

			void  Clear ();
		};

		using FrameHistory_t = StaticArray< FrameData, 2 >;


	// variables
	private:
		mutable SharedMutex		_guard;

		FrameHistory_t			_frames;

		uint					_enableTreeView	: 1;
		uint					_frameIdx		: 1;
		uint					_maxThreads		: 30;


	// methods
	public:
		ImTaskRangeHorDiagram () : _enableTreeView{true}, _frameIdx{0}, _maxThreads{0} {}

		void  Draw (INOUT RectF &region)		const;
		void  EnableTreeView (bool value)		{ EXLOCK( _guard );  _enableTreeView = value; }

		void  Begin ();
		void  Add (StringView name, RGBA8u color, nanosecondsd begin, nanosecondsd end, usize threadId, StringView threadCaption);
		void  End ();
	};


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
