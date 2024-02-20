// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ScriptFeatureSet.h"

namespace AE::PipelineCompiler
{

    //
    // Script Sampler
    //

    struct ScriptSampler final : EnableScriptRC
    {
    // variables
    public:
        const SamplerName               _name;
        const String                    _nameStr;

        SamplerDesc                     _desc;
        SamplerYcbcrConversionDesc      _ycbcrDesc;

        Array< ScriptFeatureSetPtr >    _features;
        bool                            _isValid    = false;
        bool                            _hasYcbcr   = false;


    // methods
    public:
        ScriptSampler ();
        ScriptSampler (ScriptSampler &&)                                        __NE___;
        explicit ScriptSampler (const String &name)                             __Th___;

        void  SetDesc (const SamplerDesc &)                                     __Th___;
        void  SetYcbcrDesc (const SamplerYcbcrConversionDesc &)                 __Th___;

        void  SetFilter (EFilter mag, EFilter min, EMipmapFilter mipmap)        __Th___;
        void  SetAddressModeV (EAddressMode uvw)                                __Th___;
        void  SetAddressMode (EAddressMode u, EAddressMode v, EAddressMode w)   __Th___;
        void  SetMipLodBias (float value)                                       __Th___;
        void  SetLodRange (float min, float max)                                __Th___;
        void  SetAnisotropy (float value)                                       __Th___;
        void  SetCompareOp (ECompareOp value)                                   __Th___;
        void  SetBorderColor (EBorderColor value)                               __Th___;
        void  SetNormCoordinates (bool value)                                   __Th___;
        void  SetReductionMode (EReductionMode value)                           __Th___;
        void  SetUsage (ESamplerUsage value)                                    __Th___;

        // samplerYcbcrConversion
        void  Ycbcr_SetFormat (EPixelFormat value)                              __Th___;
        void  Ycbcr_SetFormat2 (EPixelFormatExternal value)                     __Th___;
        void  Ycbcr_SetModel (ESamplerYcbcrModelConversion value)               __Th___;
        void  Ycbcr_SetRange (ESamplerYcbcrRange value)                         __Th___;
        void  Ycbcr_SetComponents (const String &value)                         __Th___;
        void  Ycbcr_SetXChromaOffset (ESamplerChromaLocation value)             __Th___;
        void  Ycbcr_SetYChromaOffset (ESamplerChromaLocation value)             __Th___;
        void  Ycbcr_SetChromaFilter (EFilter value)                             __Th___;
        void  Ycbcr_ForceExplicitReconstruction (bool value)                    __Th___;

        void  AddFeatureSet (const String &name)                                __Th___;

        ND_ bool  Validate ();
        ND_ bool  IsValid ()                                                    const   { return _isValid; }

        static void  Bind (const ScriptEnginePtr &se)                           __Th___;

        ND_ SamplerDesc const&                  Desc ()                         const   { return _desc; }
        ND_ SamplerYcbcrConversionDesc const&   YcbcrDesc ()                    const   { return _ycbcrDesc; }
        ND_ bool                                HasYcbcr ()                     const   { return _hasYcbcr; }
        ND_ ArrayView<ScriptFeatureSetPtr>      GetFeatures ()                  const   { return _features; }

        ND_ HashVal     CalcHash ()                                             const;
        ND_ StringView  GetName ()                                              const   { return _nameStr; }

    private:
        void  _CheckAddressMode (EAddressMode mode)                             __Th___;
        void  _CheckYcbcrSampler ()                                             __Th___;
    };
    using ScriptSamplerPtr = ScriptRC< ScriptSampler >;


} // AE::PipelineCompiler
