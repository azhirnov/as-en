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
	// Metal Buffer immutable data
	//

	class MBuffer
	{
	// variables
	private:
		MetalBufferRC			_buffer;
		BufferDesc				_desc;
		Strong<MemoryID>		_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MBuffer ()											__NE___	{}
		~MBuffer ()											__NE___;

		ND_ bool  Create (MResourceManager &, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ bool  Create (const MResourceManager &, const MetalBufferDesc &desc, StringView dbgName)					__NE___;
			void  Destroy (MResourceManager &)																			__NE___;
		
		ND_ MetalBuffer			Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _buffer; }
		ND_ MemoryID			MemoryID ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool				IsExclusiveSharing ()		C_NE___	{ return false; }

		ND_ BufferDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ Bytes				Size ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.size; }
		ND_ void*				MappedPtr ()				C_NE___;
		ND_ MDeviceAddress		GetDeviceAddress ()			C_NE___;

			void  DidModifyRange (Bytes offset, Bytes size) C_NE___;

		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
			
		ND_ static bool	 IsSupported (const MResourceManager &, const BufferDesc &desc)			__NE___;
		ND_ bool		 IsSupported (const MResourceManager &, const BufferViewDesc &desc)		C_NE___;

		ND_ static bool  IsSupportedForVertex (const MResourceManager &, EVertexType type)		__NE___;
		ND_ static bool  IsSupportedForASVertex (const MResourceManager &, EVertexType type)	__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
