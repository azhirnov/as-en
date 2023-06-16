// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Utils/Helpers.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Base
{

	//
	// Windows Page Allocator
	//

	class WindowsPageAllocator : Noninstanceable
	{
	// types
	public:
		enum class EMemProtection
		{
			NoAccess,
			Readonly,
			ReadWrite,
		};


	// methods
	public:
		ND_ static void*	Alloc (Bytes size)				__NE___;
		ND_ static void*	AllocLarge (Bytes size)			__NE___;
			static bool		Dealloc (void* ptr)				__NE___;
			
			static bool		Protect (void* ptr, Bytes size, EMemProtection newFlag)								 __NE___;
			static bool		Protect (void* ptr, Bytes size, EMemProtection newFlag, OUT EMemProtection &oldFlag) __NE___;

			static bool		Lock (void* ptr, Bytes size)	__NE___;
			static bool		Unlock (void* ptr, Bytes size)	__NE___;

		ND_ static Bytes	LargePageMinSize ()				__NE___;
		ND_ static bool		EnableLargePageSupport ()		__NE___;
	};

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
