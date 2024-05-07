// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Buffer immutable data
	//

	class RBuffer final
	{
	// variables
	private:
		RmBufferID					_bufferId;
		RmMemoryID					_memoryId;
		DeviceAddress				_address		= Default;
		BufferDesc					_desc;
		bool						_releaseRef		= false;
		GfxMemAllocatorPtr			_allocator;
		RmDevicePtr					_devicePtr		= Default;
		RC<SharedMem>				_hostMem;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RBuffer ()											__NE___	{}
		~RBuffer ()											__NE___;

		ND_ bool  Create (RResourceManager &, const BufferDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
		ND_ bool  Create (RResourceManager &, const RemoteBufferDesc &, GfxMemAllocatorPtr, StringView)		__NE___;
			void  Destroy (RResourceManager &)																__NE___;

		ND_ bool  GetMemoryInfo (OUT RemoteMemoryObjInfo &)	C_NE___;

		ND_ RmBufferID			Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }
		ND_ DeviceAddress		GetDeviceAddress ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  ASSERT( HasDeviceAddress() );  return _address; }
		ND_ MemoryID			MemoryId ()					C_NE___	{ return Default; }

		ND_ BufferDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ Bytes				Size ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.size; }
		ND_ bool				IsExclusiveSharing ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }
		ND_ bool				HasDeviceAddress ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return AllBits( _desc.usage, EBufferUsage::ShaderAddress ); }

		ND_ bool				IsMapped ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _devicePtr != Default; }
		ND_ RmDevicePtr			GetDevicePtr ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _devicePtr; }
		ND_ void*				GetHostPtr ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _hostMem ? _hostMem->Data() : null; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool	 IsSupported (const RResourceManager &, const BufferDesc &desc)		__NE___;
		ND_ bool		 IsSupported (const RResourceManager &, const BufferViewDesc &desc)	C_NE___;

		ND_ static bool  IsSupportedForVertex (const RResourceManager &, EVertexType type)	__NE___;
		ND_ static bool  IsSupportedForASVertex (const RResourceManager &, EVertexType type)__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
