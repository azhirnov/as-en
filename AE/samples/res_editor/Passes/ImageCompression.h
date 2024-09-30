// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

	//
	// Image Compression pass
	//

	class ImageCompressionPass final : public IPass
	{
	// types
	private:
		static constexpr uint	_BlockAlign	= 64;
		static constexpr uint	_BlockCount	= 32;

		struct alignas(_BlockAlign) Block
		{
			uint3			offset;
			uint3			dim;
			uint			arrayLayer	= 0;
			uint			mipmap		= 0;
			Bytes32u		srcSize;
			Bytes32u		dstSize;

			ND_ void*			SrcData ()		{ return AlignUp( this + SizeOf<Block>, Bytes{_BlockAlign} ); }
			ND_ void*			DstData ()		{ return AlignUp( SrcData() + srcSize, Bytes{_BlockAlign} ); }

			ND_ ImageMemView	SrcImage (EPixelFormat fmt);
			ND_ ImageMemView	DstImage (EPixelFormat fmt);
		};

		using BlockQueue_t			= Synchronized< RWSpinLock, RingBuffer<Block*> >;
		using AvailableBlockBits_t	= BitfieldAtomic< Base::BitSizeToUInt< _BlockCount >>;

		class CompressBlockTask;


	// variables
	private:
		// immutable
		RC<Image>				_src;
		RC<Image>				_dst;

		ImageID					_srcId;
		ImageID					_dstId;

		uint2					_tileDim;
		uint2					_texelBlockDim;
		uint3					_imageDim;
		uint					_imageLayers		= 0;
		uint					_imageMipmaps		= 0;
		uint					_srcBitsPerBlock;
		uint					_dstBitsPerBlock;
		bool					_decompress;

		EPixelFormat			_srcFormat;
		const EPixelFormat		_dstFormat;
		Bytes					_memBlockSize;

		// mutable
		uint3					_dimOffset;
		uint					_layerOffset		= 0;
		uint					_mipOffset			= 0;

		DynUntypedStorage		_storage;
		AvailableBlockBits_t	_availableBlocks;	// 1 - available block

		BlockQueue_t			_toUpload;


	// methods
	public:
		ImageCompressionPass (RC<Image> src, RC<Image> dst, EPixelFormat dstFormat, StringView dbgName) __NE___;
		~ImageCompressionPass ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync; }
		bool		Execute (SyncPassData &)							__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;

	private:
		ND_ Block*  _AllocBlock ();
			void	_FreeBlock (Block*);
		ND_ Block*	_GetBlockToUpload ();

		ND_ bool	_Initialize ();
	};


} // AE::ResEditor
