// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/BasePipeline.h"

namespace AE::PipelineCompiler
{

    //
    // Mesh Pipeline Specialization
    //
    struct MeshPipelineSpecScriptBinding final : BasePipelineSpec
    {
    // variables
    public:
        Graphics::MeshPipelineDesc  desc;
        Graphics::RenderState       renderState;


    // methods
    public:
        MeshPipelineSpecScriptBinding () {}
        MeshPipelineSpecScriptBinding (const MeshPipelineScriptBinding* base, const String &name) __Th___;

        // called from RTGraphicsPass
        void  SetRenderPass (RenderPassName::Ref, SubpassName::Ref)     __Th___;

        void  SetSpecValueU (const String &name, uint  value)           __Th___ { return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, value ); }
        void  SetSpecValueI (const String &name, int   value)           __Th___ { return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }
        void  SetSpecValueF (const String &name, float value)           __Th___ { return BasePipelineSpec::_SetSpecValue( INOUT desc.specialization, name, BitCast<uint>(value) ); }

        void  SetOptions (EPipelineOpt value)                           __Th___ { return BasePipelineSpec::_SetOptions( value ); }

        void  SetTaskGroupSize1 (uint x)                                __Th___ { SetTaskGroupSize3( x, UMax, UMax ); }
        void  SetTaskGroupSize2 (uint x, uint y)                        __Th___ { SetTaskGroupSize3( x, y, UMax ); }
        void  SetTaskGroupSize3 (uint x, uint y, uint z)                __Th___;
        void  SetMeshGroupSize1 (uint x)                                __Th___ { SetMeshGroupSize3( x, UMax, UMax ); }
        void  SetMeshGroupSize2 (uint x, uint y)                        __Th___ { SetMeshGroupSize3( x, y, UMax ); }
        void  SetMeshGroupSize3 (uint x, uint y, uint z)                __Th___;

        void  SetDynamicState (/*EPipelineDynamicState*/uint states)    __Th___;
        void  SetRenderState (const RenderState &state)                 __Th___;
        void  SetRenderState2 (const String &name)                      __Th___;
        void  SetViewportCount (uint value)                             __Th___;

        void  AddToRenderTech (const String &rtech, const String &pass) __Th___ { return BasePipelineSpec::_AddToRenderTech( rtech, pass ); }

        ND_ const MeshPipelineScriptBinding*    GetBase ()              const   { return Cast<MeshPipelineScriptBinding>( BasePipelineSpec::GetBase() ); }

        ND_ bool  Build (PipelineTemplUID uid);

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };
    using MeshPipelineSpecPtr = ScriptRC< MeshPipelineSpecScriptBinding >;



    //
    // Mesh Pipeline Template
    //
    struct MeshPipelineScriptBinding final : BasePipelineTmpl
    {
    // types
        using Specializations_t = Array< MeshPipelineSpecPtr >;
        using ShaderIOMap_t     = FlatHashMap< EShader, Pair< ShaderStructTypePtr, ShaderStructTypePtr >>;


    // variables
    public:
        CompiledShaderPtr       task;
        CompiledShaderPtr       mesh;
        CompiledShaderPtr       fragment;
    private:
        Optional<FragOutput_t>  _fragOutput;
        Specializations_t       _pplnSpec;
        ShaderIOMap_t           _shaderIO;
        bool                    _isPrepared = false;


    // methods
    public:
        MeshPipelineScriptBinding ();
        explicit MeshPipelineScriptBinding (const String &name)                         __Th___;

        void  SetTaskShader     (const ScriptShaderPtr &shader)                         __Th___;
        void  SetMeshShader     (const ScriptShaderPtr &shader)                         __Th___;
        void  SetFragmentShader (const ScriptShaderPtr &shader)                         __Th___;

        void  SetFragmentOutputFromRenderTech (const String &renTechName, const String &passName)       __Th___;
        void  SetFragmentOutputFromRenderPass (const String &compatRPassName, const String &subpassName)__Th___;

        void  SetShaderIO (EShader output, EShader input, const String &typeName)       __Th___;

        void  SetLayout (const String &name)                                            __Th___ { BasePipelineTmpl::_SetLayout( name ); }
        void  SetLayout2 (const PipelineLayoutPtr &pl)                                  __Th___ { BasePipelineTmpl::_SetLayout( pl ); }
        void  Define (const String &value)                                              __Th___ { BasePipelineTmpl::_Define( value ); }
        void  AddFeatureSet (const String &name)                                        __Th___ { BasePipelineTmpl::_AddFeatureSet( name ); }

        void  TestRenderPass (const String &compatRP, const String &subpass)            __Th___;
        void  GetSubpassShaderIO (OUT SubpassShaderIO &fragIO) const                    __Th___;

        ND_ MeshPipelineSpecPtr             AddSpecialization2 (const String &name)     __Th___;
        ND_ MeshPipelineSpecScriptBinding*  AddSpecialization (const String &name)      __Th___;
        ND_ Specializations_t const&        GetSpecializations ()                       const   { return _pplnSpec; }

        ND_ bool    Build ();
        ND_ usize   SpecCount ()                                                        const   { return _pplnSpec.size(); }

        static void  Bind (const ScriptEnginePtr &se)                                   __Th___;

    private:
        void  _Prepare ()                                                               __Th___;

        ND_ ShaderStructTypePtr  _GetShaderInput (EShader)                              const;
        ND_ ShaderStructTypePtr  _GetShaderOutput (EShader)                             const;
    };
    using MeshPipelinePtr = ScriptRC< MeshPipelineScriptBinding >;


} // AE::PipelineCompiler
