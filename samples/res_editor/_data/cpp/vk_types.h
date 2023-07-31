#ifndef CameraData_DEFINED
#   define CameraData_DEFINED
    // size: 368, align: 16
    struct CameraData
    {
        static constexpr auto  TypeName = ShaderStructName{"CameraData"};

        float4x4_storage  viewProj;
        float4x4_storage  invViewProj;
        float4x4_storage  proj;
        float4x4_storage  view;
        float3  pos;
        StaticArray< float4, 6 >    frustum;
    };
    STATIC_ASSERT( offsetof(CameraData, viewProj) == 0 );
    STATIC_ASSERT( offsetof(CameraData, invViewProj) == 64 );
    STATIC_ASSERT( offsetof(CameraData, proj) == 128 );
    STATIC_ASSERT( offsetof(CameraData, view) == 192 );
    STATIC_ASSERT( offsetof(CameraData, pos) == 256 );
    STATIC_ASSERT( offsetof(CameraData, frustum) == 272 );
    STATIC_ASSERT( sizeof(CameraData) == 368 );
#endif

#ifndef CameraData_DEFINED
#   define CameraData_DEFINED
    // size: 368, align: 16
    struct CameraData
    {
        static constexpr auto  TypeName = ShaderStructName{"CameraData"};

        float4x4_storage  viewProj;
        float4x4_storage  invViewProj;
        float4x4_storage  proj;
        float4x4_storage  view;
        float3  pos;
        StaticArray< float4, 6 >    frustum;
    };
    STATIC_ASSERT( offsetof(CameraData, viewProj) == 0 );
    STATIC_ASSERT( offsetof(CameraData, invViewProj) == 64 );
    STATIC_ASSERT( offsetof(CameraData, proj) == 128 );
    STATIC_ASSERT( offsetof(CameraData, view) == 192 );
    STATIC_ASSERT( offsetof(CameraData, pos) == 256 );
    STATIC_ASSERT( offsetof(CameraData, frustum) == 272 );
    STATIC_ASSERT( sizeof(CameraData) == 368 );
#endif

#ifndef ShadertoyUB_DEFINED
#   define ShadertoyUB_DEFINED
    // size: 848, align: 16
    struct ShadertoyUB
    {
        static constexpr auto  TypeName = ShaderStructName{"ShadertoyUB"};

        float3  resolution;
        float  time;
        float  timeDelta;
        uint  frame;
        uint  seed;
        float4  channelTime;
        StaticArray< float4, 4 >    channelResolution;
        float4  mouse;
        float4  date;
        float  sampleRate;
        float  customKeys;
        CameraData  camera;
        StaticArray< float4, 4 >    floatSliders;
        StaticArray< int4, 4 >    intSliders;
        StaticArray< float4, 4 >    colors;
        StaticArray< float4, 4 >    floatConst;
        StaticArray< int4, 4 >    intConst;
    };
    STATIC_ASSERT( offsetof(ShadertoyUB, resolution) == 0 );
    STATIC_ASSERT( offsetof(ShadertoyUB, time) == 16 );
    STATIC_ASSERT( offsetof(ShadertoyUB, timeDelta) == 20 );
    STATIC_ASSERT( offsetof(ShadertoyUB, frame) == 24 );
    STATIC_ASSERT( offsetof(ShadertoyUB, seed) == 28 );
    STATIC_ASSERT( offsetof(ShadertoyUB, channelTime) == 32 );
    STATIC_ASSERT( offsetof(ShadertoyUB, channelResolution) == 48 );
    STATIC_ASSERT( offsetof(ShadertoyUB, mouse) == 112 );
    STATIC_ASSERT( offsetof(ShadertoyUB, date) == 128 );
    STATIC_ASSERT( offsetof(ShadertoyUB, sampleRate) == 144 );
    STATIC_ASSERT( offsetof(ShadertoyUB, customKeys) == 148 );
    STATIC_ASSERT( offsetof(ShadertoyUB, camera) == 160 );
    STATIC_ASSERT( offsetof(ShadertoyUB, floatSliders) == 528 );
    STATIC_ASSERT( offsetof(ShadertoyUB, intSliders) == 592 );
    STATIC_ASSERT( offsetof(ShadertoyUB, colors) == 656 );
    STATIC_ASSERT( offsetof(ShadertoyUB, floatConst) == 720 );
    STATIC_ASSERT( offsetof(ShadertoyUB, intConst) == 784 );
    STATIC_ASSERT( sizeof(ShadertoyUB) == 848 );
#endif

#ifndef ComputePassUB_DEFINED
#   define ComputePassUB_DEFINED
    // size: 736, align: 16
    struct ComputePassUB
    {
        static constexpr auto  TypeName = ShaderStructName{"ComputePassUB"};

        float  time;
        float  timeDelta;
        uint  frame;
        uint  seed;
        float4  mouse;
        float  customKeys;
        CameraData  camera;
        StaticArray< float4, 4 >    floatSliders;
        StaticArray< int4, 4 >    intSliders;
        StaticArray< float4, 4 >    colors;
        StaticArray< float4, 4 >    floatConst;
        StaticArray< int4, 4 >    intConst;
    };
    STATIC_ASSERT( offsetof(ComputePassUB, time) == 0 );
    STATIC_ASSERT( offsetof(ComputePassUB, timeDelta) == 4 );
    STATIC_ASSERT( offsetof(ComputePassUB, frame) == 8 );
    STATIC_ASSERT( offsetof(ComputePassUB, seed) == 12 );
    STATIC_ASSERT( offsetof(ComputePassUB, mouse) == 16 );
    STATIC_ASSERT( offsetof(ComputePassUB, customKeys) == 32 );
    STATIC_ASSERT( offsetof(ComputePassUB, camera) == 48 );
    STATIC_ASSERT( offsetof(ComputePassUB, floatSliders) == 416 );
    STATIC_ASSERT( offsetof(ComputePassUB, intSliders) == 480 );
    STATIC_ASSERT( offsetof(ComputePassUB, colors) == 544 );
    STATIC_ASSERT( offsetof(ComputePassUB, floatConst) == 608 );
    STATIC_ASSERT( offsetof(ComputePassUB, intConst) == 672 );
    STATIC_ASSERT( sizeof(ComputePassUB) == 736 );
#endif

#ifndef SceneGraphicsPassUB_DEFINED
#   define SceneGraphicsPassUB_DEFINED
    // size: 720, align: 16
    struct SceneGraphicsPassUB
    {
        static constexpr auto  TypeName = ShaderStructName{"SceneGraphicsPassUB"};

        float2  resolution;
        float  time;
        float  timeDelta;
        uint  frame;
        uint  seed;
        CameraData  camera;
        StaticArray< float4, 4 >    floatSliders;
        StaticArray< int4, 4 >    intSliders;
        StaticArray< float4, 4 >    colors;
        StaticArray< float4, 4 >    floatConst;
        StaticArray< int4, 4 >    intConst;
    };
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, resolution) == 0 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, time) == 8 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, timeDelta) == 12 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, frame) == 16 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, seed) == 20 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, camera) == 32 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, floatSliders) == 400 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, intSliders) == 464 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, colors) == 528 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, floatConst) == 592 );
    STATIC_ASSERT( offsetof(SceneGraphicsPassUB, intConst) == 656 );
    STATIC_ASSERT( sizeof(SceneGraphicsPassUB) == 720 );
#endif

#ifndef SceneRayTracingPassUB_DEFINED
#   define SceneRayTracingPassUB_DEFINED
    // size: 720, align: 16
    struct SceneRayTracingPassUB
    {
        static constexpr auto  TypeName = ShaderStructName{"SceneRayTracingPassUB"};

        float2  resolution;
        float  time;
        float  timeDelta;
        uint  frame;
        uint  seed;
        CameraData  camera;
        StaticArray< float4, 4 >    floatSliders;
        StaticArray< int4, 4 >    intSliders;
        StaticArray< float4, 4 >    colors;
        StaticArray< float4, 4 >    floatConst;
        StaticArray< int4, 4 >    intConst;
    };
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, resolution) == 0 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, time) == 8 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, timeDelta) == 12 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, frame) == 16 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, seed) == 20 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, camera) == 32 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, floatSliders) == 400 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, intSliders) == 464 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, colors) == 528 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, floatConst) == 592 );
    STATIC_ASSERT( offsetof(SceneRayTracingPassUB, intConst) == 656 );
    STATIC_ASSERT( sizeof(SceneRayTracingPassUB) == 720 );
#endif

#ifndef SphericalCubeMaterialUB_DEFINED
#   define SphericalCubeMaterialUB_DEFINED
    // size: 68 (80), align: 16
    struct SphericalCubeMaterialUB
    {
        static constexpr auto  TypeName = ShaderStructName{"SphericalCubeMaterialUB"};

        float4x4_storage  transform;
        float  tessLevel;
    };
    STATIC_ASSERT( offsetof(SphericalCubeMaterialUB, transform) == 0 );
    STATIC_ASSERT( offsetof(SphericalCubeMaterialUB, tessLevel) == 64 );
    STATIC_ASSERT( sizeof(SphericalCubeMaterialUB) == 80 );
#endif

#ifndef UnifiedGeometryMaterialUB_DEFINED
#   define UnifiedGeometryMaterialUB_DEFINED
    // size: 64, align: 16
    struct UnifiedGeometryMaterialUB
    {
        static constexpr auto  TypeName = ShaderStructName{"UnifiedGeometryMaterialUB"};

        float4x4_storage  transform;
    };
    STATIC_ASSERT( offsetof(UnifiedGeometryMaterialUB, transform) == 0 );
    STATIC_ASSERT( sizeof(UnifiedGeometryMaterialUB) == 64 );
#endif

