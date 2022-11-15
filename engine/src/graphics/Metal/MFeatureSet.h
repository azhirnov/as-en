// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

# include "base/Utils/Version.h"
# include "base/CompileTime/StringToID.h"
# include "graphics/Public/DeviceProperties.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/GraphicsCreateInfo.h"

namespace AE::Graphics
{

	//
	// Metal GPU Family types
	//
	struct MGPUFamilies
	{
		uint	common		= 0;
		uint	apple		= 0;
		uint	mac			= 0;
		uint	metal		= 0;

		MGPUFamilies () {}

		ND_ bool  IsValid ()	const	{ return common > 0 or apple > 0 or mac > 0 or metal > 0; }
	};
	


	//
	// Metal Feature Set
	//
	struct MFeatureSet
	{
	// types
	public:
		struct Features
		{
			// from API
			bool	dynamicLibraries						: 1;
			bool	functionPointers						: 1;
			bool	functionPointersFromRender				: 1;
			bool	hasUnifiedMemory						: 1;
			bool	rayTracingFromCompute					: 1;
			bool	raytracingFromRender					: 1;
			bool	barycentricCoords						: 1;
			bool	programmableSamplePositions				: 1;
			bool	rasterOrderGroups						: 1;
			bool	msaa32Bit								: 1;
			bool	pullModelInterpolation					: 1;
			bool	depth24Stencil8Format					: 1;
			bool	floatFiltering32Bit						: 1;
			bool	pixelFormat_BC							: 1;
			bool	queryTextureLOD							: 1;
			bool	primitiveMotionBlur						: 1;
			bool	renderDynamicLibraries					: 1;

			// from GPU family
			bool	programableBlending						: 1;
			bool	pixelFormat_PVRTC						: 1;
			bool	pixelFormat_ETC							: 1;
			bool	pixelFormat_ASTC						: 1;
			bool	compressedVolumeTextureFormats			: 1;
			bool	extendedRangePixelFormat				: 1;
			bool	wideColorPixelFormat					: 1;
			bool	pixelFormat_Depth16						: 1;
			bool	linearTextures							: 1;
			bool	MSAA_depthResolve						: 1;
			bool	arrayOfTexturesRead						: 1;
			bool	arrayOfTexturesWrite					: 1;
			bool	cubeMapTextureArrays					: 1;
			bool	stencilTextureViews						: 1;
			bool	arrayOfSamplers							: 1;
			bool	samplerMaxAnisotropy					: 1;
			bool	samplerLODclamp							: 1;
			bool	samplerComparisonFunctions				: 1;
			bool	uint16Coordinates						: 1;
			bool	borderColor								: 1;
			bool	countingOcclusionQuery					: 1;
			bool	baseVertexInstanceDrawing				: 1;
			bool	layeredRendering						: 1;
			bool	layeredRenderingToMSTexture				: 1;
			bool	memorylessRenderTargets					: 1;
			bool	dualSourceBlending						: 1;
			bool	combinedMSAA_StoreAndResolve			: 1;
			bool	MSAA_blits								: 1;
			bool	programableSamplePosition				: 1;
			bool	deferredStoreActions					: 1;
			bool	textureBarriers							: 1;
			bool	memoryBarriersAppleGPU					: 1;	// Supports: compute command encoders, and for vertex-to-vertex and vertex-to-fragment stages of render command encoders.
			bool	memoryBarriersFull						: 1;
			bool	tessellation							: 1;
			bool	indirectTessellationArguments			: 1;
			bool	tessellationInIndirectCommandBuffers	: 1;
			bool	resourceHeaps							: 1;
			bool	functionSpecialization					: 1;
			bool	readOrWriteBuffersInFunctions			: 1;	// TODO ???
			bool	readOrWriteTexturesInFunctions			: 1;
			bool	extractInsertReserveBits				: 1;
			bool	SIMD_barrier							: 1;
			bool	indirectDrawAndDispatchArguments		: 1;
			bool	indirectCommandBuffersRendering			: 1;
			bool	indirectCommandBuffersCompute			: 1;
			bool	uniformType								: 1;
			bool	imageBlocks								: 1;
			bool	tileShaders								: 1;
			bool	imageblockSampleCoverageControl			: 1;
			bool	postDepthCoverage						: 1;
			bool	quadScopedPermuteOperations				: 1;
			bool	SIMD_scopedPermuteOperations			: 1;
			bool	SIMD_scopedReductionOperations			: 1;
			bool	SIMD_scopedMatrixMultiplyOPerations		: 1;
			bool	nonUniformThreadgroupSize				: 1;
			bool	multipleViewports						: 1;
			bool	deviceNotifications						: 1;
			bool	stencilFeedback							: 1;
			bool	stencilResolve							: 1;
			bool	nonSquareTileDispatch					: 1;
			bool	textureSwizzle							: 1;
			bool	placementHeap							: 1;
			bool	primitiveID								: 1;
			bool	readOrWriteCubeMapTextures				: 1;
			bool	sparseColorTextures						: 1;
			bool	sparseDepthStencilTextures				: 1;
			bool	variableRasterizationRate				: 1;
			bool	vertexAmplification						: 1;
			bool	int64Math								: 1;
			bool	lossyTextureCompression					: 1;
			bool	SIMD_shiftAndFill						: 1;
			bool	meshShader								: 1;
			//bool	MetalFX_spatialUpscaling				: 1;
			//bool	MetalFX_temporalUpscaling				: 1;
			bool	fastResourceLoading						: 1;	// TODO ???
			bool	floatingPointAtomics					: 1;

			// unsupported
			bool	depthBounds								: 1;

			ubyte	argbufTier;
			ubyte	readWriteTextureTier;

			Features () { ZeroMem( this, Sizeof(*this) ); }

			ND_ bool  accelerationStructure () const	{ return rayTracingFromCompute or raytracingFromRender; }
		};

		struct Properties
		{
			// dynamic
			uint		maxArgbufSamplers;
			BitSet<8>	maxVertexAmplification;
			BitSet<8>	maxTextureSampleCount;

			Bytes		maxThreadgroupMemoryLength;
			uint3		maxThreadsPerThreadgroup3;
			Bytes		sparseTileMemorySize;
			Bytes		maxBufferSize;

			Bytes		maxTransferRate;	// desktop only

			// function arguments
			uint		maxNumberOfVertexAttribsPerVertexDescriptor;
			uint		maxNumberOfEntriesInBufferArgumentTablePerGraphicsOrComputeFunction;
			uint		maxNumberOfEntriesInTextureArgumentTablePerGraphicsOrComputeFunction;
			uint		maxNumberOfEntriesInSamplerStateArgumentTablePerGraphicsOrComputeFunction;
			uint		maxNumberOfEntriesInThreadgroupMemoryArgumentTablePerComputeFunction;
			uint		maxNumberOfConstantBufferArgumentsPerFunction;
			Bytes		maxLengthOfConstantBufferArgumentsPerFunction;
			uint		maxThreadsPerThreadgroup;
			Bytes		maxTotalThreadgroupMemoryAllocation;
			Bytes		maxTotalTileMemoryAllocation;
			Bytes		threadgroupMemoryLengthAlignment;
			Bytes		maxFunctionMemoryAllocationForBufferInConstantAddressSpace;
			uint		maxNumberOfInputsToFragmentFunctionDeclaredWithStageInQualifier;
			uint		maxNumberOfInputComponentsToFragmentFunctionDeclaredWithStageInQualifier;
			uint		maxNumberOfFunctionConstants;
			uint		maxTessellationFactor;
			uint		maxNumberOfViewportsAndScissorRectanglesPerVertexFunction;
			uint		maxNumberOfRasterOrderGroupsPerFragmentFunction;

			// argument buffers
			uint		maxNumberOfBufferInsideArgumentBuffer;				// per stage
			uint		maxNumberOfTexturesInsideArgumentBuffer;			// per stage
			uint		maxNumberOfSamplersInsideArgumentBuffer;			// per stage

			// resources
			Bytes		minBufferOffsetAlignment;
			uint		max1DTextureSize;
			uint		max2DTextureSize;
			uint		maxCubemapTextureSize;
			uint		max3DTextureSize;
			uint		maxNumberOfLayersPerTextureArray_1D_2D_3D;
			Bytes		bufferAlignmentForCopyingExistingTextureToBuffer;
			Bytes		minUniformBufferOffsetAlign;
			Bytes		minStorageBufferOffsetAlign;

			// render targets
			uint		maxNumberOfColorRenderTargetsPerRenderPassDescriptor;
			uint		maxSizeOfPointPrimitive;
			Bytes		maxTotalRenderTargetSizePerPixel; // when using multiple color render target
			Bytes		maxVisibilityQueryOffset;

			// synchronization
			uint		maxNumberOfFences;
			uint		maxVertexCountForVertexAmplification;

			Properties () { ZeroMem( this, Sizeof(*this) ); }
		};

		
	// variables
	public:
		Features		features;
		Properties		properties;


	// methods
	public:

		void  InitFeatureSet (OUT FeatureSet &) const;
		
		static void  InitFromFSTable (const MGPUFamilies &, INOUT Features &, INOUT Properties &);
	};


} // AE::Graphics
