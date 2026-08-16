// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Should be used for sequential tasks with different execution time, like a rendering on the GPU.

	| c c c   |
	| b c c c | -- [time] cccc
	| a c c b | -- [time] bbbbb
	| a a b b | /
	| a a a a | -- [time] aaaaa
*/

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/Common.h"

namespace AE::Profiler
{

	//
	// ImGui Column History Diagram
	//

	class ImColumnHistoryDiagram final : public Noncopyable
	{
	// types
	private:
		using UniqueNames_t	= HashSet< String >;

		struct Item
		{
			const char*		name	= null;
			RGBA8u			color;
			nanosecondsd	begin;
			nanosecondsd	end;
		};

		struct FrameData
		{
			Array<Item>		items;
			nanosecondsd	min		{0.0};
			nanosecondsd	max		{0.0};

			ND_ double  Height ()	const	{ return (max - min).count(); }
		};

		static constexpr int	_HistorySize	= 100;	// in frames

		using FrameHistory_t	= StaticArray< FrameData, _HistorySize >;
		using MaxTimeHistory_t	= RingBuffer< double >;


	// variables
	private:
		mutable SharedMutex		_guard;

		double					_maxHeight			= 0.0;
		int						_currentFrameIdx	= 0;

		UniqueNames_t			_uniqueNames;
		FrameHistory_t			_frames;


	// methods
	public:
		void  Draw (INOUT RectF &region) const;

		void  Begin ();
		void  Add (StringView name, RGBA8u color, nanosecondsd begin, nanosecondsd end);
		void  End (nanosecondsd min, nanosecondsd max);
		void  End ();
	};


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
