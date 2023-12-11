// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ShaderDebugger.h"
#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Controllers/IController.h"
#include "res_editor/Dynamic/DynamicVec.h"
#include "res_editor/Resources/ResourceArray.h"

namespace AE::ResEditor
{

    //
    // Pass interface
    //

    class IPass : public EnableRC< IPass >
    {
        friend class ScriptBasePass;

    // types
    public:
        using TransferCtx_t = DirectCtx::Transfer;
        using CustomKeys_t  = StaticArray< float, 1 >;

        enum class EPassType : uint
        {
            Unknown             = 0,
            Sync                = 1 << 0,
            Async               = 1 << 1,   // for heavy tasks
            Present             = 1 << 2,
            //SeparateBatch     = 1 << 3,   // for async compute
            Update              = 1 << 16,
            Export              = 1 << 17,  // will pause rendering
        };

        enum class EDebugMode : uint
        {
            Unknown         = 0,
            Trace,
            FnProfiling,        // function profiling
            TimeHeatMap,        // profiling for the whole pass
            _Count
        };

        struct Debugger
        {
            void const*                 target      = null;
            EDebugMode                  mode        = Default;
            EShaderStages               stage       = Default;
            float2                      coord;
            Ptr<ShaderDebugger>         debugger;

            ND_ bool  IsEnabled ()                  const   { return debugger and mode != Default; }
            ND_ bool  IsEnabled (const IPass* pass) const   { return IsEnabled() and (pass == target or usize(target) == 0x1); }
        };

        struct SyncPassData
        {
            RenderTask const&           rtask;
            DirectCtx::CommandBuffer    cmdbuf;
            Debugger                    dbg;

            explicit SyncPassData (RenderTask const& rtask) : rtask{rtask} {}
        };

        struct AsyncPassData
        {
            RG::CommandBatchPtr         batch;
            Debugger                    dbg;
            ActionQueueReader           reader;
            Array<AsyncTask>            _deps;

            ND_ ArrayView<AsyncTask>  DepsRef ()    const { return _deps; }
        };

        struct PresentPassData
        {
            RG::CommandBatchPtr         batch;
            Ptr<IOutputSurface>         surface;
            Array<AsyncTask>            _deps;

            ND_ ArrayView<AsyncTask>  DepsRef ()    const { return _deps; }
        };

        struct UpdatePassData
        {
            secondsf        totalTime;
            secondsf        frameTime;
            uint            frameId         = 0;
            uint            seed            = 0;

            float2          unormCursorPos;
            float           pixToMm         = 1.f;      // for current screen
            bool            pressed         = false;    // mouse down or touch pressed
            CustomKeys_t    customKeys      = {};
        };


    protected:
        using PerFrameDescSet_t = StaticArray< Strong<DescriptorSetID>, GraphicsConfig::MaxFrames >;

        using ClearValue_t      = RenderPassDesc::ClearValue_t;

        struct RenderTarget
        {
            AttachmentName  name;
            RC<Image>       image;
            ClearValue_t    clear;

            ND_ bool  HasClearValue ()  C_NE___ { return not IsNullUnion( clear ); }
        };

        using RenderTargets_t   = Array< RenderTarget >;


    public:
        using DynamicFloatTypes_t   = TypeList< DynamicFloat, DynamicFloat2, DynamicFloat3, DynamicFloat4 >;
        using DynamicIntTypes_t     = TypeList< DynamicInt, DynamicInt2, DynamicInt3, DynamicInt4,
                                                DynamicUInt, DynamicUInt2, DynamicUInt3, DynamicUInt4 >;

        struct Constants
        {
            static constexpr uint   MaxCount = 4;

            StaticArray< RC<>, MaxCount >   f;
            StaticArray< RC<>, MaxCount >   i;
        };


    // variables
    protected:
        RC<IController>         _controller;
        Constants               _shConst;

        String                  _dbgName;
        RGBA8u                  _dbgColor;

        struct {
            RC<DynamicUInt>     dynamic;
            uint                ref         = 0;
            ECompareOp          op          = ECompareOp::Always;
        }                   _enablePass;


    // interface
    public:
        ~IPass () override;


        ND_ StringView          GetName ()                                                  C_NE___ { return _dbgName; }
        ND_ RC<IController>     GetController ()                                            C_NE___ { return _controller; }

        ND_ virtual EPassType   GetType ()                                                  C_NE___ = 0;

        // EPassType::Update    - returns 'false' on error
        // EPassType::Export    - returns 'false' if not complete
        ND_ virtual bool        Update (TransferCtx_t &, const UpdatePassData &)            __Th___ { DBG_WARNING("Update");  return false; }

        // EPassType::Sync
        ND_ virtual bool        Execute (SyncPassData &)                                    __Th___ { DBG_WARNING("Execute");  return false; }

        // EPassType::Async
        ND_ virtual AsyncTask   ExecuteAsync (const AsyncPassData &)                        __Th___ { DBG_WARNING("ExecuteAsync");  return null; }

        // EPassType::Present
        ND_ virtual AsyncTask   PresentAsync (const PresentPassData &)                      __Th___ { DBG_WARNING("PresentAsync");  return null; }

        // EPassType::SeparateBatch
        //ND_ virtual Tuple< CommandBatchPtr, AsyncTask >  SeparateBatch (const PassData &) { DBG_WARNING("SeparateBatch");  return Default; }

        ND_ virtual void        GetResourcesToResize (INOUT Array<RC<IResource>> &)         __NE___ = 0;


    protected:
        IPass ();
        explicit IPass (StringView dbgName, RGBA8u dbgColor = Default);

        void  _CopySliders (OUT StaticArray<float4,4> &, OUT StaticArray<int4,4> &,
                            OUT StaticArray<float4,4> &)                                    const;
        void  _CopyConstants (const Constants &, OUT StaticArray<float4,4> &,
                              OUT StaticArray<int4,4> &)                                    const;

        ND_ bool  _IsEnabled ()                                                             const;
    };

    AE_BIT_OPERATORS( IPass::EPassType );


} // AE::ResEditor
