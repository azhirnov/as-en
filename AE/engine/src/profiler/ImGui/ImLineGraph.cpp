// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "profiler/ImGui/ImLineGraph.h"

namespace AE::Profiler
{

/*
=================================================
	Draw
=================================================
*/
	void  ImLineGraph::Draw (const RectF &inRegion, bool isHover) const
	{
		SHAREDLOCK( _guard );

		const float		line_thickness		= 2.0f;
		const float		border_thickness	= 1.0f;
		const float		rounding			= 2.0f;
		const float		min_width			= inRegion.Width();
		const float		min_height			= inRegion.Height();
		const RectF		border_region		= RectF{ inRegion.left, inRegion.top,
													 inRegion.left + min_width, inRegion.top + min_height };
		const RectF		line_region			= RectF{border_region}.Stretch( -border_thickness * 4.f );
		const float		x_step				= line_region.Width() / _capacity;
		const float		y_scale				= Max( 0.f, line_region.Height() / (_range.y - _range.x) );
		ImDrawList*		draw_list			= ImGui::GetWindowDrawList();

		draw_list->AddRectFilled( ImVec2{border_region.left, border_region.top},
								  ImVec2{border_region.right, border_region.bottom},
								  BitCast<uint>(_style.background[_bgIndex]), rounding, 0 );

		draw_list->AddRect( ImVec2{border_region.left, border_region.top},
							ImVec2{border_region.right, border_region.bottom},
							BitCast<uint>(_style.border), rounding, 0, border_thickness );

	  #ifdef AE_PLATFORM_WINDOWS
		isHover = ImGui::IsMouseHoveringRect( ImVec2{border_region.left, border_region.top},
											  ImVec2{border_region.right, border_region.bottom},
											  true );

		if_unlikely( isHover and not _description.empty() )
		{
			String	desc = _description;
			desc << "\n----";

			for (usize l = 0; l < _lines.size(); ++l)
			{
				if ( _lines[l].label.empty() )
					desc << "\nvalue: ";
				else
					desc << '\n' << _lines[l].label << ": ";

				if ( _lines[l].points.empty() )
					desc << '-';
				else
					desc << ToString2( _lines[l].points.back(), 3 ) << _suffix;
			}
			ImGui::SetTooltip( "%s", desc.c_str() );
		}
	  #endif

		if ( _style.mode == EMode::Line or
			 _style.mode == EMode::Line_Adaptive )
		{
			const bool	adaptive	= (_style.mode == EMode::Line_Adaptive);
			float		min_y		= MaxValue<float>();
			float		max_y		= 0.f;

			for (usize l = 0; l < _lines.size(); ++l)
			{
				const auto&		src	= _lines[l].points;
				usize			j	= _capacity - Min( _capacity, src.size() );
				Array<ImVec2>	dst;
				dst.resize( _capacity );

				for (usize i = 0; i < j; ++i) {
					dst[i] = { line_region.left + float(i) * x_step, line_region.bottom };
				}

				for (usize i = 0; i < src.size(); ++i, ++j)
				{
					dst[j].x = line_region.left + float(j) * x_step;
					dst[j].y = Clamp( line_region.bottom - (src[i] - _range.x) * y_scale, line_region.top, line_region.bottom );

					min_y = Min( min_y, src[i] );
					max_y = Max( max_y, src[i] );
				}
				draw_list->AddPolyline( dst.data(), int(dst.size()), BitCast<uint>(_style.lines[l]), 0, line_thickness );
			}

			if ( _invLimits )
			{
				// green
				// yllow
				// red
				if ( max_y > _limits.y )	_bgIndex = 0;	else
				if ( max_y > _limits.x )	_bgIndex = 1;	else
											_bgIndex = 2;
			}
			else
			{
				// red
				// yellow
				// green
				if ( max_y > _limits.y )	_bgIndex = 2;	else
				if ( max_y > _limits.x )	_bgIndex = 1;	else
											_bgIndex = 0;
			}

			if ( adaptive )
			{
				_range *= float2{IsFinite( _range )};
				_range.x = Min( _range.y * 0.99f, min_y * 0.9f );
				_range.y = Max( _range.y * 0.99f, max_y * 1.1f );
				_range = Max( _range, 0.f );
			}

			if ( not _name.empty() )
			{
				const ImVec2	text_pos{ line_region.left + line_region.Width() * 0.01f,
										  border_region.top + ImGui::GetTextLineHeightWithSpacing() * 0.1f };

				draw_list->AddText( text_pos, BitCast<uint>(_style.text), _name.c_str() );
			}

			// min value
			{
				String			val_str		= ToString2( _range.x, 2 ) << _suffix;
				const float		text_width	= ImGui::GetTextLineHeight() * (4 + _suffix.size());
				const ImVec2	text_pos	{ line_region.right - text_width - line_region.Width() * 0.01f,
											  border_region.bottom - ImGui::GetTextLineHeightWithSpacing() * 1.1f };

				draw_list->AddText( text_pos, BitCast<uint>(_style.minMaxValue), val_str.c_str() );
			}

			// max value
			{
				String			val_str		= ToString2( _range.y, 2 ) << _suffix;
				const float		text_width	= ImGui::GetTextLineHeight() * (4 + _suffix.size());
				const ImVec2	text_pos	{ line_region.right - text_width - line_region.Width() * 0.01f,
											  border_region.top + ImGui::GetTextLineHeightWithSpacing() * 0.1f };

				draw_list->AddText( text_pos, BitCast<uint>(_style.minMaxValue), val_str.c_str() );
			}

			for (usize l = 0; l < _lines.size(); ++l)
			{
				const ImVec2	text_pos{ line_region.left + line_region.Width() * 0.08f,
										  border_region.top + ImGui::GetTextLineHeightWithSpacing() * (l + 1.1f) };

				draw_list->AddText( text_pos, BitCast<uint>(_style.lines[l]), _lines[l].label.c_str() );
			}
		}
		else
		if ( _style.mode == EMode::Column )
		{
			CHECK( _lines.size() == 1 );

			const auto&	points	= _lines[0].points;
			usize		j		= _capacity - Min( _capacity, points.size() );

			for (usize i = 0; i < points.size(); ++i, ++j)
			{
				float	x0 = line_region.left + float(j) * x_step;
				float	x1 = line_region.left + float(j+1) * x_step;
				float	y0 = line_region.bottom;
				float	y1 = Clamp( line_region.bottom - (points[i] - _range.x) * y_scale, line_region.top, line_region.bottom );

				draw_list->AddRectFilled( ImVec2{x0, y0}, ImVec2{x1, y1}, BitCast<uint>(_style.lines[0]), 0.f, 0 );
			}

			if ( not _name.empty() )
			{
				const ImVec2	text_pos{ line_region.left + line_region.Width() * 0.05f,
										  border_region.CenterY() - ImGui::GetTextLineHeightWithSpacing() };

				draw_list->AddText( text_pos, BitCast<uint>(_style.text), _name.c_str() );
			}
		}
	}

/*
=================================================
	SetCapacity
=================================================
*/
	void  ImLineGraph::SetCapacity (uint value, uint dim)
	{
		ASSERT( value > 0 );
		ASSERT( dim > 0 and dim <= MaxGraphs );

		EXLOCK( _guard );
		ASSERT( _lines.empty() );

		_lines.resize( Clamp( dim, 1u, MaxGraphs ));
		_capacity = value;

		for (auto& line : _lines)
		{
			line.points.reserve( _capacity );

			if ( line.points.size() > _capacity )
			{
				line.points.EraseFront( line.points.size() - _capacity );
				ASSERT_Eq( line.points.size(), _capacity );
			}
		}
	}

/*
=================================================
	Add
=================================================
*/
	void  ImLineGraph::Add (ArrayView<float> values)
	{
		EXLOCK( _guard );
		ASSERT_Eq( _lines.size(), values.size() );

		for (usize i = 0, cnt = Min( _lines.size(), values.size() ); i < cnt; ++i)
		{
			if ( _lines[i].points.size()+1 > _capacity )
				_lines[i].points.pop_front();

			_lines[i].points.push_back( values[i] );
		}
	}

/*
=================================================
	AddOpt
=================================================
*/
	void  ImLineGraph::AddOpt (ArrayView<float> values)
	{
		EXLOCK( _guard );
		ASSERT_Eq( _lines.size(), values.size() );

		bool		is_valid = true;
		const usize	count	 = Min( _lines.size(), values.size() );

		for (usize i = 0; i < count; ++i)
		{
			if ( _lines[0].points.empty() and values[i] <= _range.x )
				is_valid = false;
		}

		if ( not is_valid )
			return;

		for (usize i = 0; i < count; ++i)
		{
			if ( _lines[i].points.size()+1 > _capacity )
				_lines[i].points.pop_front();

			_lines[i].points.push_back( values[i] );
		}
	}

/*
=================================================
	AddAndUpdateRange
=================================================
*/
	void  ImLineGraph::AddAndUpdateRange (ArrayView<float> values)
	{
		EXLOCK( _guard );
		ASSERT_Eq( _lines.size(), values.size() );

		for (usize i = 0, cnt = Min( _lines.size(), values.size() ); i < cnt; ++i)
		{
			_range.y = Max( _range.y, values[i] * 1.05f );

			if ( _lines[i].points.size()+1 > _capacity )
				_lines[i].points.pop_front();

			_lines[i].points.push_back( values[i] );
		}
	}

/*
=================================================
	AddNonScaled
=================================================
*/
	void  ImLineGraph::AddNonScaled (ArrayView<float> values)
	{
		return _AddNonScaled( values );
	}

	void  ImLineGraph::AddNonScaled (ArrayView<double> values)
	{
		return _AddNonScaled( values );
	}

	template <typename T>
	void  ImLineGraph::_AddNonScaled (ArrayView<T> inValues)
	{
		StaticArray< float, MaxGraphs >		values;

		for (usize i = 0; i < inValues.size(); ++i)
			values[i] = float(inValues[i]);

		Add( ArrayView<float>{ values.data(), inValues.size() });
	}

/*
=================================================
	SetRange
=================================================
*/
	void  ImLineGraph::SetRange (float min, float max)
	{
		ASSERT_GE( max, min );
		EXLOCK( _guard );
		ASSERT( _style.mode != EMode::Line_Adaptive );

		_range = float2{min, max};
	}

/*
=================================================
	Empty
=================================================
*/
	bool  ImLineGraph::Empty () const
	{
		SHAREDLOCK( _guard );

		bool	empty = true;
		for (auto& line : _lines) {
			empty &= line.points.empty();
		}
		return empty;
	}

	bool  ImLineGraph::Empty (uint dim) const
	{
		ASSERT_Lt( dim, MaxGraphs );
		SHAREDLOCK( _guard );

		return dim < _lines.size() ? _lines[dim].points.empty() : true;
	}

/*
=================================================
	LastPoint
=================================================
*/
	float  ImLineGraph::LastPoint (uint dim) const
	{
		ASSERT_Lt( dim, MaxGraphs );
		SHAREDLOCK( _guard );

		return (dim < _lines.size() and not _lines[dim].points.empty()) ?
				_lines[dim].points.back() :
				_range.x;
	}

/*
=================================================
	Set*
=================================================
*/
	void  ImLineGraph::SetName (String value)
	{
		EXLOCK( _guard );

		if ( _description.empty() or _name == _description )
			_description = value;

		_name = RVRef(value);
	}

	void  ImLineGraph::SetLabel (String label, uint dim)
	{
		EXLOCK( _guard );
		CHECK_ERRV( dim < _lines.size() );

		_lines[dim].label = RVRef(label);
	}

	void  ImLineGraph::SetSuffix (String value)
	{
		EXLOCK( _guard );
		_suffix = RVRef(value);
	}

	void  ImLineGraph::SetDescription (String value)
	{
		EXLOCK( _guard );
		_description = RVRef(value);
	}

/*
=================================================
	SetLimits
----
	limits are used to select background color
=================================================
*/
	void  ImLineGraph::SetLimits (float val1, float val2)
	{
		EXLOCK( _guard );
		_limits.x  = val1;
		_limits.y  = Max( val1, val2 );
		_invLimits = false;
	}

	void  ImLineGraph::SetInvLimits (float val1, float val2)
	{
		EXLOCK( _guard );
		_limits.x  = val1;
		_limits.y  = Max( val1, val2 );
		_invLimits = true;
	}


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
