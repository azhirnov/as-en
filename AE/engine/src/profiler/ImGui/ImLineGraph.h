// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/Common.h"

namespace AE::Profiler
{

	//
	// ImGui Line Graph
	//

	class ImLineGraph
	{
	// types
	public:
		static constexpr uint	MaxGraphs = 4;

		enum class EMode
		{
			Line,
			Column,

			Line_Adaptive,
			//Column_Adaptive,
		};

		struct ColorStyle
		{
			RGBA8u		lines		[MaxGraphs];
			RGBA8u		background;
			RGBA8u		border;
			RGBA8u		text;
			EMode		mode		= EMode::Line;
		};

	private:
		struct LineData
		{
			RingBuffer<float>	points;
			String				label;
		};
		using LineArr_t = FixedArray< LineData, MaxGraphs >;


	// variables
	private:
		mutable SharedMutex		_guard;

		LineArr_t				_lines;
		mutable float2			_range;

		ColorStyle				_style;
		uint					_capacity	= 100;
		String					_name;


	// methods
	public:
		ImLineGraph ()											{}
		ImLineGraph (ImLineGraph &&)							{}

		ND_ bool	Empty (uint dim = 0)				const;
		ND_ float	LastPoint (uint dim = 0)			const;

		void  Draw (const RectF &region)				const;

		void  SetName (StringView value);
		void  SetLabel (StringView label, uint dim);

		void  SetColor (const ColorStyle &style)				{ EXLOCK( _guard );  _style = style; }
		void  SetCapacity (uint value, uint dim = 1);

		void  SetRange (float min, float max);

		void  Add (std::initializer_list<float> values);
		void  AddOpt (std::initializer_list<float> values);
		void  AddAndUpdateRange (std::initializer_list<float> values);
	};


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
