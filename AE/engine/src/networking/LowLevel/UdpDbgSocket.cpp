// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/UdpDbgSocket.h"

namespace AE::Networking
{

/*
=================================================
    Open
=================================================
*/
    bool  UdpDbgSocket::Open (const IpAddress &addr, const Config &cfg) __NE___
    {
        _stat   = Default;
        _config = cfg;
        return _socket.Open( addr, cfg );
    }

    bool  UdpDbgSocket::Open (const IpAddress6 &addr, const Config &cfg) __NE___
    {
        _stat   = Default;
        _config = cfg;
        return _socket.Open( addr, cfg );
    }

/*
=================================================
    Close
=================================================
*/
    void  UdpDbgSocket::Close () __NE___
    {
        // TODO: print stat?
        _socket.Close();
    }

/*
=================================================
    Send
=================================================
*/
    auto  UdpDbgSocket::Send (const IpAddress &addr, const void* data, Bytes dataSize) C_NE___ -> Tuple< SocketSendError, Bytes >
    {
        Array<char>     temp;
        if ( _DropOrCorrupt( temp, INOUT data, INOUT dataSize ))
            return Tuple{ SocketSendError::Sent, dataSize };

        auto    result = _socket.Send( addr, data, dataSize );

        _stat.sentData      += result.Get<1>();
        _stat.sentPackets   ++;

        return result;
    }

    auto  UdpDbgSocket::Send (const IpAddress6 &addr, const void* data, Bytes dataSize) C_NE___ -> Tuple< SocketSendError, Bytes >
    {
        Array<char>     temp;
        if ( _DropOrCorrupt( temp, INOUT data, INOUT dataSize ))
            return Tuple{ SocketSendError::Sent, dataSize };

        auto    result = _socket.Send( addr, data, dataSize );

        _stat.sentData      += result.Get<1>();
        _stat.sentPackets   ++;

        return result;
    }

/*
=================================================
    Receive
=================================================
*/
    auto  UdpDbgSocket::Receive (OUT IpAddress &addr, OUT void* data, Bytes dataSize) C_NE___ -> Tuple< SocketReceiveError, Bytes >
    {
        auto    result = _socket.Receive( addr, OUT data, dataSize );
        _DropOrCorrupt( INOUT data, INOUT result );
        return result;
    }

    auto  UdpDbgSocket::Receive (OUT IpAddress6 &addr, OUT void* data, Bytes dataSize) C_NE___ -> Tuple< SocketReceiveError, Bytes >
    {
        auto    result = _socket.Receive( addr, OUT data, dataSize );
        _DropOrCorrupt( INOUT data, INOUT result );
        return result;
    }

/*
=================================================
    _DropOrCorrupt
=================================================
*/
    void  UdpDbgSocket::_DropOrCorrupt (INOUT void* data, INOUT Tuple< SocketReceiveError, Bytes > &res) C_NE___
    {
        if ( res.Get<0>() != SocketReceiveError::Received )
            return;

        if ( _rnd.Uniform( 0.f, 1.f ) < _config.packetLost.GetFraction() )
        {
            _stat.lostRecvPackets ++;

            res.Get<0>() = SocketReceiveError::NotReceived;
            res.Get<1>() = 0_b;
            return;
        }

        if ( _rnd.Uniform( 0.f, 1.f ) < _config.packetCorruption.GetFraction() )
        {
            _CorruptData( INOUT data, INOUT res.Get<1>() );
            _stat.corruptRecvPackets ++;
        }
    }

    bool  UdpDbgSocket::_DropOrCorrupt (Array<char> &buffer, INOUT const void* &data, INOUT Bytes &dataSize) C_NE___
    {
        if ( _rnd.Uniform( 0.f, 1.f ) < _config.packetLost.GetFraction() )
        {
            _stat.lostSentPackets ++;
            return true;  // drop
        }

        if ( _rnd.Uniform( 0.f, 1.f ) < _config.packetCorruption.GetFraction() )
        {
            NOTHROW_ERR( buffer.assign( Cast<char>(data), Cast<char>(data + dataSize) ));

            _CorruptData( INOUT buffer.data(), INOUT dataSize );
            data = buffer.data();

            _stat.corruptSentPackets ++;
        }

        return false;  // send
    }

    void  UdpDbgSocket::_CorruptData (INOUT void* data, INOUT Bytes &dataSize) C_NE___
    {
        //const auto    bits = usize( (_rnd.Uniform( 0.1f, 1.f ) * _config.packetCorruptionBits.GetFraction()) * float(usize{dataSize}) + 0.5f );
        // TODO
    }

/*
=================================================
    PrintStat
=================================================
*/
    void  UdpDbgSocket::PrintStat () C_NE___
    {
        #ifdef AE_ENABLE_LOGS
        NOTHROW(
            String  str;
            str << "sent/lost/corrupt: " << ToString(_stat.sentPackets) << ", " << ToString(_stat.lostSentPackets) << ", " << ToString(_stat.corruptSentPackets) << '\n'
                << "recv/lost/corrupt: " << ToString(_stat.recvPackets) << ", " << ToString(_stat.lostRecvPackets) << ", " << ToString(_stat.corruptRecvPackets) << '\n'
                << "sent data:         " << ToString( _stat.sentData ) << '\n'
                << "received data:     " << ToString( _stat.recvData ) << '\n';
            AE_LOGI( str );
        )
        #endif
    }


} // AE::Networking
