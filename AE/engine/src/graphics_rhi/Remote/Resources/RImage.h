// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/ImageDesc.h"
# include "graphics_rhi/Public/EResourceState.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Image immutable data
	//

	class RImage final
	{
	// variables
	private:
		RmImageID						_imageId;
		RmMemoryID						_memoryId;
		ImageDesc						_desc;
		bool							_releaseRef		= false;
		GfxMemAllocatorPtr				_allocator;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RImage ()											__NE___	{}
		~RImage ()											__NE___;

		ND_ bool  Create (ResourceManager &, const ImageDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
		ND_ bool  Create (ResourceManager &, const RemoteImageDesc &, GfxMemAllocatorPtr, StringView)		__NE___;
			void  Destroy (ResourceManager &)																__NE___;

		ND_ bool  GetMemoryInfo (OUT RemoteMemoryObjInfo &)	C_NE___;

		ND_ RmImageID			Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		ND_ ImageDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ MemoryID			MemoryId ()					C_NE___	{ return Default; }

		ND_ uint3 const			Dimension ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.Dimension(); }
		ND_ uint				Width ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.x; }
		ND_ uint				Height ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.y; }
		ND_ uint				Depth ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.z; }
		ND_ uint				ArrayLayers ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.arrayLayers.Get(); }
		ND_ uint				MipmapLevels ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.mipLevels.Get(); }
		ND_ EPixelFormat		PixelFormat ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.format; }
		ND_ uint				Samples ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.samples.Get(); }

		ND_ bool				IsExclusiveSharing ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool	 IsSupported (const ResourceManager &, const ImageDesc &desc)		__NE___;
		ND_ static bool	 IsSupported (const ResourceManager &, const ImageDesc &, const ImageViewDesc &) __NE___;

		ND_ static Bytes  GetMemoryAlignment (const RDevice &dev, const ImageDesc &desc)	__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
