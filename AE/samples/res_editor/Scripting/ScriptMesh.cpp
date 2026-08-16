// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "geometry_tools/Cube/CubeGen.h"
#include "geometry_tools/Grid/GridGen.h"
#include "geometry_tools/Cone/ConeGen.h"
#include "geometry_tools/Cylinder/CylinderGen.h"
#include "geometry_tools/SphericalCube/SphericalCubeGen.h"
#include "geometry_tools/Triangulation/CDT_Triangulator.h"
#include "geometry_tools/Utils/TBN.h"

#include "Scripting/ScriptMesh.h"
#include "Scripting/ScriptExe.h"

AE_DECL_SCRIPT_TYPE( AE::ResEditor::ScriptMesh::EAttribute,		"EAttribute" );

namespace AE::ResEditor
{

/*
=================================================
	constructor / destructor
=================================================
*/
	ScriptMesh::ScriptMesh () __Th___
	{
		_offsets.fill( UMax );
	}

	ScriptMesh::~ScriptMesh ()
	{
		if ( not _buffer and not _subMesh )
			AE_LOGW( "Unused mesh '"s << _bufferName << "'" );
	}

/*
=================================================
	SetAttributes*
=================================================
*/
	void  ScriptMesh::SetAttributes1 (EAttribute value) __Th___
	{
		CHECK_THROW_MSG( _attribs == Default, "already set attributes" );
		_IsMutable();  // throw

		_attribs = value;
	}

	void  ScriptMesh::SetAttributes2 (uint value) __Th___
	{
		SetAttributes1( EAttribute(value) );
	}

/*
=================================================
	AddUsage
=================================================
*/
	void  ScriptMesh::AddUsage (EResourceUsage usage) __Th___
	{
		if ( AllBits( _bufferUsage, usage ))
			return;

		if ( _buffer )
			return;

		_bufferUsage |= usage;
	}

/*
=================================================
	Add
=================================================
*/
	void  ScriptMesh::Add (const ScriptMeshPtr &mesh) __Th___
	{
		CHECK_THROW_MSG( mesh );

		mesh->_subMesh = true;
		mesh->_attribs |= (_attribs & _GenMask);

		_Add( *mesh );
	}

	void  ScriptMesh::_Add (const ScriptMesh &mesh)
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding sub mesh" );
		CHECK_THROW_MSG( mesh._attribs == _attribs );
		CHECK_THROW_MSG( not mesh._indices.empty() );
		mesh._ValidateSize();  // throw
		_IsMutable();  // throw

		++_subMeshCount;

		const uint	base_vert = uint(_position.size());

		_position	.insert( _position.end(),	mesh._position.begin(),		mesh._position.end() );
		_uv			.insert( _uv.end(),			mesh._uv.begin(),			mesh._uv.end() );
		_uvw		.insert( _uvw.end(),		mesh._uvw.begin(),			mesh._uvw.end() );
		_normal		.insert( _normal.end(),		mesh._normal.begin(),		mesh._normal.end() );
		_tangent	.insert( _tangent.end(),	mesh._tangent.begin(),		mesh._tangent.end() );
		_bitangent	.insert( _bitangent.end(),	mesh._bitangent.begin(),	mesh._bitangent.end() );
		_tbnQuat	.insert( _tbnQuat.end(),	mesh._tbnQuat.begin(),		mesh._tbnQuat.end() );
		_color		.insert( _color.end(),		mesh._color.begin(),		mesh._color.end() );
		_fParams	.insert( _fParams.end(),	mesh._fParams.begin(),		mesh._fParams.end() );
		_iParams	.insert( _iParams.end(),	mesh._iParams.begin(),		mesh._iParams.end() );

		_subMeshId.resize( base_vert + mesh._subMeshId.size() );
		for (usize i = 0; i < mesh._subMeshId.size(); ++i) {
			_subMeshId[ i + base_vert ] = mesh._subMeshId[i] + _subMeshCount;
		}

		_ValidateSize();  // throw

		_indices.reserve( _indices.size() + mesh._indices.size() );

		for (uint idx : mesh._indices) {
			_indices.push_back( idx + base_vert );
		}
	}

/*
=================================================
	_AddTemp
=================================================
*/
	void  ScriptMesh::_AddTemp (INOUT ScriptMesh &temp, const ScriptTransform &tr)
	{
		temp._attribs |= (_attribs & _GenMask);
		temp._attribs &= _attribs;

		if ( AnyBits( _attribs, EAttribute::TB )		and
			 not AnyBits( temp._attribs, EAttribute::TB ))
		{
			temp._CalcTBN();
		}

		temp._Transform( tr );
		temp._subMesh = true;

		_Add( temp );
	}

/*
=================================================
	AddCube*
=================================================
*/
	void  ScriptMesh::AddCube1 () __Th___
	{
		AddCube2( {} );
	}

	void  ScriptMesh::AddCube2 (const ScriptTransform &tr) __Th___
	{
		GeometryTools::CubeGen	cube;
		CHECK_THROW( cube.Create( False{"2d"} ));

		ScriptMesh	temp;
		temp._Resize( cube.GetVertices().size() );

		for (const auto [vert, i] : WithIndex( cube.GetVertices() ))
		{
			float3	pos {SNormShortToFloat( vert.position )};

			temp._position[i]	= pos;
			temp._uv[i]			= float2{SNormShortToFloat( vert.texcoord )};
			temp._uvw[i]		= float3{ pos.x, -pos.y, pos.z } * 0.5f;
			temp._normal[i]		= float3{SNormShortToFloat( vert.normal )};
			temp._tangent[i]	= float3{SNormShortToFloat( vert.tangent )};
			temp._bitangent[i]	= float3{SNormShortToFloat( vert.bitangent )};
		}

		for (auto idx : cube.GetIndices()) {
			temp._indices.push_back( idx );
		}

		temp._attribs = EAttribute::Position | EAttribute::Texcoord2D | EAttribute::Texcoord3D | EAttribute::Normal | EAttribute::Tangent | EAttribute::BiTangent;
		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	AddSphere*
=================================================
*/
	void  ScriptMesh::AddSphere1 (uint lod) __Th___
	{
		AddSphere2( lod, {} );
	}

	void  ScriptMesh::AddSphere2 (uint lod, const ScriptTransform &tr) __Th___
	{
		GeometryTools::SphericalCubeGen	sphere;
		CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

		ArrayView<GeometryTools::SphericalCubeGen::Vertex>	verts;
		CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

		ScriptMesh	temp;
		temp._Resize( verts.size() );

		for (const auto [vert, i] : WithIndex( verts ))
		{
			temp._position[i]	= float3{SNormShortToFloat( vert.position )};
			temp._normal[i]		= float3{SNormShortToFloat( vert.position )};
			temp._uvw[i]		= float3{SNormShortToFloat( vert.texcoord )};
			temp._tangent[i]	= float3{SNormShortToFloat( vert.tangent )};
			temp._bitangent[i]	= float3{SNormShortToFloat( vert.bitangent )};
		}

		ArrayView<GeometryTools::SphericalCubeGen::Index>	idxs;
		CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

		for (auto idx : idxs) {
			temp._indices.push_back( idx );
		}

		temp._attribs = EAttribute::Position | EAttribute::Normal | EAttribute::Texcoord3D | EAttribute::Tangent | EAttribute::BiTangent;

		if ( AllBits( _attribs, EAttribute::Texcoord2D ))
		{
			CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, False{"2d"} ));

			CHECK_THROW( sphere.GetVertices( lod, OUT verts ));
			CHECK_THROW( temp._position.size() == verts.size() );

			for (const auto [vert, i] : WithIndex( verts )) {
				temp._uv[i] = float2{SNormShortToFloat( vert.texcoord )};
			}

			temp._attribs |= EAttribute::Texcoord2D;
			_dontValidate = true;
		}

		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	AddSphericalCube*
=================================================
*/
	void  ScriptMesh::AddSphericalCube1 (uint lod) __Th___
	{
		AddSphericalCube2( lod, {} );
	}

	void  ScriptMesh::AddSphericalCube2 (uint lod, const ScriptTransform &tr) __Th___
	{
		lod = lod*2 - 1;

		// for cube sides
		const uint	vcount	= lod + 2;
		const uint	icount	= lod + 1;
		uint		vert_i	= 0;

		ScriptMesh	temp;
		temp._indices.reserve( icount * icount * 6 );
		temp._Resize( vcount * vcount * 6 );

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
					temp._indices.push_back( idx[0] );	temp._indices.push_back( idx[3] );	temp._indices.push_back( idx[1] );
					temp._indices.push_back( idx[0] );	temp._indices.push_back( idx[2] );	temp._indices.push_back( idx[3] );
				}else{
					temp._indices.push_back( idx[0] );	temp._indices.push_back( idx[2] );	temp._indices.push_back( idx[1] );
					temp._indices.push_back( idx[2] );	temp._indices.push_back( idx[3] );	temp._indices.push_back( idx[1] );
				}
			}

			// generate vertices
			for (uint y = 0; y < vcount; ++y)
			for (uint x = 0; x < vcount; ++x)
			{
				float2		ncoord = ToSNorm( float2{uint2{ x, y }} / float(vcount-1) );

				temp._position[vert_i] = float3{ ncoord, float(face) };
				++vert_i;
			}
		}

		ASSERT_Eq( vert_i, temp._position.size() );

		temp._attribs = EAttribute::Position;
		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	AddGrid*
=================================================
*/
	void  ScriptMesh::AddGrid1 (uint lod) __Th___
	{
		AddGrid2( lod, {} );
	}

	void  ScriptMesh::AddGrid2 (uint lod, const ScriptTransform &tr) __Th___
	{
		GeometryTools::GridGen	grid;
		CHECK_THROW( grid.Create( 1u << (lod+1), 3u ));

		ScriptMesh	temp;
		temp._Resize( grid.GetVertices().size() );

		for (const auto [vert, i] : WithIndex( grid.GetVertices() ))
		{
			temp._position[i]	= float3{ ToSNorm( vert.uv ), 0.f };
			temp._normal[i]		= float3{ 0.f, 0.f, 1.f };
			temp._uv[i]			= float2{ vert.uv };
			temp._uvw[i]		= float3{ vert.uv, 0.f };
		}

		for (auto idx : grid.GetIndices()) {
			temp._indices.push_back( idx );
		}

		temp._attribs = EAttribute::Position | EAttribute::Normal | EAttribute::Texcoord2D | EAttribute::Texcoord3D;
		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	AddGridXZ*
=================================================
*/
	void  ScriptMesh::AddGridXZ1 (uint lod) __Th___
	{
		AddGridXZ2( lod, {} );
	}

	void  ScriptMesh::AddGridXZ2 (uint lod, const ScriptTransform &tr) __Th___
	{
		GeometryTools::GridGen	grid;
		CHECK_THROW( grid.Create( 1u << (lod+1), 3u ));

		ScriptMesh	temp;
		temp._Resize( grid.GetVertices().size() );

		for (const auto [vert, i] : WithIndex( grid.GetVertices() ))
		{
			temp._position[i]	= float3{ ToSNorm( vert.uv.x ), 0.f, ToSNorm( vert.uv.y )};
			temp._normal[i]		= float3{ 0.f, -1.f, 0.f };		// -Y is up
			temp._uv[i]			= float2{ vert.uv };
			temp._uvw[i]		= float3{ vert.uv, 0.f };
		}

		for (auto idx : grid.GetIndices()) {
			temp._indices.push_back( idx );
		}

		temp._attribs = EAttribute::Position | EAttribute::Normal | EAttribute::Texcoord2D | EAttribute::Texcoord3D;
		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	AddTube*
=================================================
*/
	void  ScriptMesh::AddTube1 (uint segmentCount) __Th___
	{
		AddTube2( segmentCount, {} );
	}

	void  ScriptMesh::AddTube2 (uint segmentCount, const ScriptTransform &tr) __Th___
	{
		_AddCylinder( int(GeometryTools::CylinderGen::EShape::Tube), segmentCount, tr );
	}

/*
=================================================
	AddInnerTube*
=================================================
*/
	void  ScriptMesh::AddInnerTube1 (uint segmentCount) __Th___
	{
		AddInnerTube2( segmentCount, {} );
	}

	void  ScriptMesh::AddInnerTube2 (uint segmentCount, const ScriptTransform &tr) __Th___
	{
		_AddCylinder( int(GeometryTools::CylinderGen::EShape::Tube_Inner), segmentCount, tr );
	}

/*
=================================================
	AddCylinder*
=================================================
*/
	void  ScriptMesh::AddCylinder1 (uint segmentCount) __Th___
	{
		AddCylinder2( segmentCount, {} );
	}

	void  ScriptMesh::AddCylinder2 (uint segmentCount, const ScriptTransform &tr) __Th___
	{
		_AddCylinder( int(GeometryTools::CylinderGen::EShape::Solid), segmentCount, tr );
	}

/*
=================================================
	AddCylinderEnds*
=================================================
*/
	void  ScriptMesh::AddCylinderEnds1 (uint segmentCount) __Th___
	{
		AddCylinderEnds2( segmentCount, {} );
	}

	void  ScriptMesh::AddCylinderEnds2 (uint segmentCount, const ScriptTransform &tr) __Th___
	{
		_AddCylinder( int(GeometryTools::CylinderGen::EShape::Ends), segmentCount, tr );
	}

/*
=================================================
	AddCapsule*
=================================================
*/
	void  ScriptMesh::AddCapsule1 (uint segmentCount) __Th___
	{
		AddCapsule2( segmentCount, {} );
	}

	void  ScriptMesh::AddCapsule2 (uint segmentCount, const ScriptTransform &tr) __Th___
	{
		_AddCylinder( int(GeometryTools::CylinderGen::EShape::Capsule), segmentCount, tr );
	}

/*
=================================================
	_AddCylinder
=================================================
*/
	void  ScriptMesh::_AddCylinder (int shape, uint segmentCount, const ScriptTransform &tr) __Th___
	{
		GeometryTools::CylinderGen	cylinder;
		CHECK_THROW( cylinder.Create( segmentCount, GeometryTools::CylinderGen::EShape(shape) ));

		ScriptMesh	temp;
		temp._Resize( cylinder.GetVertices().size() );

		for (const auto [vert, i] : WithIndex( cylinder.GetVertices() ))
		{
			temp._position[i]	= float3{SNormShortToFloat( vert.position )};
			temp._normal[i]		= float3{SNormShortToFloat( vert.normal )};
			temp._uv[i]			= float2{SNormShortToFloat( vert.texcoord )};
			temp._tangent[i]	= float3{SNormShortToFloat( vert.tangent )};
			temp._bitangent[i]	= float3{SNormShortToFloat( vert.bitangent )};
		}

		for (auto idx : cylinder.GetIndices()) {
			temp._indices.push_back( idx );
		}

		temp._attribs = EAttribute::Position | EAttribute::Texcoord2D | EAttribute::Normal | EAttribute::Tangent | EAttribute::BiTangent;
		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	AddCone*
=================================================
*/
	void  ScriptMesh::AddCone1 (uint segmentCount, float radius, float height) __Th___
	{
		AddCone2( segmentCount, radius, height, {} );
	}

	void  ScriptMesh::AddCone3 (uint segmentCount) __Th___
	{
		AddCone2( segmentCount, 1.f, 1.f, {} );
	}

	void  ScriptMesh::AddCone4 (uint segmentCount, const ScriptTransform &tr) __Th___
	{
		AddCone2( segmentCount, 1.f, 1.f, tr );
	}

	void  ScriptMesh::AddCone2 (uint segmentCount, float radius, float height, const ScriptTransform &tr) __Th___
	{
		GeometryTools::ConeGen	cone;
		CHECK_THROW( cone.Create( segmentCount, radius, height ));

		ScriptMesh	temp;
		temp._Resize( cone.GetPositions().size() );

		for (const auto [pos, i] : WithIndex( cone.GetPositions() ))
		{
			temp._position[i]	= pos;
		}

		for (auto idx : cone.GetIndices()) {
			temp._indices.push_back( idx );
		}

		temp._attribs = EAttribute::Position;
		_AddTemp( INOUT temp, tr );
	}

/*
=================================================
	_Resize
=================================================
*/
	void  ScriptMesh::_Resize (usize attrCount)
	{
		_position	.resize( attrCount );
		_uv			.resize( attrCount );
		_uvw		.resize( attrCount );
		_normal		.resize( attrCount );
		_tangent	.resize( attrCount );
		_bitangent	.resize( attrCount );
		_tbnQuat	.resize( attrCount );
		_color		.resize( attrCount );
		_fParams	.resize( attrCount );
		_iParams	.resize( attrCount );
		_subMeshId	.resize( attrCount );
	}

/*
=================================================
	_Transform
=================================================
*/
	void  ScriptMesh::_Transform (const ScriptTransform &tr)
	{
		float4x3	pos_tr = tr.ToMatrix4x3();
		float3x3	dir_tr {pos_tr};

		for (auto& pos : _position) {
			pos = float3{ pos_tr * float4{pos, 1.f} };
		}

		if ( AllBits( _attribs, EAttribute::Normal ))
		{
			for (auto& norm : _normal) {
				norm = Normalize( dir_tr * norm );
			}
		}
		if ( AllBits( _attribs, EAttribute::Tangent ))
		{
			for (auto& norm : _tangent) {
				norm = Normalize( dir_tr * norm );
			}
		}
		if ( AllBits( _attribs, EAttribute::BiTangent ))
		{
			for (auto& norm : _bitangent) {
				norm = Normalize( dir_tr * norm );
			}
		}
	}

/*
=================================================
	_CalcTBN
=================================================
*/
	void  ScriptMesh::_CalcTBN ()
	{
		using namespace AE::GeometryTools;

		CHECK_THROW_MSG( AllBits( _attribs, EAttribute::Position | EAttribute::Texcoord2D ),
			"TBN calculation requires 'Position' and 'Texcoord2D' attributes" );

		CHECK_THROW_MSG( not AllBits( _attribs, EAttribute::TBN ),
			"TBN is already calculated" );

		Array<EAttribute>	has_tbn;
		has_tbn.resize( _position.size(), _attribs & EAttribute::TBN );

		for (usize i = 0; i < _indices.size(); i += 3)
		{
			const uint		i0		= _indices[i+0];
			const uint		i1		= _indices[i+1];
			const uint		i2		= _indices[i+2];

			const float3	pos0	= _position[i0];
			const float3	pos1	= _position[i1];
			const float3	pos2	= _position[i2];

			const float2	uv0		= _uv[i0];
			const float2	uv1		= _uv[i1];
			const float2	uv2		= _uv[i2];

			float3	normal, tangent, bitangent;
			ComputeTBN( pos0, uv0,
						pos1, uv1,
						pos2, uv2,
						OUT normal, OUT tangent, OUT bitangent );

			CheckTBN( normal, tangent, bitangent );

			for (usize j = 0; j < 3; ++j)
			{
				const EAttribute	flags = has_tbn[i+j];

				if ( AllBits( flags, EAttribute::Normal ))
				{
					CHECK_THROW_MSG( Dot( normal, float3{_normal[i+j]} ) > 0.0f,
						"calculated normal must be in the same hemisphere as exist normal" );
				}

				if ( AllBits( flags, EAttribute::Tangent ))
				{
					CHECK_THROW_MSG( All(BitEqual( float3{_tangent[i+j]}, tangent )),
						"calculated tangent mismatch, may be 2 triangles share same vertex but they TBN are not equal" );
				}
				else
					_tangent[i+j] = tangent;

				if ( AllBits( flags, EAttribute::BiTangent ))
				{
					CHECK_THROW_MSG( All(BitEqual( float3{_bitangent[i+j]}, bitangent )),
						"calculated bitangent mismatch, may be 2 triangles share same vertex but they TBN are not equal" );
				}
				else
					_bitangent[i+j] = bitangent;

				has_tbn[i+j] = EAttribute::TBN;
			}
		}

		_attribs |= EAttribute::TBN;
	}

/*
=================================================
	_CalcTBNQuat
=================================================
*/
	void  ScriptMesh::_CalcTBNQuat ()
	{
		for (usize i = 0, cnt = _tangent.size(); i < cnt; ++i)
		{
			float	handedness	= Dot( Cross( _tangent[i], _bitangent[i] ), _normal[i] );
			CHECK_THROW( IsZero( handedness + 1.f ));

			// -B -- fix for left-handed TBN
			float3x3	mat {_tangent[i], -_bitangent[i], _normal[i]};
			Quat		q	{mat.Transpose()};

			_tbnQuat[i] = float4{ q.x, q.y, q.z, q.w };
		}

		const float	eps = 1.0e-5f;

		for (usize i = 0; i < _indices.size(); i += 3)
		{
			uint	i0	= _indices[i+0];
			uint	i1	= _indices[i+1];
			uint	i2	= _indices[i+2];

			Quat	q0	{ _tbnQuat[i0].w, _tbnQuat[i0].x, _tbnQuat[i0].y, _tbnQuat[i0].z };
			Quat	q1	{ _tbnQuat[i1].w, _tbnQuat[i1].x, _tbnQuat[i1].y, _tbnQuat[i1].z };
			Quat	q2	{ _tbnQuat[i2].w, _tbnQuat[i2].x, _tbnQuat[i2].y, _tbnQuat[i2].z };

			CHECK_THROW( Dot( q0, q1 ) > eps );
			CHECK_THROW( Dot( q0, q2 ) > eps );
			CHECK_THROW( Dot( q1, q2 ) > eps );
		}
	}

/*
=================================================
	_IsMutable
=================================================
*/
	void  ScriptMesh::_IsMutable ()
	{
		CHECK_THROW_MSG( not _buffer,
			"buffer is already created, can not modify vertices" );
	}

/*
=================================================
	_ValidateSize
=================================================
*/
	void  ScriptMesh::_ValidateSize () C_Th___
	{
		const usize		vert_count = _position.size();
		CHECK_THROW( vert_count == _uv.size() );
		CHECK_THROW( vert_count == _uvw.size() );
		CHECK_THROW( vert_count == _normal.size() );
		CHECK_THROW( vert_count == _tangent.size() );
		CHECK_THROW( vert_count == _bitangent.size() );
		CHECK_THROW( vert_count == _tbnQuat.size() );
		CHECK_THROW( vert_count == _color.size() );
		CHECK_THROW( vert_count == _fParams.size() );
		CHECK_THROW( vert_count == _iParams.size() );
		CHECK_THROW( vert_count == _subMeshId.size() );
	}

/*
=================================================
	_ValidateTBN
=================================================
*/
	void  ScriptMesh::_ValidateTBN () C_Th___
	{
		using namespace AE::GeometryTools;

		if ( not AllBits( _attribs, EAttribute::TBN ))
			return;

		if ( _dontValidate )
			return;

		for (usize i = 0; i < _position.size(); ++i)
		{
			CheckTBN( _normal[i], _tangent[i], _bitangent[i] );
		}
	}

/*
=================================================
	_ValidateIndices
=================================================
*/
	void  ScriptMesh::_ValidateIndices () C_Th___
	{
		CHECK_THROW_MSG( IsMultipleOf( _indices.size(), 3 ));

		for (uint idx : _indices) {
			CHECK_THROW( idx < _position.size() );
		}
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptMesh::Name (const String &name) __Th___
	{
		_IsMutable();  // throw

		_bufferName = name;
	}

/*
=================================================
	AddVertices*
=================================================
*/
	void  ScriptMesh::AddVertices1 (const ScriptArray<packed_float3> &position) __Th___
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding vertices" );
		CHECK_THROW_MSG( (_attribs & ~_GenMask) == EAttribute::Position );

		const usize	base_idx = _position.size();
		_Resize( base_idx + position.size() );

		for (const auto [pos, i] : WithIndex( position )) {
			_position[ base_idx + i ] = pos;
		}
	}

	void  ScriptMesh::AddVertices2 (const ScriptArray<packed_float3> &position,
									const ScriptArray<packed_float2> &texcoords) __Th___
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding vertices" );
		CHECK_THROW_MSG( (_attribs & ~_GenMask) == (EAttribute::Position | EAttribute::Texcoord2D) );

		const usize	base_idx = _position.size();
		_Resize( base_idx + position.size() );

		for (const auto [pos, i] : WithIndex( position )) {
			_position[ base_idx + i ] = pos;
		}
		for (const auto [uv, i] : WithIndex( texcoords )) {
			_uv[ base_idx + i ] = uv;
		}
	}

	void  ScriptMesh::AddVertices3 (const ScriptArray<packed_float3> &position,
									const ScriptArray<packed_float2> &texcoords,
									const ScriptArray<packed_float3> &normal) __Th___
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding vertices" );
		CHECK_THROW_MSG( (_attribs & ~_GenMask) == (EAttribute::Position | EAttribute::Texcoord2D | EAttribute::Normal) );

		const usize	base_idx = _position.size();
		_Resize( base_idx + position.size() );

		for (const auto [pos, i] : WithIndex( position )) {
			_position[ base_idx + i ] = pos;
		}
		for (const auto [uv, i] : WithIndex( texcoords )) {
			_uv[ base_idx + i ] = uv;
		}
		for (const auto [norm, i] : WithIndex( normal )) {
			_normal[ base_idx + i ] = norm;
		}
	}

	void  ScriptMesh::AddVertices4 (const ScriptArray<packed_float3> &position,
									const ScriptArray<packed_float2> &texcoords,
									const ScriptArray<packed_float3> &normal,
									const ScriptArray<packed_float3> &tangent,
									const ScriptArray<packed_float3> &bitangent) __Th___
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding vertices" );
		CHECK_THROW_MSG( (_attribs & ~_GenMask) == (EAttribute::Position | EAttribute::Texcoord2D | EAttribute::TBN) );

		const usize	base_idx = _position.size();
		_Resize( base_idx + position.size() );

		for (const auto [pos, i] : WithIndex( position )) {
			_position[ base_idx + i ] = pos;
		}
		for (const auto [uv, i] : WithIndex( texcoords )) {
			_uv[ base_idx + i ] = uv;
		}
		for (const auto [norm, i] : WithIndex( normal )) {
			_normal[ base_idx + i ] = norm;
		}
		for (const auto [tan, i] : WithIndex( tangent )) {
			_tangent[ base_idx + i ] = tan;
		}
		for (const auto [bitan, i] : WithIndex( bitangent )) {
			_bitangent[ base_idx + i ] = bitan;
		}
	}

	void  ScriptMesh::AddVertices5 (const ScriptArray<packed_float3> &position,
									const ScriptArray<RGBA32f>		 &color) __Th___
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding vertices" );
		CHECK_THROW_MSG( (_attribs & ~_GenMask) == (EAttribute::Position | EAttribute::Color) );

		const usize	base_idx = _position.size();
		_Resize( base_idx + position.size() );

		for (const auto [pos, i] : WithIndex( position )) {
			_position[ base_idx + i ] = pos;
		}
		for (const auto [col, i] : WithIndex( color )) {
			_color[ base_idx + i ] = col;
		}
	}

	void  ScriptMesh::AddVertices6 (const ScriptArray<packed_float3> &position,
									const ScriptArray<packed_float2> &texcoords,
									const ScriptArray<RGBA32f>		 &color) __Th___
	{
		CHECK_THROW_MSG( _attribs != Default, "attributes must be set before adding vertices" );
		CHECK_THROW_MSG( (_attribs & ~_GenMask) == (EAttribute::Position | EAttribute::Color | EAttribute::Texcoord2D) );

		const usize	base_idx = _position.size();
		_Resize( base_idx + position.size() );

		for (const auto [pos, i] : WithIndex( position )) {
			_position[ base_idx + i ] = pos;
		}
		for (const auto [col, i] : WithIndex( color )) {
			_color[ base_idx + i ] = col;
		}
		for (const auto [uv, i] : WithIndex( texcoords )) {
			_uv[ base_idx + i ] = uv;
		}
	}

/*
=================================================
	AddIndices
=================================================
*/
	void  ScriptMesh::AddIndices1 (const ScriptArray<uint> &indices) __Th___
	{
		_indices.insert( _indices.end(), indices.begin(), indices.end() );
	}

	void  ScriptMesh::AddIndices2 (const ScriptArray<uint> &indices) __Th___
	{
		const uint	base_idx = uint(_position.size());
		_indices.reserve( _indices.size() + indices.size() );

		for (uint idx : indices) {
			_indices.push_back( base_idx + idx );
		}
	}

/*
=================================================
	ScaleUV*
=================================================
*/
	void  ScriptMesh::ScaleUV1 (float uScale, float vScale) __Th___
	{
		ScaleUV2({ uScale, vScale });
	}

	void  ScriptMesh::ScaleUV2 (const packed_float2 &uvScale) __Th___
	{
		for (auto& uv : _uv) {
			uv *= uvScale;
		}
	}

/*
=================================================
	DeviceAddress
=================================================
*/
	ulong  ScriptMesh::DeviceAddress (EAttribute attrib) __Th___
	{
		if ( _buffer )
		{
			CHECK_THROW_MSG( AllBits( _bufferUsage, EResourceUsage::ShaderAddress ),
				"Buffer is already created without DeviceAddress usage" );
		}

		CHECK_THROW_MSG( AllBits( _attribs, attrib ), "unsupported attribute" );

		_bufferUsage |= EResourceUsage::ShaderAddress;

		ScriptBufferPtr	buf;
		buf.Attach( ToBuffer() );  // throw

		uint	offset = UMax;
		switch_enum( attrib )
		{
			case EAttribute::Unknown :		offset = _offsets[0];	break;	// indices
			case EAttribute::Position :		offset = _offsets[1];	break;
			case EAttribute::Texcoord2D :	offset = _offsets[2];	break;
			case EAttribute::Texcoord3D :	offset = _offsets[3];	break;
			case EAttribute::Normal :		offset = _offsets[4];	break;
			case EAttribute::Tangent :		offset = _offsets[5];	break;
			case EAttribute::BiTangent :	offset = _offsets[6];	break;
			case EAttribute::TBN_Quat :		offset = _offsets[7];	break;
			case EAttribute::Color :		offset = _offsets[8];	break;
			case EAttribute::FloatParams :	offset = _offsets[9];	break;
			case EAttribute::IntParams :	offset = _offsets[10];	break;
			case EAttribute::SubMeshID :	offset = _offsets[11];	break;

			case EAttribute::TB :
			case EAttribute::TBN :
			case EAttribute::TriangleIndices :
			case EAttribute::TriangleIndicesPack32 :
			case EAttribute::_Last :
			default :						CHECK_THROW_MSG( false, "unsupported attribute" );
		}
		switch_end
		CHECK_THROW_MSG( offset != UMax, "unsupported attribute" );

		return buf->GetDeviceAddress() + offset;
	}

/*
=================================================
	ToBuffer
=================================================
*/
	ScriptBuffer*  ScriptMesh::ToBuffer () __Th___
	{
		if ( _buffer )
			return ScriptBufferPtr{_buffer}.Detach();

		_ValidateSize();
		_ValidateIndices();
	//	_ValidateTBN();

		if ( AllBits( _attribs, EAttribute::TBN_Quat ))
			_CalcTBNQuat();

		// TODO: 2 layouts:
		//	1. pos[], norm[], ...
		//	2. DeviceAddress pos, ...

		_buffer.Set( new ScriptBuffer{} );

		if ( not _bufferName.empty() )
			_buffer->SetLayoutName( _bufferName );

		_offsets[0] = _buffer->UInt1Array2( "indices", _indices );
		for (EAttribute bit : BitfieldIterate( _attribs ))
		{
			switch_enum( bit )
			{
				case EAttribute::Position :			_offsets[1]  = _buffer->Float3Array2( "position",	_position );	break;
				case EAttribute::Texcoord2D :		_offsets[2]  = _buffer->Float2Array2( "uv",			_uv );			break;
				case EAttribute::Texcoord3D :		_offsets[3]  = _buffer->Float3Array2( "uvw",		_uvw );			break;
				case EAttribute::Normal :			_offsets[4]  = _buffer->Float3Array2( "normal",		_normal );		break;
				case EAttribute::Tangent :			_offsets[5]  = _buffer->Float3Array2( "tangent",	_tangent );		break;
				case EAttribute::BiTangent :		_offsets[6]  = _buffer->Float3Array2( "bitangent",	_bitangent );	break;
				case EAttribute::TBN_Quat :			_offsets[7]  = _buffer->Float4Array2( "tbnQuat",	_tbnQuat );		break;
				case EAttribute::Color :			_offsets[8]  = _buffer->Float4Array2( "Color",		_color );		break;
				case EAttribute::FloatParams :		_offsets[9]  = _buffer->Float4Array2( "FloatParams",_fParams );		break;
				case EAttribute::IntParams :		_offsets[10] = _buffer->Int4Array2(   "IntParams",	_iParams );		break;
				case EAttribute::SubMeshID :		_offsets[11] = _buffer->UInt1Array2(  "subMeshId",	_subMeshId );	break;

				case EAttribute::TriangleIndices :
				{
					Array<packed_uint3>		triangles;
					for (usize i = 0; i < _indices.size(); i += 3) {
						triangles.emplace_back( _indices[i+0], _indices[i+1], _indices[i+2] );
					}
					_buffer->UInt3Array2( "triangles", triangles );
					break;
				}

				case EAttribute::TriangleIndicesPack32 :
					// TODO

				case EAttribute::TB :
				case EAttribute::TBN :
				case EAttribute::_Last :
				case EAttribute::_BITOPS_ :		break;
			}
			switch_end
		}

		for (EResourceUsage usage : BitfieldIterate( _bufferUsage )) {
			_buffer->AddUsage( usage );
		}

		return ScriptBufferPtr{_buffer}.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptMesh::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			Scripting::EnumBinder<ScriptMesh::EAttribute>	binder{ se };
			binder.Create();
			binder.BindAll();
			binder.AddValue< EAttribute::TB >();
			binder.AddValue< EAttribute::TBN >();
		}

		Scripting::ClassBinder<ScriptMesh>	binder{ se };
		binder.CreateRef();

		AS_METHOD( binder, ScriptMesh::SetAttributes1,		"SetAttributes",		{} );
		AS_METHOD( binder, ScriptMesh::SetAttributes2,		"SetAttributes",		{} );

		AS_METHOD( binder, ScriptMesh::Add,					"Add",					{} );

		AS_METHOD( binder, ScriptMesh::AddCube1,			"AddCube",				{} );
		AS_METHOD( binder, ScriptMesh::AddCube2,			"AddCube",				{"transform"} );

		binder.Comment( "Add spherical cube (normalized and tessellated cube)."
						"Supports UV and cubemap UVW coords." );
		AS_METHOD( binder, ScriptMesh::AddSphere1,			"AddSphere",			{"lod"} );
		AS_METHOD( binder, ScriptMesh::AddSphere2,			"AddSphere",			{"lod", "transform"} );

		binder.Comment( "Add grid mesh, XY pos equal to UV, Z = 0."
						"Grid size calculated as 'Pow( 2, lod+1 )'."
						"Number of vertices: size * size." );
		AS_METHOD( binder, ScriptMesh::AddGrid1,			"AddGrid",				{"lod"} );
		AS_METHOD( binder, ScriptMesh::AddGrid2,			"AddGrid",				{"lod", "transform"} );

		binder.Comment( "Add grid mesh, XZ pos equal to UV, Y = 0." );
		AS_METHOD( binder, ScriptMesh::AddGridXZ1,			"AddGridXZ",			{"lod"} );
		AS_METHOD( binder, ScriptMesh::AddGridXZ2,			"AddGridXZ",			{"lod", "transform"} );

		binder.Comment( "Add open ended cylinder mesh, axis in -Y" );
		AS_METHOD( binder, ScriptMesh::AddTube1,			"AddTube",				{"segmentCount"} );
		AS_METHOD( binder, ScriptMesh::AddTube2,			"AddTube",				{"segmentCount", "transform"} );

		binder.Comment( "Add open ended cylinder mesh with inversed vertex order, axis in -Y" );
		AS_METHOD( binder, ScriptMesh::AddInnerTube1,		"AddInnerTube",			{"segmentCount"} );
		AS_METHOD( binder, ScriptMesh::AddInnerTube2,		"AddInnerTube",			{"segmentCount", "transform"} );

		binder.Comment( "Add solid cylinder mesh, axis in -Y" );
		AS_METHOD( binder, ScriptMesh::AddCylinder1,		"AddCylinder",			{"segmentCount"} );
		AS_METHOD( binder, ScriptMesh::AddCylinder2,		"AddCylinder",			{"segmentCount", "transform"} );

		binder.Comment( "Add only cylinder ends mesh, axis in -Y" );
		AS_METHOD( binder, ScriptMesh::AddCylinderEnds1,	"AddCylinderEnds",		{"segmentCount"} );
		AS_METHOD( binder, ScriptMesh::AddCylinderEnds2,	"AddCylinderEnds",		{"segmentCount", "transform"} );

		binder.Comment( "Add capsule mesh, axis in -Y" );
		AS_METHOD( binder, ScriptMesh::AddCapsule1,			"AddCapsule",			{"segmentCount"} );
		AS_METHOD( binder, ScriptMesh::AddCapsule2,			"AddCapsule",			{"segmentCount", "transform"} );

		binder.Comment( "Add cone mesh, apex in +Z" );
		AS_METHOD( binder, ScriptMesh::AddCone1,			"AddCone",				{"segmentCount", "radius", "height"} );
		AS_METHOD( binder, ScriptMesh::AddCone2,			"AddCone",				{"segmentCount", "radius", "height", "transform"} );
		AS_METHOD( binder, ScriptMesh::AddCone3,			"AddCone",				{"segmentCount"} );
		AS_METHOD( binder, ScriptMesh::AddCone4,			"AddCone",				{"segmentCount", "transform"} );

		binder.Comment( "Add tessellated cube without normalization." );
		AS_METHOD( binder, ScriptMesh::AddSphericalCube1,	"AddSphericalCube",		{"lod"} );
		AS_METHOD( binder, ScriptMesh::AddSphericalCube2,	"AddSphericalCube",		{"lod", "transform"} );

		AS_METHOD( binder, ScriptMesh::IndexCount,			"IndexCount",			{} );
		AS_METHOD( binder, ScriptMesh::PrimitiveCount,		"PrimitiveCount",		{} );
		AS_METHOD( binder, ScriptMesh::VertexCount,			"VertexCount",			{} );

		AS_METHOD( binder, ScriptMesh::AddVertices1,		"AddVertices",			{"position"} );
		AS_METHOD( binder, ScriptMesh::AddVertices2,		"AddVertices",			{"position", "uv"} );
		AS_METHOD( binder, ScriptMesh::AddVertices3,		"AddVertices",			{"position", "uv", "normal"} );
		AS_METHOD( binder, ScriptMesh::AddVertices4,		"AddVertices",			{"position", "uv", "normal", "tangent", "bitangent"} );
		AS_METHOD( binder, ScriptMesh::AddVertices5,		"AddVertices",			{"position", "color"} );
		AS_METHOD( binder, ScriptMesh::AddVertices6,		"AddVertices",			{"position", "uv", "color"} );

		binder.Comment( "Add indices without modification. You should use 'VertexCount()' to add offset to them." );
		AS_METHOD( binder, ScriptMesh::AddIndices1,			"AddIndices",			{"indices"} );

		binder.Comment( "Add indices and add 'VertexCount()' to them." );
		AS_METHOD( binder, ScriptMesh::AddIndices2,			"AddIndicesWithOffset",	{"indices"} );

		AS_METHOD( binder, ScriptMesh::ScaleUV1,			"ScaleUV",				{"uScale", "vScale"} );
		AS_METHOD( binder, ScriptMesh::ScaleUV2,			"ScaleUV",				{"uvScale"} );

		AS_METHOD( binder, ScriptMesh::DeviceAddress,		"DeviceAddress",		{"attrib"} );

		AS_METHOD( binder, ScriptMesh::Name,				"Name",					{} );
		AS_METHOD( binder, ScriptMesh::ToBuffer,			"ToBuffer",				{} );
	}


} // AE::ResEditor
