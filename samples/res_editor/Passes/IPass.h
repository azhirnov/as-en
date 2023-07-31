// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ShaderDebugger.h"
#include "res_editor/Passes/FrameGraph.h"
#include "res_editor/Controllers/IController.h"
#include "res_editor/Dynamic/DynamicVec.h"
#include "res_editor/Resources/IResource.h"

namespace AE::ResEditor
{

    //
    // Pass interface
    //

    class IPass : public EnableRC< IPass >
    {
    // types
    public:
        using TransferCtx_t = RG::DirectCtx::Transfer;
        using CustomKeys_t  = StaticArray< float, 1 >;

        enum class EPassType : uint
        {
            Unknown             = 0,
            Sync                = 1 << 0,
            Async               = 1 << 1,   // for heavy tasks
            Present             = 1 << 2,
            //SeparateBatch     = 1 << 3,   // for async compute
            Update              = 1 << 16,
            //Input             = 1 << 17,
        };

        enum class EDebugMode : uint
        {
            Unknown         = 0,
            Trace,
            FnProfiling,        // function profiling
            TimeHeatMap,        // per fragment/thread profiling
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
            ND_ bool  IsEnabled (const IPass *pass) const   { return IsEnabled() and (pass == target or usize(target) == 0x1); }
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
            ArrayView<AsyncTask>        deps;
            ActionQueueReader           reader;
        };

        struct PresentPassData
        {
            RG::CommandBatchPtr         batch;
            Ptr<IOutputSurface>         surface;
            ArrayView<AsyncTask>        deps;
        };

        struct UpdatePassData
        {
            secondsf        totalTime;
            secondsf        frameTime;
            uint            frameId     = 0;
            uint            seed        = 0;

            float2          cursorPos;              // unorm
            bool            pressed     = false;    // mouse down or touch pressed
            CustomKeys_t    customKeys  = {};
        };


    protected:
        using AnyResource_t     = Union< NullUnion, RC<Buffer>, RC<RTScene>, RC<Image>, RC<VideoImage> >;
        using Resources_t       = Array<Tuple< UniformName, AnyResource_t, EResourceState >>;
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
        struct Constants
        {
            static constexpr uint   MaxCount = 4;

            StaticArray< RC<DynamicFloat4>, MaxCount >  f;
            StaticArray< RC<DynamicInt4>,   MaxCount >  i;
        };


    // interface
    public:
        ~IPass () override;


        ND_ virtual EPassType   GetType ()                                                  C_NE___ = 0;
        ND_ virtual StringView  GetName ()                                                  C_NE___ = 0;

        ND_ virtual RC<IController> GetController ()                                        C_NE___ { return null; }

        ND_ virtual bool        Update (TransferCtx_t &, const UpdatePassData &)            __NE___ { DBG_WARNING("Update");  return false; }

        // EPassType::Sync
        ND_ virtual bool        Execute (SyncPassData &)                                    __NE___ { DBG_WARNING("Execute");  return false; }

        // EPassType::Async
        ND_ virtual AsyncTask   ExecuteAsync (const AsyncPassData &)                        __NE___ { DBG_WARNING("ExecuteAsync");  return null; }

        // EPassType::Present
        ND_ virtual AsyncTask   PresentAsync (const PresentPassData &)                      __NE___ { DBG_WARNING("PresentAsync");  return null; }

        // EPassType::SeparateBatch
        //ND_ virtual Tuple< CommandBatchPtr, AsyncTask >  SeparateBatch (const PassData &) { DBG_WARNING("SeparateBatch");  return Default; }


    protected:
        void  _CopySliders (OUT StaticArray<float4,4> &, OUT StaticArray<int4,4> &, OUT StaticArray<float4,4> &)    const;
        void  _CopyConstants (const Constants &, OUT StaticArray<float4,4> &, OUT StaticArray<int4,4> &)            const;

        template <typename CtxType>
        static void  _SetResStates (FrameUID, CtxType &ctx, const Resources_t &);

        template <typename CtxType>
        static void  _ResizeRes (CtxType &ctx, Resources_t &);

        ND_ static bool  _BindRes (FrameUID, DescriptorUpdater &updater, Resources_t &);
    };

    AE_BIT_OPERATORS( IPass::EPassType );


} // AE::ResEditor
