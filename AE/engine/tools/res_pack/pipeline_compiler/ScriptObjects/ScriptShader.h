// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/Common.h"

namespace AE::PipelineCompiler
{
    using ScriptFeatureSetPtr = ScriptRC< struct ScriptFeatureSet >;

    enum class ETessPatch
    {
        Unknown     = 0,
        Points,
        Isolines,
        Triangles,
        Quads,
        _Count
    };

    enum class ETessSpacing
    {
        Unknown     = 0,
        Equal,
        FractionalEven,
        FractionalOdd,
        _Count
    };


    //
    // Script Shader
    //
    struct ScriptShader final : EnableScriptRC
    {
    // types
    private:
        struct SpecInfo
        {
            EValueType  type    = Default;
            uint        index   = UMax;
        };
        using SpecConstInfo_t = FixedMap< SpecializationName, SpecInfo, GraphicsConfig::MaxSpecConstants >;
        using SpecConstants_t = FixedMap< SpecializationName, /*location*/uint, GraphicsConfig::MaxSpecConstants >;


    // variables
    private:
        uint                _specId             = 0;
        EShaderStages       _requiredStages     = Default;
        uint3               _defaultLocalSize   {~0u};
        uint3               _localSizeSpec      {~0u};
        SpecConstInfo_t     _spec;
        SpecConstants_t     _specConst;
        mutable PathAndLine _absolutePath;
        String              _source;
        mutable String      _entry;
        String              _filename;
        String              _defines;

        // only for mesh shader
        uint                _mashMaxVertices    = 0;
        uint                _meshMaxPrimitives  = 0;
        EPrimitive          _meshTopology       = Default;

        // only for tessellation shaders
        uint                _tcsPatchSize       = 0;
        ETessPatch          _tesPatchMode       = Default;
        ETessSpacing        _tesPatchSpacing    = Default;
        bool                _tesFrontFaceCCW    = true;

    public:
        EShader             type                = Default;
        EShaderVersion      version             = Default;
        EShaderOpt          options             = Default;


    // methods
    public:
        ScriptShader ();

        void  AddSpec (EValueType type, const String &name)                             __Th___;

        void  SetComputeSpec1 ()                                                        __Th___;
        void  SetComputeSpec2 ()                                                        __Th___;
        void  SetComputeSpec3 ()                                                        __Th___;

        void  SetComputeSpecAndDefault1 (uint x)                                        __Th___;
        void  SetComputeSpecAndDefault2 (uint x, uint y)                                __Th___;
        void  SetComputeSpecAndDefault3 (uint x, uint y, uint z)                        __Th___;

        void  SetComputeLocalSize1 (uint x)                                             __Th___;
        void  SetComputeLocalSize2 (uint x, uint y)                                     __Th___;
        void  SetComputeLocalSize3 (uint x, uint y, uint z)                             __Th___;

        void  SetMeshSpecAndDefault1 (uint x)                                           __Th___;
        void  SetMeshSpecAndDefault2 (uint x, uint y)                                   __Th___;
        void  SetMeshSpecAndDefault3 (uint x, uint y, uint z)                           __Th___;

        void  SetMeshSpec1 ()                                                           __Th___;
        void  SetMeshSpec2 ()                                                           __Th___;
        void  SetMeshSpec3 ()                                                           __Th___;

        void  SetMeshLocalSize1 (uint x)                                                __Th___;
        void  SetMeshLocalSize2 (uint x, uint y)                                        __Th___;
        void  SetMeshLocalSize3 (uint x, uint y, uint z)                                __Th___;

        void  SetMeshOutput (uint maxVertices, uint maxPrimitives, EPrimitive value)    __Th___;

        void  SetTessPatchSize (uint vertexCount)                                       __Th___;
        void  SetTessPatchMode (ETessPatch mode, ETessSpacing spacing, bool ccw)        __Th___;

        void  Define (const String &value)                                              __Th___;
        void  LoadSelf ()                                                               __Th___;

        void  SetSource (EShader type, String src);
        void  SetSource2 (EShader type, String src, PathAndLine path);

        ND_ SpecConstants_t const&  GetSpec ()                                          const   { return _specConst; }

        ND_ String          GetSource ()                                                C_Th___;
        ND_ PathAndLine     GetPath ()                                                  C_Th___;
        ND_ String const&   GetEntry ()                                                 const;
        ND_ StringView      GetDefines ()                                               const   { return _defines; }

        ND_ String          SpecToGLSL ()                                               C_Th___;
        ND_ String          SpecToMSL  ()                                               C_Th___;
        ND_ String          InputToMSL ()                                               C_Th___;
        ND_ String          MeshOutToMSL (String vertex, String primitive)              C_Th___;
        ND_ String          ThreadgroupsMSL (ArrayView<ScriptFeatureSetPtr> features)   C_Th___;

        static void  Bind (const ScriptEnginePtr &se)                                   __Th___;

    private:
        void  _UpdatePath ()                                                            C_Th___;

        void  _Validate ()                                                              C_Th___;
    };

    using ScriptShaderPtr = ScriptRC< ScriptShader >;


} // AE::PipelineCompiler
