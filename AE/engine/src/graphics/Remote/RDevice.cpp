// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/EnumToString.h"
# include "graphics/Remote/RDevice.h"
# include "graphics/Remote/RMessages.cpp.h"
# include "graphics/Remote/Allocators/RGfxMemAllocator.h"
# include "graphics/Remote/Descriptors/RDescriptorAllocator.h"
# include "graphics/Remote/Resources/RPipelinePack.h"

# include "GraphicsLib.h"

namespace AE::Graphics
{
	using namespace AE::Threading;
	using namespace AE::Networking;
	using namespace AE::RemoteGraphics;

namespace {
# ifdef AE_DEBUG
	static constexpr auto	c_Timeout = milliseconds{30 * 60 * 1'000};		// 30 min
# else
	static constexpr auto	c_Timeout = milliseconds{2'000};
# endif
}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	RDevice::RDevice () __NE___
	{}

	RDevice::~RDevice () __NE___
	{}

/*
=================================================
	Cast
=================================================
*/
	RmGfxMemAllocatorID  RDevice::Cast (const GfxMemAllocatorPtr &ptr) C_NE___
	{
		return ptr != null ? Base::Cast<RGfxMemAllocator>(ptr.get())->Handle() : Default;
	}

	RmDescriptorAllocatorID  RDevice::Cast (const DescriptorAllocatorPtr &ptr) C_NE___
	{
		return ptr != null ? Base::Cast<RDescriptorAllocator>(ptr.get())->Handle() : Default;
	}

	RmRenderTechPipelinesID  RDevice::Cast (const RenderTechPipelinesPtr &ptr) C_NE___
	{
		return ptr != null ? Base::Cast<RPipelinePack::RenderTech>(ptr.get())->Handle() : Default;
	}

/*
=================================================
	_ProcessMessages
=================================================
*/
	inline void  RDevice::_ProcessMessage (RConnection &conn, RC<BaseMsg> msg) C_NE___
	{
		const auto	id = msg->GetTypeId();

		if_likely( id == TypeIdOf<Msg::UploadDataAndCopy>() )
		{
			auto&			upload	= RefCast<Msg::UploadDataAndCopy>( *msg );
			Atomic<bool>	looping	{true};
			CHECK_FATAL( conn.ReadReceived( OUT RmCast( upload.dst ), upload.size, upload.size, looping ) == upload.size );
		}
		else
		if_likely( id == TypeIdOf<Msg::Log>() )
		{
			auto&	log = RefCast<Msg::Log>( *msg );
			if ( log.level >= ELogLevel::Error )
				log.level = ELogLevel::Warning;
			AE_PRIVATE_LOGX( log.level, log.scope, log.message, log.file, log.line );
		}
		else
		{
			DBG_WARNING( "unsupported response" );
		}
	}

/*
=================================================
	Send
=================================================
*/
	bool  RDevice::Send (const BaseMsg &msg) C_NE___
	{
		auto	lock = _connArr.Lock();
		auto&	conn = _connArr.Get( lock );
		bool	ok   = conn.Send( msg );

		for (bool loop = true; loop;)
		{
			Unused( conn.Receive() );

			loop = false;
			for (; auto msg2 = conn.Encode(); )
			{
				_ProcessMessage( conn, RVRef(msg2) );
				loop = true;
			}
		}

		_connArr.Unlock( lock );
		return ok;
	}

/*
=================================================
	Send
=================================================
*/
	bool  RDevice::Send (ArrayView<BaseMsg*> msgs) C_NE___
	{
		auto	lock = _connArr.Lock();
		auto&	conn = _connArr.Get( lock );
		bool	ok   = true;

		for (auto* msg : msgs)
			ok = ok and conn.Send( *msg );

		for (bool loop = true; loop;)
		{
			Unused( conn.Receive() );

			loop = false;
			for (; auto msg2 = conn.Encode(); )
			{
				_ProcessMessage( conn, RVRef(msg2) );
				loop = true;
			}
		}

		_connArr.Unlock( lock );
		return ok;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	RDeviceInitializer::RDeviceInitializer (Bool enableInfoLog) __NE___ :
		_enableInfoLog{ enableInfoLog }
	{
	}

	RDeviceInitializer::~RDeviceInitializer () __NE___
	{}

/*
=================================================
	CreateInstance
=================================================
*/
	bool  RDeviceInitializer::CreateInstance (const InstanceCreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( _InitClient( ci.deviceAddr ));
		Unused( _InitGraphicsLib( ci.graphicsLibPath ));

		if ( _enableInfoLog )
			AE_LOG_DBG( "Created RemoteGraphics instance" );

		UNTESTED;
		return true;
	}

/*
=================================================
	DestroyInstance
=================================================
*/
	bool  RDeviceInitializer::DestroyInstance () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _enableInfoLog )
			AE_LOG_DBG( "Destroyed RemoteGraphics instance" );

		{
			Msg::Device_DestroyInstance	msg;
			RC<Msg::DefaultResponse>	res;

			CHECK( SendAndWait( msg, OUT res ));
			CHECK( res->ok );
		}
		Unused( _connArr.Deinitialize() );

		if ( _graphics )
		{
			_graphics->Destroy();
			_graphics = null;
			_graphicsLib.Unload();
		}

		return true;
	}

/*
=================================================
	ChooseDevice
=================================================
*/
	bool  RDeviceInitializer::ChooseDevice (StringView deviceName) __NE___
	{
		UNTESTED
		return false;
	}

/*
=================================================
	ChooseHighPerformanceDevice
=================================================
*/
	bool  RDeviceInitializer::ChooseHighPerformanceDevice () __NE___
	{
		UNTESTED
		return false;
	}

/*
=================================================
	CreateDefaultQueue
=================================================
*/
	bool  RDeviceInitializer::CreateDefaultQueue () __NE___
	{
		return CreateDefaultQueues( EQueueMask::Graphics, Default );
	}

	bool  RDeviceInitializer::CreateDefaultQueues (EQueueMask required, EQueueMask optional) __NE___
	{
		DRC_EXLOCK( _drCheck );

		UNTESTED
		return false;
	}

/*
=================================================
	CreateLogicalDevice
=================================================
*/
	bool  RDeviceInitializer::CreateLogicalDevice () __NE___
	{
		DRC_EXLOCK( _drCheck );


		CHECK_ERR( _devInfo.initialized );
		CHECK_ERR( _InitQueues() );

		if ( _enableInfoLog )
			AE_LOG_DBG( "Created RemoteGraphics logical device" );

		if ( _enableInfoLog )
		{
			_devInfo.resFlags.Print();
			_devInfo.props.Print();
		}

		UNTESTED;
		return true;
	}

/*
=================================================
	DestroyLogicalDevice
=================================================
*/
	bool  RDeviceInitializer::DestroyLogicalDevice () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( not _devInfo.initialized )
			return false;

		{
			Msg::Device_DestroyLogicalDevice	msg;
			RC<Msg::DefaultResponse>			res;

			CHECK( SendAndWait( msg, OUT res ));
			CHECK( res->ok );
		}

		Reconstruct( INOUT _devInfo	);
		_queueMask	= Default;
		_queueCount	= 0;
		_queueTypes.fill( null );

		if ( _enableInfoLog )
			AE_LOG_DBG( "Destroyed RemoteGraphics logical device" );

		return true;
	}

/*
=================================================
	Init
=================================================
*/
	bool  RDeviceInitializer::Init (const GraphicsCreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( _InitClient( ci.deviceAddr ));
		Unused( _InitGraphicsLib( ci.graphicsLibPath ));

		{
			Msg::Device_Init				msg;
			RC<Msg::Device_Init_Response>	res;

			msg.info		= ci;
			msg.initSyncLog	= ci.enableSyncLog;

			CHECK_ERR( SendAndWait( msg, OUT res ));
			Reconstruct( INOUT _devInfo, RVRef(*res) );
		}

		CHECK_ERR( _devInfo.initialized );
		CHECK_MSG( _devInfo.engineVersion == Version3( AE_VERSION.Get<0>(), AE_VERSION.Get<1>(), AE_VERSION.Get<2>() ),
			"Engine version mismatch, remote ("s << ToString(_devInfo.engineVersion) <<
			"), host (" << ToString(Version3{ AE_VERSION.Get<0>(), AE_VERSION.Get<1>(), AE_VERSION.Get<2>() }) << ")" );

		CHECK_ERR( _InitQueues() );

		if ( _enableInfoLog )
		{
			_devInfo.resFlags.Print();
			_devInfo.props.Print();
			_LogLogicalDevice();
		}

		if ( _graphics )
		{
			#ifdef CMAKE_INTDIR
				StringView	config = CMAKE_INTDIR;
			#else
			# ifdef AE_CFG_DEBUG
				StringView	config = "Debug";
			# elif defined(AE_CFG_DEVELOP)
				StringView	config = "Develop";
			# elif defined(AE_CFG_PROFILE)
				StringView	config = "Profile";
			# elif defined(AE_CFG_RELEASE)
				StringView	config = "Release";
			# else
			#	error unknown config!
			# endif
			#endif
			CHECK_ERR( _graphics->Initialize( ci, config, AE_OPTIMIZE_IDS, Version3{AE_VERSION.Get<0>(), AE_VERSION.Get<1>(), AE_VERSION.Get<2>()} ));
		}
		return true;
	}

/*
=================================================
	Init
=================================================
*/
	bool  RDeviceInitializer::_InitQueues () __NE___
	{
		ASSERT( _devInfo.queues.max_size() == _queues.max_size() );

		_queueTypes.fill( null );
		_queueMask	= Default;
		_queueCount	= 0;

		for (usize i : IndicesOnly(_queues))
		{
			const auto&	src = _devInfo.queues[i];

			if ( src.type != Default )
			{
				auto&	q	= _queues [_queueCount++];

				q._device	= this;
				q.type		= src.type;
				q.minImageTransferGranularity = src.minImageTransferGranularity;

				_queueTypes[uint(q.type)] = &q;
				_queueMask |= q.type;
			}
		}

		CHECK_ERR(	_queueCount != 0	and
					_queueMask != Default );
		return true;
	}

/*
=================================================
	_LogLogicalDevice
=================================================
*/
	void  RDeviceInitializer::_LogLogicalDevice () C_Th___
	{
		String	str = "\nCreate remote graphics device:";
		str << "\n  name:  " << _devInfo.name;

		str << "\n  queues:";
		for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
		{
			auto&	q = _queues[i];
			str << "\n    [" << ToString(i) << "] '"
				<< q.debugName.c_str()
				<< "', type: " << ToString( EQueueType(i) )
				<< ' ' << ToString( q.minImageTransferGranularity );
		}

		AE_LOGI( str );
	}

/*
=================================================
	_InitClient
=================================================
*/
	bool  RDeviceInitializer::_InitClient (Networking::IpAddress addr) __NE___
	{
		using namespace AE::Serializing;

		CHECK_ERR( _connArr.Initialize( addr ));
		{
			auto&	f = _connArr.GetFactory();
			#define REG( _name_ )	f.Register< Msg::_name_ >( SerializedID{#_name_} )
			REGISTER_MESSAGES( REG, REG )
			#undef REG
			CHECK_ERR( f.Register< PipelineCompiler::ShaderTrace >( SerializedID{"ShaderTrace"} ));
		}{
			#define REG( _name_ )	_cmdFactory.Register< Msg::_name_ >( SerializedID{#_name_} )
			REGISTER_CMDBUF_COMMANDS( REG )
			#undef REG
		}{
			#define REG( _name_ )	_descFactory.Register< Msg::_name_ >( SerializedID{#_name_} )
			REGISTER_DESCUPD_COMMANDS( REG )
			#undef REG
		}
		return true;
	}

/*
=================================================
	_InitGraphicsLib
=================================================
*/
	bool  RDeviceInitializer::_InitGraphicsLib (StringView path) __NE___
	{
		if ( _graphicsLib.Load( path ))
		{
			decltype(&GraphicsLib::CreateGraphicsLib)	fn;

			CHECK_ERR( _graphicsLib.GetProcAddr( "CreateGraphicsLib", OUT fn ));

			_graphics = fn();
			CHECK_ERR( _graphics );

			return true;
		}
		return false;
	}

/*
=================================================
	EnableSyncLog
=================================================
*/
	void  RDeviceInitializer::EnableSyncLog (bool enable) C_NE___
	{
		Msg::Device_EnableSyncLog	msg;
		msg.enable = enable;
		CHECK( Send( msg ));
	}

/*
=================================================
	GetSyncLog
=================================================
*/
	void  RDeviceInitializer::GetSyncLog (OUT String &log) C_NE___
	{
		Msg::Device_GetSyncLog				msg;
		RC<Msg::Device_GetSyncLog_Response>	res;

		CHECK( SendAndWait( msg, OUT res ));
		log = RVRef(res->log);
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
