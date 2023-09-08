// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "geometry_tools/Cube/CubeGen.h"
#include "geometry_tools/Utils/TBN.h"

namespace AE::GeometryTools
{
/*
=================================================
    Create
=================================================
*/
    bool  CubeGen::Create (Bool cubeMap) __NE___
    {
        static const float  a_positions[] = {
            -1.f, -1.f, -1.f,   -1.f,  1.f, -1.f,    1.f, -1.f, -1.f,    1.f,  1.f, -1.f,   // -Z
             1.f, -1.f,  1.f,    1.f,  1.f,  1.f,   -1.f, -1.f,  1.f,   -1.f,  1.f,  1.f,   // +Z
            -1.f, -1.f,  1.f,   -1.f, -1.f, -1.f,    1.f, -1.f,  1.f,    1.f, -1.f, -1.f,   // -Y
             1.f, -1.f, -1.f,    1.f,  1.f, -1.f,    1.f, -1.f,  1.f,    1.f,  1.f,  1.f,   // +X
            -1.f,  1.f, -1.f,   -1.f,  1.f,  1.f,    1.f,  1.f, -1.f,    1.f,  1.f,  1.f,   // +Y
            -1.f, -1.f,  1.f,   -1.f,  1.f,  1.f,   -1.f, -1.f, -1.f,   -1.f,  1.f, -1.f,   // -X
        };
        static const float  a_texcoords2d[] = {
            0.f, 0.f,   0.f, 1.f,   1.f, 0.f,   1.f, 1.f
        };
        static const float  a_normals[] = {
             0.f,  0.f, -1.f,   // -Z
             0.f,  0.f,  1.f,   // +Z
             0.f, -1.f,  0.f,   // -Y
             1.f,  0.f,  0.f,   // +X
             0.f,  1.f,  0.f,   // +Y
            -1.f,  0.f,  0.f,   // -X
        };
        static const float  a_tangents[] = {
             1.f,  0.f,  0.f,   // -Z
            -1.f,  0.f,  0.f,   // +Z
             1.f,  0.f,  0.f,   // -Y
             0.f,  0.f,  1.f,   // +X
             1.f,  0.f,  0.f,   // +Y
             0.f,  0.f, -1.f,   // -X
        };
        static const float  a_bitangents[] = {
             0.f,  1.f,  0.f,   // -Z
             0.f,  1.f,  0.f,   // +Z
             0.f,  0.f, -1.f,   // -Y
             0.f,  1.f,  0.f,   // +X
             0.f,  0.f,  1.f,   // +Y
             0.f,  1.f,  0.f,   // -X
        };
        STATIC_ASSERT( CountOf(a_positions) == (CountOf(a_texcoords2d)*3/2)*6 );
        STATIC_ASSERT( CountOf(a_positions) == CountOf(a_normals)*4 );
        STATIC_ASSERT( CountOf(a_positions) == CountOf(a_tangents)*4 );
        STATIC_ASSERT( CountOf(a_positions) == CountOf(a_bitangents)*4 );

        _vertices.resize( CountOf(a_positions)/3 );

        for (usize i = 0; i < _vertices.size(); ++i)
        {
            ASSERT( i*3+2 < CountOf(a_positions) );
            _vertices[i].position = FloatToSNormShort(float4{ a_positions[i*3+0], a_positions[i*3+1], a_positions[i*3+2], 0.f });
        }

        for (usize i = 0; i < _vertices.size(); ++i)
        {
            const usize j = (i / 4) * 3;
            ASSERT( j+2 < CountOf(a_normals) );

            float3  normal      { a_normals[j+0],       a_normals[j+1],     a_normals[j+2] };
            float3  tangent     { a_tangents[j+0],      a_tangents[j+1],    a_tangents[j+2] };
            float3  bitangent   { a_bitangents[j+0],    a_bitangents[j+1],  a_bitangents[j+2] };
            CheckTBN( normal, tangent, bitangent );

            _vertices[i].normal     = FloatToSNormShort(float4{ normal,     0.f });
            _vertices[i].tangent    = FloatToSNormShort(float4{ tangent,    0.f });
            _vertices[i].bitangent  = FloatToSNormShort(float4{ bitangent,  0.f });
        }

        if ( cubeMap ){
            for (usize i = 0; i < _vertices.size(); ++i){
                _vertices[i].texcoord = FloatToSNormShort(float4{ a_positions[i*3+0], -a_positions[i*3+1], a_positions[i*3+2], 0.f } * 0.5f );
            }
        }else{
            for (usize i = 0; i < _vertices.size(); ++i)
            {
                const usize j = (i % 4);
                ASSERT( j*2+1 < CountOf(a_texcoords2d) );
                _vertices[i].texcoord = FloatToSNormShort(float4{ a_texcoords2d[j*2+0], a_texcoords2d[j*2+1], 0.f, 0.f });
            }
        }

        _indices.resize( 36 );
        for (usize i = 0; i < 6; ++i)
        {
            const usize a = i*6;
            const usize b = i*4;

            _indices[a+0] = Index(b + 0);
            _indices[a+1] = Index(b + 1);
            _indices[a+2] = Index(b + 2);

            _indices[a+3] = Index(b + 3);
            _indices[a+4] = Index(b + 2);
            _indices[a+5] = Index(b + 1);
        }

    #if 0
        for (usize i = 0; i < _indices.size(); i += 3)
        {
            const uint      i0      = _indices[i+0];
            const uint      i1      = _indices[i+1];
            const uint      i2      = _indices[i+2];

            const float3    pos0    = float3{SNormShortToFloat( _vertices[i0].position )};
            const float3    pos1    = float3{SNormShortToFloat( _vertices[i1].position )};
            const float3    pos2    = float3{SNormShortToFloat( _vertices[i2].position )};

            const float2    uv0     = float2{SNormShortToFloat( _vertices[i0].texcoord )};
            const float2    uv1     = float2{SNormShortToFloat( _vertices[i1].texcoord )};
            const float2    uv2     = float2{SNormShortToFloat( _vertices[i2].texcoord )};

            const float3    norm0   = float3{SNormShortToFloat( _vertices[i0].normal )};
            const float3    norm1   = float3{SNormShortToFloat( _vertices[i1].normal )};
            const float3    norm2   = float3{SNormShortToFloat( _vertices[i2].normal )};

            const float3    tan0    = float3{SNormShortToFloat( _vertices[i0].tangent )};
            const float3    tan1    = float3{SNormShortToFloat( _vertices[i1].tangent )};
            const float3    tan2    = float3{SNormShortToFloat( _vertices[i2].tangent )};

            const float3    bitan0  = float3{SNormShortToFloat( _vertices[i0].bitangent )};
            const float3    bitan1  = float3{SNormShortToFloat( _vertices[i1].bitangent )};
            const float3    bitan2  = float3{SNormShortToFloat( _vertices[i2].bitangent )};

            float3  normal, tangent, bitangent;
            CalcTBN1( pos0, uv0,
                      pos1, uv1,
                      pos2, uv2,
                      OUT normal, OUT tangent, OUT bitangent );

            ASSERT( All(Equals( normal, norm0 )));
            ASSERT( All(Equals( tangent, tan0 )));
            ASSERT( All(Equals( bitangent, bitan0 )));
        }
    #endif

        return true;
    }


} // AE::GeometryTools
