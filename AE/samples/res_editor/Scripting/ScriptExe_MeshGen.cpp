// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "geometry_tools/Cube/CubeGen.h"
#include "geometry_tools/Grid/GridGen.h"
#include "geometry_tools/Cylinder/CylinderGen.h"
#include "geometry_tools/SphericalCube/SphericalCubeGen.h"

#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{

/*
=================================================
    _GetCube*
=================================================
*/
    void  ScriptExe::_GetCube2 (OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<packed_float3>  &normals,
                                OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        normals.clear();
        indices.clear();

        GeometryTools::CubeGen  cube;
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

    void  ScriptExe::_GetCube3 (OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<packed_float3>  &normals,
                                OUT ScriptArray<packed_float3>  &tangents,
                                OUT ScriptArray<packed_float3>  &bitangents,
                                OUT ScriptArray<packed_float2>  &texcoords,
                                OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        normals.clear();
        tangents.clear();
        bitangents.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::CubeGen  cube;
        CHECK_THROW( cube.Create( False{"2d"} ));

        for (auto& vert : cube.GetVertices())
        {
            positions   .push_back( float3{SNormShortToFloat( vert.position )});
            normals     .push_back( float3{SNormShortToFloat( vert.normal )});
            tangents    .push_back( float3{SNormShortToFloat( vert.tangent )});
            bitangents  .push_back( float3{SNormShortToFloat( vert.bitangent )});
            texcoords   .push_back( float2{SNormShortToFloat( vert.texcoord )});
        }

        for (auto idx : cube.GetIndices()) {
            indices.push_back( idx );
        }
    }

    void  ScriptExe::_GetCube4 (OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<packed_float3>  &normals,
                                OUT ScriptArray<packed_float3>  &tangents,
                                OUT ScriptArray<packed_float3>  &bitangents,
                                OUT ScriptArray<packed_float3>  &texcoords,
                                OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        normals.clear();
        tangents.clear();
        bitangents.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::CubeGen  cube;
        CHECK_THROW( cube.Create( True{"cubeMap"} ));

        for (auto& vert : cube.GetVertices())
        {
            positions   .push_back( float3{SNormShortToFloat( vert.position )});
            normals     .push_back( float3{SNormShortToFloat( vert.normal )});
            tangents    .push_back( float3{SNormShortToFloat( vert.tangent )});
            bitangents  .push_back( float3{SNormShortToFloat( vert.bitangent )});
            texcoords   .push_back( float3{SNormShortToFloat( vert.texcoord )});
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
    void  ScriptExe::_GetSphere1 (const uint                        lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<uint>             &indices) __Th___
    {
        positions.clear();
        indices.clear();

        GeometryTools::SphericalCubeGen sphere;
        CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

        ArrayView<GeometryTools::SphericalCubeGen::Vertex>  verts;
        CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

        ArrayView<GeometryTools::SphericalCubeGen::Index>   idxs;
        CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

        for (auto& vert : verts) {
            positions.push_back( float3{SNormShortToFloat( vert.position )});
        }
        for (auto idx : idxs) {
            indices.push_back( idx );
        }
    }

    void  ScriptExe::_GetSphere2 (const uint                        lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<packed_float3>    &texcoords,
                                  OUT ScriptArray<uint>             &indices) __Th___
    {
        positions.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::SphericalCubeGen sphere;
        CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

        ArrayView<GeometryTools::SphericalCubeGen::Vertex>  verts;
        CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

        ArrayView<GeometryTools::SphericalCubeGen::Index>   idxs;
        CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

        for (auto& vert : verts) {
            positions.push_back( float3{SNormShortToFloat( vert.position )});
            texcoords.push_back( float3{SNormShortToFloat( vert.texcoord )});
        }
        for (auto idx : idxs) {
            indices.push_back( idx );
        }
    }

    void  ScriptExe::_GetSphere3 (const uint                        lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<packed_float3>    &normals,
                                  OUT ScriptArray<packed_float3>    &tangents,
                                  OUT ScriptArray<packed_float3>    &bitangents,
                                  OUT ScriptArray<packed_float3>    &texcoords,
                                  OUT ScriptArray<uint>             &indices) __Th___
    {
        positions.clear();
        normals.clear();
        tangents.clear();
        bitangents.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::SphericalCubeGen sphere;
        CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, True{"cubeMap"} ));

        ArrayView<GeometryTools::SphericalCubeGen::Vertex>  verts;
        CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

        ArrayView<GeometryTools::SphericalCubeGen::Index>   idxs;
        CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

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

    void  ScriptExe::_GetSphere4 (const uint                        lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<packed_float3>    &normals,
                                  OUT ScriptArray<packed_float3>    &tangents,
                                  OUT ScriptArray<packed_float3>    &bitangents,
                                  OUT ScriptArray<packed_float2>    &texcoords,
                                  OUT ScriptArray<uint>             &indices) __Th___
    {
        positions.clear();
        normals.clear();
        tangents.clear();
        bitangents.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::SphericalCubeGen sphere;
        CHECK_THROW( sphere.Create( lod, lod, False{"tris"}, False{"2d"} ));

        ArrayView<GeometryTools::SphericalCubeGen::Vertex>  verts;
        CHECK_THROW( sphere.GetVertices( lod, OUT verts ));

        ArrayView<GeometryTools::SphericalCubeGen::Index>   idxs;
        CHECK_THROW( sphere.GetIndices( lod, OUT idxs ));

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
    void  ScriptExe::_GetGrid1 (const uint                      size,
                                OUT ScriptArray<packed_float2>  &positions,
                                OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        indices.clear();

        GeometryTools::GridGen  grid;
        CHECK_THROW( grid.Create( size, 3u ));

        for (auto& vert : grid.GetVertices()) {
            positions.push_back( vert.uv );
        }

        for (auto idx : grid.GetIndices()) {
            indices.push_back( idx );
        }
    }

    void  ScriptExe::_GetGrid2 (const uint                      size,
                                OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        indices.clear();

        GeometryTools::GridGen  grid;
        CHECK_THROW( grid.Create( size, 3u ));

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
    void  ScriptExe::_GetCylinder1 (const uint                      segments,
                                    const bool                      inner,
                                    OUT ScriptArray<packed_float3>  &positions,
                                    OUT ScriptArray<packed_float2>  &texcoords,
                                    OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::CylinderGen  cylinder;
        CHECK_THROW( cylinder.Create( segments, Bool{inner} ));

        for (auto& vert : cylinder.GetVertices()) {
            positions.push_back( float3{SNormShortToFloat( vert.position )});
            texcoords.push_back( float2{SNormShortToFloat( vert.texcoord )});
        }
        for (auto idx : cylinder.GetIndices()) {
            indices.push_back( idx );
        }
    }

    void  ScriptExe::_GetCylinder2 (const uint                      segments,
                                    const bool                      inner,
                                    OUT ScriptArray<packed_float3>  &positions,
                                    OUT ScriptArray<packed_float3>  &normals,
                                    OUT ScriptArray<packed_float3>  &tangents,
                                    OUT ScriptArray<packed_float3>  &bitangents,
                                    OUT ScriptArray<packed_float2>  &texcoords,
                                    OUT ScriptArray<uint>           &indices) __Th___
    {
        positions.clear();
        normals.clear();
        tangents.clear();
        bitangents.clear();
        texcoords.clear();
        indices.clear();

        GeometryTools::CylinderGen  cylinder;
        CHECK_THROW( cylinder.Create( segments, Bool{inner} ));

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
    void  ScriptExe::_IndicesToPrimitives (const ScriptArray<uint>          &indices,
                                           OUT ScriptArray<packed_uint3>    &primitives) __Th___
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
    void  ScriptExe::_GetFrustumPlanes (const packed_float4x4           &viewProj,
                                        OUT ScriptArray<packed_float4>  &planes) __Th___
    {
        planes.clear();
        planes.resize( 6 );

        TFrustum<float>     frustum;
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
    void  ScriptExe::_MergeMesh (INOUT ScriptArray<uint>    &srcIndices,
                                 const uint                 srcVertexCount,
                                 const ScriptArray<uint>    &indicesToAdd) __Th___
    {
        const usize old_size = srcIndices.size();

        srcIndices.resize( old_size + indicesToAdd.size() );  // throw

        for (usize i = 0, cnt = indicesToAdd.size(); i < cnt; ++i)
        {
            srcIndices[old_size + i] = srcVertexCount + indicesToAdd[i];
        }
    }


} // AE::ResEditor
