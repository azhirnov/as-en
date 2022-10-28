// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/InPlace.h"
#include "base/Memory/IAllocator.h"
#include "base/Memory/UntypedAllocator.h"

#include "threading/Memory/LfLinearAllocator.h"
#include "threading/Memory/StackAllocator.h"
#include "threading/Memory/MemoryProfiler.h"

namespace AE::Threading
{

	//
	// Memory Manager
	//

	class MemoryManager final : public Noncopyable
	{
	// types
	public:
		using GlobalLinearAllocator_t	= LfLinearAllocator< 16u<<20, AE_CACHE_LINE, 32 >;
		using FrameAllocator_t			= StackAllocator< UntypedAlignedAllocator, 16, true >;
		
		struct FrameAlloc
		{
		private:
			uint				_idx	: 2;	// TODO: atomic ?
			FrameAllocator_t	_alloc	[4];

		public:
			FrameAlloc () : _idx{0} {}

			void  Next ()	{ ++_idx;  _alloc[_idx].Discard(); }

			ND_ FrameAllocator_t&  Get ()	{ return _alloc[_idx]; }
		};

		using DefaultAlloc_t		= AllocatorImpl< UntypedAllocator >;
		using DefaultAlignedAlloc_t	= AlignedAllocatorImpl< UntypedAlignedAllocator >;


	// variables
	private:
		GlobalLinearAllocator_t			_globalLinear;

		FrameAlloc						_graphicsFrameAlloc;
		FrameAlloc						_simulationFrameAlloc;

		InPlace<DefaultAlloc_t>			_defaultAlloc;
		InPlace<DefaultAlignedAlloc_t>	_defaultAlignedAlloc;
		
		PROFILE_ONLY(
			AtomicRC<IMemoryProfiler>	_profiler;
		)


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();
		
		void  SetProfiler (RC<IMemoryProfiler> profiler);

		ND_ GlobalLinearAllocator_t&	GetGlobalLinearAllocator ()		{ return _globalLinear; }

		ND_ FrameAlloc&					GetGraphicsFrameAllocator ()	{ return _graphicsFrameAlloc; }
		ND_ FrameAlloc&					GetSimulationFrameAllocator ()	{ return _simulationFrameAlloc; }

		ND_ RC<IAllocator>				GetDefaultAllocator ()			{ return _defaultAlloc->GetRC(); }
		ND_ RC<IAlignedAllocator>		GetDefaultAlignedAllocator ()	{ return _defaultAlignedAlloc->GetRC(); }


	private:
		MemoryManager ();
		~MemoryManager ();
		
		friend MemoryManager&  MemoryManagerInstance ();

		ND_ static MemoryManager*  _Instance ();
	};

	
/*
=================================================
	MemoryManagerInstance
=================================================
*/
	ND_ inline MemoryManager&  MemoryManagerInstance ()
	{
		return *MemoryManager::_Instance();
	}

} // AE::Threading
