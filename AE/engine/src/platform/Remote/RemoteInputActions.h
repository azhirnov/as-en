// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Currently supported:
		Android <-> Android
		GLFW	<-> Android
*/

#pragma once

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/InputActionsAndroid.h"
#endif
#ifdef AE_ENABLE_GLFW
# include "platform/GLFW/InputActionsGLFW.h"
#endif
#ifdef AE_WINAPI_WINDOW
# include "platform/WinAPI/InputActionsWinAPI.h"
#endif
#include "platform/Android/SerializableInputActionsAndroid.h"
#include "platform/Remote/Messages.h"

namespace AE::App
{

	//
	// Remote Input Actions
	//

	class RemoteInputActions
	{
	// types
	public:
		using EValueType		= IInputActions::EValueType;
		using ESensorBits		= SerializableInputActions::ESensorBits;

		enum  ERemoteSensorBits	: uint { Unknown = 0 };
		using ERemoteSensorType	= SerializableInputActionsAndroid::ESensorType;

	  #ifdef AE_PLATFORM_ANDROID
		using EInputType		= SerializableInputActionsAndroid::EInputType;
		using ESensorType		= SerializableInputActionsAndroid::ESensorType;
		using DstInputActions	= InputActionsAndroid;
	  #endif
	  #ifdef AE_ENABLE_GLFW
		using EInputType		= SerializableInputActionsGLFW::EInputType;
		using ESensorType		= SerializableInputActionsGLFW::ESensorType;
		using DstInputActions	= InputActionsGLFW;
	  #endif
	  #ifdef AE_WINAPI_WINDOW
		using EInputType		= SerializableInputActionsWinAPI::EInputType;
		using ESensorType		= int;	// not supported
		using DstInputActions	= InputActionsWinAPI;
	  #endif

	private:
		class _MsgConsumer final : public Networking::ICSMessageConsumer
		{
		private:
			RemoteInputActions &	_self;

		public:
			_MsgConsumer (RemoteInputActions &self)						__NE___	: _self{self} {}
			Networking::CSMessageGroupID  GetGroupID ()					C_NE_OV	{ return CSMessageGroup::RemoteControl; }
			void  Consume (ChunkList<const Networking::CSMessagePtr>)	__NE_OV;
		};


	// variables
	private:
		Ptr<IInputActions>			_dstIA;
		StaticRC<_MsgConsumer>		_msgConsumer;

		struct {
			Timer						connectionLostTimer		{seconds{5}};
			ERemoteSensorBits			enabledBits				= Default;	// from response
		}							_sensors;


	// methods
	public:
		RemoteInputActions ()													__NE___	: _msgConsumer{*this} {}

			void  Init (IInputActions &dstIA)									__NE___	{ _dstIA = &dstIA; }

			template <typename MsgProducer>
			void  EnableSensors (MsgProducer &, ERemoteSensorBits)				__NE___;

			template <typename MsgProducer>
			void  EnableSensors (MsgProducer &msgProducer)						__NE___;

		ND_ RC<Networking::ICSMessageConsumer>	GetMsgConsumer ()				__NE___	{ return _msgConsumer.GetRC(); }


		ND_ static ERemoteSensorBits	ConvertSensorBits (ESensorBits)			__NE___;
		ND_ static ESensorBits			ConvertSensorBits (ERemoteSensorBits)	__NE___;

	private:
		ND_ DstInputActions&  _IA ()											__NE___	{ return *Cast<DstInputActions>( _dstIA ); }

		void  _RemCtrl_EnableSensors (Networking::CSMsg_RemCtrl_EnableSensors const&)	__NE___;
		void  _RemCtrl_SensorF (Networking::CSMsg_RemCtrl_SensorF const&)				__NE___;
		void  _RemCtrl_GNS (Networking::CSMsg_RemCtrl_GNS const&)						__NE___;
	};


/*
=================================================
	EnableSensors
=================================================
*/
	template <typename MsgProducer>
	void  RemoteInputActions::EnableSensors (MsgProducer &msgProducer, ERemoteSensorBits bits) __NE___
	{
		using namespace AE::Networking;
		StaticAssert( IsBaseOf< ICSMessageProducer, MsgProducer >);

		if ( _sensors.connectionLostTimer.Tick() )
			_sensors.enabledBits = Default;

		if ( _sensors.enabledBits != bits )
		{
			auto	msg = msgProducer.template CreateMsg< CSMsg_RemCtrl_EnableSensors >();
			if ( msg ){
				msg->bits = uint(bits);
				CHECK( msgProducer.AddMessage( msg ));
			}
		}
	}

	template <typename MsgProducer>
	void  RemoteInputActions::EnableSensors (MsgProducer &msgProducer) __NE___
	{
		EnableSensors( msgProducer, ConvertSensorBits( _IA().RequiredSensorBits() ));
	}


} // AE::App
