// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/HighLevel/MessageFactory.h"
#include "platform/Public/InputActions.h"
//#include "platform/Public/HwCamera.h"

// Client/Server Message Group
namespace AE::CSMessageGroup
{
	static constexpr auto													RemoteControl			= CSMessageGroupID(4);
	static constexpr Base::CT_Counter< _MsgCounterCtx<RemoteControl> >		RemoteControl_MsgCounter;
}

namespace AE::Networking
{

	// RemoteInputActions //
	DECL_CSMSG( RemCtrl_EnableSensors,  RemoteControl,
		uint	bits;
	);

	DECL_CSMSG( RemCtrl_SensorF,  RemoteControl,
		ubyte	type;			// ESensorType
		ubyte	count;
		float	arr [1];
	);

	DECL_CSMSG( RemCtrl_GNS,  RemoteControl,
		App::IInputActions::GNSData		gns;
	);


	// RemoteInputSurface //
	DECL_CSMSG( RemCtrl_CameraInfoReq,  RemoteControl );

	DECL_CSMSG( RemCtrl_CameraInfoBegin,  RemoteControl,
		ubyte			count;
		bool			hasPermission;
	);

	DECL_CSMSG( RemCtrl_CameraInfoRes,  RemoteControl,
		//App::IHwCamera::CameraStaticInfo	info;
	);

	DECL_CSMSG( RemCtrl_CameraInfoStreamCfg,  RemoteControl,
		ubyte			cameraId;
		ubyte			format;
		ubyte			count;
		uint			arr [1];	// w, h, duration (nano)
	);

	DECL_CSMSG( RemCtrl_CameraInfoEnd,  RemoteControl,
		ubyte			cameraId;
		// TODO: hash?
	);


	DECL_CSMSG( RemCtrl_OpenCamera,  RemoteControl,
		ubyte			cameraId;
		packed_uint2	dimPx;
	);

	DECL_CSMSG( RemCtrl_CloseCamera,  RemoteControl,
		ubyte			cameraId;
	);

	DECL_CSMSG( RemCtrl_CameraFrame,  RemoteControl,
		ubyte			cameraId;
		ubyte			seqIndex;
		Bytes16u		size;
		ubyte			data [1];
	);
//-----------------------------------------------------------------------------



	CSMSG_ENC_DEC( RemCtrl_EnableSensors,					bits );
	CSMSG_ENC_DEC( RemCtrl_GNS,								gns );

	CSMSG_ENC_DEC_EXARRAY( RemCtrl_SensorF,					count, arr,  AE_ARGS( type, count ));

	CSMSG_ENC_DEC( RemCtrl_CameraInfoReq					);
	CSMSG_ENC_DEC( RemCtrl_CameraInfoRes					);
	CSMSG_ENC_DEC_EXARRAY( RemCtrl_CameraInfoStreamCfg,		count, arr,  AE_ARGS( cameraId, format, count ));
	CSMSG_ENC_DEC( RemCtrl_CameraInfoBegin,					count, hasPermission );
	CSMSG_ENC_DEC( RemCtrl_CameraInfoEnd,					cameraId );

	CSMSG_ENC_DEC( RemCtrl_OpenCamera						);
	CSMSG_ENC_DEC( RemCtrl_CloseCamera						);
	CSMSG_ENC_DEC_EXDATA( RemCtrl_CameraFrame,				size, data,  AE_ARGS( cameraId, seqIndex ));


} // AE::Networking
//-----------------------------------------------------------------------------


namespace AE::Base
{
	template <>
	struct TTriviallyDestructible< Networking::CSMsg_RemCtrl_CameraInfoRes > {
		static constexpr bool	value =	true;
	};

} // AE::Base
//-----------------------------------------------------------------------------


namespace AE::Networking
{
/*
=================================================
	Register_RemoteControl
=================================================
*/
	ND_ inline bool  Register_RemoteControl (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					// RemoteInputActions //
					CSMsg_RemCtrl_EnableSensors,
					CSMsg_RemCtrl_SensorF,
					CSMsg_RemCtrl_GNS,

					// RemoteInputSurface //
					CSMsg_RemCtrl_CameraInfoReq,
					CSMsg_RemCtrl_CameraInfoBegin,
					CSMsg_RemCtrl_CameraInfoRes,
					CSMsg_RemCtrl_CameraInfoStreamCfg,
					CSMsg_RemCtrl_CameraInfoEnd,
					CSMsg_RemCtrl_OpenCamera,
					CSMsg_RemCtrl_CameraFrame,
					CSMsg_RemCtrl_CloseCamera
				>
				( True{"lock group"} );
	}


} // AE::Networking
