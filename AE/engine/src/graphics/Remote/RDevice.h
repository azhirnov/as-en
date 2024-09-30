// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/IDevice.h"
# include "graphics/Remote/RQueue.h"
# include "graphics/Remote/RConnection.h"

namespace AE::GraphicsLib
{
	class IGraphicsLib;
}
namespace AE::Graphics
{

	//
	// Remote Graphics Host
	//

	class RDevice : public IDevice
	{
	// types
	public:
		using EFeature				= RemoteGraphics::Msg::Device_Init_Response::EFeature;
	protected:
		using DeviceInfo			= RemoteGraphics::Msg::Device_Init_Response;
		using Queues_t				= StaticArray< RQueue, uint(EQueueType::_Count) >;
		using QueueTypes_t			= StaticArray< RQueuePtr, uint(EQueueType::_Count) >;
		using RConnectionArray		= AE::RemoteGraphics::RConnectionArray;
		using BaseMsg				= RemoteGraphics::Msg::BaseMsg;
		using DevMemoryUsageSync	= Synchronized< RWSpinLock, DevMemoryUsageOpt >;
		using IGraphicsLib			= GraphicsLib::IGraphicsLib;


	// variables
	protected:
		EQueueMask					_queueMask		= Default;
		ubyte						_queueCount		= 0;
		QueueTypes_t				_queueTypes		= {};
		Queues_t					_queues;

		DeviceInfo					_devInfo;
		mutable DevMemoryUsageSync	_memUsage;

		mutable RConnectionArray	_connArr;

		Serializing::ObjectFactory	_cmdFactory;
		Serializing::ObjectFactory	_descFactory;

		Library						_graphicsLib;
		Ptr<IGraphicsLib>			_graphics;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RDevice ()													__NE___;
		~RDevice ()													__NE___;

		ND_ EGraphicsAPI			GetGraphicsAPI ()				C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.api; }
		ND_ ECPUArch				GetCPUArch ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.cpuArch; }
		ND_ EOperationSystem		GetOSType ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.os; }

		ND_ DeviceProperties const&	GetDeviceProperties ()			C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.props; }
		ND_ ResourceFlags const&	GetResourceFlags ()				C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.resFlags; }

		ND_ ArrayView<RQueue>		GetQueues ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return ArrayView<RQueue>{ _queues.data(), _queueCount }; }
		ND_ RQueuePtr				GetQueue (EQueueType type)		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _queueTypes[uint(type)]; }
		ND_ EQueueMask				GetAvailableQueues ()			C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _queueMask; }
		ND_ StringView				GetDeviceName ()				C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.name; }

		ND_ DeviceMemoryInfo		GetMemoryInfo ()				C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.memInfo; }
		ND_ DevMemoryUsageOpt		GetMemoryUsage ()				C_NE_OF	{ return _memUsage.Read(); }

		ND_ bool					IsInitialized ()				C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.initialized; }
		ND_ bool					IsUnderDebugger ()				C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.underDebugger; }

		ND_ EGraphicsAdapterType	AdapterType ()					C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.adapterType; }

		ND_ bool					CheckConstantLimits ()			C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.checkConstantLimitsOK; }
		ND_ bool					CheckExtensions ()				C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.checkExtensionsOK; }

		ND_ auto&					GetCommandsFactory ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _cmdFactory; }
		ND_ auto&					GetDescUpdaterFactory ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _descFactory; }

		ND_ bool					HasFeature (EFeature feat)		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _devInfo.features.contains( feat ); }

		ND_ Ptr<IGraphicsLib>		GetGraphicsLib ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _graphics; }

			void  InitFeatureSet (OUT FeatureSet &outFeatureSet)	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  outFeatureSet = _devInfo.fs; }

		ND_ bool  Send (const BaseMsg &msg)											C_NE___;
		ND_ bool  Send (ArrayView<BaseMsg*> msgs)									C_NE___;

		template <typename R>
		ND_ bool  SendAndWait (const BaseMsg &msg, OUT RC<R> &response)				C_NE___;
		template <typename R>
		ND_ bool  SendAndWait (ArrayView<BaseMsg*> msgs, OUT RC<R> &response)		C_NE___;

		ND_ RmGfxMemAllocatorID			Cast (const GfxMemAllocatorPtr &ptr)		C_NE___;
		ND_ RmDescriptorAllocatorID		Cast (const DescriptorAllocatorPtr &ptr)	C_NE___;
		ND_ RmRenderTechPipelinesID		Cast (const RenderTechPipelinesPtr &ptr)	C_NE___;

		void  _SetMemoryUsage (DevMemoryUsageOpt value)								C_NE___	{ _memUsage.Write( value ); }

	private:
		void  _ProcessMessage (RemoteGraphics::RConnection &, RC<BaseMsg>)			C_NE___;
	};



	//
	// Remote Device Initializer
	//

	class RDeviceInitializer final : public RDevice
	{
	// types
	public:
		struct InstanceCreateInfo
		{
			StringView				appName			= AE_ENGINE_NAME " App";
			StringView				engineName		= AE_ENGINE_NAME;
			uint					appVer			= 0;
			uint					engineVer		= 0;
			Networking::IpAddress	deviceAddr;
			StringView				graphicsLibPath;
		};


	// variable
	private:
		bool	_enableInfoLog		= false;


	// methods
	public:
		explicit RDeviceInitializer (Bool enableInfoLog = False{})							__NE___;
		~RDeviceInitializer ()																__NE___;

	  // LowLevel //
		ND_ bool  CreateInstance (const InstanceCreateInfo &ci)								__NE___;
			bool  DestroyInstance ()														__NE___;

		ND_ bool  ChooseDevice (StringView deviceName)										__NE___;
		ND_ bool  ChooseHighPerformanceDevice ()											__NE___;

		ND_ bool  CreateDefaultQueue ()														__NE___;
		ND_ bool  CreateDefaultQueues (EQueueMask required, EQueueMask optional = Default)	__NE___;

		ND_ bool  CreateLogicalDevice ()													__NE___;
			bool  DestroyLogicalDevice ()													__NE___;

	  // HighLevel //
		ND_ bool  Init (const GraphicsCreateInfo &ci)										__NE___;

	  // Utils //
			void  EnableSyncLog (bool enable)												C_NE___;
			void  GetSyncLog (OUT String &log)												C_NE___;


	private:
		ND_ bool  _InitClient (Networking::IpAddress addr)									__NE___;
		ND_ bool  _InitGraphicsLib (StringView path)										__NE___;

		ND_ bool  _InitQueues ()															__NE___;
			void  _LogLogicalDevice ()														C_Th___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	SendAndWait
=================================================
*/
	template <typename R>
	bool  RDevice::SendAndWait (const BaseMsg &msg, OUT RC<R> &response) C_NE___
	{
		StaticAssert( IsBaseOf< RemoteGraphics::Msg::BaseResponse, R >);

		auto	lock = _connArr.Lock();
		auto&	conn = _connArr.Get( lock );

		const auto	SendAndWait = [&] ()
		{{
			bool	ok   = conn.Send( msg );
			for (;ok;)
			{
				bool	ok2 = conn.Receive();

				for (; auto msg2 = conn.Encode(); )
				{
					if ( msg2->GetTypeId() == TypeIdOf<R>() )
					{
						response = Base::Cast<R>(msg2);
						return true;
					}

					_ProcessMessage( conn, RVRef(msg2) );
					ok2 = true;
				}

				if ( not ok2 )
					ThreadUtils::Sleep_1us();
			}
			return ok;
		}};

		bool	ok = SendAndWait();
		_connArr.Unlock( lock );
		return ok;
	}

/*
=================================================
	SendAndWait
=================================================
*/
	template <typename R>
	bool  RDevice::SendAndWait (ArrayView<BaseMsg*> msgs, OUT RC<R> &response) C_NE___
	{
		StaticAssert( IsBaseOf< RemoteGraphics::Msg::BaseResponse, R >);

		auto	lock = _connArr.Lock();
		auto&	conn = _connArr.Get( lock );

		const auto	SendAndWait = [&] ()
		{{
			bool	ok = true;

			for (auto* msg : msgs)
				ok = ok and conn.Send( *msg );

			for (;ok;)
			{
				bool	ok2 = conn.Receive();

				if ( auto msg2 = conn.Encode() )
				{
					if ( msg2->GetTypeId() == TypeIdOf<R>() )
					{
						response = Base::Cast<R>(msg2);
						return true;
					}

					_ProcessMessage( conn, RVRef(msg2) );
					ok2 = true;
				}

				if ( not ok2 )
					ThreadUtils::Sleep_1us();
			}
			return ok;
		}};

		bool	ok = SendAndWait();
		_connArr.Unlock( lock );
		return ok;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
