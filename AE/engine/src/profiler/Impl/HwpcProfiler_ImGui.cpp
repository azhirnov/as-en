// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "base/Math/Random.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Platforms/CPUInfo.h"
# include "profiler/Impl/HwpcProfiler.h"
# include "graphics/GraphicsImpl.h"

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

			_DrawProfilerAdrenoImGui();

			_DrawProfilerPowerVRImGui();
		}
		ImGui::End();
	}

/*
=================================================
	_InitImGui
=================================================
*/
	void  HwpcProfiler::_InitImGui ()
	{
		ImLineGraph::ColorStyle		style2;
		style2.lines[0]		= RGBA8u{255,   0,   0, 255};
		style2.lines[1]		= RGBA8u{  0, 255,   0, 255};
		style2.lines[2]		= RGBA8u{200,  50, 255, 255};
		style2.lines[3]		= RGBA8u{200, 200,  50, 255};
		style2.background	= RGBA8u{  0,   0,  50, 255};
		style2.border		= RGBA8u{200, 200, 255, 255};
		style2.text			= RGBA8u{200, 200, 200, 255};
		style2.mode			= ImLineGraph::EMode::Line;

		// CPU usage
		if ( _cpuUsage.enabled )
		{
			const uint		capacity1	= 100;
			const auto&		cpu_info	= CpuArchInfo::Get();

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
					graph.SetLabel( "total",  0 );
					graph.SetLabel( "kernel", 1 );
				}

				_cpuUsage.corePerLine = Max( _cpuUsage.corePerLine, core.LogicalCount() );
			}

			if ( cpu_info.cpu.coreTypes.size() == 1 )
				_cpuUsage.corePerLine = Max( 1u, uint( Sqrt( float(cpu_info.cpu.logicalCoreCount) ) + 0.5f ));
		}

		style2.mode			= ImLineGraph::EMode::Line_Adaptive;

		ImLineGraph::ColorStyle		style3;
		style3.lines[0]		= RGBA8u{ 20, 170,  20, 255};
		style3.background	= RGBA8u{  0,   0,  50, 255};
		style3.border		= RGBA8u{200, 200, 255, 255};
		style3.text			= RGBA8u{255,  50, 100, 255};
		style3.text			= RGBA8u{ 50, 200, 200, 255};
		style3.mode			= ImLineGraph::EMode::Line_Adaptive;

		_InitArmCountersImGui( style2, style3 );
		_InitAdrenoCountersImGui( style2, style3 );
		_InitPowerVRCountersImGui( style2, style3 );
	}
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	_DrawCpuProfilerArmImGui
=================================================
*/
	void  HwpcProfiler::_DrawCpuProfilerArmImGui ()
	{
		auto&	prof = _armProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		const float		graph_height	= 150.f;
		const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
		const float		x_offset		= wnd_pos_x + padding[0];
		const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "ARM CPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const float		xp [4] = { x_offset, x_offset + wnd_size.x*0.333f, x_offset + wnd_size.x*0.666f, x_offset + wnd_size.x };
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });

			if ( not prof.cpuCycles.Empty()		or
				 not prof.cacheMisses.Empty()	or
				 not prof.branchMisses.Empty()	)
			{
				prof.cpuCycles	 .SetName( "CPU cycles\n"s    << ToString( prof.cpuCycles.LastPoint(),    2 ) << " M/f" );
				prof.cacheMisses .SetName( "cache misses\n"s  << ToString( prof.cacheMisses.LastPoint(),  2 ) << " M/f" );
				prof.branchMisses.SetName( "branch misses\n"s << ToString( prof.branchMisses.LastPoint(), 2 ) << " M/f" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.cpuCycles   .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.cacheMisses .Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.branchMisses.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.cacheRefs.Empty()	 or
				 not prof.branchInst.Empty() )
			{
				prof.cacheRefs .SetName( "cache refs\n"s  << ToString( prof.cacheRefs.LastPoint(),  2 ) << " M/f" );
				prof.branchInst.SetName( "branch inst\n"s << ToString( prof.branchInst.LastPoint(), 2 ) << " M/f" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.cacheRefs .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.branchInst.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}
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
		auto&	prof = _armProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		String			str;
		const float		graph_height	= 150.f;
		const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
		const float		x_offset		= wnd_pos_x + padding[0];
		const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "ARM GPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const float		xp [6] = {	x_offset, x_offset + wnd_size.x*0.25f, x_offset + wnd_size.x*0.5f, x_offset + wnd_size.x*0.75f, x_offset + wnd_size.x };
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });

			// global memory line
			if ( not prof.globalMemTraffic.Empty()	or
				 not prof.globalMemAccess.Empty()	or
				 not prof.globalMemStalls.Empty()	or
				 not prof.cacheLookups.Empty()	)
			{
				str.clear();
				str	<< "Global memory";
				if ( not prof.globalMemTraffic.Empty() )
				{
					str	<< ", Traffic: "
						<< ToString(Bytes{ulong(prof.globalMemTraffic.LastPoint(0)) << 10}) << "/f | "
						<< ToString(Bytes{ulong(prof.globalMemTraffic.LastPoint(1)) << 10}) << "/f";
				}
				ImGui::TextUnformatted( str.c_str() );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.globalMemTraffic.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.globalMemAccess .Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.globalMemStalls .Draw( RectF{ xp[2], y0,  xp[3], y1 });
				prof.cacheLookups	 .Draw( RectF{ xp[3], y0,  xp[4], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.primitives.Empty()	or
				 not prof.zTest.Empty()			or
				 not prof.tiles.Empty()			or
				 not prof.transactionElim.Empty() )
			{
				str.clear();
				if ( not prof.primitives.Empty() )
				{
					str << "Tris: "
						<< ToString(ulong(prof.primitives.LastPoint(0))) << " | "
						<< ToString(ulong(prof.primitives.LastPoint(1))) << " | "
						<< ToString(ulong(prof.primitives.LastPoint(2))) << "  ";
				}
				if ( not prof.zTest.Empty() )
				{
					str << "Z: "
						<< ToString(prof.zTest.LastPoint(0)*0.001f, 1) << " / "
						<< ToString(prof.zTest.LastPoint(1)*0.001f, 1) << " | "
						<< ToString(prof.zTest.LastPoint(2)*0.001f, 1) << " / "
						<< ToString(prof.zTest.LastPoint(3)*0.001f, 1);
				}
				if ( not str.empty() )
					ImGui::TextUnformatted( str.c_str() );

				prof.tiles			.SetName( "tiles cnt\n"s	<< ToString( prof.tiles.LastPoint(),           2 ) << " K/f" );
				prof.transactionElim.SetName( "trans elim:\n"s	<< ToString( prof.transactionElim.LastPoint(), 2 ) << " /f" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.primitives		.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.zTest			.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.tiles			.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				prof.transactionElim.Draw( RectF{ xp[3], y0,  xp[4], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.gpuCycles.Empty()		or
				 not prof.shaderCycles.Empty()	or
				 not prof.shaderJobs.Empty()	or
				 not prof.textureCycles.Empty() )
			{
				ImGui::TextUnformatted( "Shaders" );

				prof.gpuCycles		.SetName( "GPU cycles\n"s << ToString( prof.gpuCycles.LastPoint(),		2 ) << " M/f" );
				prof.textureCycles	.SetName( "tex cycles\n"s << ToString( prof.textureCycles.LastPoint(),	2 ) << " M/f" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.gpuCycles		.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.shaderCycles	.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.shaderJobs		.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				prof.textureCycles  .Draw( RectF{ xp[3], y0,  xp[4], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.loadStoreCycles.Empty()	or
				 not prof.shaderALU.Empty()			or
				 not prof.shaderInterp.Empty() )
			{
				prof.loadStoreCycles.SetName( "load/store cycles\n"s << ToString( prof.loadStoreCycles.LastPoint(), 2 ) << " M/f" );
				prof.shaderALU		.SetName( "ALU cycles\n"s		 << ToString( prof.shaderALU.LastPoint(),		2 ) << " M/f" );
				prof.shaderInterp	.SetName( "interpolator\n"s		 << ToString( prof.shaderInterp.LastPoint(),	2 ) << " M/f" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.loadStoreCycles.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.shaderALU		.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.shaderInterp	.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateArmCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateArmCountersImGui (double invFC)
	{
		using ECounter = ArmProfiler::ECounter;

		auto& prof = _armProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		prof.profiler.Sample( OUT prof.counters );
		if ( prof.counters.empty() )
			return;

		const auto	AddPoint = [cnt = &prof.counters] (ECounter type, INOUT ImLineGraph &graph, double scale)
		{{
			if ( auto it = cnt->find( type ); it != cnt->end())		graph.AddAndUpdateRange({ float(it->second * scale) });
		}};

		const auto	AddPoint2 = [cnt = &prof.counters] (ECounter type0, ECounter type1, INOUT ImLineGraph &graph, double scale)
		{{
			double	value0 = 0.0,	value1 = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(value0), float(value1) });
		}};

		const auto	AddPoint3 = [cnt = &prof.counters] (ECounter type0, ECounter type1, ECounter type2, INOUT ImLineGraph &graph, double scale)
		{{
			double	value0 = 0.0,	value1 = 0.0,	value2 = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(value0), float(value1), float(value2) });
		}};

		const auto	AddPoint4 = [cnt = &prof.counters] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, INOUT ImLineGraph &graph, double scale)
		{{
			double	value0 = 0.0,	value1 = 0.0,	value2 = 0.0,	value3 = 0.0;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type3 ); it != cnt->end())	{ value3 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ float(value0), float(value1), float(value2), float(value3) });
		}};

		AddPoint2( ECounter::GPU_ExternalMemoryReadStalls,		ECounter::GPU_ExternalMemoryWriteStalls,	INOUT prof.globalMemStalls,		invFC );
		AddPoint2( ECounter::GPU_ExternalMemoryReadBytes,		ECounter::GPU_ExternalMemoryWriteBytes,		INOUT prof.globalMemTraffic,	invFC / 1024.0 );
		AddPoint2( ECounter::GPU_ExternalMemoryReadAccesses,	ECounter::GPU_ExternalMemoryWriteAccesses,	INOUT prof.globalMemAccess,		invFC );
		AddPoint2( ECounter::GPU_CacheReadLookups,				ECounter::GPU_CacheWriteLookups,			INOUT prof.cacheLookups,		invFC );

		AddPoint3( ECounter::GPU_VisiblePrimitives,	ECounter::GPU_CulledPrimitives, ECounter::GPU_InputPrimitives,										INOUT prof.primitives,		invFC	);
		AddPoint4( ECounter::GPU_VertexCycles,		ECounter::GPU_FragmentCycles,	ECounter::GPU_ComputeCycles,	ECounter::GPU_VertexComputeCycles,	INOUT prof.shaderCycles,	invFC	);
		AddPoint4( ECounter::GPU_VertexJobs,		ECounter::GPU_FragmentJobs,		ECounter::GPU_ComputeJobs,		ECounter::GPU_VertexComputeJobs,	INOUT prof.shaderJobs,		invFC	);
		AddPoint4( ECounter::GPU_EarlyZTests,		ECounter::GPU_EarlyZKilled,		ECounter::GPU_LateZTests,		ECounter::GPU_LateZKilled,			INOUT prof.zTest,			invFC	);

		AddPoint(  ECounter::GPU_Cycles,					INOUT prof.gpuCycles,			invFC * 1.0e-6 );
		AddPoint(  ECounter::GPU_ShaderTextureCycles,		INOUT prof.textureCycles,		invFC * 1.0e-6 );
		AddPoint(  ECounter::GPU_ShaderLoadStoreCycles,		INOUT prof.loadStoreCycles,		invFC * 1.0e-6 );
		AddPoint(  ECounter::GPU_Tiles,						INOUT prof.tiles,				invFC * 1.0e-3 );

		AddPoint(  ECounter::GPU_TransactionEliminations,	INOUT prof.transactionElim,		invFC );
		AddPoint(  ECounter::GPU_ShaderArithmeticCycles,	INOUT prof.shaderALU,			invFC * 1.0e-6 );
		AddPoint(  ECounter::GPU_ShaderInterpolatorCycles,	INOUT prof.shaderInterp,		invFC * 1.0e-6 );

		// processor
		AddPoint(  ECounter::CPU_Cycles,					INOUT prof.cpuCycles,			invFC * 1.0e-6 );
		AddPoint(  ECounter::CPU_CacheMisses,				INOUT prof.cacheMisses,			invFC * 1.0e-6 );
		AddPoint(  ECounter::CPU_BranchMisses,				INOUT prof.branchMisses,		invFC * 1.0e-6 );
		AddPoint(  ECounter::CPU_CacheReferences,			INOUT prof.cacheRefs,			invFC * 1.0e-6 );
		AddPoint(  ECounter::CPU_BranchInstructions,		INOUT prof.branchInst,			invFC * 1.0e-6 );
	}

/*
=================================================
	_InitArmCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitArmCountersImGui (const ImLineGraph::ColorStyle &style2, const ImLineGraph::ColorStyle &style3)
	{
		const uint	capacity2	= 50;
		auto&		prof		= _armProf;

		{
			auto&	graph = prof.globalMemStalls;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Stalls" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.globalMemTraffic;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Traffic" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.globalMemAccess;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Access" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.primitives;
			graph.SetCapacity( capacity2, 3 );
			graph.SetName( "Tris" );
			graph.SetLabel( "Culled",  0 );
			graph.SetLabel( "Visible", 1 );
			graph.SetLabel( "Input",   2 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.zTest;
			graph.SetCapacity( capacity2, 4 );
			graph.SetName( "Z" );
			graph.SetLabel( "E-test", 0 );
			graph.SetLabel( "E-kill", 1 );
			graph.SetLabel( "L-test", 2 );
			graph.SetLabel( "L-kill", 3 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.cacheLookups;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Cache lookups" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.shaderCycles;
			graph.SetCapacity( capacity2, 4 );
			graph.SetName( "Cycles" );
			graph.SetLabel( "vert", 0 );
			graph.SetLabel( "frag", 1 );
			graph.SetLabel( "comp", 2 );
			graph.SetLabel( "v/c",  3 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.shaderJobs;
			graph.SetCapacity( capacity2, 4 );
			graph.SetName( "Jobs" );
			graph.SetLabel( "vert", 0 );
			graph.SetLabel( "frag", 1 );
			graph.SetLabel( "comp", 2 );
			graph.SetLabel( "v/c",  3 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.gpuCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.textureCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.tiles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.loadStoreCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.transactionElim;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.shaderALU;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.shaderInterp;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}

		// processor
		{
			auto&	graph = prof.cpuCycles;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.cacheMisses;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.branchMisses;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.cacheRefs;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.branchInst;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}
	}
#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	_DrawProfilerPowerVRImGui
=================================================
*/
	void  HwpcProfiler::_DrawProfilerPowerVRImGui ()
	{
		auto&	prof = _pvrProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		String			str;
		const float		graph_height	= 150.f;
		const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
		const float		x_offset		= wnd_pos_x + padding[0];
		const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "PowerVR GPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const float		xp [6] = {	x_offset, x_offset + wnd_size.x*0.25f, x_offset + wnd_size.x*0.5f, x_offset + wnd_size.x*0.75f, x_offset + wnd_size.x };
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });

			if ( not prof.memoryTraffic.Empty()	or
				 not prof.memoryBusLoad.Empty()	or
				 not prof.texReadStall.Empty()	)
			{
				if ( not prof.memoryTraffic.Empty()	)
				{
					str.clear();
					str	<< "Traffic: "
						<< ToString(Bytes{ulong(prof.memoryTraffic.LastPoint(0)) << 10}) << "/f | "
						<< ToString(Bytes{ulong(prof.memoryTraffic.LastPoint(1)) << 10}) << "/f";
					ImGui::TextUnformatted( str.c_str() );
				}
				prof.memoryBusLoad	.SetName( "mem bus:\n"s			<< ToString( prof.memoryBusLoad.LastPoint(),	2 ) << " %" );
				prof.texReadStall	.SetName( "tex read stalls\n"s	<< ToString( prof.texReadStall.LastPoint(),		2 ) << " %" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.memoryTraffic	.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.memoryBusLoad	.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.texReadStall	.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.gpuTime.Empty()		or
				 not prof.gpuActive.Empty()		or
				 not prof.gpuClockSpeed.Empty()	or
				 not prof.gpuCycles.Empty()		)
			{
				if ( not prof.gpuCycles.Empty() )
				{
					str.clear();
					str << "Cycles C:"
						<< ToString( prof.gpuCycles.LastPoint(0), 1 ) << " | V:"
						<< ToString( prof.gpuCycles.LastPoint(1), 1 ) << " | F:"
						<< ToString( prof.gpuCycles.LastPoint(2), 1 );
					ImGui::TextUnformatted( str.c_str() );
				}
				prof.gpuTime		.SetName( "Time: "s		 << ToString( Max( prof.gpuTime.LastPoint(0), prof.gpuTime.LastPoint(1), prof.gpuTime.LastPoint(2) ), 2 ) << " ms" );
				prof.gpuClockSpeed	.SetName( "GPU clock\n"s << ToString( prof.gpuClockSpeed.LastPoint(),	2 ) << " Mhz" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.gpuCycles		.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.gpuTime		.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.gpuActive		.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				prof.gpuClockSpeed	.Draw( RectF{ xp[3], y0,  xp[4], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.tilerTriangleRatio.Empty()	or
				 not prof.tilerTrianglesIO.Empty()		or
				 not prof.zTest.Empty()					)
			{
				prof.tilerTriangleRatio	.SetName( "tile tris rate\n"s	<< ToString( prof.tilerTriangleRatio.LastPoint(),	2 ) << " K" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.tilerTriangleRatio	.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.tilerTrianglesIO	.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.zTest				.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.shaderLoad.Empty()		or
				 not prof.shaderLoad2.Empty()		or
				 not prof.registerOverload.Empty()	)
			{
				prof.shaderLoad.SetName( "shader load:\n"s << ToString( prof.shaderLoad.LastPoint(), 2 ) << " %" );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.shaderLoad			.Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.shaderLoad2		.Draw( RectF{ xp[1], y0,  xp[2], y1 });
				prof.registerOverload	.Draw( RectF{ xp[2], y0,  xp[3], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdatePowerVRCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdatePowerVRCountersImGui (const float invFC)
	{
		using ECounter = PowerVRProfiler::ECounter;

		auto& prof = _pvrProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		prof.profiler.Sample( OUT prof.counters );
		if ( prof.counters.empty() )
			return;

		const auto	AddPoint = [cnt = &prof.counters] (ECounter type, INOUT ImLineGraph &graph, float scale)
		{{
			if ( auto it = cnt->find( type ); it != cnt->end())		graph.AddAndUpdateRange({ it->second * scale });
		}};

		const auto	AddPoint2 = [cnt = &prof.counters] (ECounter type0, ECounter type1, INOUT ImLineGraph &graph, float scale)
		{{
			float	value0 = 0.f,	value1 = 0.f;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ value0, value1 });
		}};

		const auto	AddPoint3 = [cnt = &prof.counters] (ECounter type0, ECounter type1, ECounter type2, INOUT ImLineGraph &graph, float scale)
		{{
			float	value0 = 0.f,	value1 = 0.f,	value2 = 0.f;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ value0, value1, value2 });
		}};

		const auto	AddPoint4 = [cnt = &prof.counters] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, INOUT ImLineGraph &graph, float scale)
		{{
			float	value0 = 0.f,	value1 = 0.f,	value2 = 0.f,	value3 = 0.f;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type3 ); it != cnt->end())	{ value3 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ value0, value1, value2, value3 });
		}};

		AddPoint(  ECounter::GPU_MemoryInterfaceLoad,		INOUT prof.memoryBusLoad,		1.f );		// %
		AddPoint(  ECounter::GPU_ClockSpeed,				INOUT prof.gpuClockSpeed,		1.0e-6f );
		AddPoint(  ECounter::Tiler_TriangleRatio,			INOUT prof.tilerTriangleRatio,	1.0e-3f );
		AddPoint(  ECounter::Texture_ReadStall,				INOUT prof.texReadStall,		1.f );		// %
		AddPoint(  ECounter::Shader_ShaderProcessingLoad,	INOUT prof.shaderLoad,			invFC );

		AddPoint2( ECounter::GPU_MemoryRead,				ECounter::GPU_MemoryWrite,					INOUT prof.memoryTraffic,		invFC / 1024.f );
		AddPoint2( ECounter::VertexShader_RegisterOverload,	ECounter::PixelShader_RegisterOverload,		INOUT prof.registerOverload,	invFC );
		AddPoint2( ECounter::Tiler_TrianglesInputPerFrame,	ECounter::Tiler_TrianglesOutputPerFrame,	INOUT prof.tilerTrianglesIO,	1.0e-3f );
		AddPoint2( ECounter::Renderer_HSR_Efficiency,		ECounter::Renderer_ISP_PixelLoad,			INOUT prof.zTest,				1.f );	// %

		AddPoint3( ECounter::RendererTimePerFrame,			ECounter::GeometryTimePerFrame,			ECounter::TDM_TimePerFrame,				INOUT prof.gpuTime,		1.0e+3f );
		AddPoint3( ECounter::Shader_CyclesPerComputeKernel,	ECounter::Shader_CyclesPerVertex,		ECounter::Shader_CyclesPerPixel,		INOUT prof.gpuCycles,	invFC );
		AddPoint3( ECounter::ComputeShader_ProcessingLoad,	ECounter::VertexShader_ProcessingLoad,	ECounter::PixelShader_ProcessingLoad,	INOUT prof.shaderLoad2,	invFC );

		AddPoint4( ECounter::RendererActive,	ECounter::GeometryActive,	ECounter::TDM_Active,	ECounter::SPM_Active,	INOUT prof.gpuActive,	1.f );	// %
	}

/*
=================================================
	_InitPowerVRCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitPowerVRCountersImGui (const ImLineGraph::ColorStyle &style2, const ImLineGraph::ColorStyle &style3)
	{
		const uint	capacity2	= 50;
		auto&		prof		= _pvrProf;

		{
			auto&	graph = prof.memoryTraffic;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Mem" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.memoryBusLoad;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.gpuTime;
			graph.SetCapacity( capacity2, 3 );
			graph.SetName( "Time" );
			graph.SetLabel( "gfx",  0 );
			graph.SetLabel( "geom", 1 );
			graph.SetLabel( "tdm",  2 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.gpuActive;
			graph.SetCapacity( capacity2, 4 );
			graph.SetName( "Active" );
			graph.SetLabel( "gfx",  0 );
			graph.SetLabel( "geom", 1 );
			graph.SetLabel( "tdm",  2 );
			graph.SetLabel( "spm",  3 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.gpuClockSpeed;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.tilerTriangleRatio;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.tilerTrianglesIO;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Tile tris" );
			graph.SetLabel( "in",  0 );
			graph.SetLabel( "out", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.shaderLoad2;
			graph.SetCapacity( capacity2, 3 );
			graph.SetName( "Load" );
			graph.SetLabel( "Comp",	0 );
			graph.SetLabel( "Vert",	1 );
			graph.SetLabel( "Frag",	2 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.texReadStall;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.gpuCycles;
			graph.SetCapacity( capacity2, 3 );
			graph.SetName( "Cycles" );
			graph.SetLabel( "Comp",	0 );
			graph.SetLabel( "Vert",	1 );
			graph.SetLabel( "Frag",	2 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.shaderLoad;
			graph.SetCapacity( capacity2 );
			graph.SetColor( style3 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.registerOverload;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Reg overload" );
			graph.SetLabel( "Vert", 0 );
			graph.SetLabel( "Frag", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.zTest;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Z" );
			graph.SetLabel( "hsr", 0 );
			graph.SetLabel( "isp", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}
	}
#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	_DrawProfilerAdrenoImGui
=================================================
*/
	void  HwpcProfiler::_DrawProfilerAdrenoImGui ()
	{
		auto&	prof = _adrenoProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		String			str;
		const float		graph_height	= 150.f;
		const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
		const float		x_offset		= wnd_pos_x + padding[0];
		const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "Adreno profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const float		xp [6] = {	x_offset, x_offset + wnd_size.x*0.25f, x_offset + wnd_size.x*0.5f, x_offset + wnd_size.x*0.75f, x_offset + wnd_size.x };
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });

			if ( not prof.chipMemTraffic.Empty() )
			{
				str.clear();
				str	<< "OnChip memory";
				if ( not prof.chipMemTraffic.Empty() )
				{
					str	<< ", Traffic: "
						<< ToString(Bytes{ulong(prof.chipMemTraffic.LastPoint(0)) << 10}) << "/f | "
						<< ToString(Bytes{ulong(prof.chipMemTraffic.LastPoint(1)) << 10}) << "/f";
				}
				ImGui::TextUnformatted( str.c_str() );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.chipMemTraffic	 .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}

			if ( not prof.lrzTraffic.Empty()	or
				 not prof.lrzPrim.Empty()		)
			{
				str.clear();
				str	<< "LRZ";
				ImGui::TextUnformatted( str.c_str() );

				const float	y0	= ImGui::GetCursorScreenPos().y;
				const float	y1	= y0 + graph_height;
				prof.lrzTraffic	 .Draw( RectF{ xp[0], y0,  xp[1], y1 });
				prof.lrzPrim	 .Draw( RectF{ xp[1], y0,  xp[2], y1 });
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, y1 + padding[1] });
			}
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateAdrenoCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateAdrenoCountersImGui (float invFC)
	{
		using ECounter = AdrenoProfiler::ECounter;

		auto& prof = _adrenoProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		prof.profiler.Sample( OUT prof.counters );
		if ( prof.counters.empty() )
			return;

		const auto	AddPoint = [cnt = &prof.counters] (ECounter type, INOUT ImLineGraph &graph, float scale)
		{{
			if ( auto it = cnt->find( type ); it != cnt->end())		graph.AddAndUpdateRange({ it->second * scale });
		}};

		const auto	AddPoint2 = [cnt = &prof.counters] (ECounter type0, ECounter type1, INOUT ImLineGraph &graph, float scale)
		{{
			float	value0 = 0.f,	value1 = 0.f;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ value0, value1 });
		}};

		const auto	AddPoint3 = [cnt = &prof.counters] (ECounter type0, ECounter type1, ECounter type2, INOUT ImLineGraph &graph, float scale)
		{{
			float	value0 = 0.f,	value1 = 0.f,	value2 = 0.f;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ value0, value1, value2 });
		}};

		const auto	AddPoint4 = [cnt = &prof.counters] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, INOUT ImLineGraph &graph, float scale)
		{{
			float	value0 = 0.f,	value1 = 0.f,	value2 = 0.f,	value3 = 0.f;
			bool	exists = false;

			if ( auto it = cnt->find( type0 ); it != cnt->end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type1 ); it != cnt->end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type2 ); it != cnt->end())	{ value2 = it->second * scale;  exists = true; }
			if ( auto it = cnt->find( type3 ); it != cnt->end())	{ value3 = it->second * scale;  exists = true; }

			if ( exists )	graph.AddAndUpdateRange({ value0, value1, value2, value3 });
		}};

		AddPoint2( ECounter::GMem_Read,		ECounter::GMem_Write,		INOUT prof.chipMemTraffic,	invFC / 1024.f );
		AddPoint2( ECounter::LRZ_Read,		ECounter::LRZ_Write,		INOUT prof.lrzTraffic,		invFC / 1024.f );

		AddPoint3( ECounter::LRZ_PrimKilledByMaskGen,	ECounter::LRZ_PrimKilledByLRZ,	ECounter::LRZ_PrimPassed,	INOUT prof.lrzPrim,		invFC / 1024.f );
	}

/*
=================================================
	_InitAdrenoCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitAdrenoCountersImGui (const ImLineGraph::ColorStyle &style2, const ImLineGraph::ColorStyle &style3)
	{
		const uint	capacity2	= 50;
		auto&		prof		= _adrenoProf;

		{
			auto&	graph = prof.chipMemTraffic;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Traffic" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.lrzTraffic;
			graph.SetCapacity( capacity2, 2 );
			graph.SetName( "Traffic" );
			graph.SetLabel( "read",  0 );
			graph.SetLabel( "write", 1 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}{
			auto&	graph = prof.lrzPrim;
			graph.SetCapacity( capacity2, 3 );
			graph.SetName( "Prims" );
			graph.SetLabel( "maskgen",  0 );
			graph.SetLabel( "kill",		1 );
			graph.SetLabel( "pass",		2 );
			graph.SetColor( style2 );
			graph.SetRange( 0.f, 1.f );
		}
	}

#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	_DrawCpuUsageImGui
=================================================
*/
	void  HwpcProfiler::_DrawCpuUsageImGui ()
	{
		if ( not _cpuUsage.enabled )
			return;

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
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, left_top.y - (left_top.x == x_offset ? graph_height : 0.f) + padding[1] });
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateCpuUsageImGui
=================================================
*/
	void  HwpcProfiler::_UpdateCpuUsageImGui ()
	{
		if ( not _cpuUsage.enabled )
			return;

		StaticArray< float, 64 >	user, kernel;

		CpuPerformance::GetUsage( OUT user.data(), OUT kernel.data(), uint(user.size()) );

		const auto&		cpu_info = CpuArchInfo::Get();
		for (auto& core : cpu_info.cpu.coreTypes)
		{
			for (uint core_id : BitIndexIterate( core.logicalBits.to_ullong() ))
			{
				auto&	graph = _cpuUsage.coreUsage[core_id];

				graph.Add({ user[core_id] + kernel[core_id], kernel[core_id] });
			}
		}
	}
#endif
//-----------------------------------------------------------------------------


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
