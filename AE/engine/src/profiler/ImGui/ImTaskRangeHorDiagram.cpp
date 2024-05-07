// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "base/Algorithms/StringUtils.h"
# include "profiler/ImGui/ImTaskRangeHorDiagram.h"

namespace AE::Profiler
{

/*
=================================================
	FrameData::Clear
=================================================
*/
	void  ImTaskRangeHorDiagram::FrameData::Clear ()
	{
		tasks.clear();
		threads.clear();
		threadInfos.clear();
		sortedThreads.clear();

		min = nanosecondsd{MaxValue<double>()};
		max = nanosecondsd{0.0};
	}

/*
=================================================
	Draw
=================================================
*/
	void  ImTaskRangeHorDiagram::Draw (INOUT RectF &inoutRegion) const
	{
		SHAREDLOCK( _guard );

		const auto&		f = _frames[ _frameIdx ];

		const float		rect_thickness		= 1.0f;
		const float		padding				= rect_thickness * 2.0f;
		const float		text_width			= ImGui::GetTextLineHeight() * 16;
		const float		min_width			= Max( 300.f, Floor( inoutRegion.Width() - padding * 2.f - text_width ));
		const float		row_height			= ImGui::GetTextLineHeightWithSpacing();
		const float		min_height			= _maxThreads * row_height;
		const float		text_padding		= 10.f;

		const RectF		diag_region			= RectF{ inoutRegion.left, inoutRegion.top,
													 inoutRegion.left + min_width + padding * 2.f, inoutRegion.top + min_height + padding * 2.f };
		const RectF		diag_region_pad		= RectF{diag_region}.Stretch2( -padding );
		ImDrawList*		draw_list			= ImGui::GetWindowDrawList();

		// diagram border
		draw_list->AddRect( ImVec2{diag_region.left, diag_region.top},
							ImVec2{diag_region.right, diag_region.bottom},
							BitCast<uint>(HtmlColor::White), 0.f, 0, rect_thickness );

		// draw threads
		{
			for (auto [t_id, t_idx] : f.threads)
			{
				const uint		idx		= f.sortedThreads[ t_idx ];
				const float		row_y	= diag_region_pad.top + row_height * idx;
				const auto&		info	= f.threadInfos[ t_idx ];
				const uint		color	= BitCast<uint>( info.color );

				if ( idx > 0 )
				{
					draw_list->AddLine( ImVec2{diag_region_pad.left,  row_y},
										ImVec2{diag_region_pad.right + text_padding*2.f, row_y},
										BitCast<uint>(HtmlColor::White), rect_thickness );
				}
				draw_list->AddText( ImVec2{diag_region_pad.right + text_padding, row_y}, color,
									info.name.data(), info.name.data() + info.name.size() );
			}
		}

		// draw tasks
		{
			const double	scale_x		= double(diag_region_pad.Width()) / (f.max - f.min).count();
			const float		base_x		= diag_region_pad.left;

			for (auto& t : f.tasks)
			{
				if ( IsNaN( t.begin ) or IsNaN( t.end ))
					continue;

				uint	t_idx	= f.sortedThreads[ t.threadIdx ];
				double	begin	= (IsNaN( t.begin ) ? (f.min) : t.begin).count();
				double	end		= (IsNaN( t.end )   ? (f.max) : t.end).count();

				float	x0		= base_x + float((begin - f.min.count()) * scale_x);
				float	x1		= base_x + float((end - f.min.count()) * scale_x);

				float	y0		= diag_region_pad.top + row_height * t_idx + rect_thickness;
				float	y1		= y0 + row_height - rect_thickness * 2.0f;

				draw_list->AddRectFilled( ImVec2{x0, y0}, ImVec2{x1, y1}, BitCast<uint>(t.color) );
			}
		}

		ImGui::SetCursorScreenPos( ImVec2{ diag_region.left, diag_region.bottom + padding * 2.f });
		ImGui::Text( "time range: %s", ToString( f.max - f.min ).c_str() );

		// draw tree
		if ( _enableTreeView )
		{
			String	str;

			for (usize i = 0, cnt = f.sortedThreads.size(); i < cnt; ++i)
			{
				const uint	info_idx	= f.sortedThreads[i];
				const auto&	info		= f.threadInfos[ info_idx ];
				const void*	id			= BitCast<void*>( f.threads.GetKeyArray()[ info_idx ]);

				if ( ImGui::TreeNodeEx( id, ImGuiTreeNodeFlags_DefaultOpen, "%s", info.name.data() ))
				{
					for (usize j = 0, k = 0; (j < f.tasks.size() and k < 20); ++j)
					{
						auto&	task = f.tasks[j];

						if ( task.threadIdx != info_idx )
							continue;

						if ( IsNaN( task.begin ) or IsNaN( task.end ))
							continue;

						str.clear();
						str << task.name << " (" << ToString( secondsd{ task.end - task.begin }) << ")";

						RGBA32f	c {task.color};
						ImGui::TextColored( ImVec4{c.r, c.g, c.b, c.a}, str.c_str(), "" );
						++k;
					}
					ImGui::TreePop();
				}
			}
		}
	}

/*
=================================================
	Begin
=================================================
*/
	void  ImTaskRangeHorDiagram::Begin ()
	{
		_guard.lock();

		++_frameIdx;

		auto&	f = _frames[ _frameIdx ];

		_maxThreads = Max( _maxThreads, uint(f.threads.size()) );

		f.Clear();
	}

/*
=================================================
	Add
=================================================
*/
	void  ImTaskRangeHorDiagram::Add (StringView name, RGBA8u color, nanosecondsd begin, nanosecondsd end, usize threadId, StringView threadCaption)
	{
		CHECK_ERRV( not _guard.try_lock() );

		auto&	f = _frames[ _frameIdx ];

		// NaN is outside of capture interval
		if ( not IsNaN( begin ) and not IsNaN( end ))
		{
			f.min = Min( f.min, begin );
			f.max = Max( f.max, end );
		}
		auto [iter, inserted] = f.threads.emplace( ThreadID(threadId), CheckCast<InfoIndex>(f.threadInfos.size()) );

		if ( inserted )
		{
			ASSERT( not threadCaption.empty() );
			f.sortedThreads.push_back( CheckCast<InfoIndex>(f.threadInfos.size()) );

			RGBA8u	tcolor;
			IntToColor( OUT tcolor, threadId );

			f.threadInfos.push_back( ThreadInfo{ threadCaption, tcolor });
		}
		ASSERT( iter->second < f.threadInfos.size() );

		f.tasks.emplace_back( Task{ String{name}, color, iter->second, begin, end });
	}

/*
=================================================
	End
=================================================
*/
	void  ImTaskRangeHorDiagram::End ()
	{
		CHECK_ERRV( not _guard.try_lock() );

		auto&	f = _frames[ _frameIdx ];

		ASSERT( f.sortedThreads.size() == f.threadInfos.size() );
		ASSERT( f.sortedThreads.size() == f.threads.size() );

		std::sort( f.sortedThreads.begin(), f.sortedThreads.end(),
					[&f] (uint lhs, uint rhs)
					{
						auto&	l = f.threadInfos[ lhs ];
						auto&	r = f.threadInfos[ rhs ];
						return BitCast<uint>(l.color) < BitCast<uint>(r.color);
					});

		_guard.unlock();
	}


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
