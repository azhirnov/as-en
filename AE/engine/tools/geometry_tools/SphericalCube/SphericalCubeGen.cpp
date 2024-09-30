// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "geometry_tools/SphericalCube/SphericalCubeGen.h"
#include "geometry_tools/Utils/TBN.h"

namespace AE::GeometryTools
{
namespace
{

	ND_ static uint  GetSCLod (uint lod) __NE___
	{
		return lod * 2 - 1;
	}

/*
=================================================
	RotateTangent
=================================================
*/
	ND_ inline double3  RotateTangent (const double3 &c, ECubeFace face) __NE___
	{
		switch ( face )
		{
			case ECubeFace::XPos : return double3( c.x,  c.y,  c.z);	// X+	| +z
			case ECubeFace::XNeg : return double3(-c.x,  c.y, -c.z);	// X-	| -z
			case ECubeFace::YPos : return double3( c.z,  c.y, -c.x);	// Y+	| +x
			case ECubeFace::YNeg : return double3( c.z,  c.y, -c.x);	// Y-	| +x
			case ECubeFace::ZPos : return double3(-c.z,  c.y,  c.x);	// Z+	| -x
			case ECubeFace::ZNeg : return double3( c.z,  c.y, -c.x);	// Z-	| +x
		}
		return {};
	}

/*
=================================================
	RotateBiTangent
=================================================
*/
	ND_ inline double3  RotateBiTangent (const double3 &c, ECubeFace face) __NE___
	{
		switch ( face )
		{
			case ECubeFace::XPos : return double3( c.x,  c.z, -c.y);	// X+	| -y
			case ECubeFace::XNeg : return double3( c.x,  c.z, -c.y);	// X-	| -y
			case ECubeFace::YPos : return double3(-c.x,  c.y, -c.z);	// Y+	| -z
			case ECubeFace::YNeg : return double3( c.x,  c.y,  c.z);	// Y-	| +z
			case ECubeFace::ZPos : return double3( c.x,  c.z, -c.y);	// Z+	| -y
			case ECubeFace::ZNeg : return double3( c.x,  c.z, -c.y);	// Z-	| -y
		}
		return {};
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	Calc***Count
=================================================
*/
	uint2  SphericalCubeGen::CalcFaceVertCount2 (uint lod) __NE___
	{
		// on X/Y axis
		lod = GetSCLod( lod );
		return uint2{ lod+2 };
	}

	uint  SphericalCubeGen::CalcFaceVertCount (uint lod) __NE___
	{
		lod = GetSCLod( lod );
		return (lod+2) * (lod+2);
	}

	uint  SphericalCubeGen::CalcVertCount (const uint lod) __NE___
	{
		return CalcFaceVertCount( lod ) * FaceCount;
	}

	uint  SphericalCubeGen::CalcFaceIndexCount (uint lod, const Bool useQuads) __NE___
	{
		lod = GetSCLod( lod );
		return (useQuads ? 4 : 6) * (lod+1) * (lod+1);
	}

	uint  SphericalCubeGen::CalcIndexCount (const uint lod, const Bool useQuads) __NE___
	{
		return CalcFaceIndexCount( lod, useQuads ) * FaceCount;
	}

/*
=================================================
	Create
----
	lod = 0 -- cube, only 8 vertices, 6*6 indices
	lod = x -- has {(x+1)*(x+2)*4 + x*x*2} vertices, (6*6*(x+1)^2) indices
=================================================
*/
	bool  SphericalCubeGen::Create (const uint minLod, const uint maxLod, const Bool quads, const Bool cubeMap) __NE___
	{
		CHECK_ERR( minLod <= maxLod );
		CHECK_ERR( maxLod < MaxLod );

		_minLod = minLod;	_maxLod = maxLod;
		_quads = quads;

		// calculate total memory size
		ulong	vert_count	= 0;
		ulong	index_count	= 0;

		for (uint lod = minLod; lod <= maxLod; ++lod)
		{
			vert_count  += CalcVertCount( lod );
			index_count += CalcIndexCount( lod, Bool{_quads} );
		}

		_vertices.resize( vert_count );
		_indices.resize( index_count );

		vert_count = index_count = 0;


		for (uint lod = minLod; lod <= maxLod; ++lod)
		{
			Vertex *	dst_vertices	= &_vertices[ vert_count ];
			Index *		dst_indices		= &_indices[ index_count ];

			vert_count  += CalcVertCount( lod );
			index_count += CalcIndexCount( lod, Bool{_quads} );

			// for cube sides
			const uint	vcount	= GetSCLod( lod ) + 2;
			const uint	icount	= GetSCLod( lod ) + 1;
			uint		vert_i	= 0;
			uint		index_i	= 0;

			// for top/bottom faces
			for (uint face = 0; face < 6; ++face)
			{
				// generate indices
				for (uint y = 0; y < icount; ++y)
				for (uint x = 0; x < icount; ++x)
				{
					const Index	indices[4] = { vert_i + (x+0) + (y+0)*vcount, vert_i + (x+1) + (y+0)*vcount,
											   vert_i + (x+0) + (y+1)*vcount, vert_i + (x+1) + (y+1)*vcount };

					if ( quads )
					{
						dst_indices[index_i++] = indices[0];	dst_indices[index_i++] = indices[1];
						dst_indices[index_i++] = indices[3];	dst_indices[index_i++] = indices[2];
					}
					else
					{
						if ( (x < icount/2 and y < icount/2) or (x >= icount/2 and y >= icount/2) )
						{
							dst_indices[index_i++] = indices[0];	dst_indices[index_i++] = indices[3];	dst_indices[index_i++] = indices[1];
							dst_indices[index_i++] = indices[0];	dst_indices[index_i++] = indices[2];	dst_indices[index_i++] = indices[3];
						}
						else
						if ( (x >= icount/2 and y < icount/2) or (x < icount/2 and y >= icount/2) )
						{
							dst_indices[index_i++] = indices[0];	dst_indices[index_i++] = indices[2];	dst_indices[index_i++] = indices[1];
							dst_indices[index_i++] = indices[2];	dst_indices[index_i++] = indices[3];	dst_indices[index_i++] = indices[1];
						}
					}
				}

				// generate vertices
				for (uint y = 0; y < vcount; ++y)
				for (uint x = 0; x < vcount; ++x)
				{
					double2		ncoord		= ToSNorm( double2{uint2{ x, y }} / double(vcount-1) );
					auto&		vert		= dst_vertices[vert_i++];
					double3		pos			= ForwardProjection( ncoord, ECubeFace(face) );
					double2		proj		= VertexProjection_t::Forward( ncoord );
					double3		texc		= ForwardTexProjection( ncoord, ECubeFace(face) );

					// from https://iquilezles.org/articles/patchedsphere/
					double3		tangent		= VertexProjection_t::Forward2( -Normalize( double3{ 1.0 + Square(proj.y), -proj.x * proj.y, -proj.x }), ECubeFace(face) );
					double3		bitangent	= VertexProjection_t::Forward2( Normalize( double3{ -proj.x * proj.y, 1.0 + Square(proj.x), -proj.y }), ECubeFace(face) );

					if ( AnyEqual( ECubeFace(face), ECubeFace::YPos, ECubeFace::YNeg ))
					{
						tangent		= -tangent;
						bitangent	= -bitangent;
					}

					//CheckTBN( float3{pos}, float3{tangent}, float3{bitangent} );

					vert.position	= FloatToSNormShort(float4{ pos, float(face)/5.f });
					vert.texcoord	= FloatToSNormShort(float4{ texc, 0.f });
					vert.tangent	= FloatToSNormShort(float4{ tangent, 0.f });
					vert.bitangent	= FloatToSNormShort(float4{ bitangent, 0.f });

					if ( not cubeMap )
					{
						if ( AnyEqual( ECubeFace(face), ECubeFace::YPos, ECubeFace::YNeg ))
							ncoord.y = -ncoord.y;
						else
							ncoord.x = -ncoord.x;

						vert.texcoord	= FloatToSNormShort(float4{ ToUNorm( ncoord ), 0.f, 0.f });
					}
				}
			}
		}

		return true;
	}

/*
=================================================
	GetVertices
=================================================
*/
	bool  SphericalCubeGen::GetVertices (const uint lod, OUT ArrayView<Vertex> &result) C_NE___
	{
		CHECK_ERR( lod >= _minLod and lod <= _maxLod );

		const uint	face_vert_count	= CalcFaceVertCount( lod );
		uint		vert_offset		= 0;

		for (uint i = _minLod; i < lod; ++i) {
			vert_offset += CalcVertCount( i );
		}

		result = ArrayView<Vertex>{_vertices}.section( vert_offset, face_vert_count * FaceCount );
		return true;
	}

/*
=================================================
	GetVertices
=================================================
*/
	bool  SphericalCubeGen::GetVertices (const uint lod, const uint face, OUT ArrayView<Vertex> &result) C_NE___
	{
		CHECK_ERR( lod >= _minLod and lod <= _maxLod );
		CHECK_ERR( face < FaceCount );

		const uint	face_vert_count	= CalcFaceVertCount( lod );
		uint		vert_offset		= 0;

		for (uint i = _minLod; i < lod; ++i) {
			vert_offset += CalcVertCount( i );
		}

		vert_offset += (face_vert_count * face);

		result = ArrayView<Vertex>{_vertices}.section( vert_offset, face_vert_count );
		return true;
	}

/*
=================================================
	GetIndices
=================================================
*/
	bool  SphericalCubeGen::GetIndices (const uint lod, OUT ArrayView<Index> &result) C_NE___
	{
		CHECK_ERR( lod >= _minLod and lod <= _maxLod );

		const uint	face_idx_count	= CalcFaceIndexCount( lod, Bool{_quads} );
		uint		idx_offset		= 0;

		for (uint i = _minLod; i < lod; ++i) {
			idx_offset += CalcIndexCount( i, Bool{_quads} );
		}

		result = ArrayView<Index>{_indices}.section( idx_offset, face_idx_count * FaceCount );
		return true;
	}

/*
=================================================
	GetIndices
=================================================
*/
	bool  SphericalCubeGen::GetIndices (const uint lod, const uint face, OUT ArrayView<Index> &result) C_NE___
	{
		CHECK_ERR( lod >= _minLod and lod <= _maxLod );
		CHECK_ERR( face < FaceCount );

		const uint	face_idx_count	= CalcFaceIndexCount( lod, Bool{_quads} );
		uint		idx_offset		= 0;

		for (uint i = _minLod; i < lod; ++i) {
			idx_offset += CalcIndexCount( i, Bool{_quads} );
		}

		idx_offset += (face_idx_count * face);

		result = ArrayView<Index>{_indices}.section( idx_offset, face_idx_count );
		return true;
	}

/*
=================================================
	RayCast
----
	from http://paulbourke.net/geometry/circlesphere/index.html#linesphere
=================================================
*/
	bool  SphericalCubeGen::RayCast (const float3 &center, const float radius, const float3 &begin, const float3 &end, OUT float3 &outIntersection) __NE___
	{
		float	a = Square(end.x - begin.x) + Square(end.y - begin.y) + Square(end.z - begin.z);

		float	b = 2.0f * ( (end.x - begin.x)*(begin.x - center.x) + (end.y - begin.y)*(begin.y - center.y) + (end.z - begin.z)*(begin.z - center.z) );

		float	c = Square(center.x) + Square(center.y) + Square(center.z) + Square(begin.x) + Square(begin.y) + Square(begin.z) -
					2.0f * ( center.x*begin.x + center.y*begin.y + center.z*begin.z ) - Square(radius);

		float	i = b * b - 4.f * a * c;

		// no intersection
		if ( i < 0.0f )
			return false;

		// one intersection
		if ( IsZero( i ))
		{
			float	mu	= -b / (2.0f * a);

			if ( (mu < 0.0001f) and (mu > 1.0f) )
				return false;

			outIntersection = begin + mu * (end - begin);
			return true;
		}

		// two intersections
		float	mu1			= (-b + Sqrt( Square(b) - 4.0f * a * c )) / (2.0f * a);
		bool	mu1_valid	= (mu1 > -0.0001f) and (mu1 < 1.0001f);
		float	mu2			= (-b - Sqrt( Square(b) - 4.0f * a * c )) / (2.0f * a);
		bool	mu2_valid	= (mu2 > -0.0001f) and (mu2 < 1.0001f);
		float	mu;

		if ( mu1_valid and mu2_valid )	mu = Min( mu1, mu2 );	else
		if ( mu1_valid )				mu = mu1;				else
		if ( mu2_valid )				mu = mu2;				else
										return false;

		outIntersection = begin + mu * (end - begin);
		return true;
	}


} // AE::GeometryTools
