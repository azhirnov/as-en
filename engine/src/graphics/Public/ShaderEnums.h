// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

    //
    // Shader Type
    //
    enum class EShader : ubyte
    {
        Vertex,
        TessControl,
        TessEvaluation,
        Geometry,
        Fragment,

        Compute,
        Tile,               // subpass compute

        MeshTask,           // object shader in Metal
        Mesh,

        RayGen,
        RayAnyHit,
        RayClosestHit,
        RayMiss,
        RayIntersection,
        RayCallable,

        _Count,

        ClusterCulling  = MeshTask,
        Unknown         = 0xFF,
    };


    //
    // Shader Stages
    //
    enum class EShaderStages : ushort
    {
        Vertex          = 1 << uint(EShader::Vertex),
        TessControl     = 1 << uint(EShader::TessControl),
        TessEvaluation  = 1 << uint(EShader::TessEvaluation),
        Geometry        = 1 << uint(EShader::Geometry),
        Fragment        = 1 << uint(EShader::Fragment),
        Compute         = 1 << uint(EShader::Compute),
        Tile            = 1 << uint(EShader::Tile),
        MeshTask        = 1 << uint(EShader::MeshTask),
        Mesh            = 1 << uint(EShader::Mesh),
        RayGen          = 1 << uint(EShader::RayGen),
        RayAnyHit       = 1 << uint(EShader::RayAnyHit),
        RayClosestHit   = 1 << uint(EShader::RayClosestHit),
        RayMiss         = 1 << uint(EShader::RayMiss),
        RayIntersection = 1 << uint(EShader::RayIntersection),
        RayCallable     = 1 << uint(EShader::RayCallable),
        ClusterCulling  = 1 << uint(EShader::ClusterCulling),

        All             = (1 << uint(EShader::_Count)) - 1,
        GraphicsStages  = Vertex | TessControl | TessEvaluation | Geometry | Fragment,
        MeshStages      = MeshTask | Mesh | Fragment,
        AllGraphics     = GraphicsStages | MeshStages,
        AllRayTracing   = RayGen | RayAnyHit | RayClosestHit | RayMiss | RayIntersection | RayCallable,
        Unknown         = 0,
    };
    AE_BIT_OPERATORS( EShaderStages );

    ND_ forceinline EShaderStages  operator |  (EShaderStages lhs, EShader rhs)     { return lhs | EShaderStages(1 << uint(rhs)); }
        forceinline EShaderStages  operator |= (EShaderStages &lhs, EShader rhs)    { return (lhs |= EShaderStages(1 << uint(rhs))); }


} // AE::Graphics
