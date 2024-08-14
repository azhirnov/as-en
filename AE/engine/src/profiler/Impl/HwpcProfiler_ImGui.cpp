// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "base/Math/Random.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Platforms/CPUInfo.h"
# include "profiler/Impl/HwpcProfiler.h"
# include "graphics/GraphicsImpl.h"
# include "graphics_hl/ImGui/ImGuiRenderer.h"

namespace AE::Profiler
{
namespace
{
	static const float	c_GraphHeight	= 150.f;
	static const float2	c_GraphPadding	{ 2.0f, 8.f };


	// ARM CPU //
	static constexpr GraphName	ARM_CpuCycles				{"CpuCycles"};
	static constexpr GraphName	ARM_CacheMisses				{"CacheMisses"};
	static constexpr GraphName	ARM_BranchMisses			{"BranchMisses"};
	static constexpr GraphName	ARM_CacheRefs				{"CacheRefs"};
	static constexpr GraphName	ARM_BranchInst				{"BranchInst"};


	// PowerVR GPU //
	static constexpr GraphName	PVR_MemoryTraffic			{"MemoryTraffic"};
	static constexpr GraphName	PVR_MemoryBusLoad			{"MemoryBusLoad"};
	static constexpr GraphName	PVR_TexReadStall			{"TexReadStall"};

	static constexpr GraphName	PVR_GpuCycles				{"GpuCycles"};
	static constexpr GraphName	PVR_GpuTime					{"GpuTime"};
	static constexpr GraphName	PVR_GpuActive				{"GpuActive"};
	static constexpr GraphName	PVR_GpuClockSpeed			{"GpuClockSpeed"};

	static constexpr GraphName	PVR_TilerTriangleRatio		{"TilerTriangleRatio"};
	static constexpr GraphName	PVR_TilerTrianglesIO		{"TilerTrianglesIO"};
	static constexpr GraphName	PVR_ZTest					{"ZTest"};

	static constexpr GraphName	PVR_ShaderLoad				{"ShaderLoad"};
	static constexpr GraphName	PVR_ShaderLoad2				{"ShaderLoad2"};
	static constexpr GraphName	PVR_RegisterOverload		{"RegisterOverload"};


	// Adreno GPU //
	static constexpr GraphName	Adreno_LrzTraffic			{"LrzTraffic"};
	static constexpr GraphName	Adreno_LrzPrim				{"LrzPrim"};
	static constexpr GraphName	Adreno_LrzTileKilled		{"LrzTileKilled"};
	static constexpr GraphName	Adreno_LrzTotalPixel		{"LrzTotalPixel"};

	static constexpr GraphName	Adreno_RasTiles				{"RasTiles"};
	static constexpr GraphName	Adreno_RasFullyCoveredTiles	{"RasFullyCoveredTiles"};

	static constexpr GraphName	Adreno_RbZTraffic			{"RbZTraffic"};
	static constexpr GraphName	Adreno_RbCTraffic			{"RbCTraffic"};
	static constexpr GraphName	Adreno_RbZSPass				{"RbZSPass"};
	static constexpr GraphName	Adreno_Rb2DActive			{"Rb2DActive"};

	static constexpr GraphName	Adreno_CcuDCBlocks			{"CcuDCBlocks"};
	static constexpr GraphName	Adreno_CcuPartBlockRd		{"CcuPartBlockRd"};
	static constexpr GraphName	Adreno_CcuGMem				{"CcuGMem"};
	static constexpr GraphName	Adreno_Ccu2DPix				{"Ccu2DPix"};


	// Mali GPU //
	static constexpr GraphName	Mali_ExtMemTraffic			{"ExtMemTraffic"};
	static constexpr GraphName	Mali_ExtMemStalls			{"ExtMemStalls"};
	static constexpr GraphName	Mali_ExtReadLatency1		{"ExtReadLatency1"};
	static constexpr GraphName	Mali_ExtReadLatency2		{"ExtReadLatency2"};
	static constexpr GraphName	Mali_ExtOutstandingReads	{"ExtOutstandingReads"};
	static constexpr GraphName	Mali_ExtOutstandingWrites	{"ExtOutstandingWrites"};
	static constexpr GraphName	Mali_ExtReads				{"ExtReads"};

	static constexpr GraphName	Mali_CacheMiss				{"CacheMiss"};
	static constexpr GraphName	Mali_LSTileWrite			{"LS & Tile write"};
	static constexpr GraphName	Mali_CacheReads				{"CacheReads"};
	static constexpr GraphName	Mali_CacheFlush				{"CacheFlush"};
	static constexpr GraphName	Mali_FragTileKillRate		{"FragTileKillRate"};

	static constexpr GraphName	Mali_Primitives				{"Primitives"};
	static constexpr GraphName	Mali_PosVarShadingReq		{"PosVarShadingRequests"};
	static constexpr GraphName	Mali_PosVarShadingThread	{"PosVarShadingThreads"};
	static constexpr GraphName	Mali_PosVarShadingPerPrim	{"PosVarShadingPerInputPrimitive"};

	static constexpr GraphName	Mali_ZSTest					{"ZSTest"};
	static constexpr GraphName	Mali_ZSTest2				{"ZSTest2"};
	static constexpr GraphName	Mali_FragOpaqueRate			{"FragOpaqueRate"};
	static constexpr GraphName	Mali_FragOverdraw			{"FragOverdraw"};

	static constexpr GraphName	Mali_CoreUtil				{"Core Util"};
	static constexpr GraphName	Mali_VarUtil				{"Varying Util"};
	static constexpr GraphName	Mali_TexUtil				{"TextureUnit Util"};
	static constexpr GraphName	Mali_TexCacheUtil			{"TextureCache Util"};
	static constexpr GraphName	Mali_FragNonFragUtil		{"Frag & NonFrag Util"};
	static constexpr GraphName	Mali_WrapCount				{"WrapCount"};
	static constexpr GraphName	Mali_LSUtil					{"LoadStore Util"};
	static constexpr GraphName	Mali_RTUtil					{"RayTracing Util"};
	static constexpr GraphName	Mali_TilerUtil				{"Tiler Util"};
	static constexpr GraphName	Mali_FPKUtil				{"FPK Util"};
	static constexpr GraphName	Mali_QueueUtil				{"QueueUtil"};

	static constexpr GraphName	Mali_ALUUtil				{"ALU Util"};
	static constexpr GraphName	Mali_ArithPipeUtil			{"ArithPipeUtil"};
	static constexpr GraphName	Mali_NarrowArithUtil		{"Narrow Arithmetic Util"};
	static constexpr GraphName	Mali_WarpDivRate			{"WarpDivergence"};
	static constexpr GraphName	Mali_AllRegWarpRate			{"AllRegistersWarpRate"};
	static constexpr GraphName	Mali_FullQuadWarpRate		{"FullQuadWarpRate"};
	static constexpr GraphName	Mali_FragRastPartRate		{"FragRastPartRate"};


	// NVidia GPU //
	static constexpr GraphName	NV_MemoryUtil				{"MemUtil"};
	static constexpr GraphName	NV_MemoryClock				{"MemClock"};
	static constexpr GraphName	NV_MemUsed					{"MemUsed"};
	static constexpr GraphName	NV_MemUsedMb				{"MemUsedMb"};

	static constexpr GraphName	NV_GpuUtil					{"GpuUtil"};
	static constexpr GraphName	NV_GraphicsClock			{"GraphicsClock"};
	static constexpr GraphName	NV_SMClock					{"SMClock"};
	static constexpr GraphName	NV_VideoClock				{"VideoClock"};

	static constexpr GraphName	NV_GpuTemp					{"GpuTemp"};
	static constexpr GraphName	NV_PowerUsage				{"PowerUsage"};
	static constexpr GraphName	NV_PerfState				{"PerfState"};
	static constexpr GraphName	NV_FanSpeed					{"FanSpeed"};
}
//-----------------------------------------------------------------------------


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

			_DrawProfilerArmImGui();
			_DrawProfilerMaliImGui();
			_DrawProfilerAdrenoImGui();
			_DrawProfilerNVidiaImGui();
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
	//	const RGBA8u	text_col {255, 255, 255, 255};
		const RGBA8u	text_col {200, 200, 200, 255};

		ImLineGraph::ColorStyle		style4;
		style4.lines[0]		= RGBA8u{180,  20,  20, 255};
		style4.lines[1]		= RGBA8u{ 20, 170,  20, 255};
		style4.lines[2]		= RGBA8u{ 70,  70, 255, 255};
		style4.lines[3]		= RGBA8u{170, 170,  50, 255};
		style4.background[0]= RGBA8u{  0,   0,  40, 255};
		style4.background[1]= RGBA8u{ 30,  30,   0, 255};
		style4.background[2]= RGBA8u{ 30,   0,   0, 255};
		style4.border		= RGBA8u{200, 200, 255, 255};
		style4.text			= text_col;
		style4.minMaxValue	= text_col;
		style4.mode			= ImLineGraph::EMode::Line;

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
					_cpuUsage.coreUsage[core_id] = MakeUnique<ImLineGraph>();
					auto&	graph = *_cpuUsage.coreUsage[core_id];
					graph.SetCapacity( capacity1, 2 );
					graph.SetColor( style4 );
					graph.SetName( ToString(core_id) );
					graph.SetLabel( "total",  0 );
					graph.SetLabel( "kernel", 1 );
					graph.SetSuffix( "%" );
					graph.SetRange( 0.f, 100.f );
				}

				_cpuUsage.corePerLine = Max( _cpuUsage.corePerLine, core.LogicalCount() );
			}

			if ( cpu_info.cpu.coreTypes.size() == 1 )
				_cpuUsage.corePerLine = Max( 1u, uint( Sqrt( float(cpu_info.cpu.logicalCoreCount) ) + 0.5f ));
		}

		style4.mode			= ImLineGraph::EMode::Line_Adaptive;

		ImLineGraph::ColorStyle		style1 = style4;
		style1.lines[0]		= RGBA8u{ 20, 170,  20, 255};
		style1.border		= RGBA8u{200, 200, 255, 255};

		_InitArmCountersImGui( style4, style1 );
		_InitMaliCountersImGui( style4, style1 );
		_InitNVidiaCountersImGui( style4, style1 );
		_InitAdrenoCountersImGui( style4, style1 );
		_InitPowerVRCountersImGui( style4, style1 );
	}
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	_DrawProfilerArmImGui
=================================================
*/
	void  HwpcProfiler::_DrawProfilerArmImGui ()
	{
		auto&	prof = _armProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		const float		wnd_pos_x	= ImGui::GetCursorScreenPos().x;
		const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "ARM CPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			prof.graphTable.Draw( wnd_size.x, c_GraphHeight, c_GraphPadding, ImGui::IsItemHovered() );
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateArmCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateArmCountersImGui (const double invFC)
	{
		using ECounter = ArmProfiler::ECounter;

		auto&	prof	= _armProf;
		auto&	cnt		= prof.counters;

		if ( cnt.empty() )
			return;

		const auto	AddPoint = [&] (ECounter type, GraphName::Ref graphName, double scale = 0.0)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type ));

			scale = (scale == 0.0 ? invFC : scale);
			if ( auto it = cnt.find( type ); it != cnt.end())		graph->AddNonScaled( List{ it->second * scale });
		}};

		AddPoint(  ECounter::Cycles,				ARM_CpuCycles		);
		AddPoint(  ECounter::CacheMisses,			ARM_CacheMisses		);
		AddPoint(  ECounter::BranchMisses,			ARM_BranchMisses	);
		AddPoint(  ECounter::CacheReferences,		ARM_CacheRefs		);
		AddPoint(  ECounter::BranchInstructions,	ARM_BranchInst		);
	}

/*
=================================================
	_InitArmCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitArmCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &style1)
	{
		const uint	capacity	= 50;
		auto&		prof		= _armProf;

		{
			auto&	graph = prof.graphTable.Add( Default, ARM_CpuCycles );
			graph.SetCapacity( capacity );
			graph.SetName( "cycles" );
			graph.SetColor( style1 );
		}{
			auto&	graph = prof.graphTable.Add( Default, ARM_CacheMisses );
			graph.SetCapacity( capacity );
			graph.SetName( "cache misses" );
			graph.SetColor( style1 );
		}{
			auto&	graph = prof.graphTable.Add( Default, ARM_BranchMisses );
			graph.SetCapacity( capacity );
			graph.SetName( "branch misses" );
			graph.SetColor( style1 );
		}{
			auto&	graph = prof.graphTable.Add( Default, ARM_CacheRefs );
			graph.SetCapacity( capacity );
			graph.SetName( "cache refs" );
			graph.SetColor( style1 );
		}{
			auto&	graph = prof.graphTable.Add( Default, ARM_BranchInst );
			graph.SetCapacity( capacity );
			graph.SetName( "branch inst" );
			graph.SetColor( style1 );
		}
	}
#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	_DrawProfilerMaliImGui
=================================================
*/
	void  HwpcProfiler::_DrawProfilerMaliImGui ()
	{
		auto&	prof = _maliProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		const float		wnd_pos_x	= ImGui::GetCursorScreenPos().x;
		const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "Mali GPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			prof.graphTable.Draw( wnd_size.x, c_GraphHeight, c_GraphPadding, ImGui::IsItemHovered() );
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateMaliCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateMaliCountersImGui (const double invFC)
	{
		using ECounter = MaliProfiler::ECounter;

		auto&	prof	= _maliProf;
		auto	cnt		= prof.counters;

		if ( cnt.empty() )
			return;

		const auto	AddPoint1 = [&] (ECounter type, GraphName::Ref graphName, double scale = 0.0)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type ));

			scale = (scale == 0.0 ? invFC : scale);
			if ( auto it = cnt.find( type ); it != cnt.end())		graph->AddNonScaled( List{ it->second * scale });
		}};

		const auto	AddPoint2 = [&] (ECounter type0, ECounter type1, GraphName::Ref graphName, double scale = 0.0)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ));

					scale	= (scale == 0.0 ? invFC : scale);
			double	value0	= 0.0,	value1 = 0.0;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1 });
		}};

		const auto	AddPoint3 = [&] (ECounter type0, ECounter type1, ECounter type2, GraphName::Ref graphName, double scale = 0.0)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ) and
					prof.requiredCounters.contains( type2 ));

					scale	= (scale == 0.0 ? invFC : scale);
			double	value0	= 0.0,	value1 = 0.0,	value2 = 0.0;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type2 ); it != cnt.end())	{ value2 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1, value2 });
		}};

		const auto	AddPoint4 = [&] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, GraphName::Ref graphName, double scale = 0.0)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ) and
					prof.requiredCounters.contains( type2 ) and
					prof.requiredCounters.contains( type3 ));

					scale	= (scale == 0.0 ? invFC : scale);
			double	value0	= 0.0,	value1 = 0.0,	value2 = 0.0,	value3 = 0.0;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type2 ); it != cnt.end())	{ value2 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type3 ); it != cnt.end())	{ value3 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1, value2, value3 });
		}};

		AddPoint1( ECounter::CoreUtil,				Mali_CoreUtil,			1.0 );
		AddPoint1( ECounter::ALUUtil,				Mali_ALUUtil,			1.0 );
		AddPoint1( ECounter::VarUtil,				Mali_VarUtil,			1.0 );
		AddPoint1( ECounter::TexUtil,				Mali_TexUtil,			1.0 );
		AddPoint1( ECounter::TexCacheUtil,			Mali_TexCacheUtil,		1.0 );
		AddPoint1( ECounter::LSUtil,				Mali_LSUtil,			1.0	);
		AddPoint1( ECounter::RTUUtil,				Mali_RTUtil,			1.0	);
		AddPoint1( ECounter::EngNarrowInstrRate,	Mali_NarrowArithUtil,	1.0 );
		AddPoint1( ECounter::EngDivergedInstrRate,	Mali_WarpDivRate,		1.0 );
		AddPoint1( ECounter::FragOpaqueQdRate,		Mali_FragOpaqueRate,	1.0	);
		AddPoint1( ECounter::FragOverdraw,			Mali_FragOverdraw,		1.0	);
		AddPoint1( ECounter::CoreAllRegsWarpRate,	Mali_AllRegWarpRate,	1.0	);
		AddPoint1( ECounter::CoreFullQdWarpRate,	Mali_FullQuadWarpRate,	1.0 );
		AddPoint1( ECounter::FragRastPartQdRate,	Mali_FragRastPartRate,	1.0 );
		AddPoint1( ECounter::L2CacheFlushCy,		Mali_CacheFlush			);
		AddPoint1( ECounter::FragTileKillRate,		Mali_FragTileKillRate,	1.0 );
		AddPoint1( ECounter::TilerUtil,				Mali_TilerUtil,			1.0 );
		AddPoint1( ECounter::FragFPKBUtil,			Mali_FPKUtil,			1.0 );

		AddPoint2( ECounter::ExtBusRdStallRate,			ECounter::ExtBusWrStallRate,		Mali_ExtMemStalls,			1.0 );
		AddPoint2( ECounter::ExtBusRdBy,				ECounter::ExtBusWrBy,				Mali_ExtMemTraffic			);
	//	AddPoint2( ECounter::GeomPosShadTask,			ECounter::GeomVarShadTask,			Mali_PosVarShadingReq		);
		AddPoint2( ECounter::GeomPosShadThread,			ECounter::GeomVarShadThread,		Mali_PosVarShadingThread	);
		AddPoint2( ECounter::GeomVarShadThreadPerPrim,	ECounter::GeomPosShadThreadPerPrim,	Mali_PosVarShadingPerPrim	);
		AddPoint2( ECounter::NonFragUtil,				ECounter::FragUtil,					Mali_FragNonFragUtil,		1.0 );
		AddPoint2( ECounter::L2CacheRdMissRate,			ECounter::L2CacheWrMissRate,		Mali_CacheMiss,				1.0 );
		AddPoint2( ECounter::SCBusTileWrBy,				ECounter::SCBusLSWrBy,				Mali_LSTileWrite			);

		AddPoint3( ECounter::ExtBusRdLat0,		ECounter::ExtBusRdLat128,		ECounter::ExtBusRdLat192,	Mali_ExtReadLatency1	);
		AddPoint3( ECounter::ExtBusRdLat256,	ECounter::ExtBusRdLat320,		ECounter::ExtBusRdLat384,	Mali_ExtReadLatency2	);
		AddPoint3( ECounter::GeomTotalCullPrim,	ECounter::GeomVisiblePrim,		ECounter::GeomTotalPrim,	Mali_Primitives			);
		AddPoint3( ECounter::EngFMAPipeUtil,	ECounter::EngCVTPipeUtil,		ECounter::EngSFUPipeUtil,	Mali_ArithPipeUtil,		1.0 );
		AddPoint3( ECounter::FragEZSKillRate,	ECounter::FragFPKKillRate,		ECounter::FragLZSKillRate,	Mali_ZSTest2,			1.0 );
		AddPoint3( ECounter::SCBusFFEExtRdBy,	ECounter::SCBusLSExtRdBy,		ECounter::SCBusTexExtRdBy,	Mali_ExtReads			);
		AddPoint3( ECounter::SCBusFFEL2RdBy,	ECounter::SCBusLSL2RdBy,		ECounter::SCBusTexL2RdBy,	Mali_CacheReads			);
		AddPoint3( ECounter::GPUIRQUtil,		ECounter::NonFragQueueUtil,		ECounter::FragQueueUtil,	Mali_QueueUtil,			1.0 );

		AddPoint4( ECounter::ExtBusRdOTQ1,		ECounter::ExtBusRdOTQ2,		ECounter::ExtBusRdOTQ3,		ECounter::ExtBusRdOTQ4,		Mali_ExtOutstandingReads	);
		AddPoint4( ECounter::ExtBusWrOTQ1,		ECounter::ExtBusWrOTQ2,		ECounter::ExtBusWrOTQ3,		ECounter::ExtBusWrOTQ4,		Mali_ExtOutstandingWrites	);
		AddPoint4( ECounter::FragEZSTestQd,		ECounter::FragEZSKillQd,	ECounter::FragLZSTestQd,	ECounter::FragLZSKillQd,	Mali_ZSTest					);
		AddPoint4( ECounter::NonFragWarp,		ECounter::FragWarp,			ECounter::CoreFullQdWarp,	ECounter::CoreAllRegsWarp,	Mali_WrapCount				);
	}

/*
=================================================
	_InitMaliCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitMaliCountersImGui (const ImLineGraph::ColorStyle &style4, const ImLineGraph::ColorStyle &style1)
	{
		const uint	capacity	= 50;
		auto&		prof		= _maliProf;

		{
			constexpr SecName	sec {"ExternalMemory"};
			{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtMemTraffic );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "traffic" );
				graph.SetLabel( "read",  0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "B" );		// bytes
				graph.SetDescription( "Read/write external memory. DRAM access costs between 80mW and 100mW per GB/s of bandwidth used." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtMemStalls );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "stalls" );
				graph.SetLabel( "read",  0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "A high stall rate is indicative of content which is requesting more data than the downstream memory system can provide.\nTo optimize the workload, try to reduce memory bandwidth." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtReadLatency1 );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "read latency cy" );
				graph.SetLabel( "<127",	0 );
				graph.SetLabel( "<191",	1 );
				graph.SetLabel( "<255",	2 );
				graph.SetColor( style4 );
				graph.SetDescription( "This metric shows how many GPU cycles it takes to fetch data from the downstream memory system,\nwhich is either system cache or external DRAM." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtReadLatency2 );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "read latency cy" );
				graph.SetLabel( "<319",	0 );
				graph.SetLabel( "<383",	1 );
				graph.SetLabel( ">384",	2 );
				graph.SetColor( style4 );
				graph.SetDescription( "This metric shows how many GPU cycles it takes to fetch data from the downstream memory system,\nwhich is either system cache or external DRAM." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtOutstandingReads );
				graph.SetCapacity( capacity, 4 );
				graph.SetName( "outstanding reads" );
				graph.SetLabel( "<25%",		0 );
				graph.SetLabel( "<50%",		1 );
				graph.SetLabel( "<75%",		2 );
				graph.SetLabel( "<100%",	3 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtOutstandingWrites );
				graph.SetCapacity( capacity, 4 );
				graph.SetName( "outstanding writes" );
				graph.SetLabel( "<25%",		0 );
				graph.SetLabel( "<50%",		1 );
				graph.SetLabel( "<75%",		2 );
				graph.SetLabel( "<100%",	3 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ExtReads );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "read" );
				graph.SetLabel( "front", 0 );
				graph.SetLabel( "LS",	 1 );
				graph.SetLabel( "tex",	 2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "B" );		// bytes
				graph.SetDescription( "Shader core memory read traffic that misses in the GPU cache and that is fetched from the external memory system.\nFront - total number of bytes read from the external memory system by the fragment front-end unit.\nLS - total number of bytes read from the external memory system by the load/store unit.\nTex - total number of bytes read from the external memory system by the texture unit." );
			}
			prof.graphTable.SetCaption( sec, "External memory" );
		}{
			constexpr SecName	sec {"Cache"};
			{
				auto&	graph = prof.graphTable.Add( sec, Mali_CacheMiss );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "miss" );
				graph.SetLabel( "read",	 0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "L2 cache misses" );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_LSTileWrite );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "write" );
				graph.SetLabel( "tile",	 0 );
				graph.SetLabel( "LS",	 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "B" );		// bytes
				graph.SetDescription( "Tile - total number of bytes written to the L2 memory system by the tile write-back unit.\nLS - total number of bytes written to the L2 memory system by the load/store unit." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_CacheReads );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "read" );
				graph.SetLabel( "front", 0 );
				graph.SetLabel( "LS",	 1 );
				graph.SetLabel( "tex",	 2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "B" );		// bytes
				graph.SetDescription( "Shader core memory read traffic that is fetched from the GPU L2 cache.\nFront - total number of bytes read from the L2 memory system by the fragment front-end unit.\nLS -  total number of bytes read from the L2 memory system by the load/ store unit.\nTex - total number of bytes read from the L2 memory system by the texture unit." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_CacheFlush );
				graph.SetCapacity( capacity );
				graph.SetName( "flush cy" );
				graph.SetColor( style1 );
				graph.SetDescription( "L2 cache flush cycles" );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FragTileKillRate );
				graph.SetCapacity( capacity );
				graph.SetName( "tile kill" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "Defines the percentage of tiles that are killed by the transaction elimination CRC\ncheck because the content of a tile matches the content already stored in memory." );
			}
			prof.graphTable.SetCaption( sec, "Cache" );
		}{
			constexpr SecName	sec {"Geometry"};	// tiler, binning, culling
			{
				auto&	graph = prof.graphTable.Add( sec, Mali_Primitives );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "tris" );
				graph.SetLabel( "culled",  0 );
				graph.SetLabel( "visible", 1 );
				graph.SetLabel( "input",   2 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_PosVarShadingReq );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "shading req" );
				graph.SetLabel( "pos", 0 );
				graph.SetLabel( "var", 1 );
				graph.SetColor( style4 );
				graph.SetDescription( "Position shading - part of VS which contributes to vertex position.\nVarying shading - other part of VS." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_PosVarShadingThread );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "shading threads" );
				graph.SetLabel( "pos", 0 );
				graph.SetLabel( "var", 1 );
				graph.SetColor( style4 );
				graph.SetDescription( "Position shading - part of VS which contributes to vertex position.\nVarying shading - other part of VS." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_PosVarShadingPerPrim );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "shading per prim" );
				graph.SetLabel( "pos", 0 );
				graph.SetLabel( "var", 1 );
				graph.SetColor( style4 );
				graph.SetLimits( 1.5f, 2.7f );
				graph.SetDescription( "Efficient meshes with a good vertex reuse have average less than 1.5 vertices shaded per triangle,\nas vertex computation is shared by multiple primitives." );
			}
			prof.graphTable.SetCaption( sec, "Binning phase" );
		}{
			constexpr SecName	sec {"Rasterization"};
			{
				auto&	graph = prof.graphTable.Add( sec, Mali_ZSTest );
				graph.SetCapacity( capacity, 4 );
				graph.SetName( "ZS" );
				graph.SetLabel( "E-test", 0 );
				graph.SetLabel( "E-kill", 1 );
				graph.SetLabel( "L-test", 2 );
				graph.SetLabel( "L-kill", 3 );
				graph.SetColor( style4 );
				graph.SetDescription( "It is important that as many fragments as possible are early ZS (depth and stencil) tested before shading." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ZSTest2 );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "ZS kill rate" );
				graph.SetLabel( "early", 0 );
				graph.SetLabel( "fpk",	 1 );
				graph.SetLabel( "late",	 2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "It is important that as many fragments as possible are early ZS (depth and stencil) tested before shading." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FragOpaqueRate );
				graph.SetCapacity( capacity );
				graph.SetName( "opq/trp" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 10.f, 50.f );
				graph.SetDescription( "Occluding quad percentage" );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FragOverdraw );
				graph.SetCapacity( capacity );
				graph.SetName( "overdraw" );
				graph.SetColor( style1 );
				graph.SetDescription( "Number of fragments shaded per output pixel.\nGPU processing cost per pixel accumulates with the layer count. High overdraw can build up to a\nsignificant processing cost, especially when rendering to a high-resolution framebuffer. Minimize\noverdraw by rendering opaque objects front-to-back and minimizing use of blended transparent layers." );
			}
			prof.graphTable.SetCaption( sec, "Rasterization" );
		}{
			constexpr SecName	sec {"UnitUtil"};
			{
				auto&	graph = prof.graphTable.Add( sec, Mali_CoreUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "core" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Utilization of the programmable execution core,\nmonitoring any cycle where the shader core contains at least one warp." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_VarUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "var" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 25.f, 75.f );
				graph.SetDescription( "Utilization of the varying unit.\nThe most effective technique for reducing varying load is reducing the number of interpolated\nvalues read by the fragment shading. Increasing shader usage of 16-bit (mediump) input variables\nalso helps, as they can be interpolated as twice the speed of 32-bit variables." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_TexUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "tex" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Utilization of the texturing unit.\nThe most effective technique for reducing texturing unit load is reducing the number of texture\nsamples read by the fragment shader. Using simpler texture filters can reduce filtering cost. Using\n32bpp color formats, and the ASTC decode mode extensions can reduce data access cost." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_TexCacheUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "tex cache" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_LSUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "LS" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Utilization of the load/store unit.\nThe load/store unit is used for general-purpose memory accesses, and includes vertex attribute access, buffer access, work\ngroup shared memory access, and stack access. This unit also implements imageLoad/Store and\natomic access functionality." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_RTUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "ray tracing" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "The most effective technique for reducing ray tracing load is reducing the amount of geometry in\nthe acceleration structure, and ensuring that rays issued in each warp are coherent." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FragNonFragUtil );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "util" );
				graph.SetLabel( "non-frag",	 0 );
				graph.SetLabel( "frag",		 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Utilization of the shader core fragment and non-fragment path." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_TilerUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "tiler" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Defines the tiler utilization compared to the total GPU active cycles.\nNote that this metric measures the overall processing time for the tiler geometry pipeline. The\nmetric includes aspects of vertex shading, in addition to the fixed-function tiling process." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FPKUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "fpk" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Defines the percentage of cycles where the Forward Pixel Kill (FPK) quad buffer\ncontains at least one fragment quad. This buffer is located after early ZS but before the execution core.\n\nDuring fragment shading this counter must be close to 100%. This indicates that the fragment\nfront-end is able to keep up with the shader core shading rate. This counter commonly drops below\n100% for three reasons:\n* The running workload has many empty tiles with no geometry to render. Empty tiles are\ncommon in shadow maps, for any screen region with no shadow casters.\n* The application consists of simple shaders but a high percentage of microtriangles. This\ncombination causes the shader core to complete fragments faster than they are rasterized, so\nthe quad buffer starts to drain.\n* The application consists of layers which stall at early ZS because of a dependency on an earlier\nfragment layer which is still in flight. Stalled layers prevent new fragments entering the quad\nbuffer, so the quad buffer starts to drain." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_QueueUtil );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "queue" );
				graph.SetLabel( "irq",		0 );
				graph.SetLabel( "non-frag",	1 );
				graph.SetLabel( "frag",		2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "IRQ pending utilization compared against the GPU active cycles.\nIn a well-functioning system, this expression is ideally less than 3% of the total cycles.\nIf the value is much higher than 3%, a system issue might be preventing the CPU from efficiently handling interrupts.\n\nNon-fragment queue utilization compared against the GPU active cycles.\nFor GPU bound content, it is expected that the GPU queues process work in parallel.\nThe dominant queue must be close to 100% utilized. If no queue is dominant, but the GPU is close to\n100% utilized, then there might be a serialization or dependency problem preventing better overlap\nacross the queues.\n\nFragment queue utilization compared against the GPU active cycles.\nFor GPU bound content, the GPU queues are expected to process work in parallel.\nAim to keep the dominant queue close to 100% utilized. If no queue is dominant, but the GPU is close to\n100% utilized, then there might be a serialization or dependency problem preventing better queue overlap." );
			}
			prof.graphTable.SetCaption( sec, "Functional unit utilization" );
		}{
			constexpr SecName	sec {"ShaderCore"};
			{
				auto&	graph = prof.graphTable.Add( sec, Mali_ALUUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "ALU" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Utilization of the arithmetic unit in the execution engine.\nThe most effective technique for reducing arithmetic load is reducing the complexity of your shader programs.\nIncreasing shader usage of 16-bit (mediump) variables can also help." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_NarrowArithUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "8/16 bit" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "This expression defines the percentage of arithmetic instructions that operate on 8/16-bit types.\nThese are more energy efficient, and require fewer registers for variable storage, than 32-bit operations." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_ArithPipeUtil );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "util" );
				graph.SetLabel( "FMA", 0 );
				graph.SetLabel( "CVT", 1 );
				graph.SetLabel( "SFU", 2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 70.f, 90.f );
				graph.SetDescription( "Utilization of the ALU pipes.\nFMA pipe - multiply/add instructions\nCVT pipe - convert data types\nSFU pipe - special functions unit (sqrt/sin/cos)" );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_WarpDivRate );
				graph.SetCapacity( capacity );
				graph.SetName( "warp divg" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetInvLimits( 5.f, 25.f );
				graph.SetDescription( "This expression defines the percentage of instructions that have\ncontrol flow divergence (false branches) across the warp." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_WrapCount );
				graph.SetCapacity( capacity, 4 );
				graph.SetName( "warps" );
				graph.SetLabel( "non-frag",	 0 );
				graph.SetLabel( "frag",		 1 );
				graph.SetLabel( "full quad", 2 );
				graph.SetLabel( "all reg",	 3 );
				graph.SetColor( style4 );
				graph.SetDescription( "Number of non-fragment warps, fragment warps, full quad warps, warps using more than 32 registers." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_AllRegWarpRate );
				graph.SetCapacity( capacity );
				graph.SetName( "all reg" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetInvLimits( 5.f, 25.f );
				graph.SetDescription( "Warps that use more than 32 registers, requiring the full register allocation of 64 registers.\nWarps that require more than 32 registers halve the peak thread occupancy of the shader core,\nand can make shader performance more sensitive to cache misses and memory stalls." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FullQuadWarpRate );
				graph.SetCapacity( capacity );
				graph.SetName( "full quad" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 5.f, 25.f );
				graph.SetDescription( "Warps that have a full thread slot allocation.\nNote that allocated thread slots may not contain a running thread\nif the workload cannot fill the whole allocation.\nFully allocated warps are more likely if:\n* Draw calls avoid late ZS dependency hazards.\n* Draw calls use meshes with a low percentage of tiny primitives.\n* Compute dispatches use work groups that are a multiple of warp size." );
			}{
				auto&	graph = prof.graphTable.Add( sec, Mali_FragRastPartRate );
				graph.SetCapacity( capacity );
				graph.SetName( "part quad" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetInvLimits( 5.f, 25.f );
				graph.SetDescription( "Fragment quads that contain samples with no coverage.\nA high percentage can indicate that the content has a high density of small triangles, which are\nexpensive to process. To avoid this, use mesh level-of-detail algorithms to select simpler meshes as\nobjects move further from the camera." );
			}
			prof.graphTable.SetCaption( sec, "Shader core" );
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

		const float		wnd_pos_x	= ImGui::GetCursorScreenPos().x;
		const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "PowerVR GPU profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			prof.graphTable.Draw( wnd_size.x, c_GraphHeight, c_GraphPadding, ImGui::IsItemHovered() );
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

		auto&	prof	= _pvrProf;
		auto&	cnt		= prof.counters;

		if ( cnt.empty() )
			return;

		const auto	AddPoint = [&] (ECounter type, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type ));

			scale = (scale == 0.f ? invFC : scale);
			if ( auto it = cnt.find( type ); it != cnt.end())		graph->AddNonScaled( List{ it->second * scale });
		}};

		const auto	AddPoint2 = [&] (ECounter type0, ECounter type1, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ));

					scale	= (scale == 0.f ? invFC : scale);
			float	value0	= 0.f,	value1 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1 });
		}};

		const auto	AddPoint3 = [&] (ECounter type0, ECounter type1, ECounter type2, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ) and
					prof.requiredCounters.contains( type2 ));

					scale	= (scale == 0.f ? invFC : scale);
			float	value0	= 0.f,	value1 = 0.f,	value2 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type2 ); it != cnt.end())	{ value2 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1, value2 });
		}};

		const auto	AddPoint4 = [&] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ) and
					prof.requiredCounters.contains( type2 ) and
					prof.requiredCounters.contains( type3 ));

					scale	= (scale == 0.f ? invFC : scale);
			float	value0	= 0.f,	value1 = 0.f,	value2 = 0.f,	value3 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type2 ); it != cnt.end())	{ value2 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type3 ); it != cnt.end())	{ value3 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1, value2, value3 });
		}};

		AddPoint(  ECounter::GPU_MemoryInterfaceLoad,		PVR_MemoryBusLoad,		1.f );	// %
		AddPoint(  ECounter::GPU_ClockSpeed,				PVR_GpuClockSpeed,		1.f );
		AddPoint(  ECounter::Tiler_TriangleRatio,			PVR_TilerTriangleRatio,	1.f );
		AddPoint(  ECounter::Texture_ReadStall,				PVR_TexReadStall,		1.f );	// %
		AddPoint(  ECounter::Shader_ShaderProcessingLoad,	PVR_ShaderLoad			);

		AddPoint2( ECounter::GPU_MemoryRead,				ECounter::GPU_MemoryWrite,					PVR_MemoryTraffic		);
		AddPoint2( ECounter::VertexShader_RegisterOverload,	ECounter::PixelShader_RegisterOverload,		PVR_RegisterOverload	);
		AddPoint2( ECounter::Tiler_TrianglesInputPerFrame,	ECounter::Tiler_TrianglesOutputPerFrame,	PVR_TilerTrianglesIO,	1.f );
		AddPoint2( ECounter::Renderer_HSR_Efficiency,		ECounter::Renderer_ISP_PixelLoad,			PVR_ZTest,				1.f );	// %

		AddPoint3( ECounter::RendererTimePerFrame,			ECounter::GeometryTimePerFrame,			ECounter::TDM_TimePerFrame,				PVR_GpuTime,		1.f );
		AddPoint3( ECounter::Shader_CyclesPerComputeKernel,	ECounter::Shader_CyclesPerVertex,		ECounter::Shader_CyclesPerPixel,		PVR_GpuCycles		);
		AddPoint3( ECounter::ComputeShader_ProcessingLoad,	ECounter::VertexShader_ProcessingLoad,	ECounter::PixelShader_ProcessingLoad,	PVR_ShaderLoad2		);

		AddPoint4( ECounter::RendererActive,	ECounter::GeometryActive,	ECounter::TDM_Active,	ECounter::SPM_Active,	PVR_GpuActive,	1.f );	// %
	}

/*
=================================================
	_InitPowerVRCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitPowerVRCountersImGui (const ImLineGraph::ColorStyle &style4, const ImLineGraph::ColorStyle &style1)
	{
		const uint	capacity	= 50;
		auto&		prof		= _pvrProf;

		{
			constexpr SecName	sec {"Memory"};
			{
				auto&	graph = prof.graphTable.Add( sec, PVR_MemoryTraffic );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "traffic" );
				graph.SetLabel( "read",  0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "B" );		// bytes
				graph.SetDescription( "GPU memory read/write bytes per second" );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_MemoryBusLoad );
				graph.SetCapacity( capacity );
				graph.SetName( "bus" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "GPU memory interface load.\nShows the total utilization of the GPU memory bus, for both read and write memory operations over the GPU memory interface within the current period." );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_TexReadStall );
				graph.SetCapacity( capacity );
				graph.SetName( "tex read stall" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "Texture read stall" );
			}
			prof.graphTable.SetCaption( sec, "External memory" );
		}{
			constexpr SecName	sec {"Cycles"};
			{
				auto&	graph = prof.graphTable.Add( sec, PVR_GpuTime );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "time" );
				graph.SetLabel( "gfx",  0 );
				graph.SetLabel( "geom", 1 );
				graph.SetLabel( "tex",  2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "s" );
				graph.SetDescription( "Time per frame: Renderer, Geometry, Texture (TDM)" );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_GpuActive );
				graph.SetCapacity( capacity, 4 );
				graph.SetName( "active" );
				graph.SetLabel( "gfx",  0 );
				graph.SetLabel( "geom", 1 );
				graph.SetLabel( "tdm",  2 );
				graph.SetLabel( "spm",  3 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "Active/inactive cycles in % for: Renderer, Geometry, TDM, SPM.\nRenderer - shows percentage of time that Renderer tasks were active. Renderer time refers to\nany time that is spent processing pixels and shading them. This includes the ISP (Image Synthesis\nProcessor), Texturing and Shader Processor units.\nGeometry - input primitives, tiler ?\nTDM - texture data master ?\nSPM - If the GPU overflows the parameter buffer during vertex processing it will enter smart parameter mode and attempt to grow the parameter buffer.\n" );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_GpuClockSpeed );
				graph.SetCapacity( capacity );
				graph.SetName( "clock" );
				graph.SetColor( style1 );
				graph.SetSuffix( "Hz" );
				graph.SetDescription( "GPU clock speed" );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_GpuCycles );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "cycles" );
				graph.SetLabel( "comp",	0 );
				graph.SetLabel( "vert",	1 );
				graph.SetLabel( "frag",	2 );
				graph.SetColor( style4 );
				graph.SetDescription( "Cycles per compute kernel, Cycles per vertex, Cycles per pixel" );
			}
			prof.graphTable.SetCaption( sec, "Utilization" );
		}{
			constexpr SecName	sec {"Tiler"};
			{
				auto&	graph = prof.graphTable.Add( sec, PVR_TilerTriangleRatio );
				graph.SetCapacity( capacity );
				graph.SetName( "tile tris rate" );
				graph.SetColor( style1 );
				graph.SetDescription( "Tiler triangle ratio" );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_TilerTrianglesIO );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "tile tris" );
				graph.SetLabel( "in",  0 );
				graph.SetLabel( "out", 1 );
				graph.SetColor( style4 );
				graph.SetDescription( "Tiler: Triangles input per frame, Triangles output per frame" );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_ZTest );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "Z" );
				graph.SetLabel( "hsr", 0 );
				graph.SetLabel( "isp", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetDescription( "HSR - represents the effectiveness of the Hidden Surface Removal (HSR) engine, rejecting\nobscured pixels before they get processed. This tells you the percentage of pixels sent to be shaded,\nout of the total number of pixels submitted.\n\nISP show percentage of the time that the ISP pixel-processing is busy.\nThe Image Synthesis Processor (ISP) is the part of the graphic core that fetches the primitive data\nand performs Hidden Surface Removal (HSR), along with depth and stencil tests.\nIt would mean that a large quantity of non-visible (non-shaded) pixels are being processed. These may be hidden due to:\n* The depth/stencil test (e.g. hidden behind an opaque object).\n* A process may be rendering triangles that update only the depth/stencil buffer (and not any colour buffer).\n" );
			}
			prof.graphTable.SetCaption( sec, "Tiler / Renderer" );
		}{
			constexpr SecName	sec {"Shader"};
			{
				auto&	graph = prof.graphTable.Add( sec, PVR_ShaderLoad2 );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "load" );
				graph.SetLabel( "comp",	0 );
				graph.SetLabel( "vert",	1 );
				graph.SetLabel( "frag",	2 );
				graph.SetColor( style4 );
				graph.SetDescription( "Shader processing load: compute, vertex, pixel.\nA high value indicates that a large percentage of the Shader's workload has been spent executing shader.\nPixel: Reduce alpha blending and discard/alpha test." );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_ShaderLoad );
				graph.SetCapacity( capacity );
				graph.SetName( "shader load" );
				graph.SetColor( style1 );
				graph.SetDescription( "Shader processing load.\nA high value indicates that a large percentage of the Shader's workload has been spent processing vertices, fragments and/or compute kernels." );
			}{
				auto&	graph = prof.graphTable.Add( sec, PVR_RegisterOverload );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "reg overload" );
				graph.SetLabel( "vert", 0 );
				graph.SetLabel( "frag", 1 );
				graph.SetColor( style4 );
				graph.SetLimits( 1.f, 10.f );
				graph.SetDescription( "Shader register overload: vertex, pixel.\nThis counter indicates when the hardware is under register pressure.\nThe value should be near 0% or very low in most situations." );
			}
			prof.graphTable.SetCaption( sec, "Shader core" );
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

		const float		wnd_pos_x	= ImGui::GetCursorScreenPos().x;
		const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "Adreno profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			prof.graphTable.Draw( wnd_size.x, c_GraphHeight, c_GraphPadding, ImGui::IsItemHovered() );
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateAdrenoCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateAdrenoCountersImGui (const float invFC)
	{
		using ECounter = AdrenoProfiler::ECounter;

		auto&	prof	= _adrenoProf;
		auto&	cnt		= prof.counters;

		if ( cnt.empty() )
			return;

		const auto	AddPoint1 = [&] (ECounter type, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type ));

			scale = (scale == 0.f ? invFC : scale);
			if ( auto it = cnt.find( type ); it != cnt.end())		graph->AddNonScaled( List{ it->second * scale });
		}};

		const auto	AddPoint2 = [&] (ECounter type0, ECounter type1, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ));

					scale	= (scale == 0.f ? invFC : scale);
			float	value0	= 0.f,	value1 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1 });
		}};

		const auto	AddPoint3 = [&] (ECounter type0, ECounter type1, ECounter type2, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ) and
					prof.requiredCounters.contains( type2 ));

					scale	= (scale == 0.f ? invFC : scale);
			float	value0	= 0.f,	value1 = 0.f,	value2 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type2 ); it != cnt.end())	{ value2 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1, value2 });
		}};

		const auto	AddPoint4 = [&] (ECounter type0, ECounter type1, ECounter type2, ECounter type3, GraphName::Ref graphName, float scale = 0.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ) and
					prof.requiredCounters.contains( type2 ) and
					prof.requiredCounters.contains( type3 ));

					scale	= (scale == 0.f ? invFC : scale);
			float	value0	= 0.f,	value1 = 0.f,	value2 = 0.f,	value3 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type2 ); it != cnt.end())	{ value2 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type3 ); it != cnt.end())	{ value3 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1, value2, value3 });
		}};

		AddPoint1( ECounter::LRZ_TileKilled,		Adreno_LrzTileKilled	);
		AddPoint1( ECounter::LRZ_TotalPixel,		Adreno_LrzTotalPixel	);
		AddPoint1( ECounter::RB_AliveCycles2D,		Adreno_Rb2DActive		);
		AddPoint1( ECounter::CCU_PartialBlockRead,	Adreno_CcuPartBlockRd	);
		AddPoint1( ECounter::CCU_2DPixels,			Adreno_Ccu2DPix			);

		AddPoint2( ECounter::LRZ_Read,						ECounter::LRZ_Write,				Adreno_LrzTraffic			);
		AddPoint2( ECounter::RAS_SuperTiles,				ECounter::RAS_8x4Tiles,				Adreno_RasTiles				);
		AddPoint2( ECounter::RAS_FullyCoveredSuperTiles,	ECounter::RAS_FullyCovered8x4Tiles,	Adreno_RasFullyCoveredTiles	);
		AddPoint2( ECounter::CCU_DepthBlocks,				ECounter::CCU_ColorBlocks,			Adreno_CcuDCBlocks			);
		AddPoint2( ECounter::CCU_GMemRead,					ECounter::CCU_GMemWrite,			Adreno_CcuGMem				);
		AddPoint2( ECounter::RB_ZRead,						ECounter::RB_ZWrite,				Adreno_RbZTraffic			);
		AddPoint2( ECounter::RB_CRead,						ECounter::RB_CWrite,				Adreno_RbCTraffic			);

		AddPoint3( ECounter::LRZ_PrimKilledByMaskGen,	ECounter::LRZ_PrimKilledByLRZ,	ECounter::LRZ_PrimPassed,	Adreno_LrzPrim	);
		AddPoint3( ECounter::RB_Z_Pass,					ECounter::RB_Z_Fail,			ECounter::RB_S_Fail,		Adreno_RbZSPass	);
	}

/*
=================================================
	_InitAdrenoCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitAdrenoCountersImGui (const ImLineGraph::ColorStyle &style4, const ImLineGraph::ColorStyle &style1)
	{
		const uint	capacity	= 50;
		auto&		prof		= _adrenoProf;

		{
			constexpr SecName	sec {"LRZ"};
			{
				auto&	graph = prof.graphTable.Add( sec, Adreno_LrzTraffic );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "traffic" );
				graph.SetLabel( "read",  0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_LrzPrim );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "prims" );
				graph.SetLabel( "maskgen",  0 );
				graph.SetLabel( "kill",		1 );
				graph.SetLabel( "pass",		2 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_LrzTileKilled );
				graph.SetCapacity( capacity );
				graph.SetName( "kill tiles" );
				graph.SetColor( style1 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_LrzTotalPixel );
				graph.SetCapacity( capacity );
				graph.SetName( "pixels" );
				graph.SetColor( style1 );
			}
			prof.graphTable.SetCaption( sec, "Low resolution Z pass" );
		}{
			constexpr SecName	sec {"Rasterizer"};
			{
				auto&	graph = prof.graphTable.Add( sec, Adreno_RasTiles );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "tiles" );
				graph.SetLabel( "super", 0 );
				graph.SetLabel( "8x4",	 1 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_RasFullyCoveredTiles );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "full cov tiles" );
				graph.SetLabel( "super", 0 );
				graph.SetLabel( "8x4",	 1 );
				graph.SetColor( style4 );
			}
			prof.graphTable.SetCaption( sec, "Rasterizer" );
		}{
			constexpr SecName	sec {"Render backend"};
			{
				auto&	graph = prof.graphTable.Add( sec, Adreno_RbZTraffic );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "Z" );
				graph.SetLabel( "read",	 0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_RbCTraffic );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "color" );
				graph.SetLabel( "read",	 0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_RbZSPass );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "ZS" );
				graph.SetLabel( "Z-pass", 0 );
				graph.SetLabel( "Z-fail", 1 );
				graph.SetLabel( "S-fail", 2 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_Rb2DActive );
				graph.SetCapacity( capacity );
				graph.SetName( "2d" );
				graph.SetColor( style1 );
			}
			prof.graphTable.SetCaption( sec, "Render backend" );
		}{
			constexpr SecName	sec {"CCU"};
			{
				auto&	graph = prof.graphTable.Add( sec, Adreno_CcuDCBlocks );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "blocks" );
				graph.SetLabel( "depth", 0 );
				graph.SetLabel( "color", 1 );
				graph.SetColor( style4 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_CcuPartBlockRd );
				graph.SetCapacity( capacity );
				graph.SetName( "part block rd" );
				graph.SetColor( style1 );
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_CcuGMem );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "gmem" );
				graph.SetLabel( "read",	 0 );
				graph.SetLabel( "write", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "B" );		// bytes
			}{
				auto&	graph = prof.graphTable.Add( sec, Adreno_Ccu2DPix );
				graph.SetCapacity( capacity );
				graph.SetName( "2d pix" );
				graph.SetColor( style1 );
			}
			prof.graphTable.SetCaption( sec, "Cache and Compression Unit" );
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
		const float		x_offset	= wnd_pos_x + c_GraphPadding[0];
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
				ImGui::Text( "%s (%s)", core.name.c_str(), ToString( core.type ).data() );

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

					graph->Draw( INOUT region );

					if ( i >= core_per_line )
					{
						i = 0;
						left_top.x  = x_offset;
						left_top.y += graph_height;
					}
				}
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, left_top.y });
			}
			left_top.y += graph_height;
			ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, left_top.y - (left_top.x == x_offset ? graph_height : 0.f) + c_GraphPadding[1] });
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

				graph->Add( List{ (user[core_id] + kernel[core_id]) * 100.f, kernel[core_id] * 100.f });
			}
		}
	}
#endif
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_DrawProfilerNVidiaImGui
=================================================
*/
	void  HwpcProfiler::_DrawProfilerNVidiaImGui ()
	{
		auto&	prof = _nvProf;
		if ( not prof.profiler.IsInitialized() )
			return;

		const float		wnd_pos_x	= ImGui::GetCursorScreenPos().x;
		const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();

		if ( ImGui::CollapsingHeader( "NVidia profiler", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			prof.graphTable.Draw( wnd_size.x, c_GraphHeight, c_GraphPadding, ImGui::IsItemHovered() );
		}
		ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
	}

/*
=================================================
	_UpdateNVidiaCountersImGui
=================================================
*/
	void  HwpcProfiler::_UpdateNVidiaCountersImGui (const float)
	{
		using ECounter = NVidiaProfiler::ECounter;

		auto&	prof	= _nvProf;
		auto&	cnt		= prof.counters;

		if ( cnt.empty() )
			return;

		const auto	AddPoint1 = [&] (ECounter type, GraphName::Ref graphName, float scale = 1.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type ));

			if ( auto it = cnt.find( type ); it != cnt.end())		graph->AddNonScaled( List{ it->second * scale });
		}};

		const auto	AddPoint2 = [&] (ECounter type0, ECounter type1, GraphName::Ref graphName, float scale = 1.f)
		{{
			auto	graph = prof.graphTable.Get( graphName );
			CHECK_ERRV( graph );
			ASSERT( prof.requiredCounters.contains( type0 ) and
					prof.requiredCounters.contains( type1 ));

			float	value0	= 0.f,	value1 = 0.f;
			bool	exists	= false;

			if ( auto it = cnt.find( type0 ); it != cnt.end())	{ value0 = it->second * scale;  exists = true; }
			if ( auto it = cnt.find( type1 ); it != cnt.end())	{ value1 = it->second * scale;  exists = true; }

			if ( exists )	graph->AddNonScaled( List{ value0, value1 });
		}};

		AddPoint1( ECounter::MemoryUtil,		NV_MemoryUtil	);
		AddPoint1( ECounter::MemoryClock,		NV_MemoryClock,		1.0e+6f );

		AddPoint1( ECounter::GpuUtil,			NV_GpuUtil		);
		AddPoint1( ECounter::GraphicsClock,		NV_GraphicsClock,	1.0e+6f );
		AddPoint1( ECounter::SMClock,			NV_SMClock,			1.0e+6f );
		AddPoint1( ECounter::VideoClock,		NV_VideoClock,		1.0e+6f );

		AddPoint1( ECounter::GpuTemperature,	NV_GpuTemp		);
		AddPoint1( ECounter::PowerUsage,		NV_PowerUsage	);
		AddPoint1( ECounter::PerfState,			NV_PerfState	);
		AddPoint1( ECounter::FanSpeed,			NV_FanSpeed		);

		AddPoint2( ECounter::DevMemUsed,	ECounter::UnifiedMemUsed,	NV_MemUsed	);
		AddPoint2( ECounter::DevMemUsedMb,	ECounter::UnifiedMemUsedMb,	NV_MemUsedMb );
	}

/*
=================================================
	_InitNVidiaCountersImGui
=================================================
*/
	void  HwpcProfiler::_InitNVidiaCountersImGui (const ImLineGraph::ColorStyle &style4, const ImLineGraph::ColorStyle &style1)
	{
		const uint	capacity	= 50;
		auto&		prof		= _nvProf;

		{
			constexpr SecName	sec {"Memory"};
			{
				auto&	graph = prof.graphTable.Add( sec, NV_MemoryUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "util" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 75.f, 90.f );
				graph.SetDescription( "Memory utilization" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_MemoryClock );
				graph.SetCapacity( capacity );
				graph.SetName( "clock" );
				graph.SetColor( style1 );
				graph.SetSuffix( "Hz" );
				graph.SetDescription( "Memory clock" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_MemUsed );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "used" );
				graph.SetLabel( "dev",  0 );
				graph.SetLabel( "host", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 75.f, 90.f );
				graph.SetDescription( "Used memory in %:\nDevice - VRAM\nHost - CPU visible VRAM (unified memory)" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_MemUsedMb );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "used" );
				graph.SetLabel( "dev",  0 );
				graph.SetLabel( "host", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "b" );
				graph.SetDescription( "Used memory in bytes:\nDevice - VRAM\nHost - CPU visible VRAM (unified memory)" );
			}
			prof.graphTable.SetCaption( sec, "Memory" );
		}{
			constexpr SecName	sec {"GPU"};
			{
				auto&	graph = prof.graphTable.Add( sec, NV_GpuUtil );
				graph.SetCapacity( capacity );
				graph.SetName( "util" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 75.f, 90.f );
				graph.SetDescription( "GPU utilization" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_GraphicsClock );
				graph.SetCapacity( capacity );
				graph.SetName( "graphics" );
				graph.SetColor( style1 );
				graph.SetSuffix( "Hz" );
				graph.SetDescription( "Graphics clock" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_SMClock );
				graph.SetCapacity( capacity );
				graph.SetName( "sm" );
				graph.SetColor( style1 );
				graph.SetSuffix( "Hz" );
				graph.SetDescription( "SM clock" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_VideoClock );
				graph.SetCapacity( capacity );
				graph.SetName( "video" );
				graph.SetColor( style1 );
				graph.SetSuffix( "Hz" );
				graph.SetDescription( "Video encoder/decoder clock" );
			}
			prof.graphTable.SetCaption( sec, "GPU Core" );
		}{
			constexpr SecName	sec {"Power"};
			{
				auto&	graph = prof.graphTable.Add( sec, NV_GpuTemp );
				graph.SetCapacity( capacity );
				graph.SetName( "temp" );
				graph.SetColor( style1 );
				graph.SetSuffix( "C" );
				graph.SetLimits( 75.f, 90.f );
				graph.SetDescription( "GPU temperature" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_PowerUsage );
				graph.SetCapacity( capacity );
				graph.SetName( "usage" );
				graph.SetColor( style1 );
				graph.SetSuffix( "W" );
				graph.SetDescription( "GPU power usage in Watts" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_PerfState );
				graph.SetCapacity( capacity );
				graph.SetName( "perf" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 75.f, 90.f );
				graph.SetDescription( "Performance state:\n1 - maximum performance\n0 - minimum performance" );
			}{
				auto&	graph = prof.graphTable.Add( sec, NV_FanSpeed );
				graph.SetCapacity( capacity );
				graph.SetName( "fan" );
				graph.SetColor( style1 );
				graph.SetSuffix( "%" );
				graph.SetLimits( 75.f, 90.f );
				graph.SetDescription( "Intended fan speed in %, may be > 100%" );
			}
			prof.graphTable.SetCaption( sec, "Power" );
		}
	}

#endif
//-----------------------------------------------------------------------------

} // AE::Profiler

#endif // AE_ENABLE_IMGUI
