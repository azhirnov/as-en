// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Exact OIT
    Ray tracing from back to front, only front faces.
    Used multiple ray casting.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   define AE_RAY_QUERY
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    Random  _rnd;
    float   Rnd ()  { return _rnd.Uniform( 0.f, 1.f ); }
    float3  Rnd3 () { return float3(Rnd(), Rnd(), Rnd()); }

    void ASmain ()
    {
        // initialize
        RC<Image>       rt              = Image( EPixelFormat::RGBA16F, SurfaceSize() );    rt.Name( "RT-Color" );
        RC<FPVCamera>   camera          = FPVCamera();
        RC<Buffer>      sphere          = Buffer();
        RC<Buffer>      color_per_inst  = Buffer();
        array<float4>   colors;
        RC<RTGeometry>  geom            = RTGeometry();
        RC<RTScene>     scene           = RTScene();

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 15.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s, s );
            camera.UpDownScale( s, s );
            camera.SideMovementScale( s );

            camera.Position( float3(0.f, 0.f, -3.f));
        }

        // create sphere
        {
            array<float3>   positions;
            array<uint>     indices;
            GetSphere( 7, OUT positions, OUT indices );

            sphere.FloatArray( "positions", positions );
            sphere.UIntArray(  "indices",   indices );

            geom.AddIndexedTriangles( sphere, sphere );
        }

        // setup draw tasks
        {
            int3        ipos     (0);
            const int3  grid_dim (8);

            for (ipos.z = 0; ipos.z < grid_dim.z; ++ipos.z)
            for (ipos.y = 0; ipos.y < grid_dim.y; ++ipos.y)
            for (ipos.x = 0; ipos.x < grid_dim.x; ++ipos.x)
            {
                int     idx     = ToLinear( ipos, grid_dim );
                float   scale1  = 0.2f;
                float   scale2  = 2.5f;
                float3  pos     = (float3(ipos - grid_dim / 2) * scale2 + ToSNorm(Rnd3())) * scale1;
                float   size    = Remap( 0.f, 1.f, 0.25f, 1.f, Rnd() ) * scale1;                // sphere size
                float4  color   = float4(Rainbow( float(idx) / Area(grid_dim) ));   color.w = 0.5;

                colors.push_back( color );

                scene.AddInstance( geom, RTInstanceTransform( pos, float3(0.f), size ), RTInstanceCustomIndex(idx) );
            }

            color_per_inst.FloatArray( "colors", colors );
        }

        // render loop
        {
            RC<ComputePass>     pass = ComputePass( "", EPassFlags::Enable_ShaderTrace );
            pass.ArgIn(  camera );
            pass.ArgOut( "un_OutImage",         rt );
            pass.ArgIn(  "un_RtScene",          scene );
            pass.ArgIn(  "un_Geometry",         sphere );
            pass.ArgIn(  "un_ColorPerInstance", color_per_inst );
            pass.LocalSize( 8, 8 );
            pass.DispatchThreads( rt.Dimension() );
        }

        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
    #include "GlobalIndex.glsl"
    #include "HWRayTracing.glsl"
    #include "Blend.glsl"

    const float c_SmallOffset   = 0.0001;
    const uint  c_MaxIterations = 128;

    ND_ float4  HitShader (const uint instanceId)
    {
        return un_ColorPerInstance.colors[instanceId];
    }

    ND_ bool  CastRay (inout HWRay ray, out float4 outColor)
    {
        if ( ray.tMin >= ray.tMax )
            return false;

        gl::RayQuery    ray_query;
        RayQuery_Init( ray_query, un_RtScene, ray );

        while ( gl.rayQuery.Proceed( ray_query ))
        {
            if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
                gl.rayQuery.ConfirmIntersection( ray_query );
        }

        if ( GetCommittedIntersectionType( ray_query ) == gl::RayQueryCommittedIntersection::Triangle )
        {
            float2  attribs     = GetCommittedIntersectionBarycentrics( ray_query );
                    outColor    = HitShader( GetCommittedIntersectionInstanceCustomIndex( ray_query ));

            // 'GetCommittedIntersectionT()' - distance from 'origin' to intersection point
            ray.tMin = GetCommittedIntersectionT( ray_query ) + c_SmallOffset;
            return true;
        }
        return false;
    }


    void  Main ()
    {
        const Ray   ray     = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
        HWRay       hwray   = HWRay_Create( ray, un_PerPass.camera.clipPlanes.y );
        float4      color   = float4(0.0);

        // back to front
        hwray.rayOrigin = hwray.rayOrigin + hwray.rayDir * hwray.tMax;
        hwray.tMin      = 0.0;
        hwray.rayDir    = -hwray.rayDir;
        hwray.rayFlags  = gl::RayFlags::CullBackFacingTriangles;

        for (uint i = 0; i < c_MaxIterations; ++i)
        {
            float4  src;
            if ( ! CastRay( hwray, OUT src ))
                break;

            SeparateBlendParams     p;
            p.srcColor      = src;      // from shader
            p.dstColor      = color;    // from render target
            p.srcBlendRGB   = EBlendFactor_SrcAlpha;
            p.srcBlendA     = EBlendFactor_One;
            p.dstBlendRGB   = EBlendFactor_OneMinusSrcAlpha;
            p.dstBlendA     = EBlendFactor_OneMinusSrcAlpha;
            p.blendOpRGB    = EBlendOp_Add;
            p.blendOpA      = EBlendOp_Add;

            color = BlendFn( p );
        }

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
    }

#endif
//-----------------------------------------------------------------------------
