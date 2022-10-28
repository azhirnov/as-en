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

	class WindowsPageAllocator : Noninstancable
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
		ND_ static void*	Alloc (Bytes size);
		ND_ static void*	AllocLarge (Bytes size);
			static bool		Dealloc (void* ptr);
			
			static bool		Protect (void* ptr, Bytes size, EMemProtection newFlag);
			static bool		Protect (void* ptr, Bytes size, EMemProtection newFlag, OUT EMemProtection &oldFlag);

			static bool		Lock (void* ptr, Bytes size);
			static bool		Unlock (void* ptr, Bytes size);

		ND_ static Bytes	LargePageMinSize ();
		ND_ static bool		EnableLargePageSupport ();
	};

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
