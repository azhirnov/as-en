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
		ushort				_bitsPerPixel	= 0;
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
		ND_ uint			BitsPerPixel ()		const	{ return _bitsPerPixel; }
		ND_ Bytes			BytesPerBlock ()	const	{ return ImageUtils::BytesPerBlock( _bitsPerPixel, TexelBlockSize() ); }
		ND_ Bytes			RowSize ()			const	{ return ImageUtils::RowSize( _dimension.x, _bitsPerPixel, TexelBlockSize() ); };
		ND_ Bytes			SliceSize ()		const	{ return ImageUtils::SliceSize( _dimension.y, RowPitch(), TexelBlockSize() ); }
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

		ND_ Row_t  GetRow (uint y, uint z = 0)
		{
			ASSERT( y < Dimension().y );
			ASSERT( z < Dimension().z );

			const Bytes	row_size	= RowSize();
			const Bytes	row_offset	= SlicePitch() * z + RowPitch() * y;
			Bytes		cur_offset;

			for (auto& part : _content.Parts())
			{
				if ( IsIntersects( cur_offset, cur_offset + part.size, row_offset, row_offset + row_size ))
				{
					ASSERT( (cur_offset + part.size) >= (row_offset + row_size) );
					return Row_t{ {part.ptr + (row_offset - cur_offset)}, row_size };
				}
				cur_offset += part.size;
			}

			ASSERT(false);
			return Row_t{};
		}
		
		ND_ CRow_t  GetRow (uint y, uint z = 0) const
		{
			auto	row = const_cast<ImageMemView*>(this)->GetRow( y, z );
			return CRow_t{ {row.ptr}, row.size };
		}

		
		ND_ Slice_t  GetSlice (uint z)
		{
			ASSERT( z < Dimension().z );
			
			const Bytes	slice_size		= SliceSize();
			Bytes		slice_offset	= SlicePitch() * z;
			Bytes		cur_offset;
			Slice_t		slice;
			
			for (auto& part : _content.Parts())
			{
				if ( IsIntersects( cur_offset, cur_offset + part.size, slice_offset, slice_offset + slice_size ))
				{
					slice.PushBack( part.ptr + (cur_offset - slice_offset),
									Min( part.size + cur_offset, slice_offset + slice_size ) - cur_offset );
					slice_offset = cur_offset + part.size;
				}
				cur_offset += part.size;
			}
			ASSERT( slice_size == slice.DataSize() );
			return slice;
		}
		
		ND_ Slice_t  GetSlice (uint z) const
		{
			return const_cast<ImageMemView*>(this)->GetSlice( z );
		}
		

		ND_ Pixel_t  GetPixel (const uint3 &point) const
		{
			ASSERT(All( point < Dimension() ));

			CRow_t	row		= GetRow( point.y, point.z );
			Pixel_t	pixel	{ {row.ptr + Bytes{(_bitsPerPixel * point.x) / 8}}, Bytes{_bitsPerPixel} / 8 };

			return pixel;
		}
		
		ND_ bool  operator == (const ImageMemView &rhs) const { return Compare( rhs ) == 0_b; }

		
		// returns number of copied bytes
		ND_ Bytes  Copy (const ImageMemView &src);
		ND_ Bytes  Copy (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim);

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
		using LoadRGBA32fFun_t	= void (*) (const CRow_t &, uint x, OUT RGBA32f &);
		using LoadRGBA32uFun_t	= void (*) (const CRow_t &, uint x, OUT RGBA32u &);
		using LoadRGBA32iFun_t	= void (*) (const CRow_t &, uint x, OUT RGBA32i &);
		using StoreRGBA32fFun_t	= void (*) (OUT Row_t, uint x, const RGBA32f &);
		using StoreRGBA32uFun_t	= void (*) (OUT Row_t, uint x, const RGBA32u &);
		using StoreRGBA32iFun_t	= void (*) (OUT Row_t, uint x, const RGBA32i &);


	// variables
	private:
		LoadRGBA32fFun_t	_loadF4		= null;
		LoadRGBA32uFun_t	_loadU4		= null;
		LoadRGBA32iFun_t	_loadI4		= null;
		
		StoreRGBA32fFun_t	_storeF4	= null;
		StoreRGBA32uFun_t	_storeU4	= null;
		StoreRGBA32iFun_t	_storeI4	= null;

		
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
	};

}	// AE::Graphics
