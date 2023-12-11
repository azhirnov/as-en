// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Common.h"

namespace AE::ResEditor
{

    //
    // Render Graph
    //

    class RenderGraphImpl
    {
    // variables
    private:
        RG::RenderGraph &           _rg;
        Ptr<IOutputSurface>         _surface;
        RG::CommandBatchPtr         _uiBatch;

        DRC_ONLY( RWDataRaceCheck   _drCheck;)


    // methods
    public:
        RenderGraphImpl ()  __Th___;
        ~RenderGraphImpl ();

        // single threaded
        ND_ bool        BeginFrame (Ptr<IOutputSurface>     surface,
                                    const BeginFrameConfig  &cfg    = Default);

        template <typename ...Deps>
        ND_ AsyncTask   EndFrame (const Tuple<Deps...>  &deps);

        ND_ AsyncTask   BeginOnSurface (const RG::CommandBatchPtr &batch);


        ND_ FrameUID    GetPrevFrameId ()   const   { return _rg.GetPrevFrameId(); }
        ND_ FrameUID    GetNextFrameId ()   const   { return _rg.GetNextFrameId(); }
        ND_ auto        GetSurface ()       const   { DRC_SHAREDLOCK( _drCheck );  return _surface; }
        ND_ auto        GetSurfaceArg ()    const   { DRC_SHAREDLOCK( _drCheck );  return ArgRef( *_surface ); }


        // thread safe
    //  ND_ RG::CommandBatchPtr     Upload (EQueueType queue = EQueueType::Graphics);
    //  ND_ RG::CommandBatchPtr     Readback (EQueueType queue = EQueueType::Graphics);
        ND_ RG::CommandBatchPtr     Render (StringView name);
        ND_ RG::CommandBatchPtr     UI ();

        ND_ RG::ResStateTracker&    GetStateTracker ()  { return _rg; }


        friend RenderGraphImpl&  RenderGraph ();
    };


    ND_ RenderGraphImpl&  RenderGraph ();


/*
=================================================
    BeginFrame
=================================================
*/
    inline bool  RenderGraphImpl::BeginFrame (Ptr<IOutputSurface> surface, const BeginFrameConfig &cfg)
    {
        DRC_EXLOCK( _drCheck );

        _surface = surface;

        return _rg.BeginFrame( cfg );
    }

/*
=================================================
    EndFrame
=================================================
*/
    template <typename ...Deps>
    AsyncTask  RenderGraphImpl::EndFrame (const Tuple<Deps...> &deps)
    {
        DRC_EXLOCK( _drCheck );

        AsyncTask   ui_task;
        if ( _uiBatch )
            ui_task = _uiBatch.SubmitAsTask();

        AsyncTask   end_frame = _rg.EndFrame( TupleConcat( deps, Tuple{ui_task} ));

        _surface    = null;
        _uiBatch    = null;

        return end_frame;
    }

/*
=================================================
    BeginOnSurface
=================================================
*/
    inline AsyncTask  RenderGraphImpl::BeginOnSurface (const RG::CommandBatchPtr &batch)
    {
        DRC_EXLOCK( _drCheck );

        if ( not _surface ) return null;

        return _rg.BeginOnSurface( _surface, batch );
    }

/*
=================================================
    Render
=================================================
*/
    inline RG::CommandBatchPtr  RenderGraphImpl::Render (StringView name)
    {
        DRC_SHAREDLOCK( _drCheck );
        auto    batch = _rg.CmdBatch( EQueueType::Graphics, {name} )
                                .UploadMemory()
                                .Begin();
        batch.AsBatch()->SetSubmissionMode( ESubmitMode::Immediately ); // TODO: use deferred
        return batch;
    }

/*
=================================================
    UI
=================================================
*/
    inline RG::CommandBatchPtr  RenderGraphImpl::UI ()
    {
        DRC_SHAREDLOCK( _drCheck );
        if ( not _uiBatch )
        {
            _uiBatch = _rg.CmdBatch( EQueueType::Graphics, {"UI"} )
                                .UploadMemory()
                                .Begin();
        }
        return _uiBatch;
    }


} // AE::ResEditor
