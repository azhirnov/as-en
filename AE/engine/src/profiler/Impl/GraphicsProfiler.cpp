// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
#endif
#include "profiler/Impl/GraphicsProfiler.h"

namespace AE::Profiler
{
namespace
{
	static constexpr GraphName	CPU_FPS						{"CPU_FPS"};
	static constexpr GraphName	GPU_FrameTime				{"GPU_FrameTime"};
	static constexpr GraphName	CPU_ExternalTime			{"CPU_ExternalTime"};

	static constexpr GraphName	GPU_MemUsagePct				{"GPU_MemUsagePct"};
	static constexpr GraphName	GPU_DevMemUsage				{"GPU_DevMemUsage"};
	static constexpr GraphName	GPU_HostMemUsage			{"GPU_HostMemUsage"};
	static constexpr GraphName	GPU_UniMemUsage				{"GPU_UniMemUsage"};

	static constexpr GraphName	GPU_MemTrafficToDev			{"GPU_MemTrafficToDevPerFrame"};
	static constexpr GraphName	GPU_MemTrafficToHost		{"GPU_MemTrafficToHostPerFrame"};
	static constexpr GraphName	GPU_MemTrafficToDev2		{"GPU_MemTrafficToDevPerSec"};
	static constexpr GraphName	GPU_MemTrafficToHost2		{"GPU_MemTrafficToHostPerSec"};

	static constexpr GraphName	Stat_Primitives				{"Stat_Primitives"};
	static constexpr GraphName	Stat_VS_FS_CS_Invoc			{"Stat_VS_FS_CS_Invoc"};
	static constexpr GraphName	Stat_TS_MS_Invoc			{"Stat_TS_MS_Invoc"};

/*
=================================================
	GetStyle4
=================================================
*/
	ND_ static ImLineGraph::ColorStyle  GetStyle4 ()
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
		style4.mode			= ImLineGraph::EMode::Line_Adaptive;

		return style4;
	}

/*
=================================================
	GetStyle1
=================================================
*/
	ND_ static ImLineGraph::ColorStyle  GetStyle1 ()
	{
		ImLineGraph::ColorStyle		style1 = GetStyle4();
		style1.lines[0]		= RGBA8u{ 20, 170,  20, 255};
		style1.border		= RGBA8u{200, 200, 255, 255};
		return style1;
	}

} // namespace

	using namespace AE::Graphics;
	using EContextType = IGraphicsProfiler::EContextType;


/*
=================================================
	PerFrameData ctor
=================================================
*/
	GraphicsProfiler::PerFrameData::PerFrameData ()
	{
		activeCmdbufs.reserve( 64 );
	}

/*
=================================================
	PerFrameData::Clear
=================================================
*/
	void  GraphicsProfiler::PerFrameData::Clear ()
	{
		EXLOCK( guard );
		activeCmdbufs.clear();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	GraphicsProfiler::GraphicsProfiler (TimePoint_t startTime, PowerVRProfiler* pvrProfiler) __NE___ :
		ProfilerUtils{ startTime },
		_pvrProfiler{ pvrProfiler }
	{
		_InitImGUI( GetStyle4(), GetStyle1() );

	  #ifdef AE_ENABLE_VULKAN
		_pplnStats.hasMeshShader = GraphicsScheduler().GetDevice().GetVProperties().meshShaderFeats.meshShaderQueries;
	  #endif
		_pplnStats.graphics	= {};
		_pplnStats.compute	= {};
	}

/*
=================================================
	DrawImGUI
=================================================
*/
#ifdef AE_ENABLE_IMGUI
	void  GraphicsProfiler::DrawImGUI ()
	{
	  #ifdef AE_PLATFORM_ANDROID
		ImGui::SetNextWindowPos( ImVec2{0,0}, ImGuiCond_Once );
		ImGui::SetNextWindowSize( ImVec2{650,300}, ImGuiCond_Once );

		const ImGuiWindowFlags	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
	  #else
		const ImGuiWindowFlags	flags = ImGuiWindowFlags_NoScrollbar;
	  #endif

		if ( ImGui::Begin( "GraphicsProfiler", null, flags ))
		{
			String	str;

			// fps
			{
				str.clear();
				str << "FPS: " << ToString( _fps.result, 1 );
				str << ", dt: " << ToString( _fps.dt );
				str << ", extern: " << ToString( _fps.ext );
				ImGui::TextUnformatted( str.c_str() );
			}

			if ( ImGui::CollapsingHeader( "Counters" ))
			{
				const float		wnd_pos_x		= ImGui::GetCursorScreenPos().x;
				const ImVec2	wnd_size		= ImGui::GetContentRegionAvail();
				const float		c_GraphHeight	= 150.f;
				const float2	c_GraphPadding	{ 2.0f, 8.f };

				_graphTable.Draw( wnd_size.x, c_GraphHeight, c_GraphPadding, ImGui::IsItemHovered() );
				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos_x, ImGui::GetCursorScreenPos().y });
			}

			if ( ImGui::CollapsingHeader( "GPU time", ImGuiTreeNodeFlags_DefaultOpen ))
			{
				const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();
				const ImVec2	wnd_pos		= ImGui::GetCursorScreenPos();
				const RectF		max_region	= RectF{float2{ wnd_size.x, Abs(wnd_size.y) }} + float2{wnd_pos.x, wnd_pos.y};

				RectF	region1 = max_region;
				_imGPUTimeHistory.Draw( INOUT region1 );

				ImGui::SetCursorScreenPos( ImVec2{ wnd_pos.x, region1.bottom });
			}

			ImGui::Dummy( ImVec2{1,1} );
		}
		ImGui::End();
	}
#endif
/*
=================================================
	_InitImGUI
=================================================
*/
#ifdef AE_ENABLE_IMGUI
	void  GraphicsProfiler::_InitImGUI (const ImLineGraph::ColorStyle &style4, const ImLineGraph::ColorStyle &style1)
	{
		const uint	capacity	= 50;
		{
			constexpr SecName	sec {"Time"};
			{
				auto&	graph = _graphTable.Add( sec, CPU_FPS );
				graph.SetCapacity( capacity );
				graph.SetName( "fps" );
				graph.SetColor( style1 );
				graph.SetAlertInvLimits( 60.f, 30.f );
				graph.SetDescription( "Frames per second" );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_FrameTime );
				graph.SetCapacity( capacity );
				graph.SetName( "gpu dt" );
				graph.SetColor( style1 );
				graph.SetSuffix( "s" );
				graph.SetAlertLimits( 15.f, 18.f );
				graph.SetDescription( "Frame time on GPU side." );
			}{
				auto&	graph = _graphTable.Add( sec, CPU_ExternalTime );
				graph.SetCapacity( capacity );
				graph.SetName( "extern" );
				graph.SetColor( style1 );
				graph.SetSuffix( "s" );
				graph.SetDescription( "Frame time on CPU side minus GPU time.\nMay spend time on VSync or on CPU side." );
			}
			_graphTable.SetCaption( sec, "CPU" );
		}{
			constexpr SecName	sec {"MemoryUsage"};
			{
				auto	style		= style4;
						style.mode	= ImLineGraph::EMode::Line;
				auto&	graph = _graphTable.Add( sec, GPU_MemUsagePct );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "usage" );
				graph.SetLabel( "dev",  0 );
				graph.SetLabel( "host", 1 );
				graph.SetLabel( "uni",  2 );
				graph.SetColor( style );
				graph.SetSuffix( "%" );
				graph.SetRange( 0.f, 100.f );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_DevMemUsage );
				graph.SetCapacity( capacity );
				graph.SetName( "dev" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B" );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_HostMemUsage );
				graph.SetCapacity( capacity );
				graph.SetName( "host" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B" );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_UniMemUsage );
				graph.SetCapacity( capacity );
				graph.SetName( "unified" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B" );
			}
			_graphTable.SetCaption( sec, "Memory usage" );
		}{
			constexpr SecName	sec {"MemoryTraffic"};
			{
				auto&	graph = _graphTable.Add( sec, GPU_MemTrafficToDev );
				graph.SetCapacity( capacity );
				graph.SetName( "to dev" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B/f" );
				graph.SetDescription( "Statistics from staging buffer." );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_MemTrafficToHost );
				graph.SetCapacity( capacity );
				graph.SetName( "to host" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B/f" );
				graph.SetDescription( "Statistics from staging buffer." );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_MemTrafficToDev2 );
				graph.SetCapacity( capacity );
				graph.SetName( "to dev" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B/s" );
				graph.SetDescription( "Statistics from staging buffer." );
			}{
				auto&	graph = _graphTable.Add( sec, GPU_MemTrafficToHost2 );
				graph.SetCapacity( capacity );
				graph.SetName( "to host" );
				graph.SetColor( style1 );
				graph.SetSuffix( "B/s" );
				graph.SetDescription( "Statistics from staging buffer." );
			}
			_graphTable.SetCaption( sec, "Memory traffic" );
		}{
			constexpr SecName	sec {"PipelineStat"};
			{
				auto&	graph = _graphTable.Add( sec, Stat_Primitives );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "prim" );
				graph.SetLabel( "in",  0 );
				graph.SetLabel( "out", 1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "/f" );
				graph.SetDescription( "Input assembly primitives and output of clipping stage.\nLarge difference between them indicates that frustum culling it not used." );
			}{
				auto&	graph = _graphTable.Add( sec, Stat_VS_FS_CS_Invoc );
				graph.SetCapacity( capacity, 3 );
				graph.SetName( "invoc" );
				graph.SetLabel( "vert",  0 );
				graph.SetLabel( "frag",  1 );
				graph.SetLabel( "comp",  2 );
				graph.SetColor( style4 );
				graph.SetSuffix( "/f" );
				graph.SetDescription( "Number of vertex, fragment and compute shader invocations." );
			}{
				auto&	graph = _graphTable.Add( sec, Stat_TS_MS_Invoc );
				graph.SetCapacity( capacity, 2 );
				graph.SetName( "invoc" );
				graph.SetLabel( "task",  0 );
				graph.SetLabel( "mesh",  1 );
				graph.SetColor( style4 );
				graph.SetSuffix( "/f" );
				graph.SetDescription( "Number of task and mesh shader invocations." );
			}
			_graphTable.SetCaption( sec, "Pipeline statistic" );
		}
	}
#endif
/*
=================================================
	Draw
=================================================
*/
	void  GraphicsProfiler::Draw (Graphics::Canvas &canvas)
	{
		Unused( canvas );
	}

/*
=================================================
	Update
=================================================
*/
	void  GraphicsProfiler::Update (secondsf dt)
	{
		auto&		rts			= GraphicsScheduler();
		const uint	frame_count	= _fps.frameCount.exchange( 0 );

		// fps
		{
			double	accum_time	= _fps.accumFrameTime.exchange( 0.0 );

			_fps.result	= float(frame_count) / dt.count();
			_fps.dt		= nanosecondsf{ float( accum_time / frame_count )};
			_fps.ext	= Max( nanosecondsf{0.f}, (TimeCast<nanosecondsf>(dt) - nanosecondsf{accum_time}) / frame_count );

			if ( auto graph = _graphTable.Get( CPU_FPS ))
				graph->AddNonScaled( List{ _fps.result });

			if ( auto graph = _graphTable.Get( GPU_FrameTime ))
				graph->AddNonScaled( List{ secondsf{_fps.dt}.count() });

			if ( auto graph = _graphTable.Get( CPU_ExternalTime ))
				graph->AddNonScaled( List{ secondsf{_fps.ext}.count() });
		}

		// memory usage
		if ( auto mem_usage = rts.GetDevice().GetMemoryUsage();
			 mem_usage.has_value() )
		{
			const double	dev_usage_pct	= Max( 100.0 * double(ulong(mem_usage->deviceUsage))  / double(ulong(mem_usage->deviceAvailable  + mem_usage->deviceUsage)),  0.0 );
			const double	host_usage_pct	= Max( 100.0 * double(ulong(mem_usage->hostUsage))    / double(ulong(mem_usage->hostAvailable    + mem_usage->hostUsage)),    0.0 );
			const double	uni_usage_pct	= Max( 100.0 * double(ulong{mem_usage->unifiedUsage}) / double(ulong{mem_usage->unifiedAvailable + mem_usage->unifiedUsage}), 0.0 );

			if ( auto graph = _graphTable.Get( GPU_MemUsagePct ))
				graph->AddNonScaled( List{ dev_usage_pct, host_usage_pct, uni_usage_pct });

			if ( auto graph = _graphTable.Get( GPU_DevMemUsage ))
				graph->AddNonScaled( List{ double(ulong(mem_usage->deviceUsage)) });

			if ( auto graph = _graphTable.Get( GPU_HostMemUsage ))
				graph->AddNonScaled( List{ double(ulong(mem_usage->hostUsage)) });

			if ( auto graph = _graphTable.Get( GPU_UniMemUsage ))
				graph->AddNonScaled( List{ double(ulong(mem_usage->unifiedUsage)) });
		}

		// mem traffic
		{
			const double	write	= double(ulong{_memTraffic.accumWrite.exchange( 0_b )});
			const double	read	= double(ulong{_memTraffic.accumRead.exchange( 0_b )});

			if ( auto graph = _graphTable.Get( GPU_MemTrafficToDev ))
				graph->AddNonScaled( List{ write / double(frame_count) });

			if ( auto graph = _graphTable.Get( GPU_MemTrafficToHost ))
				graph->AddNonScaled( List{ read / double(frame_count) });

			if ( auto graph = _graphTable.Get( GPU_MemTrafficToDev2 ))
				graph->AddNonScaled( List{ write / double(dt.count()) });

			if ( auto graph = _graphTable.Get( GPU_MemTrafficToHost2 ))
				graph->AddNonScaled( List{ read / double(dt.count()) });
		}

		// pipeline statistic
		{
			ComputePipelineStatistic	c_stat;
			MeshPipelineStatistic		g_stat;
			{
				SHAREDLOCK( _pplnStats.guard );
				g_stat	= _pplnStats.graphics;
				c_stat	= _pplnStats.compute;
			}

			if ( auto graph = _graphTable.Get( Stat_Primitives ))
			{
				graph->AddNonScaled( List{	double(g_stat.inputAssemblyPrimitives) / double(dt.count()),
											double(g_stat.afterClipping) / double(dt.count()) });
			}
			if ( auto graph = _graphTable.Get( Stat_VS_FS_CS_Invoc ))
			{
				graph->AddNonScaled( List{	double(g_stat.vertShaderInvocations) / double(dt.count()),
											double(g_stat.fragShaderInvocations) / double(dt.count()),
											double(c_stat.computeInvocations) / double(dt.count()) });
			}
			if ( auto graph = _graphTable.Get( Stat_TS_MS_Invoc ))
			{
				graph->AddNonScaled( List{	double(g_stat.meshTaskInvocations) / double(dt.count()),
											double(g_stat.meshInvocations) / double(dt.count()) });
			}
		}
	}

/*
=================================================
	SetQueue
=================================================
*/
	void  GraphicsProfiler::SetQueue (EQueueType type, StringView name) __NE___
	{
		Unused( type, name );
	}

/*
=================================================
	BeginBatch
=================================================
*/
	void  GraphicsProfiler::BeginBatch (FrameUID frameId, const void* batch, StringView name) __NE___
	{
		Unused( frameId, batch, name );
	}

/*
=================================================
	SubmitBatch
=================================================
*/
	void  GraphicsProfiler::SubmitBatch (const void* batch, EQueueType queue) __NE___
	{
		Unused( batch, queue );
	}

/*
=================================================
	BatchComplete
=================================================
*/
	void  GraphicsProfiler::BatchComplete (const void* batch) __NE___
	{
		Unused( batch );
	}

/*
=================================================
	BeginDrawBatch
=================================================
*/
	void  GraphicsProfiler::BeginDrawBatch (const void* batch, StringView name) __NE___
	{
		Unused( batch, name );
	}

/*
=================================================
	_ReadResultsTask
=================================================
*/
	AsyncCoro  GraphicsProfiler::_ReadResultsTask (RC<GraphicsProfiler> self)
	{
		if ( self->_pvrProfiler and self->_pvrProfiler->IsInitialized() )
			self->_ReadResultsPVR();
		else
			self->_ReadResults();

		self = null;
		co_return;
	}

/*
=================================================
	NextFrame
=================================================
*/
	void  GraphicsProfiler::NextFrame (FrameUID frameId) __NE___
	{
		auto&	rts = GraphicsScheduler();

		{
			auto&	qm	= rts.GetQueryManager();
			uint2	idx = qm.ReadAndWriteIndices();

			_readIndex	= idx[0];
			_writeIndex	= idx[1];
		}{
			auto	task = Scheduler().Run( _ReadResultsTask( GetRC<GraphicsProfiler>() ));
			rts.AddNextFrameDeps( task );
		}{
			auto	stat = rts.GetResourceManager().GetStagingBufferFrameStat( frameId.Sub(1).value() );

			_memTraffic.accumWrite.fetch_add( stat.dynamicWrite + stat.staticWrite );
			_memTraffic.accumRead.fetch_add( stat.dynamicRead + stat.staticRead );
		}

		++_fps.frameCount;
		_perFrame[ _writeIndex ].Clear();
	}

/*
=================================================
	_ReadResults
=================================================
*/
	void  GraphicsProfiler::_ReadResults ()
	{
		auto&	f = _perFrame[_readIndex];
		EXLOCK( f.guard );

		_gpuTime.min	= nanosecondsd{0.0};
		_gpuTime.max	= nanosecondsd{0.0};

		if ( f.activeCmdbufs.empty() )
			return;

		_gpuTime.min	= nanosecondsd{MaxValue<double>()};

		auto&	qm		= GraphicsScheduler().GetQueryManager();

		MeshPipelineStatistic		g_stat = {};
		ComputePipelineStatistic	c_stat = {};

		_imGPUTimeHistory.Begin();

		for (auto& [key, info] : f.activeCmdbufs)
		{
			for (auto& pass : info.passes)
			{
				ASSERT( pass.recorded );

				if ( pass.timestamp )
				{
					nanosecondsd	time[2] = {};
					Unused( qm.GetTimestamp( pass.timestamp, OUT time, Sizeof(time) ));

					//ASSERT( time[0] <= time[1] );
					_gpuTime.min = Min( _gpuTime.min, time[0] );
					_gpuTime.max = Max( _gpuTime.max, time[1] );

					_imGPUTimeHistory.Add( pass.name, pass.color, time[0], time[1] );
				}

				if ( pass.pplnStat )
				{
					switch ( pass.pplnStat.type )
					{
						case EQueryType::GraphicsPipelineStatistic :
						{
							GraphicsPipelineStatistic	stat;
							Unused( qm.GetPipelineStatistic( pass.pplnStat, OUT &stat, Sizeof(stat) ));

							StaticAssert( IsBaseOf< GraphicsPipelineStatistic, MeshPipelineStatistic >);
							RefCast<GraphicsPipelineStatistic>(g_stat) += stat;
							break;
						}
						case EQueryType::ComputePipelineStatistic :
						{
							ComputePipelineStatistic	stat;
							Unused( qm.GetPipelineStatistic( pass.pplnStat, OUT &stat, Sizeof(stat) ));
							c_stat += stat;
							break;
						}
						case EQueryType::MeshPipelineStatistic :
						{
							MeshPipelineStatistic	stat;
							Unused( qm.GetPipelineStatistic( pass.pplnStat, OUT &stat, Sizeof(stat) ));
							g_stat += stat;
							break;
						}
					}
				}
			}
		}

		_gpuTime.min = Min( _gpuTime.min, _gpuTime.max );

		_imGPUTimeHistory.End( _gpuTime.min, _gpuTime.max );

		_fps.accumFrameTime.fetch_add( (_gpuTime.max - _gpuTime.min).count() );

		EXLOCK( _pplnStats.guard );
		_pplnStats.graphics	= g_stat;
		_pplnStats.compute	= c_stat;
	}

/*
=================================================
	_ReadResultsPVR
=================================================
*/
	void  GraphicsProfiler::_ReadResultsPVR ()
	{
		auto&	f = _perFrame[_readIndex];
		EXLOCK( f.guard );

		_pvrProfiler->ReadTimingData( OUT _pvrTimings );

		if ( _pvrTimings.empty() )
			return;

		_gpuTime.min	= nanosecondsd{MaxValue<double>()};
		_gpuTime.max	= nanosecondsd{0.0};

		ArrayView<PowerVRProfiler::TimeScope>	timings_view = _pvrTimings;

	#if 0
		// find min/max time
		nanosecondsd	max_dt	{0.0};
		nanosecondsd	avg_dt	{0.0};

		for (auto& t : timings_view)
		{
			auto	dt = t.end - t.begin;
			max_dt = Max( max_dt, dt );
			avg_dt += dt;
		}

		avg_dt /= double(timings_view.size());
		const nanosecondsd	min_dt = Min( max_dt * 0.1, avg_dt );


		// find significant time
		FixedArray< PowerVRProfiler::TimeScope, 32 >	timings;

		for (auto& t : timings_view)
		{
			_gpuTime.min = Min( _gpuTime.min, t.begin );
			_gpuTime.max = Max( _gpuTime.max, t.end );

			auto	dt = t.end - t.begin;
			if ( dt > min_dt )
				timings.try_push_back( t );
		}
		timings_view = timings;

	#elif 1
		FixedArray< PowerVRProfiler::TimeScope, 32 >	timings;
		const nanosecondsd								min_dt {milliseconds{1}};

		for (auto& t : timings_view)
		{
			_gpuTime.min = Min( _gpuTime.min, t.begin );
			_gpuTime.max = Max( _gpuTime.max, t.end );

			auto	dt = t.end - t.begin;
			if ( dt > min_dt )
				timings.try_push_back( t );
		}
		timings_view = timings;

	#else

		const nanosecondsd	min_dt = secondsd{2.0 / 60.0};

		for (auto& t : timings_view)
		{
			auto	frame_time = _gpuTime.max - _gpuTime.min;
			if_unlikely( frame_time > min_dt )
			{
				timings_view = ArrayView<PowerVRProfiler::TimeScope>{ timings_view.data(), &t };
				break;
			}

			_gpuTime.min = Min( _gpuTime.min, t.begin );
			_gpuTime.max = Max( _gpuTime.max, t.end );
		}

	#endif


		// add to graph
		_imGPUTimeHistory.Begin();

		for (auto& t : timings_view)
		{
			StringView	name;
			RGBA8u		color;

			using EPass = PowerVRProfiler::EPass;
			switch_enum( t.pass )
			{
				case EPass::Compute :		name = "Compute";		color = HtmlColor::Yellow;	break;
				case EPass::Tiler :			name = "Tiler";			color = HtmlColor::Blue;	break;
				case EPass::Renderer :		name = "Renderer";		color = HtmlColor::Lime;	break;
				case EPass::Transfer :		name = "Transfer";		color = HtmlColor::Red;		break;
				case EPass::RayTracing :	name = "RayTracing";	color = HtmlColor::Violet;	break;
				case EPass::RTASBuild :		name = "RTASBuild";		color = HtmlColor::Pink;	break;
				case EPass::Unknown :		break;
			}
			switch_end

			_imGPUTimeHistory.Add( name, color, t.begin, t.end );
		}

		_gpuTime.min = Min( _gpuTime.min, _gpuTime.max );

		_imGPUTimeHistory.End( _gpuTime.min, _gpuTime.max );

		_fps.accumFrameTime.fetch_add( (_gpuTime.max - _gpuTime.min).count() );
	}
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
	BatchCmdbufKey::operator ==
=================================================
*/
	bool  GraphicsProfiler::BatchCmdbufKey::operator == (const BatchCmdbufKey &rhs) C_NE___
	{
		return	batch	== rhs.batch	and
				cmdbuf	== rhs.cmdbuf	and
				type	== rhs.type;
	}

/*
=================================================
	BatchCmdbufKey::CalcHash
=================================================
*/
	HashVal  GraphicsProfiler::BatchCmdbufKey::CalcHash () C_NE___
	{
		return HashOf(batch) + HashOf(cmdbuf) + HashOf(type);
	}

/*
=================================================
	BeginContextTypeToStage
=================================================
*/
	ND_ static VkPipelineStageFlagBits2  BeginContextTypeToStage (EContextType type) __NE___
	{
	#if 1
		Unused( type );
		return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	#else
		switch_enum( type )
		{
			case EContextType::Transfer :		return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			case EContextType::Compute :		return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			case EContextType::RenderPass :		return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			case EContextType::ASBuild :		return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
			case EContextType::RayTracing :		return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
			case EContextType::VideoDecode :	return VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR;
			case EContextType::VideoEncode :	return VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR;
			case EContextType::Graphics :		break;
			case EContextType::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown context type" );
	#endif
	}

/*
=================================================
	EndContextTypeToStage
=================================================
*/
	ND_ static VkPipelineStageFlagBits2  EndContextTypeToStage (EContextType type) __NE___
	{
	#if 1
		Unused( type );
		return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	#else
		switch_enum( type )
		{
			case EContextType::Transfer :		return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			case EContextType::Compute :		return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			case EContextType::RenderPass :		return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			case EContextType::ASBuild :		return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
			case EContextType::RayTracing :		return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
			case EContextType::VideoDecode :	return VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR;
			case EContextType::VideoEncode :	return VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR;
			case EContextType::Graphics :		break;
			case EContextType::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown context type" );
	#endif
	}

/*
=================================================
	BeginContext
=================================================
*/
	void  GraphicsProfiler::BeginContext (const void* batchPtr, VkCommandBuffer cmdbuf, StringView taskName, RGBA8u color, EContextType type) __NE___
	{
		// not compatible with Metal, only single render pass can be measured.
		if ( type == EContextType::Graphics )
			return;

		ASSERT( not taskName.empty() );

		auto&		rts		= GraphicsScheduler();
		auto&		dev		= rts.GetDevice();
		auto&		qm		= rts.GetQueryManager();
		auto*		batch	= Cast<CommandBatch>(batchPtr);
		const auto	queue	= batch->GetQueueType();
		Pass		pass;

		if ( type == EContextType::RenderPass )
		{
			ASSERT( not pass.pplnStat );
			if ( auto q_stat = qm.AllocQuery( queue, _pplnStats.hasMeshShader ? EQueryType::MeshPipelineStatistic : EQueryType::GraphicsPipelineStatistic ))
			{
				dev.vkCmdBeginQuery( cmdbuf, q_stat.pool, q_stat.first, 0 );
				pass.pplnStat = q_stat;
			}
		}

		if ( type == EContextType::Compute )
		{
			ASSERT( not pass.pplnStat );
			if ( auto q_stat = qm.AllocQuery( queue, EQueryType::ComputePipelineStatistic ))
			{
				dev.vkCmdBeginQuery( cmdbuf, q_stat.pool, q_stat.first, 0 );
				pass.pplnStat = q_stat;
			}
		}

		if (auto q_time = qm.AllocQuery( batch->GetFrameId(), queue, EQueryType::Timestamp, 2 ))
		{
			dev.vkCmdWriteTimestamp2KHR( cmdbuf, BeginContextTypeToStage( type ), q_time.pool, q_time.first );
			pass.timestamp = q_time;
		}

		if ( not (pass.pplnStat or pass.timestamp) )
			return;	// failed to allocate

		pass.name	= String{taskName};
		pass.color	= color;

		auto&	f = _perFrame[ _writeIndex ];
		EXLOCK( f.guard );

		auto&	dst = f.activeCmdbufs[ BatchCmdbufKey{ batchPtr, cmdbuf, type }];

		dst.queue	= batch->GetQueueType();
		dst.ctxType	= type;
		dst.passes.push_back( RVRef(pass) );
	}

/*
=================================================
	EndContext
=================================================
*/
	void  GraphicsProfiler::EndContext (const void* batch, VkCommandBuffer cmdbuf, EContextType type) __NE___
	{
		if ( type == EContextType::Graphics )
			return;

		auto&	rts	= GraphicsScheduler();
		auto&	dev	= rts.GetDevice();
		Pass	pass;

		{
			auto&	f = _perFrame[ _writeIndex ];
			SHAREDLOCK( f.guard );

			auto	it = f.activeCmdbufs.find( BatchCmdbufKey{ batch, cmdbuf, type });
			if ( it != f.activeCmdbufs.end() )
			{
				ASSERT( it->second.ctxType == type );

				auto&	last = it->second.passes.back();
				CHECK( not last.recorded );

				last.recorded	= true;
				pass.pplnStat	= last.pplnStat;
				pass.timestamp	= last.timestamp;
			}
		}

		if ( pass.pplnStat )
		{
			dev.vkCmdEndQuery( cmdbuf, pass.pplnStat.pool, pass.pplnStat.first );
		}

		if ( pass.timestamp )
		{
			dev.vkCmdWriteTimestamp2KHR( cmdbuf, EndContextTypeToStage( type ), pass.timestamp.pool, pass.timestamp.first+1 );
		}
	}

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
/*
=================================================
	BeginContext
=================================================
*/
	void  GraphicsProfiler::BeginContext (OUT MetalSampleBufferAttachments &sampleBuffers, const void* batch, MetalCommandBuffer cmdbuf,
										  StringView taskName, RGBA8u color, EContextType type) __NE___
	{
		// TODO
	}

/*
=================================================
	EndContext
=================================================
*/
	void  GraphicsProfiler::EndContext (const void* batch, MetalCommandBuffer cmdbuf, EContextType type) __NE___
	{
		// TODO
	}

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_REMOTE_GRAPHICS
/*
=================================================
	BatchCmdbufKey::operator ==
=================================================
*/
	bool  GraphicsProfiler::BatchCmdbufKey::operator == (const BatchCmdbufKey &rhs) C_NE___
	{
		return	batch	== rhs.batch	and
				cmdbuf	== rhs.cmdbuf	and
				type	== rhs.type;
	}

/*
=================================================
	BatchCmdbufKey::CalcHash
=================================================
*/
	HashVal  GraphicsProfiler::BatchCmdbufKey::CalcHash () C_NE___
	{
		return HashOf(batch) + HashOf(cmdbuf) + HashOf(type);
	}

/*
=================================================
	BeginContextTypeToStage
=================================================
*/
	ND_ static EPipelineScope  BeginContextTypeToStage (EContextType type) __NE___
	{
		Unused( type );
		return EPipelineScope::All;
	}

/*
=================================================
	EndContextTypeToStage
=================================================
*/
	ND_ static EPipelineScope  EndContextTypeToStage (EContextType type) __NE___
	{
		Unused( type );
		return EPipelineScope::All;
	}

/*
=================================================
	BeginContext
=================================================
*/
	void  GraphicsProfiler::BeginContext (const void* batchPtr, void* cmdbuf, StringView taskName, RGBA8u color, EContextType type) __Th___
	{
		// not compatible with Metal, only single render pass can be measured.
		if ( type == EContextType::Graphics )
			return;

		ASSERT( not taskName.empty() );

		auto&		qm		= GraphicsScheduler().GetQueryManager();
		auto*		batch	= Cast<CommandBatch>(batchPtr);
		const auto	queue	= batch->GetQueueType();
		Pass		pass;

		if (auto q_time = qm.AllocQuery( queue, EQueryType::Timestamp, 2 ))
		{
			Cast<CmdBuf>(cmdbuf)->WriteTimestamp( q_time, 0, BeginContextTypeToStage( type ));
			pass.timestamp = q_time;
		}

		if ( type == EContextType::RenderPass )
		{
			ASSERT( not pass.pplnStat );
			if ( auto q_stat = qm.AllocQuery( queue, _pplnStats.hasMeshShader ? EQueryType::MeshPipelineStatistic : EQueryType::GraphicsPipelineStatistic ))
			{
				Cast<CmdBuf>(cmdbuf)->BeginQuery( q_stat, 0 );
				pass.pplnStat = q_stat;
			}
		}

		if ( type == EContextType::Compute )
		{
			ASSERT( not pass.pplnStat );
			if ( auto q_stat = qm.AllocQuery( queue, EQueryType::ComputePipelineStatistic ))
			{
				Cast<CmdBuf>(cmdbuf)->BeginQuery( q_stat, 0 );
				pass.pplnStat = q_stat;
			}
		}

		if ( not (pass.pplnStat or pass.timestamp) )
			return;	// failed to allocate

		pass.name	= String{taskName};
		pass.color	= color;

		auto&	f = _perFrame[ _writeIndex ];
		EXLOCK( f.guard );

		auto&	dst = f.activeCmdbufs[ BatchCmdbufKey{ batchPtr, Cast<CmdBuf>(cmdbuf), type }];

		dst.queue	= batch->GetQueueType();
		dst.ctxType	= type;
		dst.passes.push_back( RVRef(pass) );
	}

/*
=================================================
	EndContext
=================================================
*/
	void  GraphicsProfiler::EndContext (const void* batch, void* cmdbuf, EContextType type) __Th___
	{
		if ( type == EContextType::Graphics )
			return;

		Pass	pass;
		{
			auto&	f	= _perFrame[ _writeIndex ];
			SHAREDLOCK( f.guard );

			auto	it	= f.activeCmdbufs.find( BatchCmdbufKey{ batch, Cast<CmdBuf>(cmdbuf), type });
			if ( it != f.activeCmdbufs.end() )
			{
				ASSERT( it->second.ctxType == type );

				auto&	last = it->second.passes.back();
				CHECK( not last.recorded );

				last.recorded	= true;
				pass.pplnStat	= last.pplnStat;
				pass.timestamp	= last.timestamp;
			}
		}

		if ( pass.timestamp )
		{
			Cast<CmdBuf>(cmdbuf)->WriteTimestamp( pass.timestamp, 1, EndContextTypeToStage( type ));
		}
		if ( pass.pplnStat )
		{
			Cast<CmdBuf>(cmdbuf)->EndQuery( pass.pplnStat, 0 );
		}
	}

#endif // AE_ENABLE_REMOTE_GRAPHICS

} // AE::Profiler
