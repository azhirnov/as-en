// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/ResourceManager.h"
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Memory Object
	//

	class RMemoryObject final
	{
	// variables
	private:
		GfxMemAllocatorPtr				_memAllocator;
		IGfxMemAllocator::Storage_t		_storage;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RMemoryObject ()									__NE___	{}
		~RMemoryObject ()									__NE___	{}

			void  Destroy (RResourceManager &)				__NE___	{}

	//	ND_ bool  GetMemoryInfo (OUT RemoteMemoryObjInfo &)	C_NE___	{ return false; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
