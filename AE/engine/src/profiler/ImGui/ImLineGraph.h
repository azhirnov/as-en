// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/Common.h"

namespace AE::Profiler
{

	//
	// ImGui Line Graph
	//

	class ImLineGraph final : public Noncopyable
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
			RGBA8u		background	[3];			// default, >limit1, >limit2
			RGBA8u		border;
			RGBA8u		text;
			RGBA8u		minMaxValue;
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
		mutable float2			_range			{0.f, 1.f};
		mutable uint			_bgIndex		= 0;

		float2					_limits			{MaxValue<float>()};
		bool					_invLimits		= false;
		uint					_capacity		= 100;

		ColorStyle				_style;

		String					_name;
		String					_suffix;
		String					_description;	// as tooltip


	// methods
	public:
		ImLineGraph ()									__NE___ {}

		ND_ bool	Empty ()							const;
		ND_ bool	Empty (uint dim)					const;
		ND_ float	LastPoint (uint dim = 0)			const;

		void  Draw (const RectF &region,
					bool isHover = false)				const;

		void  SetName (String value);
		void  SetLabel (String label, uint dim);
		void  SetSuffix (String value);
		void  SetDescription (String value);

		void  SetColor (const ColorStyle &style)				{ EXLOCK( _guard );  _style = style; }
		void  SetCapacity (uint value, uint dim = 1);

		void  SetRange (float min, float max);
		void  SetLimits (float val1, float val2);
		void  SetInvLimits (float val1, float val2);

		void  Add (ArrayView<float> values);
		void  AddOpt (ArrayView<float> values);
		void  AddAndUpdateRange (ArrayView<float> values);

		void  AddNonScaled (ArrayView<float> values);
		void  AddNonScaled (ArrayView<double> values);

	private:
		template <typename T>
		void  _AddNonScaled (ArrayView<T> values);
	};


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
