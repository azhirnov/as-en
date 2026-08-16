// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "pch/Networking.h"

#include "platform/Android/AndroidCommon.h"
#include "platform/Android/Java.h"

#include "profiler/Remote/RemoteArmProfiler.h"
#include "profiler/Remote/RemoteMaliProfiler.h"
#include "profiler/Remote/RemoteAdrenoProfiler.h"
#include "profiler/Remote/RemotePowerVRProfiler.h"
#include "profiler/Remote/RemoteGeneralProfiler.h"

#include "platform/Remote/Messages.h"

#include "base/../../GlobalConfig.h"	// TODO: remove

using namespace AE;
using namespace AE::Base;
using namespace AE::Threading;
using namespace AE::Networking;

namespace
{
	class MsgProducer final : public IAsyncCSMessageProducer
	{
	public:
		MsgProducer ()							__NE___ :
			IAsyncCSMessageProducer{ Tag< LfLinearAllocator< usize{4_MiB}, usize{8_b}, 4 >>{} }
		{}

		EnumSet<EChannel>  GetChannels ()	C_NE_OV	{ return {EChannel::Reliable}; }

		void  SendLog (StringView text);
	};

/*
=================================================
	SendLog
=================================================
*/
	void  MsgProducer::SendLog (StringView text)
	{
		auto	msg = CreateMsg< CSMsg_Log >( StringSizeOf(text) );
		CHECK_ERRV( msg );

		msg->msg	= msg.PutExtra( text );
		msg->level	= ELogLevel::Info;

		CHECK( AddMessage( msg ));
	}
//-----------------------------------------------------------------------------



	class MsgConsumer final : public ICSMessageConsumer
	{
	public:
		MsgConsumer ()										__NE___ {}
		CSMessageGroupID  GetGroupID ()						C_NE_OV	{ return CSMessageGroup::RemoteControl; }
		void  Consume (ChunkList<const CSMessagePtr>)		__NE_OV;

	private:
		void  _Log (CSMsg_Log const& msg)							{ msg.Execute(); }
		void  _RemCtrl_EnableSensors (CSMsg_RemCtrl_EnableSensors const&);
	};

/*
=================================================
	Consume
=================================================
*/
	void  MsgConsumer::Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::Debug or
					msg->GroupId() == CSMessageGroup::RemoteControl );

			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ ## _name_::UID :	_ ## _name_( *msg->As< CSMsg_ ## _name_ >() );	break;
				CASE( Log )
				CASE( RemCtrl_EnableSensors )
				default :					DBG_WARNING( "unknown message id" ); break;
				#undef CASE
			}
		}
	}
//-----------------------------------------------------------------------------

	#ifdef AE_PLATFORM_ANDROID
	using Java::JavaMethod;
	#endif



	class Client final : public BaseClient
	{
	// variables
	private:
		FrameUID						_frameId;

		StaticRC<MsgProducer>			_msgProducer;
		StaticRC<MsgConsumer>			_msgConsumer;

		Profiler::ArmProfilerServer		_armProf;
		Profiler::MaliProfilerServer	_maliProf;
		Profiler::AdrenoProfilerServer	_adrenoProf;
		Profiler::PowerVRProfilerServer	_pvrProf;
		Profiler::GeneralProfilerServer	_genProf;

		#ifdef AE_PLATFORM_ANDROID
		struct {
			JavaMethod< jint (jint) >		enableSensors;
		}								_java;
		#endif


	// methods
	public:
		Client () {}

		#ifdef AE_PLATFORM_ANDROID
		void  Start (JavaMethod< jint (jint) >	enableSensors);
		#endif

		void  Stop ();
		void  Update ();

		// from Java
		void  SendLog (StringView log);
		void  SendSensor (int sensor, ArrayView<float> values);
		void  SendGNS (double lat, double lon, double alt, ulong time, float bearing, float speed,
					   float horAcc, float vertAcc, float bearingAcc, float speedAcc);

		// call Java
		void  EnableSensors (uint bits);
	};
	static Client	s_Client;


/*
=================================================
	Start
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
	void  Client::Start (JavaMethod< jint (jint) >	enableSensors)
	{
		_java.enableSensors	= RVRef(enableSensors);

		_frameId = FrameUID::Init( 2 );

		auto	mf = MakeRC<MessageFactory>();

		CHECK_FATAL( _Initialize( mf, MakeRC<DefaultServerProviderV1>( AE_ANDREMCTRL_SERVER_IPv4 ), null, _frameId ));

		CHECK_FATAL( Register_RemoteControl( *mf ));
		CHECK_FATAL( Register_RemoteProfilers( *mf ));
		CHECK_FATAL( mf->Register( CSMessageGroup::Debug, {}, True{"lock group"} ));

		CHECK_FATAL( _AddChannelReliableTCP() );

		CHECK_FATAL( Add( _msgProducer->GetRC() ));
		CHECK_FATAL( Add( _msgConsumer->GetRC() ));

		Unused( _genProf.Initialize( *this, _msgProducer.GetRC() ));
		Unused( _pvrProf.Initialize( *this, _msgProducer.GetRC() ));
		Unused( _armProf.Initialize( *this, _msgProducer.GetRC() ));
		Unused( _maliProf.Initialize( *this, _msgProducer.GetRC() ));
		Unused( _adrenoProf.Initialize( *this, _msgProducer.GetRC() ));

		//SendLog( CpuArchInfo::Get().Print() );
	}
#endif

/*
=================================================
	Stop
=================================================
*/
	void  Client::Stop ()
	{
		EnableSensors( 0 );

		_Deinitialize();

		_genProf.Deinitialize();
		_pvrProf.Deinitialize();
		_armProf.Deinitialize();
		_maliProf.Deinitialize();
		_adrenoProf.Deinitialize();
	}

/*
=================================================
	SendLog
=================================================
*/
	void  Client::SendLog (StringView log)
	{
		_msgProducer->SendLog( log );
	}

/*
=================================================
	SendSensor
=================================================
*/
	void  Client::SendSensor (const int sensor, ArrayView<float> values)
	{
		if ( values.empty() )
			return;

		const usize	size	= values.size();
		float		data [16];

		// see [InputActionsAndroid::SetSensor](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/platform/Android/InputActionsAndroid.cpp)
	  #ifdef AE_PLATFORM_ANDROID
		switch ( sensor )
		{
			case ASENSOR_TYPE_ROTATION_VECTOR :
			case ASENSOR_TYPE_GAME_ROTATION_VECTOR :
			{
				// only for portrait orientation
				Quat  q{ values[3], values[0], values[1], values[2] };
				q = (q.Conjugate() * Quat{0.70710678f, 0.70710678f, 0.f, 0.f}).MirrorX();
				MemCopy( OUT data, &q, Sizeof(q) );
				break;
			}

			default :
				MemCopy( OUT data, values.data(), SizeOf<float>*size );
				break;
		}
	  #endif

		auto	msg = _msgProducer->CreateMsg< CSMsg_RemCtrl_SensorF >( SizeOf<float> * (size-1) );
		if ( not msg )
			return;

		msg->type	= ubyte(sensor);
		msg->count	= ubyte(size);

		for (usize i = 0; i < size; ++i)
			msg->arr[i] = data[i];

		Unused( _msgProducer->AddMessage( msg ));
	}

/*
=================================================
	SendGNS
=================================================
*/
	void  Client::SendGNS (double lat, double lon, double alt, ulong time, float bearing, float speed,
							float horAcc, float vertAcc, float bearingAcc, float speedAcc)
	{
		auto	msg = _msgProducer->CreateMsg< CSMsg_RemCtrl_GNS >();
		if ( not msg )
			return;

		msg->gns.latitude			= lat;
		msg->gns.longitude			= lon;
		msg->gns.altitude			= alt;
		msg->gns.time				= time;
		msg->gns.bearing			= bearing;
		msg->gns.speed				= speed;
		msg->gns.horizontalAccuracy	= horAcc;
		msg->gns.verticalAccuracy	= vertAcc;
		msg->gns.bearingAccuracy	= bearingAcc;
		msg->gns.speedAccuracy		= speedAcc;

		Unused( _msgProducer->AddMessage( msg ));
	}

/*
=================================================
	Update
=================================================
*/
	void  Client::Update ()
	{
		_pvrProf.Update();
		_genProf.Update();
		_armProf.Update();
		_maliProf.Update();
		_adrenoProf.Update();

		if ( BaseClient::Update( _frameId ))
		{
			_frameId.Inc();
		}

		//AE_LOGI( "client tick" );
	}

/*
=================================================
	EnableSensors
=================================================
*/
	void  Client::EnableSensors (uint bits)
	{
	#ifdef AE_PLATFORM_ANDROID
		if ( _java.enableSensors )
		{
			bits = *_java.enableSensors( bits );

			auto	msg = _msgProducer->CreateMsg< CSMsg_RemCtrl_EnableSensors >();
			if ( msg )
			{
				msg->bits = bits;
				Unused( _msgProducer->AddMessage( msg ));
			}
		}
	#else
		Unused( bits );
	#endif
	}

	void  MsgConsumer::_RemCtrl_EnableSensors (CSMsg_RemCtrl_EnableSensors const& msg)
	{
		s_Client.EnableSensors( msg.bits );
	}

} // namespace
//-----------------------------------------------------------------------------



#ifdef AE_PLATFORM_ANDROID
/*
=================================================
	native_*
=================================================
*/
	JNICALL void  native_SendText (JNIEnv* env, jclass, jstring txt)
	{
		using namespace AE::Java;
		s_Client.SendLog( JavaString{ txt, JavaEnv{env} });
	}

	JNICALL void  native_SendSensor (JNIEnv* env, jclass, int sensor, jfloatArray values)
	{
		using namespace AE::Java;
		s_Client.SendSensor( sensor, JavaArray<jfloat>{ values, True{"readOnly"}, JavaEnv{env} });
	}

	JNICALL void  native_SendGNS (JNIEnv*, jclass,
								  double lat, double lon, double alt, jlong time, float bearing, float speed,
								  float horAcc, float vertAcc, float bearingAcc, float speedAcc)
	{
		s_Client.SendGNS( lat, lon, alt, time, bearing, speed,
						  horAcc, vertAcc, bearingAcc, speedAcc );
	}

	JNICALL void  native_SendBatteryStat1 (JNIEnv*, jclass,
										   float current, float capacity, float energy)
	{
		PerformanceStat::_SetBatteryStat1( current, capacity, energy );
	}

	JNICALL void  native_SendBatteryStat2 (JNIEnv*, jclass,
										   float level, float temperature, float voltage, jboolean isCharging)
	{
		PerformanceStat::_SetBatteryStat2( level, temperature, voltage, isCharging );
	}

	JNICALL void  native_ProcessMessages (JNIEnv*, jclass)
	{
		s_Client.Update();
	}

	JNICALL void  native_Start (JNIEnv* env, jclass, jobject jobj)
	{
		using namespace AE::Java;

		JavaObj		obj { jobj, JavaEnv{env} };

		s_Client.Start( obj.Method< jint (jint) >( "EnableSensors" )
					  );
	}

	JNICALL void  native_Stop (JNIEnv*, jclass)
	{
		s_Client.Stop();
	}

/*
=================================================
	JNI_OnLoad
=================================================
*/
	extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
	{
		using namespace AE::Java;

		JNIEnv* env;
		if ( vm->GetEnv( OUT reinterpret_cast<void**>(&env), JavaEnv::Version ) != JNI_OK )
			return -1;

		JavaEnv::SetVM( vm );
		{
			JavaClass	service_class{ "AE/RemoteControl/RemoteControlService" };
			CHECK_ERR( service_class );

			service_class.RegisterStaticMethod( "native_SendText",			&native_SendText );
			service_class.RegisterStaticMethod( "native_SendSensor",		&native_SendSensor );
			service_class.RegisterStaticMethod( "native_SendGNS",			&native_SendGNS );
			service_class.RegisterStaticMethod( "native_SendBatteryStat1",	&native_SendBatteryStat1 );
			service_class.RegisterStaticMethod( "native_SendBatteryStat2",	&native_SendBatteryStat2 );
			service_class.RegisterStaticMethod( "native_ProcessMessages",	&native_ProcessMessages );
			service_class.RegisterStaticMethod( "native_Start",				&native_Start );
			service_class.RegisterStaticMethod( "native_Stop",				&native_Stop );
		}

		StaticLogger::InitDefault();
		AE_LOGI( "JNI_OnLoad" );

		TaskScheduler::InstanceCtor::Create();

		TaskScheduler::Config	cfg;
		CHECK_ERR( Scheduler().Setup( cfg ), -1 );

		CHECK_ERR( SocketService::Instance().Initialize(), -1 );

		return JavaEnv::Version;
	}

/*
=================================================
	JNI_OnUnload
=================================================
*/
	extern "C" void JNI_OnUnload (JavaVM* vm, void *)
	{
		using namespace AE::Java;
		AE_LOGI( "JNI_OnUnload" );

		Scheduler().Release();
		TaskScheduler::InstanceCtor::Destroy();

		SocketService::Instance().Deinitialize();

		JavaEnv::SetVM( null );

		StaticLogger::Deinitialize( True{"checkMemLeaks"} );
	}

#endif // AE_PLATFORM_ANDROID
