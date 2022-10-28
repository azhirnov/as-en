// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Queue.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/PipelineDesc.h"
#include "graphics/Public/RenderPassDesc.h"
#include "graphics/Public/IDs.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/RenderStateEnums.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/BufferMemView.h"
#include "graphics/Public/ImageMemView.h"

namespace AE::Graphics
{

	struct ImageSubresourceRange
	{
		EImageAspect	aspectMask		= Default;
		MipmapLevel		baseMipLevel;
		uint			levelCount		= 1;
		ImageLayer		baseArrayLayer;
		uint			layerCount		= 1;
	};


	struct ImageSubresourceLayers
	{
		EImageAspect	aspectMask		= Default;
		MipmapLevel		mipLevel;
		ImageLayer		baseArrayLayer;
		uint			layerCount		= 1;
	};


	struct BufferCopy
	{
		Bytes			srcOffset;
		Bytes			dstOffset;
		Bytes			size;
	};
		
	// TODO: ImageCopyRegion, ImageCopySlices
	struct ImageCopy
	{
		ImageSubresourceLayers	srcSubres;
		uint3					srcOffset;
		ImageSubresourceLayers	dstSubres;
		uint3					dstOffset;
		uint3					extent;
	};


	struct BufferImageCopy
	{
		Bytes					bufferOffset;
		uint					bufferRowLength		= 0;	// pixels, must be 0 or >= imageExtent.x
		uint					bufferImageHeight	= 0;	// pixels, must be 0 or >= imageExtent.y
		ImageSubresourceLayers	imageSubres;
		uint3					imageOffset;
		uint3					imageExtent;
	};

	struct BufferImageCopy2
	{
		Bytes					bufferOffset;
		Bytes					rowPitch;
		Bytes					slicePitch;
		ImageSubresourceLayers	imageSubres;
		uint3					imageOffset;
		uint3					imageExtent;
	};
	

	struct ImageBlit
	{
		ImageSubresourceLayers	srcSubres;
		uint3					srcOffset0;
		uint3					srcOffset1;
		ImageSubresourceLayers	dstSubres;
		uint3					dstOffset0;
		uint3					dstOffset1;
	};


	struct ImageResolve
	{
		ImageSubresourceLayers	srcSubres;
		uint3					srcOffset;
		ImageSubresourceLayers	dstSubres;
		uint3					dstOffset;
		uint3					extent;
	};
//-----------------------------------------------------------------------------

	

	struct DrawCmd
	{
		uint	vertexCount		= 0;
		uint	instanceCount	= 1;
		uint	firstVertex		= 0;
		uint	firstInstance	= 0;
	};


	struct DrawIndexedCmd
	{
		uint	indexCount		= 0;
		uint	instanceCount	= 1;
		uint	firstIndex		= 0;
		int		vertexOffset	= 0;
		uint	firstInstance	= 0;
	};


	struct DrawIndirectCmd
	{
		BufferID	indirectBuffer;
		Bytes		indirectBufferOffset;
		uint		drawCount				= 1;
		Bytes		stride;
	};


	struct DrawIndexedIndirectCmd
	{
		BufferID	indirectBuffer;
		Bytes		indirectBufferOffset;
		uint		drawCount				= 1;
		Bytes		stride;
	};
//-----------------------------------------------------------------------------



	struct DispatchIndirectCommand
	{
		packed_uint3	groupCount;
	};
	STATIC_ASSERT( sizeof(DispatchIndirectCommand) == 12 );


	struct DrawIndirectCommand
	{
		uint	vertexCount;
		uint	instanceCount;
		uint	firstVertex;
		uint	firstInstance;
	};
	STATIC_ASSERT( sizeof(DrawIndirectCommand) == 16 );
	

	struct MeshDrawIndirectCommand
	{
		packed_uint3	taskCount;
	};
	STATIC_ASSERT( sizeof(MeshDrawIndirectCommand) == 12 );
//-----------------------------------------------------------------------------



	enum class EStagingHeapType : ubyte
	{
		Static		= 1 << 0,
		Dynamic		= 1 << 1,
		Any			= Static | Dynamic,		// try static, then dynamic
	};

	
	//
	// Upload Image Description
	//
	struct UploadImageDesc
	{
		uint3				imageOffset		{0};
		uint3				imageSize		{~0u};
		ImageLayer			arrayLayer;
		MipmapLevel			mipLevel;
		Bytes				dataRowPitch;	// 0 - auto
		Bytes				dataSlicePitch;	// 0 - auto
		EImageAspect		aspectMask		= EImageAspect::Color;	// must only have a single bit set
		EStagingHeapType	heapType		= EStagingHeapType::Dynamic;

		// TODO: setter
	};
	using ReadbackImageDesc = UploadImageDesc;
//-----------------------------------------------------------------------------
	


	//
	// Buffer Stream
	//
	struct BufferStream
	{
	// variables
	public:
		Bytes		pos;
	private:
		Bytes		_size;
		Bytes		_offset;
		Bytes		_blockSize;
		BufferID	_bufferId;	// TODO: Strong ?

	// methods
	public:
		BufferStream () {}
		BufferStream (BufferID id, Bytes offset, Bytes size, Bytes blockSize = 0_b) :
			_size{size}, _offset{offset}, _blockSize{blockSize}, _bufferId{id} {}

		BufferStream&  operator = (const BufferStream &) = default;

		ND_ BufferID	Buffer ()			const	{ return _bufferId; }
		ND_ Bytes		DataSize ()			const	{ return _size; }
		ND_ Bytes		OffsetAndPos ()		const	{ return _offset + pos; }
		ND_ Bytes		Begin ()			const	{ return _offset; }
		ND_ Bytes		End ()				const	{ return _offset + _size; }
		ND_ Bytes		RemainSize ()		const	{ return _size - pos; }
		ND_ bool		IsComplete ()		const	{ return pos >= _size; }
	};


	//
	// Image Stream
	//
	struct ImageStream
	{
	// variables
	public:
		packed_uint2	posYZ;
	private:
		ImageID			_imageId;	// TODO: Strong ?
		UploadImageDesc	_desc;
		
	// methods
	public:
		ImageStream () {}
		ImageStream (ImageID id, const UploadImageDesc &desc) :
			_imageId{id}, _desc{desc} {}

		ImageStream&  operator = (const ImageStream &) = default;

		ND_ ImageID			Image ()		const	{ return _imageId; }
		ND_ uint3 const&	Begin ()		const	{ return _desc.imageOffset; }
		ND_ uint3			End ()			const	{ return _desc.imageOffset + _desc.imageSize; }
		ND_ uint3 const&	RegionSize ()	const	{ return _desc.imageSize; }
		ND_ bool			IsComplete ()	const	{ return posYZ[1] >= _desc.imageSize.z; }

		ND_ UploadImageDesc const&  ToUploadDesc () const { return _desc; }
	};
//-----------------------------------------------------------------------------
	


	//
	// Vertex Stream
	//
	struct VertexStream
	{
		void*		mappedPtr	= null;		// mapped memory for host visible memory
		BufferID	id;						// single buffer for all, bind it once
		Bytes		offset;					// offset in buffer
		Bytes		size;					// same as in request
	};
//-----------------------------------------------------------------------------



	enum class ECommandBufferType : ubyte
	{
		Primary_OneTimeSubmit		= 0,
		Secondary_RenderCommands	= 1,
		//Secondary_OneTimeSubmit	= 3,	// not supported in Metal
		_Count,
		Unknown						= 0xFF,
	};


	enum class ESubmitMode : ubyte
	{
		// Allows RG to accumulate batches to minimize CPU overhead.
		Deferred,

		// Submit batch immediately when all render tasks has been complete.
		Immediately,

		// Block untill batch is not submitted
		Force,
	};


	
	//
	// Begin Frame Config
	//
	struct BeginFrameConfig
	{
		// limit for dynamic staging buffers
		struct {
			Bytes32u	write	{UMax};
			Bytes32u	read	{UMax};
		}	stagingBufferPerFrameLimits;
	};


} // AE::Graphics

