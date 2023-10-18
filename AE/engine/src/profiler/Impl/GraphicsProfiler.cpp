// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
#endif
#include "profiler/Impl/GraphicsProfiler.h"
#include "base/Algorithms/StringUtils.h"

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
    GraphicsProfiler::GraphicsProfiler (TimePoint_t startTime) :
        ProfilerUtils{startTime}
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

        const ImGuiWindowFlags  flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
      #else
        const ImGuiWindowFlags  flags = ImGuiWindowFlags_NoScrollbar;
      #endif

        if ( ImGui::Begin( "GraphicsProfiler", null, flags ))
        {
            String  str;

            // fps
            {
                str.clear();
                str << "FPS: " << ToString( _fps.result, 1 );
                str << ", dt: " << ToString( nanosecondsd{ _fps.dt });
                ImGui::TextUnformatted( str.c_str() );
            }

            // memory usage
            if ( _memInfo.has_value() )
            {
                const double    dev_usage_pct   = Max( 100.0 * double(ulong(_memInfo->deviceUsage))  / double(ulong(_memInfo->deviceAvailable)),  0.0 );
                const double    host_usage_pct  = Max( 100.0 * double(ulong(_memInfo->hostUsage))    / double(ulong(_memInfo->hostAvailable)),    0.0 );
                const double    uni_usage_pct   = Max( 100.0 * double(ulong{_memInfo->unifiedUsage}) / double(ulong{_memInfo->unifiedAvailable}), 0.0 );

                str.clear();
                str << "mem:  dev(" << ToString( _memInfo->deviceUsage ) << ' ' << ToString( dev_usage_pct, 1 ) << "%)";

                if ( _memInfo->hostAvailable > 0 )
                    str << "  host("  << ToString( _memInfo->hostUsage ) << ' ' << ToString( host_usage_pct, 1 ) << "%)";

                if ( _memInfo->unifiedAvailable > 0 )
                    str << "  unified(" << ToString( _memInfo->unifiedUsage ) << ' ' << ToString( uni_usage_pct, 1 ) << "%)";

                ImGui::TextUnformatted( str.c_str() );
            }

            const ImVec2    wnd_size    = ImGui::GetContentRegionAvail();
            const ImVec2    wnd_pos     = ImGui::GetCursorScreenPos();
            const RectF     max_region  = RectF{float2{ wnd_size.x, Abs(wnd_size.y) }} + float2{wnd_pos.x, wnd_pos.y};

            RectF   region1 = max_region;
            _imHistory.Draw( INOUT region1 );

            // TODO: batch graph

            ImGui::End();
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
        /*
        const float font_height_px  = 30.f;
        const float font_height_vp  = canvas.Dimensions().GetPixelsToViewport().x * font_height_px;

        canvas.DrawText( ("FPS: "s << ToString( _fps.result, 1 )), *renderer.font, font_height_px,
                         RectF{-1.0f, -1.0f, 1.0f, -1.0f + font_height_vp}, HtmlColor::White );

        canvas.DrawText( ("dt: "s << ToString( nanosecondsd{_gpuTime.max - _gpuTime.min })), *renderer.font, font_height_px,
                         RectF{-1.0f, -1.0f + font_height_vp * 1.1f, 1.0f, -1.0f + font_height_vp*2.2f}, HtmlColor::White );

        ctx.BindPipeline( renderer.fontPpln );
        ctx.BindDescriptorSet( 0, renderer.fontPpln_ds );
        canvas.Flush( ctx, EPrimitive::TriangleList );
        */
    }

/*
=================================================
    Update
=================================================
*/
    void  GraphicsProfiler::Update (secondsf dt)
    {
        // fps
        {
            uint    count       = _fps.frameCount.exchange( 0 );
            double  accum_time  = _fps.accumframeTime.exchange( 0.0 );

            _fps.result     = float(count) / dt.count();
            _fps.dt         = float(accum_time / count);
        }

        _memInfo = RenderTaskScheduler().GetDevice().GetMemoryUsage();
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
    RequestNextFrame
=================================================
*/
    void  GraphicsProfiler::RequestNextFrame (FrameUID frameId) __NE___
    {
        Unused( frameId );
    }

/*
=================================================
    NextFrame
=================================================
*/
    void  GraphicsProfiler::NextFrame (FrameUID) __NE___
    {
        auto&   rts = RenderTaskScheduler();
        auto&   qm  = rts.GetResourceManager().GetQueryManager();

        uint2   idx = qm.ReadAndWriteIndices();
        _writeIndex = idx[1];
        _readIndex  = idx[0];

        auto    task = MakeRC< Threading::AsyncTaskFn >( [this]() { _ReadResults(); }, "GraphicsProfiler::ReadResults", ETaskQueue::PerFrame );
        if ( Scheduler().Run( task ))
            rts.AddNextFrameDeps( task );

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
        auto&   f = _perFrame[_readIndex];
        EXLOCK( f.guard );

        _gpuTime.min    = 0.0;
        _gpuTime.max    = 0.0;

        if ( f.activeCmdbufs.empty() )
            return;

        _gpuTime.min = MaxValue<double>();

        auto&   qm  = RenderTaskScheduler().GetResourceManager().GetQueryManager();

        _imHistory.Begin();

        for (auto& [key, info] : f.activeCmdbufs)
        {
            for (auto& pass : info.passes)
            {
                ASSERT( pass.recorded );

                if ( pass.timestamp )
                {
                    double  time[2] = {};
                    Unused( qm.GetTimestamp( pass.timestamp, OUT time, Sizeof(time) ));

                    ASSERT( time[0] <= time[1] );
                    _gpuTime.min = Min( _gpuTime.min, time[0] );
                    _gpuTime.max = Max( _gpuTime.max, time[1] );

                    _imHistory.Add( pass.name, pass.color, time[0], time[1] );
                }
                /*if ( pass.pplnStat )
                {
                    PipelineStatistic   stat = {};
                    Unused( qm.GetPipelineStatistic( pass.pplnStat, OUT &stat, Sizeof(stat) ));
                    // TODO
                }*/
            }
        }

        _imHistory.End( _gpuTime.min, _gpuTime.max );

        _fps.accumframeTime.fetch_add( _gpuTime.max - _gpuTime.min );
    }
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
    BatchCmdbufKey::operator ==
=================================================
*/
    bool  GraphicsProfiler::BatchCmdbufKey::operator == (const BatchCmdbufKey &rhs) const
    {
        return  batch   == rhs.batch    and
                cmdbuf  == rhs.cmdbuf   and
                type    == rhs.type;
    }

/*
=================================================
    BatchCmdbufKey::CalcHash
=================================================
*/
    HashVal  GraphicsProfiler::BatchCmdbufKey::CalcHash () const
    {
        return HashOf(batch) + HashOf(cmdbuf) + HashOf(type);
    }

/*
=================================================
    BeginContextTypeToStage
=================================================
*/
    ND_ static VkPipelineStageFlagBits2  BeginContextTypeToStage (EContextType type)
    {
    #if 1
        Unused( type );
        return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    #else
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EContextType::Transfer :       return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            case EContextType::Compute :        return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            case EContextType::RenderPass :     return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            case EContextType::ASBuild :        return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            case EContextType::RayTracing :     return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
            case EContextType::VideoDecode :    return VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR;
            case EContextType::VideoEncode :    return VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR;
            case EContextType::Graphics :       break;
            case EContextType::Unknown :        break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown context type" );
    #endif
    }

/*
=================================================
    EndContextTypeToStage
=================================================
*/
    ND_ static VkPipelineStageFlagBits2  EndContextTypeToStage (EContextType type)
    {
    #if 1
        Unused( type );
        return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    #else
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EContextType::Transfer :       return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            case EContextType::Compute :        return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            case EContextType::RenderPass :     return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
            case EContextType::ASBuild :        return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            case EContextType::RayTracing :     return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
            case EContextType::VideoDecode :    return VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR;
            case EContextType::VideoEncode :    return VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR;
            case EContextType::Graphics :       break;
            case EContextType::Unknown :        break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown context type" );
    #endif
    }

/*
=================================================
    BeginContext
=================================================
*/
    void  GraphicsProfiler::BeginContext (const void* batch, VkCommandBuffer cmdbuf, StringView taskName, RGBA8u color, EContextType type) __NE___
    {
        // not compatible with Metal, only single render pass can be measured.
        if ( type == EContextType::Graphics )
            return;

        ASSERT( not taskName.empty() );

        auto&       rts     = RenderTaskScheduler();
        auto&       dev     = rts.GetDevice();
        auto&       qm      = rts.GetResourceManager().GetQueryManager();
        auto*       vbatch  = Cast<VCommandBatch>(batch);
        const auto  queue   = vbatch->GetQueueType();
        Pass        pass;

        /*if ( type == EContextType::RenderPass )
        {
            if ( auto q_stat = qm.AllocQuery( queue, EQueryType::PipelineStatistic ))
            {
                dev.vkCmdBeginQuery( cmdbuf, q_stat.pool, q_stat.first, 0 );
                pass.pplnStat = q_stat;
            }
        }*/

        if (auto q_time = qm.AllocQuery( queue, EQueryType::Timestamp, 2 ))
        {
            dev.vkCmdWriteTimestamp2KHR( cmdbuf, BeginContextTypeToStage( type ), q_time.pool, q_time.first );
            pass.timestamp = q_time;
        }

        if ( //not pass.pplnStat    or
             not pass.timestamp )
            return; // failed to allocate

        pass.name   = String{taskName};
        pass.color  = color;

        auto&   f = _perFrame[ _writeIndex ];
        EXLOCK( f.guard );

        auto&   dst = f.activeCmdbufs[ BatchCmdbufKey{ batch, cmdbuf, type }];

        dst.queue   = vbatch->GetQueueType();
        dst.ctxType = type;
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

        auto&   rts = RenderTaskScheduler();
        auto&   dev = rts.GetDevice();
        Pass    pass;

        {
            auto&   f   = _perFrame[ _writeIndex ];
            SHAREDLOCK( f.guard );

            auto    it  = f.activeCmdbufs.find( BatchCmdbufKey{ batch, cmdbuf, type });
            if ( it != f.activeCmdbufs.end() )
            {
                ASSERT( it->second.ctxType == type );

                auto&   last = it->second.passes.back();
                CHECK( not last.recorded );

                last.recorded   = true;
                pass.timestamp  = last.timestamp;
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

} // AE::Profiler
