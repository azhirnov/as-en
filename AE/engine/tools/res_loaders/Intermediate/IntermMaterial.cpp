// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermMaterial.h"

namespace AE::ResLoader
{

/*
=================================================
    constructor
=================================================
*/
    IntermMaterial::IntermMaterial () __NE___
    {}

    IntermMaterial::IntermMaterial (IntermMaterial &&other) __NE___ :
        _settings{ RVRef(other._settings) },
        _params{ RVRef(other._params) },
        _floatData{ RVRef(other._floatData) },
        _float4Data{ RVRef(other._float4Data) },
        _textures{ RVRef(other._textures) }
    {}

    IntermMaterial::IntermMaterial (const IntermMaterial &other) __Th___ :
        _settings{ other._settings },
        _params{ other._params },
        _floatData{ other._floatData },
        _float4Data{ other._float4Data },
        _textures{ other._textures }
    {}

/*
=================================================
    Set (name)
=================================================
*/
    void  IntermMaterial::Set (StringView name) __Th___
    {
        _settings.name = String{name};
    }

/*
=================================================
    Set (ECullMode)
=================================================
*/
    void  IntermMaterial::Set (ECullMode mode) __NE___
    {
        _settings.cullMode = mode;
    }

/*
=================================================
    Set (BlendMode)
=================================================
*/
    void  IntermMaterial::Set (const BlendMode &mode) __NE___
    {
        _settings.blendMode = mode;
    }

/*
=================================================
    Set (float)
=================================================
*/
    bool  IntermMaterial::Set (EKey key, float value) __Th___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != Default )
            return false;  // already set

        _floatData.push_back( value );  // throw

        param.idx   = CheckCast<ubyte>(_floatData.size()-1);
        param.type  = EValueType::Float;
        return true;
    }

/*
=================================================
    Set (float4)
=================================================
*/
    bool  IntermMaterial::Set (EKey key, const float4 &value) __Th___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != Default )
            return false;  // already set

        _float4Data.push_back( value );  // throw

        param.idx   = CheckCast<ubyte>(_float4Data.size()-1);
        param.type  = EValueType::Float4;
        return true;
    }

/*
=================================================
    Set (MtrTexture)
=================================================
*/
    bool  IntermMaterial::Set (EKey key, MtrTexture &&value) __Th___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != Default )
            return false;  // already set

        _textures.push_back( RVRef(value) );  // throw

        param.idx   = CheckCast<ubyte>(_textures.size()-1);
        param.type  = EValueType::Texture;
        return true;
    }

/*
=================================================
    Get (float)
=================================================
*/
    bool  IntermMaterial::Get (EKey key, OUT float &value) C_NE___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != EValueType::Float )
            return false;

        value = _floatData[ param.idx ];
        return true;
    }

/*
=================================================
    Get (float4)
=================================================
*/
    bool  IntermMaterial::Get (EKey key, OUT float4 &value) C_NE___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != EValueType::Float4 )
            return false;

        value = _float4Data[ param.idx ];
        return true;
    }

    bool  IntermMaterial::Get (EKey key, OUT RGBA32f &value) C_NE___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != EValueType::Float4 )
            return false;

        value = RGBA32f{_float4Data[ param.idx ]};
        return true;
    }

/*
=================================================
    Get (MtrTexture)
=================================================
*/
    bool  IntermMaterial::Get (EKey key, OUT Ptr<const MtrTexture> &value) C_NE___
    {
        CHECK_ERR( key < EKey::_Count );
        auto&   param = _params[ uint(key) ];

        if ( param.type != EValueType::Texture )
            return false;

        value = &_textures[ param.idx ];
        return true;
    }

    Ptr<const IntermMaterial::MtrTexture>  IntermMaterial::Get (EKey key) C_NE___
    {
        Ptr<const MtrTexture>   ptr;
        return Get( key, OUT ptr ) ? ptr : null;
    }


} // AE::ResLoader

