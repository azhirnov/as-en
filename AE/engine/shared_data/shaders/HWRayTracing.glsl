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
#   define AccelStructInstance_defined
    struct AccelStructInstance
    {
        float3x4            transform;
        uint                instanceCustomIndex24_mask8;
        uint                instanceSBTOffset24_flags8;         // flags: gl::GeometryInstanceFlags
        gl::DeviceAddress   accelerationStructureReference;
    };
# endif

/*
=================================================
    AccelStructInstance_Create
=================================================
*/
    ND_ AccelStructInstance  AccelStructInstance_Create (const float3x4             transform,
                                                         uint                       instanceCustomIndex,
                                                         uint                       mask,
                                                         uint                       instanceSBTOffset,
                                                         gl::GeometryInstanceFlags  flags,
                                                         gl::DeviceAddress          blasHandle)
    {
        AccelStructInstance result;
        result.transform                        = transform;
        result.instanceCustomIndex24_mask8      = ((instanceCustomIndex & 0xFFFFFF) << 8) | (mask & 0xFF);
        result.instanceSBTOffset24_flags8       = ((instanceSBTOffset & 0xFFFFFF) << 8) | (flags & 0xFF);
        result.accelerationStructureReference   = blasHandle;
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
#   define ASBuildIndirectCommand_defined
    struct ASBuildIndirectCommand
    {
        // Triangles count, AABBs count, Instances count
        uint        primitiveCount;

        uint        primitiveOffset;
        uint        firstVertex;
        uint        transformOffset;
    };
# endif

/*
=================================================
    ASBuildIndirectCommand_Create
=================================================
*/
    ND_ ASBuildIndirectCommand  ASBuildIndirectCommand_Create (uint primitiveCount)
    {
        ASBuildIndirectCommand  result;
        result.primitiveCount   = primitiveCount;
        result.primitiveOffset  = 0;
        result.firstVertex      = 0;
        result.transformOffset  = 0;
        return result;
    }

/*
=================================================
    ASBuildIndirectCommand_Create
=================================================
*/
    ND_ ASBuildIndirectCommand  ASBuildIndirectCommand_Create (uint primitiveCount, uint primitiveOffset, uint firstVertex, uint transformOffset)
    {
        ASBuildIndirectCommand  result;
        result.primitiveCount   = primitiveCount;
        result.primitiveOffset  = primitiveOffset;
        result.firstVertex      = firstVertex;
        result.transformOffset  = transformOffset;
        return result;
    }

#endif
//-----------------------------------------------------------------------------



#if defined(SH_RAY_GEN) || defined(SH_RAY_CHIT) || defined(SH_RAY_MISS) || defined(AE_RAY_QUERY)

    #include "Ray.glsl"

    struct HWRay
    {
        gl::RayFlags    rayFlags;
        uint            cullMask;           // only 8 bits are used
        uint            sbtRecordOffset;    // only 4 bits are used
        uint            sbtRecordStride;    // only 4 bits are used
        uint            missIndex;          // only 16 bits are used
        float3          rayOrigin;
        float           tMin;               // must be non-negative, must be < tMax
        float3          rayDir;
        float           tMax;               // must be non-negative
    };

/*
=================================================
    HWRay_Create
=================================================
*/
    ND_ HWRay  HWRay_Create ()
    {
        HWRay   res;
        res.rayFlags        = gl::RayFlags::None;
        res.cullMask        = 0xFF;
        res.sbtRecordOffset = 0;
        res.sbtRecordStride = 0;
        res.missIndex       = 0;
        res.rayOrigin       = float3(0.0);
        res.tMin            = 0.01;
        res.rayDir          = float3(0.0, 0.0, 1.0);
        res.tMax            = 10.0;
        return res;
    }

    ND_ HWRay  HWRay_Create (const Ray ray, const float tMax, const uint rayIndex, const uint sbtRecordStride)
    {
        HWRay   res;
        res.rayFlags        = gl::RayFlags::None;
        res.cullMask        = 0xFF;
        res.sbtRecordOffset = rayIndex;
        res.sbtRecordStride = sbtRecordStride;
        res.missIndex       = rayIndex;
        res.rayOrigin       = ray.origin;
        res.tMin            = ray.t;
        res.rayDir          = ray.dir;
        res.tMax            = tMax;
        return res;
    }

    ND_ HWRay  HWRay_Create (const Ray ray, const float tMax)
    {
        return HWRay_Create( ray, tMax, 0, 0 );
    }

    ND_ HWRay  HWRay_Create (const Ray ray)
    {
        return HWRay_Create( ray, ray.t + 10.f, 0, 0 );
    }

#endif
//-----------------------------------------------------------------------------



#if defined(SH_RAY_GEN) || defined(SH_RAY_CHIT) || defined(SH_RAY_MISS)
/*
=================================================
    TraceRay
=================================================
*/
//  void   HWTraceRay (gl::AccelerationStructure rtas, const HWRay params, constexpr uint payloadLoc);
#   define HWTraceRay(/*gl::AccelerationStructure*/ _rtas_, /*HWRay*/ _params_, /*compile-time uint*/ _payloadLoc_) \
        gl.TraceRay( _rtas_,                    \
                     _params_.rayFlags,         \
                     _params_.cullMask,         \
                     _params_.sbtRecordOffset,  \
                     _params_.sbtRecordStride,  \
                     _params_.missIndex,        \
                     _params_.rayOrigin,        \
                     _params_.tMin,             \
                     _params_.rayDir,           \
                     _params_.tMax,             \
                     _payloadLoc_ )

#endif
//-----------------------------------------------------------------------------



#ifdef AE_RAY_QUERY
/*
=================================================
    RayQuery_Init
=================================================
*/
//  void   RayQuery_Init (gl::RayQuery rquery, gl::AccelerationStructure rtas, const HWRay params);
#   define RayQuery_Init(/*gl::RayQuery*/ _rquery_, /*gl::AccelerationStructure*/ _rtas_, /*HWRay*/ _params_) \
        gl.rayQuery.Initialize( _rquery_,           \
                                _rtas_,             \
                                _params_.rayFlags,  \
                                _params_.cullMask,  \
                                _params_.rayOrigin, \
                                _params_.tMin,      \
                                _params_.rayDir,    \
                                _params_.tMax )

// Committed

#   define /*auto*/     GetCommittedIntersectionType(/*gl::RayQuery*/ rquery)                   gl::RayQueryCommittedIntersection(gl.rayQuery.GetIntersectionType( rquery, true ))
#   define /*float*/    GetCommittedIntersectionT(/*gl::RayQuery*/ rquery)                      gl.rayQuery.GetIntersectionT( rquery, true )
#   define /*int*/      GetCommittedIntersectionInstanceCustomIndex(/*gl::RayQuery*/ rquery)    gl.rayQuery.GetIntersectionInstanceCustomIndex( rquery, true )
#   define /*int*/      GetCommittedIntersectionInstanceId(/*gl::RayQuery*/ rquery)             gl.rayQuery.GetIntersectionInstanceId( rquery, true )
#   define /*uint*/     GetCommittedIntersectionInstanceSBTOffset(/*gl::RayQuery*/ rquery)      gl.rayQuery.GetIntersectionInstanceShaderBindingTableRecordOffset( rquery, true )
#   define /*int*/      GetCommittedIntersectionGeometryIndex(/*gl::RayQuery*/ rquery)          gl.rayQuery.GetIntersectionGeometryIndex( rquery, true )
#   define /*int*/      GetCommittedIntersectionPrimitiveIndex(/*gl::RayQuery*/ rquery)         gl.rayQuery.GetIntersectionPrimitiveIndex( rquery, true )
#   define /*float2*/   GetCommittedIntersectionBarycentrics(/*gl::RayQuery*/ rquery)           gl.rayQuery.GetIntersectionBarycentrics( rquery, true )
#   define /*bool*/     GetCommittedIntersectionFrontFace(/*gl::RayQuery*/ rquery)              gl.rayQuery.GetIntersectionFrontFace( rquery, true )
#   define /*bool*/     GetCommittedIntersectionCandidateAABBOpaque(/*gl::RayQuery*/ rquery)    gl.rayQuery.GetIntersectionCandidateAABBOpaque( rquery, true )
#   define /*float3*/   GetCommittedIntersectionObjectRayDirection(/*gl::RayQuery*/ rquery)     gl.rayQuery.GetIntersectionObjectRayDirection( rquery, true )
#   define /*float3*/   GetCommittedIntersectionObjectRayOrigin(/*gl::RayQuery*/ rquery)        gl.rayQuery.GetIntersectionObjectRayOrigin( rquery, true )
#   define /*float4x3*/ GetCommittedIntersectionObjectToWorld(/*gl::RayQuery*/ rquery)          gl.rayQuery.GetIntersectionObjectToWorld( rquery, true )
#   define /*float4x3*/ GetCommittedIntersectionWorldToObject(/*gl::RayQuery*/ rquery)          gl.rayQuery.GetIntersectionWorldToObject( rquery, true )

// Candidate

#   define /*float*/    GetCandidateIntersectionT(/*gl::RayQuery*/ rquery)                      gl.rayQuery.GetIntersectionT( rquery, false )
#   define /*int*/      GetCandidateIntersectionInstanceCustomIndex(/*gl::RayQuery*/ rquery)    gl.rayQuery.GetIntersectionInstanceCustomIndex( rquery, false )
#   define /*int*/      GetCandidateIntersectionInstanceId(/*gl::RayQuery*/ rquery)             gl.rayQuery.GetIntersectionInstanceId( rquery, false )
#   define /*auto*/     GetCandidateIntersectionType(/*gl::RayQuery*/ rquery)                   gl::RayQueryCandidateIntersection(gl.rayQuery.GetIntersectionType( rquery, false ))
#   define /*uint*/     GetCandidateIntersectionInstanceSBTOffset(/*gl::RayQuery*/ rquery)      gl.rayQuery.GetIntersectionInstanceShaderBindingTableRecordOffset( rquery, false )
#   define /*int*/      GetCandidateIntersectionGeometryIndex(/*gl::RayQuery*/ rquery)          gl.rayQuery.GetIntersectionGeometryIndex( rquery, false )
#   define /*int*/      GetCandidateIntersectionPrimitiveIndex(/*gl::RayQuery*/ rquery)         gl.rayQuery.GetIntersectionPrimitiveIndex( rquery, false )
#   define /*float2*/   GetCandidateIntersectionBarycentrics(/*gl::RayQuery*/ rquery)           gl.rayQuery.GetIntersectionBarycentrics( rquery, false )
#   define /*bool*/     GetCandidateIntersectionFrontFace(/*gl::RayQuery*/ rquery)              gl.rayQuery.GetIntersectionFrontFace( rquery, false )
#   define /*bool*/     GetCandidateIntersectionCandidateAABBOpaque(/*gl::RayQuery*/ rquery)    gl.rayQuery.GetIntersectionCandidateAABBOpaque( rquery, false )
#   define /*float3*/   GetCandidateIntersectionObjectRayDirection(/*gl::RayQuery*/ rquery)     gl.rayQuery.GetIntersectionObjectRayDirection( rquery, false )
#   define /*float3*/   GetCandidateIntersectionObjectRayOrigin(/*gl::RayQuery*/ rquery)        gl.rayQuery.GetIntersectionObjectRayOrigin( rquery, false )
#   define /*float4x3*/ GetCandidateIntersectionObjectToWorld(/*gl::RayQuery*/ rquery)          gl.rayQuery.GetIntersectionObjectToWorld( rquery, false )
#   define /*float4x3*/ GetCandidateIntersectionWorldToObject(/*gl::RayQuery*/ rquery)          gl.rayQuery.GetIntersectionWorldToObject( rquery, false )

#endif // AE_RAY_QUERY
//-----------------------------------------------------------------------------



/*
=================================================
    TriangleHitAttribsToBaricentrics
=================================================
*/
    ND_ float3  TriangleHitAttribsToBaricentrics (const float2 hitAttribs)
    {
        return float3(1.0f - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y);
    }


//-----------------------------------------------------------------------------
