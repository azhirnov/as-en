// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Client / Server messages
*/

#pragma once

#include "networking/HighLevel/MessageFactory.h"

// Client/Server Message Group
namespace AE::CSMessageGroup
{
    using AE::Networking::CSMessageGroupID;

    template <CSMessageGroupID GroupId>
    struct _MsgCounterCtx {};

    static constexpr auto                                       Global          = CSMessageGroupID(0);
    static constexpr Base::CT_Counter< _MsgCounterCtx<Global> > Global_MsgCounter;

    static constexpr auto                                       Debug           = CSMessageGroupID(1);
    static constexpr Base::CT_Counter< _MsgCounterCtx<Debug> >  Debug_MsgCounter;
}

namespace AE::Networking
{
    DECL_CSMSG( Log,  Debug,
        StringView      msg;
        SourceLoc       loc;
        ELogLevel       level   = ELogLevel::Debug;
        ELogScope       scope   = ELogScope::Unknown;

        void  Execute ()    C_NE___;
    );

    DECL_CSMSG( NextFrame,  Global );

    DECL_CSMSG( Sync,  Global,
        ulong           timeNs;
        ulong           frameId;
    );
//-----------------------------------------------------------------------------



    CSMSG_ENC_DEC( Log,  msg, loc.file, loc.line, level, scope );

    inline void  CSMsg_Log::Execute () C_NE___
    {
        AE_PRIVATE_LOGX( level, scope, msg, loc.file, loc.line );
    }


    CSMSG_EMPTY_ENC_DEC( NextFrame );

    CSMSG_ENC_DEC( Sync,  timeNs, frameId );


} // AE::Networking
