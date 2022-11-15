// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Memory/AllocatorRef.h"
#include "base/Math/Bytes.h"
#include "base/Utils/RefCounter.h"

namespace AE::Base
{
	
	//
	// Allocator interface
	//
	class IAllocator : public EnableRC<IAllocator>
	{
	public:
		static constexpr bool	IsThreadSafe = false;	// TODO ?

		template <typename T>	using StdAlloc_t = StdAllocatorRef< T, IAllocator >;

			virtual ~IAllocator ()													__NE___	{}

		ND_ virtual void*  Allocate (Bytes size)									__NE___ = 0;
			virtual void   Deallocate (void *ptr, Bytes size)						__NE___ = 0;

		ND_ virtual void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE___ = 0;
			virtual void   Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)	__NE___ = 0;
	};

	

	template <typename T>
	class AllocatorImpl final : public IAllocator
	{
	// variables
	private:
		T	_alloc;

	// methods
	public:
		AllocatorImpl ()												__NE___ {}
		AllocatorImpl (AllocatorImpl && other)							__NE___ : _alloc{ RVRef(other._alloc) } {}

		template <typename ...Args>
		explicit AllocatorImpl (Args&& ... args)						__TH___ : _alloc{ FwdArg<Args>(args)... } {}

		~AllocatorImpl ()												__NE_OV {}

		void*  Allocate (Bytes size)									__NE_OV	{ return _alloc.Allocate( size ); }
		void   Deallocate (void *ptr, Bytes size)						__NE_OV	{ return _alloc.Deallocate( ptr, size ); }

		void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE_OV	{ return _alloc.Allocate( sizeAndAlign ); }
		void   Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)	__NE_OV	{ return _alloc.Deallocate( ptr, sizeAndAlign ); }
	};
	

	template <typename T>
	class AllocatorImpl< AllocatorRef<T> > final : public IAllocator
	{
	// variables
	private:
		T	_alloc;

	// methods
	public:
		AllocatorImpl ()												__NE___ {}
		AllocatorImpl (AllocatorImpl && other)							__NE___ : _alloc{ RVRef(other._alloc) } {}
		explicit AllocatorImpl (const AllocatorRef<T> &ref)				__NE___ : _alloc{ ref.GetAllocatorRef() } {}
		
		template <typename ...Args>
		explicit AllocatorImpl (Args&& ... args)						__TH___ : _alloc{ FwdArg<Args>(args)... } {}

		~AllocatorImpl ()												__NE_OV {}

		void*  Allocate (Bytes size)									__NE_OV	{ return _alloc.Allocate( size ); }
		void   Deallocate (void *ptr, Bytes size)						__NE_OV	{ return _alloc.Deallocate( ptr, size ); }

		void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE_OV	{ return _alloc.Allocate( sizeAndAlign ); }
		void   Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)	__NE_OV	{ return _alloc.Deallocate( ptr, sizeAndAlign ); }
	};


	template <typename T>
	class AllocatorImpl2 final : public IAllocator
	{
	// variables
	private:
		T	_alloc;

		static constexpr Bytes	_BaseAlign	{__STDCPP_DEFAULT_NEW_ALIGNMENT__};

	// methods
	public:
		AllocatorImpl2 ()												__NE___ {}
		AllocatorImpl2 (AllocatorImpl2 && other)						__NE___ : _alloc{ RVRef(other._alloc) } {}
		
		template <typename ...Args>
		explicit AllocatorImpl2 (Args&& ... args)						__TH___ : _alloc{ FwdArg<Args>(args)... } {}

		~AllocatorImpl2 ()												__NE_OV {}

		void*  Allocate (Bytes size)									__NE_OV	{ return _alloc.Allocate( SizeAndAlign{ size, _BaseAlign }); }
		void   Deallocate (void *ptr, Bytes size)						__NE_OV	{ return _alloc.Deallocate( ptr, SizeAndAlign{ size, _BaseAlign }); }

		void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE_OV	{ return _alloc.Allocate( sizeAndAlign ); }
		void   Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)	__NE_OV	{ return _alloc.Deallocate( ptr, sizeAndAlign ); }
	};
	

	template <typename T>
	class AllocatorImpl2< AllocatorRef<T> > final : public IAllocator
	{
	// variables
	private:
		T	_alloc;
		
		static constexpr Bytes	_BaseAlign	{__STDCPP_DEFAULT_NEW_ALIGNMENT__};

	// methods
	public:
		AllocatorImpl2 ()												__NE___ {}
		AllocatorImpl2 (AllocatorImpl2 && other)						__NE___ : _alloc{ RVRef(other._alloc) } {}
		explicit AllocatorImpl2 (const AllocatorRef<T> &ref)			__NE___ : _alloc{ ref.GetAllocatorRef() } {}
		
		template <typename ...Args>
		explicit AllocatorImpl2 (Args&& ... args)						__TH___ : _alloc{ FwdArg<Args>(args)... } {}

		~AllocatorImpl2 ()												__NE_OV {}
		
		void*  Allocate (Bytes size)									__NE_OV	{ return _alloc.Allocate( SizeAndAlign{ size, _BaseAlign }); }
		void   Deallocate (void *ptr, Bytes size)						__NE_OV	{ return _alloc.Deallocate( ptr, SizeAndAlign{ size, _BaseAlign }); }

		void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE_OV	{ return _alloc.Allocate( sizeAndAlign ); }
		void   Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)	__NE_OV	{ return _alloc.Deallocate( ptr, sizeAndAlign ); }
	};


} // AE::Base
