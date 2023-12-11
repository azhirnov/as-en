// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe:  see ClientServerBase
*/

#pragma once

#include "networking/HighLevel/ClientServerBase.h"

namespace AE::Networking
{

    //
    // Server
    //

    class BaseServer : public ClientServerBase
    {
    // variables
    private:
        RC<IClientListener>     _clientListener;


    // methods
    protected:
        ND_ bool  _Initialize (RC<MessageFactory>   mf,
                               RC<IClientListener>  clientListener,
                               RC<IAllocator>       alloc,
                               FrameUID             firstFrameId)   __NE___;
            void  _Deinitialize ()                                  __NE___;

        ND_ bool  _AddChannelTCP (ushort port)                      __NE___;
        ND_ bool  _AddChannelUnreliableUDP (ushort port)            __NE___;

        ND_ bool  _DisconnectClient (EClientLocalID)                __NE___;
    };



    //
    // Client Event Listener interface
    //

    class IClientListener : public EnableRC<IClientListener>
    {
    // interface
    public:

        // Called when new client connected.
        // Returns valid ID if client is allowed to connect, otherwise it will be disconnected.
        //
        // Thread-safe:
        //  Can be used in multiple threads but not in concurrent when used in single client/server.
        //
        ND_ virtual EClientLocalID  OnClientConnected (EChannel, const IpAddress &)     __NE___ = 0;
        ND_ virtual EClientLocalID  OnClientConnected (EChannel, const IpAddress6 &)    __NE___ = 0;

            virtual void  OnClientDisconnected (EChannel, EClientLocalID)               __NE___ = 0;
    };


} // AE::Networking
