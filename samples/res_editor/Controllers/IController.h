// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Dynamic/DynamicDimension.h"

namespace AE::ShaderTypes {
    struct CameraData;
}
namespace AE::ResEditor
{

    //
    // Controller interface
    //

    class IController : public EnableRC<IController>
    {
    // types
    public:
        struct MovingScale
        {
            float   forward     = 1.f;
            float   backward    = 1.f;
            float   side        = 1.f;
            float   up          = 1.f;
            float   down        = 1.f;

            MovingScale () {}

            ND_ float2  Apply (const float2 &) C_NE___;
            ND_ float3  Apply (const float3 &) C_NE___;
        };

        using Frustum_t     = FrustumTempl<float>;
        using RaysGrid_t    = App::RaysGrid;


    // config
    protected:
        static constexpr float      _minZoom    = 0.1f;
        static constexpr float      _maxZoom    = 10.f;


    // interface
    public:
            virtual void  ProcessInput (ActionQueueReader, secondsf dt)         __NE___ = 0;

        ND_ virtual InputModeName   GetInputMode ()                             C_NE___ = 0;

        ND_ virtual float3          GetPosition ()                              C_NE___ = 0;

        ND_ virtual float4x4        GetViewProj ()                              C_NE___ = 0;
        ND_ virtual float4x4        GetInvViewProj ()                           C_NE___ { return GetViewProj().Inversed(); }

        ND_ virtual float4x4        GetProj ()                                  C_NE___ = 0;
    //  ND_ virtual float4x4        GetInvProj ()                               C_NE___ { return GetProj().Inversed(); }

        ND_ virtual float4x4        GetView ()                                  C_NE___ = 0;
    //  ND_ virtual float4x4        GetInvView ()                               C_NE___ { return GetView().Inversed(); }

        ND_ virtual float2          GetClipPlanes ()                            C_NE___ = 0;

    //  ND_ virtual RaysGrid_t      GetRaysGrid ()                              C_NE___ = 0;

        ND_ virtual StringView      GetHelpText ()                              C_NE___ = 0;

            virtual void            CopyTo (OUT AE::ShaderTypes::CameraData &)  C_NE___ = 0;

    protected:
            template <typename T>
            void  _CopyToCameraData (OUT T &, const Frustum_t &)                C_NE___;
    };



/*
=================================================
    MovingScale::Apply
=================================================
*/
    inline float2  IController::MovingScale::Apply (const float2 &v) C_NE___
    {
        if ( All( IsZero( v )))
            return float2{ 0.f };

        float2  res = Normalize( v );
        res.x *= (v.x < 0.f ? backward : forward);
        res.y *= side;
        return res;
    }

/*
=================================================
    MovingScale::Apply
=================================================
*/
    inline float3  IController::MovingScale::Apply (const float3 &v) C_NE___
    {
        if ( All( IsZero( v )))
            return float3{ 0.f };

        float3  res = Normalize( v );
        res.x *= (v.x < 0.f ? backward : forward);
        res.y *= side;
        res.z *= (v.z < 0.f ? up : down);
        return res;
    }

/*
=================================================
    _CopyToCameraData
=================================================
*/
    template <typename T>
    void  IController::_CopyToCameraData (OUT T &camera, const Frustum_t &frustum) C_NE___
    {
        camera.viewProj     = GetViewProj();
        camera.invViewProj  = GetInvViewProj();
        camera.proj         = GetProj();
        camera.view         = GetView();
        camera.clipPlanes   = GetClipPlanes();
        camera.pos          = GetPosition();

        for (uint i = 0; i < 6; ++i) {
            camera.frustum[i] = float4{frustum.GetPlane( i )};
        }
    }


} // AE::ResEditor
