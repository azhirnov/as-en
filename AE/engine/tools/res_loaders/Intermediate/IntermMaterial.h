// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
    using Graphics::EAddressMode;
    using Graphics::EFilter;
    using Graphics::ECullMode;
    using Graphics::EBorderColor;
    using Graphics::EBlendFactor;
    using Graphics::EBlendOp;



    //
    // Intermediate Material
    //

    class IntermMaterial final : public EnableRC<IntermMaterial>
    {
    // types
    public:
        enum class ETextureMapping : ubyte
        {
            UV,
            Sphere,
            Cylinder,
            Box,
            Plane,
            Unknown = 0xFF,
        };

        enum class EKey : ubyte
        {
            Unknown     = 0,

            // Non-PBR materials
            Diffuse,                    // texture or color
            Specular,                   // texture or color
            Ambient,                    // texture or color
            Emissive,                   // texture or color
            HeightMap,                  // texture
            NormalMap,                  // texture
            Shininess,                  // texture or float
            Opacity,                    // texture or float
            DisplacementMap,            // texture
            LightMap,                   // texture
            ReflectionMap,              // texture
            AlphaTestReference,         // float
            Transparency,               // float
            BumpScale,                  // float
            Reflectivity,               // float
            ShininessStrength,          // float
            Transparent,                // color
            Reflective,                 // color

            // PBR material
            PBR_BaseColor,              // texture or color
            PBR_NormalMap,              // texture
            PBR_Emission,               // texture or color
            PBR_Roughness,              // texture or float
            PBR_AmbientOcclusionMap,    // texture
            PBR_Sheen,                  // ???
            PBR_Clearcoat,              // ???
            PBR_Transmission,           // ???
            PBR_Metalness,              // texture or float
            PBR_Subsurface,             // texture or float
            PBR_IndexOfRefraction,      // texture, float, float array
            PBR_OpticalDepth,           // 3D texture or float

            _Count
        };

        enum class EValueType : ubyte
        {
            Unknown     = 0,
            Float,
            Float4,     // == color
            Texture,    // == MtrTexture
            _Count
        };

        struct MtrTexture
        {
            RC<IntermImage>     image;
            String              name;
            float4              valueScale      {1.f};          // texture * color
            packed_float3x3     uvTransform     = packed_float3x3::Identity();
            ETextureMapping     mapping         = Default;
            EAddressMode        addressModeU    = Default;
            EAddressMode        addressModeV    = Default;
            EAddressMode        addressModeW    = Default;
            EFilter             filter          = Default;
            EBorderColor        borderColor     = Default;
            ubyte               uvIndex         = UMax;         // == texcord index

            MtrTexture ()   __NE___ = default;
        };

        struct BlendMode
        {
            EBlendFactor        src     = Default;
            EBlendFactor        dst     = Default;
            EBlendOp            op      = Default;

            BlendMode ()                    __NE___ = default;
            ND_ explicit operator bool ()   C_NE___ { return src != Default and dst != Default and op != Default; }
        };

        struct Settings
        {
            String              name;
            ECullMode           cullMode    = ECullMode::Back;
            bool                wireframe   = false;
            float               alphaCutoff = 1.f;
            BlendMode           blendMode;

            Settings ()                 __NE___ = default;
            ND_ bool  HasAlphaTest ()   C_NE___ { return alphaCutoff < 1.f; }
            ND_ bool  IsTranslucent ()  C_NE___ { return bool{blendMode}; }
        };

    private:
        struct Parameter
        {
            EValueType          type    = Default;
            ubyte               idx     = UMax;

            Parameter ()    __NE___ = default;
        };

        using Parameters_t = StaticArray< Parameter, uint(EKey::_Count) >;


    // variables
    private:
        Settings            _settings;
        Parameters_t        _params     = {};

        Array<float>        _floatData;
        Array<float4>       _float4Data;
        Array<MtrTexture>   _textures;


    // methods
    public:
        IntermMaterial ()                                           __NE___;
        IntermMaterial (IntermMaterial &&)                          __NE___;
        IntermMaterial (const IntermMaterial &)                     __Th___;

            void  Set (StringView name)                             __Th___;
            void  Set (ECullMode mode)                              __NE___;
            void  Set (const BlendMode &mode)                       __NE___;

            bool  Set (EKey key, float value)                       __Th___;
            bool  Set (EKey key, const float4 &value)               __Th___;
            bool  Set (EKey key, const RGBA32f &value)              __Th___ { return Set( key, float4(value) ); }
            bool  Set (EKey key, MtrTexture &&value)                __Th___;

        ND_ bool  Get (EKey key, OUT float &value)                  C_NE___;
        ND_ bool  Get (EKey key, OUT float4 &value)                 C_NE___;
        ND_ bool  Get (EKey key, OUT RGBA32f &value)                C_NE___;
        ND_ bool  Get (EKey key, OUT Ptr<const MtrTexture> &value)  C_NE___;
        ND_ Ptr<const MtrTexture>  Get (EKey key)                   C_NE___;

        ND_ StringView          Name ()                             C_NE___ { return _settings.name; }
        ND_ Settings const&     GetSettings ()                      C_NE___ { return _settings; }
        ND_ Settings &          EditSettings ()                     __NE___ { return _settings; }
    };


} // AE::ResLoader
