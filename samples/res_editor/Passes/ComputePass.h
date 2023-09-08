// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/IResource.h"

namespace AE::ResEditor
{

    //
    // Compute Pass
    //

    class ComputePass final : public IPass
    {
        friend class ScriptComputePass;

    // types
    private:
        using PipelineMap_t     = FixedMap< EDebugMode, ComputePipelineID, uint(EDebugMode::_Count) >;

        struct DynamicData
        {
            int  frame  = 0;
        };

    public:
        using IterationCount_t  = Union< uint3, RC<DynamicDim>, RC<DynamicUInt>, RC<DynamicUInt2>, RC<DynamicUInt3> >;

        struct Iteration
        {
            IterationCount_t    count;
            bool                isGroups    = true;     // groups / threads
            RC<Buffer>          indirect;
            Bytes               indirectOffset;

            ND_ uint3  ThreadCount (const uint3 &localSize) const;
            ND_ uint3  GroupCount (const uint3 &localSize) const;

            ND_ static uint3  FindMaxConstThreadCount (ArrayView<Iteration>, const uint3 &localSize);
        };
        using Iterations_t  = Array< Iteration >;


    // variables
    private:
        RTechInfo               _rtech;

        PipelineMap_t           _pipelines;
        PerFrameDescSet_t       _descSets;
        DescSetBinding          _dsIndex;

        Strong<BufferID>        _ubuffer;
        mutable DynamicData     _dynData;       // used only in 'Upload()'
        RC<IController>         _controller;
        Constants               _shConst;

        ResourceArray           _resources;
        Iterations_t            _iterations;
        uint3                   _localSize;

        String                  _dbgName;
        RGBA8u                  _dbgColor;


    // methods
    public:
        ComputePass () {}
        ~ComputePass ();

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return _controller; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV;
    };


} // AE::ResEditor
