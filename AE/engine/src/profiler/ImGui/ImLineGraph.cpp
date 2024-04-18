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
	void  ImLineGraph::Draw (const RectF &inRegion) const
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
		const float		y_scale				= line_region.Height() / (_range.y - _range.x);
		ImDrawList*		draw_list			= ImGui::GetWindowDrawList();

		draw_list->AddRectFilled( ImVec2{border_region.left, border_region.top},
								  ImVec2{border_region.right, border_region.bottom},
								  BitCast<uint>(_style.background), rounding, 0 );

		draw_list->AddRect( ImVec2{border_region.left, border_region.top},
							ImVec2{border_region.right, border_region.bottom},
							BitCast<uint>(_style.border), rounding, 0, border_thickness );

		if ( _style.mode == EMode::Line or
			 _style.mode == EMode::Line_Adaptive )
		{
			const bool	adaptive	= (_style.mode == EMode::Line_Adaptive);
			float		min_y		= 1.e+38f;

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
				}
				draw_list->AddPolyline( dst.data(), int(dst.size()), BitCast<uint>(_style.lines[l]), 0, line_thickness );
			}

			if ( not _name.empty() )
			{
				const ImVec2	text_pos{ line_region.left + line_region.Width() * 0.01f,
										  border_region.top + ImGui::GetTextLineHeightWithSpacing() * 0.1f };

				draw_list->AddText( text_pos, BitCast<uint>(_style.text), _name.c_str() );
			}

			for (usize l = 0; l < _lines.size(); ++l)
			{
				const ImVec2	text_pos{ line_region.left + line_region.Width() * 0.08f,
										  border_region.top + ImGui::GetTextLineHeightWithSpacing() * (l + 1.1f) };

				draw_list->AddText( text_pos, BitCast<uint>(_style.lines[l]), _lines[l].label.c_str() );
			}

			if ( adaptive )
				_range.x = Min( _range.y * 0.99f, min_y * 0.9f );
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

		_lines.resize( Clamp( dim, 1u, MaxGraphs ));
		_capacity = value;

		for (auto& line : _lines)
		{
			line.points.reserve( _capacity );

			if ( line.points.size() > _capacity )
			{
				line.points.EraseFront( line.points.size() - _capacity );
				ASSERT( line.points.size() == _capacity );
			}
		}
	}

/*
=================================================
	Add
=================================================
*/
	void  ImLineGraph::Add (std::initializer_list<float> values)
	{
		EXLOCK( _guard );
		ASSERT( _lines.size() == values.size() );

		for (usize i = 0; i < _lines.size(); ++i)
		{
			if ( _lines[i].points.size()+1 > _capacity )
				_lines[i].points.pop_front();

			_lines[i].points.push_back( values.begin()[i] );
		}
	}

/*
=================================================
	AddOpt
=================================================
*/
	void  ImLineGraph::AddOpt (std::initializer_list<float> values)
	{
		EXLOCK( _guard );
		ASSERT( _lines.size() == values.size() );

		bool	is_valid = true;
		for (usize i = 0; i < _lines.size(); ++i)
		{
			if ( _lines[0].points.empty() and values.begin()[i] <= _range.x )
				is_valid = false;
		}

		if ( not is_valid )
			return;

		for (usize i = 0; i < _lines.size(); ++i)
		{
			if ( _lines[i].points.size()+1 > _capacity )
				_lines[i].points.pop_front();

			_lines[i].points.push_back( values.begin()[i] );
		}
	}

/*
=================================================
	AddAndUpdateRange
=================================================
*/
	void  ImLineGraph::AddAndUpdateRange (std::initializer_list<float> values)
	{
		EXLOCK( _guard );
		ASSERT( _lines.size() == values.size() );

		for (usize i = 0; i < _lines.size(); ++i)
		{
			_range.y = Max( _range.y, values.begin()[i] * 1.05f );

			if ( _lines[i].points.size()+1 > _capacity )
				_lines[i].points.pop_front();

			_lines[i].points.push_back( values.begin()[i] );
		}
	}

/*
=================================================
	SetRange
=================================================
*/
	void  ImLineGraph::SetRange (float min, float max)
	{
		ASSERT( max >= min );
		EXLOCK( _guard );

		_range = float2{min, max};
	}

/*
=================================================
	Empty
=================================================
*/
	bool  ImLineGraph::Empty (uint dim) const
	{
		ASSERT( dim < MaxGraphs );
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
		ASSERT( dim < MaxGraphs );
		SHAREDLOCK( _guard );

		return (dim < _lines.size() and not _lines[dim].points.empty()) ?
				_lines[dim].points.back() :
				_range.x;
	}

/*
=================================================
	SetName
=================================================
*/
	void  ImLineGraph::SetName (StringView value)
	{
		EXLOCK( _guard );
		_name = String{value};
	}

	void  ImLineGraph::SetLabel (StringView label, uint dim)
	{
		EXLOCK( _guard );
		ASSERT( dim < _lines.size() );

		_lines[dim].label = String{label};
	}


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
