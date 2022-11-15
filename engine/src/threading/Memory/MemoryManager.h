// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/InPlace.h"
#include "base/Memory/IAllocator.h"
#include "base/Memory/UntypedAllocator.h"

#include "threading/Memory/LfLinearAllocator.h"
#include "threading/Memory/StackAllocator.h"
#include "threading/Memory/MemoryProfiler.h"

namespace AE::Threading { class MemoryManagerImpl; }
namespace AE { Threading::MemoryManagerImpl&  MemoryManager () __NE___; }

namespace AE::Threading
{

	//
	// Memory Manager
	//

	class MemoryManagerImpl final : public Noncopyable
	{
	// types
	public:
		using GlobalLinearAllocator_t	= LfLinearAllocator< 16u<<20, AE_CACHE_LINE, 32 >;
		using FrameAllocator_t			= StackAllocator< UntypedAllocator, 16, true >;
		
		struct FrameAlloc
		{
		private:
			uint				_idx	: 2;	// TODO: atomic ?
			FrameAllocator_t	_alloc	[4];

		public:
			FrameAlloc ()					__NE___	: _idx{0} {}

			void  Next ()					__NE___	{ ++_idx;  _alloc[_idx].Discard(); }

			ND_ FrameAllocator_t&  Get ()	__NE___	{ return _alloc[_idx]; }
		};

		using DefaultAlloc_t		= AllocatorImpl< UntypedAllocator >;


	// variables
	private:
		GlobalLinearAllocator_t			_globalLinear;

		FrameAlloc						_graphicsFrameAlloc;
		FrameAlloc						_simulationFrameAlloc;

		InPlace<DefaultAlloc_t>			_defaultAlloc;
		
		PROFILE_ONLY(
			AtomicRC<IMemoryProfiler>	_profiler;
		)


	// methods
	public:
		static void  CreateInstance ()									__NE___;
		static void  DestroyInstance ()									__NE___;
		
		void  SetProfiler (RC<IMemoryProfiler> profiler)				__NE___;

		ND_ GlobalLinearAllocator_t&	GetGlobalLinearAllocator ()		__NE___	{ return _globalLinear; }

		ND_ FrameAlloc&					GetGraphicsFrameAllocator ()	__NE___	{ return _graphicsFrameAlloc; }
		ND_ FrameAlloc&					GetSimulationFrameAllocator ()	__NE___	{ return _simulationFrameAlloc; }

		ND_ RC<IAllocator>				GetDefaultAllocator ()			__NE___	{ return _defaultAlloc->GetRC(); }


	private:
		MemoryManagerImpl ()	__NE___;
		~MemoryManagerImpl ()	__NE___;
		
		friend MemoryManagerImpl&  AE::MemoryManager () __NE___;

		ND_ static MemoryManagerImpl*  _Instance () __NE___;
	};

} // AE::Threading


namespace AE
{
/*
=================================================
	MemoryManager
=================================================
*/
	ND_ forceinline Threading::MemoryManagerImpl&  MemoryManager () __NE___
	{
		return *Threading::MemoryManagerImpl::_Instance();
	}

} // AE
