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
		EnumBinder<EImage>	binder{ se };
		binder.Create();
		switch_enum( EImage::Unknown )
		{
			case EImage::Unknown :
			case EImage::_Count :
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
			case EIndex::_Count :
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
			case EPixelFormatExternal::_Count :
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
			case ECompareOp::_Count :
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
		switch_enum( EBlendFactor::Unknown )
		{
			case EBlendFactor::Unknown :
			case EBlendFactor::_Count :
			#define BIND( _name_ )		case EBlendFactor::_name_ : binder.AddValue( AE_TOSTRING(_name_), EBlendFactor::_name_ );
			BIND( Zero )
			BIND( One )
			BIND( SrcColor )
			BIND( OneMinusSrcColor )
			BIND( DstColor )
			BIND( OneMinusDstColor )
			BIND( SrcAlpha )
			BIND( OneMinusSrcAlpha )
			BIND( DstAlpha )
			BIND( OneMinusDstAlpha )
			BIND( ConstColor )
			BIND( OneMinusConstColor )
			BIND( ConstAlpha )
			BIND( OneMinusConstAlpha )
			BIND( SrcAlphaSaturate )
			BIND( Src1Color )
			BIND( OneMinusSrc1Color )
			BIND( Src1Alpha )
			BIND( OneMinusSrc1Alpha )
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
		switch_enum( EBlendOp::Unknown )
		{
			case EBlendOp::Unknown :
			case EBlendOp::_Count :
			#define BIND( _name_ )	case EBlendOp::_name_ : binder.AddValue( AE_TOSTRING(_name_), EBlendOp::_name_ );
			BIND( Add )
			BIND( Sub )
			BIND( RevSub )
			BIND( Min )
			BIND( Max )
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
		switch_enum( ELogicOp::Unknown )
		{
			case ELogicOp::Unknown :
			case ELogicOp::_Count :
			#define BIND( _name_ )	case ELogicOp::_name_ : binder.AddValue( AE_TOSTRING(_name_), ELogicOp::_name_ );
			BIND( None )
			BIND( Clear )
			BIND( Set )
			BIND( Copy )
			BIND( CopyInverted )
			BIND( NoOp )
			BIND( Invert )
			BIND( And )
			BIND( NotAnd )
			BIND( Or )
			BIND( NotOr )
			BIND( Xor )
			BIND( Equiv )
			BIND( AndReverse )
			BIND( AndInverted )
			BIND( OrReverse )
			BIND( OrInverted )
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
			case EStencilOp::_Count :
			#define BIND( _name_ )	case EStencilOp::_name_ : binder.AddValue( AE_TOSTRING(_name_), EStencilOp::_name_ );
			BIND( Keep )
			BIND( Zero )
			BIND( Replace )
			BIND( Incr )
			BIND( IncrWrap )
			BIND( Decr )
			BIND( DecrWrap )
			BIND( Invert )
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
			case EPolygonMode::_Count :
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
			case EPrimitive::_Count :
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
			case EShader::_Count :
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
			case EFilter::_Count :
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
			case EMipmapFilter::_Count :
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
			case EAddressMode::_Count :
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
			case EBorderColor::_Count :
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
			case EReductionMode::_Count :
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
		//	BIND( Subsampled )
		//	BIND( SubsampledCoarseReconstruction )
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
			case EVertexInputRate::_Count :
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
			BIND2( DontCompile, "Pipeline creation will fail if it is not exists in cache." )
			BIND2( CaptureStatistics, "When a pipeline is created, its state and shaders are compiled into zero or more device-specific executables,\nwhich are used when executing commands against that pipeline." )
			BIND2( CaptureInternalRepresentation, "May include the final shader assembly, a binary form of the compiled shader,\nor the shader compiler’s internal representation at any number of intermediate compile steps." )
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
			case ESamplerChromaLocation::_Count :
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
			case ESamplerYcbcrModelConversion::_Count :
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
			case ESamplerYcbcrRange::_Count :
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
			case ESurfaceFormat::_Count :
			#define BIND( _name_ )		case ESurfaceFormat::_name_ : binder.AddValue( AE_TOSTRING(_name_), ESurfaceFormat::_name_ );
			BIND( BGRA8_sRGB_nonlinear )
			BIND( RGBA8_sRGB_nonlinear )
			BIND( BGRA8_BT709_nonlinear )
			BIND( RGBA16F_Extended_sRGB_linear )
			BIND( RGBA16F_sRGB_nonlinear )
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
			#undef BIND
			default :
				binder.AddValue( "All",						EImageOpt::All						);
				binder.AddValue( "SparseResidencyAliased",	EImageOpt::SparseResidencyAliased	);
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
			#undef BIND
			default :
				binder.AddValue( "All",			EBufferUsage::All		);
				binder.AddValue( "Transfer",	EBufferUsage::Transfer	);
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
		switch_enum( EShadingRateCombinerOp::Unknown )
		{
			case EShadingRateCombinerOp::Unknown :
			case EShadingRateCombinerOp::_Count :
			#define BIND( _name_ )		case EShadingRateCombinerOp::_name_ : binder.AddValue( AE_TOSTRING(_name_), EShadingRateCombinerOp::_name_ );
			BIND( Keep )
			BIND( Replace )
			BIND( Min )
			BIND( Max )
			BIND( Sum )
			BIND( Mul )
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
			BIND( CopyRTAS_Read )
			BIND( CopyRTAS_Write )
			BIND( BuildRTAS_Read )
			BIND( BuildRTAS_Write )
			BIND( BuildRTAS_RW )
			BIND( BuildRTAS_IndirectBuffer )
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
			BIND( AllShaders )
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
		PlacementNew<MipmapLevel>( OUT mem, ushort(level) );
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
		binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetR,		"setR",		{} );
		binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetG,		"setG",		{} );
		binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetB,		"setB",		{} );
		binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetA,		"setA",		{} );
		binder.AddMethodFromGlobal( &RS_ColorBuffer_ColorMask_SetRGBA,	"setRGBA",	{} );
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

		binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set1<0,EBlendFactor>,	"SrcBlendFactor",	{"colorAndAlpha"} );
		binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set2<0,EBlendFactor>,	"SrcBlendFactor",	{"color", "alpha"} );
		binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set1<1,EBlendFactor>,	"DstBlendFactor",	{"colorAndAlpha"} );
		binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set2<1,EBlendFactor>,	"DstBlendFactor",	{"color", "alpha"} );
		binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set1<2,EBlendOp>,		"BlendOp",			{"colorAndAlpha"} );
		binder.AddMethodFromGlobal( &RenderState_ColorBuffer_Set2<2,EBlendOp>,		"BlendOp",			{"color", "alpha"} );

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
		binder.AddMethodFromGlobal( &RenderState_ColorBuffersState_SetColorBuffer,	"SetColorBuffer",	{"index", "cb"} );
		binder.AddMethodFromGlobal( &RenderState_ColorBuffersState_GetColorBuffer,	"GetColorBuffer",	{"index"} );
		binder.AddProperty( &RenderState::ColorBuffersState::logicOp,				"logicOp" );
		binder.AddProperty( &RenderState::ColorBuffersState::blendColor,			"blendColor" );
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
		binder.AddProperty( &RenderState::StencilFaceState::failOp,			"failOp" );
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
	static void  RenderState_StencilBufferState_FailOp (RenderState::StencilBufferState &self, EStencilOp op)
	{
		self.enabled		= true;
		self.front.failOp	= op;
		self.back.failOp	= op;
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

	static void  RenderState_StencilBufferState_Reference (RenderState::StencilBufferState &self, ubyte value)
	{
		self.enabled			= true;
		self.front.reference	= value;
		self.back.reference		= value;
	}

	static void  RenderState_StencilBufferState_WriteMask (RenderState::StencilBufferState &self, ubyte value)
	{
		self.enabled			= true;
		self.front.writeMask	= value;
		self.back.writeMask		= value;
	}

	static void  RenderState_StencilBufferState_CompareMask (RenderState::StencilBufferState &self, ubyte value)
	{
		self.enabled			= true;
		self.front.compareMask	= value;
		self.back.compareMask	= value;
	}

	static void  Bind_RenderState_StencilBufferState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::StencilBufferState>	binder{ se };
		binder.CreateClassValue();

		binder.AddProperty( &RenderState::StencilBufferState::front,	"front" );
		binder.AddProperty( &RenderState::StencilBufferState::back,		"back" );
		binder.AddProperty( &RenderState::StencilBufferState::enabled,	"enabled" );

		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_FailOp,			"FailOp",		{} );
		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_DepthFailOp,	"DepthFailOp",	{} );
		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_PassOp,			"PassOp",		{} );
		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_CompareOp,		"CompareOp",	{} );
		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_Reference,		"Reference",	{} );
		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_WriteMask,		"WriteMask",	{} );
		binder.AddMethodFromGlobal( &RenderState_StencilBufferState_CompareMask,	"CompareMask",	{} );
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
		binder.AddProperty( &RenderState::RasterizationState::depthBiasConstFactor,	"depthBiasConstFactor" );
		binder.AddProperty( &RenderState::RasterizationState::depthBiasClamp,		"depthBiasClamp" );
		binder.AddProperty( &RenderState::RasterizationState::depthBiasSlopeFactor,	"depthBiasSlopeFactor" );
		binder.AddProperty( &RenderState::RasterizationState::depthBias,			"depthBias" );
		binder.AddProperty( &RenderState::RasterizationState::polygonMode,			"polygonMode" );
		binder.AddProperty( &RenderState::RasterizationState::depthClamp,			"depthClamp" );
		binder.AddProperty( &RenderState::RasterizationState::rasterizerDiscard,	"rasterizerDiscard" );
		binder.AddProperty( &RenderState::RasterizationState::frontFaceCCW,			"frontFaceCCW" );
		binder.AddProperty( &RenderState::RasterizationState::cullMode,				"cullMode" );
	}

/*
=================================================
	Bind_RenderState_MultisampleState
=================================================
*/
	static void  Bind_RenderState_MultisampleState (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<RenderState::MultisampleState>	binder{ se };
		binder.CreateClassValue();
		binder.AddProperty( &RenderState::MultisampleState::sampleMask,			"sampleMask" );
		binder.AddProperty( &RenderState::MultisampleState::minSampleShading,	"minSampleShading" );
		binder.AddProperty( &RenderState::MultisampleState::samples,			"samples" );
		binder.AddProperty( &RenderState::MultisampleState::sampleShading,		"sampleShading" );
		binder.AddProperty( &RenderState::MultisampleState::alphaToCoverage,	"alphaToCoverage" );
		binder.AddProperty( &RenderState::MultisampleState::alphaToOne,			"alphaToOne" );
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
		binder.AddProperty( &RenderState::color,			"color" );
		binder.AddProperty( &RenderState::depth,			"depth" );
		binder.AddProperty( &RenderState::stencil,			"stencil" );
		binder.AddProperty( &RenderState::inputAssembly,	"inputAssembly" );
		binder.AddProperty( &RenderState::rasterization,	"rasterization" );
		binder.AddProperty( &RenderState::multisample,		"multisample" );
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
