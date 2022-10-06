// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Memory/LfLinearAllocator.h"
#include "threading/Memory/StackAllocator.h"

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


	// variables
	private:
		GlobalLinearAllocator_t		_globalLinear;

		FrameAlloc		_graphicsFrameAlloc;
		FrameAlloc		_simulationFrameAlloc;


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();

		ND_ GlobalLinearAllocator_t&  GetGlobalLinearAllocator ()		{ return _globalLinear; }

		ND_ FrameAlloc&				GetGraphicsFrameAllocator ()		{ return _graphicsFrameAlloc; }
		ND_ FrameAlloc&				GetSimulationFrameAllocator ()		{ return _simulationFrameAlloc; }


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
