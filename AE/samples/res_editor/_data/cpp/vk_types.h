//e0b8bb51
#ifndef CameraData_DEFINED
#	define CameraData_DEFINED
	// size: 384, align: 16
	struct CameraData
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x8142e66cu}};  // 'CameraData'

		float4x4_storage  viewProj;
		float4x4_storage  invViewProj;
		float4x4_storage  proj;
		float4x4_storage  view;
		float3  pos;
		float2  clipPlanes;
		float  zoom;
		StaticArray< float4, 6 >    frustum;
	};
#endif
	StaticAssert( offsetof(CameraData, viewProj) == 0 );
	StaticAssert( offsetof(CameraData, invViewProj) == 64 );
	StaticAssert( offsetof(CameraData, proj) == 128 );
	StaticAssert( offsetof(CameraData, view) == 192 );
	StaticAssert( offsetof(CameraData, pos) == 256 );
	StaticAssert( offsetof(CameraData, clipPlanes) == 272 );
	StaticAssert( offsetof(CameraData, zoom) == 280 );
	StaticAssert( offsetof(CameraData, frustum) == 288 );
	StaticAssert( sizeof(CameraData) == 384 );

#ifndef ShadertoyUB_DEFINED
#	define ShadertoyUB_DEFINED
	// size: 1200, align: 16
	struct ShadertoyUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa31fc14bu}};  // 'ShadertoyUB'

		float3  resolution;
		float  time;
		float  timeDelta;
		uint  frame;
		uint  passFrameId;
		uint  seed;
		float4  channelTime;
		StaticArray< float4, 4 >    channelResolution;
		float4  mouse;
		float4  date;
		float  sampleRate;
		float  customKeys;
		float  pixToMm;
		CameraData  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >    intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >    intConst;
	};
#endif
	StaticAssert( offsetof(ShadertoyUB, resolution) == 0 );
	StaticAssert( offsetof(ShadertoyUB, time) == 16 );
	StaticAssert( offsetof(ShadertoyUB, timeDelta) == 20 );
	StaticAssert( offsetof(ShadertoyUB, frame) == 24 );
	StaticAssert( offsetof(ShadertoyUB, passFrameId) == 28 );
	StaticAssert( offsetof(ShadertoyUB, seed) == 32 );
	StaticAssert( offsetof(ShadertoyUB, channelTime) == 48 );
	StaticAssert( offsetof(ShadertoyUB, channelResolution) == 64 );
	StaticAssert( offsetof(ShadertoyUB, mouse) == 128 );
	StaticAssert( offsetof(ShadertoyUB, date) == 144 );
	StaticAssert( offsetof(ShadertoyUB, sampleRate) == 160 );
	StaticAssert( offsetof(ShadertoyUB, customKeys) == 164 );
	StaticAssert( offsetof(ShadertoyUB, pixToMm) == 168 );
	StaticAssert( offsetof(ShadertoyUB, camera) == 176 );
	StaticAssert( offsetof(ShadertoyUB, floatSliders) == 560 );
	StaticAssert( offsetof(ShadertoyUB, intSliders) == 688 );
	StaticAssert( offsetof(ShadertoyUB, colors) == 816 );
	StaticAssert( offsetof(ShadertoyUB, floatConst) == 944 );
	StaticAssert( offsetof(ShadertoyUB, intConst) == 1072 );
	StaticAssert( sizeof(ShadertoyUB) == 1200 );

#ifndef ComputePassUB_DEFINED
#	define ComputePassUB_DEFINED
	// size: 1088, align: 16
	struct ComputePassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xaba36a57u}};  // 'ComputePassUB'

		float  time;
		float  timeDelta;
		uint  frame;
		uint  passFrameId;
		uint  seed;
		float4  mouse;
		float  customKeys;
		CameraData  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >    intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >    intConst;
	};
#endif
	StaticAssert( offsetof(ComputePassUB, time) == 0 );
	StaticAssert( offsetof(ComputePassUB, timeDelta) == 4 );
	StaticAssert( offsetof(ComputePassUB, frame) == 8 );
	StaticAssert( offsetof(ComputePassUB, passFrameId) == 12 );
	StaticAssert( offsetof(ComputePassUB, seed) == 16 );
	StaticAssert( offsetof(ComputePassUB, mouse) == 32 );
	StaticAssert( offsetof(ComputePassUB, customKeys) == 48 );
	StaticAssert( offsetof(ComputePassUB, camera) == 64 );
	StaticAssert( offsetof(ComputePassUB, floatSliders) == 448 );
	StaticAssert( offsetof(ComputePassUB, intSliders) == 576 );
	StaticAssert( offsetof(ComputePassUB, colors) == 704 );
	StaticAssert( offsetof(ComputePassUB, floatConst) == 832 );
	StaticAssert( offsetof(ComputePassUB, intConst) == 960 );
	StaticAssert( sizeof(ComputePassUB) == 1088 );

#ifndef ComputePassPC_DEFINED
#	define ComputePassPC_DEFINED
	// size: 4, align: 4 (16)
	struct ComputePassPC
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa1d3ae84u}};  // 'ComputePassPC'

		uint  dispatchIndex;
	};
#endif
	StaticAssert( offsetof(ComputePassPC, dispatchIndex) == 0 );
	StaticAssert( sizeof(ComputePassPC) == 4 );

#ifndef RayTracingPassUB_DEFINED
#	define RayTracingPassUB_DEFINED
	// size: 1088, align: 16
	struct RayTracingPassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x1539319au}};  // 'RayTracingPassUB'

		float  time;
		float  timeDelta;
		uint  frame;
		uint  passFrameId;
		uint  seed;
		float4  mouse;
		float  customKeys;
		CameraData  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >    intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >    intConst;
	};
#endif
	StaticAssert( offsetof(RayTracingPassUB, time) == 0 );
	StaticAssert( offsetof(RayTracingPassUB, timeDelta) == 4 );
	StaticAssert( offsetof(RayTracingPassUB, frame) == 8 );
	StaticAssert( offsetof(RayTracingPassUB, passFrameId) == 12 );
	StaticAssert( offsetof(RayTracingPassUB, seed) == 16 );
	StaticAssert( offsetof(RayTracingPassUB, mouse) == 32 );
	StaticAssert( offsetof(RayTracingPassUB, customKeys) == 48 );
	StaticAssert( offsetof(RayTracingPassUB, camera) == 64 );
	StaticAssert( offsetof(RayTracingPassUB, floatSliders) == 448 );
	StaticAssert( offsetof(RayTracingPassUB, intSliders) == 576 );
	StaticAssert( offsetof(RayTracingPassUB, colors) == 704 );
	StaticAssert( offsetof(RayTracingPassUB, floatConst) == 832 );
	StaticAssert( offsetof(RayTracingPassUB, intConst) == 960 );
	StaticAssert( sizeof(RayTracingPassUB) == 1088 );

#ifndef SceneGraphicsPassUB_DEFINED
#	define SceneGraphicsPassUB_DEFINED
	// size: 1056, align: 16
	struct SceneGraphicsPassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x1f8a4833u}};  // 'SceneGraphicsPassUB'

		float2  resolution;
		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		CameraData  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >    intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >    intConst;
	};
#endif
	StaticAssert( offsetof(SceneGraphicsPassUB, resolution) == 0 );
	StaticAssert( offsetof(SceneGraphicsPassUB, time) == 8 );
	StaticAssert( offsetof(SceneGraphicsPassUB, timeDelta) == 12 );
	StaticAssert( offsetof(SceneGraphicsPassUB, frame) == 16 );
	StaticAssert( offsetof(SceneGraphicsPassUB, seed) == 20 );
	StaticAssert( offsetof(SceneGraphicsPassUB, camera) == 32 );
	StaticAssert( offsetof(SceneGraphicsPassUB, floatSliders) == 416 );
	StaticAssert( offsetof(SceneGraphicsPassUB, intSliders) == 544 );
	StaticAssert( offsetof(SceneGraphicsPassUB, colors) == 672 );
	StaticAssert( offsetof(SceneGraphicsPassUB, floatConst) == 800 );
	StaticAssert( offsetof(SceneGraphicsPassUB, intConst) == 928 );
	StaticAssert( sizeof(SceneGraphicsPassUB) == 1056 );

#ifndef SceneRayTracingPassUB_DEFINED
#	define SceneRayTracingPassUB_DEFINED
	// size: 1040, align: 16
	struct SceneRayTracingPassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd09ba9b0u}};  // 'SceneRayTracingPassUB'

		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		CameraData  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >    intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >    intConst;
	};
#endif
	StaticAssert( offsetof(SceneRayTracingPassUB, time) == 0 );
	StaticAssert( offsetof(SceneRayTracingPassUB, timeDelta) == 4 );
	StaticAssert( offsetof(SceneRayTracingPassUB, frame) == 8 );
	StaticAssert( offsetof(SceneRayTracingPassUB, seed) == 12 );
	StaticAssert( offsetof(SceneRayTracingPassUB, camera) == 16 );
	StaticAssert( offsetof(SceneRayTracingPassUB, floatSliders) == 400 );
	StaticAssert( offsetof(SceneRayTracingPassUB, intSliders) == 528 );
	StaticAssert( offsetof(SceneRayTracingPassUB, colors) == 656 );
	StaticAssert( offsetof(SceneRayTracingPassUB, floatConst) == 784 );
	StaticAssert( offsetof(SceneRayTracingPassUB, intConst) == 912 );
	StaticAssert( sizeof(SceneRayTracingPassUB) == 1040 );

#ifndef SphericalCubeMaterialUB_DEFINED
#	define SphericalCubeMaterialUB_DEFINED
	// size: 112, align: 16
	struct SphericalCubeMaterialUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xeb01110au}};  // 'SphericalCubeMaterialUB'

		float4x4_storage  transform;
		float3x3_storage  normalMat;
	};
#endif
	StaticAssert( offsetof(SphericalCubeMaterialUB, transform) == 0 );
	StaticAssert( offsetof(SphericalCubeMaterialUB, normalMat) == 64 );
	StaticAssert( sizeof(SphericalCubeMaterialUB) == 112 );

#ifndef UnifiedGeometryMaterialUB_DEFINED
#	define UnifiedGeometryMaterialUB_DEFINED
	// size: 112, align: 16
	struct UnifiedGeometryMaterialUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x6940ef36u}};  // 'UnifiedGeometryMaterialUB'

		float4x4_storage  transform;
		float3x3_storage  normalMat;
	};
#endif
	StaticAssert( offsetof(UnifiedGeometryMaterialUB, transform) == 0 );
	StaticAssert( offsetof(UnifiedGeometryMaterialUB, normalMat) == 64 );
	StaticAssert( sizeof(UnifiedGeometryMaterialUB) == 112 );

