// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Remote/RemoteInputActions.h"

namespace AE::App
{
	using namespace AE::Networking;

/*
=================================================
	Consume
=================================================
*/
	void  RemoteInputActions::_MsgConsumer::Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::RemoteControl );
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ ## _name_::UID :	_self._ ## _name_( *msg->As< CSMsg_ ## _name_ >() );	break;
				CASE( RemCtrl_EnableSensors )
				CASE( RemCtrl_SensorF )
				CASE( RemCtrl_GNS )
				#undef CASE
			}
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_RemCtrl_SensorF
=================================================
*/
	void  RemoteInputActions::_RemCtrl_SensorF (CSMsg_RemCtrl_SensorF const& msg) __NE___
	{
		EInputType		itype	= Default;
		EValueType		vtype	= Default;
		const float*	values	= msg.arr;

		switch ( msg.type )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, _api_, _val_, _and_code_ )	\
				case _bitIndex_ :																\
					itype	= EInputType::_type_;												\
					vtype	= EValueType::_val_;												\
					break;

			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR
		}

		switch ( vtype )
		{
			case EValueType::Float :	_IA().SetSensor1f( itype, values[0] );											break;
			case EValueType::Float3 :	_IA().SetSensor3f( itype, float3{ values[0], values[1], values[2] });			break;
			case EValueType::Quat :		_IA().SetSensorQf( itype, Quat{ values[0], values[1], values[2], values[3] });	break;
		}
		_sensors.connectionLostTimer.Restart();
	}

/*
=================================================
	_RemCtrl_GNS
=================================================
*/
	void  RemoteInputActions::_RemCtrl_GNS (CSMsg_RemCtrl_GNS const& msg) __NE___
	{
		_IA().SetGNS( msg.gns );
		_sensors.connectionLostTimer.Restart();
	}

/*
=================================================
	_RemCtrl_EnableSensors
=================================================
*/
	void  RemoteInputActions::_RemCtrl_EnableSensors (CSMsg_RemCtrl_EnableSensors const &msg) __NE___
	{
		_sensors.enabledBits = ConvertSensorBits( ESensorBits(msg.bits) );
		_sensors.connectionLostTimer.Restart();
	}

/*
=================================================
	ConvertSensorBits (ESensorBits)
=================================================
*/
	inline constexpr RemoteInputActions::ERemoteSensorBits&  operator |= (RemoteInputActions::ERemoteSensorBits &lhs, const RemoteInputActions::ERemoteSensorBits rhs) __NE___
	{
		lhs = BitCast<RemoteInputActions::ERemoteSensorBits>( ToNearUInt( lhs ) | ToNearUInt( rhs ));
		return lhs;
	}

	RemoteInputActions::ERemoteSensorBits  RemoteInputActions::ConvertSensorBits (ESensorBits src) __NE___
	{
	#ifdef AE_ENABLE_GLFW
		ERemoteSensorBits	dst = Default;
		for (auto src_bit : Base::BitIndexIterate<ESensorType>( src ))
		{
			switch_enum ( src_bit )
			{
				#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )\
					case ESensorType::Android_ ## _type_ : dst |= ERemoteSensorBits(1u << uint(ERemoteSensorType::_type_)); break;

				AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
				#undef AE_ANDROID_SERNSORS_VISITOR

				case ESensorType::Unknown :	break;
			}
			switch_end
		}
		return dst;
	#endif

	#ifdef AE_PLATFORM_ANDROID
		return ERemoteSensorBits(src);
	#endif
	}

/*
=================================================
	ConvertSensorBits (ERemoteSensorBits)
=================================================
*/
	RemoteInputActions::ESensorBits  RemoteInputActions::ConvertSensorBits (ERemoteSensorBits src) __NE___
	{
	#ifdef AE_ENABLE_GLFW
		ESensorBits	dst = Default;
		for (auto src_bit : Base::BitIndexIterate<ERemoteSensorType>( src ))
		{
			switch_enum ( src_bit )
			{
				#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )\
					case ERemoteSensorType::_type_ : dst |= ESensorBits(1u << uint(ESensorType::Android_ ## _type_)); break;

				AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
				#undef AE_ANDROID_SERNSORS_VISITOR

				case ERemoteSensorType::Unknown :	break;
			}
			switch_end
		}
		return dst;
	#endif

	#ifdef AE_PLATFORM_ANDROID
		return ESensorBits(src);
	#endif
	}


} // AE::App
