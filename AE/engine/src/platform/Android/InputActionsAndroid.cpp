// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/InputActionsAndroid.h"

namespace AE::App
{

/*
=================================================
	constructor
=================================================
*/
	InputActionsAndroid::InputActionsAndroid (TsDoubleBufferedQueue* q) __NE___ :
		InputActionsBase{ q },
		_screenOrient{ Quat::Identity() },
		_gestureRecognizer{ ushort(EInputType::TouchPos),   ushort(EInputType::TouchPos_mm),
							ushort(EInputType::TouchDelta), ushort(EInputType::TouchDelta_norm), ushort(EInputType::MultiTouch) }
	{}

/*
=================================================
	Update
=================================================
*/
	void  InputActionsAndroid::Update (Duration_t timeSinceStart) __NE___
	{
		if_unlikely( _curMode == null )
			return;

		InputActionsBase::Update( timeSinceStart );

		_gestureRecognizer.Update( timeSinceStart, *this );
	}

/*
=================================================
	SetKey
=================================================
*/
	void  InputActionsAndroid::SetKey (jint key, EGestureState state, Duration_t timestamp, uint repeatCount, CharUtf32 unicodeChar) __NE___
	{
		DRC_EXLOCK( _drCheck );
		//ASSERT( repeatCount == 0 );	// TODO: for key up it always 0

		ControllerID	id = (EInputType(key) < EInputType::MouseBegin ? ControllerID::Keyboard : ControllerID::Mouse);

		_UpdateKey( EInputType(key), state, id, timestamp );

		if ( unicodeChar != CharUtf32{0} )
		{
			ASSERT( state == EGestureState::Begin );
			_AddChar( unicodeChar );
		}
	}

/*
=================================================
	SetTouch
=================================================
*/
	void  InputActionsAndroid::SetTouch (uint touchId, float x, float y, EGestureState state, Duration_t timestamp) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_gestureRecognizer.SetTouch( touchId, float2{x,y}, state, timestamp );
	}

/*
=================================================
	SetCursorPos
=================================================
*/
	void  InputActionsAndroid::SetCursorPos (float2 pos) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _curMode == null )
			return;

		const auto	id = ControllerID::Mouse;

		_Update2F( EInputType::CursorPos, EGestureType::Move, id, pos, EGestureState::Update );

		const float2	pos_mm = pos * _pixToMm;
		_Update2F( EInputType::CursorPos_mm, EGestureType::Move, id, pos_mm, EGestureState::Update );

		const float2	delta = pos - _cursorPosPx;
		_Update2F( EInputType::CursorDelta, EGestureType::Move, id, delta, EGestureState::Update );

		const float2	delta_norm = delta * _toSNorm;
		_Update2F( EInputType::CursorDelta_norm, EGestureType::Move, id, delta_norm, EGestureState::Update );

		_cursorPosPx	= pos;	// may be changed in 'CursorPosChanged()'
		_cursorDeltaPx	= delta;
	}

/*
=================================================
	SetMouseWheel
=================================================
*/
	void  InputActionsAndroid::SetMouseWheel (const float2 delta) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _curMode == null )
			return;

		const auto	id = ControllerID::Mouse;

		_Update2F( EInputType::MouseWheel, EGestureType::Move, id, delta, EGestureState::Update );
	}

/*
=================================================
	SetSensor
=================================================
*/
	void  InputActionsAndroid::SetSensor (jint sensorType, ArrayView<float> values) __NE___
	{
		const EInputType	input_type	= _SensorTypeToInputType( ESensorType(sensorType) );

		switch_enum( ESensorType(sensorType) )
		{
			case ESensorType::Accelerometer :
			case ESensorType::Gravity :
			case ESensorType::LinearAcceleration :
			case ESensorType::MagneticField :
			case ESensorType::Gyroscope :
			{
				ASSERT( values.size() >= 3 );

				// Android coordinate system for any orientation:
				// +X -- left
				// -X -- right
				// +Y -- bottom
				// -Y -- top
				// +Z -- back face
				// -Z -- screen (front face)

				float3	v {values[0], values[1], values[2]};
				v = _screenOrient * v;

				SetSensor3f( input_type, v );
				break;
			}

			case ESensorType::RotationVector :
			case ESensorType::GameRotationVector :
			{
				// values[3] is optional before API 18, and always presented after.
				ASSERT( values.size() >= 4 );

				// Android coordinate system: +Z toward to sun, +Y to north.
				// Engine (Vulkan) coordinate system: +Y down, +Z forward.
				Quat  q{ values[3], values[0], values[1], values[2] };
				q = q * _screenOrient;
				q = (q.Conjugate() * Quat{0.70710678f, 0.70710678f, 0.f, 0.f}).MirrorX();

				SetSensorQf( input_type, q );
				break;
			}

			case ESensorType::AmbientLight :
			case ESensorType::AirPressure :
			case ESensorType::Proximity :
			case ESensorType::RelativeHumidity :
			case ESensorType::AirTemperature :
			{
				ASSERT( values.size() >= 1 );
				SetSensor1f( input_type, values[0] );
				break;
			}

			case ESensorType::Pose6DOF :	// TODO
			case ESensorType::Unknown :

			default :					DBG_WARNING( "unknown sensor type" ); break;
		}
		switch_end
	}

/*
=================================================
	SetMonitor
=================================================
*/
	void  InputActionsAndroid::SetMonitor (const uint2 &surfaceSize, const Monitor &monitor) __NE___
	{
		_Setup( float2(surfaceSize), monitor.MillimetersPerPixel() );

		_screenOrient = Graphics::SurfaceTransformUtils::ToInvQuat( monitor.SurfaceTransform() );
	}

/*
=================================================
	Initialize
=================================================
*/
	void  InputActionsAndroid::Initialize (EnableSensorsFn_t fn) __NE___
	{
		_methods.enableSensors = RVRef(fn);

		if ( _curMode and _methods.enableSensors )
			_methods.enableSensors( BitCast<jint>( _curMode->enableSensors ));
	}

/*
=================================================
	SetMode
=================================================
*/
	bool  InputActionsAndroid::SetMode (InputModeName::Ref value) __NE___
	{
		DRC_EXLOCK( _drCheck );

		const auto	prev_mode	= _curMode;
		bool		res			= InputActionsBase::SetMode( value );

		if ( (prev_mode != _curMode) and _methods.enableSensors )
		{
			_methods.enableSensors( _curMode != null ? BitCast<jint>(_curMode->enableSensors) : 0 );
		}

		return res;
	}

/*
=================================================
	LoadSerialized
=================================================
*/
	bool  InputActionsAndroid::LoadSerialized (MemRefRStream &stream) __NE___
	{
		DRC_EXLOCK( _drCheck );
		_Reset();

		return SerializableInputActions::LoadSerialized( OUT _modeMap, _Version, InputActionsAndroid_Name, stream );
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  InputActionsAndroid::Serialize (Serializing::Serializer &ser) C_NE___
	{
		DRC_EXLOCK( _drCheck );

		return SerializableInputActions::Serialize( _modeMap, _Version, ser );
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  InputActionsAndroid::Deserialize (Serializing::Deserializer &des) __NE___
	{
		DRC_EXLOCK( _drCheck );
		_Reset();

		CHECK_ERR( SerializableInputActions::Deserialize( OUT _modeMap, _Version, des ));
		return true;
	}


} // AE::App

#endif // AE_PLATFORM_ANDROID
