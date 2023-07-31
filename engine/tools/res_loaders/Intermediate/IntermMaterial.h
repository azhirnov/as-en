// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
    using Graphics::EAddressMode;
    using Graphics::EFilter;
    using Graphics::ECullMode;



    //
    // Intermediate Material
    //

    class IntermMaterial final : public EnableRC<IntermMaterial>
    {
    // types
    public:
        enum class ETextureMapping : uint
        {
            UV,
            Sphere,
            Cylinder,
            Box,
            Plane,
            Unknown = ~0u,
        };

        struct MtrTexture
        {
            RC<IntermImage>     image;
            String              name;
            float3x3            uvTransform     = float3x3::Identity();
            ETextureMapping     mapping         = Default;
            EAddressMode        addressModeU    = Default;
            EAddressMode        addressModeV    = Default;
            EAddressMode        addressModeW    = Default;
            EFilter             filter          = Default;
            uint                uvIndex         = UMax;         // == texcord index
        };

        using Parameter = Union< NullUnion, float, float3, RGBA32f, MtrTexture >;

        struct Settings
        {
            String              name;

            Parameter           albedo;                         // texture or color             // UE4 'base color'
            Parameter           specular;                       // texture or color             // UE4 'specular'
            Parameter           ambient;                        // texture or color
            Parameter           emissive;                       // texture or color
            Parameter           heightMap;                      // texture
            Parameter           normalMap;                      // texture
            Parameter           shininess;                      // texture or float
            Parameter           opacity;                        // textire or float
            Parameter           displacementMap;                // texture
            Parameter           lightMap;                       // texture
            Parameter           reflectionMap;                  // texture
            Parameter           roughtness;                     // texture or float             // UE4 'roughtness'
            Parameter           metallic;                       // texture or float             // UE4 'metallic'
            Parameter           subsurface;                     // texture or color             // UE4 'subsurface color'
            Parameter           ambientOcclusion;               // texture                      // UE4 'ambient occlusion'
            Parameter           refraction;                     // texture, float, float3       // UE4 'index of refraction'
            Parameter           opticalDepth;                   // 3d texture, float, float3    //... exp( -distance * opticalDepth )
            float               shininessStrength   = 0.0f;
            float               alphaTestReference  = 0.0f;
            ECullMode           cullMode            = Default;
        };


    // variables
    private:
        Settings        _settings;


    // methods
    public:
        IntermMaterial ()                                       __NE___ {}
        explicit IntermMaterial (Settings settings)             __NE___ : _settings{ RVRef(settings) } {}

        ND_ Settings const&     GetSettings ()                  C_NE___ { return _settings; }
        ND_ Settings &          EditSettings ()                 __NE___ { return _settings; }
    };


} // AE::ResLoader
