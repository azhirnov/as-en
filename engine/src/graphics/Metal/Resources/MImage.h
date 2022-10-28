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
	// Metal Image immutable data
	//

	class MImage
	{
	// variables
	private:
		MetalImageRC			_image;
		ImageDesc				_desc;
		Strong<MMemoryID>		_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MImage () {}
		~MImage ();
		
		ND_ bool  Create (MResourceManager &, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (const MetalImageDesc &desc, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ MetalImage			Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _image; }
		ND_ ImageDesc const&	Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ MMemoryID			MemoryID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool				IsExclusiveSharing ()	const	{ return true; }
		
		ND_ uint3 const			Dimension ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension; }
		ND_ uint const			Width ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.x; }
		ND_ uint const			Height ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.y; }
		ND_ uint const			Depth ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.z; }
		ND_ uint const			ArrayLayers ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.arrayLayers.Get(); }
		ND_ uint const			MipmapLevels ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.maxLevel.Get(); }
		ND_ EPixelFormat		PixelFormat ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.format; }
		ND_ uint const			Samples ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.samples.Get(); }

		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
			
		ND_ static bool	 IsSupported (const MDevice &dev, const ImageDesc &desc);
		ND_ bool		 IsSupported (const MDevice &dev, const ImageViewDesc &desc) const;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
