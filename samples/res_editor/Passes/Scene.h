// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/Postprocess.h"
#include "res_editor/GeomSource/IGeomSource.h"

namespace AE::ResEditor
{

    //
    // Scene Data
    //
    class SceneData final : public EnableRC< SceneData >
    {
        friend class SceneGraphicsPass;
        friend class SceneRayTracingPass;
        friend class ScriptScene;

    // types
    private:
        struct GeometryInstance
        {
            RC<IGeomSource>     geometry;
            float3              position;
        };
        using GeomInstances_t = Array< GeometryInstance >;


    // variables
    private:
        GeomInstances_t     _geomInstances;


    // methods
    public:
    };



    //
    // Scene Graphics Pass
    //
    class SceneGraphicsPass final : public IPass
    {
        friend class ScriptSceneGraphicsPass;

    // types
    private:
        using Materials_t   = Array< RC<IGSMaterials> >;


    // variables
    private:
        RTechInfo               _rtech;

        RC<SceneData>           _scene;
        Materials_t             _materials;

        RenderPassDesc          _rpDesc;
        ResourceArray           _resources;         // per pass
        RenderTargets_t         _renderTargets;

        Strong<BufferID>        _ubuffer;
        PerFrameDescSet_t       _descSets;
        DescSetBinding          _dsIndex;

        RC<IController>         _controller;
        Constants               _shConst;

        String                  _dbgName;
        RGBA8u                  _dbgColor;


    // methods
    public:
        SceneGraphicsPass () {}
        ~SceneGraphicsPass ();

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return _controller; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV;
    };



    //
    // Scene Ray Tracing Pass
    //
    class SceneRayTracingPass final : public IPass
    {
        friend class ScriptSceneRayTracingPass;

    // types
    private:
        using Materials_t       = Array< RC<IGSMaterials> >;
        using PipelineMap_t     = FixedMap< EDebugMode, RayTracingPipelineID, uint(EDebugMode::_Count) >;


    // variables
    private:
        RTechInfo               _rtech;

        RC<SceneData>           _scene;
        Materials_t             _materials;

        ResourceArray           _resources;         // per pass

        PipelineMap_t           _pipelines;
        RTShaderBindingID       _sbt;

        Strong<BufferID>        _ubuffer;
        PerFrameDescSet_t       _descSets;
        DescSetBinding          _dsIndex;

        RC<IController>         _controller;
        Constants               _shConst;

        String                  _dbgName;
        RGBA8u                  _dbgColor;


    // methods
    public:
        SceneRayTracingPass () {}
        ~SceneRayTracingPass ();

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return _controller; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV;
    };


} // AE::ResEditor
