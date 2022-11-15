// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/RefCounter.h"
#include "base/DataSource/Stream.h"
#include "base/Utils/NamedID.h"

#include "threading/TaskSystem/Promise.h"
#include "threading/Primitives/DataRaceCheck.h"

namespace AE::VFS::_hidden_
{
	// tag: NamedID UID
	static constexpr uint	NamedIDs_Start	= 4 << 24;

} // AE::VFS::_hidden_


namespace AE::VFS
{
	using namespace AE::Base;

	using AE::Threading::Promise;
	using AE::Threading::SharedMutex;
	
#	if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::DataRaceCheck;
	using AE::Threading::RWDataRaceCheck;
#	endif


	using FileName		= NamedID< 64, VFS::_hidden_::NamedIDs_Start + 1, AE_OPTIMIZE_IDS >;
	using FileGroupName	= NamedID< 64, VFS::_hidden_::NamedIDs_Start + 2, AE_OPTIMIZE_IDS >;

	
	class IVirtualFileStorage;
	class VirtualFileSystem;


} // AE::VFS
