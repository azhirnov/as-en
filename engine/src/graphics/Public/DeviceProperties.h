// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use 'FeatureSet'		to validate pipelines at compile time.
	Use 'DeviceProperties'	for runtime limits like a alignment.
	Use 'DeviceLimits'		for compile time limits like a alignment.
*/

#pragma once

#include "graphics/Public/Common.h"

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
			Bytes32u	vertexDataAlign				{1};
			Bytes32u	vertexStrideAlign			{1};
			Bytes32u	indexDataAlign				{1};

			Bytes32u	aabbDataAlign				{1};
			Bytes32u	aabbStrideAlign				{1};

			Bytes32u	transformDataAlign			{1};

			Bytes32u	instanceDataAlign			{1};
			Bytes32u	instanceStrideAlign			{1};

			Bytes32u	minScratchBufferOffsetAlign	{1};
		
			// acceleration structure limits
			ulong		maxGeometries				= 0;
			ulong		maxInstances				= 0;
			ulong		maxPrimitives				= 0;

			// ray tracing pipeline limits
			uint		maxRecursion				= 0;
		};

		//
		// Resource Alignment
		//
		struct ResourceAlignment
		{
			Bytes32u	minUniformBufferOffsetAlign		{1};
			Bytes32u	minStorageBufferOffsetAlign		{1};
			Bytes32u	minThreadgroupMemoryLengthAlign	{1};	// Metal only
			Bytes32u	minUniformTexelBufferOffsetAlign{1};
			Bytes32u	minStorageTexelBufferOffsetAlign{1};
			
			Bytes32u	minVertexBufferOffsetAlign		{1};
			uint		minVertexBufferElementsAlign	{1};

			Bytes32u	maxUniformBufferRange			{1};

			uint		maxBoundDescriptorSets			= 0;
			
			// mapped memory
			Bytes32u	minMemoryMapAlignment			{1};	// TODO: vulkan only?
			Bytes32u	minNonCoherentAtomSize			{1};

			Bytes32u	minBufferCopyOffsetAlign		{1};	// buffer <-> buffer copy alignment		Vulkan: optimal, Metal: required
			Bytes32u	minBufferCopyRowPitchAlign		{1};	// buffer <-> image copy alignment		Vulkan: optimal, Metal: required
		};


	// variables
		ResourceAlignment		res;
		RayTracingProperties	rayTracing;


	// methods
		ND_ bool  CompareWithConstant ()						C_NE___;

			void  Init (AnyTypeCRef vkExt, AnyTypeCRef vkProps) __NE___;
			void  Init (AnyTypeCRef mtlFS)						__NE___;
	};
	


	//
	// Compiletime Device Properties
	//
	namespace _hidden_
	{
		struct CT_DeviceProperties : DeviceProperties
		{
			constexpr CT_DeviceProperties ()
			{
				STATIC_ASSERT( sizeof(DeviceProperties) == 128 );

				STATIC_ASSERT( sizeof(res) == sizeof(uint)*13 );
				{
					res.minUniformBufferOffsetAlign			= 256_b;	// nvidia - 64/256,  amd -  16,   intel -  64,   mali -  16,   adreno -  64,   apple - 16/32/256
					res.minStorageBufferOffsetAlign			= 256_b;	// nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16
					res.minThreadgroupMemoryLengthAlign		= 16_b;		//																		       apple - 16
					res.minUniformTexelBufferOffsetAlign	= 256_b;	// nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16/32/256
					res.minStorageTexelBufferOffsetAlign	= 256_b;	// nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16/32/256
					res.minVertexBufferOffsetAlign			= 256_b;	// vulkan -  1 (not specified),											       apple - 16
					res.minVertexBufferElementsAlign		= 4;		// nvidia -  1,      amd -   1,   intel -   1,   mali -  4,    adreno - ?,     apple - ?
					res.maxUniformBufferRange				= 16_Kb;	// nvidia - 64k,     amd - inf,   intel - inf,   mali - 64k,   adreno - 64k,   apple - inf         other - 16k
					res.maxBoundDescriptorSets				= 4;		// nvidia - 32,      amd -  32,   intel -   8,   mali -   4,   adreno -   4,   apple - 31
					res.minMemoryMapAlignment				= 4_Kb;		// nvidia - 64,      amd -  64,   intel -  4k,   mali -  64,   adreno -  64,   apple - ?
					res.minNonCoherentAtomSize				= 256_b;	// nvidia - 64,      amd - 128,   intel - 256,   mali -  64,   adreno -   1,   apple - 16/32/256
					res.minBufferCopyOffsetAlign			= 256_b;	// nvidia -  1,      amd -   1,   intel - 128,   mali -  64,   adreno -  64,   apple - 1           other - 256
					res.minBufferCopyRowPitchAlign			= 256_b;	// nvidia -  1,      amd -   1,   intel - 128,   mali -  64,   adreno -  64,   apple - 256         other - 256
				}
				STATIC_ASSERT( sizeof(rayTracing) == 72 );
				{
					rayTracing.vertexDataAlign				= 16_b;
					rayTracing.vertexStrideAlign			= 16_b;
					rayTracing.indexDataAlign				= 32_b;

					rayTracing.aabbStrideAlign				= 4_b;
					rayTracing.aabbDataAlign				= Max( 8_b, AlignUp( rayTracing.aabbStrideAlign, res.minStorageBufferOffsetAlign ));

					rayTracing.transformDataAlign			= 16_b;

					rayTracing.instanceDataAlign			= Max( 64_b, res.minStorageBufferOffsetAlign );
					rayTracing.instanceStrideAlign			= 64_b;

					rayTracing.minScratchBufferOffsetAlign	= 256_b;

					rayTracing.maxGeometries				= 16777215;
					rayTracing.maxInstances					= 16777215;
					rayTracing.maxPrimitives				= 536870911;

					rayTracing.maxRecursion					= 0;	// only inline ray tracing
				}
			}
		};
	}
	static constexpr Graphics::_hidden_::CT_DeviceProperties	DeviceLimits {};


} // AE::Graphics
