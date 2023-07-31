// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{

    //
    // Intermediate Light
    //

    class IntermLight final : public EnableRC<IntermLight>
    {
    // types
    public:
        enum class ELightType
        {
            None,
            Point,
            Spot,
            Directional,
            Ambient,
            Area,
        };

        struct Settings
        {
            float3          position;
            float3          direction;
            float3          upDirection     = {0.0f, 0.0f, 1.0f};

            float3          attenuation;
            ELightType      type            = ELightType::None;
            bool            castShadow      = false;

            float3          diffuseColor;
            float3          specularColor;
            float3          ambientColor;

            float2          coneAngleInnerOuter;
        };


    // variables
    private:
        Settings        _settings;


    // methods
    public:
        IntermLight ()                              __NE___ {}
        explicit IntermLight (Settings settings)    __NE___ : _settings{RVRef(settings)} {}
    };


} // AE::ResLoader
