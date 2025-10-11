//1d6e1909
#ifndef CameraData_DEFINED
#	define CameraData_DEFINED
	// size: 400, align: 16
	struct CameraData
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x8142e66cu}};

		float4x4_storage_std140     viewProj;
		float4x4_storage_std140     invViewProj;
		float4x4_storage_std140     proj;
		float4x4_storage_std140     view;
		float3                      pos;
		float2                      clipPlanes;
		float2                      fov;
		float                       zoom;
		StaticArray< float4, 6 >    frustum;
	};
#endif
	StaticAssert( offsetof(CameraData, viewProj) == 0 );
	StaticAssert( offsetof(CameraData, invViewProj) == 64 );
	StaticAssert( offsetof(CameraData, proj) == 128 );
	StaticAssert( offsetof(CameraData, view) == 192 );
	StaticAssert( offsetof(CameraData, pos) == 256 );
	StaticAssert( offsetof(CameraData, clipPlanes) == 272 );
	StaticAssert( offsetof(CameraData, fov) == 280 );
	StaticAssert( offsetof(CameraData, zoom) == 288 );
	StaticAssert( offsetof(CameraData, frustum) == 304 );
	StaticAssert( sizeof(CameraData) == 400 );

#ifndef CameraData_DEFINED
#	define CameraData_DEFINED
	// size: 400, align: 16
	struct CameraData
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x8142e66cu}};

		float4x4_storage_std140     viewProj;
		float4x4_storage_std140     invViewProj;
		float4x4_storage_std140     proj;
		float4x4_storage_std140     view;
		float3                      pos;
		float2                      clipPlanes;
		float2                      fov;
		float                       zoom;
		StaticArray< float4, 6 >    frustum;
	};
#endif
	StaticAssert( offsetof(CameraData, viewProj) == 0 );
	StaticAssert( offsetof(CameraData, invViewProj) == 64 );
	StaticAssert( offsetof(CameraData, proj) == 128 );
	StaticAssert( offsetof(CameraData, view) == 192 );
	StaticAssert( offsetof(CameraData, pos) == 256 );
	StaticAssert( offsetof(CameraData, clipPlanes) == 272 );
	StaticAssert( offsetof(CameraData, fov) == 280 );
	StaticAssert( offsetof(CameraData, zoom) == 288 );
	StaticAssert( offsetof(CameraData, frustum) == 304 );
	StaticAssert( sizeof(CameraData) == 400 );

#ifndef PostprocessPassUB_DEFINED
#	define PostprocessPassUB_DEFINED
	// size: 1248, align: 16
	struct PostprocessPassUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x6a9c4b92u}};

		float3                      resolution;
		float                       time;
		float2                      invResolution;
		float                       timeDelta;
		uint                        frame;
		uint                        passFrameId;
		uint                        seed;
		uint                        colorSpace;
		float4                      channelTime;
		StaticArray< float4, 4 >    channelResolution;
		float4                      mouse;
		float4                      date;
		float                       sampleRate;
		float                       pixPerMm;
		float                       mmPerPix;
		float2                      customKeys;
		CameraData                  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(PostprocessPassUB, resolution) == 0 );
	StaticAssert( offsetof(PostprocessPassUB, time) == 16 );
	StaticAssert( offsetof(PostprocessPassUB, invResolution) == 24 );
	StaticAssert( offsetof(PostprocessPassUB, timeDelta) == 32 );
	StaticAssert( offsetof(PostprocessPassUB, frame) == 36 );
	StaticAssert( offsetof(PostprocessPassUB, passFrameId) == 40 );
	StaticAssert( offsetof(PostprocessPassUB, seed) == 44 );
	StaticAssert( offsetof(PostprocessPassUB, colorSpace) == 48 );
	StaticAssert( offsetof(PostprocessPassUB, channelTime) == 64 );
	StaticAssert( offsetof(PostprocessPassUB, channelResolution) == 80 );
	StaticAssert( offsetof(PostprocessPassUB, mouse) == 144 );
	StaticAssert( offsetof(PostprocessPassUB, date) == 160 );
	StaticAssert( offsetof(PostprocessPassUB, sampleRate) == 176 );
	StaticAssert( offsetof(PostprocessPassUB, pixPerMm) == 180 );
	StaticAssert( offsetof(PostprocessPassUB, mmPerPix) == 184 );
	StaticAssert( offsetof(PostprocessPassUB, customKeys) == 192 );
	StaticAssert( offsetof(PostprocessPassUB, camera) == 208 );
	StaticAssert( offsetof(PostprocessPassUB, floatSliders) == 608 );
	StaticAssert( offsetof(PostprocessPassUB, intSliders) == 736 );
	StaticAssert( offsetof(PostprocessPassUB, colors) == 864 );
	StaticAssert( offsetof(PostprocessPassUB, floatConst) == 992 );
	StaticAssert( offsetof(PostprocessPassUB, intConst) == 1120 );
	StaticAssert( sizeof(PostprocessPassUB) == 1248 );

#ifndef ComputePassUB_DEFINED
#	define ComputePassUB_DEFINED
	// size: 1104, align: 16
	struct ComputePassUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xaba36a57u}};

		float                       time;
		float                       timeDelta;
		uint                        frame;
		uint                        passFrameId;
		uint                        seed;
		float4                      mouse;
		float2                      customKeys;
		float                       pixPerMm;
		float                       mmPerPix;
		CameraData                  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(ComputePassUB, time) == 0 );
	StaticAssert( offsetof(ComputePassUB, timeDelta) == 4 );
	StaticAssert( offsetof(ComputePassUB, frame) == 8 );
	StaticAssert( offsetof(ComputePassUB, passFrameId) == 12 );
	StaticAssert( offsetof(ComputePassUB, seed) == 16 );
	StaticAssert( offsetof(ComputePassUB, mouse) == 32 );
	StaticAssert( offsetof(ComputePassUB, customKeys) == 48 );
	StaticAssert( offsetof(ComputePassUB, pixPerMm) == 56 );
	StaticAssert( offsetof(ComputePassUB, mmPerPix) == 60 );
	StaticAssert( offsetof(ComputePassUB, camera) == 64 );
	StaticAssert( offsetof(ComputePassUB, floatSliders) == 464 );
	StaticAssert( offsetof(ComputePassUB, intSliders) == 592 );
	StaticAssert( offsetof(ComputePassUB, colors) == 720 );
	StaticAssert( offsetof(ComputePassUB, floatConst) == 848 );
	StaticAssert( offsetof(ComputePassUB, intConst) == 976 );
	StaticAssert( sizeof(ComputePassUB) == 1104 );

#ifndef ComputePassPC_DEFINED
#	define ComputePassPC_DEFINED
	// size: 16, align: 16
	struct ComputePassPC
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa1d3ae84u}};

		uint4  wgCount_dispatchIndex;
	};
#endif
	StaticAssert( offsetof(ComputePassPC, wgCount_dispatchIndex) == 0 );
	StaticAssert( sizeof(ComputePassPC) == 16 );

#ifndef ComputeMipUB_DEFINED
#	define ComputeMipUB_DEFINED
	// size: 704, align: 16
	struct ComputeMipUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x62b2bd2du}};

		float                       time;
		float                       timeDelta;
		uint                        frame;
		uint                        passFrameId;
		uint                        seed;
		float4                      mouse;
		float2                      customKeys;
		float                       pixPerMm;
		float                       mmPerPix;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(ComputeMipUB, time) == 0 );
	StaticAssert( offsetof(ComputeMipUB, timeDelta) == 4 );
	StaticAssert( offsetof(ComputeMipUB, frame) == 8 );
	StaticAssert( offsetof(ComputeMipUB, passFrameId) == 12 );
	StaticAssert( offsetof(ComputeMipUB, seed) == 16 );
	StaticAssert( offsetof(ComputeMipUB, mouse) == 32 );
	StaticAssert( offsetof(ComputeMipUB, customKeys) == 48 );
	StaticAssert( offsetof(ComputeMipUB, pixPerMm) == 56 );
	StaticAssert( offsetof(ComputeMipUB, mmPerPix) == 60 );
	StaticAssert( offsetof(ComputeMipUB, floatSliders) == 64 );
	StaticAssert( offsetof(ComputeMipUB, intSliders) == 192 );
	StaticAssert( offsetof(ComputeMipUB, colors) == 320 );
	StaticAssert( offsetof(ComputeMipUB, floatConst) == 448 );
	StaticAssert( offsetof(ComputeMipUB, intConst) == 576 );
	StaticAssert( sizeof(ComputeMipUB) == 704 );

#ifndef ComputeMipPC_DEFINED
#	define ComputeMipPC_DEFINED
	// size: 16, align: 8 (16)
	struct ComputeMipPC
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x68c279feu}};

		float2  invResolution;
		uint2   resolution;
	};
#endif
	StaticAssert( offsetof(ComputeMipPC, invResolution) == 0 );
	StaticAssert( offsetof(ComputeMipPC, resolution) == 8 );
	StaticAssert( sizeof(ComputeMipPC) == 16 );

#ifndef ComputeMipUB_DEFINED
#	define ComputeMipUB_DEFINED
	// size: 704, align: 16
	struct ComputeMipUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x62b2bd2du}};

		float                       time;
		float                       timeDelta;
		uint                        frame;
		uint                        passFrameId;
		uint                        seed;
		float4                      mouse;
		float2                      customKeys;
		float                       pixPerMm;
		float                       mmPerPix;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(ComputeMipUB, time) == 0 );
	StaticAssert( offsetof(ComputeMipUB, timeDelta) == 4 );
	StaticAssert( offsetof(ComputeMipUB, frame) == 8 );
	StaticAssert( offsetof(ComputeMipUB, passFrameId) == 12 );
	StaticAssert( offsetof(ComputeMipUB, seed) == 16 );
	StaticAssert( offsetof(ComputeMipUB, mouse) == 32 );
	StaticAssert( offsetof(ComputeMipUB, customKeys) == 48 );
	StaticAssert( offsetof(ComputeMipUB, pixPerMm) == 56 );
	StaticAssert( offsetof(ComputeMipUB, mmPerPix) == 60 );
	StaticAssert( offsetof(ComputeMipUB, floatSliders) == 64 );
	StaticAssert( offsetof(ComputeMipUB, intSliders) == 192 );
	StaticAssert( offsetof(ComputeMipUB, colors) == 320 );
	StaticAssert( offsetof(ComputeMipUB, floatConst) == 448 );
	StaticAssert( offsetof(ComputeMipUB, intConst) == 576 );
	StaticAssert( sizeof(ComputeMipUB) == 704 );

#ifndef ComputeMipPC_DEFINED
#	define ComputeMipPC_DEFINED
	// size: 16, align: 8 (16)
	struct ComputeMipPC
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x68c279feu}};

		float2  invResolution;
		uint2   resolution;
	};
#endif
	StaticAssert( offsetof(ComputeMipPC, invResolution) == 0 );
	StaticAssert( offsetof(ComputeMipPC, resolution) == 8 );
	StaticAssert( sizeof(ComputeMipPC) == 16 );

#ifndef RayTracingPassUB_DEFINED
#	define RayTracingPassUB_DEFINED
	// size: 1104, align: 16
	struct RayTracingPassUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x1539319au}};

		float                       time;
		float                       timeDelta;
		uint                        frame;
		uint                        passFrameId;
		uint                        seed;
		float4                      mouse;
		float2                      customKeys;
		float                       pixPerMm;
		float                       mmPerPix;
		CameraData                  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(RayTracingPassUB, time) == 0 );
	StaticAssert( offsetof(RayTracingPassUB, timeDelta) == 4 );
	StaticAssert( offsetof(RayTracingPassUB, frame) == 8 );
	StaticAssert( offsetof(RayTracingPassUB, passFrameId) == 12 );
	StaticAssert( offsetof(RayTracingPassUB, seed) == 16 );
	StaticAssert( offsetof(RayTracingPassUB, mouse) == 32 );
	StaticAssert( offsetof(RayTracingPassUB, customKeys) == 48 );
	StaticAssert( offsetof(RayTracingPassUB, pixPerMm) == 56 );
	StaticAssert( offsetof(RayTracingPassUB, mmPerPix) == 60 );
	StaticAssert( offsetof(RayTracingPassUB, camera) == 64 );
	StaticAssert( offsetof(RayTracingPassUB, floatSliders) == 464 );
	StaticAssert( offsetof(RayTracingPassUB, intSliders) == 592 );
	StaticAssert( offsetof(RayTracingPassUB, colors) == 720 );
	StaticAssert( offsetof(RayTracingPassUB, floatConst) == 848 );
	StaticAssert( offsetof(RayTracingPassUB, intConst) == 976 );
	StaticAssert( sizeof(RayTracingPassUB) == 1104 );

#ifndef SceneGraphicsPassUB_DEFINED
#	define SceneGraphicsPassUB_DEFINED
	// size: 1104, align: 16
	struct SceneGraphicsPassUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x1f8a4833u}};

		float2                      resolution;
		float2                      invResolution;
		float                       time;
		float                       timeDelta;
		uint                        frame;
		uint                        seed;
		float4                      mouse;
		float2                      customKeys;
		float                       pixPerMm;
		float                       mmPerPix;
		CameraData                  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(SceneGraphicsPassUB, resolution) == 0 );
	StaticAssert( offsetof(SceneGraphicsPassUB, invResolution) == 8 );
	StaticAssert( offsetof(SceneGraphicsPassUB, time) == 16 );
	StaticAssert( offsetof(SceneGraphicsPassUB, timeDelta) == 20 );
	StaticAssert( offsetof(SceneGraphicsPassUB, frame) == 24 );
	StaticAssert( offsetof(SceneGraphicsPassUB, seed) == 28 );
	StaticAssert( offsetof(SceneGraphicsPassUB, mouse) == 32 );
	StaticAssert( offsetof(SceneGraphicsPassUB, customKeys) == 48 );
	StaticAssert( offsetof(SceneGraphicsPassUB, pixPerMm) == 56 );
	StaticAssert( offsetof(SceneGraphicsPassUB, mmPerPix) == 60 );
	StaticAssert( offsetof(SceneGraphicsPassUB, camera) == 64 );
	StaticAssert( offsetof(SceneGraphicsPassUB, floatSliders) == 464 );
	StaticAssert( offsetof(SceneGraphicsPassUB, intSliders) == 592 );
	StaticAssert( offsetof(SceneGraphicsPassUB, colors) == 720 );
	StaticAssert( offsetof(SceneGraphicsPassUB, floatConst) == 848 );
	StaticAssert( offsetof(SceneGraphicsPassUB, intConst) == 976 );
	StaticAssert( sizeof(SceneGraphicsPassUB) == 1104 );

#ifndef SceneRayTracingPassUB_DEFINED
#	define SceneRayTracingPassUB_DEFINED
	// size: 1072, align: 16
	struct SceneRayTracingPassUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd09ba9b0u}};

		float                       time;
		float                       timeDelta;
		uint                        frame;
		uint                        seed;
		float2                      customKeys;
		float                       pixPerMm;
		float                       mmPerPix;
		CameraData                  camera;
		StaticArray< float4, 8 >    floatSliders;
		StaticArray< int4, 8 >      intSliders;
		StaticArray< float4, 8 >    colors;
		StaticArray< float4, 8 >    floatConst;
		StaticArray< int4, 8 >      intConst;
	};
#endif
	StaticAssert( offsetof(SceneRayTracingPassUB, time) == 0 );
	StaticAssert( offsetof(SceneRayTracingPassUB, timeDelta) == 4 );
	StaticAssert( offsetof(SceneRayTracingPassUB, frame) == 8 );
	StaticAssert( offsetof(SceneRayTracingPassUB, seed) == 12 );
	StaticAssert( offsetof(SceneRayTracingPassUB, customKeys) == 16 );
	StaticAssert( offsetof(SceneRayTracingPassUB, pixPerMm) == 24 );
	StaticAssert( offsetof(SceneRayTracingPassUB, mmPerPix) == 28 );
	StaticAssert( offsetof(SceneRayTracingPassUB, camera) == 32 );
	StaticAssert( offsetof(SceneRayTracingPassUB, floatSliders) == 432 );
	StaticAssert( offsetof(SceneRayTracingPassUB, intSliders) == 560 );
	StaticAssert( offsetof(SceneRayTracingPassUB, colors) == 688 );
	StaticAssert( offsetof(SceneRayTracingPassUB, floatConst) == 816 );
	StaticAssert( offsetof(SceneRayTracingPassUB, intConst) == 944 );
	StaticAssert( sizeof(SceneRayTracingPassUB) == 1072 );

#ifndef SphericalCubeMaterialUB_DEFINED
#	define SphericalCubeMaterialUB_DEFINED
	// size: 112, align: 16
	struct SphericalCubeMaterialUB
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xeb01110au}};

		float4x4_storage_std140  transform;
		float3x3_storage_std140  normalMat;
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
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x6940ef36u}};

		float4x4_storage_std140  transform;
		float3x3_storage_std140  normalMat;
	};
#endif
	StaticAssert( offsetof(UnifiedGeometryMaterialUB, transform) == 0 );
	StaticAssert( offsetof(UnifiedGeometryMaterialUB, normalMat) == 64 );
	StaticAssert( sizeof(UnifiedGeometryMaterialUB) == 112 );

