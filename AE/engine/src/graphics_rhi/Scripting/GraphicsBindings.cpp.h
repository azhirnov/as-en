// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_SCRIPTING
# include "pch/Scripting.h"

# include "graphics_rhi/Scripting/GraphicsBindings.h"
# include "graphics_rhi/Private/PixelFormatDefines.h"

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
		EnumBinder<EImage>	binder{ se };
		binder.Create();
		switch_enum( EImage::Unknown )
		{
			case EImage::Unknown :
			#define BIND( _name_ )	case EImage_ ## _name_ : binder.AddValue( AE_TOSTRING(_name_), EImage_ ## _name_ );
			BIND( 1D )
			BIND( 2D )
			BIND( 3D )
			BIND( 1DArray )
			BIND( 2DArray )
			BIND( Cube )
			BIND( CubeArray )
			#undef BIND
			default : break;
		}
		switch_end
	}

/*
=================================================
	Bind_EIndex
=================================================
*/
	static void  Bind_EIndex (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EIndex>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EPixelFormat
=================================================
*/
	static void  Bind_EPixelFormat (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EPixelFormat>	binder{ se };
		binder.Create();

		#define AE_PIXELFORMAT_VISIT( _name_ )	binder.AddValue( AE_TOSTRING(_name_), EPixelFormat::_name_ );
		AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT );
		#undef AE_PIXELFORMAT_VISIT

		binder.AddValue( "SwapchainColor", EPixelFormat::SwapchainColor );
	}

/*
=================================================
	Bind_EPixelFormatExternal
=================================================
*/
	static void  Bind_EPixelFormatExternal (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EPixelFormatExternal>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ECompareOp
=================================================
*/
	static void  Bind_ECompareOp (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ECompareOp>	binder{ se };
		binder.Create();
		binder.BindAll();

		// alias
		binder.AddValue( "LessOrEqual",		ECompareOp::LEqual );
		binder.AddValue( "GreaterOrEqual",	ECompareOp::GEqual );
	}

/*
=================================================
	Bind_EBlendFactor
=================================================
*/
	static void  Bind_EBlendFactor (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EBlendFactor>	binder{ se };
		binder.Create();
		binder.Comment( "S, srcColor - from shader\n"
						"D, dstColor - from render target\n"
						"S1 - from shader (dual src blend)\n"
						"cc - constant color\n"
						"result = srcColor * srcBlend [blendOp] dstColor * dstBlend\n\n");

		switch_enum( EBlendFactor::Unknown )
		{
			case EBlendFactor::Unknown :
			#define BIND( _name_, _comment_ )	case EBlendFactor::_name_ :  binder.Comment( _comment_ );  binder.AddValue( AE_TOSTRING(_name_), EBlendFactor::_name_ );
			BIND( Zero,					"0" )
			BIND( One,					"1" )
			BIND( SrcColor,				"S" )
			BIND( OneMinusSrcColor,		"1 - S" )
			BIND( DstColor,				"D" )
			BIND( OneMinusDstColor,		"1 - D" )
			BIND( SrcAlpha,				"S.a" )
			BIND( OneMinusSrcAlpha,		"1 - S.a" )
			BIND( DstAlpha,				"D.a" )
			BIND( OneMinusDstAlpha,		"1 - D.a" )
			BIND( ConstColor,			"cc" )
			BIND( OneMinusConstColor,	"1 - cc" )
			BIND( ConstAlpha,			"cc.a" )
			BIND( OneMinusConstAlpha,	"1 - cc.a" )
			BIND( SrcAlphaSaturate,		"rgb * min( S.a, D.a ), a * 1" )
			BIND( Src1Color,			"S1" )
			BIND( OneMinusSrc1Color,	"1 - S1" )
			BIND( Src1Alpha,			"S1.a" )
			BIND( OneMinusSrc1Alpha,	"1 - S1.a" )
			#undef BIND
			default : break;
		}
		switch_end
	}

/*
=================================================
	Bind_EBlendOp
=================================================
*/
	static void  Bind_EBlendOp (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EBlendOp>	binder{ se };
		binder.Create();
		binder.Comment( "S, srcColor - from shader\n"
						"D, dstColor - from render target\n"
						"result = srcColor * srcBlend [blendOp] dstColor * dstBlend\n\n");

		switch_enum( EBlendOp::Unknown )
		{
			case EBlendOp::Unknown :
			#define BIND( _name_, _comment_ )	case EBlendOp::_name_ :  binder.Comment( _comment_ );  binder.AddValue( AE_TOSTRING(_name_), EBlendOp::_name_ );
			BIND( Add,		"S + D" )
			BIND( Sub,		"S - D" )
			BIND( RevSub,	"D - S" )
			BIND( Min,		"min( S, D )" )
			BIND( Max,		"max( S, D )" )
			#undef BIND
			default : break;
		}
		switch_end
	}

/*
=================================================
	Bind_ELogicOp
=================================================
*/
	static void  Bind_ELogicOp (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ELogicOp>	binder{ se };
		binder.Create();
		binder.Comment( "S - from shader\n"
						"D - from render target\n"
						"result = S [logicOp] D\n\n");
		switch_enum( ELogicOp::Unknown )
		{
			case ELogicOp::Unknown :
			#define BIND( _name_, _comment_ )	case ELogicOp::_name_ :  binder.Comment( _comment_ );  binder.AddValue( AE_TOSTRING(_name_), ELogicOp::_name_ );
			BIND( None,			"disabled" )
			BIND( Clear,		"0" )
			BIND( Set,			"1" )
			BIND( Copy,			"S" )
			BIND( CopyInverted,	"~S" )
			BIND( NoOp,			"D" )
			BIND( Invert,		"~D" )
			BIND( And,			"S & D" )
			BIND( NotAnd,		"~ ( S & D )" )
			BIND( Or,			"S | D" )
			BIND( NotOr,		"~ ( S | D )" )
			BIND( Xor,			"S ^ D" )
			BIND( Equiv,		"~ ( S ^ D )" )
			BIND( AndReverse,	"S & ~D" )
			BIND( AndInverted,	"~S & D" )
			BIND( OrReverse,	"S | ~D" )
			BIND( OrInverted,	"~S | D" )
			#undef BIND
			default : break;
		}
		switch_end
	}

/*
=================================================
	Bind_EStencilOp
=================================================
*/
	static void  Bind_EStencilOp (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EStencilOp>	binder{ se };
		binder.Create();
		switch_enum( EStencilOp::Unknown )
		{
			case EStencilOp::Unknown :
			#define BIND( _name_, _comment_ )	case EStencilOp::_name_ : binder.Comment( _comment_ );  binder.AddValue( AE_TOSTRING(_name_), EStencilOp::_name_ );
			BIND( Keep,		"src" )
			BIND( Zero,		"0" )
			BIND( Replace,	"ref" )
			BIND( Incr,		"min( ++src, 0 )" )
			BIND( IncrWrap,	"++src & maxValue" )
			BIND( Decr,		"max( --src, 0 )" )
			BIND( DecrWrap,	"--src & maxValue" )
			BIND( Invert,	"~src" )
			#undef BIND
			default : break;
		}
		switch_end
	}

/*
=================================================
	Bind_EPolygonMode
=================================================
*/
	static void  Bind_EPolygonMode (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EPolygonMode>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EPrimitive
=================================================
*/
	static void  Bind_EPrimitive (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EPrimitive>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ECullMode
=================================================
*/
	static void  Bind_ECullMode (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ECullMode>	binder{ se };
		binder.Create();
		binder.BindAll();
		binder.AddValue< ECullMode::None >();
		binder.AddValue( "FontAndBack", ECullMode::FontAndBack );
	}

/*
=================================================
	Bind_EPipelineDynamicState
=================================================
*/
	static void  Bind_EPipelineDynamicState (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EPipelineDynamicState>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EImageAspect
=================================================
*/
	static void  Bind_EImageAspect (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EImageAspect>	binder{ se };
		binder.Create();
		binder.BindAll();
		binder.AddValue< EImageAspect::DepthStencil >();
	}

/*
=================================================
	Bind_EShaderIO
=================================================
*/
	static void  Bind_EShaderIO (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EShaderIO>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESubgroupTypes
=================================================
*/
	static void  Bind_ESubgroupTypes (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESubgroupTypes>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESubgroupOperation
=================================================
*/
	static void  Bind_ESubgroupOperation (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESubgroupOperation>		binder{ se };
		binder.Create();
		binder.BindAll();

		// ranges
		binder.AddValue( "_Basic_Begin",			ESubgroupOperation::_Basic_Begin );
		binder.AddValue( "_Basic_End",				ESubgroupOperation::_Basic_End );
		binder.AddValue( "_Vote_Begin",				ESubgroupOperation::_Vote_Begin );
		binder.AddValue( "_Vote_End",				ESubgroupOperation::_Vote_End );
		binder.AddValue( "_Arithmetic_Begin",		ESubgroupOperation::_Arithmetic_Begin );
		binder.AddValue( "_Arithmetic_End",			ESubgroupOperation::_Arithmetic_End );
		binder.AddValue( "_Ballot_Begin",			ESubgroupOperation::_Ballot_Begin );
		binder.AddValue( "_Ballot_End",				ESubgroupOperation::_Ballot_End );
		binder.AddValue( "_Shuffle_Begin",			ESubgroupOperation::_Shuffle_Begin );
		binder.AddValue( "_Shuffle_End",			ESubgroupOperation::_Shuffle_End );
		binder.AddValue( "_ShuffleRelative_Begin",	ESubgroupOperation::_ShuffleRelative_Begin );
		binder.AddValue( "_ShuffleRelative_End",	ESubgroupOperation::_ShuffleRelative_End );
		binder.AddValue( "_Clustered_Begin",		ESubgroupOperation::_Clustered_Begin );
		binder.AddValue( "_Clustered_End",			ESubgroupOperation::_Clustered_End );
		binder.AddValue( "_Quad_Begin",				ESubgroupOperation::_Quad_Begin );
		binder.AddValue( "_Quad_End",				ESubgroupOperation::_Quad_End );
	}

/*
=================================================
	Bind_EFeature
=================================================
*/
	static void  Bind_EFeature (const ScriptEnginePtr &se) __Th___
	{
		using EFeature = FeatureSet::EFeature;
		EnumBinder<EFeature>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EShader
=================================================
*/
	static void  Bind_EShader (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EShader>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EShaderStages
=================================================
*/
	static void  Bind_EShaderStages (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EShaderStages>		binder{ se };
		binder.Create();
		binder.BindAll();
		binder.AddValue< EShaderStages::GraphicsPipeStages >();
		binder.AddValue< EShaderStages::MeshPipeStages >();
		binder.AddValue< EShaderStages::VertexProcessingStages >();
		binder.AddValue< EShaderStages::PreRasterizationStages >();
		binder.AddValue< EShaderStages::PostRasterizationStages >();
		binder.AddValue< EShaderStages::AllGraphics >();
		binder.AddValue< EShaderStages::AllRayTracing >();
		binder.AddValue< EShaderStages::All >();
	}

/*
=================================================
	Bind_EVendorID
=================================================
*/
	static void  Bind_EVendorID (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EGPUVendor>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EGraphicsDeviceID
=================================================
*/
	static void  Bind_EGraphicsDeviceID (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EGraphicsDeviceID>		binder{ se };
		binder.Create();

		#define AE_GRAPHICS_DEVICE_VISIT( _name_ )		binder.AddValue( AE_TOSTRING(_name_), EGraphicsDeviceID::_name_ );
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
		EnumBinder<EVertexType>	binder{ se };
		binder.Create();

		#define AE_VERTEXTYPE_VISIT( _name_, _value_ )	binder.AddValue( AE_TOSTRING(_name_), EVertexType::_name_ );
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
		EnumBinder<EFilter>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EMipmapFilter
=================================================
*/
	static void  Bind_EMipmapFilter (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EMipmapFilter>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EAddressMode
=================================================
*/
	static void  Bind_EAddressMode (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EAddressMode>		binder{ se };
		binder.Create();
		binder.BindAll();

		// alias:
		binder.AddValue( "Clamp",		EAddressMode::ClampToEdge );
		binder.AddValue( "MirrorClamp",	EAddressMode::MirrorClampToEdge );
	}

/*
=================================================
	Bind_EBorderColor
=================================================
*/
	static void  Bind_EBorderColor (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EBorderColor>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EReductionMode
=================================================
*/
	static void  Bind_EReductionMode (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EReductionMode>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESamplerUsage
=================================================
*/
	static void  Bind_ESamplerUsage (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESamplerOpt>		binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EVertexInputRate
=================================================
*/
	static void  Bind_EVertexInputRate (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EVertexInputRate>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EDescSetUsage
=================================================
*/
	static void  Bind_EDescSetUsage (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EDescSetUsage>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EPipelineOpt
=================================================
*/
	static void  Bind_EPipelineOpt (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EPipelineOpt>	binder{ se };
		binder.Create();
		switch_enum( EPipelineOpt::Unknown )
		{
			case EPipelineOpt::Unknown :
			case EPipelineOpt::_Last :
			case EPipelineOpt::All :
			#define BIND( _name_ )				case EPipelineOpt::_name_ : binder.AddValue( AE_TOSTRING(_name_), EPipelineOpt::_name_ );
			#define BIND2( _name_, _comment_ )	case EPipelineOpt::_name_ : binder.Comment( _comment_ );  binder.AddValue( AE_TOSTRING(_name_), EPipelineOpt::_name_ );
			BIND2( Optimize, "Optimize pipeline during creation, may be slow." )
			BIND( CS_DispatchBase )
			BIND( RT_NoNullAnyHitShaders )
			BIND( RT_NoNullClosestHitShaders )
			BIND( RT_NoNullMissShaders )
			BIND( RT_NoNullIntersectionShaders )
			BIND( RT_SkipTriangles )
			BIND( RT_SkipAABBs )
			BIND( RT_AllowClusterAccelStruct )
			BIND( OpacityMicromap )
			BIND( DescriptorHeap )
			BIND2( DontCompile, "Pipeline creation will fail if it is not exists in cache." )
			BIND2( CaptureStatistics, "When a pipeline is created, its state and shaders are compiled into zero or more device-specific executables,\nwhich are used when executing commands against that pipeline." )
			BIND2( CaptureInternalRepresentation, "May include the final shader assembly, a binary form of the compiled shader,\nor the shader compiler’s internal representation at any number of intermediate compile steps." )
			BIND( IndirectBindable )
			#undef BIND
			#undef BIND2
			default :
				binder.Comment( "Disable pipeline optimization to speedup creation." );
				binder.AddValue( "DontOptimize",	EPipelineOpt::Unknown );
				binder.AddValue( "None",			EPipelineOpt::Unknown );
				break;
		}
		switch_end
	}

/*
=================================================
	Bind_EQueueMask
=================================================
*/
	static void  Bind_EQueueMask (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EQueueMask>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESamplerChromaLocation
=================================================
*/
	static void  Bind_ESamplerChromaLocation (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESamplerChromaLocation>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESamplerYcbcrModelConversion
=================================================
*/
	static void  Bind_ESamplerYcbcrModelConversion (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESamplerYcbcrModelConversion>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESamplerYcbcrRange
=================================================
*/
	static void  Bind_ESamplerYcbcrRange (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESamplerYcbcrRange>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ESurfaceFormat
=================================================
*/
	static void  Bind_ESurfaceFormat (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ESurfaceFormat>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ERTInstanceOpt
=================================================
*/
	static void  Bind_ERTInstanceOpt (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ERTInstanceOpt>	binder{ se };
		binder.Create();
		binder.BindAll();

		binder.AddValue( "TriangleCullBack",	ERTInstanceOpt::TriangleCullBack	);
		binder.AddValue( "TriangleFrontCW",		ERTInstanceOpt::TriangleFrontCW		);
	}

/*
=================================================
	Bind_EImageUsage
=================================================
*/
	static void  Bind_EImageUsage (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EImageUsage>	binder{ se };
		binder.Create();
		binder.BindAll();

		binder.AddValue< EImageUsage::All			>();
		binder.AddValue< EImageUsage::Transfer		>();
		binder.AddValue< EImageUsage::RWAttachment	>();
	}

/*
=================================================
	Bind_EImageOpt
=================================================
*/
	static void  Bind_EImageOpt (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EImageOpt>	binder{ se };
		binder.Create();
		binder.BindAll();

		binder.AddValue( "All",						EImageOpt::All						);
		binder.AddValue( "SparseResidencyAliased",	EImageOpt::SparseResidencyAliased	);
		binder.AddValue( "Blit",					EImageOpt::Blit						);
	}

/*
=================================================
	Bind_EBufferUsage
=================================================
*/
	static void  Bind_EBufferUsage (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EBufferUsage>	binder{ se };
		binder.Create();
		binder.BindAll();

		binder.AddValue( "All",				EBufferUsage::All				);
		binder.AddValue( "Transfer",		EBufferUsage::Transfer			);
		binder.AddValue( "MMBuild_Scratch",	EBufferUsage::MMBuild_Scratch	);
	}

/*
=================================================
	Bind_EBufferOpt
=================================================
*/
	static void  Bind_EBufferOpt (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EBufferOpt>	binder{ se };
		binder.Create();
		binder.BindAll();

		binder.AddValue( "All",						EBufferOpt::All						);
		binder.AddValue( "SparseResidencyAliased",	EBufferOpt::SparseResidencyAliased	);
	}

/*
=================================================
	Bind_EShadingRate
=================================================
*/
	static void  Bind_EShadingRate (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EShadingRate>	binder{ se };
		binder.Create();
		switch_enum( EShadingRate::_SizeCount )
		{
			case EShadingRate::_SizeCount :
			case EShadingRate::_SizeMask :
			case EShadingRate::_SamplesMask :
			#define BIND( _name_ )		case EShadingRate::_name_ : binder.AddValue( AE_TOSTRING(_name_), EShadingRate::_name_ );
			BIND( Size1x1 )
			BIND( Size1x2 )
			BIND( Size1x4 )
			BIND( Size2x1 )
			BIND( Size2x2 )
			BIND( Size2x4 )
			BIND( Size4x1 )
			BIND( Size4x2 )
			BIND( Size4x4 )
			#undef BIND
			case EShadingRate::Samples1 :
			case EShadingRate::Samples2 :
			case EShadingRate::Samples4 :
			case EShadingRate::Samples8 :
			case EShadingRate::Samples16 :
			case EShadingRate::Samples1_2_4 :
			case EShadingRate::Samples1_2_4_8 :	break;	// samples used only in feature set
		}
		switch_end
	}

/*
=================================================
	Bind_EShadingRateCombinerOp
=================================================
*/
	static void  Bind_EShadingRateCombinerOp (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EShadingRateCombinerOp>	binder{ se };
		binder.Create();
		binder.Comment( "S - original rate\nD - new rate" );
		switch_enum( EShadingRateCombinerOp::Unknown )
		{
			case EShadingRateCombinerOp::Unknown :
			#define BIND( _name_, _comment_ )		case EShadingRateCombinerOp::_name_ :	\
														binder.Comment( _comment_ );		\
														binder.AddValue( AE_TOSTRING(_name_), EShadingRateCombinerOp::_name_ );
			BIND( Keep,		"S" )
			BIND( Replace,	"D" )
			BIND( Min,		"min( S, D )" )
			BIND( Max,		"max( S, D )" )
			BIND( Sum,		"S + D  -- check 'fragmentShadingRateStrictMultiplyCombiner' feature" )
			BIND( Mul,		"S * D  -- check 'fragmentShadingRateStrictMultiplyCombiner' feature" )
			#undef BIND
		}
		switch_end
	}

/*
=================================================
	Bind_EIntegerDotProductFeat
=================================================
*/
	static void  Bind_EIntegerDotProductFeat (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EIntegerDotProductFeat>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ECoopMatrixComponentType
=================================================
*/
	static void  Bind_ECoopMatrixComponentType (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ECoopMatrixComponentType>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ECoopVecMatrixLayout
=================================================
*/
	static void  Bind_ECoopVecMatrixLayout (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ECoopVecMatrixLayout>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ECoopMatrixCfg
=================================================
*/
	static void  Bind_ECoopMatrixCfg (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ECoopMatrixCfg>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_ECoopVecCfg
=================================================
*/
	static void  Bind_ECoopVecCfg (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<ECoopVecCfg>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EConservativeRasterizationMode
=================================================
*/
	static void  Bind_EConservativeRasterizationMode (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EConservativeRasterizationMode>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EMicromapType
=================================================
*/
	static void  Bind_EMicromapType (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EMicromapType>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EOpacityMicromapFormat
=================================================
*/
	static void  Bind_EOpacityMicromapFormat (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EOpacityMicromapFormat>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EDescriptorFlags
=================================================
*/
	static void  Bind_EDescriptorFlags (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EDescriptorFlags>	binder{ se };
		binder.Create();
		binder.BindAll();
	}

/*
=================================================
	Bind_EResourceState
=================================================
*/
	static void  Bind_EResourceState (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<EResourceState>	binder{ se };
		binder.Create();
		switch_enum( EResourceState::_InvalidState )
		{
			case EResourceState::_InvalidState :
			#define BIND( _name_ )					case EResourceState::_name_ :  binder.AddValue( AE_TOSTRING(_name_), EResourceState::_name_ );
			BIND( Unknown )
			BIND( Preserve )
			BIND( ShaderStorage_Read )
			BIND( ShaderStorage_Write )
			BIND( ShaderStorage_RW )
			BIND( ShaderUniform )
			BIND( ShaderSample )
			BIND( CopySrc )
			BIND( CopyDst )
			BIND( ClearDst )
			BIND( BlitSrc )
			BIND( BlitDst )
			BIND( InputColorAttachment )
			BIND( InputColorAttachment_RW )
			BIND( ColorAttachment )
			BIND( ColorAttachment_Blend )
			BIND( DepthStencilAttachment_Read )
			BIND( DepthStencilAttachment_Write )
			BIND( DepthStencilAttachment_RW )
			BIND( DepthTest_StencilRW )
			BIND( DepthRW_StencilTest )
			BIND( DepthStencilTest_ShaderSample )
			BIND( DepthTest_DepthSample_StencilRW )
			BIND( InputDepthStencilAttachment )
			BIND( InputDepthStencilAttachment_RW )
			BIND( Host_Read )
			BIND( PresentImage )
			BIND( IndirectBuffer )
			BIND( IndexBuffer )
			BIND( VertexBuffer )
			BIND( ShadingRateImage )
			BIND( FragmentDensityMap )
			BIND( CopyRTAS_Read )
			BIND( CopyRTAS_Write )
			BIND( BuildRTAS_Read )
			BIND( BuildRTAS_Write )
			BIND( BuildRTAS_RW )
			BIND( BuildRTAS_IndirectBuffer )
			BIND( BuildRTAS_MicromapRead )
			BIND( BuildMicromap_Read )
			BIND( BuildMicromap_Write )
			BIND( ShaderRTAS )
			BIND( RTShaderBindingTable )
			BIND( DSTestBeforeFS )
			BIND( DSTestAfterFS )
			BIND( Invalidate )
			BIND( General )
			BIND( MeshTaskShader )
			BIND( VertexProcessingShaders )
			BIND( TileShader )
			BIND( FragmentShader )
			BIND( PreRasterizationShaders )
			BIND( PostRasterizationShaders )
			BIND( ComputeShader )
			BIND( RayTracingShaders )
			BIND( AllGraphicsShaders )
			BIND( AllShaderStages )
			BIND( CoopVecConvert_Read )
			BIND( CoopVecConvert_Write )
			BIND( ICB_Preprocess_Read )
			BIND( ICB_Preprocess_Write )
			BIND( VideoDecodeSrc )
			BIND( VideoDecodeDst )
			BIND( VideoDecodeDpb )
			BIND( VideoEncodeSrc )
			BIND( VideoEncodeDst )
			BIND( VideoEncodeDpb )
			BIND( AllStages )
			#undef BIND
			// helpers:
			default :
				binder.AddValue( "BuildRTAS_ScratchBuffer",	EResourceState::BuildRTAS_ScratchBuffer );
				binder.AddValue( "InputDepthAttachment",	EResourceState::InputDepthStencilAttachment );
				binder.AddValue( "DepthStencilAttachment",	EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS );
				break;
		}
		switch_end
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
		ClassBinder<MultiSamples>	binder{ se };
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
		ClassBinder<ImageLayer>	binder{ se };
		binder.CreateClassValue();
		binder.AddConstructor( &ImageLayer_Ctor, {} );
	}

/*
=================================================
	Bind_MipmapLevel
=================================================
*/
	static void  MipmapLevel_Ctor (void* mem, uint level) {
		PlacementNew<MipmapLevel>( OUT mem, ubyte(level) );
	}

	static void  Bind_MipmapLevel (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<MipmapLevel>	binder{ se };
		binder.CreateClassValue();
		binder.AddConstructor( &MipmapLevel_Ctor, {} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind_RenderState_ColorBuffer_ColorMask
=================================================
*/
	static void  RS_ColorBuffer_ColorMask_SetR (RenderState::ColorBuffer::ColorMask &cm, bool v)	{ cm.r = v; }
	static void  RS_ColorBuffer_ColorMask_SetG (RenderState::ColorBuffer::ColorMask &cm, bool v)	{ cm.g = v; }
	static void  RS_ColorBuffer_ColorMask_SetB (RenderState::ColorBuffer::ColorMask &cm, bool v)	{ cm.b = v; }
	static void  RS_ColorBuffer_ColorMask_SetA (RenderState::ColorBuffer::ColorMask &cm, bool v)	{ cm.a = v; }

	static void  RS_ColorBuffer_ColorMask_SetRGBA (RenderState::ColorBuffer::ColorMask &cm, bool v)	{ cm = RenderState::ColorBuffer::ColorMask{v}; }

	static void  Bind_RenderState_ColorBuffer_ColorMask (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::ColorBuffer::ColorMask>	binder{ se };
		binder.CreateClassValue();
		AS_METHOD( binder, RS_ColorBuffer_ColorMask_SetR,		"setR",		{} );
		AS_METHOD( binder, RS_ColorBuffer_ColorMask_SetG,		"setG",		{} );
		AS_METHOD( binder, RS_ColorBuffer_ColorMask_SetB,		"setB",		{} );
		AS_METHOD( binder, RS_ColorBuffer_ColorMask_SetA,		"setA",		{} );
		AS_METHOD( binder, RS_ColorBuffer_ColorMask_SetRGBA,	"setRGBA",	{} );
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
			self.srcBlendFactor.color	= value;
			self.srcBlendFactor.alpha	= value;
		}else
		if constexpr( Mode == 1 ){
			self.dstBlendFactor.color	= value;
			self.dstBlendFactor.alpha	= value;
		}else
		if constexpr( Mode == 2 ){
			self.blendOp.color			= value;
			self.blendOp.alpha			= value;
		}
	}

	template <uint Mode, typename T>
	static void  RenderState_ColorBuffer_Set2 (RenderState::ColorBuffer &self, T color, T alpha)
	{
		self.blend = true;
		if constexpr( Mode == 0 ){
			self.srcBlendFactor.color	= color;
			self.srcBlendFactor.alpha	= alpha;
		}else
		if constexpr( Mode == 1 ){
			self.dstBlendFactor.color	= color;
			self.dstBlendFactor.alpha	= alpha;
		}else
		if constexpr( Mode == 2 ){
			self.blendOp.color			= color;
			self.blendOp.alpha			= alpha;
		}
	}

/*
=================================================
	Bind_RenderState_ColorBuffer
=================================================
*/
	static void  Bind_RenderState_ColorBuffer (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::ColorBuffer>	binder{ se };
		binder.CreateClassValue();

		AS_METHOD( binder, (RenderState_ColorBuffer_Set1<0,EBlendFactor>),	"SrcBlendFactor",	{"colorAndAlpha"} );
		AS_METHOD( binder, (RenderState_ColorBuffer_Set2<0,EBlendFactor>),	"SrcBlendFactor",	{"color", "alpha"} );
		AS_METHOD( binder, (RenderState_ColorBuffer_Set1<1,EBlendFactor>),	"DstBlendFactor",	{"colorAndAlpha"} );
		AS_METHOD( binder, (RenderState_ColorBuffer_Set2<1,EBlendFactor>),	"DstBlendFactor",	{"color", "alpha"} );
		AS_METHOD( binder, (RenderState_ColorBuffer_Set1<2,EBlendOp>),		"BlendOp",			{"colorAndAlpha"} );
		AS_METHOD( binder, (RenderState_ColorBuffer_Set2<2,EBlendOp>),		"BlendOp",			{"color", "alpha"} );

	//	binder.AddProperty( &RenderState::ColorBuffer::srcBlendFactor,		"srcBlendFactor" );
	//	binder.AddProperty( &RenderState::ColorBuffer::dstBlendFactor,		"dstBlendFactor" );
	//	binder.AddProperty( &RenderState::ColorBuffer::blendOp,				"blendOp" );
		binder.AddProperty( &RenderState::ColorBuffer::blend,				"blend" );
		binder.AddProperty( &RenderState::ColorBuffer::colorMask,			"colorMask" );
	}

/*
=================================================
	Bind_RenderState_ColorBuffersState
=================================================
*/
	static void  RenderState_ColorBuffersState_SetColorBuffer (RenderState::ColorBuffersState &self, uint index, const RenderState::ColorBuffer &cb)
	{
		auto&	dst = self.buffers[index];
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
		ClassBinder<RenderState::ColorBuffersState>	binder{ se };
		binder.CreateClassValue();
		AS_METHOD( binder, RenderState_ColorBuffersState_SetColorBuffer,	"SetColorBuffer",	{"index", "cb"} );
		AS_METHOD( binder, RenderState_ColorBuffersState_GetColorBuffer,	"GetColorBuffer",	{"index"} );
		binder.AddProperty( &RenderState::ColorBuffersState::logicOp,		"logicOp" );
		binder.AddProperty( &RenderState::ColorBuffersState::blendColor,	"blendColor" );
	}

/*
=================================================
	Bind_RenderState_StencilFaceState
=================================================
*/
	static void  Bind_RenderState_StencilFaceState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::StencilFaceState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::StencilFaceState::stencilFailOp,	"stencilFailOp" );
		binder.AddProperty( &RenderState::StencilFaceState::depthFailOp,	"depthFailOp" );
		binder.AddProperty( &RenderState::StencilFaceState::passOp,			"passOp" );
		binder.AddProperty( &RenderState::StencilFaceState::compareOp,		"compareOp" );
		binder.AddProperty( &RenderState::StencilFaceState::reference,		"reference" );
		binder.AddProperty( &RenderState::StencilFaceState::writeMask,		"writeMask" );
		binder.AddProperty( &RenderState::StencilFaceState::compareMask,	"compareMask" );
	}

/*
=================================================
	Bind_RenderState_StencilBufferState
=================================================
*/
	static void  RenderState_StencilBufferState_StencilFailOp (RenderState::StencilBufferState &self, EStencilOp op)
	{
		self.enabled				= true;
		self.front.stencilFailOp	= op;
		self.back.stencilFailOp		= op;
	}

	static void  RenderState_StencilBufferState_DepthFailOp (RenderState::StencilBufferState &self, EStencilOp op)
	{
		self.enabled			= true;
		self.front.depthFailOp	= op;
		self.back.depthFailOp	= op;
	}

	static void  RenderState_StencilBufferState_PassOp (RenderState::StencilBufferState &self, EStencilOp op)
	{
		self.enabled		= true;
		self.front.passOp	= op;
		self.back.passOp	= op;
	}

	static void  RenderState_StencilBufferState_CompareOp (RenderState::StencilBufferState &self, ECompareOp op)
	{
		self.enabled			= true;
		self.front.compareOp	= op;
		self.back.compareOp		= op;
	}

	static void  RenderState_StencilBufferState_Reference (RenderState::StencilBufferState &self, uint value)
	{
		CHECK_THROW( value <= MaxValue<ubyte>(),
			"Stencil Reference (x"s << ToString<16>( value ) << ") must be <= than 0xFF" );

		self.enabled			= true;
		self.front.reference	= ubyte(value);
		self.back.reference		= ubyte(value);
	}

	static void  RenderState_StencilBufferState_WriteMask (RenderState::StencilBufferState &self, uint value)
	{
		CHECK_THROW( value <= MaxValue<ubyte>(),
			"Stencil WriteMask (x"s << ToString<16>( value ) << ") must be <= than 0xFF" );

		self.enabled			= true;
		self.front.writeMask	= ubyte(value);
		self.back.writeMask		= ubyte(value);
	}

	static void  RenderState_StencilBufferState_CompareMask (RenderState::StencilBufferState &self, uint value)
	{
		CHECK_THROW( value <= MaxValue<ubyte>(),
			"Stencil CompareMask (x"s << ToString<16>( value ) << ") must be <= than 0xFF" );

		self.enabled			= true;
		self.front.compareMask	= ubyte(value);
		self.back.compareMask	= ubyte(value);
	}

	static void  Bind_RenderState_StencilBufferState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::StencilBufferState>	binder{ se };
		binder.CreateClassValue();

		binder.AddProperty( &RenderState::StencilBufferState::front,		"front" );
		binder.AddProperty( &RenderState::StencilBufferState::back,			"back" );
		binder.AddProperty( &RenderState::StencilBufferState::enabled,		"enabled" );

		binder.Comment( "Stencil test compare operator.\n"
						"if '(stencilAttachment & CompareMask) [CompareOp] (Reference & CompareMask)' then sample passed stencil test." );
		AS_METHOD( binder, RenderState_StencilBufferState_CompareOp,		"CompareOp",		{} );
		AS_METHOD( binder, RenderState_StencilBufferState_Reference,		"Reference",		{} );
		AS_METHOD( binder, RenderState_StencilBufferState_CompareMask,		"CompareMask",		{} );

		binder.Comment( "Action performed on samples that fail the stencil test.\n"
						"'stencilValue = StencilFailOp( stencilAttachment )'\n"
						"See 'CompareOp', 'Reference' and 'CompareMask' to know how stencil test is performed." );
		AS_METHOD( binder, RenderState_StencilBufferState_StencilFailOp,	"StencilFailOp",	{} );

		binder.Comment( "Action performed on samples that pass the stencil test and fail the depth test.\n"
						"'stencilValue = DepthFailOp( stencilAttachment )'\n"
						"Depth test happens after stencil test and before stencil update." );
		AS_METHOD( binder, RenderState_StencilBufferState_DepthFailOp,		"DepthFailOp",		{} );

		binder.Comment( "Action performed on samples that pass both the depth and stencil tests.\n"
						"'stencilValue = PassOp( stencilAttachment )'" );
		AS_METHOD( binder, RenderState_StencilBufferState_PassOp,			"PassOp",			{} );

		binder.Comment( "Bitmask which is ANDed with new stencil value and stencil attachment value before updating stencil attachment.\n"
						"'stencilAttachment = (stencilAttachment & WriteMask) | (stencilValue & WriteMask)'" );
		AS_METHOD( binder, RenderState_StencilBufferState_WriteMask,		"WriteMask",		{} );
	}

/*
=================================================
	Bind_RenderState_DepthBufferState
=================================================
*/
	static void  Bind_RenderState_DepthBufferState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::DepthBufferState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::DepthBufferState::minBounds,	"minBounds" );
		binder.AddProperty( &RenderState::DepthBufferState::maxBounds,	"maxBounds" );
		binder.AddProperty( &RenderState::DepthBufferState::bounds,		"bounds" );
		binder.AddProperty( &RenderState::DepthBufferState::compareOp,	"compareOp" );
		binder.AddProperty( &RenderState::DepthBufferState::write,		"write" );
		binder.AddProperty( &RenderState::DepthBufferState::test,		"test" );
	}

/*
=================================================
	Bind_RenderState_InputAssemblyState
=================================================
*/
	static void  Bind_RenderState_InputAssemblyState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::InputAssemblyState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::InputAssemblyState::topology,			"topology" );
		binder.AddProperty( &RenderState::InputAssemblyState::primitiveRestart,	"primitiveRestart" );
	}

/*
=================================================
	Bind_RenderState_RasterizationState
=================================================
*/
	static void  Bind_RenderState_RasterizationState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::RasterizationState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::RasterizationState::depthBiasConstFactor,		"depthBiasConstFactor" );
		binder.AddProperty( &RenderState::RasterizationState::depthBiasClamp,			"depthBiasClamp" );
		binder.AddProperty( &RenderState::RasterizationState::depthBiasSlopeFactor,		"depthBiasSlopeFactor" );
		binder.AddProperty( &RenderState::RasterizationState::depthBias,				"depthBias" );
		binder.AddProperty( &RenderState::RasterizationState::polygonMode,				"polygonMode" );
		binder.AddProperty( &RenderState::RasterizationState::depthClamp,				"depthClamp" );
		binder.AddProperty( &RenderState::RasterizationState::rasterizerDiscard,		"rasterizerDiscard" );
		binder.AddProperty( &RenderState::RasterizationState::frontFaceCCW,				"frontFaceCCW" );
		binder.AddProperty( &RenderState::RasterizationState::cullMode,					"cullMode" );
		binder.AddProperty( &RenderState::RasterizationState::lineWidth,				"lineWidth" );
		binder.AddProperty( &RenderState::RasterizationState::conservativeRasterMode,	"conservativeRasterMode" );
	}

/*
=================================================
	Bind_RenderState_MultisampleState
=================================================
*/
	static void  Bind_RenderState_MultisampleState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::MultisamplingState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::MultisamplingState::sampleMask,		"sampleMask" );
		binder.AddProperty( &RenderState::MultisamplingState::minSampleShading,	"minSampleShading" );
		binder.AddProperty( &RenderState::MultisamplingState::samples,			"samples" );
		binder.AddProperty( &RenderState::MultisamplingState::sampleShading,	"sampleShading" );
		binder.AddProperty( &RenderState::MultisamplingState::alphaToCoverage,	"alphaToCoverage" );
		binder.AddProperty( &RenderState::MultisamplingState::alphaToOne,		"alphaToOne" );
	}

/*
=================================================
	Bind_RenderState_RasterizationOrderAccess
=================================================
*/
	static void  Bind_RenderState_RasterizationOrderAccess (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::RasterizationOrderAccess>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::RasterizationOrderAccess::color,		"color" );
		binder.AddProperty( &RenderState::RasterizationOrderAccess::depth,		"depth" );
		binder.AddProperty( &RenderState::RasterizationOrderAccess::stencil,	"stencil" );
	}

/*
=================================================
	Bind_RenderState
=================================================
*/
	static void  Bind_RenderState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::color,				"color" );
		binder.AddProperty( &RenderState::depth,				"depth" );
		binder.AddProperty( &RenderState::stencil,				"stencil" );
		binder.AddProperty( &RenderState::inputAssembly,		"inputAssembly" );
		binder.AddProperty( &RenderState::rasterization,		"rasterization" );
		binder.AddProperty( &RenderState::multisample,			"multisample" );
		binder.AddProperty( &RenderState::rasterOrderAccess,	"rasterOrderAccess" );
		StaticAssert( sizeof(RenderState) == 152 );
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
		Bind_EPixelFormatExternal( se );
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
		Bind_EImageUsage( se );
		Bind_EImageOpt( se );
		Bind_EBufferUsage( se );
		Bind_EBufferOpt( se );
		Bind_EShadingRate( se );
		Bind_EShadingRateCombinerOp( se );
		Bind_EIntegerDotProductFeat( se );
		Bind_ECoopMatrixComponentType( se );
		Bind_ECoopVecMatrixLayout( se );
		Bind_ECoopMatrixCfg( se );
		Bind_ECoopVecCfg( se );
		Bind_EConservativeRasterizationMode( se );
		Bind_EMicromapType( se );
		Bind_EOpacityMicromapFormat( se );
		Bind_EDescriptorFlags( se );
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
		Bind_RenderState_RasterizationOrderAccess( se );
		Bind_RenderState( se );
	}

/*
=================================================
	Bind_EDeviceValidation
=================================================
*/
	void  GraphicsBindings::Bind_EDeviceValidation (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		EnumBinder<EDeviceValidation>	binder{ se };
		binder.Create();

		switch_enum( EDeviceValidation::Disabled )
		{
			#define BIND( _name_ )		case EDeviceValidation::_name_ :	binder.AddValue( AE_TOSTRING(_name_), EDeviceValidation::_name_ );
			BIND( Disabled )
			BIND( Enabled )
			BIND( MinimalPreset )
			BIND( SynchronizationPreset )
			BIND( BestPracticesPreset )
			BIND( ShaderBasedPreset )
			BIND( ShaderPrintfPreset )
			#undef BIND
		}
		switch_end
	}


} // AE::Scripting

#endif // AE_ENABLE_SCRIPTING
