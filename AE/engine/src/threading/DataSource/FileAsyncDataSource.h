// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "threading/DataSource/WinAsyncDataSource.h"
#include "threading/DataSource/UnixAsyncDataSource.h"

namespace AE::Threading
{

#ifdef AE_PLATFORM_WINDOWS

	using FileAsyncRDataSource	= WinAsyncRDataSource;
	using FileAsyncWDataSource	= WinAsyncWDataSource;

#else

	using FileAsyncRDataSource	= UnixAsyncRDataSource;
	using FileAsyncWDataSource	= UnixAsyncWDataSource;

#endif

} // AE::Threading
