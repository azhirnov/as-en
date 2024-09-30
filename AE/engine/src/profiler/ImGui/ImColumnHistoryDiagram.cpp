// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "base/Algorithms/StringUtils.h"
# include "profiler/ImGui/ImColumnHistoryDiagram.h"

namespace AE::Base
{
	template <typename ...Args> struct TNothrowCtor< ImVec2, Args... > : CT_True {};
}

namespace AE::Profiler
{

/*
=================================================
	Draw
=================================================
*/
	void  ImColumnHistoryDiagram::Draw (INOUT RectF &inoutRegion) const
	{
		SHAREDLOCK( _guard );

		const float		text_width			= ImGui::GetTextLineHeight() * 16;
		const float		height_threshold	= 1.0f;
		const float		rect_thickness		= 1.0f;
		const float		padding				= rect_thickness * 2.0f;
		const float		column_width		= Max( 2.f, Floor( (inoutRegion.Width() - padding - text_width) / _HistorySize - padding ));
		const float		column_step			= column_width + padding;
		const float		min_width			= column_step * _HistorySize;
		const float		min_height			= Max( 200.f, inoutRegion.Height() - padding * 2.0f );

		const RectF		diag_region			= RectF{ inoutRegion.left, inoutRegion.top,
													 inoutRegion.left + min_width + padding * 2.f, inoutRegion.top + min_height + padding * 2.f };
		const RectF		diag_region_pad		= RectF{diag_region}.Stretch2( -padding );
		ImDrawList*		draw_list			= ImGui::GetWindowDrawList();

		// draw columns diagram
		{
			const float		y_scale		= min_height;
			const float		column_y	= diag_region_pad.bottom;

			ASSERT( diag_region_pad.top + y_scale == diag_region_pad.bottom );

			// diagram border
			draw_list->AddRect( ImVec2{diag_region.left, diag_region.top},
								ImVec2{diag_region.right, diag_region.bottom},
								BitCast<uint>(HtmlColor::White), 0.f, 0, rect_thickness );

			for (int i = 0; i < _HistorySize; ++i)
			{
				const uint	fi		 = Wrap( _currentFrameIdx + i+1, 0, _HistorySize-1 );
				auto&		f		 = _frames[fi];
				const float	column_x = diag_region_pad.left + column_step * i;

				for (auto& item : f.items)
				{
					float	item_y0 = column_y - float(item.begin.count() / _maxHeight) * y_scale;
					float	item_y1 = column_y - float(item.end.count()   / _maxHeight) * y_scale;

					if ( Abs( item_y1 - item_y0 ) > height_threshold )
					{
						item_y0 = Max( item_y0, diag_region_pad.top );
						item_y1 = Max( item_y1, diag_region_pad.top );

						draw_list->AddRectFilled( ImVec2{column_x, item_y0}, ImVec2{column_x + column_width, item_y1}, BitCast<uint>(item.color) );
					}
				}
			}
		}

		const float		text_padding	= 4.f;
		const float		poly_padding	= 2.f;
		const float		poly_part1		= 4.f;
		const float		poly_part2		= poly_part1 + 10.0f;
		const float		poly_part3		= poly_part2 + 4.0f;
		const float		text_x_off		= poly_part3 + 4.0f;
		const float		text_x_off2		= text_x_off + ImGui::GetTextLineHeight() * 6;

		// draw legend
		{
			String	tmp;	tmp.reserve( 64 );
			auto&	f = _frames[ _currentFrameIdx ];

			const float		text_height	= ImGui::GetTextLineHeightWithSpacing();
			const float		column_x	= diag_region.right + poly_padding;
			const float		column_y	= diag_region_pad.bottom;
			const float		y_scale		= min_height;
			uint			txt_idx		= 0;

			for (auto& item : f.items)
			{
				float	item_y0 = float(item.begin.count() / _maxHeight) * y_scale;
				float	item_y1 = float(item.end.count()   / _maxHeight) * y_scale;
				float	text_y0	= (text_height + text_padding) * txt_idx + text_padding;
				float	text_y1	= text_y0 + text_height;
				float	text_y2 = text_y0 + text_height * 0.2f;
				float	text_y3	= text_y1 - text_height * 0.2f;

				FixedArray< ImVec2, 8 >		points;
				points.emplace_back( column_x + poly_part1,	column_y - item_y0 );
				points.emplace_back( column_x,				column_y - item_y0 );
				points.emplace_back( column_x,				column_y - item_y1 );
				points.emplace_back( column_x + poly_part1,	column_y - item_y1 );
				points.emplace_back( column_x + poly_part2,	column_y - text_y3 );
				points.emplace_back( column_x + poly_part3,	column_y - text_y3 );
				points.emplace_back( column_x + poly_part3,	column_y - text_y2 );
				points.emplace_back( column_x + poly_part2,	column_y - text_y2 );

				draw_list->AddConvexPolyFilled( points.data(), int(points.size()), BitCast<uint>(item.color) );

				tmp.clear();
				tmp << '[' << ToString( item.end - item.begin, 2 ) << "] ";

				draw_list->AddText( ImVec2{column_x + text_x_off, column_y - text_y1},
									BitCast<uint>(item.color), tmp.c_str() );

				draw_list->AddText( ImVec2{column_x + text_x_off2, column_y - text_y1},
									BitCast<uint>(item.color), _uniqueNameArr[ usize(item.name) ].Get<String>().c_str() );

				++txt_idx;
			}
		}

		inoutRegion = diag_region;
	}

/*
=================================================
	Begin
=================================================
*/
	void  ImColumnHistoryDiagram::Begin ()
	{
		_guard.lock();

		_currentFrameIdx = (_currentFrameIdx + 1) % _HistorySize;

		auto&	f = _frames[ _currentFrameIdx ];

		f.min = f.max = nanosecondsd{0.0};
		f.items.clear();
	}

/*
=================================================
	Add
=================================================
*/
	void  ImColumnHistoryDiagram::Add (StringView fullName, RGBA8u color, nanosecondsd begin, nanosecondsd end)
	{
		CHECK_ERRV( not _guard.try_lock() );

		String	name;
		{
			usize	pos = fullName.rfind( "| " );
			name = String{ pos != UMax ? fullName.substr( pos+2 ) : fullName };
		}

		auto&			f				 = _frames[ _currentFrameIdx ];
		auto			[iter, inserted] = _uniqueNames.emplace( name, UniqueNameIdx{} );
		UNameInfo_t*	info;

		if ( inserted )
		{
			iter->second = UniqueNameIdx(_uniqueNameArr.size());
			info		 = &_uniqueNameArr.emplace_back();

			info->Get<String>()			= RVRef(name);
			info->Get<UniqueNameIdx>()	= iter->second;

			if ( color == Default )
				color = RGBA8u{ Rainbow( HEHash( uint(iter->second) ))};

			color.a = 255;
			info->Get<RGBA8u>() = color;
		}
		else
		{
			info = &_uniqueNameArr[ uint(iter->second) ];

			if ( color != Default and color != info->Get<RGBA8u>() )
			{
				color.a = 255;
				info->Get<RGBA8u>() = color;
			}
		}

		f.items.push_back( Item{ iter->second, info->Get<RGBA8u>(), begin, end });
	}

/*
=================================================
	End
=================================================
*/
	void  ImColumnHistoryDiagram::End (nanosecondsd min, nanosecondsd max)
	{
		CHECK_ERRV( not _guard.try_lock() );

		// sort items
		{
			auto&	f = _frames[ _currentFrameIdx ];

			f.min	= min;
			f.max	= max;

			for (auto& item : f.items)
			{
				item.begin	-= min;
				item.end	-= min;

			//	ASSERT( item.begin >= nanosecondsd{0.0} );
			//	ASSERT( item.begin <= item.end );
			}

			std::sort( f.items.begin(), f.items.end(), [](auto& lhs, auto& rhs) { return lhs.begin < rhs.begin; });
		}

		// find max height for all frames in history
		{
			_maxHeight = 0.0;

			for (auto& f : _frames) {
				_maxHeight = Max( _maxHeight, f.Height() );
			}
		}

		_guard.unlock();
	}

	void  ImColumnHistoryDiagram::End ()
	{
		auto&			f	= _frames[ _currentFrameIdx ];
		nanosecondsd	min	{MaxValue<double>()};
		nanosecondsd	max	{0.0};

		for (auto& item : f.items)
		{
			min = Min( min, item.begin );
			max = Max( max, item.end );
		}

		return End( min, max );
	}


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
