// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Scripting/ScriptPassArgs.h"

#include "res_editor/GeomSource/IGeomSource.h"

namespace AE::ResLoader {
    class IntermScene;
    class IntermVertexAttribs;
}

namespace AE::ResEditor
{

    //
    // Base Geometry Source
    //
    class ScriptGeomSource : public EnableScriptRC
    {
    // types
    public:
        using PipelineNames_t       = Array< PipelineName >;
        using CppStructsFromShaders = ScriptBasePass::CppStructsFromShaders;


    // variables
    protected:
        ScriptPassArgs      _args;


    // methods
    public:
        ScriptGeomSource ();

        // resources
        void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)                            __Th___ { _args.ArgSceneIn( name, scene ); }

        void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)                              __Th___ { _args.ArgBufferIn( name, buf ); }
        void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)                             __Th___ { _args.ArgBufferOut( name, buf ); }
        void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)                           __Th___ { _args.ArgBufferInOut( name, buf ); }

        void  ArgImageIn (const String &name, const ScriptImagePtr &img)                                __Th___ { _args.ArgImageIn( name, img ); }
        void  ArgImageOut (const String &name, const ScriptImagePtr &img)                               __Th___ { _args.ArgImageOut( name, img ); }
        void  ArgImageInOut (const String &name, const ScriptImagePtr &img)                             __Th___ { _args.ArgImageInOut( name, img ); }

        void  ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName)   __Th___ { _args.ArgTextureIn( name, tex, samplerName ); }
        void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)__Th___ { _args.ArgVideoIn( name, tex, samplerName ); }

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;

        ND_ virtual RC<IGeomSource>     ToGeomSource ()                                                 __Th___ = 0;
        ND_ virtual PipelineNames_t     FindMaterialPipeline ()                                         C_Th___ = 0;
            virtual void                AddLayoutReflection ()                                          C_Th___ { _args.AddLayoutReflection(); }
        ND_ virtual RC<IGSMaterials>    ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)    C_Th___ = 0;


    protected:
        template <typename B>
        static void  _BindBase (B &binder)                                                              __Th___;

        virtual void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)                                   C_Th___ = 0;
    };



    //
    // Spherical Cube Geometry Source
    //
    class ScriptSphericalCube final : public ScriptGeomSource
    {
    // variables
    private:
        uint                _minLod     = 0;
        uint                _maxLod     = 0;
        RC<DynamicFloat>    _tessLevel;

        RC<IGeomSource>     _geomSrc;


    // methods
    public:
        ScriptSphericalCube () {}

        void  SetDetailLevel1 (uint maxLod)                                                             __Th___;
        void  SetDetailLevel2 (uint minLod, uint maxLod)                                                __Th___;

        void  SetTessLevel1 (float level)                                                               __Th___;
        void  SetTessLevel2 (const ScriptDynamicFloatPtr &level)                                        __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;
        static void  GetShaderTypes (INOUT CppStructsFromShaders &)                                     __Th___;

    // ScriptGeomSource //
        ND_ RC<IGeomSource>     ToGeomSource ()                                                         __Th_OV;
        ND_ PipelineNames_t     FindMaterialPipeline ()                                                 C_Th_OV;
        ND_ RC<IGSMaterials>    ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)            C_Th_OV;

    private:
        void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)                                           C_Th_OV;

        ND_ static auto  _CreateUBType ()                                                               __Th___;
    };



    //
    // Unified Geometry Source
    //
    class ScriptUniGeometry final : public ScriptGeomSource
    {
    // types
    public:
        struct DrawCmd3
        {
            ScriptDynamicUIntPtr    dynVertexCount;
            ScriptDynamicUIntPtr    dynInstanceCount;
            uint                    vertexCount             = 0;
            uint                    instanceCount           = 1;
            uint                    firstVertex             = 0;
            uint                    firstInstance           = 0;

            void  SetDynVertexCount (const ScriptDynamicUIntPtr &);
            void  SetDynInstanceCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawIndexedCmd3
        {
            EIndex                  _indexType              = Default;
            ScriptBufferPtr         _indexBuffer;
            ulong                   _indexBufferOffset      = 0;
            String                  _indexBufferField;
            ScriptDynamicUIntPtr    dynIndexCount;
            ScriptDynamicUIntPtr    dynInstanceCount;
            uint                    indexCount              = 0;
            uint                    instanceCount           = 1;
            uint                    firstIndex              = 0;
            int                     vertexOffset            = 0;
            uint                    firstInstance           = 0;

            void  SetDynIndexCount (const ScriptDynamicUIntPtr &);
            void  SetDynInstanceCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawIndirectCmd3
        {
            ScriptBufferPtr         _indirectBuffer;
            ulong                   _indirectBufferOffset   = 0;
            String                  _indirectBufferField;
            uint                    drawCount               = 1;
            ScriptDynamicUIntPtr    dynDrawCount;
            uint                    stride                  = sizeof(Graphics::DrawIndirectCommand);

            void  SetDynDrawCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawIndexedIndirectCmd3
        {
            EIndex                  _indexType              = Default;
            ScriptBufferPtr         _indexBuffer;
            ulong                   _indexBufferOffset      = 0;
            String                  _indexBufferField;
            ScriptBufferPtr         _indirectBuffer;
            ulong                   _indirectBufferOffset   = 0;
            String                  _indirectBufferField;
            uint                    drawCount               = 1;
            ScriptDynamicUIntPtr    dynDrawCount;
            uint                    stride                  = sizeof(Graphics::DrawIndexedIndirectCommand);

            void  SetDynDrawCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawMeshTasksCmd3
        {
            ScriptDynamicUInt3Ptr   dynTaskCount;
            packed_uint3            taskCount               {1};

            void  SetDynTaskCount (const ScriptDynamicUInt3Ptr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawMeshTasksIndirectCmd3
        {
            ScriptBufferPtr         _indirectBuffer;
            ulong                   _indirectBufferOffset   = 0;
            String                  _indirectBufferField;
            uint                    drawCount               = 1;
            ScriptDynamicUIntPtr    dynDrawCount;
            uint                    stride                  = sizeof(Graphics::DrawMeshTasksIndirectCommand);

            void  SetDynDrawCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawIndirectCountCmd3
        {
            ScriptBufferPtr         _indirectBuffer;
            ulong                   _indirectBufferOffset   = 0;
            String                  _indirectBufferField;
            ScriptBufferPtr         _countBuffer;
            ulong                   _countBufferOffset      = 0;
            String                  _countBufferField;
            uint                    maxDrawCount            = 1;
            ScriptDynamicUIntPtr    dynMaxDrawCount;
            uint                    stride                  = sizeof(Graphics::DrawIndirectCommand);

            void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawIndexedIndirectCountCmd3
        {
            EIndex                  _indexType              = Default;
            ScriptBufferPtr         _indexBuffer;
            ulong                   _indexBufferOffset      = 0;
            String                  _indexBufferField;
            ScriptBufferPtr         _indirectBuffer;
            ulong                   _indirectBufferOffset   = 0;
            String                  _indirectBufferField;
            ScriptBufferPtr         _countBuffer;
            ulong                   _countBufferOffset      = 0;
            String                  _countBufferField;
            uint                    maxDrawCount            = 1;
            ScriptDynamicUIntPtr    dynMaxDrawCount;
            uint                    stride                  = sizeof(Graphics::DrawIndexedIndirectCommand);

            void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };

        struct DrawMeshTasksIndirectCountCmd3
        {
            ScriptBufferPtr         _indirectBuffer;
            ulong                   _indirectBufferOffset   = 0;
            String                  _indirectBufferField;
            ScriptBufferPtr         _countBuffer;
            ulong                   _countBufferOffset      = 0;
            String                  _countBufferField;
            uint                    maxDrawCount            = 1;
            ScriptDynamicUIntPtr    dynMaxDrawCount;
            uint                    stride                  = sizeof(Graphics::DrawMeshTasksIndirectCommand);

            void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);

            static void  Bind (const ScriptEnginePtr &se) __Th___;
        };


    private:
        using DrawCommand_t     = Union< DrawCmd3, DrawIndexedCmd3, DrawIndirectCmd3, DrawIndexedIndirectCmd3,
                                         DrawMeshTasksCmd3, DrawMeshTasksIndirectCmd3, DrawIndirectCountCmd3,
                                         DrawIndexedIndirectCountCmd3, DrawMeshTasksIndirectCountCmd3 >;
        using DrawCommands_t    = Array< DrawCommand_t >;


    // variables
    private:
        DrawCommands_t      _drawCommands;

        RC<IGeomSource>     _geomSrc;


    // methods
    public:
        ScriptUniGeometry () {}

        // draw commands
        void  Draw1 (const DrawCmd3 &)                                                                  __Th___;
        void  Draw2 (const DrawIndexedCmd3 &)                                                           __Th___;
        void  Draw3 (const DrawIndirectCmd3 &)                                                          __Th___;
        void  Draw4 (const DrawIndexedIndirectCmd3 &)                                                   __Th___;
        void  Draw5 (const DrawMeshTasksCmd3 &)                                                         __Th___;
        void  Draw6 (const DrawMeshTasksIndirectCmd3 &)                                                 __Th___;
        void  Draw7 (const DrawIndirectCountCmd3 &)                                                     __Th___;
        void  Draw8 (const DrawIndexedIndirectCountCmd3 &)                                              __Th___;
        void  Draw9 (const DrawMeshTasksIndirectCountCmd3 &)                                            __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;
        static void  GetShaderTypes (INOUT CppStructsFromShaders &)                                     __Th___;

        ScriptUniGeometry*  Clone ()                                                                    C_Th___;

    // ScriptGeomSource //
        ND_ RC<IGeomSource>     ToGeomSource ()                                                         __Th_OV;
        ND_ PipelineNames_t     FindMaterialPipeline ()                                                 C_Th_OV;
        ND_ RC<IGSMaterials>    ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)            C_Th_OV;

    private:
        void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)                                           C_Th_OV;

        ND_ static auto  _CreateUBType ()                                                               __Th___;
    };



    //
    // Model Geometry Source
    //
    class ScriptModelGeometrySrc final : public ScriptGeomSource
    {
    // types
    private:


    // variables
    private:
        Path                            _scenePath;
        String                          _dbgName;
        Array<Path>                     _texSearchDirs;

        ScriptRTGeometryPtr             _opaqueRTGeom;
        ScriptRTGeometryPtr             _translucentRTGeom;

        RC< ResLoader::IntermScene >    _intermScene;
        float4x4                        _initialTransform;

        const uint                      _maxTextures    = 128;

        RC<IGeomSource>                 _geomSrc;


    // methods
    public:
        ScriptModelGeometrySrc ()                                                                       __Th___;
        ScriptModelGeometrySrc (const String &filename)                                                 __Th___;
        ~ScriptModelGeometrySrc ();

            void  Name (const String &name)                                                             __Th___;
            void  AddTextureSearchDir (const String &value)                                             __Th___;
            void  SetInitialTransform (const packed_float4x4 &value)                                    __Th___;

        ND_ ScriptRTGeometry*  GetOpaqueRTGeometry ()                                                   __Th___;
        ND_ ScriptRTGeometry*  GetTranslucentRTGeometry ()                                              __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;
        static void  GetShaderTypes (INOUT CppStructsFromShaders &)                                     __Th___;

    // ScriptGeomSource //
        ND_ RC<IGeomSource>     ToGeomSource ()                                                         __Th_OV;
        ND_ PipelineNames_t     FindMaterialPipeline ()                                                 C_Th_OV;
        ND_ RC<IGSMaterials>    ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)            C_Th_OV;

    private:
        void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)                                           C_Th_OV;

        ND_ static String  _AttribsToVBName (const ResLoader::IntermVertexAttribs &)                    __Th___;
    };


} // AE::ResEditor