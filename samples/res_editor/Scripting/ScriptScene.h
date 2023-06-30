// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBaseRenderPass.h"
#include "res_editor/Passes/Scene.h"

namespace AE::ResEditor
{

    //
    // Scene
    //
    class ScriptScene final : public EnableScriptRC
    {
    // types
    private:
        struct GeometryInstance
        {
            ScriptGeomSourcePtr     geom;
            float3                  pos;
        };
        using GeomInstances_t = Array< GeometryInstance >;


    // variables
    public:
        ScriptBaseControllerPtr     _controller;
        GeomInstances_t             _geomInstances;
        uint                        _passCount  = 0;

        RC<SceneData>               _scene;


    // methods
    public:
        ScriptScene () {}

        void  SetDebugName (const String &name)                                             __Th___;

        // default controller, can be overrided by pass
        void  InputController (const ScriptBaseControllerPtr &)                             __Th___;

        void  InputGeometry1 (const ScriptGeomSourcePtr &, const packed_float3 &pos)        __Th___;
        void  InputGeometry2 (const ScriptGeomSourcePtr &)                                  __Th___;

        ScriptSceneGraphicsPass*    AddGraphicsPass (const String &name)                    __Th___;
        ScriptSceneRayTracingPass*  AddRayTracingPass (const String &name)                  __Th___;

        ND_ RC<SceneData>            ToScene ()                                             __Th___;
        ND_ ScriptBaseControllerPtr  GetController ()                                       __Th___ { return _controller; }

        static void  Bind (const ScriptEnginePtr &se)                                       __Th___;
    };



    //
    // Scene Graphics Pass
    //
    class ScriptSceneGraphicsPass final : public ScriptBaseRenderPass
    {
        friend class ScriptScene;

    // types
    private:
        using PipelinePathes_t      = Array< Path >;
        using UniquePipelines_t     = FlatHashSet< Path >;
        using PipelineNames_t       = Array< Array< PipelineName >>;


    // variables
    public:
        ScriptScenePtr              _scene;
        ScriptBaseControllerPtr     _controller;
        const String                _passName;

        PipelinePathes_t            _pipelines;
        UniquePipelines_t           _uniquePplns;


    // methods
    private:
        ScriptSceneGraphicsPass (ScriptScenePtr scene, const String &passName)              __Th___;

    public:
        ScriptSceneGraphicsPass () : ScriptBaseRenderPass{Default} {}

        void  AddPipeline (const String &pplnFile)                                          __Th___;
        void  AddPipelines (const String &pplnsFolder)                                      __Th___;

        void  InputController (const ScriptBaseControllerPtr &)                             __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                       __Th___;
        static void  GetShaderTypes (INOUT CppStructsFromShaders &)                         __Th___;

    // ScriptBasePass //
        RC<IPass>  ToPass ()                                                                C_Th_OV;


    private:
        ND_ RTechInfo   _CompilePipelines (OUT PipelineNames_t &)                           C_Th___;
            void        _CompilePipelines2 (ScriptEnginePtr se, OUT PipelineNames_t &)      C_Th___;

        ND_ static auto  _CreateUBType ()                                                   __Th___;
    };



    //
    // Scene Graphics Pass
    //
    class ScriptSceneRayTracingPass final : public ScriptBaseRenderPass
    {
        friend class ScriptScene;

    // variables
    public:
        ScriptScenePtr              _scene;
        ScriptBaseControllerPtr     _controller;
        const String                _passName;
        Path                        _pplnName;


    // methods
    private:
        ScriptSceneRayTracingPass (ScriptScenePtr scene, const String &passName)            __Th___;

    public:
        ScriptSceneRayTracingPass () : ScriptBaseRenderPass{Default} {}

        void  SetPipeline (const String &pplnFile)                                          __Th___;

        void  InputController (const ScriptBaseControllerPtr &)                             __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                       __Th___;
        static void  GetShaderTypes (INOUT CppStructsFromShaders &)                         __Th___;

    // ScriptBasePass //
        RC<IPass>  ToPass ()                                                                C_Th_OV;


    private:
        ND_ static auto  _CreateUBType ()                                                   __Th___;
    };


} // AE::ResEditor
