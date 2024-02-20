// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Utils/DefaultClientListener.h"

namespace AE::Networking
{

/*
=================================================
    ClientInfo::IsEmpty
=================================================
*/
    bool  DefaultClientListener::ClientInfo::IsEmpty () C_NE___
    {
        bool    res = true;
        for (auto& addr : ipv4Addr) res &= not addr.IsValid();
        for (auto& addr : ipv6Addr) res &= not addr.IsValid();
        return res;
    }

/*
=================================================
    SetMaxClients
=================================================
*/
    void  DefaultClientListener::SetMaxClients (uint count) __NE___
    {
        EXLOCK( _guard );

        ASSERT( count > 0 );
        ASSERT( count > _clients.size() );
        ASSERT( count <= c_MaxClients );

        _maxClients = Min( count, c_MaxClients );
    }

/*
=================================================
    AddToBlackList
=================================================
*/
    void  DefaultClientListener::AddToBlackList (ArrayView<IpAddress> arr) __NE___
    {
        EXLOCK( _guard );

        NOTHROW(
            for (auto& addr : arr)
                _blackListV4.insert( addr );
        )
    }

    void  DefaultClientListener::AddToBlackList (ArrayView<IpAddress6> arr) __NE___
    {
        EXLOCK( _guard );

        NOTHROW(
            for (auto& addr : arr)
                _blackListV6.insert( addr );
        )
    }

/*
=================================================
    RemoveFromBlackList
=================================================
*/
    void  DefaultClientListener::RemoveFromBlackList (ArrayView<IpAddress> arr) __NE___
    {
        EXLOCK( _guard );

        for (auto& addr : arr)
            _blackListV4.erase( addr );
    }

    void  DefaultClientListener::RemoveFromBlackList (ArrayView<IpAddress6> arr) __NE___
    {
        EXLOCK( _guard );

        for (auto& addr : arr)
            _blackListV6.erase( addr );
    }

/*
=================================================
    OnClientConnected
=================================================
*/
    EClientLocalID  DefaultClientListener::OnClientConnected (EChannel channel, const IpAddress &addr) __NE___
    {
        NOTHROW_ERR(
            return _OnClientConnected( channel, addr, _blackListV4, _addrToClientV4 );)
    }

    EClientLocalID  DefaultClientListener::OnClientConnected (EChannel channel, const IpAddress6 &addr) __NE___
    {
        NOTHROW_ERR(
            return _OnClientConnected( channel, addr, _blackListV6, _addrToClientV6 );)
    }

/*
=================================================
    OnClientConnected
----
    Searching ignores port number.
=================================================
*/
    template <typename Address>
    EClientLocalID  DefaultClientListener::_OnClientConnected (EChannel channel, const Address &addr,
                                                               const AddrSet_t<Address> &blackList,
                                                               AddrToClient_t<Address> &addrToClient) __Th___
    {
        EXLOCK( _guard );

        if ( blackList.contains( addr ))
            return Default;     // banned

        auto    it = addrToClient.find( addr );
        if ( it != addrToClient.end() )
        {
            _clients[ it->second ].Set( channel, addr );  // throw
            return it->second;  // already connected to another channel
        }

        if ( _clients.size() >= _maxClients )
            return Default;     // too much clients

        EClientLocalID  id = Default;
        for (;; ++_counter)
        {
            if ( not _clients.contains( EClientLocalID(_counter) ))
            {
                id = EClientLocalID(_counter);
                break;
            }
        }

        auto&   client = _clients[id];      // throw

        ASSERT( client.IsEmpty() );
        client.Set( channel, addr );

        addrToClient.emplace( addr, id );   // throw
        return id;
    }

/*
=================================================
    OnClientDisconnected
=================================================
*/
    void  DefaultClientListener::OnClientDisconnected (EChannel channel, EClientLocalID clientId) __NE___
    {
        EXLOCK( _guard );

        auto    it = _clients.find( clientId );
        if ( it != _clients.end() )
        {
            const uint  ch = uint(channel);

            if ( it->second.ipv4Addr[ch].IsValid() )
                _addrToClientV4.erase( it->second.ipv4Addr[ch] );

            if ( it->second.ipv6Addr[ch].IsValid() )
                _addrToClientV6.erase( it->second.ipv6Addr[ch] );

            it->second.ipv4Addr[ch] = Default;
            it->second.ipv6Addr[ch] = Default;

            if ( it->second.IsEmpty() )
                _clients.erase( it );
        }
    }

/*
=================================================
    destructor
=================================================
*/
    DefaultClientListener::~DefaultClientListener () __NE___
    {
        EXLOCK( _guard );

        CHECK( _clients.empty() );
    }


} // AE::Networking
