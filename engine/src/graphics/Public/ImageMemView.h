// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Color.h"
#include "graphics/Public/BufferMemView.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ImageUtils.h"

namespace AE::Graphics
{

	//
	// Image Memory View
	//

	struct ImageMemView
	{
	// types
	public:
		using Pixel_t	= BufferMemView::ConstData;
		using Row_t		= BufferMemView::Data;
		using CRow_t	= BufferMemView::ConstData;
		using Slice_t	= BufferMemView;

		static constexpr uint	Count = BufferMemView::Count;


	// variables
	protected:
		BufferMemView		_content;
		Bytes32u			_rowPitch;
		Bytes32u			_slicePitch;
		ushort3				_offset;
		ushort3				_dimension;
		ushort				_bitsPerBlock	= 0;
		ubyte2				_texBlockSize;
		EPixelFormat		_format			= Default;
		EImageAspect		_aspect			= Default;


	// methods
	public:
		ImageMemView () {}
		ImageMemView (const BufferMemView& content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect);
		ImageMemView (void *content, Bytes contentSize, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect);

		template <typename T>
		ImageMemView (Array<T> &content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) :
			ImageMemView{ BufferMemView{content}, off, dim, rowPitch, slicePitch, format, aspect }
		{}

		ImageMemView (const ImageMemView &) = default;
		ImageMemView (ImageMemView &&) = default;

		ImageMemView&  operator = (const ImageMemView &) = default;
		ImageMemView&  operator = (ImageMemView &&) = default;

		ND_ uint3			Offset ()			const	{ return uint3{ _offset }; }
		ND_ uint3			Dimension ()		const	{ return uint3{ _dimension }; }
		ND_ Bytes			RowPitch ()			const	{ return Bytes(_rowPitch); }
		ND_ Bytes			SlicePitch ()		const	{ return Bytes(_slicePitch); }
		ND_ uint			BitsPerBlock ()		const	{ return _bitsPerBlock; }
		ND_ Bytes			BytesPerBlock ()	const	{ return ImageUtils::BytesPerBlock( _bitsPerBlock, TexelBlockSize() ); }
		ND_ Bytes			MinRowSize ()		const	{ return ImageUtils::RowSize( _dimension.x, _bitsPerBlock, TexelBlockSize() ); };
		ND_ Bytes			MinSliceSize ()		const	{ return ImageUtils::SliceSize( _dimension.y, RowPitch(), TexelBlockSize() ); }
		ND_ Bytes			ImageSize ()		const	{ return SlicePitch() * _dimension.z; }
		ND_ Bytes			ContentSize ()		const	{ return _content.DataSize(); }
		ND_ EPixelFormat	Format ()			const	{ return _format; }
		ND_ auto			Parts ()					{ return _content.Parts(); }
		ND_ auto			Parts ()			const	{ return _content.Parts(); }
		ND_ bool			Empty ()			const	{ return _content.Empty(); }
		ND_ uint2			TexelBlockSize ()	const	{ return uint2{_texBlockSize}; }
		ND_ uint2			TexelBlocks ()		const	{ return uint2{_dimension.x, _dimension.y} / TexelBlockSize(); }
		

		bool  PushBack (void *ptr, Bytes size)
		{
			return _content.PushBack( ptr, size );
		}

		ND_ Row_t   GetRow (uint y, uint z = 0);
		ND_ CRow_t  GetRow (uint y, uint z = 0) const;

		ND_ Slice_t  GetSlice (uint z);
		ND_ Slice_t  GetSlice (uint z) const;
		
		ND_ Pixel_t  GetPixel (const uint3 &point) const;
		
		ND_ bool  operator == (const ImageMemView &rhs) const { return Compare( rhs ) == 0_b; }

		
		// returns number of copied bytes
			bool  Copy (const ImageMemView &src, OUT Bytes &dataSize);
			bool  Copy (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim, OUT Bytes &dataSize);
		ND_ bool  Copy (const ImageMemView &src);
		ND_ bool  Copy (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim);

		// returns how much bytes are different
		ND_ Bytes  Compare (const ImageMemView &rhs) const;
		ND_ Bytes  Compare (const uint3 &lhsOffset, const uint3 &rhsOffset, const ImageMemView &rhsImage, const uint3 &dim) const;
	};

	

	//
	// Read/Write Image Memory View
	//
	struct RWImageMemView : public ImageMemView
	{
	// types
	public:
		using PixStorage_t		= StaticArray<uint,4>;

		using LoadPixelFn_t		= void (*) (const CRow_t &, uint x, OUT PixStorage_t &);
		using LoadRGBA32fFn_t	= void (*) (const CRow_t &, uint x, OUT RGBA32f &);
		using LoadRGBA32uFn_t	= void (*) (const CRow_t &, uint x, OUT RGBA32u &);
		using LoadRGBA32iFn_t	= void (*) (const CRow_t &, uint x, OUT RGBA32i &);
		using LoadDSFn_t		= void (*) (const CRow_t &, uint x, OUT DepthStencil &);
		
		using StorePixelFn_t	= void (*) (OUT Row_t, uint x, const PixStorage_t &);
		using StoreRGBA32fFn_t	= void (*) (OUT Row_t, uint x, const RGBA32f &);
		using StoreRGBA32uFn_t	= void (*) (OUT Row_t, uint x, const RGBA32u &);
		using StoreRGBA32iFn_t	= void (*) (OUT Row_t, uint x, const RGBA32i &);
		using StoreDSFn_t		= void (*) (OUT Row_t, uint x, const DepthStencil &);


	// variables
	private:
		LoadRGBA32fFn_t		_loadF4		= null;
		LoadRGBA32uFn_t		_loadU4		= null;
		LoadRGBA32iFn_t		_loadI4		= null;
		LoadDSFn_t			_loadDS		= null;

		StoreRGBA32fFn_t	_storeF4	= null;
		StoreRGBA32uFn_t	_storeU4	= null;
		StoreRGBA32iFn_t	_storeI4	= null;
		StoreDSFn_t			_storeDS	= null;

		
	// methods
	public:
		RWImageMemView ();
		RWImageMemView (const ImageMemView& other);
		RWImageMemView (const BufferMemView& content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect);

		void  Load (const uint3 &point, OUT RGBA32f &col) const
		{
			ASSERT( _loadF4 );
			_loadF4( GetRow( point.y, point.z ), point.x, OUT col );
		}

		void  Load (const uint3 &point, OUT RGBA32u &col) const
		{
			ASSERT( _loadU4 );
			return _loadU4( GetRow( point.y, point.z ), point.x, OUT col );
		}

		void  Load (const uint3 &point, OUT RGBA32i &col) const
		{
			ASSERT( _loadI4 );
			return _loadI4( GetRow( point.y, point.z ), point.x, OUT col );
		}
		
		void  Load (const uint3 &point, OUT DepthStencil &ds) const
		{
			ASSERT( _loadDS );
			return _loadDS( GetRow( point.y, point.z ), point.x, OUT ds );
		}
		

		void  Store (const uint3 &point, const RGBA32f &col)
		{
			ASSERT( _storeF4 );
			_storeF4( GetRow( point.y, point.z ), point.x, col );
		}

		void  Store (const uint3 &point, const RGBA32u &col)
		{
			ASSERT( _storeU4 );
			return _storeU4( GetRow( point.y, point.z ), point.x, col );
		}

		void  Store (const uint3 &point, const RGBA32i &col)
		{
			ASSERT( _storeI4 );
			return _storeI4( GetRow( point.y, point.z ), point.x, col );
		}

		void  Store (const uint3 &point, const DepthStencil &ds)
		{
			ASSERT( _storeDS );
			return _storeDS( GetRow( point.y, point.z ), point.x, ds );
		}


		ND_ bool  Blit (const RWImageMemView &src, OUT Bytes &readn, OUT Bytes &written);
		ND_ bool  Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage, const uint3 &dim, OUT Bytes &readn, OUT Bytes &written);
		ND_ bool  Blit (const RWImageMemView &src);
		ND_ bool  Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage, const uint3 &dim);


		ND_ bool  Fill (const RGBA32f &col)		{ return Fill( col, uint3{}, Dimension() ); }
		ND_ bool  Fill (const RGBA32u &col)		{ return Fill( col, uint3{}, Dimension() ); }
		ND_ bool  Fill (const RGBA32i &col)		{ return Fill( col, uint3{}, Dimension() ); }

		ND_ bool  Fill (const RGBA32f &col, const uint3 &offset, const uint3 &dim);
		ND_ bool  Fill (const RGBA32u &col, const uint3 &offset, const uint3 &dim);
		ND_ bool  Fill (const RGBA32i &col, const uint3 &offset, const uint3 &dim);

	private:
		ND_ bool  _Fill (const CRow_t &data, const uint3 &offset, const uint3 &dim);

	};

} // AE::Graphics
