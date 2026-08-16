// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "res_loaders/Intermediate/IntermMesh.h"

namespace AE::ResLoader
{

/*
=================================================
	InvertFrontFace
=================================================
*/
	bool  IntermMesh::InvertFrontFace () __NE___
	{
		CHECK_ERR( _topology == EPrimitive::TriangleList );

		const auto	ReverseOrder = [] (auto indices)
		{{
			for (usize i = 0; i+2 < indices.size(); i += 3)
			{
				auto	i0	= indices[i+0];
				auto	i2	= indices[i+2];

				indices[i+0] = i2;
				indices[i+2] = i0;
			}
		}};

		switch_enum( IndexType() )
		{
			case EIndex::UShort :
				ReverseOrder( GetIndexData<ushort>() );		break;

			case EIndex::UInt :
				ReverseOrder( GetIndexData<uint>() );		break;

			case EIndex::_Count :
			default :
				return false;
		}
		switch_end
		return true;
	}

/*
=================================================
	InvertNormals
=================================================
*/
	template <typename T>
	static void  InvertNormals2 (INOUT void* ptr, const usize count, const Bytes stride)
	{
		for (usize i = 0; i < count; ++i)
		{
			T	n;
			memcpy( OUT &n, ptr, sizeof(T) );

			n = -n;
			memcpy( OUT ptr, &n, sizeof(T) );

			ptr += stride;
		}
	}

	ND_ static bool  InvertNormals1 (INOUT void* ptr, usize count, Bytes stride, EVertexType type)
	{
		switch ( type )
		{
			case EVertexType::Float3 :	InvertNormals2< packed_float3 >( ptr, count, stride );	return true;
		//	case EVertexType::Half3 :	InvertNormals2< packed_half3 >( ptr, count, stride );	return true;

			// TODO
		}
		return false;
	}


	bool  IntermMesh::InvertNormals () __NE___
	{
		CHECK_ERR( _normalEncoding == Default );
		CHECK_ERR( _attribs );

		const usize	count = VertexCount();

		if ( auto* norm = _attribs->FindVertex( VertexAttributeName::Normal ))
		{
			CHECK_ERR( InvertNormals1( INOUT _vertices.data() + norm->offset, count, _vertexStride, norm->type ));
		}

		if ( auto* tan = _attribs->FindVertex( VertexAttributeName::Tangent ))
		{
			CHECK_ERR( InvertNormals1( INOUT _vertices.data() + tan->offset, count, _vertexStride, tan->type ));
		}

		if ( auto* bitan = _attribs->FindVertex( VertexAttributeName::BiTangent ))
		{
			CHECK_ERR( InvertNormals1( INOUT _vertices.data() + bitan->offset, count, _vertexStride, bitan->type ));
		}

		return true;
	}

/*
=================================================
	Recenter
=================================================
*/
	bool  IntermMesh::Recenter (const float3 offset) __NE___
	{
		CHECK_ERR( _attribs and _vertexStride > 0 and _vertices.size() );

		auto	positions = _attribs->GetData< packed_float3 >( VertexAttributeName::Position, _vertices.data(),
																VertexCount(), _vertexStride );
		if ( positions.empty() )
			return false;

		for (auto& pos : positions)
		{
			ConstCast( pos ) += offset;
		}

		if ( _boundingBox.has_value() )
			CHECK( CalcAABB() );

		if ( _boundingSphere.has_value() )
			CHECK( CalcSphere() );

		return true;
	}

	bool  IntermMesh::Recenter () __NE___
	{
		if ( not _boundingBox.has_value() )
			CHECK_ERR( CalcAABB() );

		return Recenter( -_boundingBox->Center() );
	}


} // AE::ResLoader
