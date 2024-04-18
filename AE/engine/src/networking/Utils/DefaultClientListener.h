// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/HighLevel/Server.h"

namespace AE::Networking
{

	//
	// Default Client Listener
	//

	class DefaultClientListener : public IClientListener
	{
	// types
	private:
		template <typename T>
		struct AddrHash {
			ND_ usize  operator () (const T &key) C_NE___ { return usize(key.CalcHashOfAddress()); }
		};

		template <typename T>
		struct AddrEqual {
			ND_ bool  operator () (const T &lhs, const T &rhs) C_NE___ { return lhs.EqualAddress( rhs ); }
		};

		template <typename T>	using AddrSet_t			= FlatHashSet< T, AddrHash<T>, AddrEqual<T> >;
		template <typename T>	using AddrToClient_t	= FlatHashMap< T, EClientLocalID, AddrHash<T>, AddrEqual<T> >;

		using BlackListV4_t		= AddrSet_t< IpAddress  >;
		using BlackListV6_t		= AddrSet_t< IpAddress6 >;

		using AddrToClientV4_t	= AddrToClient_t< IpAddress  >;
		using AddrToClientV6_t	= AddrToClient_t< IpAddress6 >;

		struct ClientInfo
		{
			StaticArray< IpAddress,  uint(EChannel::_Count) >	ipv4Addr;
			StaticArray< IpAddress6, uint(EChannel::_Count) >	ipv6Addr;

			ND_ bool  IsEmpty ()								C_NE___;

				void  Set (EChannel ch, const IpAddress &addr)	__NE___	{ ipv4Addr[uint(ch)] = addr; }
				void  Set (EChannel ch, const IpAddress6 &addr)	__NE___	{ ipv6Addr[uint(ch)] = addr; }
		};

		using ClientMap_t	= FlatHashMap< EClientLocalID, ClientInfo >;

		static constexpr uint	c_MaxClients = Max( NetConfig::TCP_Reliable_MaxClients, 0u );	// TODO: other channels


	// variables
	private:
		mutable Threading::SpinLock		_guard;

		uint					_counter		= 0;
		uint					_maxClients		= c_MaxClients;

		ClientMap_t				_clients;

		AddrToClientV4_t		_addrToClientV4;
		AddrToClientV6_t		_addrToClientV6;

		BlackListV4_t			_blackListV4;
		BlackListV6_t			_blackListV6;


	// methods
	public:
		DefaultClientListener ()											__NE___ {}
		~DefaultClientListener ()											__NE___;

		void  AddToBlackList (ArrayView<IpAddress>)							__NE___;
		void  AddToBlackList (ArrayView<IpAddress6>)						__NE___;

		void  RemoveFromBlackList (ArrayView<IpAddress>)					__NE___;
		void  RemoveFromBlackList (ArrayView<IpAddress6>)					__NE___;

		void  SetMaxClients (uint count)									__NE___;

		ND_ usize  ClientCount ()											C_NE___	{ EXLOCK( _guard );  return _clients.size(); }


		// IClientListener //
		EClientLocalID  OnClientConnected (EChannel, const IpAddress &)		__NE_OV;
		EClientLocalID  OnClientConnected (EChannel, const IpAddress6 &)	__NE_OV;

		void  OnClientDisconnected (EChannel, EClientLocalID)				__NE_OV;

	private:
		template <typename Address>
		ND_ EClientLocalID  _OnClientConnected (EChannel,
												const Address &,
												const AddrSet_t<Address> &,
												AddrToClient_t<Address> &)	__Th___;
	};


} // AE::Networking
