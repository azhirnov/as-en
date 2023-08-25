// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/GeometryTools.pch.h"

namespace AE::GeometryTools
{

/*
=================================================
    CheckTBN
=================================================
*/
    inline void  CheckTBN (const float3 &normal, const float3 &tangent, const float3 &bitangent)
    {
        const float3    normal2     = Normalize( Cross( bitangent, tangent ));
        const float3    tangent2    = Normalize( Cross( normal, bitangent ));
        const float3    bitangent2  = Normalize( Cross( tangent, normal ));

        ASSERT( All(Equals( normal,     normal2,    0.0001f )));
        ASSERT( All(Equals( tangent,    tangent2,   0.0001f )));
        ASSERT( All(Equals( bitangent,  bitangent2, 0.0001f )));
    }

/*
=================================================
    CalcTBN1
=================================================
*/
    inline void  CalcTBN1  (/*vert0*/  const float3 position0, const float2 texcoord0,
                            /*vert1*/  const float3 position1, const float2 texcoord1,
                            /*vert2*/  const float3 position2, const float2 texcoord2,
                            /*output*/ OUT float3 &normal, OUT float3 &tangent, OUT float3 &bitangent)
    {
        float3  e0 = float3( position1.x - position0.x,
                             texcoord1.x - texcoord0.x,
                             texcoord1.y - texcoord0.y );
        float3  e1 = float3( position2.x - position0.x,
                             texcoord2.x - texcoord0.x,
                             texcoord2.y - texcoord0.y );
        float3  cp = Cross( e0, e1 );

        normal = Normalize( Cross( position1 - position0, position2 - position0 ));

        if ( IsNotZero( cp.x )) {
            tangent.x   = -cp.y / cp.x;
            bitangent.x = -cp.z / cp.x;
        }else{
            tangent.x   = 0.0;
            bitangent.x = 0.0;
        }

        e0.x = position1.y - position0.y;
        e1.x = position2.y - position0.y;
        cp   = Cross( e0, e1 );

        if ( IsNotZero( cp.x )) {
            tangent.y   = -cp.y / cp.x;
            bitangent.y = -cp.z / cp.x;
        }else{
            tangent.y   = 0.0;
            bitangent.y = 0.0;
        }

        e0.x = position1.z - position0.z;
        e1.x = position2.z - position0.z;
        cp   = Cross( e0, e1 );

        if ( IsNotZero( cp.x )) {
            tangent.z   = -cp.y / cp.x;
            bitangent.z = -cp.z / cp.x;
        }else{
            tangent.z   = 0.0;
            bitangent.z = 0.0;
        }

        tangent     = Normalize( tangent );
        bitangent   = Normalize( bitangent );

        //if ( Dot( Cross( tangent, bitangent ), normal ) < 0.0 )
        //  tangent = -tangent;
    }

/*
=================================================
    CalcTBN2
=================================================
*/
    inline void  CalcTBN2  (/*vert0*/  const float3 position0, const float2 texcoord0,
                            /*vert1*/  const float3 position1, const float2 texcoord1,
                            /*vert2*/  const float3 position2, const float2 texcoord2,
                            /*bump*/   const float fBumpFactor /*= 0.1*/,
                            /*output*/ OUT float3 &normal, OUT float3 &tangent, OUT float3 &bitangent)
    {
        float3  sw;
        float3  u  = float3( texcoord0.x, texcoord1.x, texcoord2.x );
        float3  v  = float3( texcoord0.y, texcoord1.y, texcoord2.y );
        float3  id = float3( 1.0, 1.0, 1.0 );

        sw = Cross( u, id );
        sw = sw * (fBumpFactor / Dot(v, sw));

        tangent = Normalize( position0 * sw.x + position1 * sw.y + position2 * sw.z );

        sw = Cross( v, id );
        sw = sw * (fBumpFactor / Dot(u, sw));

        bitangent = Normalize( position0 * sw.x + position1 * sw.y + position2 * sw.z );
        normal    = Normalize( Cross( tangent, bitangent ));
    }


} // AE::GeometryTools
