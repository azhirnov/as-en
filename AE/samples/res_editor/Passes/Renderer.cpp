// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Renderer.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Core/EditorCore.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    Renderer::Renderer (uint seed) __Th___ :
        _lastUpdateTime{ TimePoint_t::clock::now() },
        _seed{ seed }
    {
        const auto& shader_trace_folder = ResEditorAppConfig::Get().shaderTraceFolder;

        if ( not shader_trace_folder.empty() )
        {
            CHECK_THROW( FileSystem::IsDirectory( shader_trace_folder ));
            _shaderDebugger.reset( new ShaderDebugger{});
        }
    }

/*
=================================================
    destructor
=================================================
*/
    Renderer::~Renderer ()
    {
        // remove global sliders
        UIInteraction::Instance().RemovePass( this );
    }

/*
=================================================
    GetInputMode
=================================================
*/
    InputModeName  Renderer::GetInputMode () const
    {
        return _controller ? _controller->GetInputMode() : Default;
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  Renderer::ProcessInput (ActionQueueReader reader, OUT bool &switchMode)
    {
        constexpr auto&     IA      = InputActions::SwitchInputMode;
        constexpr auto&     ui_IA   = InputActions::Main_UI;

        if ( _controller )
        {
            // validate time
            const secondsf  dt = Min( GraphicsScheduler().GetFrameTimeDelta(), secondsf{1.f/30.f} );

            _controller->ProcessInput( reader, dt );
        }

        auto    input = _input.WriteNoLock();
        EXLOCK( input );

        switchMode              = false;
        input->pressed          = false;
        input->customKeys[0]    = 0.f;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            StaticAssert( IA.actionCount == 9 );
            switch ( uint{hdr.name} )
            {
                // compatible with UI
                case ui_IA.UI_MousePos :
                    input->cursorPos = reader.Data<packed_float2>( hdr.offset );    break;

                case ui_IA.UI_MouseRBDown :
                    input->pressed = true;                                          break;

                // ui_IA.UI_ShowHide        - ignore
                // ui_IA.UI_Screenshot      - ignore
                // ui_IA.UI_ReloadScript    - ignore
                // ui_IA.FullscreenOnOff    - ignore


                // compatible with UI & controller
                case IA.SwitchInputMode :
                    switchMode = true;                                              break;

                case IA.PauseRendering :
                    input->pauseRendering = not input->pauseRendering;              break;

                case IA.CustomKey1 :
                    input->customKeys[0] = reader.Data<float>( hdr.offset );        break;

                case ui_IA.UI_ResExport :
                    _resExport.store( EExportState::Started );                      break;
            }
        }
    }

/*
=================================================
    Execute
=================================================
*/
    AsyncTask  Renderer::Execute (ArrayView<AsyncTask> inDeps)
    {
        if_unlikely( _input.ConstPtr()->pauseRendering )
        {
            return Scheduler().WaitAsync( ETaskQueue::Renderer, Tuple{inDeps} );
        }

        if_unlikely( _resExport.load() != EExportState::Completed )
            return _Export( inDeps );


        using EPass = IPass::EPassType;

        _UpdateDynSliders();

        auto&   rg      = RenderGraph();
        auto    batch   = rg.Render( "RenderPasses" );
        auto    ui_batch = rg.UI();

        CHECK_ERR( batch and ui_batch );

        AsyncTask               surf_acquire = rg.BeginOnSurface( ui_batch );
        Array<AsyncTask>        out_deps;
        PassArr_t               update_passes;
        PassArr_t               sync_passes;
        PassArr_t               present;
        IPass::UpdatePassData   update_pd;
        IPass::Debugger         pass_debugger;
        Array<RC<IResource>>    res_to_resize;
        Array<AsyncTask>        deps {inDeps};

        // update timers
        {
            constexpr auto  max_dt  = nanoseconds{seconds{1}} / 30;
            const auto      time    = TimePoint_t::clock::now();
            const auto      dt      = Min( time - _lastUpdateTime, max_dt );

            update_pd.frameTime = secondsf{dt};
            update_pd.totalTime = secondsf{_totalTime};
            update_pd.frameId   = _frameCounter;
            update_pd.seed      = _seed;

            _totalTime      += TimeCast<microseconds>( dt );
            _lastUpdateTime  = time;
            _frameCounter    ++;

            float2  surf_size   {1.f};
            float   pix_to_mm   = 1.f;
            if ( auto  surf = rg.GetSurface() ) {
                if ( auto  infos = surf->GetTargetInfo();  not infos.empty() ) {
                    surf_size   = infos[0].dimension;
                    pix_to_mm   = infos[0].pixToMm;
                }
            }

            auto    input = _input.ReadNoLock();
            SHAREDLOCK( input );

            update_pd.unormCursorPos= input->cursorPos / surf_size;
            update_pd.pressed       = input->pressed;
            update_pd.customKeys    = input->customKeys;
            update_pd.pixToMm       = pix_to_mm;
        }

        // setup shader debugger
        {
            auto    dbg = UIInteraction::Instance().debugger.WriteNoLock();
            EXLOCK( dbg );
            pass_debugger.debugger  = _shaderDebugger.get();
            pass_debugger.target    = dbg->target;
            pass_debugger.mode      = dbg->mode;
            pass_debugger.stage     = dbg->stage;
            pass_debugger.coord     = dbg->coord;
        }

        // which passes need to update
        for (auto& pass : _passes)
        {
            if ( AllBits( pass->GetType(), EPass::Update ))
                update_passes.push_back( pass );

            pass->GetResourcesToResize( INOUT res_to_resize );
        }

        // resize (recreate) resources
        if ( not res_to_resize.empty() )
        {
            auto    task = batch.Task( _ResizeRes( RVRef(res_to_resize) ),
                                       DebugLabel{ "ResizeRes", HtmlColor::BlueViolet })
                                    .Run( Tuple{inDeps} );
            deps.push_back( task );
        }
        ArrayView<AsyncTask>    deps_ref = deps;

        // upload resources
        {
            auto&   dtq = GetDataTransferQueue();

            Unused( dtq.Upload( batch, inDeps ));

            if ( _uploadInProgress and dtq.UploadFramesWithoutWork() > _minFramesWithoutWork )
            {
                _uploadInProgress = false;
                AE_LOGI( "Async loading completed" );
            }
        }

        const auto  RunSyncPasses = [&] ()
        {{
            if ( update_passes.size() | sync_passes.size() )
            {
                Unused( batch.Task( _SyncPasses( RVRef(update_passes), RVRef(sync_passes), pass_debugger, update_pd ),
                                    DebugLabel{ "SyncPasses", HtmlColor::Blue })
                                .Run( Tuple{deps_ref} ));
            }
        }};

        for (auto& pass : _passes)
        {
            if ( AnyBits( pass->GetType(), EPass::Present ))
                present.push_back( pass );
        }
        if ( not present.empty() and surf_acquire )
        {
            Array<AsyncTask>    surf_deps {deps_ref};
            surf_deps.push_back( surf_acquire );

            IPass::PresentPassData  pd;
            pd.batch    = rg.UI();
            pd.surface  = rg.GetSurface();
            pd._deps    = RVRef(surf_deps);

            CHECK_ERR( pd.surface );

            for (auto& pass : present) {
                out_deps.push_back( pass->PresentAsync( pd ));
            }
        }

        for (auto& pass : _passes)
        {
            for (auto t : BitfieldIterate( pass->GetType() ))
            {
                BEGIN_ENUM_CHECKS();
                switch ( t )
                {
                    case EPass::Sync :
                    {
                        sync_passes.push_back( pass );
                        break;
                    }

                    case EPass::Export :            // skip
                    case EPass::Update :
                    case EPass::Present :   break;  // already processed

                    case EPass::Async :
                    case EPass::Unknown :
                    default :               RETURN_ERR( "unknown pass type" );
                }
                END_ENUM_CHECKS();
            }
        }

        RunSyncPasses();

        out_deps.push_back( batch.SubmitAsTask() );


        // read shader debugger output
        if ( _shaderDebugger and _shaderDebugger->HasPendingRequests() )
        {
            out_deps.push_back( rg.UI().Task(
                    _ReadShaderTrace(),
                    {"Read shader debugger output"} )
                .Run( Tuple{deps_ref} ) );
        }

        return Scheduler().WaitAsync( ETaskQueue::Renderer, Tuple{ArrayView{out_deps}} );
    }

/*
=================================================
    _SyncPasses
=================================================
*/
    RenderTaskCoro  Renderer::_SyncPasses (PassArr_t updatePasses, PassArr_t passes, IPass::Debugger dbg, IPass::UpdatePassData updatePD) __Th___
    {
        auto&               rtask   = co_await RenderTask_GetRef;
        IPass::SyncPassData pd      { rtask };

        pd.dbg = dbg;

        // begin
        if ( not updatePasses.empty() )
        {
            DirectCtx::Transfer     ctx{ rtask };

            for (auto& pass : updatePasses) {
                CHECK_CE( pass->Update( ctx, updatePD ));
            }

            pd.cmdbuf = ctx.ReleaseCommandBuffer();
        }

        for (auto& pass : passes)
        {
            ASSERT( AllBits( pass->GetType(), IPass::EPassType::Sync ));
            CHECK_CE( pass->Execute( pd ));
        }

        // end
        {
            DirectCtx::Transfer     ctx{ rtask, RVRef(pd.cmdbuf) };
            co_await RenderTask_Execute{ ctx };
        }
    }

/*
=================================================
    _ResizeRes
=================================================
*/
    RenderTaskCoro  Renderer::_ResizeRes (Array<RC<IResource>> resources) __Th___
    {
        auto&                   rtask   = co_await RenderTask_GetRef;
        DirectCtx::Transfer     ctx     { rtask };

        for (auto& res : resources) {
            res->Resize( ctx );
        }

        co_await RenderTask_Execute{ ctx };
    }

/*
=================================================
    _Export
=================================================
*/
    AsyncTask  Renderer::_Export (ArrayView<AsyncTask> inDeps)
    {
        auto&   rg          = RenderGraph();
        auto    ui_batch    = rg.UI();
        CHECK_ERR( ui_batch );

        AsyncTask   surf_acquire = rg.BeginOnSurface( ui_batch );
        PassArr_t   export_passes;

        for (auto& pass : _passes)
        {
            if ( AllBits( pass->GetType(), IPass::EPassType::Export ))
                export_passes.push_back( pass );
        }

        if ( export_passes.empty() )
        {
            _resExport.store( EExportState::Completed );
            return null;
        }

        // readback resources
        {
            auto&   dtq = GetDataTransferQueue();

            Unused( dtq.Readback( ui_batch, inDeps ));
            /*
            if ( dtq.ReadbackFramesWithoutWork() > _minFramesWithoutWork )
            {
                _resExport.store( EExportState::Completed );
                AE_LOGI( "Async readback completed" );
            }*/
        }

        IPass::UpdatePassData   update_pd;
        update_pd.totalTime = secondsf{_totalTime};
        update_pd.frameId   = _frameCounter;
        update_pd.seed      = _seed;

        return ui_batch.Task( _ExportPasses( RVRef(export_passes), GetRC<Renderer>(), update_pd ))
                        .Run( Tuple{ inDeps, surf_acquire });
    }

/*
=================================================
    _ExportPasses
=================================================
*/
    RenderTaskCoro  Renderer::_ExportPasses (PassArr_t passes, RC<Renderer> self, IPass::UpdatePassData updatePD) __Th___
    {
        auto&                   rtask       = co_await RenderTask_GetRef;
        bool                    complete    = true;
        DirectCtx::Transfer     ctx         { rtask };

        for (auto& pass : passes)
        {
            ASSERT( AllBits( pass->GetType(), IPass::EPassType::Export ));
            complete &= pass->Update( ctx, updatePD );
        }

        if ( complete )
            self->_resExport.store( EExportState::Completed );

        co_await RenderTask_Execute{ ctx };
    }

/*
=================================================
    AddPass
=================================================
*/
    void  Renderer::AddPass (RC<IPass> pass) __Th___
    {
        CHECK_THROW( pass );
        _passes.emplace_back( RVRef(pass) );
    }

/*
=================================================
    SetController
=================================================
*/
    void  Renderer::SetController (RC<IController> cont) __Th___
    {
        CHECK_THROW( cont );
        CHECK_THROW( (not _controller) or (_controller == cont) );

        _controller = RVRef(cont);
    }

/*
=================================================
    SetDependencies
=================================================
*
    void  Renderer::SetDependencies (Array<Path> dependencies) __Th___
    {
        EXLOCK( _scriptFile.guard );

        for (auto& dep : dependencies) {
            _scriptFile.files.push_back( ScriptFile{ dep });
        }
        //RemoveDuplicates( INOUT _scriptFile.files, [](auto& lhs, auto& rhs) { return lhs.path < rhs.path; });

        for (auto& f : _scriptFile.files) {
            f.time = FileSystem::LastWriteTime( f.path );
        }

        _scriptFile.lastCheck = TimePoint_t::clock::now();
    }

/*
=================================================
    IsFileChanged
=================================================
*
    bool  Renderer::IsFileChanged ()
    {
        SHAREDLOCK( _scriptFile.guard );

        const auto  cur_time = TimePoint_t::clock::now();

        if ( secondsf{cur_time - _scriptFile.lastCheck} < _scriptFile.updateInterval )
            return false;

        _scriptFile.lastCheck = cur_time;

        for (auto& f : _scriptFile.files)
        {
            auto    t = FileSystem::LastWriteTime( f.path );

            if_unlikely( t != f.time )
                return true;
        }
        return false;
    }

/*
=================================================
    _ReadShaderTrace
=================================================
*/
    RenderTaskCoro  Renderer::_ReadShaderTrace () __Th___
    {
        auto&               rtask   = co_await RenderTask_GetRef;
        DirectCtx::Transfer ctx     {rtask};

        _shaderDebugger->ReadAll( ctx, ShaderDebugger::ELogFormat::VS )
            .Then( [this] (const Array<String> &output)
                    {
                        _PrintDbgTrace( output );
                    });

        co_await RenderTask_Execute( ctx );
    }

/*
=================================================
    _PrintDbgTrace
=================================================
*/
    void  Renderer::_PrintDbgTrace (const Array<String> &output) const
    {
        if ( output.empty() )
            return;

        const auto& shader_trace_folder = ResEditorAppConfig::Get().shaderTraceFolder;

        const auto  BuildName = [&shader_trace_folder] (OUT Path &fname, usize index)
        {{
            fname = shader_trace_folder / ("trace_"s << ToString(index) << ".glsl_dbg");
        }};

        usize       trace_idx   = 0;
        const auto  WriteToFile = [&output, &trace_idx] (const Path &fname) -> bool
        {{
            StringView      str     = output[trace_idx];
            FileWStream     file    {fname};
            if ( file.IsOpen() and file.Write( str ))
                AE_LOGI( "trace saved", ToString(fname), 1u );

            ++trace_idx;
            return not (trace_idx < output.size());
        }};

        FileSystem::FindUnusedFilename( BuildName, WriteToFile );

        AE_LOGI( "----" );
    }

/*
=================================================
    _UpdateDynSliders
=================================================
*/
    void  Renderer::_UpdateDynSliders ()
    {
        uint    i_idx   = 0;
        uint    f_idx   = 0;

        if ( auto p_sliders = UIInteraction::Instance().GetSliders( this ))
        {
            auto    sliders = p_sliders->ReadNoLock();
            SHAREDLOCK( sliders );

            for (auto& slider : _sliders)
            {
                Visit( slider,
                    [&] (RC<DynamicInt>  &dst)  { dst->Set( sliders->intSliders[i_idx].x );         ++i_idx; },
                    [&] (RC<DynamicInt2> &dst)  { dst->Set( int2(sliders->intSliders[i_idx]) );     ++i_idx; },
                    [&] (RC<DynamicInt3> &dst)  { dst->Set( int3(sliders->intSliders[i_idx]) );     ++i_idx; },
                    [&] (RC<DynamicInt4> &dst)  { dst->Set( sliders->intSliders[i_idx] );           ++i_idx; },

                    [&] (RC<DynamicUInt>  &dst) { dst->Set( uint(sliders->intSliders[i_idx].x) );   ++i_idx; },
                    [&] (RC<DynamicUInt2> &dst) { dst->Set( uint2(sliders->intSliders[i_idx]) );    ++i_idx; },
                    [&] (RC<DynamicUInt3> &dst) { dst->Set( uint3(sliders->intSliders[i_idx]) );    ++i_idx; },
                    [&] (RC<DynamicUInt4> &dst) { dst->Set( uint4(sliders->intSliders[i_idx]) );    ++i_idx; },

                    [&] (RC<DynamicFloat>  &dst){ dst->Set( sliders->floatSliders[f_idx].x );       ++f_idx; },
                    [&] (RC<DynamicFloat2> &dst){ dst->Set( float2(sliders->floatSliders[f_idx]) ); ++f_idx; },
                    [&] (RC<DynamicFloat3> &dst){ dst->Set( float3(sliders->floatSliders[f_idx]) ); ++f_idx; },
                    [&] (RC<DynamicFloat4> &dst){ dst->Set( sliders->floatSliders[f_idx] );         ++f_idx; }
                );
            }
        }
    }

/*
=================================================
    GetHelpText
=================================================
*/
    String  Renderer::GetHelpText () const
    {
        String  str;

        if ( _controller )
            str << _controller->GetHelpText();

        str << R"(
  '0..9' - set value to customKey[0] which can be used in shader)";
        return str;
    }


} // AE::ResEditor
