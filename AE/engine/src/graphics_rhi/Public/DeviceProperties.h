// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use 'FeatureSet'		to validate pipelines at compile time.
	Use 'DeviceProperties'	for runtime limits like a alignment.
	Use 'DeviceLimits'		for compile time limits like a alignment.

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/DeviceProperties.md)
*/

#pragma once

#include "graphics_rhi/Public/Common.h"
#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/DescriptorSet.h"

namespace AE::Graphics
{

	//
	// Runtime Device Properties
	//
	struct DeviceProperties
	{
	// types

		//
		// Ray Tracing Properties
		//
		struct RayTracingProperties
		{
			// acceleration structure data alignment
			POTBytes	vertexDataAlign;
			POTBytes	vertexStrideAlign;
			POTBytes	indexDataAlign;

			POTBytes	aabbDataAlign;
			POTBytes	aabbStrideAlign;

			POTBytes	transformDataAlign;

			POTBytes	instanceDataAlign;					// for device address
			POTBytes	instanceStrideAlign;				// Vulkan: not supported, Metal: supported

			POTBytes	scratchBufferAlign;					// for device address

			// TODO: shaderGroupBaseAlignment, shaderGroupHandleAlignment

			// acceleration structure limits
			ulong		maxGeometries				= 0;
			ulong		maxInstances				= 0;
			ulong		maxPrimitives				= 0;

			// ray tracing pipeline limits
			uint		maxRecursion				= 0;
			uint		maxDispatchInvocations		= 0;
			uint		maxThreadCount [3]			= {};

			// cluster acceleration structure limits
			uint		maxVerticesPerCluster		= 0;
			uint		maxTrianglesPerCluster		= 0;
			uint		maxClusterGeometryIndex		= 0;

			// partitioned top level acceleration structure limits
			uint		maxPartitionCount			= 0;
		};

		//
		// Resource Alignment
		//
		struct ResourceAlignment
		{
			POTBytes	minUniformBufferOffsetAlign;
			POTBytes	minStorageBufferOffsetAlign;
			POTBytes	minThreadgroupMemoryLengthAlign;		// Metal only
			POTBytes	minUniformTexelBufferOffsetAlign;
			POTBytes	minStorageTexelBufferOffsetAlign;

			POTValue	maxVerticesPerRenderPass;				// for Mali GPU
			POTBytes	minVertexBufferOffsetAlign;
			uint		minVertexBufferElementsAlign	{1};

			Bytes32u	maxUniformBufferRange			{1};

			uint		maxBoundDescriptorSets			= 0;

			// mapped memory
			POTBytes	minMemoryMapAlign;						// TODO: vulkan only?
			POTBytes	minNonCoherentAtomSize;

			POTBytes	minBufferCopyOffsetAlign;				// buffer <-> buffer copy alignment		Vulkan: optimal, Metal: required
			POTBytes	minBufferCopyRowPitchAlign;				// buffer <-> image copy alignment		Vulkan: optimal, Metal: required

			// video
		//	POTBytes	minVideoBitstreamBufferOffsetAlignment;
		//	POTBytes	minVideoBitstreamBufferSizeAlignment;
		};

		//
		// Shader Hardware Properties
		//
		struct ShaderHWProperties
		{
			uint		cores						= 0;	// NV: SM,  Apple: core,  ARM: core,  AMD: CU,  Intel: EU
			uint		warpsPerCore				= 0;	// zero if not known
			uint		threadsPerWarp				= 0;	// same as subgroup size
			uint		maxConcurrentWarpsPerCore	= 0;	// indicates register capacity

			ND_ uint	TotalWarps ()			C_NE___	{ return cores * Max( warpsPerCore, 1u ); }
			ND_ uint	TotalThreads ()			C_NE___	{ return TotalWarps() * threadsPerWarp; }	// min threads for full workload
			ND_ uint	MaxConcurrentThreads ()	C_NE___	{ return cores * maxConcurrentWarpsPerCore * threadsPerWarp; }
		};

		//
		// Compute & Mesh shader Properties
		//
		struct ComputeProperties
		{
			uint		computeGroupCount [3]	= {};
			
			uint		taskTotalGroups			= 0;
			uint		taskGroupCount [3]		= {};

			uint		meshTotalGroups			= 0;
			uint		meshGroupCount [3]		= {};
			
			bool		prefersLocalInvocationVertexOutput		: 1;
			bool		prefersLocalInvocationPrimitiveOutput	: 1;
			bool		prefersCompactVertexOutput				: 1;
			bool		prefersCompactPrimitiveOutput			: 1;

			__Cx__ ComputeProperties () __NE___ :
				prefersLocalInvocationVertexOutput{false}, prefersLocalInvocationPrimitiveOutput{false},
				prefersCompactVertexOutput{false}, prefersCompactPrimitiveOutput{false}
			{}
		};


	// variables
		ResourceAlignment		res;
		RayTracingProperties	rayTracing;
		ShaderHWProperties		shaderHW;
		ComputeProperties		compute;


	// methods
		ND_ bool  CompareWithConstant (AnyTypeCRef vkExt_mtlFS)			C_NE___;

			void  InitVulkan (AnyTypeCRef vkExt, AnyTypeCRef vkProps)	__NE___;
			void  InitMetal (AnyTypeCRef mtlFS, StringView devName)		__NE___;

			void  Print ()												C_NE___;
	};



	//
	// Device Resource Flags
	//
	struct DeviceResourceFlags
	{
	// variables
		// contains all available resource usage & options and memory types

		EBufferUsage				bufferUsage		= Default;
		EBufferOpt					bufferOptions	= Default;

		EImageUsage					imageUsage		= Default;
		EImageOpt					imageOptions	= Default;

		EnumSet<EDescriptorType>	descrTypes;

		FixedSet<EMemoryType, 8>	memTypes;


	// methods
		void  Print ()	C_NE___;
	};



	//
	// Device Memory Info
	//
	struct DeviceMemoryInfo
	{
		Bytes	deviceTotal;		// VRAM heap size
		Bytes	hostTotal;			// RAM heap size
		Bytes	unifiedTotal;		// host visible VRAM heap size
	};


	//
	// Device Memory Usage
	//
	struct DeviceMemoryUsage
	{
		Bytes	deviceUsage;		// VRAM used by process
		Bytes	deviceAvailable;	// VRAM available for allocation

		Bytes	hostUsage;			// RAM used by GPU process
		Bytes	hostAvailable;		// RAM available for allocation

		Bytes	unifiedUsage;		// host visible VRAM used by process
		Bytes	unifiedAvailable;	// host visible VRAM available for allocation
	};



	//
	// Compile time Device Properties
	//
	namespace _hidden_
	{
		struct CT_DeviceProperties : DeviceProperties
		{
			constexpr CT_DeviceProperties ()
			{
				StaticAssert( sizeof(DeviceProperties) == 168 );

				StaticAssert( sizeof(res) == 24 );
				{
					res.minUniformBufferOffsetAlign			= POTBytes_From< 256 >;		// nvidia - 64/256,  amd -  16,   intel -  64,   mali -  16,   adreno -  64,   apple - 16/32/256
					res.minStorageBufferOffsetAlign			= POTBytes_From< 256 >;		// nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16
					res.minThreadgroupMemoryLengthAlign		= POTBytes_From<  16 >;		//                                                                             apple - 16
					res.minUniformTexelBufferOffsetAlign	= POTBytes_From< 256 >;		// nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16/32/256
					res.minStorageTexelBufferOffsetAlign	= POTBytes_From< 256 >;		// nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16/32/256
					res.minVertexBufferOffsetAlign			= POTBytes_From< 16 >;		// vulkan -  1 (not specified),                                                apple - 16
					res.minVertexBufferElementsAlign		= 4;						// nvidia -  1,      amd -   1,   intel -   1,   mali -  4,    adreno - ?,     apple - ?
					res.maxVerticesPerRenderPass			= POTValue_From< 2ull<<30 >;//                                               mali - 2M+
					res.maxUniformBufferRange				= 16_KiB;					// nvidia - 64k,     amd - inf,   intel - inf,   mali - 64k,   adreno - 64k,   apple - inf         other - 16k
					res.maxBoundDescriptorSets				= 4;						// nvidia - 32,      amd -  32,   intel -   8,   mali -   4,   adreno -   4,   apple - 31
					res.minMemoryMapAlign					= POTBytes_From< 4<<10 >;	// nvidia - 64,      amd -  64,   intel -  4k,   mali -  64,   adreno -  64,   apple - ?
					res.minNonCoherentAtomSize				= POTBytes_From< 256 >;		// nvidia - 64,      amd - 128,   intel - 256,   mali -  64,   adreno -   1,   apple - 16/32/256
					res.minBufferCopyOffsetAlign			= POTBytes_From< 512 >;		// nvidia -  1,      amd -   1,   intel - 128,   mali -  64,   adreno -  64,   apple - 1           other - 256
					res.minBufferCopyRowPitchAlign			= POTBytes_From< 512 >;		// nvidia -  1,      amd -   1,   intel - 128,   mali -  64,   adreno -  64,   apple - 256         other - 256
				}
				StaticAssert( sizeof(rayTracing) == 80 );
				{
					rayTracing.vertexDataAlign				= POTBytes_From< 4 >;		// vulkan - 4,  metal - 4
					rayTracing.vertexStrideAlign			= POTBytes_From< 4 >;		// vulkan - 4,  metal - 4
					rayTracing.indexDataAlign				= POTBytes_From< 4 >;

					rayTracing.aabbStrideAlign				= POTBytes_From< 4 >;
					rayTracing.aabbDataAlign				= Max( POTBytes_From< 8 >, rayTracing.aabbStrideAlign, res.minStorageBufferOffsetAlign );

					rayTracing.transformDataAlign			= POTBytes_From< 16 >;

					rayTracing.instanceDataAlign			= Max( POTBytes_From< 64 >, res.minStorageBufferOffsetAlign );
					rayTracing.instanceStrideAlign			= POTBytes_From< 64 >;

					rayTracing.scratchBufferAlign			= POTBytes_From< 256 >;

					rayTracing.maxGeometries				= 16777215;
					rayTracing.maxInstances					= 16777215;
					rayTracing.maxPrimitives				= 16777215;

					rayTracing.maxRecursion					= 0;						// nvidia/intel - 31, amd/samsung - 1, apple - ???
					rayTracing.maxDispatchInvocations		= 67108864;					// amd/samsung/nvidia - 1073741824, amd - 67108864, intel - 4294967295, apple - ???

					//rayTracing.maxThreadCount				= {};
					
					//rayTracing.maxVerticesPerCluster		= 0;
					//rayTracing.maxTrianglesPerCluster		= 0;
					//rayTracing.maxClusterGeometryIndex	= 0;

					//rayTracing.maxPartitionCount			= 0;
				}
				// ignore shaderHW
			}
		};
	}
	static constexpr Graphics::_hidden_::CT_DeviceProperties	DeviceLimits {};

} // AE::Graphics


namespace AE::Base
{
	template <> struct TTriviallySerializable< Graphics::DeviceProperties >	: CT_True {};
	template <> struct TTriviallySerializable< Graphics::DeviceMemoryInfo >	: CT_True {};
}
