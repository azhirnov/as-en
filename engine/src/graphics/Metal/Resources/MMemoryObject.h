// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Memory Object
	//

	class MMemoryObject final
	{
	// variables
	private:
		GfxMemAllocatorPtr			_memAllocator;
		IGfxMemAllocator::Storage_t	_storage;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MMemoryObject () {}
		~MMemoryObject ();
		
		ND_ bool  Create (OUT MetalBufferRC &buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (OUT MetalImageRC &image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (OUT MetalAccelStructRC &as, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (OUT MetalAccelStructRC &as, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ bool  GetMemoryInfo (OUT MetalMemoryObjInfo &) const;

		DEBUG_ONLY(  ND_ StringView  GetDebugName () const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
