// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Draw animated grid using ray query in compute shader.
    Grid vertices updated every frame in compute shader.
    BLAS and TLAS are being rebuild at every frame.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   define AE_RAY_QUERY
#   include <aestyle.glsl.h>
#   define GEN_MESH
#   define GEN_INSTANCES
#   define RAYTRACE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>       rt          = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT" );
        RC<RTScene>     scene       = RTScene();
        RC<Buffer>      triangles   = Buffer();
        const uint      grid_size   = 64;
        RC<RTGeometry>  geom        = RTGeometry();
        RC<FPVCamera>   camera      = FPVCamera();

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 10.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s, s );
            camera.UpDownScale( s, s );
            camera.SideMovementScale( s );
        }

        // create geometry
        {
            array<float3>   positions;
            array<uint>     indices;
            GetGrid( grid_size, OUT positions, OUT indices );

            for (uint i = 0; i < positions.size(); ++i) {
                positions[i].x = ToSNorm( positions[i].x );
                positions[i].y = ToSNorm( positions[i].y );
            }

            triangles.Uint(         "gridSize",     grid_size );
            triangles.Float(        "invGridSize",  1.f / float(grid_size) );
            triangles.FloatArray(   "positions",    positions );
            triangles.FloatArray(   "normals",      positions );
            triangles.UIntArray(    "indices",      indices );

            geom.AddIndexedTriangles( triangles, triangles );

            scene.AddInstance( geom, float3(0.f, 0.f, 2.f), RTInstanceCustomIndex(0) );
        }

        // render loop
        {
            RC<ComputePass>     upd_verts = ComputePass( "", "GEN_MESH", EPassFlags::Enable_ShaderTrace );
            upd_verts.ArgInOut( "un_Triangles", triangles );
            upd_verts.LocalSize( 8, 8 );
            upd_verts.DispatchThreads( grid_size, grid_size );
        }{
        //  RC<ComputePass>     upd_inst = ComputePass( "", "GEN_INSTANCES" );
        //  upd_inst.ArgInOut( "un_Instances", scene.InstanceBuffer() );
        //  upd_inst.LocalSize( 1 );
        //  upd_inst.DispatchThreads( scene.InstanceCount() );
        }{
            BuildRTGeometry( geom );
            BuildRTScene( scene );
        }{
            RC<ComputePass>     rtrace = ComputePass( "", "RAYTRACE" );
            rtrace.ArgIn(   camera );
            rtrace.ArgIn(   "un_Triangles", triangles );
            rtrace.ArgOut(  "un_OutImage",  rt );
            rtrace.ArgIn(   "un_RtScene",   scene );
            rtrace.LocalSize( 8, 8 );
            rtrace.DispatchThreads( rt.Dimension() );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MESH
    #include "Math.glsl"
    #include "GlobalIndex.glsl"
    #include "Normal.glsl"

    ND_ float  BumpStep (float x, const float edge0, const float edge1)
    {
        x = Saturate( (x - edge0) / (edge1 - edge0) );
        return 1.0 - Abs(x - 0.5) * 2.0;
    }

    ND_ float  WaveAnim (const float2 snorm, const float timeBias)
    {
        const float wave_width  = 0.2;
        float       wave_r      = 1.5 * Fract( (un_PerPass.time + timeBias) * 0.5 );
        float       wave        = BumpStep( Length(snorm), wave_r, wave_r + wave_width );
        return wave * 0.1;
    }

    ND_ float3  GetPosition (const float2 snorm) {
        return float3( snorm, Max( WaveAnim( snorm, 0.f ), WaveAnim( snorm, 0.5f )) );
    }

    ND_ float3  GetPosition (const float2 snorm, const float2 offset) {
        return GetPosition( snorm + offset );
    }

    ND_ float3  GetNormal (const float2 snorm)
    {
        float3  norm;
        SmoothNormal3x3f( OUT norm, GetPosition, snorm, un_Triangles.invGridSize );
        return norm;
    }

    void  Main ()
    {
        const int   idx     = GetGlobalIndex();
        float2      snorm   = GetGlobalCoordSNorm().xy;

        un_Triangles.positions[idx] = GetPosition( snorm );
        un_Triangles.normals[idx]   = GetNormal( snorm );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_INSTANCES
    #include "GlobalIndex.glsl"
    #include "HWRayTracing.glsl"
    #include "Matrix.glsl"

    void  Main ()
    {
        const int   idx         = GetGlobalIndex();
        float3x4    transform   = f3x4_Rotate( Pi() * un_PerPass.time * 0.2f, float3(0.f, 1.f, 0.f) );

        un_Instances.elements[idx].transform = transform;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef RAYTRACE
    #include "GlobalIndex.glsl"
    #include "HWRayTracing.glsl"

    ND_ float4  MissShader ()
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    ND_ float4  HitShader (const float2 barycentric, const uint primitiveId)
    {
        float3  n0      = un_Triangles.normals[ un_Triangles.indices[ primitiveId * 3 + 0 ]];
        float3  n1      = un_Triangles.normals[ un_Triangles.indices[ primitiveId * 3 + 1 ]];
        float3  n2      = un_Triangles.normals[ un_Triangles.indices[ primitiveId * 3 + 2 ]];
        float3  norm    = BaryLerp( n0, n1, n2, barycentric );
        return float4(ToUNorm(norm), 1.f);
    }

    void  Main ()
    {
        Ray             ray     = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
        gl::RayQuery    ray_query;
        float4          color;

        gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::None,
                                /*cullMask*/0xFF, ray.origin, /*Tmin*/ray.t, ray.dir, /*Tmax*/un_PerPass.camera.clipPlanes.y );

        while ( gl.rayQuery.Proceed( ray_query ))
        {
            if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
                gl.rayQuery.ConfirmIntersection( ray_query );
        }

        if ( GetCommittedIntersectionType( ray_query ) == gl::RayQueryCommittedIntersection::None )
        {
            color = MissShader();
        }
        else
        {
            color = HitShader( GetCommittedIntersectionBarycentrics( ray_query ),
                               GetCommittedIntersectionPrimitiveIndex( ray_query ));
        }

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
    }

#endif
//-----------------------------------------------------------------------------
