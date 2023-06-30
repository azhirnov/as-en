// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Controllers/Controller2D.h"
#include "res_editor/Controllers/FlightCamera.h"
#include "res_editor/Controllers/FPSCamera.h"
#include "res_editor/Controllers/FPVCamera.h"

#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"

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
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  ScriptController2D::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptController2D>  binder{ se };
        binder.CreateRef();
        _BindBase( binder );
    }

/*
=================================================
    ToController
=================================================
*/
    RC<IController>  ScriptController2D::ToController (RC<DynamicDim> dim) __Th___
    {
        return MakeRC<Controller2D>( RVRef(dim) );
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
    SetRotationScale*
=================================================
*/
    void  ScriptControllerCamera3D::SetRotationScale1 (float value) __Th___
    {
        _rotationScale = float3{value};
    }

    void  ScriptControllerCamera3D::SetRotationScale2 (float x, float y) __Th___
    {
        _rotationScale = float3{x,y,0.f};
    }

    void  ScriptControllerCamera3D::SetRotationScale3 (float x, float y, float z) __Th___
    {
        _rotationScale = float3{x,y,z};
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

/*
=================================================
    SetPosition
=================================================
*/
    void  ScriptControllerCamera3D::SetPosition (const packed_float3 &pos) __Th___
    {
        _initialPos = pos;
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
    RC<IController>  ScriptControllerFlightCamera::ToController (RC<DynamicDim> dim) __Th___
    {
        return MakeRC<FlightCamera>( RVRef(dim), _clipPlanes, Rad::FromDeg( _fovY ),
                                     _engineThrustRange, _rotationScale, _initialPos );
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
    RC<IController>  ScriptControllerFPVCamera::ToController (RC<DynamicDim> dim) __Th___
    {
        return MakeRC<FPSCamera>( RVRef(dim), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
                                  float2{_rotationScale}, _initialPos );
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
    RC<IController>  ScriptControllerFreeCamera::ToController (RC<DynamicDim> dim) __Th___
    {
        return MakeRC<FPVCamera>( RVRef(dim), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
                                      float2{_rotationScale}, _initialPos );
    }


} // AE::ResEditor
