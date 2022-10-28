// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"
#include "threading/TaskSystem/Promise.h"

namespace AE::Threading
{

	//
	// Async Read-only Stream
	//

	class AsyncRStream : public EnableRC< RStream >
	{
	// types
	public:
		struct Result
		{
			const void*		data	= null;
			Bytes			datasize;
			Bytes			offset;
		};


	// methods
	public:
		AsyncRStream () {}

		AsyncRStream (const AsyncRStream &) = delete;
		AsyncRStream (AsyncRStream &&) = delete;

		virtual ~AsyncRStream () {}
		
		AsyncRStream&  operator = (const AsyncRStream &) = delete;
		AsyncRStream&  operator = (AsyncRStream &&) = delete;
		
		ND_ virtual Bytes	Size ()				const = 0;
		ND_ virtual bool	IsThreadSafe ()		const = 0;

		ND_ virtual Promise<Result>  Read (Bytes offset, Bytes size) = 0;
	};



	//
	// Async Write-only Stream
	//

	class AsyncWStream : public EnableRC< WStream >
	{
	// types
	public:
		struct MemBlock
		{
			void *		data	= null;
			Bytes		dataSoze;
		};


	// methods
	public:
		AsyncWStream () {}

		AsyncWStream (const AsyncWStream &) = delete;
		AsyncWStream (AsyncWStream &&) = delete;

		virtual ~AsyncWStream () {}
		
		AsyncWStream&  operator = (const AsyncWStream &) = delete;
		AsyncWStream&  operator = (AsyncWStream &&) = delete;
		
		ND_ virtual Bytes	Size ()				const = 0;
		ND_ virtual bool	IsThreadSafe ()		const = 0;

		// write to file from memory block.
		// memory will be released.
		ND_ virtual Promise<bool>  Write (Bytes offset, MemBlock block) = 0;

		ND_ virtual MemBlock  Alloc (Bytes size) = 0;
	};


} // AE::Threading
