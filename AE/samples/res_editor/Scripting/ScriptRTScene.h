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
        uint        value   = 0xFF;

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
    // RayTracing Instance Transformation
    //
    struct RTInstanceTransform
    {
    // variables
        packed_float3   pos     {0.f};
        packed_float3   angles  {0.f};
        float           scale   = 1.f;

    // methods
        RTInstanceTransform () {}
        RTInstanceTransform (const packed_float3 &pos, const packed_float3 &angles, float scale = 1.f) : pos{pos}, angles{angles}, scale{scale} {}

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

            String              vbufferField;
            String              ibufferField;

            Bytes32u            vertexStride;
            Bytes               vertexDataOffset;
            Bytes               indexDataOffset;
        };
        using TriangleMeshes_t  = Array< TriangleMesh >;


    // variables
    private:
        TriangleMeshes_t        _triangleMeshes;
        String                  _dbgName;
        ScriptBufferPtr         _indirectBuffer;

        bool                    _immutableGeom      = false;
        bool                    _dummy              = false;
        bool                    _allowUpdate        = false;

        RC<RTGeometry>          _resource;


    // methods
    public:
        ScriptRTGeometry ()                                                                             __Th___;
        explicit ScriptRTGeometry (Bool isDummy)                                                        __Th___;
        ~ScriptRTGeometry ();

        void  Name (const String &name)                                                                 __Th___;
        void  EnableHistory ()                                                                          __Th___;
        void  AllowUpdate ()                                                                            __Th___;

        void  AddTriangles1 (const ScriptBufferPtr &vbuf)                                               __Th___;
        void  AddTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives)           __Th___;
        void  AddTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField)                        __Th___;
        void  AddTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField,
                             uint maxVertex, uint maxPrimitives)                                        __Th___;

        void  AddIndexedTriangles1 (const ScriptBufferPtr &vbuf, const ScriptBufferPtr &ibuf)           __Th___;
        void  AddIndexedTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives,
                                    const ScriptBufferPtr &ibuf, EIndex indexType)                      __Th___;
        void  AddIndexedTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField,
                                    const ScriptBufferPtr &ibuf, const String &ibField)                 __Th___;
        void  AddIndexedTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField,
                                    uint maxVertex, uint maxPrimitives,
                                    const ScriptBufferPtr &ibuf, const String &ibField)                 __Th___;

        void  MakeImmutable ()                                                                          __Th___;

        ND_ bool            HasIndirectBuffer ()                                                        const   { return bool{_indirectBuffer}; }
        ND_ ScriptBufferPtr GetIndirectBuffer ()                                                        __Th___;

        ND_ bool            WithHistory ()                                                              C_Th___;

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
            float4x3                transform           = float4x3::Identity();
            uint                    instanceCustomIndex = UMax;
            uint                    mask                = UMax;
            uint                    instanceSBTOffset   = UMax;
            ERTInstanceOpt          flags               = Default;
        };


    // variables
    private:
        Array<Instance>         _instances;
        ScriptBufferPtr         _instanceBuffer;
        ScriptBufferPtr         _indirectBuffer;
        String                  _dbgName;

        uint                    _maxRayTypes        = 0;
        bool                    _immutableInstances = false;
        bool                    _allowUpdate        = false;

        RC<RTScene>             _resource;


    // methods
    public:
        ScriptRTScene ()                                                                                __Th___;
        ~ScriptRTScene ();

        void  Name (const String &name)                                                                 __Th___;
        void  EnableHistory ()                                                                          __Th___;
        void  MaxRayTypes (uint value)                                                                  __Th___;
        void  AllowUpdate ()                                                                            __Th___;

        ND_ bool            HasIndirectBuffer ()                                                        const   { return bool{_indirectBuffer}; }
        ND_ ScriptBufferPtr GetInstanceBuffer ()                                                        __Th___;
        ND_ ScriptBufferPtr GetIndirectBuffer ()                                                        __Th___;

        ND_ uint            GetInstanceCount ()                                                         __Th___;
        ND_ uint            GetMaxRayTypes ()                                                           C_NE___ { return _maxRayTypes; }

        ND_ StringView      GetName ()                                                                  C_NE___ { return _dbgName; }

        ND_ bool            WithHistory ()                                                              C_Th___;

        void  AddInstance (const ScriptRTGeometryPtr &geom, const float4x3 &transform,
                           const RTInstanceCustomIndex &, const RTInstanceMask &,
                           const RTInstanceSBTOffset &, ERTInstanceOpt)                                 __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;

        ND_ RC<RTScene>  ToResource ()                                                                  __Th___;


    private:
                void  _AddInstance2 (Scripting::ScriptArgList args)                                     __Th___;
        static  void  _AddInstance (Scripting::ScriptArgList args)                                      __Th___;

        ND_ ScriptBuffer*   _GetInstanceBuffer ()                                                       __Th___;

        ND_ ScriptBuffer*   _GetIndirectBuffer ()                                                       __Th___;

            void    _MakeInstancesImmutable ();
    };


} // AE::ResEditor
