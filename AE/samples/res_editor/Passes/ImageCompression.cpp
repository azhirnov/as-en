// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/ImageCompression.h"
#include "res_editor/Resources/Image.h"

#ifdef AE_ENABLE_COMPRESSONATOR
# include "compressonator.h"
namespace AE::ResEditor { namespace {
#	include "Compressonator.cpp.h"
}}
#endif

#ifdef AE_ENABLE_ASTC_ENCODER
# include "astcenc.h"
namespace AE::ResEditor { namespace {
#	include "AstcEncoder.cpp.h"
}}
#endif

namespace AE::ResEditor
{
	static constexpr EStagingHeapType	c_HeapType	= EStagingHeapType::Static;
	static constexpr uint				c_TileSize	= 32;

/*
=================================================
	SrcImage / DstImage
=================================================
*/
	ImageMemView  ImageCompressionPass::Block::SrcImage (EPixelFormat fmt)
	{
		return ImageMemView{ SrcData(), srcSize, offset, dim, 0_b, 0_b, fmt, EImageAspect::Color };
	}

	ImageMemView  ImageCompressionPass::Block::DstImage (EPixelFormat fmt)
	{
		return ImageMemView{ DstData(), dstSize, offset, dim, 0_b, 0_b, fmt, EImageAspect::Color };
	}

/*
=================================================
	CompressBlockTask
=================================================
*/
	class ImageCompressionPass::CompressBlockTask final : public Threading::IAsyncTask
	{
	// variables
	private:
		Ptr<Block>					_block;
		RC<ImageCompressionPass>	_pass;

	// methods
	public:
		CompressBlockTask (RC<ImageCompressionPass> pass, Block* block) __NE___ :
			IAsyncTask{ ETaskQueue::Background },
			_block{block}, _pass{pass}
		{}

		~CompressBlockTask ()
		{
			if ( _block and _pass )
				_pass->_FreeBlock( _block.get() );
		}

		void  Run () __Th_OV
		{
			CHECK_TE( _block and _pass );

			const EPixelFormat	src_fmt			= _pass->_srcFormat;
			const EPixelFormat	dst_fmt			= _pass->_dstFormat;
			const uint			thread_count	= 0;	// current thread
			const float			quality			= 1.f;	// best

		  #ifdef AE_ENABLE_COMPRESSONATOR
			if ( EPixelFormat_IsETC( dst_fmt ) or EPixelFormat_IsBC( dst_fmt ))
			{
				CHECK_TE( Compressonator_Compress( _block->SrcImage(src_fmt), _block->DstImage(dst_fmt), thread_count, quality ));

				if ( _pass->_decompress )
					CHECK_TE( Compressonator_Compress( _block->DstImage(dst_fmt), _block->SrcImage(src_fmt), thread_count, quality ));
			}
			else
		  #endif

		  #ifdef AE_ENABLE_ASTC_ENCODER
			if ( EPixelFormat_IsASTC( dst_fmt ))
			{
				CHECK_TE( AstcEncode( _block->SrcImage(src_fmt), _block->DstImage(dst_fmt), thread_count, quality ));

				if ( _pass->_decompress )
					CHECK_TE( AstcDecode( _block->DstImage(dst_fmt), _block->SrcImage(src_fmt), thread_count ));
			}
			else
		  #endif
			{
				CHECK_TE( false );
			}

			_pass->_toUpload->push_back( _block.get() );

			_block	= null;
			_pass	= null;
		}

		StringView  DbgName () C_NE_OV	{ return "CompressImageBlock"; }
	};

/*
=================================================
	constructor
=================================================
*/
	ImageCompressionPass::ImageCompressionPass (RC<Image> src, RC<Image> dst, EPixelFormat dstFormat, StringView dbgName) __NE___ :
		IPass{ dbgName }, _src{RVRef(src)}, _dst{RVRef(dst)}, _dstFormat{dstFormat}
	{
	  #ifdef AE_ENABLE_COMPRESSONATOR
		Unused( Compressonator_GetBCLib() );
	  #endif
	}

/*
=================================================
	destructor
=================================================
*/
	ImageCompressionPass::~ImageCompressionPass ()
	{
		CHECK_Eq( _availableBlocks.ZeroBitCount(), _toUpload->size() );
	}

/*
=================================================
	_Initialize
=================================================
*/
	bool  ImageCompressionPass::_Initialize ()
	{
		if_likely( _srcId and _dstId )
			return true;

		if ( _src->GetStatus() != IResource::EUploadStatus::Completed or
			 _dst->GetStatus() != IResource::EUploadStatus::Completed or
			 _dst->RequireResize() )
			return false;

		const auto		src_view	= _src->GetViewDesc();
		const auto		dst_view	= _dst->GetViewDesc();

		const auto		src_desc	= _src->GetImageDesc();
		const auto		dst_desc	= _dst->GetImageDesc();

		const auto&		src_fmt		= EPixelFormat_GetInfo( src_view.format );
		const auto&		dst_fmt		= EPixelFormat_GetInfo( _dstFormat );

		CHECK_THROW( not src_fmt.IsCompressed() and dst_fmt.IsCompressed() );
		CHECK_THROW( src_fmt.IsColor() and dst_fmt.IsColor() );

		CHECK_THROW( All( src_view.Dimension() == dst_view.Dimension() ));
		CHECK_THROW( src_view.layerCount == dst_view.layerCount );
		CHECK_THROW( src_view.mipmapCount == dst_view.mipmapCount );

		CHECK_THROW( All( IsMultipleOf( uint2{dst_view.Dimension2()}, dst_fmt.TexBlockDim() )));
		CHECK_THROW( dst_view.format == src_view.format or _dstFormat == dst_view.format );

		_srcId				= _src->GetImageId();
		_dstId				= _dst->GetImageId();

		_srcFormat			= src_view.format;
		_srcBitsPerBlock	= src_fmt.bitsPerBlock;
		_dstBitsPerBlock	= dst_fmt.bitsPerBlock;
		_decompress			= (src_view.format == dst_view.format);

		_texelBlockDim		= dst_fmt.TexBlockDim();
		_imageDim			= src_desc.Dimension();
		_imageLayers		= src_desc.arrayLayers.Get();
		_imageMipmaps		= src_desc.mipLevels.Get();
		_tileDim			= AlignUp( uint2{c_TileSize}, _texelBlockDim );

		_memBlockSize		= SizeOf<Block>;
		_memBlockSize		+= AlignUp( ImageUtils::SliceSize( _tileDim, _srcBitsPerBlock, uint2{1} ), Bytes{_BlockAlign} );
		_memBlockSize		+= AlignUp( ImageUtils::SliceSize( _tileDim, _dstBitsPerBlock, _texelBlockDim ), Bytes{_BlockAlign} );

		_toUpload->reserve( _BlockCount );

		_availableBlocks.Store( AvailableBlockBits_t::Bitfield_t{0} );

		if ( _storage.Alloc( _memBlockSize * _BlockCount, Bytes{_BlockAlign}, null ))
			_availableBlocks.SetRange( 0, _BlockCount );

		return true;
	}

/*
=================================================
	Execute
=================================================
*/
	bool  ImageCompressionPass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _Initialize() )
			return true;  // skip

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{GetName()} };

		// read new block
		if ( auto* block = _AllocBlock() )
		{
			const uint3		mip_dim	= Max( _imageDim << _mipOffset, 1u );

			block->offset		= _dimOffset;
			block->dim			= Min( _dimOffset + uint3{_tileDim, 1}, mip_dim ) - _dimOffset;
			block->srcSize		= ImageUtils::SliceSize( uint2{block->dim}, _srcBitsPerBlock, uint2{1} );
			block->dstSize		= ImageUtils::SliceSize( AlignUp( uint2{block->dim}, _texelBlockDim ), _dstBitsPerBlock, _texelBlockDim );
			block->arrayLayer	= 0;
			block->mipmap		= 0;

			_dimOffset.x += block->dim.x;
			if ( _dimOffset.x >= mip_dim.x )
			{
				_dimOffset.x	= 0;
				_dimOffset.y	+= block->dim.y;
			}
			if ( _dimOffset.y >= mip_dim.y )
			{
				_dimOffset.x	= 0;
				_dimOffset.y	= 0;
				_dimOffset.z	+= block->dim.z;
			}
			if ( _dimOffset.z >= mip_dim.z )
			{
				_dimOffset = uint3{0};
				++_layerOffset;
			}
			if ( _layerOffset >= _imageLayers )
			{
				_layerOffset = 0;
				++_mipOffset;
			}
			if ( _mipOffset >= _imageMipmaps )
			{
				_mipOffset = 0;
			}

			ReadbackImageDesc	read;
			read.imageOffset	= block->offset;
			read.imageDim		= block->dim;
			read.arrayLayer		= ImageLayer{ block->arrayLayer };
			read.mipLevel		= MipmapLevel{ block->mipmap };
			read.heapType		= c_HeapType;

			auto	read_res = ctx.ReadbackImage( _srcId, read );

			if_likely( read_res.IsCompleted() )
			{
				read_res.Then( [self = GetRC<ImageCompressionPass>(), block] (const ImageMemView &view)
				{
					CHECK( view.CopyTo( OUT block->SrcData(), block->srcSize ));
					block->offset		= view.Offset();
					block->dim			= view.Dimension();
					block->arrayLayer	= 0;
					block->mipmap		= 0;

					Scheduler().Run<CompressBlockTask>( Tuple{ RVRef(self), block });
				});
			}
			else
			{
				_FreeBlock( block );
			}
		}

		// upload compressed block
		if ( auto* block = _GetBlockToUpload() )
		{
			UploadImageDesc		upload;
			upload.imageOffset	= block->offset;
			upload.imageDim		= _decompress ? block->dim : AlignUp( block->dim, uint3{_texelBlockDim,1} );
			upload.arrayLayer	= ImageLayer{ block->arrayLayer };
			upload.mipLevel		= MipmapLevel{ block->mipmap };
			upload.heapType		= c_HeapType;

			ImageMemView	mem;
			ctx.UploadImage( _dstId, upload, OUT mem );

			if ( All( mem.Dimension() == upload.imageDim ))
			{
				CHECK( mem.CopyFrom( _decompress ? block->SrcImage(_srcFormat) : block->DstImage(_dstFormat) ));
				_FreeBlock( block );
			}
			else
			{
				// try again later
				_toUpload->push_front( block );
			}
		}

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}

/*
=================================================
	_AllocBlock
=================================================
*/
	ImageCompressionPass::Block*  ImageCompressionPass::_AllocBlock ()
	{
		int		idx = _availableBlocks.ExtractBitIndex();	// 0 -> 1
		if_likely( idx >= 0 )
		{
			MemoryBarrier( EMemoryOrder::Acquire );

			auto*	res = Cast<Block>( _storage.Data().get() + _memBlockSize * idx );
			ZeroMem( OUT res, _memBlockSize );

			return res;
		}
		return null;
	}

/*
=================================================
	_FreeBlock
=================================================
*/
	void  ImageCompressionPass::_FreeBlock (Block* block)
	{
		usize	idx = usize{ (Bytes{block} - Bytes{_storage.Data()}) / _memBlockSize };

		_availableBlocks.Set( idx, EMemoryOrder::Release );	// 1 -> 0
	}

/*
=================================================
	_GetBlockToUpload
=================================================
*/
	ImageCompressionPass::Block*  ImageCompressionPass::_GetBlockToUpload ()
	{
		auto	q = _toUpload.WriteLock();

		if ( q->empty() )
			return null;

		return q->ExtractFront();
	}

/*
=================================================
	GetResourcesToResize
=================================================
*/
	void  ImageCompressionPass::GetResourcesToResize (INOUT Array<RC<IResource>> &resources) __NE___
	{
		if ( _src->RequireResize() )
			resources.push_back( _src );

		if ( _dst->RequireResize() )
			resources.push_back( _dst );
	}


} // AE::ResEditor
