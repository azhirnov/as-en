// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "graphics_rhi/Public/ImageMemView.h"
#include "graphics_rhi/Private/EnumUtils.h"
#include "graphics_rhi/Public/SamplerDesc.h"

namespace AE::Graphics
{
namespace
{
	union FloatBits
	{
		struct
		{
			uint	m	: 23;	// mantissa bits
			uint	e	: 8;	// exponent bits
			uint	s	: 1;	// sign bit
		}		bits;
		float	f;

		__Cx__ FloatBits ()			__NE___	: f{0.f} {}

		NdCx__ operator float ()	C_NE___	{ return f; }
	};

	// warning: bit order may be compiler defined
	struct R11G11B10Bits
	{
		uint r_m : 6;   // bits  0..5
		uint r_e : 5;   // bits  6..10
		uint g_m : 6;   // bits 11..16
		uint g_e : 5;   // bits 17..21
		uint b_m : 5;   // bits 22..26
		uint b_e : 5;   // bits 27..31
	};
	StaticAssert( CT_SizeOfInBits<R11G11B10Bits> == (11+11+10) );


/*
=================================================
	ScaleUNorm
=================================================
*/
	template <uint Bits>
	forceinline static float  ScaleUNorm (uint value) __NE___
	{
		StaticAssert( Bits <= 32 );

		FloatBits	f;
		f.bits.e	= 127 + Bits - 1;
		f.bits.m	= 0x7F0000;

		return float(value) / f;
	}

/*
=================================================
	ScaleSNorm
=================================================
*/
	template <uint Bits>
	forceinline static float  ScaleSNorm (uint value) __NE___
	{
		StaticAssert( Bits <= 32 );

		if constexpr( Bits > 0 )
		{
			constexpr uint	max_neg	= 1u << (Bits - 1);
			constexpr uint	mask	= (~0u >> (32 - Bits + 1));

			FloatBits	f;
			f.bits.m	= (1u << 23) - 1;
			f.bits.e	= 127 + Bits - 2;

			if ( value < max_neg )
			{
				// positive
				return float(value) / f;
			}
			else
			{
				// negative
				f.bits.s = 1;
				return float(mask - (value - max_neg)) / f;
			}
		}
		else
			return 0.f;
	}

/*
=================================================
	ReadUIntScalar
=================================================
*/
	template <uint Bits, uint OffsetBits>
	forceinline uint  ReadUIntScalar (const StaticArray<uint,4> &data) __NE___
	{
		StaticAssert( Bits <= 32 );
		StaticAssert( Bits + (OffsetBits & 31) <= 32 );

		if constexpr( Bits == 0 )
		{
			Unused( data );
			return 0;
		}
		else
		{
			constexpr uint	mask	= (~0u >> (32 - Bits));
			constexpr uint	offset	= (OffsetBits % 32);
			constexpr uint	index	= (OffsetBits / 32);

			return (data[index] >> offset) & mask;
		}
	}

/*
=================================================
	ReadIntScalar
=================================================
*/
	template <uint Bits, uint OffsetBits>
	forceinline static int  ReadIntScalar (const StaticArray<uint,4> &data) __NE___
	{
		const uint	value = ReadUIntScalar< Bits, OffsetBits >( data );

		if constexpr( Bits == 0 )
			return 0;
		else
		if constexpr( Bits == 32 )
			return int(value);
		else
			return (value >> (Bits-1)) ? -int(value) : int(value);	// TODO: check
	}

/*
=================================================
	ReadInt
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  ReadInt (const BufferMemView::ConstData &row, uint x, OUT RGBA32i &result) __NE___
	{
		StaticArray< uint, 4 >	bits	= {};
		constexpr uint			px_size = (R+G+B+A+7)/8;

		NonNull( row.ptr );
		MemCopy( OUT bits.data(), row.ptr + Bytes{x * px_size}, Bytes{px_size} );

		result.r = ReadIntScalar< R, 0 >( bits );
		result.g = ReadIntScalar< G, R >( bits );
		result.b = ReadIntScalar< B, R+G >( bits );
		result.a = ReadIntScalar< A, R+G+B >( bits );
	}

/*
=================================================
	ReadUInt
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  ReadUInt (const BufferMemView::ConstData &row, uint x, OUT RGBA32u &result) __NE___
	{
		StaticArray< uint, 4 >	bits	= {};
		constexpr uint			px_size = (R+G+B+A+7)/8;

		NonNull( row.ptr );
		MemCopy( OUT bits.data(), row.ptr + Bytes{x * px_size}, Bytes{px_size} );

		result.r = ReadUIntScalar< R, 0 >( bits );
		result.g = ReadUIntScalar< G, R >( bits );
		result.b = ReadUIntScalar< B, R+G >( bits );
		result.a = ReadUIntScalar< A, R+G+B >( bits );
	}

/*
=================================================
	ReadUNorm
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  ReadUNorm (const BufferMemView::ConstData &row, uint x, OUT RGBA32f &result) __NE___
	{
		RGBA32u		c;
		ReadUInt<R,G,B,A>( row, x, OUT c );

		result.r = ScaleUNorm<R>( c.r );
		result.g = ScaleUNorm<G>( c.g );
		result.b = ScaleUNorm<B>( c.b );
		result.a = ScaleUNorm<A>( c.a );
	}

/*
=================================================
	ReadSNorm
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  ReadSNorm (const BufferMemView::ConstData &row, uint x, OUT RGBA32f &result) __NE___
	{
		RGBA32u		c;
		ReadUInt<R,G,B,A>( row, x, OUT c );

		result.r = ScaleSNorm<R>( c.r );
		result.g = ScaleSNorm<G>( c.g );
		result.b = ScaleSNorm<B>( c.b );
		result.a = ScaleSNorm<A>( c.a );
	}

/*
=================================================
	ReadFloat
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  ReadFloat (const BufferMemView::ConstData &row, uint x, OUT RGBA32f &result) __NE___
	{
		constexpr uint	px_size = (R+G+B+A+7)/8;
		NonNull( row.ptr );

		if constexpr( R == 16 )
		{
			StaticArray< half, 4 >	src = {};
			MemCopy( OUT src.data(), row.ptr + Bytes{x * px_size}, Bytes{px_size} );

			for (uint i = 0; i < 4; ++i)
			{
				result[i] = float{src[i]};
			}
		}
		else
		if constexpr( R == 32 )
		{
			result = {};
			MemCopy( OUT result.data(), row.ptr + Bytes{x * px_size}, Bytes{px_size} );
		}
		else
		{
			DBG_WARNING( "not supported" );
		}
	}

/*
=================================================
	ReadFloat_11_11_10
=================================================
*/
	static void  ReadFloat_11_11_10 (const BufferMemView::ConstData &row, uint x, OUT RGBA32f &result) __NE___
	{
		R11G11B10Bits	bits = {};

		NonNull( row.ptr );
		MemCopy( OUT &bits, row.ptr + Bytes{x * sizeof(R11G11B10Bits)}, Sizeof(bits) );

		FloatBits	f;

		f.bits.m = bits.r_m << (23 - 6);
		f.bits.e = bits.r_e + (127 - 15);
		result.r = f;

		f.bits.m = bits.g_m << (23 - 6);
		f.bits.e = bits.g_e + (127 - 15);
		result.g = f;

		f.bits.m = bits.b_m << (23 - 5);
		f.bits.e = bits.b_e + (127 - 15);
		result.b = f;

		result.a = 1.0f;
	}

/*
=================================================
	ReadBlock*
=================================================
*/
	static void  ReadBlock64 (const BufferMemView::ConstData &row, uint x, OUT RGBA32u &result) __NE___
	{
		NonNull( row.ptr );
		MemCopy( OUT &result.r, row.ptr + 8_b * x, 8_b );
	}

	static void  ReadBlock128 (const BufferMemView::ConstData &row, uint x, OUT RGBA32u &result) __NE___
	{
		NonNull( row.ptr );
		MemCopy( OUT &result.r, row.ptr + 16_b * x, 16_b );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	UNormToUInt
=================================================
*/
	template <uint Bits>
	ND_ forceinline static uint  UNormToUInt (float value) __NE___
	{
		StaticAssert( Bits <= 32 );
		value = Clamp( value, 0.0f, 1.0f );

		FloatBits	f;
		f.bits.e	= 127 + Bits - 1;
		f.bits.m	= 0x7F0000;

		return uint(Max( 0.f, value ) * f);
	}

/*
=================================================
	SNormToUInt
=================================================
*/
	template <uint Bits>
	ND_ forceinline static uint  SNormToUInt (float value) __NE___
	{
		if constexpr( Bits > 0 )
		{
			StaticAssert( Bits <= 32 );
			value = Clamp( value, -1.0f, +1.0f );

			FloatBits	f;
			f.bits.e	= 127 + Bits - 2;
			f.bits.m	= (1u << 23) - 1;

			if ( value >= 0.0f )
				return uint(value * f);
			else
			{
				constexpr uint	mask = (~0u >> (32 - Bits));

				return mask - uint(-value * f);
			}
		}
		else
			return 0;
	}

/*
=================================================
	WriteUIntScalar
=================================================
*/
	template <uint Bits, uint OffsetBits>
	forceinline static void  WriteUIntScalar (uint col, OUT StaticArray<uint,4> &data) __NE___
	{
		StaticAssert( Bits <= 32 );
		StaticAssert( Bits + (OffsetBits & 31) <= 32 );

		if constexpr( Bits == 0 )
		{
			Unused( col, data );
		}
		else
		{
			constexpr uint	mask	= (~0u >> (32 - Bits));
			constexpr uint	offset	= (OffsetBits % 32);
			constexpr uint	index	= (OffsetBits / 32);

			data[index] |= (col & mask) << offset;
		}
	}

/*
=================================================
	WriteIntScalar
=================================================
*/
	template <uint Bits, uint OffsetBits>
	forceinline static void  WriteIntScalar (int col, OUT StaticArray<uint,4> &data) __NE___
	{
		uint	ucol;

		if constexpr( Bits == 0 )
			ucol = 0;
		else
		if constexpr( Bits == 32 )
			ucol = uint(col);
		else
			ucol = (col >> (Bits-1)) ? uint(-col) : uint(col);	// TODO: check

		WriteUIntScalar< Bits, OffsetBits >( ucol, OUT data );
	}

/*
=================================================
	WriteUInt
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  WriteUInt (BufferMemView::Data row, uint x, const RGBA32u &result) __NE___
	{
		StaticArray< uint, 4 >	bits	= {};
		constexpr uint			px_size = (R+G+B+A+7)/8;

		WriteUIntScalar< R, 0 >( result.r, OUT bits );
		WriteUIntScalar< G, R >( result.g, OUT bits );
		WriteUIntScalar< B, R+G >( result.b, OUT bits );
		WriteUIntScalar< A, R+G+B >( result.a, OUT bits );

		NonNull( row.ptr );
		MemCopy( OUT row.ptr + Bytes{x * px_size}, bits.data(), Bytes{px_size} );
	}

/*
=================================================
	WriteInt
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  WriteInt (BufferMemView::Data row, uint x, const RGBA32i &result) __NE___
	{
		StaticArray< uint, 4 >	bits	= {};
		constexpr uint			px_size = (R+G+B+A+7)/8;

		WriteIntScalar< R, 0 >( result.r, OUT bits );
		WriteIntScalar< G, R >( result.g, OUT bits );
		WriteIntScalar< B, R+G >( result.b, OUT bits );
		WriteIntScalar< A, R+G+B >( result.a, OUT bits );

		NonNull( row.ptr );
		MemCopy( OUT row.ptr + Bytes{x * px_size}, bits.data(), Bytes{px_size} );
	}

/*
=================================================
	WriteUNorm
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  WriteUNorm (BufferMemView::Data row, uint x, const RGBA32f &result) __NE___
	{
		RGBA32u		c;
		c.r = UNormToUInt<R>( result.r );
		c.g = UNormToUInt<G>( result.g );
		c.b = UNormToUInt<B>( result.b );
		c.a = UNormToUInt<A>( result.a );

		WriteUInt<R,G,B,A>( row, x, c );
	}

/*
=================================================
	WriteSNorm
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  WriteSNorm (BufferMemView::Data row, uint x, const RGBA32f &result) __NE___
	{
		RGBA32u		c;
		c.r = SNormToUInt<R>( result.r );
		c.g = SNormToUInt<G>( result.g );
		c.b = SNormToUInt<B>( result.b );
		c.a = SNormToUInt<A>( result.a );

		WriteUInt<R,G,B,A>( row, x, c );
	}

/*
=================================================
	WriteFloat
=================================================
*/
	template <uint R, uint G, uint B, uint A>
	static void  WriteFloat (BufferMemView::Data row, uint x, const RGBA32f &result) __NE___
	{
		constexpr uint	px_size = (R+G+B+A+7)/8;
		NonNull( row.ptr );

		if constexpr( R == 16 )
		{
			StaticAssert( (G == 16 or G == 0) and (B == 16 or B == 0) and (A == 16 or A == 0) );

			StaticArray< half, 4 >	dst = {};

			for (usize i = 0; i < dst.size(); ++i) {
				dst[i] = half{ result[i] };
			}
			MemCopy( OUT row.ptr + Bytes{x * px_size}, dst.data(), Bytes{px_size} );
		}
		else
		if constexpr( R == 32 )
		{
			StaticAssert( (G == 32 or G == 0) and (B == 32 or B == 0) and (A == 32 or A == 0) );

			MemCopy( OUT row.ptr + Bytes{x * px_size}, result.data(), Bytes{px_size} );
		}
		else
		{
			DBG_WARNING( "not supported" );
		}
	}

/*
=================================================
	WriteFloat_11_11_10
=================================================
*/
	static void  WriteFloat_11_11_10 (BufferMemView::Data row, uint x, const RGBA32f &result) __NE___
	{
		R11G11B10Bits	bits;
		FloatBits		f;

		f.f = result.r;
		bits.r_m = f.bits.m >> (23 - 6);
		bits.r_e = f.bits.e - (127 - 15);

		f.f = result.g;
		bits.g_m = f.bits.m >> (23 - 6);
		bits.g_e = f.bits.e - (127 - 15);

		f.f = result.b;
		bits.b_m = f.bits.m >> (23 - 5);
		bits.b_e = f.bits.e - (127 - 15);

		NonNull( row.ptr );
		MemCopy( OUT row.ptr + Bytes{x * sizeof(R11G11B10Bits)}, &bits, Sizeof(bits) );
	}

/*
=================================================
	Filter
=================================================
*/
	static void  NearestFilter (const RWImageMemView::PixStorage_t srcPixels[4], OUT RWImageMemView::PixStorage_t &dstPix) __NE___
	{
		std::memcpy( dstPix.data(), srcPixels[0].data(), sizeof(dstPix) );
	}

	static void  LinearFilterF4 (const RGBA32f srcPixels[4], OUT RGBA32f &dstPix) __NE___
	{
		dstPix = (srcPixels[0] + srcPixels[1] + srcPixels[2] + srcPixels[3]) * 0.25f;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ImageMemView::ImageMemView (const BufferMemView& content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___ :
		_content{ content },
		_rowPitch{ rowPitch },		_slicePitch{ slicePitch },
		_offset{ CheckCast{ off }},	_dimension{ CheckCast{ Max( dim, 1u )}},
		_format{ format },			_aspect{ aspect }
	{
		ASSERT( RowPitch() == rowPitch );
		ASSERT( SlicePitch() == slicePitch );
		ASSERT( IsSingleBitSet( _aspect ));

		const auto&	fmt_info = EPixelFormat_GetInfo( format );

		_texBlockDim = fmt_info.blockDim;
		ASSERT( All( _texBlockDim > ubyte2{0} ));

		if ( _aspect == EImageAspect::Stencil )
		{
			ASSERT( fmt_info.bitsPerBlock2 != 0 );
			_bitsPerBlock = CheckCast{ fmt_info.bitsPerBlock2 };
		}
		else
			_bitsPerBlock = CheckCast{ fmt_info.bitsPerBlock };

		if ( _rowPitch == 0 )
			_rowPitch = CheckCast{ MinRowSize() };

		if ( _slicePitch == 0 )
			_slicePitch = CheckCast{ MinSliceSize() };

		ASSERT( BitsPerBlock() > 0_b );
		ASSERT_GE( RowPitch(), MinRowSize() );
		ASSERT_GE( SlicePitch(), MinSliceSize() );

		if ( _dimension.z > 1 ){
			ASSERT_Eq( Image3DSize(), ContentSize() );
		}else{
			ASSERT_Eq( Image2DSize(), ContentSize() );
		}
	}

	ImageMemView::ImageMemView (void* content, Bytes contentSize, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___ :
		ImageMemView{ BufferMemView{content, contentSize}, off, dim, rowPitch, slicePitch, format, aspect }
	{}

/*
=================================================
	GetRow
=================================================
*/
	ImageMemView::Row_t  ImageMemView::GetRow (uint y, uint z) __NE___
	{
		ASSERT( y < Dimension().y );
		ASSERT( z < Dimension().z );

		const Bytes	row_size	= MinRowSize();
		const Bytes	row_offset	= SlicePitch() * z + RowPitch() * y;
		Bytes		cur_offset;

		for (auto& part : _content.Parts())
		{
			if ( IsIntersects( cur_offset, cur_offset + part.size, row_offset, row_offset + row_size ))
			{
				ASSERT( (cur_offset + part.size) >= (row_offset + row_size) );
				NonNull( part.ptr );

				return Row_t{ (part.ptr + (row_offset - cur_offset)), row_size };
			}
			cur_offset += part.size;
		}

		ASSERT(false);
		return Row_t{};
	}

	ImageMemView::CRow_t  ImageMemView::GetRow (uint y, uint z) C_NE___
	{
		auto	row = const_cast<ImageMemView*>(this)->GetRow( y, z );
		NonNull( row.ptr );
		return CRow_t{ row.ptr, row.size };
	}

/*
=================================================
	GetSlice
=================================================
*/
	ImageMemView::Slice_t  ImageMemView::GetSlice (uint z) __NE___
	{
		ASSERT( z < Dimension().z );

		const Bytes	slice_size		= MinSliceSize();
		Bytes		slice_offset	= SlicePitch() * z;
		Bytes		cur_offset;
		Slice_t		slice;

		for (auto& part : _content.Parts())
		{
			if ( IsIntersects( cur_offset, cur_offset + part.size, slice_offset, slice_offset + slice_size ))
			{
				NonNull( part.ptr );
				slice.PushBack( part.ptr + (cur_offset - slice_offset),
								Min( part.size + cur_offset, slice_offset + slice_size ) - cur_offset );
				slice_offset = cur_offset + part.size;
			}
			cur_offset += part.size;
		}
		ASSERT( slice_size == slice.DataSize() );
		return slice;
	}

	ImageMemView::Slice_t  ImageMemView::GetSlice (uint z) C_NE___
	{
		return const_cast<ImageMemView*>(this)->GetSlice( z );
	}

/*
=================================================
	GetPixel
=================================================
*/
	ImageMemView::Pixel_t  ImageMemView::GetPixel (const uint3 &point) C_NE___
	{
		ASSERT( All( point < Dimension() ));

		CRow_t	row	= GetRow( point.y, point.z );
		NonNull( row.ptr );

		Pixel_t	pixel{ (row.ptr + Bytes{(_bitsPerBlock * point.x) / 8}), Bytes{_bitsPerBlock} / 8 };	// TODO: compressed format
		return pixel;
	}

/*
=================================================
	CopyFrom
=================================================
*/
	bool  ImageMemView::CopyFrom (const ImageMemView &src) __NE___
	{
		if_unlikely( Any( Dimension() != src.Dimension() ))
			return false;

		Bytes	data_size;
		return CopyFrom( uint3{}, uint3{}, src, Dimension(), OUT data_size );
	}

	bool  ImageMemView::CopyFrom (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim) __NE___
	{
		Bytes	data_size;
		return CopyFrom( dstOffset, srcOffset, srcImage, dim, OUT data_size );
	}

	bool  ImageMemView::CopyFrom (const uint3 &dstOffset, const uint3 &srcOffset, const uint3 &dstDim, const uint3 &srcDim, const ImageMemView &srcImage) __NE___
	{
		Bytes	data_size;
		return _CopyImage( dstOffset, srcOffset, dstDim, srcDim, *this, srcImage, OUT data_size );
	}

	bool  ImageMemView::CopyFrom (const ImageMemView &src, OUT Bytes &written) __NE___
	{
		if_unlikely( Any( Dimension() != src.Dimension() ))
			return false;

		return CopyFrom( uint3{}, uint3{}, src, Dimension(), OUT written );
	}

	bool  ImageMemView::CopyFrom (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim, OUT Bytes &written) __NE___
	{
		if ( Any( this->TexBlockDim() != srcImage.TexBlockDim() ))
			return false;

		return _CopyImage( dstOffset, srcOffset, dim, dim, *this, srcImage, OUT written );
	}

	bool  ImageMemView::CopyFrom (const uint3 &dstOffset, const uint3 &srcOffset, const uint3 &dstDim, const uint3 &srcDim, const ImageMemView &srcImage, OUT Bytes &written) __NE___
	{
		return _CopyImage( dstOffset, srcOffset, dstDim, srcDim, *this, srcImage, OUT written );
	}

/*
=================================================
	_CopyImage
=================================================
*/
	bool  ImageMemView::_CopyImage (const uint3 &dstOffset, const uint3 &srcOffset, const uint3 &dstDim, const uint3 &srcDim,
									ImageMemView &dstImage, const ImageMemView &srcImage, OUT Bytes &written) __NE___
	{
		written = 0_b;

		if_unlikely( dstImage.Empty() or srcImage.Empty() )
			return false;

		ASSERT( All( dstOffset + dstDim <= dstImage.Dimension() ));
		ASSERT( All( srcOffset + srcDim <= srcImage.Dimension() ));

		ASSERT( All( IsMultipleOf( uint2{dstOffset}, dstImage.TexBlockDim() )));
		ASSERT( All( IsMultipleOf( uint2{srcOffset}, srcImage.TexBlockDim() )));

		ASSERT( All( IsMultipleOf( uint2{dstOffset + dstDim}, dstImage.TexBlockDim() )));
		ASSERT( All( IsMultipleOf( uint2{srcOffset + srcDim}, srcImage.TexBlockDim() )));

		ASSERT( dstImage._bitsPerBlock == srcImage._bitsPerBlock );

		if_unlikely( not All( dstOffset + dstDim <= dstImage.Dimension() )	or
					 not All( srcOffset + srcDim <= srcImage.Dimension() )	or
					 dstImage._bitsPerBlock != srcImage._bitsPerBlock		or
					 dstImage._aspect != srcImage._aspect )
		{
			return false;
		}

		const Bytes		row_size		= ImageUtils::RowSize( dstDim.x, dstImage._bitsPerBlock, dstImage.TexBlockDim() );
		const Bytes		src_row_size	= ImageUtils::RowSize( srcDim.x, srcImage._bitsPerBlock, srcImage.TexBlockDim() );
		ASSERT( row_size == src_row_size );

		const Bytes		dst_row_pitch	= dstImage.RowPitch();
		const Bytes		src_row_pitch	= srcImage.RowPitch();
		const Bytes		dst_slice_pitch	= dstImage.SlicePitch();
		const Bytes		src_slice_pitch	= srcImage.SlicePitch();

		const uint		block_count_y	= dstDim.y / dstImage.TexBlockDim().y;
		const Bytes		src_row_off		= ImageUtils::RowSize( srcOffset.x, srcImage._bitsPerBlock, srcImage.TexBlockDim() );
		const Bytes		dst_row_off		= ImageUtils::RowSize( dstOffset.x, dstImage._bitsPerBlock, dstImage.TexBlockDim() );
		const uint		src_offset_y	= srcOffset.y / srcImage.TexBlockDim().y;
		const uint		dst_offset_y	= dstOffset.y / dstImage.TexBlockDim().y;

		const auto		dst_parts		= dstImage.Parts();
		const auto		src_parts		= srcImage.Parts();
		auto			dst_part_it		= dst_parts.begin();
		auto			src_part_it		= src_parts.begin();

		Bytes			dst_offset;
		Bytes			src_offset;

		for (uint z = 0; z < dstDim.z; ++z)
		{
			for (uint y = 0; y < block_count_y; ++y)
			{
				const Bytes	dst_row_offset	= dst_slice_pitch * (z + dstOffset.z) + dst_row_pitch * (y + dst_offset_y) + dst_row_off;
				const Bytes	src_row_offset	= src_slice_pitch * (z + srcOffset.z) + src_row_pitch * (y + src_offset_y) + src_row_off;

				for (;;)
				{
					if_likely( IsIntersects( dst_offset, dst_offset + dst_part_it->size, dst_row_offset, dst_row_offset + row_size ))
						break;

					dst_offset += dst_part_it->size;

					if_unlikely( ++dst_part_it == dst_parts.end() )
						return false;
				}
				for (;;)
				{
					if_likely( IsIntersects( src_offset, src_offset + src_part_it->size, src_row_offset, src_row_offset + row_size ))
						break;

					src_offset += src_part_it->size;

					if_unlikely( ++src_part_it == src_parts.end() )
						return false;
				}

				ASSERT( IsCompletelyInside( dst_offset, dst_offset + dst_part_it->size, dst_row_offset, dst_row_offset + row_size ));
				ASSERT( IsCompletelyInside( src_offset, src_offset + src_part_it->size, src_row_offset, src_row_offset + row_size ));
				NonNull( src_part_it->ptr );
				NonNull( dst_part_it->ptr );

				ubyte*			dst_row = Cast<ubyte>( dst_part_it->ptr + (dst_row_offset - dst_offset) );
				ubyte const*	src_row = Cast<ubyte>( src_part_it->ptr + (src_row_offset - src_offset) );

				MemCopy( OUT dst_row, src_row, row_size );
				written += row_size;
			}
		}
		return true;
	}

/*
=================================================
	CopyTo
=================================================
*/
	bool  ImageMemView::CopyTo (OUT void* data, const Bytes size) C_NE___
	{
		CHECK_ERR( size == ContentSize() );

		Bytes	offset;
		for (auto& part : _content.Parts())
		{
			NonNull( part.ptr );
			MemCopy( OUT data + offset, part.ptr, part.size );
			offset += part.size;
		}
		ASSERT( offset == size );
		return true;
	}

	bool  ImageMemView::CopyTo (BufferMemView &dst) C_NE___
	{
		Bytes	copied = dst.CopyFrom( _content );
		return	copied == _content.DataSize();
	}

/*
=================================================
	Compare
=================================================
*/
	Bytes  ImageMemView::Compare (const ImageMemView &rhs) C_NE___
	{
		return Compare( uint3{}, uint3{}, rhs, Dimension() );
	}

	Bytes  ImageMemView::Compare (const uint3 &lhsOffset, const uint3 &rhsOffset, const ImageMemView &rhsImage, const uint3 &dim) C_NE___
	{
		if_unlikely( this->Empty() or rhsImage.Empty() )
			return UMax;

		ASSERT( All( lhsOffset + dim <= this->Dimension() ));
		ASSERT( All( rhsOffset + dim <= rhsImage.Dimension() ));

		ASSERT( All( IsMultipleOf( uint2{lhsOffset}, TexBlockDim() )));
		ASSERT( All( IsMultipleOf( uint2{rhsOffset}, TexBlockDim() )));

		ASSERT( All( IsMultipleOf( uint2{lhsOffset + dim}, TexBlockDim() )));
		ASSERT( All( IsMultipleOf( uint2{rhsOffset + dim}, TexBlockDim() )));

		if ( not All( lhsOffset + dim <= this->Dimension() )	or
			 not All( rhsOffset + dim <= rhsImage.Dimension() )	or
			 this->_format != rhsImage._format					or
			 this->_aspect != rhsImage._aspect )
		{
			return UMax;
		}

		ASSERT( this->_bitsPerBlock == rhsImage._bitsPerBlock );

		const Bytes		row_size		= ImageUtils::RowSize( dim.x, _bitsPerBlock, TexBlockDim() );
		const Bytes		lhs_row_pitch	= this->RowPitch();
		const Bytes		rhs_row_pitch	= rhsImage.RowPitch();
		const Bytes		lhs_slice_pitch	= this->SlicePitch();
		const Bytes		rhs_slice_pitch	= rhsImage.SlicePitch();

		const uint		block_count_y	= dim.y / TexBlockDim().y;
		const Bytes		lhs_row_off		= ImageUtils::RowSize( lhsOffset.x, _bitsPerBlock, TexBlockDim() );
		const Bytes		rhs_row_off		= ImageUtils::RowSize( rhsOffset.x, _bitsPerBlock, TexBlockDim() );
		const uint		lhs_offset_y	= lhsOffset.y / TexBlockDim().y;
		const uint		rhs_offset_y	= rhsOffset.y / TexBlockDim().y;

		const auto		lhs_parts		= this->Parts();
		const auto		rhs_parts		= rhsImage.Parts();
		auto			lhs_part_iter	= lhs_parts.begin();
		auto			rhs_part_iter	= rhs_parts.begin();

		Bytes			lhs_offset;
		Bytes			rhs_offset;
		Bytes			diff		= 0_b;

		for (uint z = 0; z < dim.z; ++z)
		{
			for (uint y = 0; y < block_count_y; ++y)
			{
				const Bytes	lhs_row_offset	= lhs_slice_pitch * (z + lhsOffset.z) + lhs_row_pitch * (y + lhs_offset_y) + lhs_row_off;
				const Bytes	rhs_row_offset	= rhs_slice_pitch * (z + rhsOffset.z) + rhs_row_pitch * (y + rhs_offset_y) + rhs_row_off;

				for (;;)
				{
					if_likely( IsIntersects( lhs_offset, lhs_offset + lhs_part_iter->size, lhs_row_offset, lhs_row_offset + row_size ))
						break;

					if_unlikely( ++lhs_part_iter == lhs_parts.end() )
						return UMax;

					lhs_offset += lhs_part_iter->size;
				}
				for (;;)
				{
					if_likely( IsIntersects( rhs_offset, rhs_offset + rhs_part_iter->size, rhs_row_offset, rhs_row_offset + row_size ))
						break;

					if_unlikely( ++rhs_part_iter == rhs_parts.end() )
						return UMax;

					rhs_offset += rhs_part_iter->size;
				}

				ASSERT( IsCompletelyInside( lhs_offset, lhs_offset + lhs_part_iter->size, lhs_row_offset, lhs_row_offset + row_size ));
				ASSERT( IsCompletelyInside( rhs_offset, rhs_offset + rhs_part_iter->size, rhs_row_offset, rhs_row_offset + row_size ));
				NonNull( lhs_part_iter->ptr );
				NonNull( rhs_part_iter->ptr );

				const ubyte*	lhs_row = Cast<ubyte>( lhs_part_iter->ptr + (lhs_row_offset - lhs_offset) );
				const ubyte*	rhs_row = Cast<ubyte>( rhs_part_iter->ptr + (rhs_row_offset - rhs_offset) );

				for (uint i = 0, cnt = uint(row_size); i < cnt; ++i)	// TODO: optimize
				{
					diff += uint(lhs_row[i] != rhs_row[i]);
				}
			}
		}
		return diff;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Swizzle
=================================================
*/
	RWImageMemView::Swizzle::Swizzle (VecSwizzle src0, VecSwizzle src1) __NE___ :
		Swizzle{ src0.ToVec(), src1.ToVec() }
	{}

	RWImageMemView::Swizzle::Swizzle (ImageSwizzle src0, ImageSwizzle src1) __NE___ :
		Swizzle{ src0.ToVec(), src1.ToVec() }
	{}

	RWImageMemView::Swizzle::Swizzle (const uint4 &sw0, const uint4 & sw1) __NE___ :
		_value{
			(sw0.x >= 1 and sw0.x <= 4) ? (sw0.x - 1) : ((sw1.x >= 1 and sw1.x <= 4) ? (sw1.x - 1) : 4),
			(sw0.y >= 1 and sw0.y <= 4) ? (sw0.y - 1) : ((sw1.y >= 1 and sw1.y <= 4) ? (sw1.y - 1) : 5),
			(sw0.z >= 1 and sw0.z <= 4) ? (sw0.z - 1) : ((sw1.z >= 1 and sw1.z <= 4) ? (sw1.z - 1) : 6),
			(sw0.w >= 1 and sw0.w <= 4) ? (sw0.w - 1) : ((sw1.w >= 1 and sw1.w <= 4) ? (sw1.w - 1) : 7) }
	{}

	template <typename T>
	RGBAColor<T>  RWImageMemView::Swizzle::Transform (const RGBAColor<T> &src0, const RGBAColor<T> &src1) C_NE___
	{
		const T	temp[8] = { src0.r, src0.g, src0.b, src0.a, src1.r, src1.g, src1.b, src1.a, };
		return RGBAColor<T>{ temp[_value.x & 7], temp[_value.y & 7], temp[_value.z & 7], temp[_value.w & 7] };
	}

	bool  RWImageMemView::Swizzle::IsSrc0Identity () C_NE___
	{
		return All( _value == uint4{0, 1, 2, 3} );
	}

	bool  RWImageMemView::Swizzle::IsSrc1Identity () C_NE___
	{
		return All( _value == uint4{4, 5, 6, 7} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BlitOp
=================================================
*/
	RWImageMemView::BlitOp::BlitOp (RWImageMemView const& src, RWImageMemView &dst) __NE___ :
		dim{src.Dimension()}, dstImage{dst}, srcImage{src}
	{}

	RWImageMemView::BlitOp::BlitOp (RWImageMemView const& src, RWImageMemView &dst, const Swizzle &sw) __NE___ :
		dim{src.Dimension()}, swizzle{sw}, dstImage{dst}, srcImage{src}
	{}

/*
=================================================
	FilteredBlitOp
=================================================
*/
	RWImageMemView::FilteredBlitOp::FilteredBlitOp (RWImageMemView const& src, RWImageMemView &dst, EFilter filter) __NE___ :
		dstDim{dst.Dimension()}, srcDim{src.Dimension()}, filter{filter}, dstImage{dst}, srcImage{src}
	{}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RWImageMemView::RWImageMemView (const BufferMemView& content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___ :
		RWImageMemView{ ImageMemView{ content, off, dim, rowPitch, slicePitch, format, aspect }}
	{}

	RWImageMemView::RWImageMemView (void* content, Bytes contentSize, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___ :
		RWImageMemView{ ImageMemView{ content, contentSize, off, dim, rowPitch, slicePitch, format, aspect }}
	{}

	RWImageMemView::RWImageMemView (const ImageMemView& other) __NE___ : ImageMemView{other}
	{
		#define REQ_COLOR_ASPECT()															\
			ASSERT( _aspect == EImageAspect::Color or										\
					(_aspect >= EImageAspect::Plane_0 and _aspect >= EImageAspect::Plane_2 ))

		#define REQ_DEPTH_ASPECT() \
			ASSERT( _aspect == EImageAspect::Depth )

		#define REQ_DEPTH_STENCIL_ASPECT() \
			ASSERT( _aspect == EImageAspect::Depth or _aspect == EImageAspect::Stencil )

		ASSERT( IsSingleBitSet( _aspect ));

		switch_enum( _format )
		{
			case EPixelFormat::RGBA4_UNorm :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*4 );
				_loadF4 = &ReadUNorm<4,4,4,4>;
				_loadI4 = &ReadInt<4,4,4,4>;
				_loadU4	= &ReadUInt<4,4,4,4>;
				_storeF4 = &WriteUNorm<4,4,4,4>;
				_storeI4 = &WriteInt<4,4,4,4>;
				_storeU4 = &WriteUInt<4,4,4,4>;
				break;

			case EPixelFormat::RGB5_A1_UNorm :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*5 + 1 );
				_loadF4 = &ReadUNorm<5,5,5,1>;
				_loadI4 = &ReadInt<5,5,5,1>;
				_loadU4 = &ReadUInt<5,5,5,1>;
				_storeF4 = &WriteUNorm<5,5,5,1>;
				_storeI4 = &WriteInt<5,5,5,1>;
				_storeU4 = &WriteUInt<5,5,5,1>;
				break;

			case EPixelFormat::R5G6B5_UNorm :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 5+6+5 );
				_loadF4 = &ReadUNorm<5,6,5,0>;
				_loadI4 = &ReadInt<5,6,5,0>;
				_loadU4 = &ReadUInt<5,6,5,0>;
				_storeF4 = &WriteUNorm<5,6,5,0>;
				_storeI4 = &WriteInt<5,6,5,0>;
				_storeU4 = &WriteUInt<5,6,5,0>;
				break;

			case EPixelFormat::RGB10_A2_UNorm :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*10 + 2 );
				_loadF4 = &ReadUNorm<10,10,10,2>;
				_loadI4 = &ReadInt<10,10,10,2>;
				_loadU4 = &ReadUInt<10,10,10,2>;
				_storeF4 = &WriteUNorm<10,10,10,2>;
				_storeI4 = &WriteInt<10,10,10,2>;
				_storeU4 = &WriteUInt<10,10,10,2>;
				break;

			case EPixelFormat::R8_SNorm :
			case EPixelFormat::R8_UNorm :
			case EPixelFormat::R8I :
			case EPixelFormat::R8U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 1*8 );
				_loadF4 = (_format == EPixelFormat::R8_SNorm ? &ReadSNorm<8,0,0,0> : _format == EPixelFormat::R8_UNorm ? &ReadUNorm<8,0,0,0> : null);
				_loadI4 = &ReadInt<8,0,0,0>;
				_loadU4 = &ReadUInt<8,0,0,0>;
				_storeF4 = (_format == EPixelFormat::R8_SNorm ? &WriteSNorm<8,0,0,0> : _format == EPixelFormat::R8_UNorm ? &WriteUNorm<8,0,0,0> : null);
				_storeI4 = &WriteInt<8,0,0,0>;
				_storeU4 = &WriteUInt<8,0,0,0>;
				break;

			case EPixelFormat::RG8_SNorm :
			case EPixelFormat::RG8_UNorm :
			case EPixelFormat::RG8I :
			case EPixelFormat::RG8U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 2*8 );
				_loadF4 = (_format == EPixelFormat::RG8_SNorm ? &ReadSNorm<8,8,0,0> : _format == EPixelFormat::RG8_UNorm ? &ReadUNorm<8,8,0,0> : null);
				_loadI4 = &ReadInt<8,8,0,0>;
				_loadU4 = &ReadUInt<8,8,0,0>;
				_storeF4 = (_format == EPixelFormat::RG8_SNorm ? &WriteSNorm<8,8,0,0> : _format == EPixelFormat::RG8_UNorm ? &WriteUNorm<8,8,0,0> : null);
				_storeI4 = &WriteInt<8,8,0,0>;
				_storeU4 = &WriteUInt<8,8,0,0>;
				break;

			case EPixelFormat::RGB8_SNorm :
			case EPixelFormat::RGB8_UNorm :
			case EPixelFormat::RGB8I :
			case EPixelFormat::RGB8U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*8 );
				_loadF4 = (_format == EPixelFormat::RGB8_SNorm ? &ReadSNorm<8,8,8,0> : _format == EPixelFormat::RGB8_UNorm ? &ReadUNorm<8,8,8,0> : null);
				_loadI4 = &ReadInt<8,8,8,0>;
				_loadU4 = &ReadUInt<8,8,8,0>;
				_storeF4 = (_format == EPixelFormat::RGB8_SNorm ? &WriteSNorm<8,8,8,0> : _format == EPixelFormat::RGB8_UNorm ? &WriteUNorm<8,8,8,0> : null);
				_storeI4 = &WriteInt<8,8,8,0>;
				_storeU4 = &WriteUInt<8,8,8,0>;
				break;


			case EPixelFormat::RGBA8_SNorm :
			case EPixelFormat::RGBA8_UNorm :
			case EPixelFormat::RGBA8I :
			case EPixelFormat::RGBA8U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*8 );
				_loadF4 = (_format == EPixelFormat::RGBA8_SNorm ? &ReadSNorm<8,8,8,8> : _format == EPixelFormat::RGBA8_UNorm ? &ReadUNorm<8,8,8,8> : null);
				_loadI4 = &ReadInt<8,8,8,8>;
				_loadU4 = &ReadUInt<8,8,8,8>;
				_storeF4 = (_format == EPixelFormat::RGBA8_SNorm ? &WriteSNorm<8,8,8,8> : _format == EPixelFormat::RGBA8_UNorm ? &WriteUNorm<8,8,8,8> : null);
				_storeI4 = &WriteInt<8,8,8,8>;
				_storeU4 = &WriteUInt<8,8,8,8>;
				break;

			case EPixelFormat::R16_SNorm :
			case EPixelFormat::R16_UNorm :
			case EPixelFormat::R16I :
			case EPixelFormat::R16U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 1*16 );
				_loadF4 = (_format == EPixelFormat::R16_SNorm ? &ReadSNorm<16,0,0,0> : _format == EPixelFormat::R16_UNorm ? &ReadUNorm<16,0,0,0> : null);
				_loadI4 = &ReadInt<16,0,0,0>;
				_loadU4 = &ReadUInt<16,0,0,0>;
				_storeF4 = (_format == EPixelFormat::R16_SNorm ? &WriteSNorm<16,0,0,0> : _format == EPixelFormat::R16_UNorm ? &WriteUNorm<16,0,0,0> : null);
				_storeI4 = &WriteInt<16,0,0,0>;
				_storeU4 = &WriteUInt<16,0,0,0>;
				break;

			case EPixelFormat::RG16_SNorm :
			case EPixelFormat::RG16_UNorm :
			case EPixelFormat::RG16I :
			case EPixelFormat::RG16U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 2*16 );
				_loadF4 = (_format == EPixelFormat::RG16_SNorm ? &ReadSNorm<16,16,0,0> : _format == EPixelFormat::RG16_UNorm ? &ReadUNorm<16,16,0,0> : null);
				_loadI4 = &ReadInt<16,16,0,0>;
				_loadU4 = &ReadUInt<16,16,0,0>;
				_storeF4 = (_format == EPixelFormat::RG16_SNorm ? &WriteSNorm<16,16,0,0> : _format == EPixelFormat::RG16_UNorm ? &WriteUNorm<16,16,0,0> : null);
				_storeI4 = &WriteInt<16,16,0,0>;
				_storeU4 = &WriteUInt<16,16,0,0>;
				break;

			case EPixelFormat::RGB16_SNorm :
			case EPixelFormat::RGB16_UNorm :
			case EPixelFormat::RGB16I :
			case EPixelFormat::RGB16U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*16 );
				_loadF4 = (_format == EPixelFormat::RGB16_SNorm ? &ReadSNorm<16,16,16,0> : _format == EPixelFormat::RGB16_UNorm ? &ReadUNorm<16,16,16,0> : null);
				_loadI4 = &ReadInt<16,16,16,0>;
				_loadU4 = &ReadUInt<16,16,16,0>;
				_storeF4 = (_format == EPixelFormat::RGB16_SNorm ? &WriteSNorm<16,16,16,0> : _format == EPixelFormat::RGB16_UNorm ? &WriteUNorm<16,16,16,0> : null);
				_storeI4 = &WriteInt<16,16,16,0>;
				_storeU4 = &WriteUInt<16,16,16,0>;
				break;

			case EPixelFormat::RGBA16_SNorm :
			case EPixelFormat::RGBA16_UNorm :
			case EPixelFormat::RGBA16I :
			case EPixelFormat::RGBA16U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*16 );
				_loadF4 = (_format == EPixelFormat::RGBA16_SNorm ? &ReadSNorm<16,16,16,16> : _format == EPixelFormat::RGBA16_UNorm ? &ReadUNorm<16,16,16,16> : null);
				_loadI4 = &ReadInt<16,16,16,16>;
				_loadU4 = &ReadUInt<16,16,16,16>;
				_storeF4 = (_format == EPixelFormat::RGBA16_SNorm ? &WriteSNorm<16,16,16,16> : _format == EPixelFormat::RGBA16_UNorm ? &WriteUNorm<16,16,16,16> : null);
				_storeI4 = &WriteInt<16,16,16,16>;
				_storeU4 = &WriteUInt<16,16,16,16>;
				break;

			case EPixelFormat::R32I :
			case EPixelFormat::R32U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 1*32 );
				_loadI4 = &ReadInt<32,0,0,0>;
				_loadU4 = &ReadUInt<32,0,0,0>;
				_storeI4 = &WriteInt<32,0,0,0>;
				_storeU4 = &WriteUInt<32,0,0,0>;
				break;

			case EPixelFormat::RG32I :
			case EPixelFormat::RG32U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 2*32 );
				_loadI4 = &ReadInt<32,32,0,0>;
				_loadU4 = &ReadUInt<32,32,0,0>;
				_storeI4 = &WriteInt<32,32,0,0>;
				_storeU4 = &WriteUInt<32,32,0,0>;
				break;

			case EPixelFormat::RGB32I :
			case EPixelFormat::RGB32U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*32 );
				_loadI4 = &ReadInt<32,32,32,0>;
				_loadU4 = &ReadUInt<32,32,32,0>;
				_storeI4 = &WriteInt<32,32,32,0>;
				_storeU4 = &WriteUInt<32,32,32,0>;
				break;

			case EPixelFormat::RGBA32I :
			case EPixelFormat::RGBA32U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*32 );
				_loadI4 = &ReadInt<32,32,32,32>;
				_loadU4 = &ReadUInt<32,32,32,32>;
				_storeI4 = &WriteInt<32,32,32,32>;
				_storeU4 = &WriteUInt<32,32,32,32>;
				break;

			case EPixelFormat::RGB10_A2U :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*10 + 2 );
				_loadI4 = &ReadInt<10,10,10,2>;
				_loadU4 = &ReadUInt<10,10,10,2>;
				_storeI4 = &WriteInt<10,10,10,2>;
				_storeU4 = &WriteUInt<10,10,10,2>;
				break;

			case EPixelFormat::R16F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 1*16 );
				_loadF4 = &ReadFloat<16,0,0,0>;
				_loadI4 = &ReadInt<16,0,0,0>;
				_loadU4 = &ReadUInt<16,0,0,0>;
				_storeF4 = &WriteFloat<16,0,0,0>;
				_storeI4 = &WriteInt<16,0,0,0>;
				_storeU4 = &WriteUInt<16,0,0,0>;
				break;

			case EPixelFormat::RG16F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 2*16 );
				_loadF4 = &ReadFloat<16,16,0,0>;
				_loadI4 = &ReadInt<16,16,0,0>;
				_loadU4 = &ReadUInt<16,16,0,0>;
				_storeF4 = &WriteFloat<16,16,0,0>;
				_storeI4 = &WriteInt<16,16,0,0>;
				_storeU4 = &WriteUInt<16,16,0,0>;
				break;

			case EPixelFormat::RGB16F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*16 );
				_loadF4 = &ReadFloat<16,16,16,0>;
				_loadI4 = &ReadInt<16,16,16,0>;
				_loadU4 = &ReadUInt<16,16,16,0>;
				_storeF4 = &WriteFloat<16,16,16,0>;
				_storeI4 = &WriteInt<16,16,16,0>;
				_storeU4 = &WriteUInt<16,16,16,0>;
				break;

			case EPixelFormat::RGBA16F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*16 );
				_loadF4 = &ReadFloat<16,16,16,16>;
				_loadI4 = &ReadInt<16,16,16,16>;
				_loadU4 = &ReadUInt<16,16,16,16>;
				_storeF4 = &WriteFloat<16,16,16,16>;
				_storeI4 = &WriteInt<16,16,16,16>;
				_storeU4 = &WriteUInt<16,16,16,16>;
				break;

			case EPixelFormat::R11G11B10F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 11 + 11 + 10 );
				_loadF4 = &ReadFloat_11_11_10;
				_loadI4 = &ReadInt<11,11,10,0>;
				_loadU4 = &ReadUInt<11,11,10,0>;
				_storeF4 = &WriteFloat_11_11_10;
				_storeI4 = &WriteInt<11,11,10,0>;
				_storeU4 = &WriteUInt<11,11,10,0>;
				break;

			case EPixelFormat::R32F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 1*32 );
				_loadF4 = &ReadFloat<32,0,0,0>;
				_loadI4 = &ReadInt<32,0,0,0>;
				_loadU4 = &ReadUInt<32,0,0,0>;
				_storeF4 = &WriteFloat<32,0,0,0>;
				_storeI4 = &WriteInt<32,0,0,0>;
				_storeU4 = &WriteUInt<32,0,0,0>;
				break;

			case EPixelFormat::RG32F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 2*32 );
				_loadF4 = &ReadFloat<32,32,0,0>;
				_loadI4 = &ReadInt<32,32,0,0>;
				_loadU4 = &ReadUInt<32,32,0,0>;
				_storeF4 = &WriteFloat<32,32,0,0>;
				_storeI4 = &WriteInt<32,32,0,0>;
				_storeU4 = &WriteUInt<32,32,0,0>;
				break;

			case EPixelFormat::RGB32F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*32 );
				_loadF4 = &ReadFloat<32,32,32,0>;
				_loadI4 = &ReadInt<32,32,32,0>;
				_loadU4 = &ReadUInt<32,32,32,0>;
				_storeF4 = &WriteFloat<32,32,32,0>;
				_storeI4 = &WriteInt<32,32,32,0>;
				_storeU4 = &WriteUInt<32,32,32,0>;
				break;

			case EPixelFormat::RGBA32F :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*32 );
				_loadF4 = &ReadFloat<32,32,32,32>;
				_loadI4 = &ReadInt<32,32,32,32>;
				_loadU4 = &ReadUInt<32,32,32,32>;
				_storeF4 = &WriteFloat<32,32,32,32>;
				_storeI4 = &WriteInt<32,32,32,32>;
				_storeU4 = &WriteUInt<32,32,32,32>;
				break;

			case EPixelFormat::Depth16 :
			case EPixelFormat::Depth24 :
			case EPixelFormat::Depth32F :
				REQ_DEPTH_ASPECT();
				DBG_WARNING( "TODO" );
				break;

			case EPixelFormat::Depth16_Stencil8	:
			case EPixelFormat::Depth24_Stencil8 :
			case EPixelFormat::Depth32F_Stencil8 :
				REQ_DEPTH_STENCIL_ASPECT();
				DBG_WARNING( "TODO" );
				break;

			case EPixelFormat::sRGB8 :
			case EPixelFormat::sBGR8 :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 3*8 );
				_loadF4 = &ReadUNorm<8,8,8,0>;
				_loadI4 = &ReadInt<8,8,8,0>;
				_loadU4 = &ReadUInt<8,8,8,0>;
				_storeF4 = &WriteUNorm<8,8,8,0>;
				_storeI4 = &WriteInt<8,8,8,0>;
				_storeU4 = &WriteUInt<8,8,8,0>;
				break;

			case EPixelFormat::sRGB8_A8 :
			case EPixelFormat::sBGR8_A8 :
				REQ_COLOR_ASPECT();
				ASSERT( _bitsPerBlock == 4*8 );
				_loadF4 = &ReadUNorm<8,8,8,8>;
				_loadI4 = &ReadInt<8,8,8,8>;
				_loadU4 = &ReadUInt<8,8,8,8>;
				_storeF4 = &WriteUNorm<8,8,8,8>;
				_storeI4 = &WriteInt<8,8,8,8>;
				_storeU4 = &WriteUInt<8,8,8,8>;
				break;

			case EPixelFormat::RGB9F_E5 :
			case EPixelFormat::R64I :
			case EPixelFormat::R64U :
			case EPixelFormat::BGR8_UNorm :
			case EPixelFormat::BGRA8_UNorm :
			case EPixelFormat::BC1_RGB8_UNorm :
			case EPixelFormat::BC1_sRGB8 :
			case EPixelFormat::BC1_RGB8_A1_UNorm :
			case EPixelFormat::BC1_sRGB8_A1 :
			case EPixelFormat::BC2_sRGB8 :
			case EPixelFormat::BC2_RGBA8_UNorm :
			case EPixelFormat::BC3_RGBA8_UNorm :
			case EPixelFormat::BC3_sRGB8 :
			case EPixelFormat::BC4_R8_SNorm :
			case EPixelFormat::BC4_R8_UNorm :
			case EPixelFormat::BC5_RG8_SNorm :
			case EPixelFormat::BC5_RG8_UNorm :
			case EPixelFormat::BC7_RGBA8_UNorm :
			case EPixelFormat::BC7_sRGB8_A8 :
			case EPixelFormat::BC6H_RGB16F :
			case EPixelFormat::BC6H_RGB16UF :
			case EPixelFormat::ETC2_RGB8_UNorm :
			case EPixelFormat::ETC2_sRGB8 :
			case EPixelFormat::ETC2_RGB8_A1_UNorm :
			case EPixelFormat::ETC2_sRGB8_A1 :
			case EPixelFormat::ETC2_RGBA8_UNorm :
			case EPixelFormat::ETC2_sRGB8_A8 :
			case EPixelFormat::EAC_R11_SNorm :
			case EPixelFormat::EAC_R11_UNorm :
			case EPixelFormat::EAC_RG11_SNorm :
			case EPixelFormat::EAC_RG11_UNorm :
			case EPixelFormat::ASTC_RGBA8_4x4 :
			case EPixelFormat::ASTC_RGBA8_5x4 :
			case EPixelFormat::ASTC_RGBA8_5x5 :
			case EPixelFormat::ASTC_RGBA8_6x5 :
			case EPixelFormat::ASTC_RGBA8_6x6 :
			case EPixelFormat::ASTC_RGBA8_8x5 :
			case EPixelFormat::ASTC_RGBA8_8x6 :
			case EPixelFormat::ASTC_RGBA8_8x8 :
			case EPixelFormat::ASTC_RGBA8_10x5 :
			case EPixelFormat::ASTC_RGBA8_10x6 :
			case EPixelFormat::ASTC_RGBA8_10x8 :
			case EPixelFormat::ASTC_RGBA8_10x10 :
			case EPixelFormat::ASTC_RGBA8_12x10 :
			case EPixelFormat::ASTC_RGBA8_12x12 :
			case EPixelFormat::ASTC_sRGB8_A8_4x4 :
			case EPixelFormat::ASTC_sRGB8_A8_5x4 :
			case EPixelFormat::ASTC_sRGB8_A8_5x5 :
			case EPixelFormat::ASTC_sRGB8_A8_6x5 :
			case EPixelFormat::ASTC_sRGB8_A8_6x6 :
			case EPixelFormat::ASTC_sRGB8_A8_8x5 :
			case EPixelFormat::ASTC_sRGB8_A8_8x6 :
			case EPixelFormat::ASTC_sRGB8_A8_8x8 :
			case EPixelFormat::ASTC_sRGB8_A8_10x5 :
			case EPixelFormat::ASTC_sRGB8_A8_10x6 :
			case EPixelFormat::ASTC_sRGB8_A8_10x8 :
			case EPixelFormat::ASTC_sRGB8_A8_10x10 :
			case EPixelFormat::ASTC_sRGB8_A8_12x10 :
			case EPixelFormat::ASTC_sRGB8_A8_12x12 :
			case EPixelFormat::ASTC_RGBA16F_4x4 :
			case EPixelFormat::ASTC_RGBA16F_5x4 :
			case EPixelFormat::ASTC_RGBA16F_5x5 :
			case EPixelFormat::ASTC_RGBA16F_6x5 :
			case EPixelFormat::ASTC_RGBA16F_6x6 :
			case EPixelFormat::ASTC_RGBA16F_8x5 :
			case EPixelFormat::ASTC_RGBA16F_8x6 :
			case EPixelFormat::ASTC_RGBA16F_8x8 :
			case EPixelFormat::ASTC_RGBA16F_10x5 :
			case EPixelFormat::ASTC_RGBA16F_10x6 :
			case EPixelFormat::ASTC_RGBA16F_10x8 :
			case EPixelFormat::ASTC_RGBA16F_10x10 :
			case EPixelFormat::ASTC_RGBA16F_12x10 :
			case EPixelFormat::ASTC_RGBA16F_12x12 :
			{
				REQ_COLOR_ASPECT();
				if ( _bitsPerBlock == 64 )
					_loadU4 = &ReadBlock64;
				else
				if ( _bitsPerBlock == 128 )
					_loadU4 = &ReadBlock128;
				break;
			}

			case EPixelFormat::G8B8G8R8_422_UNorm :
			case EPixelFormat::B8G8R8G8_422_UNorm :
			case EPixelFormat::B16G16R16G16_422_UNorm :
			case EPixelFormat::G16B16G16R16_422_UNorm :
			case EPixelFormat::B10x6G10x6R10x6G10x6_422_UNorm :
			case EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm :
			case EPixelFormat::B12x4G12x4R12x4G12x4_422_UNorm :
			case EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm :
			case EPixelFormat::G8_B8R8_420_UNorm :
			case EPixelFormat::G8_B8R8_422_UNorm :
			case EPixelFormat::G8_B8R8_444_UNorm :
			case EPixelFormat::G16_B16R16_420_UNorm :
			case EPixelFormat::G16_B16R16_422_UNorm :
			case EPixelFormat::G16_B16R16_444_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_422_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_444_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_422_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_444_UNorm :
			case EPixelFormat::G8_B8_R8_420_UNorm :
			case EPixelFormat::G8_B8_R8_422_UNorm :
			case EPixelFormat::G8_B8_R8_444_UNorm :
			case EPixelFormat::G16_B16_R16_420_UNorm :
			case EPixelFormat::G16_B16_R16_422_UNorm :
			case EPixelFormat::G16_B16_R16_444_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_420_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_422_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_444_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_420_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_422_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_444_UNorm :
			case EPixelFormat::R10x6G10x6B10x6A10x6_UNorm :
			case EPixelFormat::R10x6G10x6_UNorm :
			case EPixelFormat::R10x6_UNorm :
			case EPixelFormat::R12x4G12x4B12x4A12x4_UNorm :
			case EPixelFormat::R12x4G12x4_UNorm :
			case EPixelFormat::R12x4_UNorm :
				DBG_WARNING( "use separate ImageMemView per plane" );
				break;

			case EPixelFormat::SwapchainColor :
			case EPixelFormat::Unknown :
			default_unlikely:
				DBG_WARNING( "unknown pixel format" );
				break;
		}
		switch_end
	}

/*
=================================================
	Blit (without filtering)
=================================================
*/
	bool  RWImageMemView::Blit (const BlitOp &op) __NE___
	{
		BlitResult	res;
		return Blit( op, OUT res );
	}

	bool  RWImageMemView::Blit (const BlitOp &op, OUT BlitResult &res) __NE___
	{
		res.readn	= 0_b;
		res.written	= 0_b;

		if_unlikely( op.dstImage.Empty() or op.srcImage.Empty() )
			return false;

		const auto&		src_info	= EPixelFormat_GetInfo( op.srcImage._format );
		const auto&		dst_info	= EPixelFormat_GetInfo( op.dstImage._format );

		CHECK_ERR( not src_info.IsCompressed() );
		CHECK_ERR( not dst_info.IsCompressed() );

		CHECK_ERR( All( op.dstOffset + op.dim <= op.dstImage.Dimension() ));
		CHECK_ERR( All( op.srcOffset + op.dim <= op.srcImage.Dimension() ));
		CHECK_ERR( op.dstImage._aspect == op.srcImage._aspect );

		using EType = PixelFormatInfo::EType;
		constexpr auto	float_mask	= EType::UNorm | EType::SNorm | EType::SFloat | EType::UFloat;
		constexpr auto	int_mask	= EType::SInt;
		constexpr auto	uint_mask	= EType::UInt;

		if ( op.swizzle.IsSrc0Identity() )
		{
			if ( op.dstImage._format == op.srcImage._format )
				return op.dstImage.CopyFrom( op.dstOffset, op.srcOffset, op.srcImage, op.dim );

			LoadPixelFn_t	load	= null;
			StorePixelFn_t	store	= null;
			{
				if ( AnyBits( src_info.valueType, float_mask ) and AnyBits( dst_info.valueType, float_mask ))
				{
					load	= FnUnsafeCast<LoadPixelFn_t >( op.srcImage._loadF4 );
					store	= FnUnsafeCast<StorePixelFn_t>( op.dstImage._storeF4 );
				}else
				if ( AnyBits( src_info.valueType, int_mask ) and AnyBits( dst_info.valueType, int_mask ))
				{
					load	= FnUnsafeCast<LoadPixelFn_t >( op.srcImage._loadI4 );
					store	= FnUnsafeCast<StorePixelFn_t>( op.dstImage._storeI4 );
				}else
				if ( AnyBits( src_info.valueType, uint_mask ) and AnyBits( dst_info.valueType, uint_mask ))
				{
					load	= FnUnsafeCast<LoadPixelFn_t >( op.srcImage._loadU4 );
					store	= FnUnsafeCast<StorePixelFn_t>( op.dstImage._storeU4 );
				}
			}
			CHECK_ERR( load != null and store != null );
			return _Blit( op, OUT res, load, store );
		}
		else
		{
			if ( AnyBits( src_info.valueType, float_mask ) and AnyBits( dst_info.valueType, float_mask ))
				return _BlitSw<RGBA32f>( op, OUT res, op.srcImage._loadF4, op.dstImage._loadF4, op.dstImage._storeF4 );

			if ( AnyBits( src_info.valueType, int_mask ) and AnyBits( dst_info.valueType, int_mask ))
				return _BlitSw<RGBA32i>( op, OUT res, op.srcImage._loadI4, op.dstImage._loadI4, op.dstImage._storeI4 );

			if ( AnyBits( src_info.valueType, uint_mask ) and AnyBits( dst_info.valueType, uint_mask ))
				return _BlitSw<RGBA32u>( op, OUT res, op.srcImage._loadU4, op.dstImage._loadU4, op.dstImage._storeU4 );
		}
		return false;
	}

/*
=================================================
	_Blit (without swizzle)
=================================================
*/
	bool  RWImageMemView::_Blit (const BlitOp &op, OUT BlitResult &res, LoadPixelFn_t load, StorePixelFn_t store) __NE___
	{
		const Bytes		src_row_size	= ImageUtils::RowSize( op.dim.x, op.srcImage._bitsPerBlock, op.srcImage.TexBlockDim() );
		const Bytes		dst_row_size	= ImageUtils::RowSize( op.dim.x, op.dstImage._bitsPerBlock, op.dstImage.TexBlockDim() );

		for (uint z = 0; z < op.dim.z; ++z)
		{
			for (uint y = 0; y < op.dim.y; ++y)
			{
				auto	src_row	= op.srcImage.GetRow( op.srcOffset.y + y, op.srcOffset.z + z );
				auto	dst_row	= op.dstImage.GetRow( op.dstOffset.y + y, op.dstOffset.z + z );

				if_unlikely( src_row.Empty() or dst_row.Empty() )
					return false;

				for (uint x = 0; x < op.dim.x; ++x)
				{
					PixStorage_t	pix_storage;
					load(  src_row, op.srcOffset.x + x, OUT pix_storage );
					store( dst_row, op.dstOffset.x + x, pix_storage );
				}

				res.readn	+= src_row_size;
				res.written	+= dst_row_size;
			}
		}
		return true;
	}

/*
=================================================
	_BlitSw (with swizzle)
=================================================
*/
	template <typename T>
	bool  RWImageMemView::_BlitSw (const BlitOp &op, OUT BlitResult &res, LoadPixelTFn_t<T> loadSrc, LoadPixelTFn_t<T> loadDst, StorePixelTFn_t<T> store) __NE___
	{
		StaticAssert( sizeof(T) == sizeof(PixStorage_t) );

		const Bytes		src_row_size	= ImageUtils::RowSize( op.dim.x, op.srcImage._bitsPerBlock, op.srcImage.TexBlockDim() );
		const Bytes		dst_row_size	= ImageUtils::RowSize( op.dim.x, op.dstImage._bitsPerBlock, op.dstImage.TexBlockDim() );

		for (uint z = 0; z < op.dim.z; ++z)
		{
			for (uint y = 0; y < op.dim.y; ++y)
			{
				auto	src_row	= op.srcImage.GetRow( op.srcOffset.y + y, op.srcOffset.z + z );
				auto	dst_row	= op.dstImage.GetRow( op.dstOffset.y + y, op.dstOffset.z + z );

				if_unlikely( src_row.Empty() or dst_row.Empty() )
					return false;

				for (uint x = 0; x < op.dim.x; ++x)
				{
					T	pix_storage0;
					T	pix_storage1;

					loadSrc( src_row,         op.srcOffset.x + x, OUT pix_storage0 );
					loadDst( CRow_t{dst_row}, op.dstOffset.x + x, OUT pix_storage1 );

					T	pix_storage = op.swizzle.Transform( pix_storage0, pix_storage1 );

					store( dst_row, op.dstOffset.x + x, pix_storage );
				}

				res.readn	+= src_row_size;
				res.written	+= dst_row_size;
			}
		}
		return true;
	}

/*
=================================================
	Blit (with filtering)
=================================================
*/
	bool  RWImageMemView::Blit (const FilteredBlitOp &op) __NE___
	{
		BlitResult	res;
		return Blit( op, OUT res );
	}

	bool  RWImageMemView::Blit (const FilteredBlitOp &op, OUT BlitResult &res) __NE___
	{
		res.readn	= 0_b;
		res.written	= 0_b;

		if_unlikely( op.dstImage.Empty() or op.srcImage.Empty() )
			return false;

		const auto&		src_info	= EPixelFormat_GetInfo( op.srcImage._format );
		const auto&		dst_info	= EPixelFormat_GetInfo( op.dstImage._format );

		CHECK_ERR( not src_info.IsCompressed() );
		CHECK_ERR( not dst_info.IsCompressed() );

		CHECK_ERR( All( op.dstDim == Max( op.srcDim/2, 1u )));
		CHECK_ERR( All( op.dstOffset + op.dstDim <= op.dstImage.Dimension() ));
		CHECK_ERR( All( op.srcOffset + op.srcDim <= op.srcImage.Dimension() ));
		CHECK_ERR( op.dstImage._aspect == op.srcImage._aspect );

		using EType = PixelFormatInfo::EType;
		constexpr auto	float_mask	= EType::UNorm | EType::SNorm | EType::SFloat | EType::UFloat;
		constexpr auto	int_mask	= EType::SInt;
		constexpr auto	uint_mask	= EType::UInt;

		LoadPixelFn_t		load	= null;
		StorePixelFn_t		store	= null;
		FilterPixelsFn_t	filter	= null;
		{
			if ( AnyBits( src_info.valueType, float_mask ) and AnyBits( dst_info.valueType, float_mask ))
			{
				load	= FnUnsafeCast<LoadPixelFn_t >( op.srcImage._loadF4 );
				store	= FnUnsafeCast<StorePixelFn_t>( op.dstImage._storeF4 );
				filter	= op.filter == EFilter::Linear ? FnUnsafeCast<FilterPixelsFn_t>( &LinearFilterF4 ) : &NearestFilter;
			}else
			if ( AnyBits( src_info.valueType, int_mask ) and AnyBits( dst_info.valueType, int_mask ))
			{
				load	= FnUnsafeCast<LoadPixelFn_t >( op.srcImage._loadI4 );
				store	= FnUnsafeCast<StorePixelFn_t>( op.dstImage._storeI4 );
				filter	= &NearestFilter;
			}else
			if ( AnyBits( src_info.valueType, uint_mask ) and AnyBits( dst_info.valueType, uint_mask ))
			{
				load	= FnUnsafeCast<LoadPixelFn_t >( op.srcImage._loadU4 );
				store	= FnUnsafeCast<StorePixelFn_t>( op.dstImage._storeU4 );
				filter	= &NearestFilter;
			}
		}
		CHECK_ERR( load != null and store != null );
		return _FilteredBlit( op, OUT res, load, store, filter );
	}

/*
=================================================
	_FilteredBlit
=================================================
*/
	bool  RWImageMemView::_FilteredBlit (const FilteredBlitOp &op, OUT BlitResult &res,
										 LoadPixelFn_t load, StorePixelFn_t store, FilterPixelsFn_t filter) __NE___
	{
		res.readn	= 0_b;
		res.written	= 0_b;

		const Bytes		src_row_size	= ImageUtils::RowSize( op.srcDim.x, op.srcImage._bitsPerBlock, op.srcImage.TexBlockDim() );
		const Bytes		dst_row_size	= ImageUtils::RowSize( op.dstDim.x, op.dstImage._bitsPerBlock, op.dstImage.TexBlockDim() );

		for (uint z = 0; z < op.dstDim.z; ++z)
		{
			for (uint y = 0; y < op.dstDim.y; ++y)
			{
				auto	src_row0	= op.srcImage.GetRow( op.srcOffset.y + y*2 + 0,	op.srcOffset.z + z );
				auto	src_row1	= op.srcImage.GetRow( op.srcOffset.y + y*2 + 1,	op.srcOffset.z + z );
				auto	dst_row		= op.dstImage.GetRow( op.dstOffset.y + y,		op.dstOffset.z + z );

				if_unlikely( src_row0.Empty() or src_row1.Empty() or dst_row.Empty() )
					return false;

				for (uint x = 0; x < op.dstDim.x; ++x)
				{
					PixStorage_t	src_pixels[4];
					PixStorage_t	dst_pixel;

					for (uint y2 = 0; y2 < 2; ++y2)
					for (uint x2 = 0; x2 < 2; ++x2){
						load( (y2 == 0 ? src_row0 : src_row1), op.srcOffset.x + x*2 + x2, OUT src_pixels[x2 + y2*2] );
					}

					filter( src_pixels, OUT dst_pixel );
					store( dst_row, op.dstOffset.x + x, dst_pixel );
				}

				res.readn	+= src_row_size;
				res.written	+= dst_row_size;
			}
		}
		return true;
	}

/*
=================================================
	Fill
=================================================
*/
	bool  RWImageMemView::Fill (const RGBA32f &col, const uint3 &offset, const uint3 &dim) __NE___
	{
		if_unlikely( _storeF4 == null )
			return false;

		PixStorage_t	data;
		Row_t			row{ data.data(), Sizeof(data) };

		_storeF4( OUT row, 0, col );
		return _Fill( CRow_t{row}, offset, dim );
	}

	bool  RWImageMemView::Fill (const RGBA32u &col, const uint3 &offset, const uint3 &dim) __NE___
	{
		if_unlikely( _storeU4 == null )
			return false;

		PixStorage_t	data;
		Row_t			row{ data.data(), Sizeof(data) };

		_storeU4( OUT row, 0, col );
		return _Fill( CRow_t{row}, offset, dim );
	}

	bool  RWImageMemView::Fill (const RGBA32i &col, const uint3 &offset, const uint3 &dim) __NE___
	{
		if_unlikely( _storeI4 == null )
			return false;

		PixStorage_t	data;
		Row_t			row{ data.data(), Sizeof(data) };

		_storeI4( OUT row, 0, col );
		return _Fill( CRow_t{row}, offset, dim );
	}

	bool  RWImageMemView::_Fill (const CRow_t &data, const uint3 &offset, const uint3 &dim) __NE___
	{
		CHECK_ERR( All( offset + dim <= Dimension() ));

		const auto&		fmt_info	= EPixelFormat_GetInfo( _format );
		const Bytes		row_size	= ImageUtils::RowSize( dim.x, _bitsPerBlock, TexBlockDim() );
		const Bytes		pix_size	= Bytes{fmt_info.BitsPerPixel() / 8};

		CHECK_ERR( not fmt_info.IsCompressed() );
		CHECK_ERR( pix_size * dim.x == row_size );
		CHECK_ERR( pix_size <= data.size );

		for (uint z = 0; z < dim.z; ++z)
		{
			for (uint y = 0; y < dim.y; ++y)
			{
				auto	dst_row	= this->GetRow( offset.y + y, offset.z + z );

				for (uint x = 0; x < dim.x; ++x)
				{
					MemCopy( OUT dst_row.ptr + (x + offset.x) * pix_size, data.ptr, pix_size );
				}
			}
		}
		return true;
	}


} // AE::Graphics
