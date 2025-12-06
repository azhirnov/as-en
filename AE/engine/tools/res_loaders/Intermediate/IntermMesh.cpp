// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermMesh.h"

namespace AE::ResLoader
{
	using namespace AE::Graphics;

/*
=================================================
	Set
=================================================
*/
	void  IntermMesh::Set (Array<ubyte> vertices, RC<IntermVertexAttribs> attribs,
						   Bytes vertStride, EPrimitive topology,
						   Array<ubyte> indices, EIndex indexType,
						   ENormalEncoding normalEncoding) __NE___
	{
		_vertices		= RVRef(vertices);
		_attribs		= RVRef(attribs);
		_vertexStride	= vertStride;
		_topology		= topology;
		_indices		= RVRef(indices);
		_indexType		= indexType;
		_normalEncoding	= normalEncoding;

		_boundingBox.reset();
		_boundingSphere.reset();
	}

/*
=================================================
	CalcAABB
=================================================
*/
	void  IntermMesh::CalcAABB () __NE___
	{
		CHECK_ERRV( _attribs and _vertexStride > 0 and _vertices.size() );

		auto	positions = _attribs->GetData< packed_float3 >( VertexAttributeName::Position, _vertices.data(),
																VertexCount(), _vertexStride );
		if ( positions.empty() )
			return;

		AABB	bbox{ positions[0] };

		for (size_t i = 1; i < positions.size(); ++i)
		{
			bbox.Add( positions[i] );
		}

		_boundingBox = bbox;
	}

/*
=================================================
	CalcSphere
=================================================
*/
	void  IntermMesh::CalcSphere () __NE___
	{
		CHECK_ERRV( _attribs and _vertexStride > 0 and _vertices.size() );

		auto	positions = _attribs->GetData< packed_float3 >( VertexAttributeName::Position, _vertices.data(),
																VertexCount(), _vertexStride );
		if ( positions.empty() )
			return;

		if ( not _boundingBox.has_value() )
			return;

		packed_float3	center	= _boundingBox->Center();
		float			radius	= 0.0f;

		for (size_t i = 1; i < positions.size(); ++i)
		{
			float	r = Distance( positions[i], center );
			radius = Max( radius, r );
		}

		_boundingSphere = Sphere{ center, radius };
	}

/*
=================================================
	IndexStride
=================================================
*/
	Bytes  IntermMesh::IndexStride () C_NE___
	{
		return EIndex_SizeOf( _indexType );
	}

/*
=================================================
	IsValid
=================================================
*/
	bool  IntermMesh::IsValid () C_NE___
	{
		CHECK_ERR( _attribs );

		/*if (auto normals = GetDataOpt< packed_float3 >( VertexAttributeName::Normal );  not normals.empty() )
		{
			bool	valid = true;
			for (auto& n : normals) {
				valid &= IsNormalized( n );
			}
			CHECK_ERR( valid );
		}*/

		return true;
	}


} // AE::ResLoader
