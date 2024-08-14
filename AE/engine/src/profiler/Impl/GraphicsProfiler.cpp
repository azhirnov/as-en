// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
#endif
#include "profiler/Impl/GraphicsProfiler.h"

namespace AE::Profiler
{
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
	{}

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

			// memory usage
			if ( _memUsage.has_value() )
			{
				const double	dev_usage_pct	= Max( 100.0 * double(ulong(_memUsage->deviceUsage))  / double(ulong(_memUsage->deviceAvailable  + _memUsage->deviceUsage)),  0.0 );
				const double	host_usage_pct	= Max( 100.0 * double(ulong(_memUsage->hostUsage))    / double(ulong(_memUsage->hostAvailable    + _memUsage->hostUsage)),    0.0 );
				const double	uni_usage_pct	= Max( 100.0 * double(ulong{_memUsage->unifiedUsage}) / double(ulong{_memUsage->unifiedAvailable + _memUsage->unifiedUsage}), 0.0 );

				str.clear();
				str << "mem:  dev(" << ToString( _memUsage->deviceUsage ) << ' ' << ToString( dev_usage_pct, 1 ) << "%)";

				if ( _memUsage->hostAvailable > 0 )
					str << "  host("  << ToString( _memUsage->hostUsage ) << ' ' << ToString( host_usage_pct, 1 ) << "%)";

				if ( _memUsage->unifiedAvailable > 0 )
					str << "  unified(" << ToString( _memUsage->unifiedUsage ) << ' ' << ToString( uni_usage_pct, 1 ) << "%)";

				ImGui::TextUnformatted( str.c_str() );
			}

			// memory traffic
			{
				str.clear();
				str << "to_dev: " << ToString( _memTraffic.avgWrite ) << "  to_host: " << ToString( _memTraffic.avgRead );
				ImGui::TextUnformatted( str.c_str() );
			}

			const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();
			const ImVec2	wnd_pos		= ImGui::GetCursorScreenPos();
			const RectF		max_region	= RectF{float2{ wnd_size.x, Abs(wnd_size.y) }} + float2{wnd_pos.x, wnd_pos.y};

			RectF	region1 = max_region;
			_imHistory.Draw( INOUT region1 );

			// TODO: batch graph
		}
		ImGui::End();
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
		}

		_memUsage = rts.GetDevice().GetMemoryUsage();

		// mem traffic
		{
			Bytes	write	= _memTraffic.accumWrite.exchange( 0_b );
			Bytes	read	= _memTraffic.accumRead.exchange( 0_b );

			_memTraffic.avgWrite	= Bytes{ulong(double(ulong{write}) / double(frame_count))};
			_memTraffic.avgRead		= Bytes{ulong(double(ulong{read}) / double(frame_count))};
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
	NextFrame
=================================================
*/
	void  GraphicsProfiler::NextFrame (FrameUID frameId) __NE___
	{
		auto&	rts = GraphicsScheduler();

		{
			auto&	qm	= rts.GetResourceManager().GetQueryManager();
			uint2	idx = qm.ReadAndWriteIndices();

			_writeIndex	= idx[1];
			_readIndex	= idx[0];
		}{
			auto	task = MakeRCNe< Threading::AsyncTaskFn >( [this]()
										{
										  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_PVRCOUNTER)
											if ( _pvrProfiler and _pvrProfiler->IsInitialized() )
												_ReadResultsPVR();
											else
										  #endif
												_ReadResults();
										},
										"GraphicsProfiler::ReadResults",
										ETaskQueue::PerFrame );
			if ( Scheduler().Run( task ))
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

		auto&	qm		= GraphicsScheduler().GetResourceManager().GetQueryManager();

		_imHistory.Begin();

		for (auto& [key, info] : f.activeCmdbufs)
		{
			for (auto& pass : info.passes)
			{
				ASSERT( pass.recorded );

				if ( pass.timestamp )
				{
					nanosecondsd	time[2] = {};
					Unused( qm.GetTimestamp( pass.timestamp, OUT time, Sizeof(time) ));

					ASSERT( time[0] <= time[1] );
					_gpuTime.min = Min( _gpuTime.min, time[0] );
					_gpuTime.max = Max( _gpuTime.max, time[1] );

					_imHistory.Add( pass.name, pass.color, time[0], time[1] );
				}
				/*if ( pass.pplnStat )
				{
					PipelineStatistic	stat = {};
					Unused( qm.GetPipelineStatistic( pass.pplnStat, OUT &stat, Sizeof(stat) ));
					// TODO
				}*/
			}
		}

		_gpuTime.min = Min( _gpuTime.min, _gpuTime.max );

		_imHistory.End( _gpuTime.min, _gpuTime.max );

		_fps.accumFrameTime.fetch_add( (_gpuTime.max - _gpuTime.min).count() );
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

		_gpuTime.min	= nanosecondsd{MaxValue<double>()};
		_gpuTime.max	= nanosecondsd{0.0};

		_imHistory.Begin();

		_pvrProfiler->ReadTimingData( OUT _pvrTimings );

		for (auto& t : _pvrTimings)
		{
			ASSERT( t.begin <= t.end );
			_gpuTime.min = Min( _gpuTime.min, t.begin );
			_gpuTime.max = Max( _gpuTime.max, t.end );

			StringView	name;
			RGBA8u		color;

			using EPass = PowerVRProfiler::EPass;
			switch_enum( t.pass )
			{
				case EPass::Compute :		name = "Compute";		color = HtmlColor::Yellow;	break;
				case EPass::TileAccel :		name = "TileAccel";		color = HtmlColor::Blue;	break;
				case EPass::TBDR :			name = "TBDR";			color = HtmlColor::Lime;	break;
				case EPass::Blit :			name = "Blit";			color = HtmlColor::Red;		break;
				case EPass::RayTracing :	name = "RayTracing";	color = HtmlColor::Violet;	break;
				case EPass::RTASBuild :		name = "RTASBuild";		color = HtmlColor::Pink;	break;
				case EPass::Unknown :		break;
			}
			switch_end

			_imHistory.Add( name, color, t.begin, t.end );
		}

		_gpuTime.min = Min( _gpuTime.min, _gpuTime.max );

		_imHistory.End( _gpuTime.min, _gpuTime.max );

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
		auto&		qm		= rts.GetResourceManager().GetQueryManager();
		auto*		batch	= Cast<VCommandBatch>(batchPtr);
		const auto	queue	= batch->GetQueueType();
		Pass		pass;

		/*if ( type == EContextType::RenderPass )
		{
			if ( auto q_stat = qm.AllocQuery( queue, EQueryType::PipelineStatistic ))
			{
				dev.vkCmdBeginQuery( cmdbuf, q_stat.pool, q_stat.first, 0 );
				pass.pplnStat = q_stat;
			}
		}*/

		if (auto q_time = qm.AllocQuery( batch->GetFrameId(), queue, EQueryType::Timestamp, 2 ))
		{
			dev.vkCmdWriteTimestamp2KHR( cmdbuf, BeginContextTypeToStage( type ), q_time.pool, q_time.first );
			pass.timestamp = q_time;
		}

		if ( //not pass.pplnStat	or
			 not pass.timestamp )
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
			auto&	f	= _perFrame[ _writeIndex ];
			SHAREDLOCK( f.guard );

			auto	it	= f.activeCmdbufs.find( BatchCmdbufKey{ batch, cmdbuf, type });
			if ( it != f.activeCmdbufs.end() )
			{
				ASSERT( it->second.ctxType == type );

				auto&	last = it->second.passes.back();
				CHECK( not last.recorded );

				last.recorded	= true;
				pass.timestamp	= last.timestamp;
			}
		}

		/*if ( pass.pplnStat )
		{
			dev.vkCmdEndQuery( cmdbuf, pass.pplnStat.pool, pass.pplnStat.first );
		}*/

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

		auto&		qm		= GraphicsScheduler().GetResourceManager().GetQueryManager();
		auto*		batch	= Cast<RCommandBatch>(batchPtr);
		const auto	queue	= batch->GetQueueType();
		Pass		pass;

		if (auto q_time = qm.AllocQuery( queue, EQueryType::Timestamp, 2 ))
		{
			Cast<CmdBuf>(cmdbuf)->WriteTimestamp( q_time, 0, BeginContextTypeToStage( type ));
			pass.timestamp = q_time;
		}

		if ( //not pass.pplnStat	or
			 not pass.timestamp )
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
				pass.timestamp	= last.timestamp;
			}
		}

		if ( pass.timestamp )
		{
			Cast<CmdBuf>(cmdbuf)->WriteTimestamp( pass.timestamp, 1, EndContextTypeToStage( type ));
		}
	}

#endif // AE_ENABLE_REMOTE_GRAPHICS

} // AE::Profiler
