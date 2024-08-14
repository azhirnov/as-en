// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "profiler/ImGui/ImLineGraphTable.h"

namespace AE::Profiler
{

/*
=================================================
	GraphMap::Empty
=================================================
*/
	bool  ImLineGraphTable::GraphMap::Empty () const
	{
		bool	empty = true;
		for (auto& graph : ordered) {
			empty &= graph->Empty();
		}
		return empty;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	Draw
=================================================
*/
	void  ImLineGraphTable::Draw (const float width, const float rowHeight, const float2 padding, const bool isHover) const
	{
		const uint	col_per_row	= 3;
		float		col_pos [col_per_row+2];
		const float	pos_x		= ImGui::GetCursorScreenPos().x;

		for (usize i = 0; i < CountOf(col_pos); ++i)
			col_pos[i] = pos_x + padding.x + width * (float(i) / (CountOf(col_pos)-1));

		ImGui::SetCursorScreenPos( ImVec2{ pos_x, ImGui::GetCursorScreenPos().y });

		for (auto& sec : _sections.ordered)
		{
			if ( sec.Empty() )
				continue;

			ImGui::TextUnformatted( sec.caption.c_str() );

			float	y0	= ImGui::GetCursorScreenPos().y;
			float	y1	= y0 + rowHeight;
			uint	i	= 0;

			for (auto& graph : sec.ordered)
			{
				if ( graph->Empty() )
					continue;

				graph->Draw( RectF{ col_pos[i], y0,  col_pos[i+1], y1 }, isHover );
				++i;

				if_unlikely( i == col_per_row+1 )
				{
					ImGui::SetCursorScreenPos( ImVec2{ pos_x, y1 + padding.y });
					y0 = ImGui::GetCursorScreenPos().y;
					y1 = y0 + rowHeight;
					i  = 0;
				}
			}

			if ( i != 0 )
				ImGui::SetCursorScreenPos( ImVec2{ pos_x, y1 + padding.y });
		}
	}

/*
=================================================
	Add
=================================================
*/
	ImLineGraph&  ImLineGraphTable::Add (SecName::Ref section, GraphName::Ref graph)
	{
		auto	s_it = _sections.map.find( section );

		if_unlikely( s_it == _sections.map.end() )
		{
			s_it = _sections.map.emplace( section, uint(_sections.ordered.size()) ).first;
			_sections.ordered.emplace_back();
		}

		auto&	sec  = _sections.ordered[ s_it->second ];
		auto	g_it = sec.map.find( graph );

		if_likely( g_it == sec.map.end() )
		{
			g_it = sec.map.emplace( graph, uint(sec.ordered.size()) ).first;
			sec.ordered.push_back( MakeUnique<ImLineGraph>() );

			CHECK( _allGraphs.emplace( graph, uint2{s_it->second, g_it->second} ).second );
		}else{
			CHECK_MSG( false, "section already created" );
		}

		return *sec.ordered[ g_it->second ];
	}

/*
=================================================
	Get
=================================================
*/
	Ptr<ImLineGraph>  ImLineGraphTable::Get (SecName::Ref section, GraphName::Ref graph)
	{
		auto	s_it = _sections.map.find( section );

		if ( s_it == _sections.map.end() )
			return null;

		auto&	sec  = _sections.ordered[ s_it->second ];
		auto	g_it = sec.map.find( graph );

		if ( g_it == sec.map.end() )
			return null;

		return sec.ordered[ g_it->second ].get();
	}

/*
=================================================
	Get
=================================================
*/
	Ptr<ImLineGraph>  ImLineGraphTable::Get (GraphName::Ref graph)
	{
		auto	it = _allGraphs.find( graph );

		if_unlikely( it == _allGraphs.end() )
			return null;

		auto&	sec = _sections.ordered[ it->second.x ];
		return sec.ordered[ it->second.y ].get();
	}

/*
=================================================
	SetCaption
=================================================
*/
	bool  ImLineGraphTable::SetCaption (SecName::Ref section, String cap)
	{
		auto	it = _sections.map.find( section );

		if_unlikely( it == _sections.map.end() )
			return false;

		auto&	sec  = _sections.ordered[ it->second ];

		sec.caption = RVRef(cap);
		return true;
	}


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
