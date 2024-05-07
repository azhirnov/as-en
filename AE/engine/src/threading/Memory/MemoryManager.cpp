// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Memory/TsSharedMem.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

/*
=================================================
	FrameAlloc::BeginFrame
=================================================
*/
	void  MemoryManagerImpl::FrameAlloc::BeginFrame (FrameUID frameId) __NE___
	{
		const uint	idx = frameId.Index();
		_alloc[idx].Discard();

		DEBUG_ONLY( _dbgFrameId.store( frameId );)
		_idx.store( idx );
	}

/*
=================================================
	FrameAlloc::EndFrame
=================================================
*/
	void  MemoryManagerImpl::FrameAlloc::EndFrame (FrameUID frameId) __NE___
	{
		ASSERT( _dbgFrameId.load() == frameId );
		DEBUG_ONLY( _dbgFrameId.store( Default );)
		Unused( frameId );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	MemoryManagerImpl::MemoryManagerImpl () __NE___
	{
	}

/*
=================================================
	destructor
=================================================
*/
	MemoryManagerImpl::~MemoryManagerImpl () __NE___
	{
	}

/*
=================================================
	_Instance
=================================================
*/
	INTERNAL_LINKAGE( InPlace<MemoryManagerImpl>  s_MemoryManager );

	MemoryManagerImpl&  MemoryManagerImpl::_Instance () __NE___
	{
		return s_MemoryManager.AsRef();
	}

/*
=================================================
	InstanceCtor
=================================================
*/
	void  MemoryManagerImpl::InstanceCtor::Create () __NE___
	{
		s_MemoryManager.Create();
	}

	void  MemoryManagerImpl::InstanceCtor::Destroy () __NE___
	{
		s_MemoryManager.Destroy();
	}

/*
=================================================
	SetProfiler
=================================================
*/
	void  MemoryManagerImpl::SetProfiler (RC<IMemoryProfiler> profiler) __NE___
	{
		PROFILE_ONLY(
			_profiler.store( RVRef(profiler) );
		)
		Unused( profiler );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	TsSharedMem::Create
=================================================
*/
	RC<TsSharedMem>  TsSharedMem::Create (RC<IAllocator> alloc, Bytes size, Bytes align) __NE___
	{
		if_likely( alloc and size > 0 )
		{
			auto	align_pot	= POTBytes{ align };
			void*	self		= alloc->Allocate( _CalcSize( size, align_pot ));

			if_likely( self != null )
				return RC<TsSharedMem>{ new(self) TsSharedMem{ size, align_pot, RVRef(alloc) }};
		}
		return Default;
	}

	RC<TsSharedMem>  TsSharedMem::Create (RC<IAllocator> alloc, const SizeAndAlign sizeAndAlign) __NE___
	{
		return TsSharedMem::Create( RVRef(alloc), sizeAndAlign.size, sizeAndAlign.align );
	}

	RC<TsSharedMem>  TsSharedMem::Create (Bytes size, Bytes align) __NE___
	{
		return TsSharedMem::Create( AE::GetDefaultAllocator(), size, align );
	}


} // AE::Threading
