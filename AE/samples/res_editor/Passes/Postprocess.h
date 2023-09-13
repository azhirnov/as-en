// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

    //
    // Post Process
    //

    class Postprocess final : public IPass
    {
        friend class ScriptPostprocess;

    // types
    protected:
        using PipelineMap_t = FixedMap< EDebugMode, GraphicsPipelineID, uint(EDebugMode::_Count) >;

        struct DynamicData
        {
            int  frame  = 0;
        };


    // variables
    protected:
        RTechInfo               _rtech;
        RenderPassDesc          _rpDesc;
        float2                  _depthRange     {0.f, 1.f};

        PipelineMap_t           _pipelines;
        PerFrameDescSet_t       _descSets;
        DescSetBinding          _dsIndex;

        Strong<BufferID>        _ubuffer;
        mutable DynamicData     _dynData;       // used only in 'Upload()'
        RC<IController>         _controller;
        Constants               _shConst;

        ResourceArray           _resources;
        RenderTargets_t         _renderTargets;

        String                  _dbgName;
        RGBA8u                  _dbgColor;


    // methods
    public:
        Postprocess () {}
        ~Postprocess ();

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return _controller; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV;
    };


} // AE::ResEditor
