// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Math/Bytes.h"
#include "base/Math/POTValue.h"
#include "base/Utils/SourceLoc.h"

namespace AE::Base
{

	//
	// Multithreaded (Lock-free) Allocator
	//

	struct MtAllocator
	{
		static constexpr bool	IsThreadSafe	= true;
		static constexpr Bytes	DefaultAlign	= 8_b;

		
	// default align //
		NDRST( static void *)  Allocate (Bytes size)										{ return Allocate( POTValue::From(ulong(size)), DefaultAlign ); }
		NDRST( static void *)  Allocate (Bytes size, const SourceLoc &loc)					{ return Allocate( POTValue::From(ulong(size)), DefaultAlign, loc ); }
		
		static void  Deallocate (void *ptr);
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size)										{ return Deallocate( ptr, POTValue::From(ulong(size)) ); }
		

	// custom align //
		NDRST( static void *)  Allocate (Bytes size, Bytes align)							{ return Allocate( POTValue::From(ulong(size)), align ); }
		NDRST( static void *)  Allocate (Bytes size, Bytes align, const SourceLoc &loc)		{ return Allocate( POTValue::From(ulong(size)), align, loc ); }
		
		//static void  Deallocate (void *ptr, Bytes align)									{ Unused( align );  return Deallocate( ptr ); }
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size, Bytes align)						{ Unused( align );  return Deallocate( ptr, POTValue::From(ulong(size)) ); }
		
		
	// custom align and POT size //
		NDRST( static void *)  Allocate (POTValue size, Bytes align)						{ return Allocate( size, align, SourceLoc_Current() ); }
		NDRST( static void *)  Allocate (POTValue size, Bytes align, const SourceLoc &loc);
		
		static void  Deallocate (void* ptr, POTValue size);


	// utils
		ND_ static Bytes	ReservedSize ();
		ND_ static usize	MaxAllowedAllocations (Bytes size)		{ return MaxAllowedAllocations( POTValue::From(ulong(size)) ); }
		ND_ static usize	MaxAllowedAllocations (POTValue size);
			static void		ReleaseAll ();
	};


} // AE::Base
