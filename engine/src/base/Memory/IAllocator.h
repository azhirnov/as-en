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

		virtual ~IAllocator () {}

		ND_ virtual void*  Allocate (Bytes size) = 0;
			virtual void   Deallocate (void *ptr, Bytes size) = 0;
	};
	


	//
	// Aligned Allocator interface
	//
	class IAlignedAllocator : public EnableRC<IAlignedAllocator>
	{
	public:
		static constexpr bool	IsThreadSafe = false;	// TODO ?

		template <typename T>	using StdAlloc_t = StdAllocatorRef< T, IAlignedAllocator >;
		
		virtual ~IAlignedAllocator () {}

		ND_ virtual void*  Allocate (Bytes size, Bytes align) = 0;
			virtual void   Deallocate (void *ptr, Bytes size, Bytes align) = 0;
	};

	

	template <typename T>
	class AllocatorImpl final : public IAllocator
	{
	// variables
	private:
		T	_alloc;

	// methods
	public:
		AllocatorImpl () {}
		explicit AllocatorImpl (T alloc) : _alloc{alloc} {}
		~AllocatorImpl () override {}

		void*  Allocate (Bytes size)				override	{ return _alloc.Allocate( size ); }
		void   Deallocate (void *ptr, Bytes size)	override	{ return _alloc.Deallocate( ptr, size ); }
	};
	

	template <typename T>
	class AllocatorImpl< AllocatorRef<T> > final : public IAllocator
	{
	// variables
	private:
		T	_alloc;

	// methods
	public:
		AllocatorImpl () {}
		explicit AllocatorImpl (T alloc) : _alloc{alloc} {}
		explicit AllocatorImpl (const AllocatorRef<T> &ref) : _alloc{ref.GetAllocatorRef()} {}
		~AllocatorImpl () override {}

		void*  Allocate (Bytes size)				override	{ return _alloc.Allocate( size ); }
		void   Deallocate (void *ptr, Bytes size)	override	{ return _alloc.Deallocate( ptr, size ); }
	};


	template <typename T>
	class AlignedAllocatorImpl final : public IAlignedAllocator
	{
	// variables
	private:
		T	_alloc;

	// methods
	public:
		AlignedAllocatorImpl () {}
		explicit AlignedAllocatorImpl (T alloc) : _alloc{alloc} {}
		~AlignedAllocatorImpl () override {}

		void*  Allocate (Bytes size, Bytes align)				override	{ return _alloc.Allocate( size, align ); }
		void   Deallocate (void *ptr, Bytes size, Bytes align)	override	{ return _alloc.Deallocate( ptr, size, align ); }
	};


	template <typename T>
	class AlignedAllocatorImpl< AlignedAllocatorRef<T> > final : public IAlignedAllocator
	{
	// variables
	private:
		T	_alloc;

	// methods
	public:
		AlignedAllocatorImpl () {}
		explicit AlignedAllocatorImpl (T alloc) : _alloc{alloc} {}
		explicit AlignedAllocatorImpl (const AlignedAllocatorRef<T> &ref) : _alloc{ref.GetAllocatorRef()} {}
		~AlignedAllocatorImpl () override {}

		void*  Allocate (Bytes size, Bytes align)				override	{ return _alloc.Allocate( size, align ); }
		void   Deallocate (void *ptr, Bytes size, Bytes align)	override	{ return _alloc.Deallocate( ptr, size, align ); }
	};


} // AE::Base
