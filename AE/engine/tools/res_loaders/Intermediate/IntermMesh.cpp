// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
	bool  IntermMesh::CalcAABB () __NE___
	{
		CHECK_ERR( _attribs and _vertexStride > 0 and _vertices.size() );

		auto	positions = _attribs->GetData< packed_float3 >( VertexAttributeName::Position, _vertices.data(),
																VertexCount(), _vertexStride );
		if ( positions.empty() )
			return false;

		AABB	bbox{ positions[0] };

		for (auto& pos : positions)
		{
			bbox.Add( pos );
		}

		_boundingBox = bbox;
		return true;
	}

/*
=================================================
	CalcSphere
=================================================
*/
	bool  IntermMesh::CalcSphere () __NE___
	{
		CHECK_ERR( _attribs and _vertexStride > 0 and _vertices.size() );

		auto	positions = _attribs->GetData< packed_float3 >( VertexAttributeName::Position, _vertices.data(),
																VertexCount(), _vertexStride );
		if ( positions.empty() )
			return false;

		if ( not _boundingBox.has_value() )
			return false;

		packed_float3	center	= _boundingBox->Center();
		float			radius	= 0.0f;

		for (auto& pos : positions)
		{
			float	r = Distance( pos, center );
			radius = Max( radius, r );
		}

		_boundingSphere = Sphere{ center, radius };
		return true;
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
