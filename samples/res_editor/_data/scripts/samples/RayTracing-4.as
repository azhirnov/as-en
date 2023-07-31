// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor>
#   define AE_RAY_QUERY
#   define AE_RTAS_BUILD
#   include <aestyle.glsl.h>
#   define UPD_INSTANCES
#   define TRACE_RAYS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>   rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT" );
        RC<RTScene> scene   = RTScene();

        // create geometry
        {
            RC<RTGeometry>  geom        = RTGeometry();
            RC<Buffer>      triangles   = Buffer();
            array<float2>   positions   = { float2(0.f, -0.2f), float2(0.2f, 0.2f), float2(-0.2f, 0.2f) };
            array<uint>     indices     = { 0, 1, 2 };

            triangles.FloatArray(   "positions",    positions );
            triangles.UIntArray(    "indices",      indices );

            geom.AddIndexedTriangles( triangles, triangles );

            for (uint i = 0; i < 12; ++i) {
                scene.AddInstance( geom );
            }
        }

        // render loop
        {
            RC<ComputePass>     inst_pass = ComputePass( "", "UPD_INSTANCES" );
            inst_pass.ArgInOut( "un_Instances",     scene.InstanceBuffer() );
            inst_pass.ArgInOut( "un_IndirectCmd",   scene.IndirectBuffer() );
            inst_pass.LocalSize( 1 );
            inst_pass.DispatchThreads( scene.InstanceCount() );

            // update RTScene using instance data from prev pass
            BuildRTSceneIndirect( scene );

            RC<ComputePass>     rt_pass = ComputePass( "", "TRACE_RAYS", EPassFlags::Enable_ShaderTrace );
            rt_pass.ArgOut( "un_OutImage",  rt );
            rt_pass.ArgIn(  "un_RtScene",   scene );
            rt_pass.LocalSize( 8, 8 );
            rt_pass.DispatchThreads( rt.Dimension() );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef UPD_INSTANCES
    #include "GlobalIndex.glsl"
    #include "HWRayTracing.glsl"
    #include "Matrix.glsl"

    void  Main ()
    {
        const int   idx         = GetGlobalIndex();
        const int   inst_count  = un_Instances.elements.length();
        float       time        = Pi() * un_PerPass.time * 0.2f + float(idx);
        float3x4    transform   = f3x4_Rotate( time, float3(0.f, 0.f, 1.f) );
        float3      pos         = float3( ToSNorm( float(idx % 4) * 0.3 ),
                                          ToSNorm( float(idx / 4) * 0.3 ) + 0.5,
                                          0.0 );

        transform = Translate( transform, pos );

        un_Instances.elements[idx].transform = transform;

        if ( idx == 0 )
        {
            un_IndirectCmd.elements[0].primitiveCount =
                Clamp( int(Fract(un_PerPass.time * 0.5) * inst_count + 0.5), 1, inst_count );
        }
    }

#endif
//-----------------------------------------------------------------------------
#ifdef TRACE_RAYS
    #include "GlobalIndex.glsl"

    ND_ float4  MissShader ()
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    ND_ float4  HitShader (const float3 barycentrics, uint id)
    {
        return float4(barycentrics, 1.0f);
    }

    void  Main ()
    {
        const float2    uv          = GetGlobalCoordSNormCorrected() * 1.5f;
        const float3    origin      = float3(uv, -1.0f);
        const float3    direction   = float3(0.0f, 0.0f, 1.0f);
        gl::RayQuery    ray_query;
        float4          color;

        gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::None,
                                /*cullMask*/0xFF, origin, /*Tmin*/0.0f, direction, /*Tmax*/10.0f );

        while ( gl.rayQuery.Proceed( ray_query ))
        {
            if ( gl::RayQueryCandidateIntersection(gl.rayQuery.GetIntersectionType( ray_query, false )) == gl::RayQueryCandidateIntersection::Triangle )
                gl.rayQuery.ConfirmIntersection( ray_query );
        }

        if ( gl::RayQueryCommittedIntersection(gl.rayQuery.GetIntersectionType( ray_query, true )) == gl::RayQueryCommittedIntersection::None )
        {
            color = MissShader();
        }
        else
        {
            float2  attribs      = gl.rayQuery.GetIntersectionBarycentrics( ray_query, true );
            float3  barycentrics = float3( 1.0f - attribs.x - attribs.y, attribs.x, attribs.y );
                    color        = HitShader( barycentrics, gl.rayQuery.GetIntersectionInstanceCustomIndex( ray_query, true ));
        }

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
    }

#endif
//-----------------------------------------------------------------------------
