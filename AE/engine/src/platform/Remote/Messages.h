// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/HighLevel/MessageFactory.h"
#include "platform/Public/InputActions.h"

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

//-----------------------------------------------------------------------------



	CSMSG_ENC_DEC( RemCtrl_EnableSensors,					bits );
	CSMSG_ENC_DEC( RemCtrl_GNS,								gns );

	CSMSG_ENC_DEC_EXARRAY( RemCtrl_SensorF,					count, arr,  AE_ARGS( type, count ));

} // AE::Networking
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
					CSMsg_RemCtrl_GNS
				>
				( True{"lock group"} );
	}


} // AE::Networking
