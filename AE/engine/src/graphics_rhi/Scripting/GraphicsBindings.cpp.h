// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		switch_enum( EIndex::Unknown )
		{
			case EIndex::Unknown :
			#define BIND( _name_ )	case EIndex::_name_ : binder.AddValue( AE_TOSTRING(_name_), EIndex::_name_ );
			BIND( UShort )
			BIND( UInt )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EPixelFormatExternal::Unknown )
		{
			case EPixelFormatExternal::Unknown :
			case EPixelFormatExternal::Android_Private :
			case EPixelFormatExternal::Android_RawPrivate :
			case EPixelFormatExternal::_Android_End :
			#define BIND( _name_ )	case EPixelFormatExternal::_name_ : binder.AddValue( AE_TOSTRING(_name_), EPixelFormatExternal::_name_ );
			BIND( Android_Depth16 )
			BIND( Android_DepthJPEG )
			BIND( Android_DepthPointCloud )
			BIND( Android_JPEG )
			BIND( Android_Raw16 )
			BIND( Android_Raw12 )
			BIND( Android_Raw10 )
			BIND( Android_NV16 )
			BIND( Android_NV21 )
			BIND( Android_YCBCR_P010 )
			BIND( Android_YUV_420 )
			BIND( Android_YUV_422 )
			BIND( Android_YUV_444 )
			BIND( Android_YUY2 )
			BIND( Android_YV12 )
			BIND( Android_Y8 )
			BIND( Android_HEIC )
			#undef BIND
			default : break;
		}
		switch_end

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
		switch_enum( ECompareOp::Unknown )
		{
			case ECompareOp::Unknown :
			#define BIND( _name_ )	case ECompareOp::_name_ : binder.AddValue( AE_TOSTRING(_name_), ECompareOp::_name_ );
			BIND( Never )
			BIND( Less )
			BIND( Equal )
			BIND( LEqual )
			BIND( Greater )
			BIND( NotEqual )
			BIND( GEqual )
			BIND( Always )
			#undef BIND
			default :
				// alias
				binder.AddValue( "LessOrEqual",		ECompareOp::LEqual );
				binder.AddValue( "GreaterOrEqual",	ECompareOp::GEqual );
				break;
		}
		switch_end
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
		switch_enum( EPolygonMode::Unknown )
		{
			case EPolygonMode::Unknown :
			#define BIND( _name_ )	case EPolygonMode::_name_ : binder.AddValue( AE_TOSTRING(_name_), EPolygonMode::_name_ );
			BIND( Point )
			BIND( Line )
			BIND( Fill )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EPrimitive::Unknown )
		{
			case EPrimitive::Unknown :
			#define BIND( _name_ )	case EPrimitive::_name_ : binder.AddValue( AE_TOSTRING(_name_), EPrimitive::_name_ );
			BIND( Point )
			BIND( LineList )
			BIND( LineStrip )
			BIND( LineListAdjacency )
			BIND( LineStripAdjacency )
			BIND( TriangleList )
			BIND( TriangleStrip )
			BIND( TriangleFan )
			BIND( TriangleListAdjacency )
			BIND( TriangleStripAdjacency )
			BIND( Patch )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ECullMode::None )
		{
			#define BIND( _name_ )	case ECullMode::_name_ : binder.AddValue( AE_TOSTRING(_name_), ECullMode::_name_ );
			BIND( None )
			BIND( Front )
			BIND( Back )
			BIND( FontAndBack )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EPipelineDynamicState::All )
		{
			case EPipelineDynamicState::All :
			case EPipelineDynamicState::_Last :
			case EPipelineDynamicState::GraphicsPipelineMask :
			case EPipelineDynamicState::Unknown :	binder.AddValue( "None", EPipelineDynamicState::Unknown );
			#define BIND( _name_ )					case EPipelineDynamicState::_name_ : binder.AddValue( AE_TOSTRING(_name_), EPipelineDynamicState::_name_ );
			BIND( StencilCompareMask )
			BIND( StencilWriteMask )
			BIND( StencilReference )
			BIND( DepthBias )
			BIND( BlendConstants )
			BIND( RTStackSize )
			BIND( FragmentShadingRate )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EImageAspect::Unknown )
		{
			case EImageAspect::Unknown :
			case EImageAspect::Metadata :
			case EImageAspect::All :
			case EImageAspect::_Last :
			case EImageAspect::_PlaneMask :
			#define BIND( _name_ )			case EImageAspect::_name_ : binder.AddValue( AE_TOSTRING(_name_), EImageAspect::_name_ );
			BIND( Color )
			BIND( Depth )
			BIND( Stencil )
			BIND( DepthStencil )
			BIND( Plane_0 )
			BIND( Plane_1 )
			BIND( Plane_2 )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EShaderIO::Unknown )
		{
			case EShaderIO::Unknown :
			case EShaderIO::_Count :
			#define BIND( _name_ )			case EShaderIO::_name_ : binder.AddValue( AE_TOSTRING(_name_), EShaderIO::_name_ );
			BIND( Int )
			BIND( UInt )
			BIND( Float )
			BIND( UFloat )
			BIND( Half )
			BIND( UNorm )
			BIND( SNorm )
			BIND( sRGB )
			BIND( AnyColor )
			BIND( Depth )
			BIND( Stencil )
			BIND( DepthStencil )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESubgroupTypes::Unknown )
		{
			case ESubgroupTypes::Unknown :
			case ESubgroupTypes::_Last :
			case ESubgroupTypes::All :
			#define BIND( _name_ )			case ESubgroupTypes::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESubgroupTypes::_name_ );
			BIND( Float16 )
			BIND( Float32 )
			BIND( Int8 )
			BIND( Int16 )
			BIND( Int32 )
			BIND( Int64 )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESubgroupOperation::_Count )
		{
			case ESubgroupOperation::_Count :
			#define BIND( _name_ )		case ESubgroupOperation::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESubgroupOperation::_name_ );
			// Basic
			BIND( IndexAndSize )
			BIND( Elect )
			BIND( Barrier )
			// Vote
			BIND( Any )
			BIND( All )
			BIND( AllEqual )
			// Arithmetic
			BIND( Add )
			BIND( Mul );
			BIND( Min );
			BIND( Max );
			BIND( And );
			BIND( Or );
			BIND( Xor );
			BIND( InclusiveMul );
			BIND( InclusiveAdd );
			BIND( InclusiveMin );
			BIND( InclusiveMax );
			BIND( InclusiveAnd );
			BIND( InclusiveOr );
			BIND( InclusiveXor );
			BIND( ExclusiveAdd );
			BIND( ExclusiveMul );
			BIND( ExclusiveMin );
			BIND( ExclusiveMax );
			BIND( ExclusiveAnd );
			BIND( ExclusiveOr );
			BIND( ExclusiveXor );
			// Ballot
			BIND( Ballot )
			BIND( Broadcast )
			BIND( BroadcastFirst )
			BIND( InverseBallot )
			BIND( BallotBitExtract )
			BIND( BallotBitCount )
			BIND( BallotInclusiveBitCount )
			BIND( BallotExclusiveBitCount )
			BIND( BallotFindLSB )
			BIND( BallotFindMSB )
			// Shuffle
			BIND( Shuffle )
			BIND( ShuffleXor )
			// Shuffle Relative
			BIND( ShuffleUp )
			BIND( ShuffleDown )
			// Clustered
			BIND( ClusteredAdd )
			BIND( ClusteredMul )
			BIND( ClusteredMin )
			BIND( ClusteredMax )
			BIND( ClusteredAnd )
			BIND( ClusteredOr )
			BIND( ClusteredXor )
			// Quad
			BIND( QuadBroadcast )
			BIND( QuadSwapHorizontal )
			BIND( QuadSwapVertical )
			BIND( QuadSwapDiagonal )
			// Other
			BIND( PartitionedNV )
			BIND( Rotate )
			BIND( RotateClustered )
			#undef BIND
			// ranges
			default :
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
				break;
		}
		switch_end
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
		switch_enum( EFeature::_Count )
		{
			case EFeature::_Count :
			#define BIND( _name_ )		case EFeature::_name_ : binder.AddValue( AE_TOSTRING(_name_), EFeature::_name_ );
			BIND( Ignore )
			BIND( RequireTrue )
			BIND( RequireFalse )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EShader::Unknown )
		{
			case EShader::Unknown :
			#define BIND( _name_ )		case EShader::_name_ : binder.AddValue( AE_TOSTRING(_name_), EShader::_name_ );
			BIND( Vertex )
			BIND( TessControl )
			BIND( TessEvaluation )
			BIND( Geometry )
			BIND( Fragment )
			BIND( Compute )
			BIND( Tile )
			BIND( MeshTask )
			BIND( Mesh )
			BIND( RayGen )
			BIND( RayAnyHit )
			BIND( RayClosestHit )
			BIND( RayMiss )
			BIND( RayIntersection )
			BIND( RayCallable )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EShaderStages::Unknown )
		{
			case EShaderStages::Unknown :
			#define BIND( _name_ )			case EShaderStages::_name_ : binder.AddValue( AE_TOSTRING(_name_), EShaderStages::_name_ );
			BIND( Vertex )
			BIND( TessControl )
			BIND( TessEvaluation )
			BIND( Geometry )
			BIND( Fragment )
			BIND( Compute )
			BIND( Tile )
			BIND( MeshTask )
			BIND( Mesh )
			BIND( RayGen )
			BIND( RayAnyHit )
			BIND( RayClosestHit )
			BIND( RayMiss )
			BIND( RayIntersection )
			BIND( RayCallable )
			BIND( All )
			BIND( AllGraphics )
			BIND( GraphicsPipeStages )
			BIND( MeshPipeStages )
			BIND( VertexProcessingStages )
			BIND( PreRasterizationStages )
			BIND( PostRasterizationStages )
			BIND( AllRayTracing )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EGPUVendor::Unknown )
		{
			case EGPUVendor::Unknown :
			#define BIND( _name_ )		case EGPUVendor::_name_ : binder.AddValue( AE_TOSTRING(_name_), EGPUVendor::_name_ );
			BIND( AMD )
			BIND( NVidia )
			BIND( Intel )
			BIND( ARM )
			BIND( Qualcomm )
			BIND( ImgTech )
			BIND( Microsoft )
			BIND( Apple )
			BIND( Mesa )
			BIND( Broadcom )
			BIND( Samsung )
			BIND( VeriSilicon )
			BIND( Huawei )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EFilter::Unknown )
		{
			case EFilter::Unknown :
			#define BIND( _name_ )		case EFilter::_name_ : binder.AddValue( AE_TOSTRING(_name_), EFilter::_name_ );
			BIND( Nearest )
			BIND( Linear )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EMipmapFilter::Unknown )
		{
			case EMipmapFilter::Unknown :
			#define BIND( _name_ )		case EMipmapFilter::_name_ : binder.AddValue( AE_TOSTRING(_name_), EMipmapFilter::_name_ );
			BIND( None )
			BIND( Nearest )
			BIND( Linear )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EAddressMode::Unknown )
		{
			case EAddressMode::Unknown :
			#define BIND( _name_ )		case EAddressMode::_name_ : binder.AddValue( AE_TOSTRING(_name_), EAddressMode::_name_ );
			BIND( Repeat )
			BIND( MirrorRepeat )
			BIND( ClampToEdge )
			BIND( ClampToBorder )
			BIND( MirrorClampToEdge )
			#undef BIND
			default :
				// alias:
				binder.AddValue( "Clamp",		EAddressMode::ClampToEdge );
				binder.AddValue( "MirrorClamp",	EAddressMode::MirrorClampToEdge );
				break;
		}
		switch_end
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
		switch_enum( EBorderColor::Unknown )
		{
			case EBorderColor::Unknown :
			#define BIND( _name_ )		case EBorderColor::_name_ : binder.AddValue( AE_TOSTRING(_name_), EBorderColor::_name_ );
			BIND( FloatTransparentBlack )
			BIND( FloatOpaqueBlack )
			BIND( FloatOpaqueWhite )
			BIND( IntTransparentBlack )
			BIND( IntOpaqueBlack )
			BIND( IntOpaqueWhite )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EReductionMode::Unknown )
		{
			case EReductionMode::Unknown :
			#define BIND( _name_ )		case EReductionMode::_name_ : binder.AddValue( AE_TOSTRING(_name_), EReductionMode::_name_ );
			BIND( Average )
			BIND( Min )
			BIND( Max )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESamplerOpt::Unknown )
		{
			case ESamplerOpt::Unknown :
			#define BIND( _name_ )		case ESamplerOpt::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESamplerOpt::_name_ );
			BIND( ArgumentBuffer )
			BIND( UnnormalizedCoordinates )
			BIND( NonSeamlessCubeMap )
			BIND( Subsampled )
			BIND( SubsampledCoarseReconstruction )
			#undef BIND
			case ESamplerOpt::_Last :
			case ESamplerOpt::All :
			default : break;
		}
		switch_end
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
		switch_enum( EVertexInputRate::Unknown )
		{
			case EVertexInputRate::Unknown :
			#define BIND( _name_ )		case EVertexInputRate::_name_ : binder.AddValue( AE_TOSTRING(_name_), EVertexInputRate::_name_ );
			BIND( Vertex )
			BIND( Instance )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EDescSetUsage::Unknown )
		{
			case EDescSetUsage::Unknown :
			case EDescSetUsage::_Last :
			case EDescSetUsage::All :
			#define BIND( _name_ )		case EDescSetUsage::_name_ : binder.AddValue( AE_TOSTRING(_name_), EDescSetUsage::_name_ );
			BIND( AllowPartialyUpdate )
			BIND( UpdateTemplate )
			BIND( ArgumentBuffer )
			BIND( MutableArgBuffer )
			BIND( MaybeUnsupported )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( EQueueMask::Unknown )
		{
			case EQueueMask::Unknown :
			#define BIND( _name_ )		case EQueueMask::_name_ : binder.AddValue( AE_TOSTRING(_name_), EQueueMask::_name_ );
			BIND( Graphics )
			BIND( AsyncCompute )
			BIND( AsyncTransfer )
			BIND( VideoEncode )
			BIND( VideoDecode )
			BIND( All )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESamplerChromaLocation::Unknown )
		{
			case ESamplerChromaLocation::Unknown :
			#define BIND( _name_ )		case ESamplerChromaLocation::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESamplerChromaLocation::_name_ );
			BIND( CositedEven )
			BIND( Midpoint )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESamplerYcbcrModelConversion::Unknown )
		{
			case ESamplerYcbcrModelConversion::Unknown :
			#define BIND( _name_ )		case ESamplerYcbcrModelConversion::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESamplerYcbcrModelConversion::_name_ );
			BIND( RGB_Identity )
			BIND( Ycbcr_Identity )
			BIND( Ycbcr_709 )
			BIND( Ycbcr_601 )
			BIND( Ycbcr_2020 )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESamplerYcbcrRange::Unknown )
		{
			case ESamplerYcbcrRange::Unknown :
			#define BIND( _name_ )		case ESamplerYcbcrRange::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESamplerYcbcrRange::_name_ );
			BIND( ITU_Full )
			BIND( ITU_Narrow )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ESurfaceFormat::Unknown )
		{
			case ESurfaceFormat::Unknown :
			#define BIND( _name_ )		case ESurfaceFormat::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESurfaceFormat::_name_ );
			BIND( BGRA8_sRGB_nonlinear )
			BIND( RGBA8_sRGB_nonlinear )
			BIND( BGRA8_BT709_nonlinear )
			BIND( RGBA16F_sRGB_nonlinear )
			BIND( RGBA16F_Extended_sRGB_linear )
			BIND( RGBA16F_Extended_sRGB_nonlinear )
			BIND( RGBA16F_BT709_nonlinear )
			BIND( RGBA16F_HDR10_ST2084 )
			BIND( RGBA16F_BT2020_linear )
			BIND( RGB10A2_sRGB_nonlinear )
			BIND( RGB10A2_HDR10_ST2084 )
			#undef BIND
			default : break;
		}
		switch_end
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
		switch_enum( ERTInstanceOpt::Unknown )
		{
			case ERTInstanceOpt::Unknown :
			case ERTInstanceOpt::_Last :
			case ERTInstanceOpt::All :
			#define BIND( _name_ )		case ERTInstanceOpt::_name_ : binder.AddValue( AE_TOSTRING(_name_), ERTInstanceOpt::_name_ );
			BIND( TriangleCullDisable )
			BIND( TriangleFrontCCW )
			BIND( ForceOpaque )
			BIND( ForceNonOpaque )
			BIND( DisableOpacityMicromaps )
			BIND( ForceOpacityMicromap2State )
			#undef BIND
			default :
				binder.AddValue( "TriangleCullBack",	ERTInstanceOpt::TriangleCullBack	);
				binder.AddValue( "TriangleFrontCW",		ERTInstanceOpt::TriangleFrontCW		);
				break;
		}
		switch_end
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
		switch_enum( EImageUsage::Unknown )
		{
			case EImageUsage::Unknown :
			case EImageUsage::_Last :
			case EImageUsage::All :
			case EImageUsage::Transfer :
			case EImageUsage::RWAttachment :
			#define BIND( _name_ )		case EImageUsage::_name_ : binder.AddValue( AE_TOSTRING(_name_), EImageUsage::_name_ );
			BIND( TransferSrc )
			BIND( TransferDst )
			BIND( Sampled )
			BIND( Storage )
			BIND( ColorAttachment )
			BIND( DepthStencilAttachment )
			BIND( InputAttachment )
			BIND( ShadingRate )
			BIND( FragmentDensityMap )
			#undef BIND
			default :
				binder.AddValue( "All",				EImageUsage::All			);
				binder.AddValue( "Transfer",		EImageUsage::Transfer		);
				binder.AddValue( "RWAttachment",	EImageUsage::RWAttachment	);
				break;
		}
		switch_end
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
		switch_enum( EImageOpt::Unknown )
		{
			case EImageOpt::Unknown :
			case EImageOpt::_Last :
			case EImageOpt::All :
			case EImageOpt::Blit :
			case EImageOpt::SparseResidencyAliased :
			#define BIND( _name_ )		case EImageOpt::_name_ : binder.AddValue( AE_TOSTRING(_name_), EImageOpt::_name_ );
			BIND( BlitSrc )
			BIND( BlitDst )
			BIND( CubeCompatible )
			BIND( MutableFormat )
			BIND( Array2DCompatible )
			BIND( BlockTexelViewCompatible )
			BIND( SparseResidency )
			BIND( SparseAliased )
			BIND( Alias )
			BIND( SampleLocationsCompatible )
			BIND( StorageAtomic )
			BIND( ColorAttachmentBlend )
			BIND( SampledLinear )
			BIND( SampledMinMax )
			BIND( VertexPplnStore )
			BIND( FragmentPplnStore )
			BIND( LossyRTCompression )
			BIND( ExtendedUsage )
			BIND( SeparatePlanes )
			BIND( Subsampled )
			#undef BIND
			default :
				binder.AddValue( "All",						EImageOpt::All						);
				binder.AddValue( "SparseResidencyAliased",	EImageOpt::SparseResidencyAliased	);
				binder.AddValue( "Blit",					EImageOpt::Blit						);
				break;
		}
		switch_end
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
		switch_enum( EBufferUsage::Unknown )
		{
			case EBufferUsage::Unknown :
			case EBufferUsage::_Last :
			case EBufferUsage::All :
			case EBufferUsage::Transfer :
			#define BIND( _name_ )		case EBufferUsage::_name_ : binder.AddValue( AE_TOSTRING(_name_), EBufferUsage::_name_ );
			BIND( TransferSrc )
			BIND( TransferDst )
			BIND( UniformTexel )
			BIND( StorageTexel )
			BIND( Uniform )
			BIND( Storage )
			BIND( Index )
			BIND( Vertex )
			BIND( Indirect )
			BIND( ShaderAddress )
			BIND( ShaderBindingTable )
			BIND( ASBuild_ReadOnly )
			BIND( ASBuild_Scratch )
			BIND( MMBuild_ReadOnly )
			BIND( RTAS_Storage )
			BIND( ICB_Preprocess )
			#undef BIND
			default :
				binder.AddValue( "All",				EBufferUsage::All		);
				binder.AddValue( "Transfer",		EBufferUsage::Transfer	);
				binder.AddValue( "MMBuild_Scratch",	EBufferUsage::MMBuild_Scratch	);
				break;
		}
		switch_end
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
		switch_enum( EBufferOpt::Unknown )
		{
			case EBufferOpt::Unknown :
			case EBufferOpt::_Last :
			case EBufferOpt::All :
			case EBufferOpt::SparseResidencyAliased :
			#define BIND( _name_ )		case EBufferOpt::_name_ : binder.AddValue( AE_TOSTRING(_name_), EBufferOpt::_name_ );
			BIND( SparseResidency )
			BIND( SparseAliased )
			BIND( VertexPplnStore )
			BIND( FragmentPplnStore )
			BIND( StorageTexelAtomic )
			#undef BIND
			default :
				binder.AddValue( "All",						EBufferOpt::All						);
				binder.AddValue( "SparseResidencyAliased",	EBufferOpt::SparseResidencyAliased	);
				break;
		}
		switch_end
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

		switch_enum( EIntegerDotProductFeat::_Count )
		{
			case EIntegerDotProductFeat::_Count :
			#define BIND( _name_ )		case EIntegerDotProductFeat::_name_ :	binder.AddValue( AE_TOSTRING(_name_), EIntegerDotProductFeat::_name_ );
			BIND( Unsigned8bit )
			BIND( Signed8bit )
			BIND( MixedSignedness8bit )
			BIND( Unsigned4x8bit )
			BIND( Signed4x8bit )
			BIND( MixedSignedness4x8bit )
			BIND( Unsigned16bit )
			BIND( Signed16bit )
			BIND( MixedSignedness16bit )
			BIND( Unsigned32bit )
			BIND( Signed32bit )
			BIND( MixedSignedness32bit )
			BIND( Unsigned64bit )
			BIND( Signed64bit )
			BIND( MixedSignedness64bit )
			BIND( AccSat_Unsigned8bit )
			BIND( AccSat_Signed8bit )
			BIND( AccSat_MixedSignedness8bit )
			BIND( AccSat_Unsigned4x8bit )
			BIND( AccSat_Signed4x8bit )
			BIND( AccSat_MixedSignedness4x8bit )
			BIND( AccSat_Unsigned16bit )
			BIND( AccSat_Signed16bit )
			BIND( AccSat_MixedSignedness16bit )
			BIND( AccSat_Unsigned32bit )
			BIND( AccSat_Signed32bit )
			BIND( AccSat_MixedSignedness32bit )
			BIND( AccSat_Unsigned64bit )
			BIND( AccSat_Signed64bit )
			BIND( AccSat_MixedSignedness64bit )
			#undef BIND
		}
		switch_end
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

		switch_enum( ECoopMatrixComponentType::_Count )
		{
			case ECoopMatrixComponentType::_Count :
			#define BIND( _name_ )		case ECoopMatrixComponentType::_name_ :	binder.AddValue( AE_TOSTRING(_name_), ECoopMatrixComponentType::_name_ );
			BIND( Float16 )
			BIND( Float32 )
			BIND( Float64 )
			BIND( BFloat16 )
			BIND( Float8_E4M3 )
			BIND( Float8_E5M2 )
			BIND( SInt8 )
			BIND( SInt16 )
			BIND( SInt32 )
			BIND( SInt64 )
			BIND( SInt8x4 )
			BIND( UInt8 )
			BIND( UInt16 )
			BIND( UInt32 )
			BIND( UInt64 )
			BIND( UInt8x4 )
			#undef BIND
		}
		switch_end
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

		switch_enum( ECoopVecMatrixLayout::_Count )
		{
			case ECoopVecMatrixLayout::_Count :
			#define BIND( _name_ )		case ECoopVecMatrixLayout::_name_ :	binder.AddValue( AE_TOSTRING(_name_), ECoopVecMatrixLayout::_name_ );
			BIND( RowMajor )
			BIND( ColumnMajor )
			BIND( InferencingOptimal )
			BIND( TrainingOptimal )
			#undef BIND
		}
		switch_end
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

		switch_enum( ECoopMatrixCfg::_Count )
		{
			case ECoopMatrixCfg::_Count :
			#define BIND( _name_ )		case ECoopMatrixCfg::_name_ :	binder.AddValue( AE_TOSTRING(_name_), ECoopMatrixCfg::_name_ );
			BIND( Afp16_Bfp16_Cfp16_Rfp16_M16_N16_K16 )
			BIND( Afp16_Bfp16_Cfp32_Rfp32_M16_N16_K16 )
			BIND( Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16 )
			BIND( Au8_Bu8_Cu32_Ru32_M16_N16_K32 )
			BIND( As8_Bs8_Cs32_Rs32_M16_N16_K32 )
			BIND( Au8_Bu8_Cu32_Ru32_M16_N16_K16 )
			BIND( As8_Bs8_Cs32_Rs32_M16_N16_K16 )
			BIND( Au8_Bu8_Cu32_Ru32_M8_N8_K32 )
			BIND( As8_Bs8_Cs32_Rs32_M8_N8_K32 )
			#undef BIND
		}
		switch_end
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

		switch_enum( ECoopVecCfg::_Count )
		{
			case ECoopVecCfg::_Count :
			#define BIND( _name_ )		case ECoopVecCfg::_name_ :	binder.AddValue( AE_TOSTRING(_name_), ECoopVecCfg::_name_ );
			BIND( Tfp16_Ifp16_Mfp16_Bfp16_Rfp16_Tp )
			BIND( Tfp16_Ifp8e4m3_Mfp8e4m3_Bfp16_Rfp16 )
			BIND( Tfp16_Ifp8e5m2_Mfp8e5m2_Bfp16_Rfp16 )
			BIND( Ts8_Is8_Ms8_Bs32_Rs32 )
			#undef BIND
		}
		switch_end
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

		switch_enum( EConservativeRasterizationMode::_Count )
		{
			case EConservativeRasterizationMode::_Count :
			#define BIND( _name_ )		case EConservativeRasterizationMode::_name_ :	binder.AddValue( AE_TOSTRING(_name_), EConservativeRasterizationMode::_name_ );
			BIND( Disabled )
			BIND( Overestimate )
			//BIND( Underestimate )
			#undef BIND
		}
		switch_end
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

		switch_enum( EMicromapType::_Count )
		{
			case EMicromapType::_Count :
			#define BIND( _name_ )		case EMicromapType::_name_ :	binder.AddValue( AE_TOSTRING(_name_), EMicromapType::_name_ );
			BIND( Opacity )
			BIND( Displacement )
			#undef BIND
		}
		switch_end
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

		switch_enum( EOpacityMicromapFormat::Unknown )
		{
			case EOpacityMicromapFormat::Unknown :
			#define BIND( _name_ )		case EOpacityMicromapFormat::_name_ :	binder.AddValue( AE_TOSTRING(_name_), EOpacityMicromapFormat::_name_ );
			BIND( TwoState )
			BIND( FourState )
			#undef BIND
		}
		switch_end
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
