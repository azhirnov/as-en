// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "base/Math/Random.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Platforms/CPUInfo.h"
# include "profiler/Impl/HwpcProfiler.h"
# include "graphics/Public/GraphicsImpl.h"

namespace AE::Profiler
{
namespace
{
	static const float	padding[2] = { 2.0f, 8.f };
}

/*
=================================================
	DrawImGUI
=================================================
*/
	void  HwpcProfiler::DrawImGUI ()
	{
		if ( not _initialized )
			return;

	  #ifdef AE_PLATFORM_ANDROID
		ImGui::SetNextWindowPos( ImVec2{700, 0}, ImGuiCond_Once );
		ImGui::SetNextWindowSize( ImVec2{800, 900}, ImGuiCond_Once );

		const ImGuiWindowFlags	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
	  #else
		const ImGuiWindowFlags	flags = ImGuiWindowFlags_NoScrollbar;
	  #endif

		if ( ImGui::Begin( "HWProfiler", null, flags ))
		{
			_DrawCpuUsageImGui();

			_DrawCpuProfilerArmImGui();
			_DrawGpuProfilerArmImGui();

			ImGui::End();
		}
	}

/*
=================================================
	_DrawCpuUsageImGui
=================================================
*/
	void  HwpcProfiler::_DrawCpuUsageImGui ()
	{
		const float		wnd_pos_x	= ImGui::GetCursorScreenPos().x;
		const float		x_offset	= wnd_pos_x + padding[0];
		const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "CPU usage", 0 ))
		{
			const auto&		cpu_info		= CpuArchInfo::Get();
			const uint		core_per_line	= _cpuUsage.corePerLine;
			const float		graph_width		= wnd_size.x / core_per_line;
			const float		graph_height	= 100.f;
			float2			left_top		= float2{ x_offset, 0.f };

			for (auto& core : cpu_info.cpu.coreTypes)
			{
				ImGui::TextUnformatted( core.name.c_str() );

				left_top.x = x_offset;
				left_top.y = ImGui::GetCursorScreenPos().y;

				for (ulong bits = core.logicalBits.to_ullong(), i = 1; bits != 0; ++i)
				{
					uint	core_id = ExtractBitIndex( INOUT bits );
					auto&	graph	= _cpuUsage.coreUsage[core_id];

					RectF	region;
					region.left		= left_top.x;
					region.right	= left_top.x + graph_width;
					region.top		= left_top.y;
					region.bottom	= left_top.y + graph_height;
					left_top.x		= region.right;

					graph.Draw( INOUT region );

					if ( i >= core_per_line )
					{
						i = 0;
						left_top.x  = x_offset;
						left_top.y += graph_height;
					}
				}
				left_top.y += graph_height;
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, left_top.y });
			}
			ImGui::TreePop();
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, left_top.y - (left_top.x == x_offset ? graph_height : 0.f) + padding[1] });
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_DrawCpuProfilerArmImGui
=================================================
*/
	void  HwpcProfiler::_DrawCpuProfilerArmImGui ()
	{
		const float		graph_height	= 150.f;
		const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
		const float		x_offset		= wnd_pos_x + padding[0];
		const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "CPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const float		xp [4] = { x_offset, x_offset + wnd_size.x*0.333f, x_offset + wnd_size.x*0.666f, x_offset + wnd_size.x };
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });

			if ( not _armProf.cpuCycles.Empty()		or
				 not _armProf.cacheMisses.Empty()	or
				 not _armProf.branchMisses.Empty()	)
			{
				_armProf.cpuCycles	 .SetName( "CPU cycles\n"s    << ToString( _armProf.cpuCycles.LastPoint(),    2 ) << " M/s" );
				_armProf.cacheMisses .SetName( "cache misses\n"s  << ToString( _armProf.cacheMisses.LastPoint(),  2 ) << " M/s" );
				_armProf.branchMisses.SetName( "branch misses\n"s << ToString( _armProf.branchMisses.LastPoint(), 2 ) << " M/s" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				_armProf.cpuCycles   .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				_armProf.cacheMisses .Draw( RectF{ xp[1], y0,  xp[2], y1 });
				_armProf.branchMisses.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not _armProf.cacheRefs.Empty()	 or
				 not _armProf.branchInst.Empty() )
			{
				_armProf.cacheRefs .SetName( "cache refs\n"s  << ToString( _armProf.cacheRefs.LastPoint(),  2 ) << " M/s" );
				_armProf.branchInst.SetName( "branch inst\n"s << ToString( _armProf.branchInst.LastPoint(), 2 ) << " M/s" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				_armProf.cacheRefs .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				_armProf.branchInst.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			ImGui::TreePop();
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_DrawGpuProfilerArmImGui
=================================================
*/
	void  HwpcProfiler::_DrawGpuProfilerArmImGui ()
	{
		String			str;
		const float		graph_height	= 150.f;
		const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
		const float		x_offset		= wnd_pos_x + padding[0];
		const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "Graphics profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const float		xp [4] = { x_offset, x_offset + wnd_size.x*0.333f, x_offset + wnd_size.x*0.666f, x_offset + wnd_size.x };
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });

			if ( not _armProf.globalMemStalls.Empty()	or
				 not _armProf.globalMemTraffic.Empty()	or
				 not _armProf.globalMemAccess.Empty()	)
			{
				str.clear();
				str	<< "Traffic: "
					<< ToString(Bytes{ulong(_armProf.globalMemTraffic.LastPoint(0)) << 10}) << "/s | "
					<< ToString(Bytes{ulong(_armProf.globalMemTraffic.LastPoint(1)) << 10}) << "/s";
				ImGui::TextUnformatted( str.c_str() );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				_armProf.globalMemStalls .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				_armProf.globalMemTraffic.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				_armProf.globalMemAccess .Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not _armProf.primitives.Empty()	or
				 not _armProf.zTest.Empty()			or
				 not _armProf.cacheLookups.Empty()	)
			{
				str.clear();
				str << "Tris: "
					<< ToString(ulong(_armProf.primitives.LastPoint(0))) << " | "
					<< ToString(ulong(_armProf.primitives.LastPoint(1))) << " | "
					<< ToString(ulong(_armProf.primitives.LastPoint(2)));
				ImGui::TextUnformatted( str.c_str() );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				_armProf.primitives  .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				_armProf.zTest       .Draw( RectF{ xp[1], y0,  xp[2], y1 });
				_armProf.cacheLookups.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not _armProf.gpuCycles.Empty()		or
				 not _armProf.shaderCycles.Empty()	or
				 not _armProf.shaderJobs.Empty()	)
			{
				_armProf.gpuCycles.SetName( "GPU cycles\n"s << ToString( _armProf.gpuCycles.LastPoint(), 2 ) << " M/s" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				_armProf.gpuCycles   .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				_armProf.shaderCycles.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				_armProf.shaderJobs  .Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not _armProf.textureCycles.Empty()	 or
				 not _armProf.tiles.Empty()			 or
				 not _armProf.loadStoreCycles.Empty() )
			{
				_armProf.textureCycles	.SetName( "tex cycles\n"s << ToString( _armProf.textureCycles.LastPoint(),   2 ) << " M/s" );
				_armProf.tiles			.SetName( "tiles\n"s      << ToString( _armProf.tiles.LastPoint(),           2 ) << " M/s" );
				_armProf.loadStoreCycles.SetName( "load/store\n"s << ToString( _armProf.loadStoreCycles.LastPoint(), 2 ) << " M/s" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				_armProf.textureCycles  .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				_armProf.tiles          .Draw( RectF{ xp[1], y0,  xp[2], y1 });
				_armProf.loadStoreCycles.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			ImGui::TreePop();
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateArmCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateArmCountersImGui (double invdt)
	{
		using ECounter = ArmProfiler::ECounter;

		_armProf.profiler.Sample( OUT _armProf.counters );
		if ( _armProf.counters.empty() )
			return;

		const auto	AddPoint = [cnt = &_armProf.counters] (ECounter type, INOUT ImLineGraph &graph, double scale)
		{{
			if ( auto it = cnt->find( type ); it != cnt->end())		graph.AddAndUpdateRange({ float(it->second * scale) });
		}};

		const auto	AddPoint2 = [cnt = &_armProf.counters] (ECounter type0, ECounter type1, INOUT ImLineGraph &graph, double scale)
		{{
			double	value0 = 0.0,	value1 = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 += it->second;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 += it->second;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(value0 * scale), float(value1 * scale) });
		}};

		const auto	AddPoint3 = [cnt = &_armProf.counters] (ECounter type0, ECounter type1, ECounter type2, INOUT ImLineGraph &graph, double scale)
		{{
			double	value0 = 0.0,	value1 = 0.0,	value2 = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 += it->second;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 += it->second;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 += it->second;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(value0 * scale), float(value1 * scale), float(value2 * scale) });
		}};

		const auto	AddPoint4 = [cnt = &_armProf.counters] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, INOUT ImLineGraph &graph, double scale)
		{{
			double	value0 = 0.0,	value1 = 0.0,	value2 = 0.0,	value3 = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 += it->second;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 += it->second;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 += it->second;  exists = true; }
			if ( auto it = cnt->find( type3 ); it != cnt->end())	{ value3 += it->second;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(value0 * scale), float(value1 * scale), float(value2 * scale), float(value3 * scale) });
		}};

		const auto	AddPoint_zTest = [cnt = &_armProf.counters] (INOUT ImLineGraph &graph, double scale)
		{{
			double	tests = 0.0,  killed = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( ECounter::GPU_EarlyZTests  );  it != cnt->end())	{ tests  += it->second;  exists = true; }
			if ( auto it = cnt->find( ECounter::GPU_LateZTests   );  it != cnt->end())	{ tests  += it->second;  exists = true; }
			if ( auto it = cnt->find( ECounter::GPU_EarlyZKilled );  it != cnt->end())	{ killed += it->second;  exists = true; }
			if ( auto it = cnt->find( ECounter::GPU_LateZKilled  );  it != cnt->end())	{ killed += it->second;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(tests * scale), float(killed * scale) });
		}};

		AddPoint2( ECounter::GPU_ExternalMemoryReadStalls,		ECounter::GPU_ExternalMemoryWriteStalls,	INOUT _armProf.globalMemStalls,		invdt );
		AddPoint2( ECounter::GPU_ExternalMemoryReadBytes,		ECounter::GPU_ExternalMemoryWriteBytes,		INOUT _armProf.globalMemTraffic,	invdt / 1024.0 );
		AddPoint2( ECounter::GPU_ExternalMemoryReadAccesses,	ECounter::GPU_ExternalMemoryWriteAccesses,	INOUT _armProf.globalMemAccess,		invdt );
		AddPoint2( ECounter::GPU_CacheReadLookups,				ECounter::GPU_CacheWriteLookups,			INOUT _armProf.cacheLookups,		invdt );

		AddPoint3( ECounter::GPU_VisiblePrimitives,	ECounter::GPU_CulledPrimitives, ECounter::GPU_InputPrimitives,										INOUT _armProf.primitives,		invdt	);
		AddPoint4( ECounter::GPU_VertexCycles,		ECounter::GPU_FragmentCycles,	ECounter::GPU_ComputeCycles,	ECounter::GPU_VertexComputeCycles,	INOUT _armProf.shaderCycles,	invdt	);
		AddPoint4( ECounter::GPU_VertexJobs,		ECounter::GPU_FragmentJobs,		ECounter::GPU_ComputeJobs,		ECounter::GPU_VertexComputeJobs,	INOUT _armProf.shaderJobs,		invdt	);

		AddPoint(  ECounter::GPU_Cycles,				INOUT _armProf.gpuCycles,			invdt * 1.0e-6 );
		AddPoint(  ECounter::GPU_ShaderTextureCycles,	INOUT _armProf.textureCycles,		invdt * 1.0e-6 );
		AddPoint(  ECounter::GPU_Tiles,					INOUT _armProf.tiles,				invdt * 1.0e-6 );
		AddPoint(  ECounter::GPU_ShaderLoadStoreCycles,	INOUT _armProf.loadStoreCycles,		invdt * 1.0e-6 );

		AddPoint(  ECounter::CPU_Cycles,				INOUT _armProf.cpuCycles,			invdt * 1.0e-6 );
		AddPoint(  ECounter::CPU_CacheMisses,			INOUT _armProf.cacheMisses,			invdt * 1.0e-6 );
		AddPoint(  ECounter::CPU_BranchMisses,			INOUT _armProf.branchMisses,		invdt * 1.0e-6 );
		AddPoint(  ECounter::CPU_CacheReferences,		INOUT _armProf.cacheRefs,			invdt * 1.0e-6 );
		AddPoint(  ECounter::CPU_BranchInstructions,	INOUT _armProf.branchInst,			invdt * 1.0e-6 );

		AddPoint_zTest( INOUT _armProf.zTest, invdt );
	}

/*
=================================================
	_UpdateAndroidGpuCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateAndroidGpuCountersImGui (secondsf)
	{
	}

/*
=================================================
	_InitImGui
=================================================
*/
	void  HwpcProfiler::_InitImGui ()
	{
		ImLineGraph::ColorStyle		style1;
		style1.lines[0]		= RGBA8u{ 20, 170,  20, 255};
		style1.background	= RGBA8u{  0,   0,  50, 255};
		style1.border		= RGBA8u{200, 200, 255, 255};
		style1.text			= RGBA8u{255,  50, 100, 255};
		style1.mode			= ImLineGraph::EMode::Column;

		ImLineGraph::ColorStyle		style2;
		style2.lines[0]		= RGBA8u{255,   0,   0, 255};
		style2.lines[1]		= RGBA8u{  0, 255,   0, 255};
		style2.lines[2]		= RGBA8u{200,  50, 255, 255};
		style2.lines[3]		= RGBA8u{200, 200,  50, 255};
		style2.background	= RGBA8u{  0,   0,  50, 255};
		style2.border		= RGBA8u{200, 200, 255, 255};
		style2.text			= RGBA8u{200, 200, 200, 255};
		style2.mode			= ImLineGraph::EMode::Line;

		ImLineGraph::ColorStyle		style3 = style1;
		style3.text			= RGBA8u{ 50, 200, 200, 255};
		style3.mode			= ImLineGraph::EMode::Line_Adaptive;

		const uint		capacity1 = 100;
		const uint		capacity2 = 50;

		// CPU usage
		{
			const auto&		cpu_info = CpuArchInfo::Get();

			_cpuUsage.coreUsage.resize( cpu_info.cpu.logicalCoreCount );
			_cpuUsage.corePerLine = 1;

			for (auto& core : cpu_info.cpu.coreTypes)
			{
				for (uint core_id : BitIndexIterate( core.logicalBits.to_ullong() ))
				{
					auto&	graph = _cpuUsage.coreUsage[core_id];
					graph.SetCapacity( capacity1, 2 );
					graph.SetColor( style2 );
					graph.SetRange( 0.f, 1.f );
					graph.SetName( ToString(core_id) );
					graph.SetLabel( "user",   0 );
					graph.SetLabel( "kernel", 1 );
				}

				_cpuUsage.corePerLine = Max( _cpuUsage.corePerLine, core.LogicalCount() );
			}

			if ( cpu_info.cpu.coreTypes.size() == 1 )
				_cpuUsage.corePerLine = Max( 1u, uint( Sqrt( float(cpu_info.cpu.logicalCoreCount) ) + 0.5f ));
		}

		Unused( capacity2 );
		{
			auto&	graph = _armProf.globalMemStalls;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Stalls" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.globalMemTraffic;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Traffic" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.globalMemAccess;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Access" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.primitives;
			graph.SetCapacity( capacity2, 3 );
			graph.SetName( "Tris" );
			graph.SetLabel( "Culled",  0 );
			graph.SetLabel( "Visible", 1 );
			graph.SetLabel( "Input",   2 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.zTest;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Z" );
			graph.SetLabel( "Tests",  0 );
			graph.SetLabel( "Killed", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.cacheLookups;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Cache lookups" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.shaderCycles;
			graph.SetCapacity( capacity2, 4 );
			graph.SetName( "Cycles" );
			graph.SetLabel( "vert", 0 );
			graph.SetLabel( "frag", 1 );
			graph.SetLabel( "comp", 2 );
			graph.SetLabel( "v/c",  3 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.shaderJobs;
			graph.SetCapacity( capacity2, 4 );
			graph.SetName( "Jobs" );
			graph.SetLabel( "vert", 0 );
			graph.SetLabel( "frag", 1 );
			graph.SetLabel( "comp", 2 );
			graph.SetLabel( "v/c",  3 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}

		{
			auto&	graph = _armProf.gpuCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.textureCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.tiles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.loadStoreCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}

		// processor
		{
			auto&	graph = _armProf.cpuCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.cacheMisses;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.branchMisses;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.cacheRefs;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = _armProf.branchInst;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}
	}

} // AE::Profiler

#endif // AE_ENABLE_IMGUI
