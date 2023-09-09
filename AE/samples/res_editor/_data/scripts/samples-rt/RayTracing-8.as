// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Simple ray tracing with recursion.
    Draw spheres, cubes, plane and animated omni light.
    Vertex attributes passed to shader as a buffer reference.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   define SH_RAY_CHIT
#   include <aestyle.glsl.h>
#   define PRIMARY_MISS
#   define SHADOW_MISS
#   define PRIMARY_HIT
#   define SHADOW_HIT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    Random  _rnd;
    float   Rnd () { return _rnd.Uniform( 0.f, 1.f ); }

    void ASmain ()
    {
        // initialize
        RC<Image>       rt          = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT" );
        RC<RTScene>     scene       = RTScene();
        RC<FPVCamera>   camera      = FPVCamera();
        RC<Buffer>      geom_data   = Buffer();

        const string    cm_addr = "res/humus/Teide/";   const string  cm_ext = ".jpg";  const uint2 cm_dim (2048);

        RC<Image>       cubemap         = Image( EPixelFormat::RGBA8_UNorm, cm_dim, ImageLayer(6), MipmapLevel(~0) );   cubemap.Name( "Cubemap tex" );
        RC<Image>       cubemap_view    = cubemap.CreateView( EImage::Cube );

        array<ulong>    normals_addr;
        array<ulong>    texcoords_addr;
        array<ulong>    indices_addr;

        // load cubemap
        {
            cubemap.LoadLayer( cm_addr+ "posx" +cm_ext, 0, ImageLoadOpFlags::GenMipmaps );  // -Z
            cubemap.LoadLayer( cm_addr+ "negx" +cm_ext, 1, ImageLoadOpFlags::GenMipmaps );  // +Z
            cubemap.LoadLayer( cm_addr+ "posy" +cm_ext, 2, ImageLoadOpFlags::GenMipmaps );  // +Y   - up
            cubemap.LoadLayer( cm_addr+ "negy" +cm_ext, 3, ImageLoadOpFlags::GenMipmaps );  // -Y   - down
            cubemap.LoadLayer( cm_addr+ "posz" +cm_ext, 4, ImageLoadOpFlags::GenMipmaps );  // -X
            cubemap.LoadLayer( cm_addr+ "negz" +cm_ext, 5, ImageLoadOpFlags::GenMipmaps );  // +X
        }

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 30.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s, s );
            camera.UpDownScale( s, s );
            camera.SideMovementScale( s );
        }

        const uint  hit_group_count = 1;
        scene.HitGroupStride( hit_group_count );

        // create cube
        {
            RC<RTGeometry>  geom    = RTGeometry();
            RC<Buffer>      cube    = Buffer();

            array<float3>   positions;
            array<float3>   normals;
            array<float3>   tangents;
            array<float3>   bitangents;
            array<float2>   texcoords;
            array<uint>     indices;
            GetCube( OUT positions, OUT normals, OUT tangents, OUT bitangents, OUT texcoords, OUT indices );
            // TODO: sphere

            uint    pos_off     = cube.FloatArray(  "positions",    positions );
            uint    norm_off    = cube.FloatArray(  "normals",      normals );
            uint    texc_off    = cube.FloatArray(  "texcoords",    texcoords );
            uint    idx_off     = cube.UIntArray(   "indices",      indices );

            geom.AddIndexedTriangles( cube, cube );

            scene.AddInstance( geom, RTInstanceTransform( float3(0.f, 2.0f, 4.f), float3(0.f, ToRad(45.f), 0.f) ));

            normals_addr    .push_back( cube.DeviceAddress() + norm_off );
            texcoords_addr  .push_back( cube.DeviceAddress() + texc_off );
            indices_addr    .push_back( cube.DeviceAddress() + idx_off );

            geom_data.AddReference( cube );
        }

        // render loop
        {
            RC<RayTracingPass>      pass = RayTracingPass( EPassFlags::Enable_ShaderTrace );
            pass.ArgIn(  camera );
            pass.ArgOut( "un_OutImage",     rt );
            pass.ArgIn(  "un_RtScene",      scene );
            pass.ArgIn(  "un_Geometry",     geom_data );
            pass.ArgIn(  "un_CubeMap",      cubemap_view,       Sampler_LinearMipmapRepeat );
            pass.Dispatch( rt.Dimension() );

            pass.ColorSelector( "iMaterialColor",       RGBA32f(0.33f, 0.93f, 0.29f, 1.f) );
            pass.ColorSelector( "iReflectionColorMask", RGBA32f(0.22f, 0.83f, 0.93f, 1.f) );
            pass.Slider( "iAbsorptionScale",    1.f,    10.f,   2.5f );
            pass.Slider( "iIndexOfRefraction",  1.f,    3.f,    1.1f );
            pass.Slider( "iMaxRecursion",       1,      16,     5 );

            // setup SBT
            pass.RayGen( RTShader("") );
            pass.HitGroupStride( hit_group_count );

            pass.RayMiss( MissIndex(0), RTShader("") );

            for (uint i = 0; i < indices_addr.size(); ++i)
            {
                pass.TriangleHit( RayIndex(0), InstanceIndex(i), RTShader("") );
            }
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------

#define PRIMARY_RAY_INDEX   0

struct PrimaryRayPayload
{
    // out
    float3  color;
    float   depth;

    // inout
    uint    recursion;

    // int
    int     wavelengthIdx;
};

layout(std430, buffer_reference, buffer_reference_align= 8) buffer readonly TexcoordsRef    { float2    uvs     []; };
layout(std430, buffer_reference, buffer_reference_align=16) buffer readonly NormalsRef      { float3    normals []; };
layout(std430, buffer_reference, buffer_reference_align= 4) buffer readonly IndicesRef      { uint      indices []; };

//-----------------------------------------------------------------------------
#if defined(SH_RAY_GEN) || defined(SH_RAY_CHIT)
    #include "Math.glsl"
    #include "HWRayTracing.glsl"

    #ifdef SH_RAY_GEN
        layout(location=PRIMARY_RAY_INDEX)  gl::RayPayload   PrimaryRayPayload  PrimaryRay;
    #else
        layout(location=PRIMARY_RAY_INDEX)  gl::RayPayloadIn PrimaryRayPayload  PrimaryRay;
    #endif

    void  CastPrimaryRay (in HWRay ray, uint recursion, int wavelengthIdx)
    {
        PrimaryRay.color            = float3(0.0);
        PrimaryRay.depth            = 0.0;
        PrimaryRay.recursion        = recursion;
        PrimaryRay.wavelengthIdx    = wavelengthIdx;

        if ( recursion > iMaxRecursion )
        {
            PrimaryRay.color = float3(0.95, 0.18, 0.95);
            return;
        }

        ray.missIndex       = PRIMARY_RAY_INDEX;
        ray.sbtRecordOffset = PRIMARY_RAY_INDEX;
        ray.sbtRecordStride = HitGroupStride;

        HWTraceRay( un_RtScene, ray, /*payload*/PRIMARY_RAY_INDEX );
    }

    void  CastPrimaryRay (const HWRay ray) {
        CastPrimaryRay( ray, 0, -1 );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
    #include "GlobalIndex.glsl"

    void Main ()
    {
        Ray     ray     = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
        HWRay   hwray   = HWRay_Create( ray, un_PerPass.camera.clipPlanes.y, PRIMARY_RAY_INDEX, HitGroupStride );

        // hitShader = RTSceneBuild::Instance::instanceSBTOffset + geometryIndex * sbtRecordStride + sbtRecordOffset

        CastPrimaryRay( hwray );

        gl.image.Store( un_OutImage, int2(gl.LaunchID), float4(PrimaryRay.color, 1.f) );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_MISS
    #include "Math.glsl"

    layout(location=PRIMARY_RAY_INDEX)  gl::RayPayloadIn PrimaryRayPayload  PrimaryRay;

    void Main ()
    {
        float3  uv = gl.WorldRayDirection * float3(1.f, -1.f, 1.f);

        PrimaryRay.color    = gl.texture.SampleLod( un_CubeMap, uv, 0.f ).rgb;
        PrimaryRay.depth    = gl.RayTmax;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_CHIT
    #include "PBR.glsl"

    gl::HitAttribute float2  in_HitAttribs;

    const float c_SmallOffset   = 0.0001;
    const float c_AirIOR        = 1.0;      // Air index of refraction


    ND_ float3  LightAbsorption (const float3 color, const float depth, const float absorption)
    {
        float   factor  = Saturate( Exp( -depth * (1.0f - absorption) * iAbsorptionScale ));
        return color1 * factor * iMaterialColor.rgb;
    }

    ND_ float3  BlendWithReflection (const float3 srcColor, const float3 reflectionColor, const float factor)
    {
        return Lerp( srcColor, reflectionColor * iReflectionColorMask.rgb, factor );
    }


    void  RayTrace (float3 normal, const uint recursion, const int wavelengthIdx)
    {
        float   absorption      = 0.5;
        float   relative_ior    = 1.0;
        float3  reflection_dir  = gl.WorldRayDirection;
        HWRay   hwray           = HWRay_Create();

        hwray.tMin  = c_SmallOffset;
        hwray.tMax  = 100.0;

        // Refraction at the interface between air and current object
        if ( gl.HitKind == gl::TriangleHitKind::FrontFacing )
        {
            relative_ior    = c_AirIOR / iIndexOfRefraction;
            reflection_dir  = Refract( reflection_dir, normal, relative_ior );
        }
        else
        // Refraction at the interface between current object and air
        if ( gl.HitKind == gl::TriangleHitKind::BackFacing )
        {
            relative_ior    = iIndexOfRefraction / c_AirIOR;
            normal          = -normal;
            reflection_dir  = Refract( reflection_dir, normal, relative_ior );
        }

        float   fresnel             = Fresnel( relative_ior, Dot( gl.WorldRayDirection, -normal ));
        float3  reflection_color    = float3(0.0);
        float3  result_color        = float3(0.0);

        // Reflection
        {
            hwray.rayOrigin = gl.WorldRayOrigin + gl.WorldRayDirection * gl.HitT + normal * c_SmallOffset;
            hwray.rayDir    = Reflect( gl.WorldRayDirection, normal );

            CastPrimaryRay( hwray, recursion+1, wavelengthIdx );
            reflection_color = PrimaryRay.color;

            if ( gl.HitKind == gl::TriangleHitKind::BackFacing )
            {
                reflection_color = LightAbsorption( reflection_color, PrimaryRay.depth, absorption );
            }
        }

        // Refraction
        if ( fresnel < 1.0f )
        {
            hwray.rayOrigin = gl.WorldRayOrigin + gl.WorldRayDirection * gl.HitT;
            hwray.rayDir    = reflection_dir;

            CastPrimaryRay( hwray, recursion+1, wavelengthIdx );
            result_color = PrimaryRay.color;

            if ( gl.HitKind == gl::TriangleHitKind::FrontFacing  or
                 recursion == 0 )
            {
                result_color = LightAbsorption( result_color, PrimaryRay.depth, absorption );
            }
        }

        // 'PrimaryRay' was modified in 'CastPrimaryRay()' so restore previous state
        // and set new color and depth
        PrimaryRay.color         = BlendWithReflection( result_color, reflection_color, fresnel );
        PrimaryRay.depth         = gl.HitT;
        PrimaryRay.recursion     = recursion;
        PrimaryRay.wavelengthIdx = wavelengthIdx;
    }


    void Main ()
    {
        NormalsRef      norm_addr       = NormalsRef( un_Geometry.normals[ gl.InstanceID ]);
        IndicesRef      idx_addr        = IndicesRef( un_Geometry.indices[ gl.InstanceID ]);

        const uint      idx             = gl.PrimitiveID*3;
        const float3    normal          = Normalize( BaryLerp(  norm_addr.normals[ idx_addr.indices[ idx+0 ]],
                                                                norm_addr.normals[ idx_addr.indices[ idx+1 ]],
                                                                norm_addr.normals[ idx_addr.indices[ idx+2 ]],
                                                                in_HitAttribs ) * float3x3(gl.ObjectToWorld3x4) );

        RayTrace( normal, PrimaryRay.recursion, PrimaryRay.wavelengthIdx );
    }

#endif
//-----------------------------------------------------------------------------
