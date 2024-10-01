// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "geometry_tools/Cube/CubeGen.h"
#include "geometry_tools/Grid/GridGen.h"
#include "geometry_tools/Cylinder/CylinderGen.h"
#include "geometry_tools/SphericalCube/SphericalCubeGen.h"
#include "geometry_tools/Triangulation/CDT_Triangulator.h"

#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{

/*
=================================================
	_GetCube*
=================================================
*/
	void  ScriptExe::_GetCube2 (OUT ScriptArray<packed_float3>	&positions,
								OUT ScriptArray<packed_float3>	&normals,
								OUT ScriptArray<uint>			&indices) __Th___
	{
		positions.clear();
		normals.clear();
		indices.clear();

		GeometryTools::CubeGen	cube;
		CHECK_THROW( cube.Create( True{} ));

		for (auto& vert : cube.GetVertices())
		{
			positions.push_back( float3{SNormShortToFloat( vert.position )});
			normals  .push_back( float3{SNormShortToFloat( vert.normal )});
		}

		for (auto idx : cube.GetIndices()) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetCube3 (OUT ScriptArray<packed_float3>	&positions,
							    OUT ScriptArray<packed_float3>	&normals,
							    OUT ScriptArray<packed_float3>	&tangents,
							    OUT ScriptArray<packed_float3>	&bitangents,
							    OUT ScriptArray<packed_float2>	&texcoords,
							    OUT ScriptArray<uint>			&indices) __Th___
	{
		positions .clear();
		normals   .clear();
		tangents  .clear();
		bitangents.clear();
		texcoords .clear();
		indices   .clear();

		GeometryTools::CubeGen	cube;
		CHECK_THROW( cube.Create( False{"2d"} ));

		for (auto& vert : cube.GetVertices())
		{
			positions .push_back( float3{SNormShortToFloat( vert.position )});
			normals   .push_back( float3{SNormShortToFloat( vert.normal )});
			tangents  .push_back( float3{SNormShortToFloat( vert.tangent )});
			bitangents.push_back( float3{SNormShortToFloat( vert.bitangent )});
			texcoords .push_back( float2{SNormShortToFloat( vert.texcoord )});
		}

		for (auto idx : cube.GetIndices()) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetCube4 (OUT ScriptArray<packed_float3>	&positions,
							    OUT ScriptArray<packed_float3>	&normals,
							    OUT ScriptArray<packed_float3>	&tangents,
							    OUT ScriptArray<packed_float3>	&bitangents,
							    OUT ScriptArray<packed_float3>	&texcoords,
							    OUT ScriptArray<uint>			&indices) __Th___
	{
		positions .clear();
		normals   .clear();
		tangents  .clear();
		bitangents.clear();
		texcoords .clear();
		indices   .clear();

		GeometryTools::CubeGen	cube;
		CHECK_THROW( cube.Create( True{"cubeMap"} ));

		for (auto& vert : cube.GetVertices())
		{
			positions .push_back( float3{SNormShortToFloat( vert.position )});
			normals   .push_back( float3{SNormShortToFloat( vert.normal )});
			tangents  .push_back( float3{SNormShortToFloat( vert.tangent )});
			bitangents.push_back( float3{SNormShortToFloat( vert.bitangent )});
			texcoords .push_back( float3{SNormShortToFloat( vert.texcoord )});
		}

		for (auto idx : cube.GetIndices()) {
			indices.push_back( idx );
		}
	}

/*
=================================================
	_GetSphere*
=================================================
*/
	void  ScriptExe::_GetSphere1 (const uint						lod,
								  OUT ScriptArray<packed_float3>	&positions,
								  OUT ScriptArray<uint>				&indices) __Th___
	{
		positions.clear();
		indices  .clear();

		GeometryTools::SphericalCubeGen	sphere;
		CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

		ArrayView<GeometryTools::SphericalCubeGen::Vertex>	verts;
		CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

		ArrayView<GeometryTools::SphericalCubeGen::Index>	idxs;
		CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

		positions.reserve( verts.size() );
		indices  .reserve( idxs.size() );

		for (auto& vert : verts) {
			positions.push_back( float3{SNormShortToFloat( vert.position )});
		}
		for (auto idx : idxs) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetSphere2 (const uint						lod,
								  OUT ScriptArray<packed_float3>	&positions,
								  OUT ScriptArray<packed_float3>	&texcoords,
								  OUT ScriptArray<uint>				&indices) __Th___
	{
		positions.clear();
		texcoords.clear();
		indices  .clear();

		GeometryTools::SphericalCubeGen	sphere;
		CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

		ArrayView<GeometryTools::SphericalCubeGen::Vertex>	verts;
		CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

		ArrayView<GeometryTools::SphericalCubeGen::Index>	idxs;
		CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

		positions.reserve( verts.size() );
		texcoords.reserve( verts.size() );
		indices  .reserve( idxs.size() );

		for (auto& vert : verts) {
			positions.push_back( float3{SNormShortToFloat( vert.position )});
			texcoords.push_back( float3{SNormShortToFloat( vert.texcoord )});
		}
		for (auto idx : idxs) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetSphere3 (const uint						lod,
								  OUT ScriptArray<packed_float3>	&positions,
								  OUT ScriptArray<packed_float3>	&normals,
							      OUT ScriptArray<packed_float3>	&tangents,
							      OUT ScriptArray<packed_float3>	&bitangents,
							      OUT ScriptArray<packed_float3>	&texcoords,
							      OUT ScriptArray<uint>				&indices) __Th___
	{
		positions .clear();
		normals   .clear();
		tangents  .clear();
		bitangents.clear();
		texcoords .clear();
		indices   .clear();

		GeometryTools::SphericalCubeGen	sphere;
		CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

		ArrayView<GeometryTools::SphericalCubeGen::Vertex>	verts;
		CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

		ArrayView<GeometryTools::SphericalCubeGen::Index>	idxs;
		CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

		positions .reserve( verts.size() );
		normals   .reserve( verts.size() );
		tangents  .reserve( verts.size() );
		bitangents.reserve( verts.size() );
		texcoords .reserve( verts.size() );
		indices   .reserve( idxs.size() );

		for (auto& vert : verts) {
			positions .push_back( float3{SNormShortToFloat( vert.position )});
			normals   .push_back( float3{SNormShortToFloat( vert.position )});
			texcoords .push_back( float3{SNormShortToFloat( vert.texcoord )});
			tangents  .push_back( float3{SNormShortToFloat( vert.tangent )});
			bitangents.push_back( float3{SNormShortToFloat( vert.bitangent )});
		}
		for (auto idx : idxs) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetSphere4 (const uint						lod,
								  OUT ScriptArray<packed_float3>	&positions,
								  OUT ScriptArray<packed_float3>	&normals,
							      OUT ScriptArray<packed_float3>	&tangents,
							      OUT ScriptArray<packed_float3>	&bitangents,
							      OUT ScriptArray<packed_float2>	&texcoords,
							      OUT ScriptArray<uint>				&indices) __Th___
	{
		positions .clear();
		normals   .clear();
		tangents  .clear();
		bitangents.clear();
		texcoords .clear();
		indices   .clear();

		GeometryTools::SphericalCubeGen	sphere;
		CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, False{"2d"} ));

		ArrayView<GeometryTools::SphericalCubeGen::Vertex>	verts;
		CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

		ArrayView<GeometryTools::SphericalCubeGen::Index>	idxs;
		CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

		positions .reserve( verts.size() );
		normals   .reserve( verts.size() );
		tangents  .reserve( verts.size() );
		bitangents.reserve( verts.size() );
		texcoords .reserve( verts.size() );
		indices   .reserve( idxs.size() );

		for (auto& vert : verts) {
			positions .push_back( float3{SNormShortToFloat( vert.position )});
			normals   .push_back( float3{SNormShortToFloat( vert.position )});
			texcoords .push_back( float2{SNormShortToFloat( vert.texcoord )});
			tangents  .push_back( float3{SNormShortToFloat( vert.tangent )});
			bitangents.push_back( float3{SNormShortToFloat( vert.bitangent )});
		}
		for (auto idx : idxs) {
			indices.push_back( idx );
		}
	}

/*
=================================================
	_GetGrid*
=================================================
*/
	void  ScriptExe::_GetGrid1 (const uint						size,
								OUT ScriptArray<packed_float2>	&positions,
								OUT ScriptArray<uint>			&indices) __Th___
	{
		positions.clear();
		indices  .clear();

		GeometryTools::GridGen	grid;
		CHECK_THROW( grid.Create( size, 3u ));

		positions.reserve( grid.GetVertices().size() );
		indices  .reserve( grid.GetIndices().size() );

		for (auto& vert : grid.GetVertices()) {
			positions.push_back( vert.uv );
		}

		for (auto idx : grid.GetIndices()) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetGrid2 (const uint						size,
								OUT ScriptArray<packed_float3>	&positions,
								OUT ScriptArray<uint>			&indices) __Th___
	{
		positions.clear();
		indices  .clear();

		GeometryTools::GridGen	grid;
		CHECK_THROW( grid.Create( size, 3u ));

		positions.reserve( grid.GetVertices().size() );
		indices  .reserve( grid.GetIndices().size() );

		for (auto& vert : grid.GetVertices()) {
			positions.push_back(packed_float3{ vert.uv.x, vert.uv.y, 0.f });
		}

		for (auto idx : grid.GetIndices()) {
			indices.push_back( idx );
		}
	}

/*
=================================================
	_GetCylinder*
=================================================
*/
	void  ScriptExe::_GetCylinder1 (const uint						segments,
									const bool						inner,
									OUT ScriptArray<packed_float3>	&positions,
									OUT ScriptArray<packed_float2>	&texcoords,
									OUT ScriptArray<uint>			&indices) __Th___
	{
		positions.clear();
		texcoords.clear();
		indices  .clear();

		GeometryTools::CylinderGen	cylinder;
		CHECK_THROW( cylinder.Create( segments, Bool{inner} ));

		positions.reserve( cylinder.GetVertices().size() );
		texcoords.reserve( cylinder.GetVertices().size() );
		indices  .reserve( cylinder.GetIndices().size() );

		for (auto& vert : cylinder.GetVertices()) {
			positions.push_back( float3{SNormShortToFloat( vert.position )});
			texcoords.push_back( float2{SNormShortToFloat( vert.texcoord )});
		}
		for (auto idx : cylinder.GetIndices()) {
			indices.push_back( idx );
		}
	}

	void  ScriptExe::_GetCylinder2 (const uint						segments,
									const bool						inner,
									OUT ScriptArray<packed_float3>	&positions,
									OUT ScriptArray<packed_float3>	&normals,
									OUT ScriptArray<packed_float3>	&tangents,
									OUT ScriptArray<packed_float3>	&bitangents,
									OUT ScriptArray<packed_float2>	&texcoords,
									OUT ScriptArray<uint>			&indices) __Th___
	{
		positions .clear();
		normals   .clear();
		tangents  .clear();
		bitangents.clear();
		texcoords .clear();
		indices   .clear();

		GeometryTools::CylinderGen	cylinder;
		CHECK_THROW( cylinder.Create( segments, Bool{inner} ));

		positions .reserve( cylinder.GetVertices().size() );
		normals   .reserve( cylinder.GetVertices().size() );
		texcoords .reserve( cylinder.GetVertices().size() );
		tangents  .reserve( cylinder.GetVertices().size() );
		bitangents.reserve( cylinder.GetVertices().size() );
		indices   .reserve( cylinder.GetIndices().size() );

		for (auto& vert : cylinder.GetVertices()) {
			positions .push_back( float3{SNormShortToFloat( vert.position )});
			normals   .push_back( float3{SNormShortToFloat( vert.normal )});
			texcoords .push_back( float2{SNormShortToFloat( vert.texcoord )});
			tangents  .push_back( float3{SNormShortToFloat( vert.tangent )});
			bitangents.push_back( float3{SNormShortToFloat( vert.bitangent )});
		}
		for (auto idx : cylinder.GetIndices()) {
			indices.push_back( idx );
		}
	}

/*
=================================================
	_IndicesToPrimitives
=================================================
*/
	void  ScriptExe::_IndicesToPrimitives (const ScriptArray<uint>			&indices,
										   OUT ScriptArray<packed_uint3>	&primitives) __Th___
	{
		primitives.clear();
		primitives.reserve( indices.size()/3 );

		for (usize i = 0; i < indices.size(); i += 3) {
			primitives.emplace_back( indices[i+0], indices[i+1], indices[i+2] );
		}
	}

/*
=================================================
	_GetFrustumPlanes
=================================================
*/
	void  ScriptExe::_GetFrustumPlanes (const packed_float4x4			&viewProj,
										OUT ScriptArray<packed_float4>	&planes) __Th___
	{
		planes.clear();
		planes.resize( 6 );

		TFrustum<float>		frustum;
		frustum.Setup( float4x4{viewProj} );

		for (uint i = 0; i < 6; ++i) {
			planes[i] = float4{frustum.GetPlane( i )};
		}
	}

/*
=================================================
	_MergeMesh
=================================================
*/
	void  ScriptExe::_MergeMesh (INOUT ScriptArray<uint>	&srcIndices,
								 const uint					srcVertexCount,
								 const ScriptArray<uint>	&indicesToAdd) __Th___
	{
		const usize	old_size = srcIndices.size();

		srcIndices.resize( old_size + indicesToAdd.size() );  // throw

		for (usize i = 0, cnt = indicesToAdd.size(); i < cnt; ++i)
		{
			srcIndices[old_size + i] = srcVertexCount + indicesToAdd[i];
		}
	}

/*
=================================================
	_ExtrudeAndMerge
=================================================
*/
	void  ScriptExe::_ExtrudeAndMerge (const ScriptArray<packed_float2>	&lineStrip,
									   float							height,
									   INOUT ScriptArray<packed_float3>	&outPositions,
									   INOUT ScriptArray<uint>			&outIndices) __Th___
	{
		CHECK_THROW_MSG( lineStrip.size() >= 3 );
		CHECK_THROW_MSG( height > 0.f );
		height *= 0.5f;

		const usize		base_v		= outPositions.size();
		const usize		base_idx	= outIndices.size();

		outPositions.resize( base_v + (lineStrip.size()+1)*2 );
		outIndices.resize( base_idx + lineStrip.size()*6 );

		for (usize i = 0; i <= lineStrip.size(); ++i)
		{
			const usize	i0	= base_v + (i == lineStrip.size() ? 0 : i) * 2;
			const usize	i1	= base_v + i0 + 1;
			const auto&	c	= lineStrip[ i == lineStrip.size() ? 0 : i ];

			outPositions[i0] = float3{ c.x, -height, c.y };
			outPositions[i1] = float3{ c.x,  height, c.y };
		}

		for (usize i = 0; i < lineStrip.size(); ++i)
		{
			const usize	j	= base_idx + i * 6;
			const uint	i0	= uint(base_v + i * 2);
			const uint	i2	= uint(base_v + (i+1 == lineStrip.size() ? 0 : i+1) * 2);

			ASSERT( i0+1 < outPositions.size() );
			ASSERT( i2+1 < outPositions.size() );

			outIndices[j+0] = i0;
			outIndices[j+1] = i0+1;
			outIndices[j+2] = i2;

			outIndices[j+3] = i2;
			outIndices[j+4] = i0+1;
			outIndices[j+5] = i2+1;
		}
	}

#ifdef AE_ENABLE_CDT
/*
=================================================
	_TriangulateAndMerge1
=================================================
*/
	void  ScriptExe::_TriangulateAndMerge1 (const ScriptArray<packed_float2>	&lineStrip,
										    const float							yCoord,
										    INOUT ScriptArray<packed_float3>	&outPositions,
										    INOUT ScriptArray<uint>				&outIndices) __Th___
	{
		using EFlags = GeometryTools::CDT_Triangulator::EFlags;
		constexpr auto	flags =	EFlags::ConformingDelaunayTriangulation |
								EFlags::ResolveIntersections |
								EFlags::LineStrip;

		CHECK_THROW_MSG( lineStrip.size() >= 3 );

		Array<float2>	tmp_vertices;
		Array<uint>		tmp_indices;
		{
			Array<float2>	line_strip;
			line_strip.resize( lineStrip.size() );

			for (usize i : IndicesOnly( line_strip )) {
				line_strip[i] = lineStrip[i];
			}

			CHECK_THROW_MSG( GeometryTools::CDT_Triangulator::Triangulate( line_strip, flags, OUT tmp_vertices, OUT tmp_indices ),
				"triangulation failed" );
		}
		_ImplTriangulateAndMerge( tmp_vertices, tmp_indices, yCoord, INOUT outPositions, INOUT outIndices );
	}

	void  ScriptExe::_TriangulateAndMerge2 (const ScriptArray<packed_float2>	&inVertices,
										    const ScriptArray<uint>				&inLineListIndices,
										    const float							yCoord,
										    INOUT ScriptArray<packed_float3>	&outPositions,
										    INOUT ScriptArray<uint>				&outIndices) __Th___
	{
		using EFlags = GeometryTools::CDT_Triangulator::EFlags;
		constexpr auto	flags =	EFlags::ConformingDelaunayTriangulation |
								EFlags::ResolveIntersections |
								EFlags::IndexedLineList;

		CHECK_THROW_MSG( inVertices.size() >= 3 );
		CHECK_THROW_MSG( inLineListIndices.size() >= 6 );

		Array<float2>	tmp_vertices;
		Array<uint>		tmp_indices;
		{
			Array<float2>	vertices;
			vertices.resize( inVertices.size() );

			for (usize i : IndicesOnly( vertices )) {
				vertices[i] = inVertices[i];
			}

			CHECK_THROW_MSG( GeometryTools::CDT_Triangulator::Triangulate( vertices, ArrayView<uint>{inLineListIndices}, flags, OUT tmp_vertices, OUT tmp_indices ),
				"triangulation failed" );
		}
		_ImplTriangulateAndMerge( tmp_vertices, tmp_indices, yCoord, INOUT outPositions, INOUT outIndices );
	}

	void  ScriptExe::_ImplTriangulateAndMerge (ArrayView<float2>				tmp_vertices,
											   ArrayView<uint>					tmp_indices,
											   const float						yCoord,
											   INOUT ScriptArray<packed_float3>	&outPositions,
											   INOUT ScriptArray<uint>			&outIndices) __Th___
	{
		const usize		base_v		= outPositions.size();
		const usize		base_idx	= outIndices.size();

		outPositions.resize( base_v + tmp_vertices.size() );
		outIndices.resize( base_idx + tmp_indices.size() );

		for (usize i : IndicesOnly( tmp_vertices ))
		{
			outPositions[ base_v+i ] = float3{ tmp_vertices[i].x, yCoord, tmp_vertices[i].y };
		}

		for (usize i : IndicesOnly( tmp_indices ))
		{
			outIndices[ base_idx+i ] = uint(base_v + tmp_indices[i]);
		}
	}

/*
=================================================
	_TriangulateExtrudeAndMerge*
=================================================
*/
	void  ScriptExe::_TriangulateExtrudeAndMerge1 (const ScriptArray<packed_float2>	&lineStrip,
												   float							height,
												   INOUT ScriptArray<packed_float3>	&outPositions,
												   INOUT ScriptArray<uint>			&outIndices) __Th___
	{
		using EFlags = GeometryTools::CDT_Triangulator::EFlags;
		constexpr auto	flags =	EFlags::ConformingDelaunayTriangulation |
								EFlags::ResolveIntersections |
								EFlags::LineStrip;

		CHECK_THROW_MSG( lineStrip.size() >= 3 );

		Array<float2>	tmp_vertices;
		Array<uint>		tmp_indices;
		Array<uint>		tmp_boundary;
		{
			Array<float2>	line_strip;
			line_strip.resize( lineStrip.size() );

			for (usize i : IndicesOnly( line_strip )) {
				line_strip[i] = lineStrip[i];
			}

			CHECK_THROW_MSG( GeometryTools::CDT_Triangulator::Triangulate( line_strip, flags,
																		   OUT tmp_vertices, OUT tmp_indices, OUT &tmp_boundary ),
				"triangulation failed" );
			CHECK_THROW( IsMultipleOf( tmp_boundary.size(), 2 ) and tmp_boundary.size() >= 2 );
		}

		_ImplTriangulateExtrudeAndMerge( tmp_vertices, tmp_indices, tmp_boundary, height, INOUT outPositions, INOUT outIndices );
	}

	void  ScriptExe::_TriangulateExtrudeAndMerge2 (const ScriptArray<packed_float2>	&inVertices,
												   const ScriptArray<uint>			&inLineListIndices,
												   float							height,
												   INOUT ScriptArray<packed_float3>	&outPositions,
												   INOUT ScriptArray<uint>			&outIndices) __Th___
	{
		using EFlags = GeometryTools::CDT_Triangulator::EFlags;
		constexpr auto	flags =	EFlags::ConformingDelaunayTriangulation |
								EFlags::ResolveIntersections |
								EFlags::IndexedLineList;

		CHECK_THROW_MSG( inVertices.size() >= 3 );
		CHECK_THROW_MSG( inLineListIndices.size() >= 6 );

		Array<float2>	tmp_vertices;
		Array<uint>		tmp_indices;
		Array<uint>		tmp_boundary;
		{
			Array<float2>	vertices;
			vertices.resize( inVertices.size() );

			for (usize i : IndicesOnly( vertices )) {
				vertices[i] = inVertices[i];
			}

			CHECK_THROW_MSG( GeometryTools::CDT_Triangulator::Triangulate( vertices, ArrayView<uint>{inLineListIndices}, flags,
																		   OUT tmp_vertices, OUT tmp_indices, OUT &tmp_boundary ),
				"triangulation failed" );

			CHECK_THROW( IsMultipleOf( tmp_boundary.size(), 2 ) and tmp_boundary.size() >= 2 );
		}

		_ImplTriangulateExtrudeAndMerge( tmp_vertices, tmp_indices, tmp_boundary, height, INOUT outPositions, INOUT outIndices );
	}

	void  ScriptExe::_ImplTriangulateExtrudeAndMerge (ArrayView<float2>					tmp_vertices,
													  ArrayView<uint>					tmp_indices,
													  ArrayView<uint>					tmp_boundary,
													  float								height,
													  INOUT ScriptArray<packed_float3>	&outPositions,
													  INOUT ScriptArray<uint>			&outIndices) __Th___
	{
		CHECK_THROW_MSG( height > 0.f );
		height *= 0.5f;

		const usize		base1_v		= outPositions.size();
		const usize		base1_idx	= outIndices.size();

		const usize		base2_v		= outPositions.size() + tmp_vertices.size();
		const usize		base2_idx	= outIndices.size() + tmp_indices.size();

		const usize		base3_idx	= outIndices.size()+ tmp_indices.size()*2;

		outPositions.resize( base1_v + tmp_vertices.size()*2 );
		outIndices.resize( base1_idx + tmp_indices.size()*2 + tmp_boundary.size()*3 );

		for (usize i : IndicesOnly( tmp_vertices ))
		{
			outPositions[ base1_v+i ] = float3{ tmp_vertices[i].x, -height, tmp_vertices[i].y };
			outPositions[ base2_v+i ] = float3{ tmp_vertices[i].x,  height, tmp_vertices[i].y };
		}

		for (usize i = 0; i < tmp_indices.size(); i += 3)
		{
			outIndices[ base1_idx+i+0 ] = uint(base1_v + tmp_indices[i+0]);
			outIndices[ base1_idx+i+1 ] = uint(base1_v + tmp_indices[i+1]);
			outIndices[ base1_idx+i+2 ] = uint(base1_v + tmp_indices[i+2]);

			outIndices[ base2_idx+i+0 ] = uint(base2_v + tmp_indices[i+2]);
			outIndices[ base2_idx+i+1 ] = uint(base2_v + tmp_indices[i+1]);
			outIndices[ base2_idx+i+2 ] = uint(base2_v + tmp_indices[i+0]);
		}

		for (usize i = 0, j = 0; i < tmp_boundary.size(); i += 2, j += 6)
		{
			const uint	e0 = tmp_boundary[i+0];
			const uint	e1 = tmp_boundary[i+1];

			outIndices[ base3_idx+j+0 ] = uint(base1_v + e0);
			outIndices[ base3_idx+j+1 ] = uint(base2_v + e0);
			outIndices[ base3_idx+j+2 ] = uint(base2_v + e1);

			outIndices[ base3_idx+j+3 ] = uint(base2_v + e1);
			outIndices[ base3_idx+j+4 ] = uint(base1_v + e1);
			outIndices[ base3_idx+j+5 ] = uint(base1_v + e0);
		}
	}
#endif // AE_ENABLE_CDT

/*
=================================================
	_GetSphericalCube1
=================================================
*/
	void  ScriptExe::_GetSphericalCube1 (uint							lod,
										 OUT ScriptArray<packed_float3>	&positions,
										 OUT ScriptArray<uint>			&indices) __Th___
	{
		positions.clear();
		indices.clear();

		lod = lod*2 - 1;

		// for cube sides
		const uint	vcount	= lod + 2;
		const uint	icount	= lod + 1;
		uint		vert_i	= 0;

		indices.reserve( icount * icount * 6 );
		positions.reserve( vcount * vcount );

		// for top/bottom faces
		for (uint face = 0; face < 6; ++face)
		{
			// generate indices
			for (uint y = 0; y < icount; ++y)
			for (uint x = 0; x < icount; ++x)
			{
				const uint	idx[4] = {	vert_i + (x+0) + (y+0)*vcount, vert_i + (x+1) + (y+0)*vcount,
										vert_i + (x+0) + (y+1)*vcount, vert_i + (x+1) + (y+1)*vcount };

				if ( (x < icount/2 and y < icount/2) or (x >= icount/2 and y >= icount/2) )
				{
					indices.push_back( idx[0] );	indices.push_back( idx[3] );	indices.push_back( idx[1] );
					indices.push_back( idx[0] );	indices.push_back( idx[2] );	indices.push_back( idx[3] );
				}else{
					indices.push_back( idx[0] );	indices.push_back( idx[2] );	indices.push_back( idx[1] );
					indices.push_back( idx[2] );	indices.push_back( idx[3] );	indices.push_back( idx[1] );
				}
			}

			// generate vertices
			for (uint y = 0; y < vcount; ++y)
			for (uint x = 0; x < vcount; ++x)
			{
				float2		ncoord = ToSNorm( float2{uint2{ x, y }} / float(vcount-1) );

				positions.push_back(float3{ ncoord, float(face) });
				++vert_i;
			}
		}
	}


/*
=================================================
	_CM_CubeSC_Forward
	_CM_IdentitySC_Forward
	_CM_TangentialSC_Forward
=================================================
*/
	packed_float3  ScriptExe::_CM_CubeSC_Forward (const packed_float3 &c)
	{
		using namespace AE::GeometryTools;
		return packed_float3{ SCProj2_Cube< SCProj1_Identity >::Forward( double2{float2{c.x, c.y}}, ECubeFace(c.z) )};
	}

	packed_float3  ScriptExe::_CM_IdentitySC_Forward (const packed_float3 &c)
	{
		using namespace AE::GeometryTools;
		return packed_float3{ SCProj2_Spherical< SCProj1_Identity >::Forward( double2{float2{c.x, c.y}}, ECubeFace(c.z) )};
	}

	packed_float3  ScriptExe::_CM_TangentialSC_Forward (const packed_float3 &c)
	{
		using namespace AE::GeometryTools;
		return packed_float3{ SCProj2_Spherical< SCProj1_Tangential >::Forward( double2{float2{c.x, c.y}}, ECubeFace(c.z) )};
	}


} // AE::ResEditor
