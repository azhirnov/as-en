// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Stream/BufferedAsyncStream.h"
#include "graphics/Public/ResourceManager.h"
#include "graphics/Public/CommandBuffer.h"

namespace AE::Graphics
{

	//
	// Resource Upload Manager
	//

	class ResourceUploadManager
	{
	// types
	public:
		struct BufferUploadResult
		{
			BufferID		id;
			Bytes			offset;
			Bytes			size;
		};

		struct ImageUploadResult
		{
			ImageID		id;
			uint3		offset;
			uint3		dimension;
		};

		using BufferResult		= Promise< BufferUploadResult >;
		using ImageResult		= Promise< ImageUploadResult >;
		using TransferContext_t	= ITransferContext;


	private:
		using AsyncReader = Threading::AsyncRStreamBufferedReader;

		// Disk -> RAM -> VRAM

		template <typename StreamType, typename ResultType>
		struct UploadTask
		{
			AsyncReader		diskToMem;
			StreamType		memToGPU;
			ResultType		result;
		};

		using BufferTask_t			= UploadTask< BufferStream, BufferResult >;
		using BufferUploadTasks_t	= Array< Unique< BufferTask_t >>;	// TODO: block allocator

		using ImageTask_t			= UploadTask< ImageStream, ImageResult >;
		using ImageUploadTasks_t	= Array< Unique< ImageTask_t >>;

		static constexpr uint	MaxFailed = 4;


	// variables
	private:
		ImageUploadTasks_t		_imageUpload;
		BufferUploadTasks_t		_bufferUpload;
		RC<IAllocator>			_allocator;


	// methods
	public:
		ResourceUploadManager ();
		~ResourceUploadManager ();

		ND_ ImageResult  ReadImage (ImageID imageId, const uint2 &imageOffset, const uint2 &dim, RC<RStream> stream, const Range<Bytes> &fileRange);
		//ND_ ImageResult  UploadImage ();

		ND_ BufferResult  ReadBuffer (BufferID bufferId, const Range<Bytes> &bufferRange, RC<RStream> stream, const Range<Bytes> &fileRange);
		ND_ BufferResult  ReadBuffer (BufferID bufferId, RC<RStream> stream, const Range<Bytes> &fileRange);
		//ND_ BufferResult  UploadBuffer ();


		void  Upload (TransferContext_t &ctx, EStagingHeapType heapType)
		{
			_UploadImages( ctx, heapType );
			_UploadBuffers( ctx, heapType );
		}

	private:
		void  _UploadImages (TransferContext_t &ctx, EStagingHeapType heapType);
		void  _UploadBuffers (TransferContext_t &ctx, EStagingHeapType heapType);
	};


} // AE::Graphics
