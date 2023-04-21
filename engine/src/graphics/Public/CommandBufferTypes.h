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


	struct RTShaderBindingTable
	{
		// non-portable
		#ifdef AE_ENABLE_VULKAN
			VkStridedDeviceAddressRegionKHR		raygen		{};
			VkStridedDeviceAddressRegionKHR		miss		{};
			VkStridedDeviceAddressRegionKHR		hit			{};
			VkStridedDeviceAddressRegionKHR		callable	{};

		#elif defined(AE_ENABLE_METAL)
			MetalIntersectionFnTable			intersectionTable;
			MetalVisibleFnTable					visibleTable;

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			// TODO
			
		#else
		#	error not implemented
		#endif

		RTShaderBindingTable () {}
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


	struct DrawIndexedIndirectCommand
	{
		uint	indexCount;
		uint	instanceCount;
		uint	firstIndex;
		int		vertexOffset;
		uint	firstInstance;
	};
	STATIC_ASSERT( sizeof(DrawIndexedIndirectCommand) == 20 );


	struct MeshDrawIndirectCommand
	{
		packed_uint3	taskCount;
	};
	STATIC_ASSERT( sizeof(MeshDrawIndirectCommand) == 12 );


	using TraceRayIndirectCommand = DispatchIndirectCommand;
	
	struct TraceRayIndirectCommand2
	{
		#ifdef AE_ENABLE_VULKAN
			VkDeviceAddress		raygenShaderRecordAddress;
			VkDeviceSize		raygenShaderRecordSize;
			VkDeviceAddress		missShaderBindingTableAddress;
			VkDeviceSize		missShaderBindingTableSize;
			VkDeviceSize		missShaderBindingTableStride;
			VkDeviceAddress		hitShaderBindingTableAddress;
			VkDeviceSize		hitShaderBindingTableSize;
			VkDeviceSize		hitShaderBindingTableStride;
			VkDeviceAddress		callableShaderBindingTableAddress;
			VkDeviceSize		callableShaderBindingTableSize;
			VkDeviceSize		callableShaderBindingTableStride;
			uint				width;
			uint				height;
			uint				depth;

		#elif defined(AE_ENABLE_METAL)
			// TODO

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			// TODO
			
		#else
		#	error not implemented
		#endif
	};
	//STATIC_ASSERT( sizeof(TraceRayIndirectCommand2) == 104 );

//-----------------------------------------------------------------------------



	enum class EStagingHeapType : ubyte
	{
		Static		= 1 << 0,
		Dynamic		= 1 << 1,
		Any			= Static | Dynamic,		// try static, then dynamic
	};

	// TODO: UploadBufferDesc

	
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
		Bytes				pos;
	private:
		Bytes				_size;
		Bytes				_offset;
		Bytes				_blockSize;
		BufferID			_bufferId;	// TODO: Strong ?
		EStagingHeapType	_heapType		= EStagingHeapType::Static;

	// methods
	public:
		BufferStream () __NE___ {}
		BufferStream (BufferID id, Bytes offset, Bytes size, Bytes blockSize = 0_b, EStagingHeapType heapType = EStagingHeapType::Static) __NE___ :
			_size{size}, _offset{offset}, _blockSize{blockSize}, _bufferId{id}, _heapType{heapType} {}

		BufferStream&  operator = (const BufferStream &)	__NE___ = default;
		
		BufferStream&  SetHeapType (EStagingHeapType type)	__NE___	{ _heapType = type;  return *this; }

		ND_ BufferID			Buffer ()					C_NE___	{ return _bufferId; }
		ND_ Bytes				DataSize ()					C_NE___	{ return _size; }
		ND_ Bytes				OffsetAndPos ()				C_NE___	{ return _offset + pos; }
		ND_ Bytes				Begin ()					C_NE___	{ return _offset; }
		ND_ Bytes				End ()						C_NE___	{ return _offset + _size; }
		ND_ Bytes				RemainSize ()				C_NE___	{ return _size - pos; }
		ND_ bool				IsCompleted ()				C_NE___	{ return pos >= _size; }
		ND_ EStagingHeapType	HeapType ()					C_NE___	{ return _heapType; }
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
		ImageStream () __NE___ {}
		ImageStream (ImageID id, const UploadImageDesc &desc) __NE___ :
			_imageId{id}, _desc{desc} {}

		ImageStream&  operator = (const ImageStream &)		__NE___ = default;

		ImageStream&  SetHeapType (EStagingHeapType type)	__NE___	{ _desc.heapType = type;  return *this; }

		ND_ ImageID				Image ()					C_NE___	{ return _imageId; }
		ND_ uint3 const&		Begin ()					C_NE___	{ return _desc.imageOffset; }
		ND_ uint3				End ()						C_NE___	{ return _desc.imageOffset + _desc.imageSize; }
		ND_ uint3 const&		RegionSize ()				C_NE___	{ return _desc.imageSize; }
		ND_ auto const&			ToUploadDesc ()				C_NE___ { return _desc; }
		ND_ Bytes				DataOffset ()				C_NE___	{ return posYZ[0] * _desc.dataRowPitch + posYZ[1] * _desc.dataSlicePitch; }
		ND_ EStagingHeapType	HeapType ()					C_NE___	{ return _desc.heapType; }

		ND_ bool				IsInitialized ()			C_NE___	{ return _imageId != Default; }
		ND_ bool				IsCompleted ()				C_NE___	{ return IsInitialized() & (posYZ[1] >= _desc.imageSize.z); }
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

	
	
	//
	// Video Decode command
	//
	struct VideoDecodeCmd
	{
	};

	
	//
	// Video Encode command
	//
	struct VideoEncodeCmd
	{
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
		Auto	= 0,

		// Allows RG to accumulate batches to minimize CPU overhead.
		Deferred,

		// Submit batch immediately when all render tasks has been complete.
		Immediately,

		// Block untill batch is not submitted
		Force,

		Unknown	= Auto,
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

