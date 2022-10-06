// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use 'FeatureSet' for static limits.
	Use 'DeviceFeatures' for runtime limits like a alignment.
*/

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Graphics Device Properties
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

			Bytes32u	scratchBufferOffsetAlign	{1};
		
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
		};


	// variables
		RayTracingProperties	rayTracing;
		ResourceAlignment		res;
	};


} // AE::Graphics
