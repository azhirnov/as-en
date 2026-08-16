// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_LOGS
# include "graphics_rhi/Public/EResourceState.h"
# include "graphics_rhi/Public/ResourceEnums.h"
# include "graphics_rhi/Public/ShaderEnums.h"
# include "graphics_rhi/Public/VertexEnums.h"
# include "graphics_rhi/Public/RenderStateEnums.h"
# include "graphics_rhi/Public/FeatureSetEnums.h"
# include "graphics_rhi/Public/Queue.h"
# include "graphics_rhi/Public/SamplerDesc.h"
# include "graphics_rhi/Public/DescriptorSet.h"
# include "graphics_rhi/Public/PipelineDesc.h"
# include "graphics_rhi/Public/RayTracingEnums.h"
# include "graphics_rhi/Public/VideoEnums.h"
# include "graphics_rhi/Public/ImageSwizzle.h"
# include "graphics_rhi/Public/CoopVector.h"
# include "graphics_rhi/Private/PixelFormatDefines.h"

namespace AE::Base
{
	using Graphics::EPixelFormat;
	using Graphics::EResourceState;
	using Graphics::EShaderStages;
	using Graphics::EVertexType;
	using Graphics::EGraphicsDeviceID;
	using Graphics::EMemoryType;
	using Graphics::EDescriptorType;
	using Graphics::ImageSwizzle;
	using Graphics::ECoopMatrixCfg;
	using Graphics::ECoopVecCfg;
	using Graphics::EImageDim;

/*
=================================================
	ToString (EPixelFormat)
=================================================
*/
	Nd__In StringView  ToString (EPixelFormat value) __NE___
	{
		switch_enum( value )
		{
			#define AE_PIXELFORMAT_VISIT( _name_ )		case EPixelFormat::_name_ : return AE_TOSTRING( _name_ );
			AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT )
			#undef AE_PIXELFORMAT_VISIT

			case EPixelFormat::SwapchainColor : return "SwapchainColor";
			case EPixelFormat::Unknown :		return "Unknown";
		}
		switch_end
		RETURN_ERR( "unknown pixel format" );
	}

/*
=================================================
	ToString (EShaderStages)
=================================================
*/
	Nd__In String  ToString (EShaderStages values) __Th___
	{
		String	str;

		if ( AllBits( values, EShaderStages::AllGraphics ))
		{
			if ( not str.empty() ) str << " | ";
			str << "AllGraphics";
			values &= ~EShaderStages::AllGraphics;
		}

		if ( AllBits( values, EShaderStages::AllRayTracing ))
		{
			if ( not str.empty() ) str << " | ";
			str << "AllRayTracing";
			values &= ~EShaderStages::AllRayTracing;
		}

		if ( AllBits( values, EShaderStages::MeshPipeStages ))
		{
			if ( not str.empty() ) str << " | ";
			str << "MeshPipeStages";
			values &= ~EShaderStages::MeshPipeStages;
		}

		if ( AllBits( values, EShaderStages::GraphicsPipeStages ))
		{
			if ( not str.empty() ) str << " | ";
			str << "GraphicsPipeStages";
			values &= ~EShaderStages::GraphicsPipeStages;
		}

		for (auto bit : BitIndexIterate<Graphics::EShader>( values ))
		{
			if ( not str.empty() )
				str << " | ";

			str << ToString( bit );
		}
		return str;
	}

/*
=================================================
	ToString (EVertexType)
=================================================
*/
	Nd__In StringView  ToString (EVertexType value) __NE___
	{
		switch ( value )
		{
			#define AE_VERTEXTYPE_VISIT( _name_, _value_ )		case EVertexType::_name_ : return AE_TOSTRING( _name_ );
			AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT )
			#undef AE_VERTEXTYPE_VISIT
		}
		RETURN_ERR( "unknown vertex type" );
	}

/*
=================================================
	ToString (EGraphicsDeviceID)
=================================================
*/
	Nd__In StringView  ToString (EGraphicsDeviceID value) __NE___
	{
		switch ( value )
		{
			#define AE_GRAPHICS_DEVICE_VISIT( _name_ )		case EGraphicsDeviceID::_name_ : return AE_TOSTRING( _name_ );
			AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT )
			#undef AE_GRAPHICS_DEVICE_VISIT
		}
		RETURN_ERR( "unknown graphics device type" );
	}

/*
=================================================
	ToString (EMemoryType)
=================================================
*/
	Nd__In String  ToString (EMemoryType values) __Th___
	{
		switch ( values )
		{
			case EMemoryType::Unified :				return "Unified";
			case EMemoryType::UnifiedCached :		return "UnifiedCached";
			case EMemoryType::HostCachedCoherent :	return "HostCachedCoherent";
		}

		return BitEnumToString<EMemoryType>::ToString( values );
	}

/*
=================================================
	ToString (EDescriptorType)
=================================================
*/
	Nd__In StringView  ToString (EDescriptorType value, int) __NE___
	{
		switch_enum( value )
		{
			case EDescriptorType::UniformBuffer :
			case EDescriptorType::StorageBuffer :					return "Buffer";
			case EDescriptorType::UniformTexelBuffer :
			case EDescriptorType::StorageTexelBuffer :				return "TexelBuffer";
			case EDescriptorType::StorageImage :
			case EDescriptorType::SampledImage :
			case EDescriptorType::CombinedImage :
			case EDescriptorType::CombinedImage_ImmutableSampler :
			case EDescriptorType::SubpassInput :					return "Image";
			case EDescriptorType::Sampler :
			case EDescriptorType::ImmutableSampler :				return "Sampler";
			case EDescriptorType::RayTracingScene :					return "RayTracingScene";
			case EDescriptorType::RayTracingPartitionedScene :		return "RayTracingPartitionedScene";
			case EDescriptorType::_Count :							break;
		}
		switch_end
		RETURN_ERR( "unknown descriptor type" );
	}

/*
=================================================
	ToString (EResourceState)
=================================================
*/
	Nd__In String  ToString (const EResourceState value) __Th___
	{
		using _EResState = Graphics::_EResState;

		if ( value == EResourceState::_InvalidState )
			return "<InvalidState>";

		String	str {EnumToString<_EResState::EState>::ToString( ToEResState( value ))};

		if ( AnyBits( value, EResourceState::Invalidate ))
			str << " | Invalidate";

		// stages
		{
			EResourceState	stages = value & EResourceState::AllStages;

			if ( AllBits( stages, EResourceState::AllShaderStages ))
			{
				stages &= ~EResourceState::AllShaderStages;
				str << " | AllShaderStages";
			}
			if ( AllBits( stages, EResourceState::AllGraphicsShaders ))
			{
				stages &= ~EResourceState::AllGraphicsShaders;
				str << " | AllGraphicsShaders";
			}
			if ( AllBits( stages, EResourceState::PreRasterizationShaders ))
			{
				stages &= ~EResourceState::PreRasterizationShaders;
				str << " | PreRasterizationShaders";
			}
			if ( AllBits( stages, EResourceState::PostRasterizationShaders ))
			{
				stages &= ~EResourceState::PostRasterizationShaders;
				str << " | PostRasterizationShaders";
			}

			if ( AnyBits( value, EResourceState::DSTestBeforeFS ))
			{
				stages &= ~EResourceState::DSTestBeforeFS;
				str << " | DSTestBeforeFS";
			}

			if ( AnyBits( value, EResourceState::DSTestAfterFS ))
			{
				stages &= ~EResourceState::DSTestAfterFS;
				str << " | DSTestAfterFS";
			}

			for (auto stage : BitfieldIterate( stages ))
			{
				switch ( stage )
				{
					case EResourceState::MeshTaskShader :			str << " | MeshTaskShader";				break;
					case EResourceState::VertexProcessingShaders :	str << " | VertexProcessingShaders";	break;
					case EResourceState::TileShader :				str << " | TileShader";					break;
					case EResourceState::FragmentShader :			str << " | FragmentShader";				break;
					case EResourceState::ComputeShader :			str << " | ComputeShader";				break;
					case EResourceState::RayTracingShaders :		str << " | RayTracingShaders";			break;
					default :										DBG_WARNING( "unknown resource state stage" );	break;
				}
				StaticAssert( uint(EResourceState::AllShaderStages) == 0x1F8000 );
			}
		}

		return str;
	}

/*
=================================================
	ToString (ImageSwizzle)
=================================================
*/
	Nd__In String  ToString (const ImageSwizzle &value) __NE___
	{
		String			str;
		const uint4		comp		= value.ToVec();
		const char		comp_str[]	= ".RGBA01";

		for (uint i = 0; i < 4; ++i) {
			str << comp_str[ comp[i] ];
		}
		return str;
	}

/*
=================================================
	ToString (ECoopMatrixCfg / ECoopVecCfg)
=================================================
*/
	Nd__In String  ToString (ECoopMatrixCfg type) __Th___
	{
		return Graphics::CoopMatrixConfig{type}.ToString();
	}

	Nd__In String  ToString (ECoopVecCfg type) __Th___
	{
		return Graphics::CoopVectorConfig{type}.ToString();
	}

/*
=================================================
	ToString (EImageDim)
=================================================
*/
	Nd__In StringView  ToString (EImageDim value) __Th___
	{
		switch_enum( value )
		{
			case EImageDim::_1D :		return "1D";
			case EImageDim::_2D :		return "2D";
			case EImageDim::_3D :		return "3D";
			case EImageDim::_Count :
			default :					break;
		}
		switch_end
		RETURN_ERR( "unknown image dimension type" );
	}

} // AE::Base

#endif // AE_ENABLE_LOGS
