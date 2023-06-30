// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   define GEN_HEIGHT
#   define GEN_COLOR
#   include <res_editor>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain (RC<Collection> collection)
    {
        RC<Image>       height_map      = collection.Image("height");
        RC<Image>       normal_map      = collection.Image("normal");
        RC<Image>       albedo_map      = collection.Image("albedo");
        RC<Image>       emission_map    = collection.Image("emission");
        const uint2     local_size      = uint2( 8, 8 );
        const uint2     face_size       = height_map.Dimension2();
        const uint      layers          = height_map.ArrayLayers();

        // height & normal
        {
            RC<ComputePass>     gen_height  = ComputePass( "", "GEN_HEIGHT", EPassFlags::None );
            const uint2         group_count = (face_size + local_size - 3) / (local_size - 2);  // 1 pixel border

            gen_height.ArgOut( "un_OutHeight", height_map );
            gen_height.ArgOut( "un_OutNormal", normal_map );

            gen_height.LocalSize( local_size );
            gen_height.DispatchGroups( uint3( group_count, layers ));
        }

        // color
        {
            RC<ComputePass>     gen_color = ComputePass( "", "GEN_COLOR", EPassFlags::None );

            gen_color.ArgIn(  "un_HeightMap",   height_map );
            gen_color.ArgIn(  "un_NormalMap",   normal_map );
            gen_color.ArgOut( "un_OutAlbedo",   albedo_map );
            gen_color.ArgOut( "un_OutEmission", emission_map );

            gen_color.LocalSize( local_size );
            gen_color.DispatchThreads( uint3( face_size, layers ));
        }

        GenMipmaps( height_map );
        GenMipmaps( normal_map );
        GenMipmaps( albedo_map );
        GenMipmaps( emission_map );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_HEIGHT
    #define PROJECTION  CM_TangentialSC_Forward

    #include "GlobalIndex.glsl"
    #include "SDF.glsl"
    #include "CubeMap.glsl"
    #include "Hash.glsl"
    #include "Noise.glsl"

    int2    faceDim;
    int     face;

    float  FBM (in float3 coord)
    {
        float   total       = 0.0;
        float   amplitude   = 1.0;
        float   freq        = 1.0;

        for (int i = 0; i < 7; ++i)
        {
            total += GradientNoise( coord*freq ) * amplitude;
            freq *= 2.5;
            amplitude *= 0.5;
        }
        return total;
    }


    float4  GetPosition (const int2 coord)
    {
        float2  ncoord  = ToSNorm( float2(coord) / float2(faceDim - 1) );
        float3  pos     = PROJECTION( ncoord, face );
        float   height  = FBM( pos ) * 2.0;

        return float4( pos, height );
    }


    // positions with 1 pixel border for normals calculation
    shared float3  s_Positions[ gl.WorkGroupSize.x * gl.WorkGroupSize.y ];


    float3  ReadPosition (int2 local)
    {
        local += 1;
        return s_Positions[ local.x + local.y * gl.WorkGroupSize.x ];
    }


    void  Main ()
    {
        faceDim = gl.image.GetSize( un_OutHeight ).xy;
        face    = GetGroupCoord().z;

        const int2      local       = GetLocalCoord().xy - 1;
        const int2      lsize       = GetLocalSize().xy - 2;
        const int2      group       = GetGroupCoord().xy;
        const int2      coord       = local + lsize * group;
        const int3      coord3      = int3( coord, face );
        const float4    pos_h       = GetPosition( coord );
        const float3    pos         = pos_h.xyz * (1.0 + pos_h.w);
        const bool      is_active   = IsInsideRect( local, int2(0), lsize );

        s_Positions[ GetLocalIndex() ] = pos;

        memoryBarrierShared();
        barrier();
        memoryBarrierShared();

        // calculate smooth normal
        if ( is_active )
        {
            const int3      offset  = int3(-1, 0, 1);
            const float3    v0      = ReadPosition( local + offset.xx );
            const float3    v1      = ReadPosition( local + offset.yx );
            const float3    v2      = ReadPosition( local + offset.zx );
            const float3    v3      = ReadPosition( local + offset.xy );
            const float3    v4      = pos;
            const float3    v5      = ReadPosition( local + offset.zy );
            const float3    v6      = ReadPosition( local + offset.xz );
            const float3    v7      = ReadPosition( local + offset.yz );
            const float3    v8      = ReadPosition( local + offset.zz );
            float3          normal  = float3(0.0);

            normal += Cross( v1 - v4, v2 - v4 );    // 1-4, 2-4
            normal += Cross( v5 - v4, v8 - v4 );    // 5-4, 8-4
            normal += Cross( v7 - v4, v6 - v4 );    // 7-4, 6-4
            normal += Cross( v3 - v4, v0 - v4 );    // 3-4, 0-4
            normal  = Normalize( normal );

            gl.image.Store( un_OutHeight, coord3, float4(pos_h.w) );
            gl.image.Store( un_OutNormal, coord3, float4(normal, 0.0) );
        }
    }

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_COLOR
    #define PROJECTION  CM_TangentialSC_Forward

    #include "GlobalIndex.glsl"
    #include "SDF.glsl"
    #include "CubeMap.glsl"
    #include "Hash.glsl"
    #include "Noise.glsl"
    #include "Color.glsl"

    int2    faceDim;
    int     face;

    shared float3  s_Positions[ gl.WorkGroupSize.x * gl.WorkGroupSize.y ];
    shared float3  s_Normals  [ gl.WorkGroupSize.x * gl.WorkGroupSize.y ];


    void Main ()
    {
        faceDim = gl.image.GetSize( un_HeightMap ).xy;
        face    = GetGroupCoord().z;

        const int3  coord3  = GetGlobalCoord();
        const int2  coord   = coord3.xy;

        // read height map
        float3  sphere_pos;
        {
            float   height  = gl.image.Load( un_HeightMap, coord3 ).r;
            float3  norm    = gl.image.Load( un_NormalMap, coord3 ).rgb;
            float2  ncoord  = ToSNorm( float2(coord) / float2(faceDim - 1) );
            sphere_pos      = PROJECTION( ncoord, face );

            s_Positions[ GetLocalIndex() ] = sphere_pos * (1.0 + height);
            s_Normals[ GetLocalIndex() ]   = norm;
        }

        memoryBarrierShared();
        barrier();
        memoryBarrierShared();


        float3  albedo      = float3(1.0);
        float   emission    = 0.0;
        float   temperature = 0.0;
        float3  pos         = s_Positions[ GetLocalIndex() ];

        float   biom        = DHash13( Voronoi( Turbulence( sphere_pos * 8.0, 1.0, 2.0, 0.6, 7 ), float2(3.9672) ).icenter );
        int     mtr_id      = int(biom * 255.0f) & 0xF;

        albedo = HSVtoRGB( float3( biom, 1.0, 1.0 ));

        gl.image.Store( un_OutAlbedo, coord3, float4(albedo, 0.0) );
        gl.image.Store( un_OutEmission, coord3, float4(emission, temperature, 0.0, 0.0) );
    }

#endif
//-----------------------------------------------------------------------------
