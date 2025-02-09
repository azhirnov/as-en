// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Hardware Ray Tracing.

	Requires AEStyle preprocessor.
*/

#ifdef __cplusplus
# pragma once
#endif


#ifdef AE_RTAS_BUILD
# ifndef AccelStructInstance_defined
#	define AccelStructInstance_defined

	// VkAccelerationStructureInstanceKHR
	// AE::Graphics::RTSceneBuild::InstanceVk
	struct AccelStructInstance
	{
		float3x4			transform;							// 3x4 row-major affine transformation matrix
		uint				instanceCustomIndex24_mask8;
		uint				instanceSBTOffset24_flags8;			// flags: gl::GeometryInstanceFlags
		gl::DeviceAddress	accelerationStructureReference;
	};
# endif

/*
=================================================
	AccelStructInstance_Create
=================================================
*/
	ND_ AccelStructInstance  AccelStructInstance_Create (const float3x4				transform,
														 uint						instanceCustomIndex,
														 uint						mask,
														 uint						instanceSBTOffset,
														 gl::GeometryInstanceFlags	flags,
														 gl::DeviceAddress			blasHandle)
	{
		AccelStructInstance	result;
		result.transform						= transform;
		result.instanceCustomIndex24_mask8		= ((instanceCustomIndex & 0xFFFFFF) << 8) | (mask & 0xFF);
		result.instanceSBTOffset24_flags8		= ((instanceSBTOffset & 0xFFFFFF) << 8) | (flags & 0xFF);
		result.accelerationStructureReference	= blasHandle;
		return result;
	}

/*
=================================================
	AccelStructInstance_Get*
=================================================
*/
	ND_ uint  AccelStructInstance_InstanceCustomIndex (const AccelStructInstance inst)
	{
		return inst.instanceCustomIndex24_mask8 >> 8;
	}

	ND_ uint  AccelStructInstance_Mask (const AccelStructInstance inst)
	{
		return inst.instanceCustomIndex24_mask8 & 0xFF;
	}

	ND_ uint  AccelStructInstance_InstanceSBTOffset (const AccelStructInstance inst)
	{
		return inst.instanceSBTOffset24_flags8 >> 8;
	}

	ND_ gl::GeometryInstanceFlags  AccelStructInstance_Flags (const AccelStructInstance inst)
	{
		return gl::GeometryInstanceFlags(inst.instanceSBTOffset24_flags8 & 0xFF);
	}
#endif
//-----------------------------------------------------------------------------



#ifdef AE_RTAS_BUILD
# ifndef ASBuildIndirectCommand_defined
#	define ASBuildIndirectCommand_defined

	// VkAccelerationStructureBuildRangeInfoKHR
	// AE::Graphics::ASBuildIndirectCommand
	struct ASBuildIndirectCommand
	{
		// Triangles count, AABBs count, Instances count
		uint		primitiveCount;

		uint		primitiveOffset;
		uint		firstVertex;
		uint		transformOffset;
	};
# endif

/*
=================================================
	ASBuildIndirectCommand_Create
=================================================
*/
	ND_ ASBuildIndirectCommand  ASBuildIndirectCommand_Create (uint primitiveCount)
	{
		ASBuildIndirectCommand	result;
		result.primitiveCount	= primitiveCount;
		result.primitiveOffset	= 0;
		result.firstVertex		= 0;
		result.transformOffset	= 0;
		return result;
	}

/*
=================================================
	ASBuildIndirectCommand_Create
=================================================
*/
	ND_ ASBuildIndirectCommand  ASBuildIndirectCommand_Create (uint primitiveCount, uint primitiveOffset, uint firstVertex, uint transformOffset)
	{
		ASBuildIndirectCommand	result;
		result.primitiveCount	= primitiveCount;
		result.primitiveOffset	= primitiveOffset;
		result.firstVertex		= firstVertex;
		result.transformOffset	= transformOffset;
		return result;
	}

#endif
//-----------------------------------------------------------------------------



#ifdef AE_RTAS_BUILD
# ifndef PTLAS_WriteInstance_defined
#	define PTLAS_WriteInstance_defined

	// VkPartitionedAccelerationStructureWriteInstanceDataNV
	// AE::Graphics::RTPartitionedSceneBuild::WriteInstanceVk
	struct PTLAS_WriteInstance
	{
		float3x4			transform;				// 3x4 row-major affine transformation matrix
		float				explicitAABB_minX;
		float				explicitAABB_minY;
		float				explicitAABB_minZ;
		float				explicitAABB_maxX;
		float				explicitAABB_maxY;
		float				explicitAABB_maxZ;
		uint				instanceCustomIndex;
		uint				instanceMask;
		uint				instanceSBTOffset;
		uint				instanceFlags;			// ERTInstanceOpt | VkPartitionedAccelerationStructureInstanceFlagsNV
		uint				instanceIndex;
		uint				partitionIndex;
		gl::DeviceAddress	rtas;
	};
# endif

# ifndef PTLAS_UpdateInstance_defined
#	define PTLAS_UpdateInstance_defined

	// VkPartitionedAccelerationStructureUpdateInstanceDataNV
	// AE::Graphics::RTPartitionedSceneBuild::UpdateInstanceVk
	struct PTLAS_UpdateInstance
	{
		uint				instanceIndex;
		uint				instanceSBTOffset;
		gl::DeviceAddress	rtas;
	};
# endif

# ifndef PTLAS_WritePartitionTranslation_defined
#	define PTLAS_WritePartitionTranslation_defined

	// VkPartitionedAccelerationStructureWritePartitionTranslationDataNV
	// AE::Graphics::RTPartitionedSceneBuild::UpdateInstanceVk
	struct PTLAS_WritePartitionTranslation
	{
		uint		partitionIndex;
		float		partitionTranslationX;
		float		partitionTranslationY;
		float		partitionTranslationZ;
	};
# endif

/*
=================================================
	PTLASWriteInstance_Create
=================================================
*/
	ND_ PTLAS_WriteInstance  PTLASWriteInstance_Create (float3x4			transform,
														float3				explicitAABB_min,
														float3				explicitAABB_max,
														uint				instanceCustomIndex,
														uint				instanceMask,
														uint				instanceSBTOffset,
														uint				instanceFlags,
														uint				instanceIndex,
														uint				partitionIndex,
														gl::DeviceAddress	rtas)
	{
		PTLAS_WriteInstance		result;
		result.transform			= transform;
		result.explicitAABB_minX	= explicitAABB_min.x;
		result.explicitAABB_minY	= explicitAABB_min.y;
		result.explicitAABB_minZ	= explicitAABB_min.z;
		result.explicitAABB_maxX	= explicitAABB_max.x;
		result.explicitAABB_maxY	= explicitAABB_max.y;
		result.explicitAABB_maxZ	= explicitAABB_max.z;
		result.instanceCustomIndex	= instanceCustomIndex;
		result.instanceMask			= instanceMask;
		result.instanceSBTOffset	= instanceSBTOffset;
		result.instanceFlags		= instanceFlags;
		result.instanceIndex		= instanceIndex;
		result.partitionIndex		= partitionIndex;
		result.rtas					= rtas;
		return result;
	}

/*
=================================================
	PTLASUpdateInstance_Create
=================================================
*/
	ND_ PTLAS_UpdateInstance  PTLASUpdateInstance_Create (uint				instanceIndex,
														  uint				instanceSBTOffset,
														  gl::DeviceAddress	rtas)
	{
		PTLAS_UpdateInstance	result;
		result.instanceIndex		= instanceIndex;
		result.instanceSBTOffset	= instanceSBTOffset;
		result.rtas					= rtas;
		return result;
	}

/*
=================================================
	PTLASWritePartitionTranslation_Create
=================================================
*/
	ND_ PTLAS_WritePartitionTranslation  PTLASWritePartitionTranslation_Create (uint	partitionIndex,
																				float3	partitionTranslation)
	{
		PTLAS_WritePartitionTranslation		result;
		result.partitionIndex			= partitionIndex;
		result.partitionTranslationX	= partitionTranslation.x;
		result.partitionTranslationY	= partitionTranslation.y;
		result.partitionTranslationZ	= partitionTranslation.z;
		return result;
	}

#endif
//-----------------------------------------------------------------------------



#if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT) or defined(SH_RAY_MISS) or defined(AE_ray_query)

	#include "Ray.glsl"

	struct HWRay
	{
		gl::RayFlags	rayFlags;
		uint			cullMask;			// only 8 bits are used
		uint			rayIndex;			// only 4 bits are used		// used as 'sbtRecordStride' and 'missIndex'
		float3			rayOrigin;
		float			tMin;				// must be non-negative, must be < tMax
		float3			rayDir;
		float			tMax;				// must be non-negative
	};

/*
=================================================
	HWRay_Create
=================================================
*/
	ND_ HWRay  HWRay_Create ()
	{
		HWRay	res;
		res.rayFlags	= gl::RayFlags::None;
		res.cullMask	= 0xFF;
		res.rayIndex	= 0;
		res.rayOrigin	= float3(0.0);
		res.tMin		= 0.01;
		res.rayDir		= float3(0.0, 0.0, 1.0);
		res.tMax		= 10.0;
		return res;
	}

	ND_ HWRay  HWRay_Create (const Ray ray, const float tMax, const uint rayIndex)
	{
		HWRay	res;
		res.rayFlags	= gl::RayFlags::None;
		res.cullMask	= 0xFF;
		res.rayIndex	= rayIndex;
		res.rayOrigin	= ray.origin;
		res.tMin		= ray.t;
		res.rayDir		= ray.dir;
		res.tMax		= tMax;
		return res;
	}

	ND_ HWRay  HWRay_Create (const Ray ray, const float tMax)
	{
		return HWRay_Create( ray, tMax, 0 );
	}

	ND_ HWRay  HWRay_Create (const Ray ray)
	{
		return HWRay_Create( ray, ray.t + 10.f, 0 );
	}

#endif
//-----------------------------------------------------------------------------



#if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT) or defined(SH_RAY_MISS)
/*
=================================================
	TraceRay
=================================================
*/
//	void   HWTraceRay (gl::AccelerationStructure rtas, const HWRay params, constexpr uint payloadLoc);
#	define HWTraceRay(/*gl::AccelerationStructure*/ _rtas_, /*HWRay*/ _params_, /*compile-time uint*/ _payloadLoc_) \
		gl.TraceRay( _rtas_,					\
					 _params_.rayFlags,			\
					 _params_.cullMask,			\
					 _params_.rayIndex,			\
					 /*sbtRecordStride*/0,		\
					 _params_.rayIndex,			\
					 _params_.rayOrigin,		\
					 _params_.tMin,				\
					 _params_.rayDir,			\
					 _params_.tMax,				\
					 _payloadLoc_ )

#endif
//-----------------------------------------------------------------------------



#ifdef AE_ray_query
/*
=================================================
	RayQuery_Init
=================================================
*/
//	void   RayQuery_Init (gl::RayQuery rquery, gl::AccelerationStructure rtas, const HWRay params);
#	define RayQuery_Init(/*gl::RayQuery*/ _rquery_, /*gl::AccelerationStructure*/ _rtas_, /*HWRay*/ _params_) \
		gl.rayQuery.Initialize(	_rquery_,			\
								_rtas_,				\
								_params_.rayFlags,	\
								_params_.cullMask,	\
								_params_.rayOrigin,	\
								_params_.tMin,		\
								_params_.rayDir,	\
								_params_.tMax )

// Committed

#	define /*auto*/		GetCommittedIntersectionType(/*gl::RayQuery*/ _rquery_)					gl::RayQueryCommittedIntersection(gl.rayQuery.GetIntersectionType( _rquery_, true ))
#	define /*float*/	GetCommittedIntersectionT(/*gl::RayQuery*/ _rquery_)					gl.rayQuery.GetIntersectionT( _rquery_, true )
#	define /*int*/		GetCommittedIntersectionInstanceCustomIndex(/*gl::RayQuery*/ _rquery_)	gl.rayQuery.GetIntersectionInstanceCustomIndex( _rquery_, true )
#	define /*int*/		GetCommittedIntersectionInstanceId(/*gl::RayQuery*/ _rquery_)			gl.rayQuery.GetIntersectionInstanceId( _rquery_, true )
#	define /*uint*/		GetCommittedIntersectionInstanceSBTOffset(/*gl::RayQuery*/ _rquery_) 	gl.rayQuery.GetIntersectionInstanceShaderBindingTableRecordOffset( _rquery_, true )
#	define /*int*/		GetCommittedIntersectionGeometryIndex(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionGeometryIndex( _rquery_, true )
#	define /*int*/		GetCommittedIntersectionPrimitiveIndex(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionPrimitiveIndex( _rquery_, true )
#	define /*float2*/	GetCommittedIntersectionBarycentrics(/*gl::RayQuery*/ _rquery_)			gl.rayQuery.GetIntersectionBarycentrics( _rquery_, true )
#	define /*bool*/		GetCommittedIntersectionFrontFace(/*gl::RayQuery*/ _rquery_)			gl.rayQuery.GetIntersectionFrontFace( _rquery_, true )
#	define /*float3*/	GetCommittedIntersectionObjectRayDirection(/*gl::RayQuery*/ _rquery_)	gl.rayQuery.GetIntersectionObjectRayDirection( _rquery_, true )
#	define /*float3*/	GetCommittedIntersectionObjectRayOrigin(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionObjectRayOrigin( _rquery_, true )
#	define /*float4x3*/	GetCommittedIntersectionObjectToWorld(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionObjectToWorld( _rquery_, true )
#	define /*float4x3*/	GetCommittedIntersectionWorldToObject(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionWorldToObject( _rquery_, true )
#	define /*float3x4*/	GetCommittedIntersectionObjectToWorld3x4(/*gl::RayQuery*/ _rquery_)		MatTranspose(gl.rayQuery.GetIntersectionObjectToWorld( _rquery_, true ))
#	define /*float3x4*/	GetCommittedIntersectionWorldToObject3x4(/*gl::RayQuery*/ _rquery_)		MatTranspose(gl.rayQuery.GetIntersectionWorldToObject( _rquery_, true ))
#	define /*void*/		GetCommittedIntersectionTriangleVertexPositions(/*gl::RayQuery*/ _rquery_, /*float3[3]*/_pos_)	gl.rayQuery.GetIntersectionTriangleVertexPositions( _rquery_, true, _pos_ )

// Candidate

#	define /*float*/	GetCandidateIntersectionT(/*gl::RayQuery*/ _rquery_)					gl.rayQuery.GetIntersectionT( _rquery_, false )
#	define /*int*/		GetCandidateIntersectionInstanceCustomIndex(/*gl::RayQuery*/ _rquery_)	gl.rayQuery.GetIntersectionInstanceCustomIndex( _rquery_, false )
#	define /*int*/		GetCandidateIntersectionInstanceId(/*gl::RayQuery*/ _rquery_)			gl.rayQuery.GetIntersectionInstanceId( _rquery_, false )
#	define /*auto*/		GetCandidateIntersectionType(/*gl::RayQuery*/ _rquery_)					gl::RayQueryCandidateIntersection(gl.rayQuery.GetIntersectionType( _rquery_, false ))
#	define /*uint*/		GetCandidateIntersectionInstanceSBTOffset(/*gl::RayQuery*/ _rquery_)	gl.rayQuery.GetIntersectionInstanceShaderBindingTableRecordOffset( _rquery_, false )
#	define /*int*/		GetCandidateIntersectionGeometryIndex(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionGeometryIndex( _rquery_, false )
#	define /*int*/		GetCandidateIntersectionPrimitiveIndex(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionPrimitiveIndex( _rquery_, false )
#	define /*float2*/	GetCandidateIntersectionBarycentrics(/*gl::RayQuery*/ _rquery_)			gl.rayQuery.GetIntersectionBarycentrics( _rquery_, false )
#	define /*bool*/		GetCandidateIntersectionFrontFace(/*gl::RayQuery*/ _rquery_)			gl.rayQuery.GetIntersectionFrontFace( _rquery_, false )
#	define /*float3*/	GetCandidateIntersectionObjectRayDirection(/*gl::RayQuery*/ _rquery_)	gl.rayQuery.GetIntersectionObjectRayDirection( _rquery_, false )
#	define /*float3*/	GetCandidateIntersectionObjectRayOrigin(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionObjectRayOrigin( _rquery_, false )
#	define /*float4x3*/	GetCandidateIntersectionObjectToWorld(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionObjectToWorld( _rquery_, false )
#	define /*float4x3*/	GetCandidateIntersectionWorldToObject(/*gl::RayQuery*/ _rquery_)		gl.rayQuery.GetIntersectionWorldToObject( _rquery_, false )
#	define /*float3x4*/	GetCandidateIntersectionObjectToWorld3x4(/*gl::RayQuery*/ _rquery_)		MatTranspose(gl.rayQuery.GetIntersectionObjectToWorld( _rquery_, false ))
#	define /*float3x4*/	GetCandidateIntersectionWorldToObject3x4(/*gl::RayQuery*/ _rquery_)		MatTranspose(gl.rayQuery.GetIntersectionWorldToObject( _rquery_, false ))
#	define /*void*/		GetCandidateIntersectionTriangleVertexPositions(/*gl::RayQuery*/ _rquery_, /*float3[3]*/_pos_)	gl.rayQuery.GetIntersectionTriangleVertexPositions( _rquery_, false, _pos_ )

#endif // AE_ray_query
//-----------------------------------------------------------------------------



/*
=================================================
	TriangleHitAttribsToBaricentrics
----
	use 'BaryLerp(... float2)' instead
=================================================
*/
	ND_ float3  TriangleHitAttribsToBaricentrics (const float2 hitAttribs)
	{
		return float3(1.0f - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y);
	}

/*
=================================================
	ComputeHitPos
----
	from https://github.com/NVIDIA/self-intersection-avoidance/blob/main/SelfIntersectionAvoidance.glsl
	(BSD-3-Clause license)
----
	Compute the object and world space position and normal corresponding to a triangle hit point.
	Compute a safe spawn point offset along the normal in world space to prevent self intersection of secondary rays.
=================================================
*/
#ifdef AE_LICENSE_BSD3
	void  ComputeHitPos (const float3	pos0, const float3 pos1, const float3 pos2,
						 const float2	barys,
						 const float4x3	objectToWorld,
						 const float4x3	worldToObject,
						 out float3		outObjectPos,
						 out float3		outWorldPos,
						 out float3		outObjectNorm,
						 out float3		outWorldNorm,
						 out float		outWorldOffset)	// safe offset for spawn position in world space
	{
		precise float3	edge1		= pos1 - pos0;
		precise float3	edge2		= pos2 - pos0;
		precise float3	obj_pos		= pos0 + FusedMulAdd( barys.xxx, edge1, (barys.yyy * edge2) );
		float3			obj_norm	= Cross( edge1, edge2 );

		precise float3	wld_pos;
		wld_pos.x =	objectToWorld[3][0] +
						FusedMulAdd( objectToWorld[0][0], obj_pos.x,
							FusedMulAdd( objectToWorld[1][0], obj_pos.y, (objectToWorld[2][0] * obj_pos.z) ));
		wld_pos.y =	objectToWorld[3][1] +
						FusedMulAdd( objectToWorld[0][1], obj_pos.x,
							FusedMulAdd( objectToWorld[1][1], obj_pos.y, (objectToWorld[2][1] * obj_pos.z) ));
		wld_pos.z =	objectToWorld[3][2] +
						FusedMulAdd( objectToWorld[0][2], obj_pos.x,
							FusedMulAdd( objectToWorld[1][2], obj_pos.y, (objectToWorld[2][2] * obj_pos.z) ));

		float3			wld_norm	= MatTranspose( float3x3( worldToObject )) * obj_norm;
		const float		wld_scale	= InvSqrt( Dot( wld_norm, wld_norm ));
						wld_norm	= wld_scale * wld_norm;		// normalize world space normal

		const float		c0			= 5.9604644775390625E-8f;
		const float		c1			= 1.788139769587360206060111522674560546875E-7f;

		const float3	extent3		= Abs( edge1 ) + Abs( edge2 ) + Abs( Abs( edge1 ) - Abs( edge2 ));
		const float		extent		= Max( Max( extent3.x, extent3.y ), extent3.z );

		// bound object space error due to reconstruction and intersection
		float3			obj_err		= FusedMulAdd( float3(c0), Abs( pos0 ), float3(c1 * extent) );

		// bound world space error due to object to world transform
		const float		c2			= 1.19209317972490680404007434844970703125E-7f;
		float4x3		abs_o2w		= float4x3( Abs( objectToWorld[0] ), Abs( objectToWorld[1] ), Abs( objectToWorld[2] ), Abs( objectToWorld[3] ));
		float3			wld_err		= FusedMulAdd( float3(c1), float3x3( abs_o2w ) * Abs( obj_pos ), (c2 * Abs( objectToWorld[3] )) );

		// bound object space error due to world to object transform
		float4x3		abs_w2o		= float4x3( Abs( worldToObject[0] ), Abs( worldToObject[1] ), Abs( worldToObject[2] ), Abs( worldToObject[3] ));
						obj_err		= FusedMulAdd( float3(c2), (abs_w2o * float4( Abs(wld_pos), 1.f )), obj_err );

		// compute world space self intersection avoidance offset
		float			wld_off		= Dot( wld_err, Abs( wld_norm ));
		float			obj_off		= Dot( obj_err, Abs( obj_norm ));
						wld_off		= FusedMulAdd( wld_scale, obj_off, wld_off );

		outObjectPos	= obj_pos;
		outWorldPos		= wld_pos;
		outObjectNorm	= Normalize( obj_norm );
		outWorldNorm	= wld_norm;
		outWorldOffset	= wld_off;
	}
#endif
/*
=================================================
	SafeSpawnPoint
----
	from https://github.com/NVIDIA/self-intersection-avoidance/blob/main/SelfIntersectionAvoidance.glsl
	(BSD-3-Clause license)
----
	Offset the world-space position along the world-space normal by the safe offset to obtain the safe spawn point.
=================================================
*/
#ifdef AE_LICENSE_BSD3
	ND_ float3  SafeSpawnPoint (const float3	position,
								const float3	normal,
								const float		offset)
	{
		precise float3 p = FusedMulAdd( float3(offset), normal, position );
		return p;
	}
#endif
//-----------------------------------------------------------------------------
