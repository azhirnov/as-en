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
                            // |  Vulkan  |  Metal  |
        Vertex,             //      x     |    x
        TessControl,        //      x     |    -
        TessEvaluation,     //      x     |    -
        Geometry,           //      x     |    -
        Fragment,           //      x     |    x

        Compute,            //      x     |    x
        Tile,               //     ext    |    x

        MeshTask,           //    task    |  object
        Mesh,               //      x     |    x

        RayGen,             //      x     |    -
        RayAnyHit,          //      x     |    -
        RayClosestHit,      //      x     |    -
        RayMiss,            //      x     |    -
        RayIntersection,    //      x     |    -
        RayCallable,        //      x     |    -

        _Count,

        ClusterCulling  = MeshTask, // VK_HUAWEI_cluster_culling_shader
        SubpassShading  = Tile,     // VK_HUAWEI_subpass_shading
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
        SubpassShading  = 1 << uint(EShader::SubpassShading),
        All             = (1 << uint(EShader::_Count)) - 1,

        GraphicsStages          = Vertex | TessControl | TessEvaluation | Geometry | Fragment,
        MeshStages              = MeshTask | Mesh | Fragment,
        VertexProcessingStages  = Vertex | TessControl | TessEvaluation | Geometry | Mesh,
        PreRasterizationStages  = MeshTask | VertexProcessingStages,
        PostRasterizationStages = Tile | Fragment,
        AllGraphics             = GraphicsStages | MeshStages,
        AllRayTracing           = RayGen | RayAnyHit | RayClosestHit | RayMiss | RayIntersection | RayCallable,
        Unknown                 = 0,
    };
    AE_BIT_OPERATORS( EShaderStages );

    ND_ constexpr EShaderStages  operator |  (EShaderStages lhs, EShader rhs)   __NE___ { return lhs | EShaderStages(1 << uint(rhs)); }
        constexpr EShaderStages  operator |= (EShaderStages &lhs, EShader rhs)  __NE___ { return (lhs |= EShaderStages(1 << uint(rhs))); }


} // AE::Graphics
