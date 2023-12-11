// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptSampler.h"
#include "ScriptObjects/Common.inl.h"

AE_DECL_SCRIPT_OBJ_RC( AE::PipelineCompiler::ScriptSampler, "Sampler" );


namespace AE::PipelineCompiler
{
namespace
{
    static ScriptSampler*  SamplerDesc_Ctor (const String &name) {
        return ScriptSamplerPtr{new ScriptSampler{ name }}.Detach();
    }
}

/*
=================================================
    constructor
=================================================
*/
    ScriptSampler::ScriptSampler () :
        ScriptSampler{"<unknown>"}
    {}

    ScriptSampler::ScriptSampler (const String &name) __Th___ :
        _name{ SamplerName{name} },
        _nameStr{ name },
        _features{ ObjectStorage::Instance()->GetDefaultFeatureSets() }
    {
        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<SamplerName>( name );
        CHECK_THROW_MSG( storage.samplerMap.emplace( name, storage.samplerRefs.size() ).second,
            "Sampler with name '"s << name << "' is already defined" );
        storage.samplerRefs.push_back( ScriptSamplerPtr{this} );
    }

    ScriptSampler::ScriptSampler (ScriptSampler &&other) __NE___ :
        _name{ other._name },
        _desc{ other._desc },
        _features{ RVRef(other._features) }
    {}

/*
=================================================
    AddFeatureSet
=================================================
*/
    void  ScriptSampler::AddFeatureSet (const String &name) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        auto    fs_it   = storage.featureSets.find( FeatureSetName{name} );
        CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
            "FeatureSet with name '"s << name << "' is not found" );

        _features.push_back( fs_it->second );
    }

/*
=================================================
    _CheckAddressMode
=================================================
*/
    void  ScriptSampler::_CheckAddressMode (EAddressMode mode) __Th___
    {
        CHECK_THROW_MSG( mode < EAddressMode::_Count );

        BEGIN_ENUM_CHECKS();
        switch ( mode )
        {
            case EAddressMode::MirrorClampToEdge :
                TEST_FEATURE( _features, samplerMirrorClampToEdge );
                break;

            case EAddressMode::Repeat :
            case EAddressMode::MirrorRepeat :
            case EAddressMode::ClampToEdge :
            case EAddressMode::ClampToBorder :
            case EAddressMode::_Count :
            case EAddressMode::Unknown :                break;
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    SetFilter
=================================================
*/
    void  ScriptSampler::SetFilter (EFilter mag, EFilter min, EMipmapFilter mipmap) __Th___
    {
        _desc.magFilter     = mag;
        _desc.minFilter     = min;
        _desc.mipmapMode    = mipmap;
    }

/*
=================================================
    SetAddressMode
=================================================
*/
    void  ScriptSampler::SetAddressModeV (EAddressMode uvw) __Th___
    {
        _CheckAddressMode( uvw );

        _desc.addressMode = {uvw, uvw, uvw};
    }

    void  ScriptSampler::SetAddressMode (EAddressMode u, EAddressMode v, EAddressMode w) __Th___
    {
        _CheckAddressMode( u );
        _CheckAddressMode( v );
        _CheckAddressMode( w );

        _desc.addressMode = {u, v, w};
    }

/*
=================================================
    SetMipLodBias
=================================================
*/
    void  ScriptSampler::SetMipLodBias (float value) __Th___
    {
        TEST_FEATURE( _features, samplerMipLodBias );
        TestFeature_Min( _features, &FeatureSet::maxSamplerLodBias, value, "maxSamplerLodBias", "mipLodBias" );

        _desc.mipLodBias = value;
    }

/*
=================================================
    SetLodRange
=================================================
*/
    void  ScriptSampler::SetLodRange (float min, float max) __Th___
    {
        CHECK_THROW_MSG( min <= max );

        _desc.minLod = min;
        _desc.maxLod = max;
    }

/*
=================================================
    SetAnisotropy
=================================================
*/
    void  ScriptSampler::SetAnisotropy (float value) __Th___
    {
        TEST_FEATURE( _features, samplerAnisotropy );
        TestFeature_Min( _features, &FeatureSet::maxSamplerAnisotropy, value, "maxSamplerAnisotropy", "maxAnisotropy" );

        _desc.maxAnisotropy = value;
    }

/*
=================================================
    SetCompareOp
=================================================
*/
    void  ScriptSampler::SetCompareOp (ECompareOp value) __Th___
    {
        CHECK_THROW_MSG( value < ECompareOp::_Count );

        _desc.compareOp = value;
    }

/*
=================================================
    SetBorderColor
=================================================
*/
    void  ScriptSampler::SetBorderColor (EBorderColor value) __Th___
    {
        CHECK_THROW_MSG( value < EBorderColor::_Count );

        _desc.borderColor = value;
    }

/*
=================================================
    SetNormCoordinates
=================================================
*/
    void  ScriptSampler::SetNormCoordinates (bool value) __Th___
    {
        _desc.unnormalizedCoordinates = not value;
    }

/*
=================================================
    SetReductionMode
=================================================
*/
    void  ScriptSampler::SetReductionMode (EReductionMode value) __Th___
    {
        CHECK_THROW_MSG( value < EReductionMode::_Count );

        if ( value != EReductionMode::Average )
        {
            TEST_FEATURE( _features, samplerFilterMinmax );
        }
        _desc.reductionMode = value;
    }

/*
=================================================
    SetUsage
=================================================
*/
    void  ScriptSampler::SetUsage (ESamplerUsage value) __Th___
    {
        CHECK_THROW_MSG( value < ESamplerUsage::_Count );

        _desc.usage = value;
    }

/*
=================================================
    Ycbcr_SetFormat
=================================================
*/
    void  ScriptSampler::Ycbcr_SetFormat (EPixelFormat value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.format = value;
    }

/*
=================================================
    Ycbcr_SetModel
=================================================
*/
    void  ScriptSampler::Ycbcr_SetModel (ESamplerYcbcrModelConversion value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.ycbcrModel = value;
    }

/*
=================================================
    Ycbcr_SetRange
=================================================
*/
    void  ScriptSampler::Ycbcr_SetRange (ESamplerYcbcrRange value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.ycbcrRange = value;
    }

/*
=================================================
    Ycbcr_SetComponents
=================================================
*/
    void  ScriptSampler::Ycbcr_SetComponents (const String &value) __Th___
    {
        _CheckYcbcrSampler();
        CHECK_THROW_MSG( not value.empty() );

        _ycbcrDesc.components = ImageSwizzle::FromString( value.data(), value.size() );
    }

/*
=================================================
    Ycbcr_SetXChromaOffset
=================================================
*/
    void  ScriptSampler::Ycbcr_SetXChromaOffset (ESamplerChromaLocation value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.xChromaOffset = value;
    }

/*
=================================================
    Ycbcr_SetYChromaOffset
=================================================
*/
    void  ScriptSampler::Ycbcr_SetYChromaOffset (ESamplerChromaLocation value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.yChromaOffset = value;
    }

/*
=================================================
    Ycbcr_SetChromaFilter
=================================================
*/
    void  ScriptSampler::Ycbcr_SetChromaFilter (EFilter value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.chromaFilter = value;
    }

/*
=================================================
    Ycbcr_ForceExplicitReconstruction
=================================================
*/
    void  ScriptSampler::Ycbcr_ForceExplicitReconstruction (bool value) __Th___
    {
        _CheckYcbcrSampler();

        _ycbcrDesc.forceExplicitReconstruction = value;
    }

/*
=================================================
    _CheckYcbcrSampler
=================================================
*/
    void  ScriptSampler::_CheckYcbcrSampler () __Th___
    {
        TEST_FEATURE( _features, samplerYcbcrConversion );
        _hasYcbcr = true;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptSampler::Bind (const ScriptEnginePtr &se) __Th___
    {
        // bind sampler
        {
            ClassBinder<ScriptSampler>  binder{ se };
            binder.CreateRef();

            binder.Comment( "Create sampler.\n"
                            "Name is used as typename for immutable samplers or to get sampler in C++ code." );
            binder.AddFactoryCtor( &SamplerDesc_Ctor, {"name"} );

            binder.Comment( "Add FeatureSet to the sampler.\n"
                            "Sampler must only use features which is enable in FS." );
            binder.AddMethod( &ScriptSampler::AddFeatureSet,        "AddFeatureSet",    {"fsName"} );

            binder.Comment( "Set min/mag/mipmap filter." );
            binder.AddMethod( &ScriptSampler::SetFilter,            "Filter",           {"min", "mag", "mipmap"} );

            binder.Comment( "Set UVW address mode." );
            binder.AddMethod( &ScriptSampler::SetAddressModeV,      "AddressMode",      {"uvw"} );
            binder.AddMethod( &ScriptSampler::SetAddressMode,       "AddressMode",      {"u", "v", "w"} );

            binder.Comment( "Set mipmap level of detail bias." );
            binder.AddMethod( &ScriptSampler::SetMipLodBias,        "MipLodBias",       {"bias"} );

            binder.Comment( "Set level of detail range." );
            binder.AddMethod( &ScriptSampler::SetLodRange,          "LodRange",         {"min", "max"} );

            binder.Comment( "Set anisotropy level." );
            binder.AddMethod( &ScriptSampler::SetAnisotropy,        "Anisotropy",       {"level"} );

            binder.Comment( "Set compare operator." );
            binder.AddMethod( &ScriptSampler::SetCompareOp,         "CompareOp",        {"op"} );

            binder.Comment( "Set border color." );
            binder.AddMethod( &ScriptSampler::SetBorderColor,       "BorderColor",      {"color"} );

            binder.Comment( "Use normalized coordinates.\n"
                            "Default is 'true'. Set 'false' to use pixel coordinates." );
            binder.AddMethod( &ScriptSampler::SetNormCoordinates,   "NormCoordinates",  {"norm"} );

            binder.Comment( "Set reduction mode.\n"
                            "Requires 'samplerFilterMinmax' feature. Default value is 'Average'." );
            binder.AddMethod( &ScriptSampler::SetReductionMode,     "ReductionMode",    {"mode"} );

            binder.Comment( "Set sampler usage." );
            binder.AddMethod( &ScriptSampler::SetUsage,             "Usage",            {} );


            // samplerYcbcrConversion
            binder.Comment( "----\nRequires 'samplerYcbcrConversion' feature\n----\n" );

            binder.Comment( "Set Ycbcr format. Requires multiplanar format." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetFormat,                      "Ycbcr_Format",                     {} );

            binder.Comment( "Set Ycbcr model conversion." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetModel,                       "Ycbcr_Model",                      {} );

            binder.Comment( "Set Ycbcr range." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetRange,                       "Ycbcr_Range",                      {} );

            binder.Comment( "Set Ycbcr component swizzle.\n"
                            "Format: 'ARGB', 'R001'." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetComponents,                  "Ycbcr_Components",                 {} );

            binder.Comment( "Set Ycbcr X chroma location." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetXChromaOffset,               "Ycbcr_XChromaOffset",              {} );

            binder.Comment( "Set Ycbcr Y chroma location." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetYChromaOffset,               "Ycbcr_YChromaOffset",              {} );

            binder.Comment( "Set Ycbcr Y chroma filter." );
            binder.AddMethod( &ScriptSampler::Ycbcr_SetChromaFilter,                "Ycbcr_ChromaFilter",               {} );

            binder.Comment( "Set Ycbcr force explicit reconstruction." );
            binder.AddMethod( &ScriptSampler::Ycbcr_ForceExplicitReconstruction,    "Ycbcr_ForceExplicitReconstruction", {} );
        }
    }

/*
=================================================
    Validate
=================================================
*/
    bool  ScriptSampler::Validate ()
    {
    #define SLOG( ... ) { AE_LOGI( "Sampler '"s <<  GetName() << "': " << __VA_ARGS__ ); result = false; }

        bool    result = true;

        if ( _desc.unnormalizedCoordinates )
        {
            if ( _desc.minFilter != _desc.magFilter )
            {
                SLOG( "min & mag filter for unnormalized coordinates must equal" );
                _desc.magFilter = _desc.minFilter;
            }

            if ( _desc.mipmapMode != EMipmapFilter::Nearest )
            {
                SLOG( "mipmap filter for unnormalized coordinates must be 'nearest'" );
                _desc.mipmapMode = EMipmapFilter::Nearest;
            }

            if ( _desc.minLod != 0.0f or _desc.maxLod != 0.0f )
            {
                //SLOG( "min & max LOD for unnormalized coordinates must be zero" );
                _desc.minLod = _desc.maxLod = 0.0f;
            }

            if ( _desc.addressMode.x != EAddressMode::ClampToEdge   and
                 _desc.addressMode.x != EAddressMode::ClampToBorder )
            {
                SLOG( "U-address mode for unnormalized coordinates must be 'clamp'" );
                _desc.addressMode.x = EAddressMode::ClampToEdge;
            }

            if ( _desc.addressMode.y != EAddressMode::ClampToEdge   and
                 _desc.addressMode.y != EAddressMode::ClampToBorder )
            {
                SLOG( "V-address mode for unnormalized coordinates must be 'clamp'" );
                _desc.addressMode.y = EAddressMode::ClampToEdge;
            }

            if ( _desc.maxAnisotropy.has_value() )
            {
                SLOG( "anisotropy filter for unnormalized coordinates is not supported" );
                _desc.maxAnisotropy = {};
            }

            if ( _desc.compareOp.has_value() )
            {
                SLOG( "compare mode for unnormalized coordinates is not supported" );
                _desc.compareOp = {};
            }
        }

        if ( _desc.maxLod < _desc.minLod )
        {
            SLOG( "min LOD must be less than or equal to max LOD" );
            _desc.maxLod = _desc.minLod;
        }

        //if ( (minFilter == EFilter::Cubic or magFilter == EFilter::Cubic) and maxAnisotropy.has_value() )
        //{
        //  SLOG( "anisotropy filter is not supported for cubic filter" );
        //  maxAnisotropy   = {};
        //}

        if ( _desc.addressMode.x != EAddressMode::ClampToBorder and
             _desc.addressMode.y != EAddressMode::ClampToBorder and
             _desc.addressMode.z != EAddressMode::ClampToBorder )
        {
            // reset border color, because it is unused
            _desc.borderColor = EBorderColor::FloatTransparentBlack;
        }


        if ( _hasYcbcr )
        {
            // TODO
        }

        _isValid = result;
        return result;
    #undef SLOG
    }

/*
=================================================
    CalcHash
=================================================
*/
    HashVal  ScriptSampler::CalcHash () const
    {
        HashVal result;

        result << HashOf( _desc.usage );
        result << HashOf( _desc.magFilter ) << HashOf( _desc.minFilter ) << HashOf( _desc.mipmapMode );
        result << HashOf( _desc.addressMode );
        result << HashOf( _desc.mipLodBias ) << HashOf( _desc.minLod ) << HashOf( _desc.maxLod );
        result << HashOf( _desc.maxAnisotropy ) << HashOf( _desc.compareOp );
        result << HashOf( _desc.borderColor );
        result << HashOf( _desc.unnormalizedCoordinates );
        result << HashOf( _desc.reductionMode );

        if ( _hasYcbcr )
        {
            result << HashOf( _ycbcrDesc.format );
            result << HashOf( _ycbcrDesc.ycbcrModel ) << HashOf( _ycbcrDesc.ycbcrRange );
            result << HashOf( _ycbcrDesc.components );
            result << HashOf( _ycbcrDesc.xChromaOffset ) << HashOf( _ycbcrDesc.yChromaOffset );
            result << HashOf( _ycbcrDesc.chromaFilter ) << HashOf( _ycbcrDesc.forceExplicitReconstruction );
        }
        return result;
    }

} // AE::PipelineCompiler
