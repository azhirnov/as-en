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
		Strong<MMemoryID>		_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MBuffer () {}
		~MBuffer ();

		ND_ bool  Create (MResourceManager &, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (const MetalBufferDesc &desc, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ MetalBuffer			Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _buffer; }
		ND_ MMemoryID			MemoryID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool				IsExclusiveSharing ()	const	{ return true; }

		ND_ BufferDesc const&	Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ Bytes				Size ()					const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.size; }
		ND_ void*				MappedPtr ()			const;
		ND_ MDeviceAddress		GetDeviceAddress ()		const;

			void  DidModifyRange (Bytes offset, Bytes size) const;

		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
			
		ND_ static bool	 IsSupported (const MDevice &dev, const BufferDesc &desc);
		ND_ bool		 IsSupported (const MDevice &dev, const BufferViewDesc &desc) const;

		ND_ static bool  IsSupportedForVertex (const MDevice &dev, EVertexType type);
		ND_ static bool  IsSupportedForASVertex (const MDevice &dev, EVertexType type);
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
