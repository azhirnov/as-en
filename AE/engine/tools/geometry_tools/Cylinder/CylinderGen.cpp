// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "geometry_tools/Cylinder/CylinderGen.h"
#include "geometry_tools/Utils/TBN.h"

namespace AE::GeometryTools
{
/*
=================================================
    Create
=================================================
*/
    bool  CylinderGen::Create (const uint segments, const Bool inner) __NE___
    {
        CHECK_ERR( (segments+1)*2 < MaxValue<Index>() );

        _vertices.resize( (segments+1) * 2 );

        const Rad   angle_scale = 2.0f * Pi / float(segments);

        // i=0  -Z face

        for (uint i = 0; i <= segments; ++i)
        {
            const uint      j       = i*2;
            const float2    sc      = -SinCos( float(i) * angle_scale );
            float           unorm   = float(i) / segments;
            Vertex&         vert0   = _vertices[ j+0 ];
            Vertex&         vert1   = _vertices[ j+1 ];

            float3          pos     = float3{ sc[0], -1.f, sc[1] };
            float3          normal, tangent, bitangent;

            if ( inner )
            {
                normal      = float3{-sc[0], 0.f, -sc[1] };
                tangent     = float3{ sc[1], 0.f, -sc[0] };
                bitangent   = float3{   0.f, 1.f,    0.f };
            }
            else
            {
                normal      = float3{ sc[0],  0.f,  sc[1] };
                tangent     = float3{-sc[1],  0.f,  sc[0] };
                bitangent   = float3{   0.f,  1.f,    0.f };
                unorm       = 1.f - unorm;
            }

            CheckTBN( normal, tangent, bitangent );

            vert0.position  = FloatToSNormShort(float4{ pos.x,  pos.y, pos.z, 0.f });
            vert1.position  = FloatToSNormShort(float4{ pos.x, -pos.y, pos.z, 0.f });

            vert0.texcoord  = FloatToSNormShort(float4{ unorm, 0.f, 0.f, 0.f });
            vert1.texcoord  = FloatToSNormShort(float4{ unorm, 1.f, 0.f, 0.f });

            vert0.normal    = FloatToSNormShort(float4{ normal, 0.f });
            vert1.normal    = FloatToSNormShort(float4{ normal, 0.f });

            vert0.tangent   = FloatToSNormShort(float4{ tangent, 0.f });
            vert1.tangent   = FloatToSNormShort(float4{ tangent, 0.f });

            vert0.bitangent = FloatToSNormShort(float4{ bitangent, 0.f });
            vert1.bitangent = FloatToSNormShort(float4{ bitangent, 0.f });
        }

        _indices.resize( segments * 6 );

        if ( inner )
        {
            for (uint i = 0; i < segments; ++i)
            {
                const uint  j   = i*6;
                const uint  a   = i*2;
                const uint  b   = (i+1)*2;

                _indices[j+0] = Index(a+0); // 0
                _indices[j+1] = Index(a+1); // 1
                _indices[j+2] = Index(b+0); // 2

                _indices[j+3] = Index(b+1); // 3
                _indices[j+4] = Index(b+0); // 2
                _indices[j+5] = Index(a+1); // 1
            }
        }
        else
        {
            for (uint i = 0; i < segments; ++i)
            {
                const uint  j   = i*6;
                const uint  a   = i*2;
                const uint  b   = (i+1)*2;

                _indices[j+0] = Index(b+0); // 2
                _indices[j+1] = Index(a+1); // 1
                _indices[j+2] = Index(a+0); // 0

                _indices[j+3] = Index(a+1); // 1
                _indices[j+4] = Index(b+0); // 2
                _indices[j+5] = Index(b+1); // 3
            }
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
            ComputeTBN( pos0, uv0,
                        pos1, uv1,
                        pos2, uv2,
                        OUT normal, OUT tangent, OUT bitangent );

            CheckTBN( normal, tangent, bitangent );

            ASSERT( All(Equal( normal,      norm0,  0.1f )));
            ASSERT( All(Equal( tangent, tan0,   0.1f )));
            ASSERT( All(Equal( bitangent,   bitan0, 0.1f )));
        }
    #endif

        return true;
    }


} // AE::GeometryTools
