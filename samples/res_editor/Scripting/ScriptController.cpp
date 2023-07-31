// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Controllers/ScaleBiasCamera.h"
#include "res_editor/Controllers/TopDownCamera.h"
#include "res_editor/Controllers/IsometricCamera.h"
#include "res_editor/Controllers/FlightCamera.h"
#include "res_editor/Controllers/FPSCamera.h"
#include "res_editor/Controllers/FPVCamera.h"

namespace AE::ResEditor
{
namespace
{
/*
=================================================
    ScriptBaseController_ToBase
=================================================
*/
    template <typename T>
    static ScriptBaseController*  ScriptBaseController_ToBase (T* ptr)
    {
        STATIC_ASSERT( IsBaseOf< ScriptBaseController, T >);

        ScriptBaseControllerPtr result{ ptr };
        return result.Detach();
    }
}
//-----------------------------------------------------------------------------


    using namespace AE::Scripting;

/*
=================================================
    Bind
=================================================
*/
    void  ScriptBaseController::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptBaseController>   binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
    }

    template <typename B>
    void  ScriptBaseController::_BindBase (B &binder) __Th___
    {
        using T = typename B::Class_t;

        binder.Operators().ImplCast( &ScriptBaseController_ToBase<T> );

        binder.AddMethod( &ScriptBaseController::_Dimension,    "Dimension" );
        binder.AddMethod( &ScriptBaseController::_SetDimension, "Dimension" );
    }

/*
=================================================
    _SetDimension
=================================================
*/
    void  ScriptBaseController::_SetDimension (const ScriptDynamicDimPtr &dynDim) __Th___
    {
        CHECK_THROW_MSG( dynDim and dynDim->Get() );
        CHECK_THROW_MSG( not _dynamicDim, "Dimension is already set" );

        _dynamicDim = dynDim;
    }

    void  ScriptBaseController::SetDimensionIfNotSet (const ScriptDynamicDimPtr &dynDim) __Th___
    {
        if ( not _dynamicDim )
        {
            CHECK_THROW_MSG( dynDim and dynDim->Get() );

            _dynamicDim = dynDim;
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  ScriptControllerScaleBias::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptControllerScaleBias>  binder{ se };
        binder.CreateRef();
        _BindBase( binder );
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptControllerScaleBias::ToController () __Th___
    {
        CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

        if ( _controller )
            return _controller;

        _controller = MakeRC<ScaleBiasCamera>( _dynamicDim->Get() );
        return _controller;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  ScriptControllerTopDown::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptControllerTopDown>  binder{ se };
        binder.CreateRef();
        binder.AddMethod( &ScriptControllerTopDown::ForwardBackwardScale1,      "ForwardBackwardScale" );
        binder.AddMethod( &ScriptControllerTopDown::ForwardBackwardScale2,      "ForwardBackwardScale" );
        binder.AddMethod( &ScriptControllerTopDown::SideMovementScale,          "SideMovementScale" );
        binder.AddMethod( &ScriptControllerTopDown::SetRotationScale,           "RotationScale" );
        binder.AddMethod( &ScriptControllerTopDown::SetPosition,                "Position" );
        _BindBase( binder );
    }

/*
=================================================
    SetRotationScale
=================================================
*/
    void  ScriptControllerTopDown::SetRotationScale (float value) __Th___
    {
        _rotationScale = value;
    }

/*
=================================================
    ForwardBackwardScale*
=================================================
*/
    void  ScriptControllerTopDown::ForwardBackwardScale1 (float value) __Th___
    {
        _movingScale.forward  = value;
        _movingScale.backward = value;
    }

    void  ScriptControllerTopDown::ForwardBackwardScale2 (float forward, float backward) __Th___
    {
        _movingScale.forward  = forward;
        _movingScale.backward = backward;
    }

/*
=================================================
    SideMovementScale
=================================================
*/
    void  ScriptControllerTopDown::SideMovementScale (float value) __Th___
    {
        _movingScale.side = value;
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptControllerTopDown::ToController () __Th___
    {
        CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

        if ( _controller )
            return _controller;

        _controller = MakeRC<TopDownCamera>( _dynamicDim->Get(), _movingScale, _rotationScale, _initialPos );
        return _controller;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    SetFovY
=================================================
*/
    void  ScriptControllerIsometricCamera::SetFovY (float value) __Th___
    {
        CHECK_THROW_MSG( value >= 1.0f );
        CHECK_THROW_MSG( value <= 90.0f );

        _fovY = value;
    }

/*
=================================================
    SetClipPlanes
=================================================
*/
    void  ScriptControllerIsometricCamera::SetClipPlanes (float near, float far) __Th___
    {
        CHECK_THROW_MSG( near > 0.0f );
        CHECK_THROW_MSG( near < far );
        CHECK_THROW_MSG( (far - near) > 1.0f );

        _clipPlanes = float2{ near, far };
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptControllerIsometricCamera::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptControllerIsometricCamera>  binder{ se };
        binder.CreateRef();
        _BindBase( binder );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetFovY,                "FovY" );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetClipPlanes,          "ClipPlanes" );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetRotationScale1,      "RotationScale" );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetRotationScale2,      "RotationScale" );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetOffsetScale,         "OffsetScale" );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetPosition,            "Position" );
        binder.AddMethod( &ScriptControllerIsometricCamera::SetOffset,              "Offset" );
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptControllerIsometricCamera::ToController () __Th___
    {
        CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

        if ( _controller )
            return _controller;

        _controller = MakeRC<IsometricCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ),
                                                _rotationScale, _offsetScale, _initialPos, _initialOffset );
        return _controller;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    SetFovY
=================================================
*/
    void  ScriptControllerCamera3D::SetFovY (float value) __Th___
    {
        CHECK_THROW_MSG( value >= 1.0f );
        CHECK_THROW_MSG( value <= 90.0f );

        _fovY = value;
    }

/*
=================================================
    SetClipPlanes
=================================================
*/
    void  ScriptControllerCamera3D::SetClipPlanes (float near, float far) __Th___
    {
        CHECK_THROW_MSG( near > 0.0f );
        CHECK_THROW_MSG( near < far );
        CHECK_THROW_MSG( (far - near) > 1.0f );

        _clipPlanes = float2{ near, far };
    }

/*
=================================================
    ForwardBackwardScale*
=================================================
*/
    void  ScriptControllerCamera3D::ForwardBackwardScale1 (float value) __Th___
    {
        _movingScale.forward  = value;
        _movingScale.backward = value;
    }

    void  ScriptControllerCamera3D::ForwardBackwardScale2 (float forward, float backward) __Th___
    {
        _movingScale.forward  = forward;
        _movingScale.backward = backward;
    }

/*
=================================================
    UpDownScale*
=================================================
*/
    void  ScriptControllerCamera3D::UpDownScale1 (float value) __Th___
    {
        _movingScale.up   = value;
        _movingScale.down = value;
    }

    void  ScriptControllerCamera3D::UpDownScale2 (float up, float down) __Th___
    {
        _movingScale.up   = up;
        _movingScale.down = down;
    }

/*
=================================================
    SideMovementScale
=================================================
*/
    void  ScriptControllerCamera3D::SideMovementScale (float value) __Th___
    {
        _movingScale.side = value;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    SetEngineThrustRange
=================================================
*/
    void  ScriptControllerFlightCamera::SetEngineThrustRange (float min, float max) __Th___
    {
        CHECK_THROW_MSG( min < max );
        CHECK_THROW_MSG( max > 0.f );
        CHECK_THROW_MSG( min <= 0.f );

        _engineThrustRange.x = min;
        _engineThrustRange.y = max;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptControllerFlightCamera::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptControllerFlightCamera>  binder{ se };
        binder.CreateRef();
        binder.AddMethod( &ScriptControllerCamera3D::SetFovY,                   "FovY" );
        binder.AddMethod( &ScriptControllerCamera3D::SetClipPlanes,             "ClipPlanes" );
        binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale1,         "RotationScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale3,         "RotationScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetPosition,               "Position" );
        binder.AddMethod( &ScriptControllerFlightCamera::SetEngineThrustRange,  "EngineThrust" );
        _BindBase( binder );
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptControllerFlightCamera::ToController () __Th___
    {
        CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

        if ( _controller )
            return _controller;

        _controller = MakeRC<FlightCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ),
                                            _engineThrustRange, _rotationScale, _initialPos );
        return _controller;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  ScriptControllerFPVCamera::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptControllerFPVCamera>  binder{ se };
        binder.CreateRef();
        binder.AddMethod( &ScriptControllerCamera3D::SetFovY,               "FovY" );
        binder.AddMethod( &ScriptControllerCamera3D::SetClipPlanes,         "ClipPlanes" );
        binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale1, "ForwardBackwardScale" );
        binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale2, "ForwardBackwardScale" );
        binder.AddMethod( &ScriptControllerCamera3D::UpDownScale1,          "UpDownScale" );
        binder.AddMethod( &ScriptControllerCamera3D::UpDownScale2,          "UpDownScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SideMovementScale,     "SideMovementScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale1,     "RotationScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale2,     "RotationScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetPosition,           "Position" );
        _BindBase( binder );
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptControllerFPVCamera::ToController () __Th___
    {
        CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

        if ( _controller )
            return _controller;

        _controller = MakeRC<FPSCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
                                         float2{_rotationScale}, _initialPos );
        return _controller;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  ScriptControllerFreeCamera::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptControllerFreeCamera>  binder{ se };
        binder.CreateRef();
        binder.AddMethod( &ScriptControllerCamera3D::SetFovY,               "FovY" );
        binder.AddMethod( &ScriptControllerCamera3D::SetClipPlanes,         "ClipPlanes" );
        binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale1, "ForwardBackwardScale" );
        binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale2, "ForwardBackwardScale" );
        binder.AddMethod( &ScriptControllerCamera3D::UpDownScale1,          "UpDownScale" );
        binder.AddMethod( &ScriptControllerCamera3D::UpDownScale2,          "UpDownScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SideMovementScale,     "SideMovementScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale1,     "RotationScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale2,     "RotationScale" );
        binder.AddMethod( &ScriptControllerCamera3D::SetPosition,           "Position" );
        _BindBase( binder );
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptControllerFreeCamera::ToController () __Th___
    {
        CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

        if ( _controller )
            return _controller;

        _controller = MakeRC<FPVCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
                                         float2{_rotationScale}, _initialPos );
        return _controller;
    }


} // AE::ResEditor
