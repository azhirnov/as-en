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
		NDRST( static void *)  Allocate (Bytes size)											__NE___	{ return Allocate( POTValue::From(ulong(size)), DefaultAlign ); }
		NDRST( static void *)  Allocate (Bytes size, const SourceLoc &loc)						__NE___	{ return Allocate( POTValue::From(ulong(size)), DefaultAlign, loc ); }
		
		static void  Deallocate (void *ptr)														__NE___;
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size)											__NE___	{ return Deallocate( ptr, POTValue::From(ulong(size)) ); }
		

	// custom align //
		NDRST( static void *)  Allocate (const SizeAndAlign sizeAndAlign)						__NE___	{ return Allocate( POTValue::From(ulong(sizeAndAlign.size)), sizeAndAlign.align ); }
		NDRST( static void *)  Allocate (const SizeAndAlign sizeAndAlign, const SourceLoc &loc)	__NE___	{ return Allocate( POTValue::From(ulong(sizeAndAlign.size)), sizeAndAlign.align, loc ); }
		
		//static void  Deallocate (void *ptr, Bytes align)										__NE___	{ Unused( align );  return Deallocate( ptr ); }
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)					__NE___	{ Unused( sizeAndAlign.align );  return Deallocate( ptr, POTValue::From(ulong(sizeAndAlign.size)) ); }
		
		
	// custom align and POT size //
		NDRST( static void *)  Allocate (POTValue size, Bytes align)							__NE___	{ return Allocate( size, align, SourceLoc_Current() ); }
		NDRST( static void *)  Allocate (POTValue size, Bytes align, const SourceLoc &loc)		__NE___;
		
		static void  Deallocate (void* ptr, POTValue size)										__NE___;


	// utils
		ND_ static Bytes	ReservedSize ()							__NE___;
		ND_ static usize	MaxAllowedAllocations (Bytes size)		__NE___	{ return MaxAllowedAllocations( POTValue::From(ulong(size)) ); }
		ND_ static usize	MaxAllowedAllocations (POTValue size)	__NE___;
			static void		ReleaseAll ()							__NE___;
	};


} // AE::Base
