// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Raw/IpAddress.h"

namespace AE::Networking
{

	//
	// Base Socket for BSD system
	//

	class BaseSocket
	{
	// types
	protected:
		#ifdef AE_PLATFORM_WINDOWS
			enum class Socket_t : usize { Unknown = ~usize(0) };
		#elif defined(AE_PLATFORM_UNIX_BASED)
			enum class Socket_t : int { Unknown = -1 };
		#else
			#error Unsupported platform!
		#endif


	// variables
	protected:
		Socket_t	_handle	= Default;


	// methods
	public:
		BaseSocket ();
		~BaseSocket ();
		
			void  Close ();

		ND_ bool  IsOpen ()	const	{ return _handle != Default; }

			bool  GetAddress (OUT IpAddress &outAddr) const;

	protected:
		ND_ bool  _SetNonBlocking ();
	};

} // AE::Networking
