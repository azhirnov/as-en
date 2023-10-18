// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_SCRIPTING
# include "pch/Scripting.h"

# include "graphics/Scripting/GraphicsBindings.h"
# include "graphics/Private/PixelFormatDefines.h"

namespace AE::Scripting
{
namespace
{
    using namespace AE::Graphics;

/*
=================================================
    Bind_EImage
=================================================
*/
    static void  Bind_EImage (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EImage>  binder{ se };
        binder.Create();
        binder.AddValue( "1D",          EImage::_1D );
        binder.AddValue( "2D",          EImage::_2D );
        binder.AddValue( "3D",          EImage::_3D );
        binder.AddValue( "1DArray",     EImage::_1DArray );
        binder.AddValue( "2DArray",     EImage::_2DArray );
        binder.AddValue( "Cube",        EImage::Cube );
        binder.AddValue( "CubeArray",   EImage::CubeArray );
        STATIC_ASSERT( uint(EImage::_Count) == 7 );
    }

/*
=================================================
    Bind_EIndex
=================================================
*/
    static void  Bind_EIndex (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EIndex>  binder{ se };
        binder.Create();
        binder.AddValue( "UShort",      EIndex::UShort );
        binder.AddValue( "UInt",        EIndex::UInt );
        STATIC_ASSERT( uint(EIndex::_Count) == 2 );
    }

/*
=================================================
    Bind_EPixelFormat
=================================================
*/
    static void  Bind_EPixelFormat (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EPixelFormat>    binder{ se };
        binder.Create();

        #define AE_PIXELFORMAT_VISIT( _name_ )  binder.AddValue( AE_TOSTRING( _name_ ), EPixelFormat::_name_ );
        AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT );
        #undef AE_PIXELFORMAT_VISIT

        binder.AddValue( "SwapchainColor", EPixelFormat::SwapchainColor );
    }

/*
=================================================
    Bind_EAttachmentLoadOp
=================================================
*/
    static void  Bind_EAttachmentLoadOp (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EAttachmentLoadOp>   binder{ se };
        binder.Create();
        binder.AddValue( "Invalidate",  EAttachmentLoadOp::Invalidate );
        binder.AddValue( "Load",        EAttachmentLoadOp::Load );
        binder.AddValue( "Clear",       EAttachmentLoadOp::Clear );
        binder.AddValue( "None",        EAttachmentLoadOp::None );
        STATIC_ASSERT( uint(EAttachmentLoadOp::_Count) == 4 );
    }

/*
=================================================
    Bind_EAttachmentStoreOp
=================================================
*/
    static void  Bind_EAttachmentStoreOp (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EAttachmentStoreOp>  binder{ se };
        binder.Create();
        binder.AddValue( "Invalidate",  EAttachmentStoreOp::Invalidate );
        binder.AddValue( "Store",       EAttachmentStoreOp::Store );
        binder.AddValue( "None",        EAttachmentStoreOp::None );
        STATIC_ASSERT( uint(EAttachmentStoreOp::_Count) == 4 );
    }

/*
=================================================
    Bind_ECompareOp
=================================================
*/
    static void  Bind_ECompareOp (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ECompareOp>  binder{ se };
        binder.Create();
        binder.AddValue( "Never",       ECompareOp::Never );
        binder.AddValue( "Less",        ECompareOp::Less );
        binder.AddValue( "Equal",       ECompareOp::Equal );
        binder.AddValue( "LEqual",      ECompareOp::LEqual );
        binder.AddValue( "Greater",     ECompareOp::Greater );
        binder.AddValue( "NotEqual",    ECompareOp::NotEqual );
        binder.AddValue( "GEqual",      ECompareOp::GEqual );
        binder.AddValue( "Always",      ECompareOp::Always );
        STATIC_ASSERT( uint(ECompareOp::_Count) == 8 );

        // alias
        binder.AddValue( "LessOrEqual",     ECompareOp::LEqual );
        binder.AddValue( "GreaterOrEqual",  ECompareOp::GEqual );
    }

/*
=================================================
    Bind_EBlendFactor
=================================================
*/
    static void  Bind_EBlendFactor (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EBlendFactor>    binder{ se };
        binder.Create();
        binder.AddValue( "Zero",                EBlendFactor::Zero );
        binder.AddValue( "One",                 EBlendFactor::One );
        binder.AddValue( "SrcColor",            EBlendFactor::SrcColor );
        binder.AddValue( "OneMinusSrcColor",    EBlendFactor::OneMinusSrcColor );
        binder.AddValue( "DstColor",            EBlendFactor::DstColor );
        binder.AddValue( "OneMinusDstColor",    EBlendFactor::OneMinusDstColor );
        binder.AddValue( "SrcAlpha",            EBlendFactor::SrcAlpha );
        binder.AddValue( "OneMinusSrcAlpha",    EBlendFactor::OneMinusSrcAlpha );
        binder.AddValue( "DstAlpha",            EBlendFactor::DstAlpha );
        binder.AddValue( "OneMinusDstAlpha",    EBlendFactor::OneMinusDstAlpha );
        binder.AddValue( "ConstColor",          EBlendFactor::ConstColor );
        binder.AddValue( "OneMinusConstColor",  EBlendFactor::OneMinusConstColor );
        binder.AddValue( "ConstAlpha",          EBlendFactor::ConstAlpha );
        binder.AddValue( "OneMinusConstAlpha",  EBlendFactor::OneMinusConstAlpha );
        binder.AddValue( "SrcAlphaSaturate",    EBlendFactor::SrcAlphaSaturate );
        binder.AddValue( "Src1Color",           EBlendFactor::Src1Color );
        binder.AddValue( "OneMinusSrc1Color",   EBlendFactor::OneMinusSrc1Color );
        binder.AddValue( "Src1Alpha",           EBlendFactor::Src1Alpha );
        binder.AddValue( "OneMinusSrc1Alpha",   EBlendFactor::OneMinusSrc1Alpha );
        STATIC_ASSERT( uint(EBlendFactor::_Count) == 19 );
    }

/*
=================================================
    Bind_EBlendOp
=================================================
*/
    static void  Bind_EBlendOp (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EBlendOp>    binder{ se };
        binder.Create();
        binder.AddValue( "Add",     EBlendOp::Add );
        binder.AddValue( "Sub",     EBlendOp::Sub );
        binder.AddValue( "RevSub",  EBlendOp::RevSub );
        binder.AddValue( "Min",     EBlendOp::Min );
        binder.AddValue( "Max",     EBlendOp::Max );
        STATIC_ASSERT( uint(EBlendOp::_Count) == 5 );
    }

/*
=================================================
    Bind_ELogicOp
=================================================
*/
    static void  Bind_ELogicOp (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ELogicOp>    binder{ se };
        binder.Create();
        binder.AddValue( "None",        ELogicOp::None );
        binder.AddValue( "Clear",       ELogicOp::Clear );
        binder.AddValue( "Set",         ELogicOp::Set );
        binder.AddValue( "Copy",        ELogicOp::Copy );
        binder.AddValue( "CopyInverted",ELogicOp::CopyInverted );
        binder.AddValue( "NoOp",        ELogicOp::NoOp );
        binder.AddValue( "Invert",      ELogicOp::Invert );
        binder.AddValue( "And",         ELogicOp::And );
        binder.AddValue( "NotAnd",      ELogicOp::NotAnd );
        binder.AddValue( "Or",          ELogicOp::Or );
        binder.AddValue( "NotOr",       ELogicOp::NotOr );
        binder.AddValue( "Xor",         ELogicOp::Xor );
        binder.AddValue( "Equiv",       ELogicOp::Equiv );
        binder.AddValue( "AndReverse",  ELogicOp::AndReverse );
        binder.AddValue( "AndInverted", ELogicOp::AndInverted );
        binder.AddValue( "OrReverse",   ELogicOp::OrReverse );
        binder.AddValue( "OrInverted",  ELogicOp::OrInverted );
        STATIC_ASSERT( uint(ELogicOp::_Count) == 17 );
    }

/*
=================================================
    Bind_EStencilOp
=================================================
*/
    static void  Bind_EStencilOp (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EStencilOp>  binder{ se };
        binder.Create();
        binder.AddValue( "Keep",        EStencilOp::Keep );
        binder.AddValue( "Zero",        EStencilOp::Zero );
        binder.AddValue( "Replace",     EStencilOp::Replace );
        binder.AddValue( "Incr",        EStencilOp::Incr );
        binder.AddValue( "IncrWrap",    EStencilOp::IncrWrap );
        binder.AddValue( "Decr",        EStencilOp::Decr );
        binder.AddValue( "DecrWrap",    EStencilOp::DecrWrap );
        binder.AddValue( "Invert",      EStencilOp::Invert );
        STATIC_ASSERT( uint(EStencilOp::_Count) == 8 );
    }

/*
=================================================
    Bind_EPolygonMode
=================================================
*/
    static void  Bind_EPolygonMode (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EPolygonMode>    binder{ se };
        binder.Create();
        binder.AddValue( "Point",   EPolygonMode::Point );
        binder.AddValue( "Line",    EPolygonMode::Line );
        binder.AddValue( "Fill",    EPolygonMode::Fill );
        STATIC_ASSERT( uint(EPolygonMode::_Count) == 3 );
    }

/*
=================================================
    Bind_EPrimitive
=================================================
*/
    static void  Bind_EPrimitive (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EPrimitive>  binder{ se };
        binder.Create();
        binder.AddValue( "Point",                   EPrimitive::Point );
        binder.AddValue( "LineList",                EPrimitive::LineList );
        binder.AddValue( "LineStrip",               EPrimitive::LineStrip );
        binder.AddValue( "LineListAdjacency",       EPrimitive::LineListAdjacency );
        binder.AddValue( "LineStripAdjacency",      EPrimitive::LineStripAdjacency );
        binder.AddValue( "TriangleList",            EPrimitive::TriangleList );
        binder.AddValue( "TriangleStrip",           EPrimitive::TriangleStrip );
        binder.AddValue( "TriangleFan",             EPrimitive::TriangleFan );
        binder.AddValue( "TriangleListAdjacency",   EPrimitive::TriangleListAdjacency );
        binder.AddValue( "TriangleStripAdjacency",  EPrimitive::TriangleStripAdjacency );
        binder.AddValue( "Patch",                   EPrimitive::Patch );
        STATIC_ASSERT( uint(EPrimitive::_Count) == 11 );
    }

/*
=================================================
    Bind_ECullMode
=================================================
*/
    static void  Bind_ECullMode (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ECullMode>   binder{ se };
        binder.Create();
        binder.AddValue( "None",        ECullMode::None );
        binder.AddValue( "Front",       ECullMode::Front );
        binder.AddValue( "Back",        ECullMode::Back );
        binder.AddValue( "FontAndBack", ECullMode::FontAndBack );
        STATIC_ASSERT( uint(ECullMode::_Last) == 3 );
    }

/*
=================================================
    Bind_EPipelineDynamicState
=================================================
*/
    static void  Bind_EPipelineDynamicState (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EPipelineDynamicState>   binder{ se };
        binder.Create();
        binder.AddValue( "None",                EPipelineDynamicState::Unknown );
        binder.AddValue( "StencilCompareMask",  EPipelineDynamicState::StencilCompareMask );
        binder.AddValue( "StencilWriteMask",    EPipelineDynamicState::StencilWriteMask );
        binder.AddValue( "StencilReference",    EPipelineDynamicState::StencilReference );
        binder.AddValue( "DepthBias",           EPipelineDynamicState::DepthBias );
        binder.AddValue( "BlendConstants",      EPipelineDynamicState::BlendConstants );
        //binder.AddValue( "DepthBounds",       EPipelineDynamicState::DepthBounds );
        binder.AddValue( "RTStackSize",         EPipelineDynamicState::RTStackSize );
        binder.AddValue( "FragmentShadingRate", EPipelineDynamicState::FragmentShadingRate );
        STATIC_ASSERT( uint(EPipelineDynamicState::_Last) == 129 );
    }

/*
=================================================
    Bind_EImageAspect
=================================================
*/
    static void  Bind_EImageAspect (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EImageAspect>    binder{ se };
        binder.Create();
        binder.AddValue( "Color",       EImageAspect::Color );
        binder.AddValue( "Depth",       EImageAspect::Depth );
        binder.AddValue( "Stencil",     EImageAspect::Stencil );
        //binder.AddValue( "Metadata",  EImageAspect::Metadata );
        binder.AddValue( "Plane_0",     EImageAspect::Plane_0 );
        binder.AddValue( "Plane_1",     EImageAspect::Plane_1 );
        binder.AddValue( "Plane_2",     EImageAspect::Plane_2 );
        STATIC_ASSERT( uint(EImageAspect::_Last) == 65 );
    }

/*
=================================================
    Bind_EShaderIO
=================================================
*/
    static void  Bind_EShaderIO (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EShaderIO>       binder{ se };
        binder.Create();
        binder.AddValue( "Int",         EShaderIO::Int );
        binder.AddValue( "UInt",        EShaderIO::UInt );
        binder.AddValue( "Float",       EShaderIO::Float );
        binder.AddValue( "UFloat",      EShaderIO::UFloat );
        binder.AddValue( "Half",        EShaderIO::Half );
        binder.AddValue( "UNorm",       EShaderIO::UNorm );
        binder.AddValue( "SNorm",       EShaderIO::SNorm );
        binder.AddValue( "sRGB",        EShaderIO::sRGB );
        binder.AddValue( "AnyColor",    EShaderIO::AnyColor );
        binder.AddValue( "Depth",       EShaderIO::Depth );
        binder.AddValue( "Stencil",     EShaderIO::Stencil );
        binder.AddValue( "DepthStencil",EShaderIO::DepthStencil );
        STATIC_ASSERT( uint(EShaderIO::_Count) == 13 );
    }

/*
=================================================
    Bind_ESubgroupTypes
=================================================
*/
    static void  Bind_ESubgroupTypes (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESubgroupTypes>      binder{ se };
        binder.Create();
        binder.AddValue( "Float16", ESubgroupTypes::Float16 );
        binder.AddValue( "Float32", ESubgroupTypes::Float32 );
        binder.AddValue( "Int8",    ESubgroupTypes::Int8 );
        binder.AddValue( "Int16",   ESubgroupTypes::Int16 );
        binder.AddValue( "Int32",   ESubgroupTypes::Int32 );
        binder.AddValue( "Int64",   ESubgroupTypes::Int64 );
        STATIC_ASSERT( uint(ESubgroupTypes::All) == 63 );
    }

/*
=================================================
    Bind_ESubgroupOperation
=================================================
*/
    static void  Bind_ESubgroupOperation (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESubgroupOperation>      binder{ se };
        binder.Create();
        // Basic
        binder.AddValue( "IndexAndSize",            ESubgroupOperation::IndexAndSize );
        binder.AddValue( "Elect",                   ESubgroupOperation::Elect );
        binder.AddValue( "Barrier",                 ESubgroupOperation::Barrier );
        // Vote
        binder.AddValue( "Any",                     ESubgroupOperation::Any );
        binder.AddValue( "All",                     ESubgroupOperation::All );
        binder.AddValue( "AllEqual",                ESubgroupOperation::AllEqual );
        // Arithmetic
        binder.AddValue( "Add",                     ESubgroupOperation::Add );
        binder.AddValue( "Mul",                     ESubgroupOperation::Mul );
        binder.AddValue( "Min",                     ESubgroupOperation::Min );
        binder.AddValue( "Max",                     ESubgroupOperation::Max );
        binder.AddValue( "And",                     ESubgroupOperation::And );
        binder.AddValue( "Or",                      ESubgroupOperation::Or );
        binder.AddValue( "Xor",                     ESubgroupOperation::Xor );
        binder.AddValue( "InclusiveMul",            ESubgroupOperation::InclusiveMul );
        binder.AddValue( "InclusiveAdd",            ESubgroupOperation::InclusiveAdd );
        binder.AddValue( "InclusiveMin",            ESubgroupOperation::InclusiveMin );
        binder.AddValue( "InclusiveMax",            ESubgroupOperation::InclusiveMax );
        binder.AddValue( "InclusiveAnd",            ESubgroupOperation::InclusiveAnd );
        binder.AddValue( "InclusiveOr",             ESubgroupOperation::InclusiveOr );
        binder.AddValue( "InclusiveXor",            ESubgroupOperation::InclusiveXor );
        binder.AddValue( "ExclusiveAdd",            ESubgroupOperation::ExclusiveAdd );
        binder.AddValue( "ExclusiveMul",            ESubgroupOperation::ExclusiveMul );
        binder.AddValue( "ExclusiveMin",            ESubgroupOperation::ExclusiveMin );
        binder.AddValue( "ExclusiveMax",            ESubgroupOperation::ExclusiveMax );
        binder.AddValue( "ExclusiveAnd",            ESubgroupOperation::ExclusiveAnd );
        binder.AddValue( "ExclusiveOr",             ESubgroupOperation::ExclusiveOr );
        binder.AddValue( "ExclusiveXor",            ESubgroupOperation::ExclusiveXor );
        // Ballot
        binder.AddValue( "Ballot",                  ESubgroupOperation::Ballot );
        binder.AddValue( "Broadcast",               ESubgroupOperation::Broadcast );
        binder.AddValue( "BroadcastFirst",          ESubgroupOperation::BroadcastFirst );
        binder.AddValue( "InverseBallot",           ESubgroupOperation::InverseBallot );
        binder.AddValue( "BallotBitExtract",        ESubgroupOperation::BallotBitExtract );
        binder.AddValue( "BallotBitCount",          ESubgroupOperation::BallotBitCount );
        binder.AddValue( "BallotInclusiveBitCount", ESubgroupOperation::BallotInclusiveBitCount );
        binder.AddValue( "BallotExclusiveBitCount", ESubgroupOperation::BallotExclusiveBitCount );
        binder.AddValue( "BallotFindLSB",           ESubgroupOperation::BallotFindLSB );
        binder.AddValue( "BallotFindMSB",           ESubgroupOperation::BallotFindMSB );
        // Shuffle
        binder.AddValue( "Shuffle",                 ESubgroupOperation::Shuffle );
        binder.AddValue( "ShuffleXor",              ESubgroupOperation::ShuffleXor );
        // Shuffle Relative
        binder.AddValue( "ShuffleUp",               ESubgroupOperation::ShuffleUp );
        binder.AddValue( "ShuffleDown",             ESubgroupOperation::ShuffleDown );
        // Clustered
        binder.AddValue( "ClusteredAdd",            ESubgroupOperation::ClusteredAdd );
        binder.AddValue( "ClusteredMul",            ESubgroupOperation::ClusteredMul );
        binder.AddValue( "ClusteredMin",            ESubgroupOperation::ClusteredMin );
        binder.AddValue( "ClusteredMax",            ESubgroupOperation::ClusteredMax );
        binder.AddValue( "ClusteredAnd",            ESubgroupOperation::ClusteredAnd );
        binder.AddValue( "ClusteredOr",             ESubgroupOperation::ClusteredOr );
        binder.AddValue( "ClusteredXor",            ESubgroupOperation::ClusteredXor );
        // Quad
        binder.AddValue( "QuadBroadcast",           ESubgroupOperation::QuadBroadcast );
        binder.AddValue( "QuadSwapHorizontal",      ESubgroupOperation::QuadSwapHorizontal );
        binder.AddValue( "QuadSwapVertical",        ESubgroupOperation::QuadSwapVertical );
        binder.AddValue( "QuadSwapDiagonal",        ESubgroupOperation::QuadSwapDiagonal );
        // ranges
        binder.AddValue( "_Basic_Begin",            ESubgroupOperation::_Basic_Begin );
        binder.AddValue( "_Basic_End",              ESubgroupOperation::_Basic_End );
        binder.AddValue( "_Vote_Begin",             ESubgroupOperation::_Vote_Begin );
        binder.AddValue( "_Vote_End",               ESubgroupOperation::_Vote_End );
        binder.AddValue( "_Arithmetic_Begin",       ESubgroupOperation::_Arithmetic_Begin );
        binder.AddValue( "_Arithmetic_End",         ESubgroupOperation::_Arithmetic_End );
        binder.AddValue( "_Ballot_Begin",           ESubgroupOperation::_Ballot_Begin );
        binder.AddValue( "_Ballot_End",             ESubgroupOperation::_Ballot_End );
        binder.AddValue( "_Shuffle_Begin",          ESubgroupOperation::_Shuffle_Begin );
        binder.AddValue( "_Shuffle_End",            ESubgroupOperation::_Shuffle_End );
        binder.AddValue( "_ShuffleRelative_Begin",  ESubgroupOperation::_ShuffleRelative_Begin );
        binder.AddValue( "_ShuffleRelative_End",    ESubgroupOperation::_ShuffleRelative_End );
        binder.AddValue( "_Clustered_Begin",        ESubgroupOperation::_Clustered_Begin );
        binder.AddValue( "_Clustered_End",          ESubgroupOperation::_Clustered_End );
        binder.AddValue( "_Quad_Begin",             ESubgroupOperation::_Quad_Begin );
        binder.AddValue( "_Quad_End",               ESubgroupOperation::_Quad_End );
        STATIC_ASSERT( uint(ESubgroupOperation::_Count) == 52 );
    }

/*
=================================================
    Bind_EFeature
=================================================
*/
    static void  Bind_EFeature (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EFeature>        binder{ se };
        binder.Create();
        binder.AddValue( "Ignore",          EFeature::Ignore );
        binder.AddValue( "RequireTrue",     EFeature::RequireTrue );
        binder.AddValue( "RequireFalse",    EFeature::RequireFalse );
        STATIC_ASSERT( uint(EFeature::_Count) == 3 );
    }

/*
=================================================
    Bind_EShader
=================================================
*/
    static void  Bind_EShader (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EShader>     binder{ se };
        binder.Create();
        binder.AddValue( "Vertex",          EShader::Vertex );
        binder.AddValue( "TessControl",     EShader::TessControl );
        binder.AddValue( "TessEvaluation",  EShader::TessEvaluation );
        binder.AddValue( "Geometry",        EShader::Geometry );
        binder.AddValue( "Fragment",        EShader::Fragment );
        binder.AddValue( "Compute",         EShader::Compute );
        binder.AddValue( "Tile",            EShader::Tile );
        binder.AddValue( "MeshTask",        EShader::MeshTask );
        binder.AddValue( "Mesh",            EShader::Mesh );
        binder.AddValue( "RayGen",          EShader::RayGen );
        binder.AddValue( "RayAnyHit",       EShader::RayAnyHit );
        binder.AddValue( "RayClosestHit",   EShader::RayClosestHit );
        binder.AddValue( "RayMiss",         EShader::RayMiss );
        binder.AddValue( "RayIntersection", EShader::RayIntersection );
        binder.AddValue( "RayCallable",     EShader::RayCallable );
        STATIC_ASSERT( uint(EShader::_Count) == 15 );
    }

/*
=================================================
    Bind_EShaderStages
=================================================
*/
    static void  Bind_EShaderStages (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EShaderStages>       binder{ se };
        binder.Create();
        binder.AddValue( "Vertex",          EShaderStages::Vertex );
        binder.AddValue( "TessControl",     EShaderStages::TessControl );
        binder.AddValue( "TessEvaluation",  EShaderStages::TessEvaluation );
        binder.AddValue( "Geometry",        EShaderStages::Geometry );
        binder.AddValue( "Fragment",        EShaderStages::Fragment );
        binder.AddValue( "Compute",         EShaderStages::Compute );
        binder.AddValue( "Tile",            EShaderStages::Tile );
        binder.AddValue( "MeshTask",        EShaderStages::MeshTask );
        binder.AddValue( "Mesh",            EShaderStages::Mesh );
        binder.AddValue( "RayGen",          EShaderStages::RayGen );
        binder.AddValue( "RayAnyHit",       EShaderStages::RayAnyHit );
        binder.AddValue( "RayClosestHit",   EShaderStages::RayClosestHit );
        binder.AddValue( "RayMiss",         EShaderStages::RayMiss );
        binder.AddValue( "RayIntersection", EShaderStages::RayIntersection );
        binder.AddValue( "RayCallable",     EShaderStages::RayCallable );

        binder.AddValue( "All",                     EShaderStages::All );
        binder.AddValue( "AllGraphics",             EShaderStages::AllGraphics );
        binder.AddValue( "AllRayTracing",           EShaderStages::AllRayTracing );
        binder.AddValue( "GraphicsStages",          EShaderStages::GraphicsStages );
        binder.AddValue( "MeshStages",              EShaderStages::MeshStages );
        binder.AddValue( "VertexProcessingStages",  EShaderStages::VertexProcessingStages );
        binder.AddValue( "PreRasterizationStages",  EShaderStages::PreRasterizationStages );
        binder.AddValue( "PostRasterizationStages", EShaderStages::PostRasterizationStages );
        STATIC_ASSERT( uint(EShaderStages::All) == 0x7FFF );
    }

/*
=================================================
    Bind_EVendorID
=================================================
*/
    static void  Bind_EVendorID (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EVendorID>       binder{ se };
        binder.Create();
        binder.AddValue( "AMD",         EVendorID::AMD );
        binder.AddValue( "NVidia",      EVendorID::NVidia );
        binder.AddValue( "Intel",       EVendorID::Intel );
        binder.AddValue( "ARM",         EVendorID::ARM );
        binder.AddValue( "Qualcomm",    EVendorID::Qualcomm );
        binder.AddValue( "ImgTech",     EVendorID::ImgTech );
        binder.AddValue( "Microsoft",   EVendorID::Microsoft );
        binder.AddValue( "Apple",       EVendorID::Apple );
        binder.AddValue( "Mesa",        EVendorID::Mesa );
        binder.AddValue( "Broadcom",    EVendorID::Broadcom );
        binder.AddValue( "Samsung",     EVendorID::Samsung );
        binder.AddValue( "VeriSilicon", EVendorID::VeriSilicon );
        STATIC_ASSERT( uint(EVendorID::_Count) == 12 );
    }

/*
=================================================
    Bind_EGraphicsDeviceID
=================================================
*/
    static void  Bind_EGraphicsDeviceID (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EGraphicsDeviceID>       binder{ se };
        binder.Create();

        #define AE_GRAPHICS_DEVICE_VISIT( _name_ )      binder.AddValue( AE_TOSTRING( _name_ ), EGraphicsDeviceID::_name_ );
        AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT );
        #undef AE_GRAPHICS_DEVICE_VISIT
    }

/*
=================================================
    Bind_EVertexType
=================================================
*/
    static void  Bind_EVertexType (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EVertexType> binder{ se };
        binder.Create();

        #define AE_VERTEXTYPE_VISIT( _name_, _value_ )  binder.AddValue( AE_TOSTRING( _name_ ), EVertexType::_name_ );
        AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT );
        #undef AE_VERTEXTYPE_VISIT
    }

/*
=================================================
    Bind_EFilter
=================================================
*/
    static void  Bind_EFilter (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EFilter>     binder{ se };
        binder.Create();
        binder.AddValue( "Nearest",     EFilter::Nearest );
        binder.AddValue( "Linear",      EFilter::Linear );
        STATIC_ASSERT( uint(EFilter::_Count) == 2 );
    }

/*
=================================================
    Bind_EMipmapFilter
=================================================
*/
    static void  Bind_EMipmapFilter (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EMipmapFilter>       binder{ se };
        binder.Create();
        binder.AddValue( "None",        EMipmapFilter::None );
        binder.AddValue( "Nearest",     EMipmapFilter::Nearest );
        binder.AddValue( "Linear",      EMipmapFilter::Linear );
        STATIC_ASSERT( uint(EMipmapFilter::_Count) == 3 );
    }

/*
=================================================
    Bind_EAddressMode
=================================================
*/
    static void  Bind_EAddressMode (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EAddressMode>        binder{ se };
        binder.Create();
        binder.AddValue( "Repeat",              EAddressMode::Repeat );
        binder.AddValue( "MirrorRepeat",        EAddressMode::MirrorRepeat );
        binder.AddValue( "ClampToEdge",         EAddressMode::ClampToEdge );
        binder.AddValue( "ClampToBorder",       EAddressMode::ClampToBorder );
        binder.AddValue( "MirrorClampToEdge",   EAddressMode::MirrorClampToEdge );
        // alias:
        binder.AddValue( "Clamp",               EAddressMode::ClampToEdge );
        binder.AddValue( "MirrorClamp",         EAddressMode::MirrorClampToEdge );
        STATIC_ASSERT( uint(EAddressMode::_Count) == 5 );
    }

/*
=================================================
    Bind_EBorderColor
=================================================
*/
    static void  Bind_EBorderColor (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EBorderColor>        binder{ se };
        binder.Create();
        binder.AddValue( "FloatTransparentBlack",   EBorderColor::FloatTransparentBlack );
        binder.AddValue( "FloatOpaqueBlack",        EBorderColor::FloatOpaqueBlack );
        binder.AddValue( "FloatOpaqueWhite",        EBorderColor::FloatOpaqueWhite );
        binder.AddValue( "IntTransparentBlack",     EBorderColor::IntTransparentBlack );
        binder.AddValue( "IntOpaqueBlack",          EBorderColor::IntOpaqueBlack );
        binder.AddValue( "IntOpaqueWhite",          EBorderColor::IntOpaqueWhite );
        STATIC_ASSERT( uint(EBorderColor::_Count) == 6 );
    }

/*
=================================================
    Bind_EReductionMode
=================================================
*/
    static void  Bind_EReductionMode (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EReductionMode>      binder{ se };
        binder.Create();
        binder.AddValue( "Average", EReductionMode::Average );
        binder.AddValue( "Min",     EReductionMode::Min );
        binder.AddValue( "Max",     EReductionMode::Max );
        STATIC_ASSERT( uint(EReductionMode::_Count) == 3 );
    }

/*
=================================================
    Bind_ESamplerUsage
=================================================
*/
    static void  Bind_ESamplerUsage (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESamplerUsage>       binder{ se };
        binder.Create();
        binder.AddValue( "Default",                         ESamplerUsage::Default );
        binder.AddValue( "Subsampled",                      ESamplerUsage::Subsampled );
        binder.AddValue( "SubsampledCoarseReconstruction",  ESamplerUsage::SubsampledCoarseReconstruction );
        STATIC_ASSERT( uint(ESamplerUsage::_Count) == 3 );
    }

/*
=================================================
    Bind_EVertexInputRate
=================================================
*/
    static void  Bind_EVertexInputRate (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EVertexInputRate>    binder{ se };
        binder.Create();
        binder.AddValue( "Vertex",      EVertexInputRate::Vertex );
        binder.AddValue( "Instance",    EVertexInputRate::Instance );
    }

/*
=================================================
    Bind_EDescSetUsage
=================================================
*/
    static void  Bind_EDescSetUsage (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EDescSetUsage>   binder{ se };
        binder.Create();
        binder.AddValue( "AllowPartialyUpdate", EDescSetUsage::AllowPartialyUpdate );
        binder.AddValue( "UpdateTemplate",      EDescSetUsage::UpdateTemplate );
        binder.AddValue( "ArgumentBuffer",      EDescSetUsage::ArgumentBuffer );
        binder.AddValue( "MutableArgBuffer",    EDescSetUsage::MutableArgBuffer );
        STATIC_ASSERT( uint(EDescSetUsage::All) == 0xF );
    }

/*
=================================================
    Bind_EPipelineOpt
=================================================
*/
    static void  Bind_EPipelineOpt (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EPipelineOpt>    binder{ se };
        binder.Create();
        binder.AddValue( "Optimize",                        EPipelineOpt::Optimize                      );
        binder.AddValue( "CS_DispatchBase",                 EPipelineOpt::CS_DispatchBase               );
        binder.AddValue( "RT_NoNullAnyHitShaders",          EPipelineOpt::RT_NoNullAnyHitShaders        );
        binder.AddValue( "RT_NoNullClosestHitShaders",      EPipelineOpt::RT_NoNullClosestHitShaders    );
        binder.AddValue( "RT_NoNullMissShaders",            EPipelineOpt::RT_NoNullMissShaders          );
        binder.AddValue( "RT_NoNullIntersectionShaders",    EPipelineOpt::RT_NoNullIntersectionShaders  );
        binder.AddValue( "RT_SkipTriangles",                EPipelineOpt::RT_SkipTriangles              );
        binder.AddValue( "RT_SkipAABBs",                    EPipelineOpt::RT_SkipAABBs                  );
        STATIC_ASSERT( uint(EPipelineOpt::All) == 0xFF );
    }

/*
=================================================
    Bind_EQueueMask
=================================================
*/
    static void  Bind_EQueueMask (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EQueueMask>  binder{ se };
        binder.Create();
        binder.AddValue( "Graphics",        EQueueMask::Graphics        );
        binder.AddValue( "AsyncCompute",    EQueueMask::AsyncCompute    );
        binder.AddValue( "AsyncTransfer",   EQueueMask::AsyncTransfer   );
        binder.AddValue( "VideoEncode",     EQueueMask::VideoEncode     );
        binder.AddValue( "VideoDecode",     EQueueMask::VideoDecode     );
        STATIC_ASSERT( uint(EQueueMask::All) == 31 );
    }

/*
=================================================
    Bind_ESamplerChromaLocation
=================================================
*/
    static void  Bind_ESamplerChromaLocation (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESamplerChromaLocation>  binder{ se };
        binder.Create();
        binder.AddValue( "CositedEven",     ESamplerChromaLocation::CositedEven     );
        binder.AddValue( "Midpoint",        ESamplerChromaLocation::Midpoint        );
        STATIC_ASSERT( uint(ESamplerChromaLocation::_Count) == 2 );
    }

/*
=================================================
    Bind_ESamplerYcbcrModelConversion
=================================================
*/
    static void  Bind_ESamplerYcbcrModelConversion (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESamplerYcbcrModelConversion>    binder{ se };
        binder.Create();
        binder.AddValue( "RGB_Identity",    ESamplerYcbcrModelConversion::RGB_Identity  );
        binder.AddValue( "Ycbcr_Identity",  ESamplerYcbcrModelConversion::Ycbcr_Identity);
        binder.AddValue( "Ycbcr_709",       ESamplerYcbcrModelConversion::Ycbcr_709     );
        binder.AddValue( "Ycbcr_601",       ESamplerYcbcrModelConversion::Ycbcr_601     );
        binder.AddValue( "Ycbcr_2020",      ESamplerYcbcrModelConversion::Ycbcr_2020    );
        STATIC_ASSERT( uint(ESamplerYcbcrModelConversion::_Count) == 5 );
    }

/*
=================================================
    Bind_ESamplerYcbcrRange
=================================================
*/
    static void  Bind_ESamplerYcbcrRange (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESamplerYcbcrRange>  binder{ se };
        binder.Create();
        binder.AddValue( "ITU_Full",        ESamplerYcbcrRange::ITU_Full    );
        binder.AddValue( "ITU_Narrow",      ESamplerYcbcrRange::ITU_Narrow  );
        STATIC_ASSERT( uint(ESamplerYcbcrRange::_Count) == 2 );
    }

/*
=================================================
    Bind_ESurfaceFormat
=================================================
*/
    static void  Bind_ESurfaceFormat (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ESurfaceFormat>  binder{ se };
        binder.Create();
        binder.AddValue( "BGRA8_sRGB_nonlinear",            ESurfaceFormat::BGRA8_sRGB_nonlinear            );
        binder.AddValue( "RGBA8_sRGB_nonlinear",            ESurfaceFormat::RGBA8_sRGB_nonlinear            );
        binder.AddValue( "BGRA8_BT709_nonlinear",           ESurfaceFormat::BGRA8_BT709_nonlinear           );
        binder.AddValue( "RGBA16F_Extended_sRGB_linear",    ESurfaceFormat::RGBA16F_Extended_sRGB_linear    );
        binder.AddValue( "RGBA16F_sRGB_nonlinear",          ESurfaceFormat::RGBA16F_sRGB_nonlinear          );
        binder.AddValue( "RGBA16F_BT709_nonlinear",         ESurfaceFormat::RGBA16F_BT709_nonlinear         );
        binder.AddValue( "RGBA16F_HDR10_ST2084",            ESurfaceFormat::RGBA16F_HDR10_ST2084            );
        binder.AddValue( "RGBA16F_BT2020_linear",           ESurfaceFormat::RGBA16F_BT2020_linear           );
        binder.AddValue( "RGB10A2_sRGB_nonlinear",          ESurfaceFormat::RGB10A2_sRGB_nonlinear          );
        binder.AddValue( "RGB10A2_HDR10_ST2084",            ESurfaceFormat::RGB10A2_HDR10_ST2084            );
        STATIC_ASSERT( uint(ESurfaceFormat::_Count) == 10 );
    }

/*
=================================================
    Bind
=================================================
*/
    static void  Bind_ERTInstanceOpt (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<ERTInstanceOpt>  binder{ se };
        binder.Create();
        binder.AddValue( "TriangleCullDisable", ERTInstanceOpt::TriangleCullDisable );
        binder.AddValue( "TriangleFrontCCW",    ERTInstanceOpt::TriangleFrontCCW    );
        binder.AddValue( "ForceOpaque",         ERTInstanceOpt::ForceOpaque         );
        binder.AddValue( "ForceNonOpaque",      ERTInstanceOpt::ForceNonOpaque      );
        binder.AddValue( "TriangleCullBack",    ERTInstanceOpt::TriangleCullBack    );
        binder.AddValue( "TriangleFrontCW",     ERTInstanceOpt::TriangleFrontCW     );
        STATIC_ASSERT( uint(ERTInstanceOpt::All) == 15 );
    }

/*
=================================================
    Bind_EResourceState
=================================================
*/
    static void  Bind_EResourceState (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<EResourceState>  binder{ se };
        binder.Create();

        BEGIN_ENUM_CHECKS();
        switch ( EResourceState(0) )
        {
        #define BIND( _name_ )\
            case EResourceState::_name_ :  binder.AddValue( AE_TOSTRING(_name_), EResourceState::_name_ );

            case EResourceState::_InvalidState :    break;
            BIND( Unknown );
            BIND( Preserve );
            BIND( ShaderStorage_Read );
            BIND( ShaderStorage_Write );
            BIND( ShaderStorage_RW );
            BIND( ShaderUniform );
            BIND( ShaderSample );
            BIND( CopySrc );
            BIND( CopyDst );
            BIND( ClearDst );
            BIND( BlitSrc );
            BIND( BlitDst );
            BIND( InputColorAttachment );
            BIND( InputColorAttachment_RW );
            BIND( ColorAttachment );
            BIND( ColorAttachment_Blend );
            BIND( DepthStencilAttachment_Read );
            BIND( DepthStencilAttachment_Write );
            BIND( DepthStencilAttachment_RW );
            BIND( DepthTest_StencilRW );
            BIND( DepthRW_StencilTest );
            BIND( DepthStencilTest_ShaderSample );
            BIND( DepthTest_DepthSample_StencilRW );
            BIND( InputDepthStencilAttachment );
            BIND( InputDepthStencilAttachment_RW );
            BIND( Host_Read );
            BIND( Host_Write );
            BIND( PresentImage );
            BIND( IndirectBuffer );
            BIND( IndexBuffer );
            BIND( VertexBuffer );
            BIND( ShadingRateImage );
            BIND( CopyRTAS_Read );
            BIND( CopyRTAS_Write );
            BIND( BuildRTAS_Read );
            BIND( BuildRTAS_Write );
            BIND( BuildRTAS_RW );
            BIND( BuildRTAS_IndirectBuffer );
            BIND( ShaderRTAS );
            BIND( RTShaderBindingTable );
            BIND( DSTestBeforeFS );
            BIND( DSTestAfterFS );
            BIND( Invalidate );
            BIND( General );
            BIND( MeshTaskShader );
            BIND( VertexProcessingShaders );
            BIND( TileShader );
            BIND( FragmentShader );
            BIND( PreRasterizationShaders );
            BIND( PostRasterizationShaders );
            BIND( ComputeShader );
            BIND( RayTracingShaders );
            BIND( AllGraphicsShaders );
            BIND( AllShaders );
            // helpers:
            default :   binder.AddValue( "BuildRTAS_ScratchBuffer", EResourceState::BuildRTAS_ScratchBuffer );
                        binder.AddValue( "InputDepthAttachment",    EResourceState::InputDepthStencilAttachment );
                        binder.AddValue( "DepthStencilAttachment",  EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS );
                        break;
        #undef BIND
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    Bind_MultiSamples
=================================================
*/
    static void  MultiSamples_Ctor (void* mem, uint samples) {
        CHECK( IsPowerOfTwo( samples ));
        PlacementNew<MultiSamples>( OUT mem, samples );
    }

    static void  Bind_MultiSamples (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<MultiSamples>   binder{ se };
        binder.CreateClassValue();
        binder.AddConstructor( &MultiSamples_Ctor, {} );
    }

/*
=================================================
    Bind_ImageLayer
=================================================
*/
    static void  ImageLayer_Ctor (void* mem, uint layers) {
        PlacementNew<ImageLayer>( OUT mem, layers );
    }

    static void  Bind_ImageLayer (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ImageLayer> binder{ se };
        binder.CreateClassValue();
        binder.AddConstructor( &ImageLayer_Ctor, {} );
    }

/*
=================================================
    Bind_MipmapLevel
=================================================
*/
    static void  MipmapLevel_Ctor (void* mem, uint level) {
        PlacementNew<MipmapLevel>( OUT mem, ushort(level) );
    }

    static void  Bind_MipmapLevel (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<MipmapLevel>    binder{ se };
        binder.CreateClassValue();
        binder.AddConstructor( &MipmapLevel_Ctor, {} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind_RenderState_ColorBuffer_ColorPair
=================================================
*
    template <typename T>
    static void  RenderState_ColorBuffer_ColorPair_Set1 (RenderState::ColorBuffer::ColorPair<T> &self, T value) {
        self.color  = value;
        self.alpha  = value;
    }

    template <typename T>
    static void  RenderState_ColorBuffer_ColorPair_Set2 (RenderState::ColorBuffer::ColorPair<T> &self, T color, T alpha) {
        self.color  = color;
        self.alpha  = alpha;
    }

    template <typename T>
    static void  Bind_RenderState_ColorBuffer_ColorPair (const ScriptEnginePtr &se) __Th___
    {
        using ColorPair = RenderState::ColorBuffer::ColorPair<T>;

        ClassBinder<ColorPair>  binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &ColorPair::color, "color" );
        binder.AddProperty( &ColorPair::alpha, "alpha" );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_ColorPair_Set1<T>, "set" );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_ColorPair_Set2<T>, "set" );
    }

/*
=================================================
    Bind_RenderState_ColorBuffer_ColorMask
=================================================
*/
    static void  RS_ColorBuffer_ColorMask_SetR (RenderState::ColorBuffer::ColorMask &cm, bool v)    { cm.r = v; }
    static void  RS_ColorBuffer_ColorMask_SetG (RenderState::ColorBuffer::ColorMask &cm, bool v)    { cm.g = v; }
    static void  RS_ColorBuffer_ColorMask_SetB (RenderState::ColorBuffer::ColorMask &cm, bool v)    { cm.b = v; }
    static void  RS_ColorBuffer_ColorMask_SetA (RenderState::ColorBuffer::ColorMask &cm, bool v)    { cm.a = v; }

    static void  RS_ColorBuffer_ColorMask_SetRGBA (RenderState::ColorBuffer::ColorMask &cm, bool v) { cm = RenderState::ColorBuffer::ColorMask{v}; }

    static void  Bind_RenderState_ColorBuffer_ColorMask (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::ColorBuffer::ColorMask>    binder{ se };
        binder.CreateClassValue();
        binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetR,     "setR",     {} );
        binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetG,     "setG",     {} );
        binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetB,     "setB",     {} );
        binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetA,     "setA",     {} );
        binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetRGBA,  "setRGBA",  {} );
    }

/*
=================================================
    RenderState_ColorBuffer_ColorPair_Set
=================================================
*/
    template <uint Mode, typename T>
    static void  RenderState_ColorBuffer_Set1 (RenderState::ColorBuffer &self, T value)
    {
        self.blend = true;
        if constexpr( Mode == 0 ){
            self.srcBlendFactor.color   = value;
            self.srcBlendFactor.alpha   = value;
        }else
        if constexpr( Mode == 1 ){
            self.dstBlendFactor.color   = value;
            self.dstBlendFactor.alpha   = value;
        }else
        if constexpr( Mode == 2 ){
            self.blendOp.color          = value;
            self.blendOp.alpha          = value;
        }
    }

    template <uint Mode, typename T>
    static void  RenderState_ColorBuffer_Set2 (RenderState::ColorBuffer &self, T color, T alpha)
    {
        self.blend = true;
        if constexpr( Mode == 0 ){
            self.srcBlendFactor.color   = color;
            self.srcBlendFactor.alpha   = alpha;
        }else
        if constexpr( Mode == 1 ){
            self.dstBlendFactor.color   = color;
            self.dstBlendFactor.alpha   = alpha;
        }else
        if constexpr( Mode == 2 ){
            self.blendOp.color          = color;
            self.blendOp.alpha          = alpha;
        }
    }

/*
=================================================
    Bind_RenderState_ColorBuffer
=================================================
*/
    static void  Bind_RenderState_ColorBuffer (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::ColorBuffer>   binder{ se };
        binder.CreateClassValue();

        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set1<0,EBlendFactor>,  "SrcBlendFactor",   {"colorAndAlpha"} );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set2<0,EBlendFactor>,  "SrcBlendFactor",   {"color", "alpha"} );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set1<1,EBlendFactor>,  "DstBlendFactor",   {"colorAndAlpha"} );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set2<1,EBlendFactor>,  "DstBlendFactor",   {"color", "alpha"} );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set1<2,EBlendOp>,      "BlendOp",          {"colorAndAlpha"} );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set2<2,EBlendOp>,      "BlendOp",          {"color", "alpha"} );

    //  binder.AddProperty( &RenderState::ColorBuffer::srcBlendFactor,      "srcBlendFactor" );
    //  binder.AddProperty( &RenderState::ColorBuffer::dstBlendFactor,      "dstBlendFactor" );
    //  binder.AddProperty( &RenderState::ColorBuffer::blendOp,             "blendOp" );
        binder.AddProperty( &RenderState::ColorBuffer::blend,               "blend" );
        binder.AddProperty( &RenderState::ColorBuffer::colorMask,           "colorMask" );
    }

/*
=================================================
    Bind_RenderState_ColorBuffersState
=================================================
*/
    static void  RenderState_ColorBuffersState_SetColorBuffer (RenderState::ColorBuffersState &self, uint index, const RenderState::ColorBuffer &cb)
    {
        auto&   dst = self.buffers[index];
        ASSERT( dst == Default );

        if ( not cb.blend and cb.colorMask.All() )
        {
            DBG_WARNING( "set default color buffer state, it is redundant call" );
            dst = Default;
        }else{
            dst = cb;
        }
    }

    static RenderState::ColorBuffer const*  RenderState_ColorBuffersState_GetColorBuffer (const RenderState::ColorBuffersState &self, uint index) {
        return &self.buffers[index];
    }

    static void  Bind_RenderState_ColorBuffersState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::ColorBuffersState> binder{ se };
        binder.CreateClassValue();
        binder.AddMethodFromGlobal( &RenderState_ColorBuffersState_SetColorBuffer,  "SetColorBuffer",   {"index", "cb"} );
        binder.AddMethodFromGlobal( &RenderState_ColorBuffersState_GetColorBuffer,  "GetColorBuffer",   {"index"} );
        binder.AddProperty( &RenderState::ColorBuffersState::logicOp,               "logicOp" );
        binder.AddProperty( &RenderState::ColorBuffersState::blendColor,            "blendColor" );
    }

/*
=================================================
    Bind_RenderState_StencilFaceState
=================================================
*/
    static void  Bind_RenderState_StencilFaceState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::StencilFaceState>  binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &RenderState::StencilFaceState::failOp,         "failOp" );
        binder.AddProperty( &RenderState::StencilFaceState::depthFailOp,    "depthFailOp" );
        binder.AddProperty( &RenderState::StencilFaceState::passOp,         "passOp" );
        binder.AddProperty( &RenderState::StencilFaceState::compareOp,      "compareOp" );
        binder.AddProperty( &RenderState::StencilFaceState::reference,      "reference" );
        binder.AddProperty( &RenderState::StencilFaceState::writeMask,      "writeMask" );
        binder.AddProperty( &RenderState::StencilFaceState::compareMask,    "compareMask" );
    }

/*
=================================================
    Bind_RenderState_StencilBufferState
=================================================
*/
    static void  RenderState_StencilBufferState_FailOp (RenderState::StencilBufferState &self, EStencilOp op)
    {
        self.enabled        = true;
        self.front.failOp   = op;
        self.back.failOp    = op;
    }

    static void  RenderState_StencilBufferState_DepthFailOp (RenderState::StencilBufferState &self, EStencilOp op)
    {
        self.enabled            = true;
        self.front.depthFailOp  = op;
        self.back.depthFailOp   = op;
    }

    static void  RenderState_StencilBufferState_PassOp (RenderState::StencilBufferState &self, EStencilOp op)
    {
        self.enabled        = true;
        self.front.passOp   = op;
        self.back.passOp    = op;
    }

    static void  RenderState_StencilBufferState_CompareOp (RenderState::StencilBufferState &self, ECompareOp op)
    {
        self.enabled            = true;
        self.front.compareOp    = op;
        self.back.compareOp     = op;
    }

    static void  RenderState_StencilBufferState_Reference (RenderState::StencilBufferState &self, ubyte value)
    {
        self.enabled            = true;
        self.front.reference    = value;
        self.back.reference     = value;
    }

    static void  RenderState_StencilBufferState_WriteMask (RenderState::StencilBufferState &self, ubyte value)
    {
        self.enabled            = true;
        self.front.writeMask    = value;
        self.back.writeMask     = value;
    }

    static void  RenderState_StencilBufferState_CompareMask (RenderState::StencilBufferState &self, ubyte value)
    {
        self.enabled            = true;
        self.front.compareMask  = value;
        self.back.compareMask   = value;
    }

    static void  Bind_RenderState_StencilBufferState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::StencilBufferState>    binder{ se };
        binder.CreateClassValue();

        binder.AddProperty( &RenderState::StencilBufferState::front,    "front" );
        binder.AddProperty( &RenderState::StencilBufferState::back,     "back" );
        binder.AddProperty( &RenderState::StencilBufferState::enabled,  "enabled" );

        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_FailOp,         "FailOp",       {} );
        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_DepthFailOp,    "DepthFailOp",  {} );
        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_PassOp,         "PassOp",       {} );
        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_CompareOp,      "CompareOp",    {} );
        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_Reference,      "Reference",    {} );
        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_WriteMask,      "WriteMask",    {} );
        binder.AddMethodFromGlobal( &RenderState_StencilBufferState_CompareMask,    "CompareMask",  {} );
    }

/*
=================================================
    Bind_RenderState_DepthBufferState
=================================================
*/
    static void  Bind_RenderState_DepthBufferState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::DepthBufferState>  binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &RenderState::DepthBufferState::minBounds,  "minBounds" );
        binder.AddProperty( &RenderState::DepthBufferState::maxBounds,  "maxBounds" );
        binder.AddProperty( &RenderState::DepthBufferState::bounds,     "bounds" );
        binder.AddProperty( &RenderState::DepthBufferState::compareOp,  "compareOp" );
        binder.AddProperty( &RenderState::DepthBufferState::write,      "write" );
        binder.AddProperty( &RenderState::DepthBufferState::test,       "test" );
    }

/*
=================================================
    Bind_RenderState_InputAssemblyState
=================================================
*/
    static void  Bind_RenderState_InputAssemblyState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::InputAssemblyState>    binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &RenderState::InputAssemblyState::topology,         "topology" );
        binder.AddProperty( &RenderState::InputAssemblyState::primitiveRestart, "primitiveRestart" );
    }

/*
=================================================
    Bind_RenderState_RasterizationState
=================================================
*/
    static void  Bind_RenderState_RasterizationState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::RasterizationState>    binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &RenderState::RasterizationState::depthBiasConstFactor, "depthBiasConstFactor" );
        binder.AddProperty( &RenderState::RasterizationState::depthBiasClamp,       "depthBiasClamp" );
        binder.AddProperty( &RenderState::RasterizationState::depthBiasSlopeFactor, "depthBiasSlopeFactor" );
        binder.AddProperty( &RenderState::RasterizationState::depthBias,            "depthBias" );
        binder.AddProperty( &RenderState::RasterizationState::polygonMode,          "polygonMode" );
        binder.AddProperty( &RenderState::RasterizationState::depthClamp,           "depthClamp" );
        binder.AddProperty( &RenderState::RasterizationState::rasterizerDiscard,    "rasterizerDiscard" );
        binder.AddProperty( &RenderState::RasterizationState::frontFaceCCW,         "frontFaceCCW" );
        binder.AddProperty( &RenderState::RasterizationState::cullMode,             "cullMode" );
    }

/*
=================================================
    Bind_RenderState_MultisampleState
=================================================
*/
    static void  Bind_RenderState_MultisampleState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState::MultisampleState>  binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &RenderState::MultisampleState::sampleMask,         "sampleMask" );
        binder.AddProperty( &RenderState::MultisampleState::minSampleShading,   "minSampleShading" );
        binder.AddProperty( &RenderState::MultisampleState::samples,            "samples" );
        binder.AddProperty( &RenderState::MultisampleState::sampleShading,      "sampleShading" );
        binder.AddProperty( &RenderState::MultisampleState::alphaToCoverage,    "alphaToCoverage" );
        binder.AddProperty( &RenderState::MultisampleState::alphaToOne,         "alphaToOne" );
    }

/*
=================================================
    Bind_RenderState
=================================================
*/
    static void  Bind_RenderState (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<RenderState>    binder{ se };
        binder.CreateClassValue();
        binder.AddProperty( &RenderState::color,            "color" );
        binder.AddProperty( &RenderState::depth,            "depth" );
        binder.AddProperty( &RenderState::stencil,          "stencil" );
        binder.AddProperty( &RenderState::inputAssembly,    "inputAssembly" );
        binder.AddProperty( &RenderState::rasterization,    "rasterization" );
        binder.AddProperty( &RenderState::multisample,      "multisample" );
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    BindEnums
=================================================
*/
    void  GraphicsBindings::BindEnums (const ScriptEnginePtr &se) __Th___
    {
        CHECK_THROW( se and se->IsInitialized() );

        Bind_EImage( se );
        Bind_EIndex( se );
        Bind_EPixelFormat( se );
        Bind_EAttachmentLoadOp( se );
        Bind_EAttachmentStoreOp( se );
        Bind_ECompareOp( se );
        Bind_EBlendFactor( se );
        Bind_EBlendOp( se );
        Bind_ELogicOp( se );
        Bind_EStencilOp( se );
        Bind_EPolygonMode( se );
        Bind_EPrimitive( se );
        Bind_ECullMode( se );
        Bind_EPipelineDynamicState( se );
        Bind_EResourceState( se );
        Bind_EImageAspect( se );
        Bind_EShaderIO( se );
        Bind_ESubgroupTypes( se );
        Bind_ESubgroupOperation( se );
        Bind_EFeature( se );
        Bind_EShader( se );
        Bind_EShaderStages( se );
        Bind_EVendorID( se );
        Bind_EVertexType( se );
        Bind_EGraphicsDeviceID( se );
        Bind_EFilter( se );
        Bind_EMipmapFilter( se );
        Bind_EAddressMode( se );
        Bind_EBorderColor( se );
        Bind_EReductionMode( se );
        Bind_ESamplerUsage( se );
        Bind_EVertexInputRate( se );
        Bind_EDescSetUsage( se );
        Bind_EPipelineOpt( se );
        Bind_EQueueMask( se );
        Bind_ESamplerChromaLocation( se );
        Bind_ESamplerYcbcrModelConversion( se );
        Bind_ESamplerYcbcrRange( se );
        Bind_ESurfaceFormat( se );
        Bind_ERTInstanceOpt( se );
    }

/*
=================================================
    BindTypes
=================================================
*/
    void  GraphicsBindings::BindTypes (const ScriptEnginePtr &se) __Th___
    {
        CHECK_THROW( se and se->IsInitialized() );

        Bind_MultiSamples( se );
        Bind_ImageLayer( se );
        Bind_MipmapLevel( se );
    }

/*
=================================================
    BindRenderState
=================================================
*/
    void  GraphicsBindings::BindRenderState (const ScriptEnginePtr &se) __Th___
    {
        CHECK_THROW( se and se->IsInitialized() );

        //Bind_RenderState_ColorBuffer_ColorPair<EBlendFactor>( se );
        //Bind_RenderState_ColorBuffer_ColorPair<EBlendOp>( se );
        Bind_RenderState_ColorBuffer_ColorMask( se );
        Bind_RenderState_ColorBuffer( se );
        Bind_RenderState_ColorBuffersState( se );
        Bind_RenderState_StencilFaceState( se );
        Bind_RenderState_StencilBufferState( se );
        Bind_RenderState_DepthBufferState( se );
        Bind_RenderState_InputAssemblyState( se );
        Bind_RenderState_RasterizationState( se );
        Bind_RenderState_MultisampleState( se );
        Bind_RenderState( se );
    }


} // AE::Scripting

#endif // AE_ENABLE_SCRIPTING
