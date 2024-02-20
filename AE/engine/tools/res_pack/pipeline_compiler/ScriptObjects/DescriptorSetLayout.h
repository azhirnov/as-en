// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ShaderStructType.h"

namespace AE::PipelineCompiler
{

    //
    // Descriptor Set Layout
    //
    struct DescriptorSetLayout final : EnableScriptRC
    {
        friend struct PipelineLayout;

    // types
    public:
        struct MSLBindings
        {
            uint    samplerIdx          = 0;
            uint    imtblSamplerCount   = 0;
            uint    textureIdx          = 0;    // texture, texture_buffer
            uint    bufferIdx           = 0;    // <constant> buffer, <device> buffer, argument buffer, acceleration structure, visibility fn, intersection fn
            uint    dsBinding           = 0;

            ND_ uint  SamplerCount ()   const   { return samplerIdx + imtblSamplerCount; }
            ND_ uint  BufferCount ()    const   { return bufferIdx + dsBinding; }

            ND_ bool  operator == (const MSLBindings &rhs) const;
        };
        using UniqueTypes_t = ShaderStructType::UniqueTypes_t;

    private:
        struct AuxInfo
        {
            EAccessType         access  = Default;
            ShaderStructTypePtr type;
        };
        using InfoMap_t = HashMap< UniformName, AuxInfo >;

        using Uniform           = DescriptorSetLayoutDesc::Uniform;
        using BindingIndex_t    = DescriptorSetLayoutDesc::BindingIndex_t;
        using ArraySize_t       = DescriptorSetLayoutDesc::ArraySize_t;


    // variables
    private:
        HashSet< String >               _uniqueNames;
        DescriptorSetLayoutDesc         _dsLayout;
        InfoMap_t                       _infoMap;
        Array< ScriptFeatureSetPtr >    _features;
        const String                    _name;
        Optional<DescrSetUID>           _uid;
        String                          _defines;


    // methods
    public:
        DescriptorSetLayout () {}
        explicit DescriptorSetLayout (const String &name)                                                                                                   __Th___;

        void  AddFeatureSet (const String &name)                                                                                                            __Th___;
        void  Define (const String &value)                                                                                                                  __Th___;

        void  AddUniformBuffer (EShaderStages, const String &name, const ArraySize &, const String &typeName, EResourceState, Bool dynamic)                 __Th___;
        void  AddStorageBuffer (EShaderStages, const String &name, const ArraySize &, const String &typeName, EAccessType, EResourceState, Bool dynamic)    __Th___;
        void  AddUniformTexelBuffer (EShaderStages, const String &name, const ArraySize &, EImageType, EResourceState)                                      __Th___;
        void  AddStorageTexelBuffer (EShaderStages, const String &name, const ArraySize &, EImageType, EPixelFormat, EAccessType, EResourceState)           __Th___;
        void  AddStorageImage (EShaderStages, const String &name, const ArraySize &, EImageType, EPixelFormat, EAccessType, EResourceState)                 __Th___;
        void  AddSampledImage (EShaderStages, const String &name, const ArraySize &, EImageType, EResourceState)                                            __Th___;
        void  AddCombinedImage (EShaderStages, const String &name, const ArraySize &, EImageType, EResourceState)                                           __Th___;
        void  AddCombinedImage_ImmutableSampler (EShaderStages, const String &name, EImageType, EResourceState, const String &samplerName)                  __Th___;
        void  AddCombinedImage_ImmutableSampler (EShaderStages, const String &name, EImageType, EResourceState, ArrayView<String> samplerNames)             __Th___;
        void  AddSubpassInput (EShaderStages, const String &name, uint index, EImageType, EResourceState)                                                   __Th___;
        void  AddSubpassInputFromRenderTech (const String &renTechName, const String &passName)                                                             __Th___;
        void  AddSubpassInputFromRenderPass (const String &compatRPassName, const String &subpassName)                                                      __Th___;
        void  AddSampler (EShaderStages, const String &name, const ArraySize &)                                                                             __Th___;
        void  AddImmutableSampler (EShaderStages, const String &name, const String &samplerName)                                                            __Th___;
        void  AddImmutableSampler (EShaderStages, const String &name, ArrayView<String> samplerNames)                                                       __Th___;
        void  AddRayTracingScene (EShaderStages, const String &name, const ArraySize &)                                                                     __Th___;
        void  AddDebugStorageBuffer (const String &name, EShaderStages, Bytes staticSize, Bytes arraySize)                                                  __Th___;

        void  SetUsage (EDescSetUsage value)                                                                                                                __Th___;

        void  ToGLSL (EShaderStages stages, uint dsBinding, INOUT String &outTypes, OUT String &outDecl, INOUT UniqueTypes_t &uniqueTypes)                  C_Th___;
        void  ToMSL (EShaderStages stages, INOUT MSLBindings &bindings, INOUT String &outTypes, OUT String &outDecl, INOUT UniqueTypes_t &uniqueTypes)      C_Th___;

        ND_ bool  CountMSLBindings (EShaderStages stages, INOUT MSLBindings &bindings)                                                                      C_NE___;

        void  CountDescriptors (INOUT DescriptorCount &total, INOUT PerStageDescCount_t &perStage)                                                          C_NE___;

        static void  Bind (const ScriptEnginePtr &se)                                                                                                       __Th___;

        ND_ StringView      Name ()                             const   { return _name; }
        ND_ DescrSetUID     UID ()                              const   { return _uid.value_or( Default ); }
        ND_ String const&   GetDefines ()                       const   { return _defines; }
        ND_ EShaderStages   GetStages ()                        const   { return _dsLayout.stages; }
        ND_ EDescSetUsage   GetUsage ()                         const   { return _dsLayout.usage; }
        ND_ auto const&     GetUniforms ()                      const   { return _dsLayout.uniforms; }
        ND_ SamplerName     GetSampler (const Uniform &)        C_Th___;
        ND_ SamplerName     GetSampler (uint indexInStorage)    const;

        ND_ bool  IsCompatibleWithVulkan ()                     const;
        ND_ bool  IsCompatibleWithMetal ()                      const;

        ND_ static bool  CheckDescriptorLimits (const DescriptorCount &total, const PerStageDescCount_t &perStage,
                                                ArrayView<ScriptFeatureSetPtr> features, StringView name);

        ND_ bool  Build ();

    private:
        static void  _AddUniformBuffer (Scripting::ScriptArgList args)                      __Th___;
        static void  _AddStorageBuffer (Scripting::ScriptArgList args)                      __Th___;
        static void  _AddUniformBufferDynamic (Scripting::ScriptArgList args)               __Th___;
        static void  _AddStorageBufferDynamic (Scripting::ScriptArgList args)               __Th___;
        static void  _AddUniformTexelBuffer (Scripting::ScriptArgList args)                 __Th___;
        static void  _AddStorageTexelBuffer (Scripting::ScriptArgList args)                 __Th___;
        static void  _AddStorageImage (Scripting::ScriptArgList args)                       __Th___;
        static void  _AddSampledImage (Scripting::ScriptArgList args)                       __Th___;
        static void  _AddCombinedImage (Scripting::ScriptArgList args)                      __Th___;
        static void  _AddCombinedImage_ImmutableSampler (Scripting::ScriptArgList args)     __Th___;
        static void  _AddSubpassInput (Scripting::ScriptArgList args)                       __Th___;
        static void  _AddSampler (Scripting::ScriptArgList args)                            __Th___;
        static void  _AddImmutableSampler (Scripting::ScriptArgList args)                   __Th___;
        static void  _AddRayTracingScene (Scripting::ScriptArgList args)                    __Th___;

        void  _SetUsage (uint value)                                                        __Th___;

        void  _CheckUniformName (const String &name)                                        __Th___;
        void  _CheckArraySize (uint size)                                                   C_Th___;
        void  _CheckSamplerName (const String &name)                                        __Th___;
        void  _CheckAccessType (EAccessType access)                                         C_Th___;
        void  _CheckStorageFormat (EPixelFormat fmt, bool isReadOnly)                       C_Th___;
        void  _CheckFields (const String &fields)                                           C_Th___;
    };
    using DescriptorSetLayoutPtr = ScriptRC< DescriptorSetLayout >;


} // AE::PipelineCompiler
