// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/VFS.pch.h"

namespace AE::VFS::_hidden_
{
	// tag: NamedID UID
	static constexpr uint	NamedIDs_Start	= 5 << 24;

} // AE::VFS::_hidden_


namespace AE::Networking
{
	class ClientServerBase;

} // AE::Networking


namespace AE::VFS
{
	using namespace AE::Base;

	using AE::Threading::Promise;
	using AE::Threading::AsyncRDataSource;
	using AE::Threading::AsyncWDataSource;
	using AE::Threading::AsyncRStream;
	using AE::Threading::AsyncWStream;

	#if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::DataRaceCheck;
	using AE::Threading::RWDataRaceCheck;
	#endif


	using FileName		= NamedID< 128, VFS::_hidden_::NamedIDs_Start + 1, AE_OPTIMIZE_IDS >;
	using FileGroupName	= NamedID<  64, VFS::_hidden_::NamedIDs_Start + 2, AE_OPTIMIZE_IDS >;
	using StorageName	= NamedID<  64, VFS::_hidden_::NamedIDs_Start + 3, AE_OPTIMIZE_IDS >;


	class IVirtualFileStorage;
	class VirtualFileSystem;


} // AE::VFS
