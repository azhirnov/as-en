// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermMesh.h"
#include "res_loaders/Intermediate/IntermVertexAttribs_Setter.h"

namespace AE::ResLoader
{
namespace
{
	using ENormalEncoding = IntermMesh::ENormalEncoding;

/*
=================================================
	NeedConvertVertexData
=================================================
*/
	ND_ static bool  NeedConvertVertexData (const IntermVertexAttribs &attribs, StringView id, const EVertexType requiredType)
	{
		auto*	vert = attribs.FindVertex( id );
		if ( vert == null )
			return false;

		const EVertexType	mask		= ~(EVertexType::NormalizedFlag | EVertexType::ScaledFlag);
		const EVertexType	cur_type	= vert->type & mask;
		const EVertexType	req_type	= requiredType & mask;

		// same types, don't convert
		if ( cur_type == req_type )
			return false;

		return true;
	}

/*
=================================================
	NeedConvertMesh
=================================================
*/
	ND_ static bool  NeedConvertMesh (const IntermVertexAttribs &attribs)
	{
		return	NeedConvertVertexData( attribs, VertexAttributeName::Position,		EVertexType::Float3 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::Position_1,	EVertexType::Float3 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::Normal,		EVertexType::Float3 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::BiTangent,		EVertexType::Float3 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::Tangent,		EVertexType::Float3 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::LightmapUV,	EVertexType::Float2 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::TextureUVs[0],	EVertexType::Float2 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::TextureUVs[1],	EVertexType::Float2 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::TextureUVs[2],	EVertexType::Float2 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::TextureUVs[3],	EVertexType::Float2 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::Color,			EVertexType::Float4 )	or
				NeedConvertVertexData( attribs, VertexAttributeName::ObjectID,		EVertexType::UInt )		or
				NeedConvertVertexData( attribs, VertexAttributeName::MaterialID,	EVertexType::UInt );
				// TODO: BoneWeights
	}

/*
=================================================
	CreateVertexAttribs
=================================================
*/
	static void  CreateVertexAttribs (const IntermVertexAttribs &oldAttribs, OUT Bytes &stride, OUT IntermVertexAttribs &newAttribs)
	{
		using Name_t = IntermVertexAttribs::Name_t;

		IntermVertexAttribs::Setter	set{ newAttribs };

		stride = 0_b;

		const auto	AddAttrib = [&oldAttribs, &stride, &set] (const Name_t &id, EVertexType requiredType)
		{{
			auto*	vert = oldAttribs.FindVertex( id );
			if ( vert != null )
			{
				set.AddVertex( id, requiredType, stride );
				stride += EVertexType_SizeOf( requiredType );
			}
		}};
		const auto	AddAttribUV = [&oldAttribs, &stride, &set] (const Name_t &id)
		{{
			auto*	vert = oldAttribs.FindVertex( id );
			if ( vert != null )
			{
				EVertexType		dst_type = EVertexType::Float2;
				if ( EVertexType_GetVec( vert->type ) >= 3 )
					dst_type = EVertexType::Float3;

				set.AddVertex( id, dst_type, stride );
				stride += EVertexType_SizeOf( dst_type );
			}
		}};

		AddAttrib( VertexAttributeName::Position,		EVertexType::Float3 );
		AddAttrib( VertexAttributeName::Position_1,		EVertexType::Float3 );
		AddAttrib( VertexAttributeName::Normal,			EVertexType::Float3 );
		AddAttrib( VertexAttributeName::BiTangent,		EVertexType::Float3 );
		AddAttrib( VertexAttributeName::Tangent,		EVertexType::Float3 );
		AddAttrib( VertexAttributeName::LightmapUV,		EVertexType::Float2 );
		AddAttrib( VertexAttributeName::Color,			EVertexType::Float4 );
		AddAttrib( VertexAttributeName::ObjectID,		EVertexType::UInt );
		AddAttrib( VertexAttributeName::MaterialID,		EVertexType::UInt );
		AddAttribUV( VertexAttributeName::TextureUVs[0] );
		AddAttribUV( VertexAttributeName::TextureUVs[1] );
		AddAttribUV( VertexAttributeName::TextureUVs[2] );
		AddAttribUV( VertexAttributeName::TextureUVs[3] );
		// TODO: BoneWeights

		set.AddBuffer( stride, EVertexInputRate::Vertex );
	}

/*
=================================================
	CopyVertices
=================================================
*/
	struct CopyVertexParams
	{
		ArrayView<ubyte>		srcVertices;
		MutableArrayView<ubyte>	dstVertices;

		usize	vertexCount		= 0;

		Bytes	srcElemOffset;
		Bytes	dstElemOffset;

		Bytes	srcVertexStride;
		Bytes	dstVertexStride;

		Bytes	srcElemSize;
		Bytes	dstElemSize;
	};

	ND_ static bool  CopyVertices (CopyVertexParams &p)
	{
		CHECK_ERR( p.srcElemSize == p.dstElemSize );

		const void*		src	= p.srcVertices.data() + p.srcElemOffset;
		void*			dst	= p.dstVertices.data() + p.dstElemOffset;

		for (usize i = 0; i < p.vertexCount; ++i)
		{
			memcpy( OUT dst, src, usize{p.srcElemSize} );

			src += p.srcVertexStride;
			dst += p.dstVertexStride;
		}
		return true;
	}

/*
=================================================
	Converter
=================================================
*/
	enum class EConvMode
	{
		Cast,
		NormalizedInt,
		ScaledInt,
	};

	template <typename Dst, typename Src, EConvMode Mode>
	ND_ Dst  Converter (const Src &src)
	{
		using SrcScalar	= UnwrapType< Src >;
		using DstScalar	= UnwrapType< Dst >;

		constexpr uint	src_len = VecSize< Src >;
		constexpr uint	dst_len = VecSize< Dst >;
		constexpr uint	len		= Min( src_len, dst_len );
		Dst				dst;

		if constexpr( IsSame< DstScalar, float > and Mode == EConvMode::NormalizedInt )
		{
			if constexpr( IsSame< SrcScalar, ushort >) {
				for (uint i = 0; i < len; ++i)	dst[i] = UNormShortToFloat( src[i] );
				return dst;
			}
			if constexpr( IsSame< SrcScalar, short  >) {
				for (uint i = 0; i < len; ++i)	dst[i] = SNormShortToFloat( src[i] );
				return dst;
			}
			if constexpr( IsSame< SrcScalar, ubyte  >) {
				for (uint i = 0; i < len; ++i)	dst[i] = UNormByteToFloat( src[i] );
				return dst;
			}
			if constexpr( IsSame< SrcScalar, sbyte  >) {
				for (uint i = 0; i < len; ++i)	dst[i] = SNormByteToFloat( src[i] );
				return dst;
			}
		}

		if constexpr( IsSame< DstScalar, float > and (Mode == EConvMode::ScaledInt or Mode == EConvMode::Cast) )
		{
			for (uint i = 0; i < len; ++i) dst[i] = DstScalar( src[i] );
			return dst;
		}
	}

/*
=================================================
	ConvertVerticesT
=================================================
*/
	template <typename SrcType, typename DstType, EConvMode Mode>
	ND_ static bool  ConvertVerticesT (CopyVertexParams &p)
	{
		CHECK_ERR( p.srcElemSize == sizeof(SrcType) );
		CHECK_ERR( p.dstElemSize == sizeof(DstType) );

		const void*		src	= p.srcVertices.data() + p.srcElemOffset;
		void*			dst	= p.dstVertices.data() + p.dstElemOffset;

		for (usize i = 0; i < p.vertexCount; ++i)
		{
			SrcType	from;
			memcpy( OUT &from, src, sizeof(SrcType) );

			DstType	to = Converter< DstType, SrcType, Mode >( from );

			memcpy( OUT dst, &to, sizeof(DstType) );

			src += p.srcVertexStride;
			dst += p.dstVertexStride;
		}
		return true;
	}

/*
=================================================
	ConvertVertices*
=================================================
*/
	template <typename T>
	ND_ static bool  ConvertVerticesNormInt (CopyVertexParams &p, EVertexType reqType)
	{
		StaticAssert( IsInteger< UnwrapType< T >>);
		switch ( reqType )
		{
			case EVertexType::Float2 :	return ConvertVerticesT< T, packed_float2, EConvMode::NormalizedInt >( p );
			case EVertexType::Float3 :	return ConvertVerticesT< T, packed_float3, EConvMode::NormalizedInt >( p );
			case EVertexType::Float4 :	return ConvertVerticesT< T, packed_float4, EConvMode::NormalizedInt >( p );
		}
		return false;
	}

	template <typename T>
	ND_ static bool  ConvertVerticesScaled (CopyVertexParams &p, EVertexType reqType)
	{
		StaticAssert( IsInteger< UnwrapType< T >>);
		switch ( reqType )
		{
			case EVertexType::Float2 :	return ConvertVerticesT< T, packed_float2, EConvMode::ScaledInt >( p );
			case EVertexType::Float3 :	return ConvertVerticesT< T, packed_float3, EConvMode::ScaledInt >( p );
			case EVertexType::Float4 :	return ConvertVerticesT< T, packed_float4, EConvMode::ScaledInt >( p );
		}
		return false;
	}

	template <typename T>
	ND_ static bool  ConvertVerticesFp (CopyVertexParams &p, EVertexType reqType)
	{
		StaticAssert( IsAnyFloatPoint< UnwrapType< T >>);
		switch ( reqType )
		{
			case EVertexType::Float2 :	return ConvertVerticesT< T, packed_float2, EConvMode::Cast >( p );
			case EVertexType::Float3 :	return ConvertVerticesT< T, packed_float3, EConvMode::Cast >( p );
			case EVertexType::Float4 :	return ConvertVerticesT< T, packed_float4, EConvMode::Cast >( p );
		}
		return false;
	}

/*
=================================================
	ConvertVerticesNorm
=================================================
*/
	template <typename SrcType, typename DstType, EConvMode Mode>
	ND_ static bool  ConvertVerticesNormT (CopyVertexParams &p)
	{
		CHECK_ERR( p.srcElemSize == sizeof(SrcType) );
		CHECK_ERR( p.dstElemSize == sizeof(DstType) );

		const void*		src	= p.srcVertices.data() + p.srcElemOffset;
		void*			dst	= p.dstVertices.data() + p.dstElemOffset;

		for (usize i = 0; i < p.vertexCount; ++i)
		{
			SrcType	from;
			memcpy( OUT &from, src, sizeof(SrcType) );

			DstType	to = Converter< DstType, SrcType, Mode >( from );

			if constexpr( IsUnsignedInteger< UnwrapType<SrcType> >)
				to = ToSNorm( to );

			//ASSERT( IsNormalized( to, 1.0e-2f ));
			to = Normalize( to );

			memcpy( OUT dst, &to, sizeof(DstType) );

			src += p.srcVertexStride;
			dst += p.dstVertexStride;
		}
		return true;
	}

	template <typename T>
	ND_ static bool  ConvertVerticesNorm (CopyVertexParams &p, EVertexType, ENormalEncoding normalEncoding)
	{
		StaticAssert( IsInteger< UnwrapType< T >>);

		switch_enum( normalEncoding )
		{
			case ENormalEncoding::Unknown :
				return ConvertVerticesNormT< T, packed_float3, EConvMode::NormalizedInt >( p );

			// TODO
		}
		switch_end
		return false;
	}

/*
=================================================
	ConvertVertices
=================================================
*/
	ND_ static bool  ConvertVertices (const IntermVertexAttribs &oldAttribs, ArrayView<ubyte> oldVertices,
									  const IntermVertexAttribs &newAttribs, OUT MutableArrayView<ubyte> newVertices,
									  const usize vertexCount, ENormalEncoding normalEncoding)
	{
		const auto	ConvertAttrib = [&] (StringView id) -> bool
		{{
			auto*	src_vert = oldAttribs.FindVertex( id );
			auto*	dst_vert = newAttribs.FindVertex( id );

			CHECK( (src_vert == null) == (dst_vert == null) );
			if ( src_vert == null or dst_vert == null )
				return true;

			const auto	cur_type	= src_vert->type;
			const auto	req_type	= dst_vert->type;
			const bool	is_norm		= id == VertexAttributeName::Normal		or
									  id == VertexAttributeName::BiTangent	or
									  id == VertexAttributeName::Tangent;

			CopyVertexParams	params;
			params.srcElemOffset	= src_vert->offset;
			params.dstElemOffset	= dst_vert->offset;
			params.srcElemSize		= EVertexType_SizeOf( cur_type );
			params.dstElemSize		= EVertexType_SizeOf( req_type );
			params.srcVertexStride	= oldAttribs.BufferBindings()[0].second.stride;
			params.dstVertexStride	= newAttribs.BufferBindings()[0].second.stride;
			params.srcVertices		= oldVertices;
			params.dstVertices		= newVertices;
			params.vertexCount		= vertexCount;

			if ( (cur_type & ~EVertexType::_FlagMask) == (req_type & ~EVertexType::_FlagMask) )
				return CopyVertices( params );

			if ( is_norm )
			{
				CHECK_ERR( EVertexType_GetVec( req_type ) == 3 );
				switch ( cur_type )
				{
					// normalized int
					case EVertexType::Byte4_Norm :		return ConvertVerticesNorm< sbyte4  >( params, req_type, normalEncoding );
					case EVertexType::UByte4_Norm :		return ConvertVerticesNorm< ubyte4  >( params, req_type, normalEncoding );
					case EVertexType::Short4_Norm :		return ConvertVerticesNorm< short4  >( params, req_type, normalEncoding );
					case EVertexType::UShort4_Norm :	return ConvertVerticesNorm< ushort4 >( params, req_type, normalEncoding );
				}
			}

			switch ( cur_type )
			{
				// normalized int
				case EVertexType::Byte2_Norm :		return ConvertVerticesNormInt< sbyte2 >( params, req_type );
				case EVertexType::Byte3_Norm :		return ConvertVerticesNormInt< sbyte3 >( params, req_type );
				case EVertexType::Byte4_Norm :		return ConvertVerticesNormInt< sbyte4 >( params, req_type );

				case EVertexType::UByte2_Norm :		return ConvertVerticesNormInt< ubyte2 >( params, req_type );
				case EVertexType::UByte3_Norm :		return ConvertVerticesNormInt< ubyte3 >( params, req_type );
				case EVertexType::UByte4_Norm :		return ConvertVerticesNormInt< ubyte4 >( params, req_type );

				case EVertexType::Short2_Norm :		return ConvertVerticesNormInt< short2 >( params, req_type );
				case EVertexType::Short3_Norm :		return ConvertVerticesNormInt< short3 >( params, req_type );
				case EVertexType::Short4_Norm :		return ConvertVerticesNormInt< short4 >( params, req_type );

				case EVertexType::UShort2_Norm :	return ConvertVerticesNormInt< ushort2 >( params, req_type );
				case EVertexType::UShort3_Norm :	return ConvertVerticesNormInt< ushort3 >( params, req_type );
				case EVertexType::UShort4_Norm :	return ConvertVerticesNormInt< ushort4 >( params, req_type );

				// scaled int
				case EVertexType::Byte2_Scaled :	return ConvertVerticesScaled< sbyte2 >( params, req_type );
				case EVertexType::Byte3_Scaled :	return ConvertVerticesScaled< sbyte3 >( params, req_type );
				case EVertexType::Byte4_Scaled :	return ConvertVerticesScaled< sbyte4 >( params, req_type );

				case EVertexType::UByte2_Scaled :	return ConvertVerticesScaled< ubyte2 >( params, req_type );
				case EVertexType::UByte3_Scaled :	return ConvertVerticesScaled< ubyte3 >( params, req_type );
				case EVertexType::UByte4_Scaled :	return ConvertVerticesScaled< ubyte4 >( params, req_type );

				case EVertexType::Short2_Scaled :	return ConvertVerticesScaled< short2 >( params, req_type );
				case EVertexType::Short3_Scaled :	return ConvertVerticesScaled< short3 >( params, req_type );
				case EVertexType::Short4_Scaled :	return ConvertVerticesScaled< short4 >( params, req_type );

				case EVertexType::UShort2_Scaled :	return ConvertVerticesScaled< ushort2 >( params, req_type );
				case EVertexType::UShort3_Scaled :	return ConvertVerticesScaled< ushort3 >( params, req_type );
				case EVertexType::UShort4_Scaled :	return ConvertVerticesScaled< ushort4 >( params, req_type );

				// floating point
				case EVertexType::Half2 :			return ConvertVerticesFp< packed_half2 >( params, req_type );
				case EVertexType::Half3 :			return ConvertVerticesFp< packed_half3 >( params, req_type );
				case EVertexType::Half4 :			return ConvertVerticesFp< packed_half4 >( params, req_type );

				case EVertexType::Float2 :			return ConvertVerticesFp< packed_float2 >( params, req_type );
				case EVertexType::Float3 :			return ConvertVerticesFp< packed_float3 >( params, req_type );
				case EVertexType::Float4 :			return ConvertVerticesFp< packed_float4 >( params, req_type );
			}
			RETURN_ERR( "unsupported type" );
		}};

		CHECK_ERR( oldAttribs.BufferBindings().size() == 1 );
		CHECK_ERR( newAttribs.BufferBindings().size() == 1 );

		return	ConvertAttrib( VertexAttributeName::Position		) and
				ConvertAttrib( VertexAttributeName::Position_1		) and
				ConvertAttrib( VertexAttributeName::Normal			) and
				ConvertAttrib( VertexAttributeName::BiTangent		) and
				ConvertAttrib( VertexAttributeName::Tangent			) and
				ConvertAttrib( VertexAttributeName::LightmapUV		) and
				ConvertAttrib( VertexAttributeName::TextureUVs[0]	) and
				ConvertAttrib( VertexAttributeName::TextureUVs[1]	) and
				ConvertAttrib( VertexAttributeName::TextureUVs[2]	) and
				ConvertAttrib( VertexAttributeName::TextureUVs[3]	) and
				ConvertAttrib( VertexAttributeName::Color			) and
				ConvertAttrib( VertexAttributeName::ObjectID		) and
				ConvertAttrib( VertexAttributeName::MaterialID		);
				// TODO: BoneWeights
	}

} // namespace

/*
=================================================
	ConvertToFloatPointFormat
=================================================
*/
	bool  IntermMesh::ConvertToFloatPointFormat (OUT IntermMesh &dstMesh) C_NE___
	{
		CHECK_ERR( _attribs );

		if ( not NeedConvertMesh( *_attribs ))
		{
			NOTHROW_ERR( dstMesh.Set( _vertices, _attribs, _vertexStride, _topology, _indices, _indexType ));
			return true;
		}

		Bytes			new_stride;
		Array<ubyte>	new_vertices;
		auto			new_attribs = MakeRC<IntermVertexAttribs>();

		CreateVertexAttribs( *_attribs, OUT new_stride, OUT *new_attribs );

		NOTHROW_ERR( new_vertices.resize( usize{new_stride} * VertexCount() ));

		CHECK_ERR( ConvertVertices( *_attribs, _vertices, *new_attribs, OUT new_vertices, VertexCount(), NormalEncoding() ));

		NOTHROW_ERR( dstMesh.Set( RVRef(new_vertices), RVRef(new_attribs), new_stride, _topology, _indices, _indexType ));
		return true;
	}


} // AE::ResLoader
