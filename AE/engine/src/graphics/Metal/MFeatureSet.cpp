// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Metal/MFeatureSet.h"

namespace AE::Graphics
{
namespace
{
#ifndef AE_PLATFORM_APPLE
	struct AppleUtils
	{
		ND_ static Version3		GetOSVersion ()												{ return Version3{13,0}; }
		ND_ static bool			VersionIsAtLeast (Version2 ver, Version2 macos, Version2)	{ return ver >= macos; }

		static constexpr bool	Is_MacOS	= true;
		static constexpr bool	Is_iOS		= false;
	};
#endif
}

/*
=================================================
	InitFromFSTable
----
	from https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
	TODO: Apple9
=================================================
*/
	void  MFeatureSet::InitFromFSTable (const MGPUFamilies &f, Version2 msl, INOUT Features &outFeats, INOUT Properties &outProps) __NE___
	{
		// may be overriden
		outFeats.argbufTier							= ubyte(Max( (f.apple <  6 ? 1 :
																  f.apple >= 6 ? 2 : 0),
																 (f.mac >= 2 ? 2 : 0),
																 outFeats.argbufTier
																));
		outFeats.dynamicLibraries					= (f.apple >= 6 or f.mac >= 2);
		outFeats.rayTracingFromCompute				= (f.apple >= 6 or f.mac >= 2 or f.metal >= 3);
		outFeats.raytracingFromRender				= (f.apple >= 6);
		outFeats.barycentricCoords					= (f.apple >= 7);
		outFeats.rasterOrderGroups					= (f.common >= 3 or f.apple >= 4 or f.metal >= 3);
		outFeats.pixelFormat_BC						= (f.mac >= 2);
		outFeats.renderDynamicLibraries				= (f.apple >= 6);

		outFeats.programableBlending				= (f.apple >= 2);
		outFeats.pixelFormat_PVRTC					= (f.apple >= 2);
		outFeats.pixelFormat_ETC					= (f.apple >= 2);
		outFeats.pixelFormat_ASTC					= (f.apple >= 2);
		outFeats.compressedVolumeTextureFormats		= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.extendedRangePixelFormat			= (f.apple >= 3);
		outFeats.wideColorPixelFormat				= true;
		outFeats.pixelFormat_Depth16				= true;
		outFeats.linearTextures						= true;
		outFeats.MSAA_depthResolve					= (f.common >= 3 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.arrayOfTexturesRead				= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.arrayOfTexturesWrite				= (f.common >= 2 or f.apple >= 6 or f.mac >= 2 or f.metal >= 3);
		outFeats.cubeMapTextureArrays				= (f.common >= 2 or f.apple >= 4 or f.mac >= 2 or f.metal >= 3);
		outFeats.stencilTextureViews				= true;
		outFeats.arrayOfSamplers					= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.samplerMaxAnisotropy				= true;
		outFeats.samplerLODclamp					= true;
		outFeats.samplerComparisonFunctions			= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.uint16Coordinates					= true;
		outFeats.borderColor						= (f.apple >= 7 or f.mac >= 2 or f.metal >= 3);
		outFeats.countingOcclusionQuery				= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.baseVertexInstanceDrawing			= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.layeredRendering					= (f.common >= 3 or f.apple >= 5 or f.mac >= 2 or f.metal >= 3);
		outFeats.layeredRenderingToMSTexture		= (f.common >= 3 or f.apple >= 7 or f.mac >= 2 or f.metal >= 3);
		outFeats.memorylessRenderTargets			= (f.apple >= 2);
		outFeats.dualSourceBlending					= true;
		outFeats.combinedMSAA_StoreAndResolve		= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.MSAA_blits							= true;
		outFeats.deferredStoreActions				= true;
		outFeats.textureBarriers					= (f.mac >= 2);
		outFeats.memoryBarriersAppleGPU				= (f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.memoryBarriersFull					= (f.mac >= 2 or f.metal >= 3);
		outFeats.tessellation						= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.indirectTessellationArguments		= (f.apple >= 5 or f.mac >= 2 or f.metal >= 3);
		outFeats.tessellationInIndirectCommandBuffers=(f.apple >= 5 or f.mac >= 2 or f.metal >= 3);
		outFeats.resourceHeaps						= true;
		outFeats.functionSpecialization				= true;
		outFeats.readOrWriteBuffersInFunctions		= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.readOrWriteTexturesInFunctions		= (f.common >= 3 or f.apple >= 4 or f.mac >= 2 or f.metal >= 3);
		outFeats.extractInsertReserveBits			= true;
		outFeats.SIMD_barrier						= true;
		outFeats.indirectDrawAndDispatchArguments	= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.indirectCommandBuffersRendering	= (f.common >= 2 or f.apple >= 3 or f.mac >= 2 or f.metal >= 3);
		outFeats.indirectCommandBuffersCompute		= (f.common >= 2 or f.apple >= 3 or f.metal >= 3);
		outFeats.uniformType						= true;
		outFeats.imageBlocks						= (f.apple >= 4);
		outFeats.tileShaders						= (f.apple >= 4);
		outFeats.imageblockSampleCoverageControl	= (f.apple >= 4);
		outFeats.postDepthCoverage					= (f.apple >= 4);
		outFeats.quadScopedPermuteOperations		= (f.common >= 3 or f.apple >= 4 or f.mac >= 2 or f.metal >= 3);
		outFeats.SIMD_scopedPermuteOperations		= (f.apple >= 6 or f.mac >= 2 or f.metal >= 3);
		outFeats.SIMD_scopedReductionOperations		= (f.apple >= 7 or f.mac >= 2 or f.metal >= 3);
		outFeats.SIMD_scopedMatrixMultiplyOPerations= (f.apple >= 7);
		outFeats.nonUniformThreadgroupSize			= (f.common >= 3 or f.apple >= 4 or f.mac >= 2 or f.metal >= 3);
		outFeats.multipleViewports					= (f.common >= 3 or f.apple >= 5 or f.mac >= 2 or f.metal >= 3);
		outFeats.deviceNotifications				= (f.mac >= 2);
		outFeats.stencilFeedback					= (f.common >= 3 or f.apple >= 5 or f.mac >= 2 or f.metal >= 3);
		outFeats.stencilResolve						= (f.common >= 3 or f.apple >= 5 or f.mac >= 2 or f.metal >= 3);
		outFeats.nonSquareTileDispatch				= (f.apple >= 5);
		outFeats.textureSwizzle						= (f.apple >= 2 or f.mac >= 2);
		outFeats.placementHeap						= (f.apple >= 2 or f.mac >= 2);
		outFeats.primitiveID						= (f.apple >= 7 or f.mac >= 2 or f.metal >= 3);
		outFeats.readOrWriteCubeMapTextures			= (f.apple >= 4 or f.mac >= 2 or f.metal >= 3);
		outFeats.sparseColorTextures				= (f.apple >= 6);
		outFeats.sparseDepthStencilTextures			= (f.apple >= 8);
		outFeats.variableRasterizationRate			= (f.apple >= 6);	// TODO: use supportsRasterizationRateMap(layerCount:)
		outFeats.vertexAmplification				= (f.apple >= 6);	// TODO: use supportsVertexAmplificationCount(_:)
		outFeats.int64Math							= (f.apple >= 3 or f.metal >= 3) and (msl >= Version2{2,3});	// mtl2.2 in shader, mtl2.3 in buffer
		outFeats.lossyTextureCompression			= (f.apple >= 8);
		outFeats.SIMD_shiftAndFill					= (f.apple >= 8);
		outFeats.meshShader							= (f.apple >= 6 or f.mac >= 2 or f.metal >= 3) and (msl >= Version2{3,0});
		outFeats.fastResourceLoading				= true;
		outFeats.floatingPointAtomics				= (f.apple >= 7 or f.mac >= 2);


		outProps.maxArgumentBufferSamplerCount		= 0;		// [dev maxArgumentBufferSamplerCount]
		outProps.sparseTileMemorySize				= 0_b;		// [dev sparseTileSizeInBytes]
		outProps.maxBufferSize						= 64_Kb;	// [dev maxBufferLength]
		outProps.maxTransferRate					= UMax;		// [dev maxTransferRate]

		outProps.maxNumberOfVertexAttribsPerVertexDescriptor = 31;
		outProps.maxNumberOfEntriesInBufferArgumentTablePerGraphicsOrComputeFunction = 31;
		outProps.maxNumberOfEntriesInTextureArgumentTablePerGraphicsOrComputeFunction =
			Max( (f.apple <= 3 ? 31 :
				  f.apple <= 5 ? 96 :
				  f.apple >= 6 ? 128 : 0),
				 (f.mac >= 2 ? 128 : 0)
				);
		outProps.maxNumberOfEntriesInSamplerStateArgumentTablePerGraphicsOrComputeFunction = 16;
		outProps.maxNumberOfEntriesInThreadgroupMemoryArgumentTablePerComputeFunction = 31;
		outProps.maxNumberOfConstantBufferArgumentsPerFunction =
			Max( (f.apple >= 2 ? 31 : 0),
				 (f.mac   >= 2 ? 14 : 0)
				);
		outProps.maxLengthOfConstantBufferArgumentsPerFunction = 4_Kb;
		outProps.maxThreadsPerThreadgroup =	// [dev maxThreadsPerThreadgroup]
			Max( (f.apple <= 3 ? 512 :
				  f.apple >= 4 ? 1024 : 0),
				 (f.mac   >= 2 ? 1024 : 0)
				);
		outProps.maxTotalThreadgroupMemoryAllocation =
			Max( (f.apple <= 2 ? 16352_b :
				  f.apple <= 3 ? 16_Kb :
				  f.apple >= 4 ? 32_Kb : 0_b),
				 (f.mac   >= 2 ? 32_Kb : 0_b)
				);
		outProps.maxTotalTileMemoryAllocation =
			(f.apple >= 4 ? 32_Kb :
			 0_b);
		outProps.threadgroupMemoryLengthAlignment = 16_b;
		outProps.maxFunctionMemoryAllocationForBufferInConstantAddressSpace = UMax; // no limit

		outProps.maxNumberOfInputsToFragmentFunctionDeclaredWithStageInQualifier =
			Max( (f.apple <= 3 ? 60 :
				  f.apple >= 4 ? 124 : 0),
				 (f.mac   >= 2 ? 32 : 0)
				);
		outProps.maxNumberOfInputComponentsToFragmentFunctionDeclaredWithStageInQualifier =
			(f.apple <= 3 ? 60 :
			 f.apple >= 4 ? 124 :
			 f.mac   >= 2 ? 124 :
			 0);
		outProps.maxNumberOfFunctionConstants = 65536;
		outProps.maxTessellationFactor =
			Max( (f.apple <= 2 ? 0 :
				  f.apple <= 4 ? 16 :
				  f.apple >= 5 ? 64 : 0),
				 (f.mac   >= 2 ? 64 : 0)
				);
		outProps.maxNumberOfViewportsAndScissorRectanglesPerVertexFunction =
			Max( (f.apple <= 4 ? 1 :
				  f.apple >= 5 ? 16 : 0),
				 (f.mac   >= 2 ? 16 : 0)
				);
		outProps.maxNumberOfRasterOrderGroupsPerFragmentFunction =
			Max( (f.apple <= 3 ? 0 :
				  f.apple >= 4 ? 8 : 0),
				 (f.mac   >= 2 ? 8 : 0)
				);

		// https://developer.apple.com/documentation/metal/buffers/about_argument_buffers?language=objc
		outProps.maxNumberOfBufferInsideArgumentBuffer =
			Max( (f.apple <= 3 ? 31 :
				  f.apple <= 5 ? 96 :
				  f.apple >= 6 ? ~0u : 0),			// no limits,  old limit: 500'000
				 (f.mac == 1 ? 64 :
				  f.mac >= 2 ? ~0u : 0)
				);
		outProps.maxNumberOfTexturesInsideArgumentBuffer =
			Max( (f.apple <= 3 ? 31 :
				  f.apple <= 5 ? 96 :
				  f.apple >= 6 ? 1'000'000 : 0),	// old limit: 500'000
				 (f.mac == 1 ? 128 :
				  f.mac >= 2 ? 1'000'000 : 0)
				);
		outProps.maxNumberOfSamplersInsideArgumentBuffer =
			Max( (f.apple <  6 ? 16 :
				  f.apple >= 6 ? 1024 : 0),
				 (f.mac >= 2 ? 1024 : 0)
				);
		outProps.maxNumberOfArgumentBuffersInsideArgumentBuffer =
			(outFeats.argbufTier > 1 ? 8 :
			 0);

		outProps.minUniformBufferOffsetAlign =
			Max( (f.apple >= 2 ? 4_b : 0_b),
				 (f.mac   >= 2 ? 32_b :	0_b) // old limit: 256
				);
		outProps.max1DTextureSize =
			Max( (f.apple <= 2 ? 8192 :
				  f.apple >= 3 ? 16384 : 0),
				 (f.mac   >= 2 ? 16384 : 0)
				);
		outProps.max2DTextureSize =
			Max( (f.apple <= 2 ? 8192 :
				  f.apple >= 3 ? 16384 : 0),
				 (f.mac   >= 2 ? 16384 : 0)
				);
		outProps.maxCubemapTextureSize =
			Max( (f.apple <= 2 ? 8192 :
				  f.apple >= 3 ? 16384 : 0),
				 (f.mac   >= 2 ? 16384 : 0)
				);
		outProps.max3DTextureSize = 2048;
		outProps.maxNumberOfLayersPerTextureArray_1D_2D_3D = 2048;
		outProps.bufferAlignmentForCopyingExistingTextureToBuffer =
			Max( (f.apple <= 2 ? 64_b :
				  f.apple >= 3 ? 16_b : 0_b),
				 (f.mac   >= 2 ? 256_b : 0_b)
				);

		outProps.maxNumberOfColorRenderTargetsPerRenderPassDescriptor =
			Max( (f.apple == 1 ? 4 :
				  f.apple >= 2 ? 8 : 0),
				 (f.mac   >= 2 ? 8 : 0)
				);
		outProps.maxSizeOfPointPrimitive = 54;

		outProps.maxTotalRenderTargetSizePerPixel =
			Max( (f.apple == 1 ? 128_b / 8 :
				  f.apple <= 3 ? 256_b / 8 :
				  f.apple >= 4 ? 512_b / 8 : 0_b),
				 (f.mac   >= 2 ? UMax : 0_b)
				);
		outProps.maxVisibilityQueryOffset =
			Max( (f.apple <= 6 ? 65528_b :
				  f.apple >= 7 ? 256_Kb : 0_b),
				 (f.mac   >= 2 ? 256_Kb : 0_b)
				);

		outProps.maxNumberOfFences = 32768;
		outProps.maxVertexCountForVertexAmplification =
			(f.apple >= 6 ? 2 :
			 0);

		const Bytes			max_value_align = 16_b;	// 16 byte alignment for float4 and float4x4

		// In specs:
		//   For buffers in the device address space, align the offset to the data type consumed by the
		//   compute function (which is always less than or equal to 16 bytes).
		//   For buffers in the constant address space, align the offset to 256 bytes in macOS. In iOS, align
		//   the offset to the maximum of either the data type consumed by the compute function, or 4 bytes.
		//   A 16-byte alignment is safe in iOS if you don't need to consider the data type.
		outProps.minUniformBufferOffsetAlign	= AlignUp( outProps.minUniformBufferOffsetAlign, max_value_align );
		outProps.minStorageBufferOffsetAlign	= max_value_align;
	}

/*
=================================================
	InitFeatureSet
=================================================
*/
	void  MFeatureSet::InitFeatureSet (const MGPUFamilies &f, INOUT FeatureSet &outFeatureSet) C_NE___
	{
		StaticAssert( sizeof(FeatureSet) == 696 );

		using SubgroupOperationBits = FeatureSet::SubgroupOperationBits;

		const auto	MetalIsAtLeast = [] (Version2 msl, Version2 mac, Version2 ios)
		{{
			Unused( msl, mac, ios );
			if constexpr( AppleUtils::Is_MacOS )	return msl >= mac;
			if constexpr( AppleUtils::Is_iOS )		return msl >= ios;
		}};

		const EFeature	True	= EFeature::RequireTrue;
		const EFeature	False	= EFeature::RequireFalse;

		ASSERT( outFeatureSet.maxShaderVersion.metal > 0 );
		const Version2	msl				= Version2::From100( outFeatureSet.maxShaderVersion.metal );
		const Version2	os_ver			{ AppleUtils::GetOSVersion() };
		const bool		mac13_ios16		= AppleUtils::VersionIsAtLeast( os_ver, Version2{13,0}, Version2{16,0} );
		const bool		mac_ios14		= AppleUtils::VersionIsAtLeast( os_ver, Version2{10,0}, Version2{14,0} );
		const bool		mac20_ios22		= MetalIsAtLeast( msl, Version2{2,0}, Version2{2,2} );
		const bool		mac21_ios22		= MetalIsAtLeast( msl, Version2{2,1}, Version2{2,2} );
		const bool		mac21_ios23		= MetalIsAtLeast( msl, Version2{2,1}, Version2{2,3} );
		const bool		mac21			= AppleUtils::Is_MacOS and (msl >= Version2{2,1});
		const bool		msl22			= msl >= Version2{2,2};
		const bool		msl30			= msl >= Version2{3,0};

		outFeatureSet.SetAll( False );

		EShaderStages	all_stages = EShaderStages::Compute | EShaderStages::Vertex | EShaderStages::Fragment;
		if ( features.tileShaders )		all_stages |= EShaderStages::Tile;
		if ( features.meshShader )		all_stages |= (EShaderStages::Mesh | EShaderStages::MeshTask);
		//if ( features.rayTracing() )	all_stages |= EShaderStages::AllRayTracing;		// TODO

		// render states
		{
			outFeatureSet.alphaToOne						= True;
			outFeatureSet.depthBiasClamp					= True;
			outFeatureSet.depthClamp						= True;
			outFeatureSet.depthBounds						= features.depthBounds() ? True : False;
			outFeatureSet.dualSrcBlend						= features.dualSourceBlending ? True : False;
			outFeatureSet.fillModeNonSolid					= True;
			outFeatureSet.independentBlend					= True;
			outFeatureSet.logicOp							= False;
			outFeatureSet.sampleRateShading					= True;
			outFeatureSet.constantAlphaColorBlendFactors	= True;
			outFeatureSet.pointPolygons						= False;
		//	outFeatureSet.separateStencilMaskRef			= True;
			outFeatureSet.triangleFans						= False;
		}
		// subgroup
		if ( mac20_ios22 )
		{
			ASSERT( features.SIMD_barrier );
			ASSERT( features.quadScopedPermuteOperations );

			outFeatureSet.subgroup							= True;
			outFeatureSet.subgroupBroadcastDynamicId		= features.SIMD_scopedPermuteOperations or features.quadScopedPermuteOperations ? True : False;
			outFeatureSet.subgroupSizeControl				= features.SIMD_scopedPermuteOperations or features.quadScopedPermuteOperations ? True : False;
			outFeatureSet.shaderSubgroupUniformControlFlow	= False;

			{
				auto&	ops = outFeatureSet.subgroupOperations;

				// Basic
				if ( msl22 )		ops |= SubgroupOperationBits{ ESubgroupOperation::IndexAndSize };
				if ( mac21_ios22 )	ops |= SubgroupOperationBits{ ESubgroupOperation::Elect, ESubgroupOperation::Barrier };

				// Vote
				if ( mac21_ios22 )	ops |= SubgroupOperationBits{ ESubgroupOperation::Any, ESubgroupOperation::All };

				// Arithmetic
				if ( mac21_ios23 )	ops |= SubgroupOperationBits{ ESubgroupOperation::Add, ESubgroupOperation::Mul, ESubgroupOperation::Min, ESubgroupOperation::Max,
																  ESubgroupOperation::Or, ESubgroupOperation::Xor, ESubgroupOperation::InclusiveMul, ESubgroupOperation::InclusiveAdd,
																  ESubgroupOperation::ExclusiveAdd, ESubgroupOperation::ExclusiveMul };
				if ( mac21 )		ops |= ESubgroupOperation::And;

				// Ballot
				if ( mac21_ios22 )	ops |= SubgroupOperationBits{ ESubgroupOperation::Ballot, ESubgroupOperation::BroadcastFirst };
				if ( mac20_ios22 )	ops |= ESubgroupOperation::Broadcast;

				// Shuffle
				if ( mac20_ios22 )	ops |= SubgroupOperationBits{ ESubgroupOperation::Shuffle, ESubgroupOperation::ShuffleXor };

				// Shuffle Relative
				if ( mac20_ios22 )	ops |= SubgroupOperationBits{ ESubgroupOperation::ShuffleUp, ESubgroupOperation::ShuffleDown };

				// Quad
				if ( mac21_ios22 )	ops |= ESubgroupOperation::QuadBroadcast;
			}

			// represent a scalar or vector of any integer or floating-point type
			outFeatureSet.subgroupTypes = ESubgroupTypes::Float32 | ESubgroupTypes::Float16 |
										  ESubgroupTypes::Int32 | ESubgroupTypes::Int8 | ESubgroupTypes::Int16;
			if ( features.int64Math )
				outFeatureSet.subgroupTypes |= ESubgroupTypes::Int64;

			outFeatureSet.subgroupStages |= EShaderStages::Fragment | EShaderStages::Compute;
			if ( features.rayTracingFromCompute )
				outFeatureSet.subgroupStages |= EShaderStages::AllRayTracing;	// TODO: visible function
			if ( features.meshShader )
				outFeatureSet.subgroupStages |= EShaderStages::Mesh | EShaderStages::MeshTask;
			outFeatureSet.subgroupStages &= all_stages;

			outFeatureSet.subgroupQuadStages		 = outFeatureSet.subgroupStages | EShaderStages::Vertex;
			outFeatureSet.requiredSubgroupSizeStages = {};	// none

			outFeatureSet.minSubgroupSize = outFeatureSet.maxSubgroupSize = 4;	// require 'quadScopedPermuteOperations'

			if ( features.SIMD_scopedPermuteOperations )
			{
				auto	vendor_bits = outFeatureSet.vendorIds.include;
				ASSERT( vendor_bits.BitCount() == 1 );

				const EGPUVendor	vendor = vendor_bits.ExtractFirst();

				// default
				outFeatureSet.minSubgroupSize = 4;
				outFeatureSet.maxSubgroupSize = 32;

				switch ( vendor )
				{
					case EGPUVendor::Apple :
						break;	// default

					case EGPUVendor::AMD :
					{
						auto	device_bits = outFeatureSet.devicesIds.include;
						ASSERT_Eq( device_bits.BitCount(), 1 );

						switch ( device_bits.ExtractFirst() )
						{
							case EGraphicsDeviceID::AMD_RDNA1 :
							case EGraphicsDeviceID::AMD_RDNA2 :
							case EGraphicsDeviceID::AMD_RDNA2_APU :
								outFeatureSet.minSubgroupSize = 4;
								outFeatureSet.maxSubgroupSize = 64;
								break;
						}
						break;
					}
					case EGPUVendor::Intel :
						outFeatureSet.minSubgroupSize = 8;
						outFeatureSet.maxSubgroupSize = 32;
						break;
				}
			}
		}
		// types
		{
			outFeatureSet.shaderInt8		= True;
			outFeatureSet.shaderInt16		= True;
			outFeatureSet.shaderInt64		= features.int64Math ? True : False;
			outFeatureSet.shaderFloat16		= True;
			outFeatureSet.shaderFloat64		= False;
		}
		// uniform/storage buffer
		{
			outFeatureSet.storageBuffer16BitAccess				= True;
			outFeatureSet.uniformAndStorageBuffer16BitAccess	= True;
			outFeatureSet.storageInputOutput16					= True;
			outFeatureSet.storageBuffer8BitAccess				= True;
			outFeatureSet.uniformAndStorageBuffer8BitAccess		= True;
			outFeatureSet.uniformBufferStandardLayout			= True;
			outFeatureSet.scalarBlockLayout						= True;
			outFeatureSet.bufferDeviceAddress					= features.argbufTier >= 2 and mac13_ios16 ? True : False;
		}
		// push constant
		{
			outFeatureSet.storagePushConstant8		= True;		// TODO: not supported
			outFeatureSet.storagePushConstant16		= False;
		}
		// atomic
		{
			outFeatureSet.fragmentStoresAndAtomics			= msl22 ? True : False;
			outFeatureSet.vertexPipelineStoresAndAtomics	= msl22 ? True : False;
			outFeatureSet.shaderBufferFloat32Atomics		= msl30 and features.floatingPointAtomics ? True : False;
			outFeatureSet.shaderBufferFloat32AtomicAdd		= msl30 and features.floatingPointAtomics ? True : False;
			outFeatureSet.shaderSharedFloat32Atomics		= msl30 and features.floatingPointAtomics ? True : False;
			outFeatureSet.shaderSharedFloat32AtomicAdd		= msl30 and features.floatingPointAtomics ? True : False;
			outFeatureSet.shaderBufferFloat32AtomicMinMax	= msl30 and features.floatingPointAtomics ? True : False;
			outFeatureSet.shaderSharedInt64Atomics			= msl >= Version2{2,4} and features.int64Math ? True : False;
		}
		// output
		{
			outFeatureSet.shaderOutputViewportIndex		= features.multipleViewports ? True : False;
			outFeatureSet.shaderOutputLayer				= features.layeredRendering ? True : False;
			// TODO: layeredRenderingToMSTexture
		}
		// clock
		{
			outFeatureSet.shaderSubgroupClock		= False;
			outFeatureSet.shaderDeviceClock			= False;
		}
		// shader features/limits
		{
			outFeatureSet.shaderClipDistance		= True;
			outFeatureSet.shaderCullDistance		= False;
			outFeatureSet.shaderResourceMinLod		= (f.apple >= 6) ? True : False;	// TODO
			outFeatureSet.shaderDrawParameters		= True;
			outFeatureSet.runtimeDescriptorArray	= True;
			//outFeatureSet.shaderSMBuiltinsNV
			//outFeatureSet.shaderCoreBuiltinsARM
			outFeatureSet.shaderSampleRateInterpolationFunctions	= features.pullModelInterpolation ? True : False;
			outFeatureSet.shaderStencilExport		= True;

			// array dynamic indexing
			outFeatureSet.shaderSampledImageArrayDynamicIndexing		= features.arrayOfTexturesRead ? True : False;
			outFeatureSet.shaderStorageBufferArrayDynamicIndexing		= True;
			outFeatureSet.shaderStorageImageArrayDynamicIndexing		= features.arrayOfTexturesWrite ? True : False;
			outFeatureSet.shaderUniformBufferArrayDynamicIndexing		= True;
			outFeatureSet.shaderInputAttachmentArrayDynamicIndexing		= features.arrayOfTexturesRead ? True : False;
			outFeatureSet.shaderUniformTexelBufferArrayDynamicIndexing	= features.arrayOfTexturesRead ? True : False;
			outFeatureSet.shaderStorageTexelBufferArrayDynamicIndexing	= features.arrayOfTexturesWrite ? True : False;

			// non uniform indexing
			outFeatureSet.shaderUniformBufferArrayNonUniformIndexing		= False;
			outFeatureSet.shaderSampledImageArrayNonUniformIndexing			= features.arrayOfTexturesRead and features.arrayOfSamplers ? True : False;
			outFeatureSet.shaderStorageBufferArrayNonUniformIndexing		= False;
			outFeatureSet.shaderStorageImageArrayNonUniformIndexing			= features.arrayOfTexturesWrite ? True : False;
			outFeatureSet.shaderInputAttachmentArrayNonUniformIndexing		= features.arrayOfTexturesRead ? True : False;
			outFeatureSet.shaderUniformTexelBufferArrayNonUniformIndexing	= features.arrayOfTexturesRead ? True : False;
			outFeatureSet.shaderStorageTexelBufferArrayNonUniformIndexing	= features.arrayOfTexturesWrite ? True : False;
			outFeatureSet.shaderUniformBufferArrayNonUniformIndexingNative	= outFeatureSet.shaderUniformBufferArrayNonUniformIndexing;
			outFeatureSet.shaderSampledImageArrayNonUniformIndexingNative	= outFeatureSet.shaderSampledImageArrayNonUniformIndexing;
			outFeatureSet.shaderStorageBufferArrayNonUniformIndexingNative	= outFeatureSet.shaderStorageBufferArrayNonUniformIndexing;
			outFeatureSet.shaderStorageImageArrayNonUniformIndexingNative	= outFeatureSet.shaderStorageImageArrayNonUniformIndexing;
			outFeatureSet.shaderInputAttachmentArrayNonUniformIndexingNative= outFeatureSet.shaderInputAttachmentArrayNonUniformIndexing;

			// storage image format
			//outFeatureSet.shaderStorageImageMultisample			// TODO
			outFeatureSet.shaderStorageImageReadWithoutFormat			= True;
			outFeatureSet.shaderStorageImageWriteWithoutFormat			= True;

			// memory model
			outFeatureSet.vulkanMemoryModel								= False;
			outFeatureSet.vulkanMemoryModelDeviceScope					= False;
			outFeatureSet.vulkanMemoryModelAvailabilityVisibilityChains	= False;

			//
			//outFeatureSet.shaderDemoteToHelperInvocation
			//outFeatureSet.shaderTerminateInvocation
			//outFeatureSet.shaderZeroInitializeWorkgroupMemory
			//outFeatureSet.shaderIntegerDotProduct

			// fragment shader interlock
			outFeatureSet.fragmentShaderSampleInterlock			= features.rasterOrderGroups ? True : False;
			outFeatureSet.fragmentShaderPixelInterlock			= features.rasterOrderGroups ? True : False;
			//outFeatureSet.fragmentShaderShadingRateInterlock

			// fragment shading rate
			//outFeatureSet.pipelineFragmentShadingRate
			//outFeatureSet.primitiveFragmentShadingRate
			//outFeatureSet.attachmentFragmentShadingRate
			//outFeatureSet.primitiveFragmentShadingRateWithMultipleViewports
			//outFeatureSet.layeredShadingRateAttachments
			//outFeatureSet.fragmentShadingRateWithShaderDepthStencilWrites
			//outFeatureSet.fragmentShadingRateWithSampleMask
			//outFeatureSet.fragmentShadingRateWithShaderSampleMask
			//outFeatureSet.fragmentShadingRateWithFragmentShaderInterlock
			//outFeatureSet.fragmentShadingRateWithCustomSampleLocations
			//outFeatureSet.fragmentShadingRateTexelSize

			// inline ray tracing
			outFeatureSet.rayQuery			= features.rayTracingFromCompute or features.raytracingFromRender ? True : False;
			outFeatureSet.rayQueryStages	= (features.rayTracingFromCompute ? EShaderStages::Compute : EShaderStages::Unknown) |
											  (features.raytracingFromRender ? EShaderStages::AllGraphics : EShaderStages::Unknown);
			outFeatureSet.rayQueryStages	&= all_stages;

			// ray tracing
			outFeatureSet.rayTracingPipeline			= False;
			outFeatureSet.rayTraversalPrimitiveCulling	= False;
			outFeatureSet.maxRayRecursionDepth			= 0;

			// RTAS
			outFeatureSet.accelerationStructureIndirectBuild = False;

			// shader version
			//outFeatureSet.maxShaderVersion	// already defined

			outFeatureSet.drawIndirectFirstInstance	= True;
			outFeatureSet.drawIndirectCount			= False;	// use indirect command buffer instead

			// multiview
			//outFeatureSet.multiview					= True;		// TODO: vertex amplification?
			//outFeatureSet.multiviewGeometryShader		= False;
			//outFeatureSet.multiviewTessellationShader	= False;
			//outFeatureSet.maxMultiviewViewCount		= 32;

			// multi viewport
			outFeatureSet.multiViewport				= features.multipleViewports ? True : False;
			outFeatureSet.maxViewports				= 16;	// TODO

			// sample locations
			outFeatureSet.sampleLocations			= features.programmableSamplePositions ? True : False;
			outFeatureSet.variableSampleLocations	= False;

			// tessellation
			//outFeatureSet.tessellationIsolines
			//outFeatureSet.tessellationPointMode

			// shader limits
			outFeatureSet.maxTexelBufferElements			= LimitCast<uint>(properties.max1DTextureSize * 4_Kb);
			outFeatureSet.maxUniformBufferSize				= LimitCast<uint>(properties.maxBufferSize);
			outFeatureSet.maxStorageBufferSize				= LimitCast<uint>(properties.maxBufferSize);
			outFeatureSet.maxDescriptorSets					= GraphicsConfig::MaxDescriptorSets;
			outFeatureSet.perDescrSet_maxUniformBuffersDynamic = 8;
			outFeatureSet.perDescrSet_maxStorageBuffersDynamic = 4;
			outFeatureSet.perDescrSet.maxInputAttachments	= properties.maxNumberOfTexturesInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxSampledImages		= properties.maxNumberOfTexturesInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxSamplers			= properties.maxNumberOfSamplersInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxStorageBuffers		= properties.maxNumberOfBufferInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxStorageImages		= properties.maxNumberOfTexturesInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxUniformBuffers		= properties.maxNumberOfBufferInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxAccelStructures	= properties.maxNumberOfBufferInsideArgumentBuffer;
			outFeatureSet.perDescrSet.maxTotalResources		= (properties.maxNumberOfBufferInsideArgumentBuffer + properties.maxNumberOfTexturesInsideArgumentBuffer + properties.maxNumberOfSamplersInsideArgumentBuffer);
			outFeatureSet.perStage.maxInputAttachments		= outFeatureSet.maxDescriptorSets * properties.maxNumberOfTexturesInsideArgumentBuffer;
			outFeatureSet.perStage.maxSampledImages			= outFeatureSet.maxDescriptorSets * properties.maxNumberOfTexturesInsideArgumentBuffer;
			outFeatureSet.perStage.maxSamplers				= outFeatureSet.maxDescriptorSets * properties.maxNumberOfSamplersInsideArgumentBuffer;
			outFeatureSet.perStage.maxStorageBuffers		= outFeatureSet.maxDescriptorSets * properties.maxNumberOfBufferInsideArgumentBuffer;
			outFeatureSet.perStage.maxStorageImages			= outFeatureSet.maxDescriptorSets * properties.maxNumberOfTexturesInsideArgumentBuffer;
			outFeatureSet.perStage.maxUniformBuffers		= outFeatureSet.maxDescriptorSets * properties.maxNumberOfBufferInsideArgumentBuffer;
			outFeatureSet.perStage.maxAccelStructures		= outFeatureSet.maxDescriptorSets * properties.maxNumberOfBufferInsideArgumentBuffer;
			outFeatureSet.perStage.maxTotalResources		= outFeatureSet.maxDescriptorSets * (properties.maxNumberOfBufferInsideArgumentBuffer + properties.maxNumberOfTexturesInsideArgumentBuffer + properties.maxNumberOfSamplersInsideArgumentBuffer);
			outFeatureSet.maxTexelOffset					= 7;	// [-8, 7]
			outFeatureSet.maxTexelGatherOffset				= 7;	// [-8, 7]
			outFeatureSet.maxFragmentOutputAttachments		= CheckCast<ushort>(properties.maxNumberOfColorRenderTargetsPerRenderPassDescriptor);
			outFeatureSet.maxFragmentDualSrcAttachments		= features.dualSourceBlending ? 1 : 0;
			outFeatureSet.maxFragmentCombinedOutputResources= outFeatureSet.perStage.maxTotalResources;
			outFeatureSet.maxPushConstantsSize				= 4 << 10;
			outFeatureSet.maxTotalThreadgroupSize			= LimitCast<uint>(properties.maxThreadgroupMemoryLength);
			outFeatureSet.maxTotalTileMemory				= LimitCast<uint>(properties.maxTotalTileMemoryAllocation);
			outFeatureSet.maxVertAmplification				= LimitCast<uint>(properties.maxVertexCountForVertexAmplification);

			// compute shader
			outFeatureSet.maxComputeSharedMemorySize		= LimitCast<uint>(Max( properties.maxTotalThreadgroupMemoryAllocation, properties.maxThreadgroupMemoryLength ));
			outFeatureSet.maxComputeWorkGroupInvocations	= properties.maxThreadsPerThreadgroup;
			outFeatureSet.maxComputeWorkGroupSizeX			= Max( properties.maxThreadsPerThreadgroup3.x, properties.maxThreadsPerThreadgroup );
			outFeatureSet.maxComputeWorkGroupSizeY			= Max( properties.maxThreadsPerThreadgroup3.y, properties.maxThreadsPerThreadgroup );
			outFeatureSet.maxComputeWorkGroupSizeZ			= Max( properties.maxThreadsPerThreadgroup3.z, properties.maxThreadsPerThreadgroup );

			// mesh shader
			if ( features.meshShader )
			{
				outFeatureSet.taskShader							= True;
				outFeatureSet.meshShader							= True;
				outFeatureSet.maxTaskWorkGroupSize					= properties.maxThreadsPerThreadgroup;
				outFeatureSet.maxMeshWorkGroupSize					= properties.maxThreadsPerThreadgroup;
				//		set default		// TODO: validate
				outFeatureSet.maxMeshOutputVertices					= 128;
				outFeatureSet.maxMeshOutputPrimitives				= 128;
				outFeatureSet.maxMeshOutputPerVertexGranularity		= 32;
				outFeatureSet.maxMeshOutputPerPrimitiveGranularity	= 32;
				outFeatureSet.maxTaskPayloadSize					= (16<<10);
				outFeatureSet.maxTaskSharedMemorySize				= (16<<10);
				outFeatureSet.maxTaskPayloadAndSharedMemorySize		= (16<<10);
				outFeatureSet.maxMeshSharedMemorySize				= (16<<10);
				outFeatureSet.maxMeshPayloadAndSharedMemorySize		= (16<<10);
				outFeatureSet.maxMeshOutputMemorySize				= (16<<10);
				outFeatureSet.maxMeshPayloadAndOutputMemorySize		= (16<<10);
				outFeatureSet.maxMeshMultiviewViewCount				= 0;
				outFeatureSet.maxPreferredTaskWorkGroupInvocations	= 32;
				outFeatureSet.maxPreferredMeshWorkGroupInvocations	= 32;
			}

			// raster order group
			outFeatureSet.maxRasterOrderGroups		= CheckCast<ushort>(properties.maxNumberOfRasterOrderGroupsPerFragmentFunction);

			// shaders
			outFeatureSet.geometryShader			= False;
			outFeatureSet.tessellationShader		= False;
			outFeatureSet.computeShader				= True;
			outFeatureSet.tileShader				= features.tileShaders ? True : False;

			// vertex buffer
			outFeatureSet.maxVertexAttributes		= properties.maxNumberOfVertexAttribsPerVertexDescriptor;
			outFeatureSet.maxVertexBuffers			= 31;	// TODO ?
		}
		// buffer
		{
			outFeatureSet.vertexFormats = FeatureSet::VertexFormatSet_t{
				EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte3, EVertexType::Byte4,
				EVertexType::UByte, EVertexType::UByte2, EVertexType::UByte3, EVertexType::UByte4,
				EVertexType::Short, EVertexType::Short2, EVertexType::Short3, EVertexType::Short4,
				EVertexType::UShort, EVertexType::UShort2, EVertexType::UShort3, EVertexType::UShort4,
				EVertexType::Int, EVertexType::Int2, EVertexType::Int3, EVertexType::Int4,
				EVertexType::UInt, EVertexType::UInt2, EVertexType::UInt3, EVertexType::UInt4,
				EVertexType::Half, EVertexType::Half2, EVertexType::Half3, EVertexType::Half4,
				EVertexType::Float, EVertexType::Float2, EVertexType::Float3, EVertexType::Float4,
				EVertexType::Byte_Norm, EVertexType::Byte2_Norm, EVertexType::Byte3_Norm, EVertexType::Byte4_Norm,
				EVertexType::UByte_Norm, EVertexType::UByte2_Norm, EVertexType::UByte3_Norm, EVertexType::UByte4_Norm,
				EVertexType::Short_Norm, EVertexType::Short2_Norm, EVertexType::Short3_Norm, EVertexType::Short4_Norm,
				EVertexType::UShort_Norm, EVertexType::UShort2_Norm, EVertexType::UShort3_Norm, EVertexType::UShort4_Norm,
				EVertexType::UInt_2_10_10_10_Norm, EVertexType::Byte_Scaled, EVertexType::Byte2_Scaled, EVertexType::Byte3_Scaled,
				EVertexType::Byte4_Scaled, EVertexType::UByte_Scaled, EVertexType::UByte2_Scaled, EVertexType::UByte3_Scaled,
				EVertexType::UByte4_Scaled, EVertexType::Short_Scaled, EVertexType::Short2_Scaled, EVertexType::Short3_Scaled,
				EVertexType::Short4_Scaled, EVertexType::UShort_Scaled, EVertexType::UShort2_Scaled, EVertexType::UShort3_Scaled,
				EVertexType::UShort4_Scaled
			};
			outFeatureSet.uniformTexBufferFormats = FeatureSet::PixelFormatSet_t{
				EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm,
				EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm,
				EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm,
				EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8,
				EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I,
				EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I,
				EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U,
				EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U,
				EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F,
				EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F,
				EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB9F_E5	// TODO
			};
			outFeatureSet.storageTexBufferFormats = FeatureSet::PixelFormatSet_t{
				EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm,
				EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm,
				EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm,
				EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I,
				EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I,
				EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U,
				EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U,
				EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U,
				EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F,
				EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F	// TODO
			};
			if ( f.apple >= 2 ) {
				outFeatureSet.storageTexBufferAtomicFormats = FeatureSet::PixelFormatSet_t{
					EPixelFormat::R32I, EPixelFormat::R32U
				};
			}
			if ( features.accelerationStructure() )
				outFeatureSet.accelStructVertexFormats = FeatureSet::VertexFormatSet_t{ EVertexType::Float3 };	// TODO: metal3
		}
		// image
		{
			outFeatureSet.imageCubeArray				= features.cubeMapTextureArrays ? True : False;
			outFeatureSet.textureCompressionASTC_LDR	= features.pixelFormat_ASTC ? True : False;
			outFeatureSet.textureCompressionASTC_HDR	= features.pixelFormat_ASTC and f.apple >= 6 ? True : False;
			outFeatureSet.textureCompressionBC			= features.pixelFormat_BC ? True : False;
			outFeatureSet.textureCompressionETC2		= features.pixelFormat_ETC ? True : False;
			outFeatureSet.imageViewMinLod				= False;
			outFeatureSet.multisampleArrayImage			= mac_ios14 ? True : False;
			outFeatureSet.maxImageArrayLayers			= properties.maxNumberOfLayersPerTextureArray_1D_2D_3D;
			outFeatureSet.imageViewFormatList			= True;	// TODO
			outFeatureSet.imageViewExtendedUsage		= True;	// TODO

			FeatureSet::PixelFormatSet_t	sparseImageFormats;			// TODO
			FeatureSet::PixelFormatSet_t	multisampleImageFormats;	// TODO

			if ( f.apple >= 2 or f.mac >= 2 )
			{
				outFeatureSet.storageImageAtomicFormats	|= FeatureSet::PixelFormatSet_t{ EPixelFormat::R32I, EPixelFormat::R32U };

				// Write
				outFeatureSet.storageImageFormats		|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::R8_UNorm, EPixelFormat::R8_SNorm, EPixelFormat::R8U, EPixelFormat::R8I,
					EPixelFormat::R16_UNorm, EPixelFormat::R16_SNorm, EPixelFormat::R16U, EPixelFormat::R16I,
					EPixelFormat::R16F, EPixelFormat::RG8_UNorm, EPixelFormat::RG8_SNorm, EPixelFormat::RG8U, EPixelFormat::RG8I,
					EPixelFormat::R32U, EPixelFormat::R32I, EPixelFormat::R32F, EPixelFormat::RG16_UNorm, EPixelFormat::RG16_SNorm,
					EPixelFormat::RG16U, EPixelFormat::RG16I, EPixelFormat::RG16F, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGBA8_SNorm,
					EPixelFormat::sRGB8_A8, EPixelFormat::RGBA8U, EPixelFormat::RGBA8I, EPixelFormat::BGRA8_UNorm, EPixelFormat::sBGR8_A8,
					EPixelFormat::RG32U, EPixelFormat::RG32I, EPixelFormat::RG32F, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA16_SNorm,
					EPixelFormat::RGBA16U, EPixelFormat::RGBA16I, EPixelFormat::RGBA16F, EPixelFormat::RGBA32U, EPixelFormat::RGBA32I, EPixelFormat::RGBA32F
				};
				// Blend
				outFeatureSet.attachmentBlendFormats	|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::R8_UNorm, EPixelFormat::R8_SNorm, EPixelFormat::R16F, EPixelFormat::R16_UNorm, EPixelFormat::R16_SNorm,
					EPixelFormat::RG8_UNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R32F, EPixelFormat::RG16_UNorm, EPixelFormat::RG16_SNorm,
					EPixelFormat::RG16F, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::sRGB8_A8, EPixelFormat::BGRA8_UNorm,
					EPixelFormat::sBGR8_A8, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RG32F, EPixelFormat::RGBA16_UNorm,
					EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA16F, EPixelFormat::RGB_11_11_10F
				};
				// Color
				outFeatureSet.attachmentFormats			|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::R8_UNorm, EPixelFormat::R8_SNorm, EPixelFormat::R8U, EPixelFormat::R8I,
					EPixelFormat::R16_UNorm, EPixelFormat::R16_SNorm, EPixelFormat::R16U, EPixelFormat::R16I,
					EPixelFormat::R16F, EPixelFormat::RG8_UNorm, EPixelFormat::RG8_SNorm, EPixelFormat::RG8U, EPixelFormat::RG8I,
					EPixelFormat::R32U, EPixelFormat::R32I, EPixelFormat::R32F, EPixelFormat::RG16_UNorm, EPixelFormat::RG16_SNorm,
					EPixelFormat::RG16F, EPixelFormat::RG16U, EPixelFormat::RG16I, EPixelFormat::RGBA8_UNorm,
					EPixelFormat::RGBA8_SNorm, EPixelFormat::sRGB8_A8, EPixelFormat::RGBA8U, EPixelFormat::RGBA8I,
					EPixelFormat::BGRA8_UNorm, EPixelFormat::sBGR8_A8, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB10_A2U,
					EPixelFormat::RG32U, EPixelFormat::RG32I, EPixelFormat::RG32F, EPixelFormat::RGBA16_UNorm,
					EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA16U, EPixelFormat::RGBA16I, EPixelFormat::RGBA16F,
					EPixelFormat::RGBA32U, EPixelFormat::RGBA32I, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
				};
				// Depth
				outFeatureSet.attachmentFormats			|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::Depth16, EPixelFormat::Depth32F, EPixelFormat::Depth32F_Stencil8
				};
				// Filter
				outFeatureSet.linearSampledFormats		|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::R8_UNorm, EPixelFormat::R8_SNorm, EPixelFormat::R16_UNorm, EPixelFormat::R16_SNorm,
					EPixelFormat::R16F, EPixelFormat::RG8_UNorm, EPixelFormat::RG8_SNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG16_SNorm,
					EPixelFormat::RG16F, EPixelFormat::RGBA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::RGBA8_SNorm,
					EPixelFormat::BGRA8_UNorm, EPixelFormat::sBGR8_A8, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB9F_E5,
					EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA16F, EPixelFormat::RGB_11_11_10F,
					EPixelFormat::Depth16
				};
				// MSAA
				multisampleImageFormats |= FeatureSet::PixelFormatSet_t{
					EPixelFormat::R8_UNorm, EPixelFormat::R8_SNorm, EPixelFormat::R8U, EPixelFormat::R8I,
					EPixelFormat::R16_UNorm, EPixelFormat::R16_SNorm, EPixelFormat::R16U, EPixelFormat::R16I,
					EPixelFormat::R16F, EPixelFormat::RG8_UNorm, EPixelFormat::RG8_SNorm, EPixelFormat::RG8U, EPixelFormat::RG8I,
					EPixelFormat::R32F, EPixelFormat::RG16_UNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG16U, EPixelFormat::RG16I,
					EPixelFormat::RG16F, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::sRGB8_A8,
					EPixelFormat::RGBA8U, EPixelFormat::RGBA8I, EPixelFormat::BGRA8_UNorm, EPixelFormat::sBGR8_A8,
					EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB10_A2U,  EPixelFormat::RGB_11_11_10F,
					EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA16F, EPixelFormat::RGBA16U, EPixelFormat::RGBA16I,
					EPixelFormat::Depth16, EPixelFormat::Depth32F, EPixelFormat::Depth32F_Stencil8
				};
			}
			if ( f.apple >= 2 )
			{
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::ASTC_RGBA8_4x4, EPixelFormat::ASTC_RGBA8_12x12 );
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::ASTC_sRGB8_A8_4x4, EPixelFormat::ASTC_sRGB8_A8_12x12 );
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ETC2_sRGB8_A8 );
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_RG11_UNorm );

				outFeatureSet.attachmentBlendFormats	|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::RGBA4_UNorm };
				outFeatureSet.attachmentFormats			|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB9F_E5 };
				outFeatureSet.linearSampledFormats		|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::RGBA4_UNorm };
				multisampleImageFormats					|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB9F_E5 };
			}
			if ( f.apple >= 3 )
			{
				outFeatureSet.linearSampledFormats		|= EPixelFormat::RGB_11_11_10F;
				outFeatureSet.storageImageFormats		|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB10_A2U, EPixelFormat::RGB9F_E5, EPixelFormat::RGB_11_11_10F };
				outFeatureSet.attachmentBlendFormats	|= EPixelFormat::RGB9F_E5;
			}
			if ( f.apple >= 6 )
			{
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::ASTC_RGBA16F_4x4, EPixelFormat::ASTC_RGBA16F_12x12 );

				sparseImageFormats		|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::R8_UNorm, EPixelFormat::R8_SNorm, EPixelFormat::R8U, EPixelFormat::R8I,
					EPixelFormat::R16_UNorm, EPixelFormat::R16_SNorm, EPixelFormat::R16U, EPixelFormat::R16I,
					EPixelFormat::R16F, EPixelFormat::RG8_UNorm, EPixelFormat::RG8_SNorm, EPixelFormat::RG8U, EPixelFormat::RG8I,
					EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::RGBA4_UNorm,
					EPixelFormat::R32U, EPixelFormat::R32I, EPixelFormat::R32F, EPixelFormat::RG16_UNorm, EPixelFormat::RG16_SNorm,
					EPixelFormat::RG16U, EPixelFormat::RG16I, EPixelFormat::RG16F, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGBA8_SNorm,
					EPixelFormat::sRGB8_A8, EPixelFormat::RGBA8U, EPixelFormat::RGBA8I, EPixelFormat::BGRA8_UNorm, EPixelFormat::sBGR8_A8,
					EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB10_A2U, EPixelFormat::RGB9F_E5, EPixelFormat::RG32U, EPixelFormat::RG32I,
					EPixelFormat::RG32F, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA16U, EPixelFormat::RGBA16I,
					EPixelFormat::RGBA16F, EPixelFormat::RGBA32U, EPixelFormat::RGBA32I, EPixelFormat::RGBA32F
				};
				sparseImageFormats.InsertRange( EPixelFormat::ASTC_RGBA8_4x4, EPixelFormat::ASTC_RGBA8_12x12 );
				sparseImageFormats.InsertRange( EPixelFormat::ASTC_sRGB8_A8_4x4, EPixelFormat::ASTC_sRGB8_A8_12x12 );
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ETC2_sRGB8_A8 );
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_RG11_UNorm );
			}
			if ( f.apple >= 7 )
			{
				multisampleImageFormats |= FeatureSet::PixelFormatSet_t{
					EPixelFormat::RG32U, EPixelFormat::RG32I, EPixelFormat::RG32F, EPixelFormat::RGBA32F
				};
			}
			if ( f.mac >= 2 )
			{
				outFeatureSet.linearSampledFormats.InsertRange( EPixelFormat::BC1_RGB8_UNorm, EPixelFormat::BC7_sRGB8_A8 );

				outFeatureSet.attachmentBlendFormats	|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGBA32F };
				outFeatureSet.attachmentFormats			|= FeatureSet::PixelFormatSet_t{ EPixelFormat::Depth24_Stencil8 };
				outFeatureSet.storageImageFormats		|= FeatureSet::PixelFormatSet_t{ EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB10_A2U, EPixelFormat::RGB_11_11_10F };

				outFeatureSet.linearSampledFormats		|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::RGB_11_11_10F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F,
					EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8
				};
				multisampleImageFormats					|= FeatureSet::PixelFormatSet_t{
					EPixelFormat::RG32U, EPixelFormat::RG32I, EPixelFormat::Depth24_Stencil8
				};
			}
		}
		// sampler
		{
			outFeatureSet.samplerAnisotropy					= True;
			outFeatureSet.samplerMirrorClampToEdge			= (f.mac >= 1 or f.apple >= 7) and mac_ios14 ? True : False;
			outFeatureSet.samplerFilterMinmax				= False;
			outFeatureSet.filterMinmaxImageComponentMapping	= False;
			outFeatureSet.samplerMipLodBias					= False;
			outFeatureSet.maxSamplerAnisotropy				= 16;
			outFeatureSet.maxSamplerLodBias					= 4;	// TODO
		}
		// framebuffer
		{
			outFeatureSet.framebufferColorSampleCounts	= FeatureSet::SampleCountBits( properties.maxTextureSampleCount.to_ulong() );
			outFeatureSet.framebufferDepthSampleCounts	= FeatureSet::SampleCountBits( properties.maxTextureSampleCount.to_ulong() );
			outFeatureSet.maxFramebufferLayers			= properties.maxNumberOfLayersPerTextureArray_1D_2D_3D;
		}
		// render pass
		{
			//outFeatureSet.variableMultisampleRate
		}
		// metal
		{
			outFeatureSet.metalArgBufferTier	= features.argbufTier;
		}
		// other
		{
			outFeatureSet.queues.supported |= EQueueMask::Graphics;

			if ( f.apple >= 7 or f.mac >= 2 )
				outFeatureSet.queues.supported |= EQueueMask::AsyncCompute;

			if ( mac13_ios16 )
				outFeatureSet.queues.supported |= EQueueMask::AsyncTransfer;	// IO command queue

			//outFeatureSet.vendorIds		// already defined
			//outFeatureSet.devicesIds		// already defined
		}
	}

} // AE::Graphics
