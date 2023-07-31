// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Resources/RTScene.h"

namespace AE::ResEditor
{

    //
    // RayTracing Instance Custom Index
    //
    struct RTInstanceCustomIndex
    {
    // variables
        uint        value   = 0;

    // methods
        RTInstanceCustomIndex () {}
        explicit RTInstanceCustomIndex (uint v) : value{v} {}

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };


    //
    // RayTracing Instance Mask
    //
    struct RTInstanceMask
    {
    // variables
        uint        value   = 0;

    // methods
        RTInstanceMask () {}
        explicit RTInstanceMask (uint v) : value{v} {}

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };


    //
    // RayTracing Instance SBT Offset
    //
    struct RTInstanceSBTOffset
    {
    // variables
        uint        value   = 0;

    // methods
        RTInstanceSBTOffset () {}
        explicit RTInstanceSBTOffset (uint v) : value{v} {}

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };


    //
    // RayTracing Instance Rotation
    //
    struct RTInstanceRotation
    {
    // variables
        packed_float3   angles;

    // methods
        RTInstanceRotation () {}
        explicit RTInstanceRotation (const packed_float3 &v) : angles{v} {}

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };



    //
    // RayTracing Geometry
    //

    class ScriptRTGeometry final : public EnableScriptRC
    {
    // types
    private:
        struct TriangleMesh : RTGeometryBuild::TrianglesInfo
        {
            ScriptBufferPtr     vbuffer;
            ScriptBufferPtr     ibuffer;

            Bytes32u            vertexStride;
            Bytes               vertexDataOffset;
            Bytes               indexDataOffset;
        };
        using TriangleMeshes_t  = Array< TriangleMesh >;


    // variables
    public:
        TriangleMeshes_t        _triangleMeshes;
        String                  _dbgName;
        ScriptBufferPtr         _indirectBuffer;

        bool                    _immutableGeom      = false;
        RC<RTGeometry>          _resource;


    // methods
    public:
        ScriptRTGeometry ()                                                                             __Th___;

        void  Name (const String &name)                                                                 __Th___;
        void  EnableHistory ()                                                                          __Th___;

        void  AddTriangles1 (const ScriptBufferPtr &vbuf)                                               __Th___;
        void  AddTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives)           __Th___;

        void  AddIndexedTriangles1 (const ScriptBufferPtr &vbuf, const ScriptBufferPtr &ibuf)           __Th___;
        void  AddIndexedTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives,
                                    const ScriptBufferPtr &ibuf, EIndex indexType)                      __Th___;

        void  MakeImmutable ()                                                                          __Th___;

        ND_ bool            HasIndirectBuffer ()                                                        const   { return bool{_indirectBuffer}; }
        ND_ ScriptBufferPtr GetIndirectBuffer ()                                                        __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;

        ND_ RC<RTGeometry>  ToResource ()                                                               __Th___;

    private:
               void  _Validate ()                                                                       __Th___;
               void  _Validate2 ()                                                                      __Th___;

               void  _MutableResource ()                                                                C_Th___;
        static void  _CheckBuffer (const ScriptBufferPtr &buf)                                          __Th___;

        ND_ ScriptBuffer*   _GetIndirectBuffer ()                                                       __Th___;
        ND_ uint            _GetGeometryCount ()                                                        __Th___;
    };



    //
    // RayTracing Scene
    //

    class ScriptRTScene final : public EnableScriptRC
    {
    // types
    private:
        struct Instance
        {
            ScriptRTGeometryPtr     geometry;
            float3x4                transform           = float3x4::Identity();
            uint                    instanceCustomIndex = UMax;
            uint                    mask                = UMax;
            uint                    instanceSBTOffset   = UMax;
            ERTInstanceOpt          flags               = Default;
        };


    // variables
    public:
        Array<Instance>         _instances;
        ScriptBufferPtr         _instanceBuffer;
        ScriptBufferPtr         _indirectBuffer;
        String                  _dbgName;

        bool                    _immutableInstances = false;
        RC<RTScene>             _resource;


    // methods
    public:
        ScriptRTScene ()                                                                                __Th___;

        void  Name (const String &name)                                                                 __Th___;
        void  EnableHistory ()                                                                          __Th___;

        ND_ bool            HasIndirectBuffer ()                                                        const   { return bool{_indirectBuffer}; }
        ND_ ScriptBufferPtr GetInstanceBuffer ()                                                        __Th___;
        ND_ ScriptBufferPtr GetIndirectBuffer ()                                                        __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;

        ND_ RC<RTScene>  ToResource ()                                                                  __Th___;

    private:
                void  _AddInstance2 (Scripting::ScriptArgList args)                                     __Th___;
        static  void  _AddInstance (Scripting::ScriptArgList args)                                      __Th___;

        ND_ ScriptBuffer*   _GetInstanceBuffer ()                                                       __Th___;
        ND_ uint            _GetInstanceCount ()                                                        __Th___;

        ND_ ScriptBuffer*   _GetIndirectBuffer ()                                                       __Th___;

            void    _MakeInstancesImmutable ();
    };


} // AE::ResEditor
