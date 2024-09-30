// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/DescriptorSetLayout.h"
#include "ScriptObjects/Common.inl.h"
#include "Compiler/MetalCompiler.h"

namespace AE::PipelineCompiler
{
namespace
{
	constexpr EShaderStages		s_MetalStages =
		EShaderStages::Vertex | EShaderStages::Fragment | EShaderStages::Tile | EShaderStages::Compute |
		EShaderStages::Mesh | EShaderStages::MeshTask;

	static DescriptorSetLayout*  DescriptorSetLayout_Ctor (const String &name) {
		return DescriptorSetLayoutPtr{ new DescriptorSetLayout{ name }}.Detach();
	}

/*
=================================================
	AccessToStr
=================================================
*/
	ND_ static String  AccessToStr (EAccessType type, EResourceState state) __Th___
	{
		String	str;
		switch_enum( type )
		{
			// requires GL_KHR_memory_scope_semantics
			case EAccessType::DeviceCoherent :		str << "devicecoherent";		break;
			case EAccessType::QueueFamilyCoherent :	str << "queuefamilycoherent";	break;
			case EAccessType::WorkgroupCoherent :	str << "workgroupcoherent";		break;
			case EAccessType::SubgroupCoherent :	str << "subgroupcoherent";		break;
			case EAccessType::NonPrivate :			str << "nonprivate";			break;

			case EAccessType::Volatile :			str << "volatile";	break;
			case EAccessType::Restrict :			str << "restrict";	break;
			case EAccessType::Coherent :			str << "coherent";	break;

			case EAccessType::Unknown :
			case EAccessType::_MemoryModel :
			case EAccessType::_Count :
			default :
				CHECK_THROW_MSG( false, "unknown access type" ); break;
		}
		switch_end

		switch ( ToEResState( state ))
		{
			case _EResState::ShaderStorage_Read :	str << " readonly";		break;
			case _EResState::ShaderStorage_Write :	str << " writeonly";	break;
		}
		return str;
	}

/*
=================================================
	SamplerDescToMSL
=================================================
*/
	ND_ static StringView  AddressMSL (EAddressMode value) __Th___
	{
		switch_enum( value )
		{
			case EAddressMode::Repeat :				return "repeat";
			case EAddressMode::MirrorRepeat :		return "mirrored_repeat";
			case EAddressMode::ClampToEdge :		return "clamp_to_edge";
			case EAddressMode::ClampToBorder :		return "clamp_to_border";
			case EAddressMode::MirrorClampToEdge :
			case EAddressMode::_Count :
			case EAddressMode::Unknown :			break;
		}
		switch_end
		CHECK_THROW_MSG( false, "unknown address mode" );
	}

	// Metal iOS 2.3+
	ND_ static StringView  BorderColorMSL (EBorderColor value) __Th___
	{
		switch_enum( value )
		{
			case EBorderColor::FloatTransparentBlack :
			case EBorderColor::IntTransparentBlack :	return "transparent_black";
			case EBorderColor::FloatOpaqueBlack :
			case EBorderColor::IntOpaqueBlack :			return "opaque_black";
			case EBorderColor::FloatOpaqueWhite :
			case EBorderColor::IntOpaqueWhite :			return "opaque_white";
			case EBorderColor::_Count :
			case EBorderColor::Unknown :				break;
		}
		switch_end
		CHECK_THROW_MSG( false, "unknown border color" );
	}

	ND_ static StringView  FilterMSL (EFilter value) __Th___
	{
		switch_enum( value )
		{
			case EFilter::Nearest :	return "nearest";
			case EFilter::Linear :	return "linear";
			case EFilter::_Count :
			case EFilter::Unknown :	break;
		}
		switch_end
		CHECK_THROW_MSG( false, "unknown filter" );
	}

	ND_ static StringView  FilterMSL (EMipmapFilter value) __Th___
	{
		switch_enum( value )
		{
			case EMipmapFilter::None :		return "none";
			case EMipmapFilter::Nearest :	return "nearest";
			case EMipmapFilter::Linear :	return "linear";
			case EMipmapFilter::_Count :
			case EMipmapFilter::Unknown :	break;
		}
		switch_end
		CHECK_THROW_MSG( false, "unknown mipmap filter" );
	}

	ND_ static StringView  CompareOpMSL (ECompareOp value) __Th___
	{
		switch_enum( value )
		{
			case ECompareOp::Never :	return "never";
			case ECompareOp::Less :		return "less";
			case ECompareOp::Equal :	return "equal";
			case ECompareOp::LEqual :	return "less_equal";
			case ECompareOp::Greater :	return "greater";
			case ECompareOp::NotEqual :	return "not_equal";
			case ECompareOp::GEqual :	return "greater_equal";
			case ECompareOp::Always :	return "always";
			case ECompareOp::_Count :
			case ECompareOp::Unknown :	break;
		}
		switch_end
		CHECK_THROW_MSG( false, "unknown compare op" );
	}

	ND_ static String  SamplerDescToMSL (const SamplerDesc &desc) __Th___
	{
		CHECK_THROW_MSG( desc.options == Default );
		CHECK_THROW_MSG( desc.reductionMode == EReductionMode::Average );
		CHECK_THROW_MSG( IsZero( desc.mipLodBias ));

		String	str;
		str << "  coord::" << (desc.UnnormalizedCoordinates() ? "pixel" : "normalized") << ",\n"
			<< "  s_address::" << AddressMSL( desc.addressMode.x ) << ",\n"
			<< "  t_address::" << AddressMSL( desc.addressMode.y ) << ",\n"
			<< "  r_address::" << AddressMSL( desc.addressMode.z ) << ",\n"
			<< "  border_color::" << BorderColorMSL( desc.borderColor ) << ",\n"
			<< "  mag_filter::" << FilterMSL( desc.magFilter ) << ",\n"
			<< "  min_filter::" << FilterMSL( desc.minFilter ) << ",\n"
			<< "  mip_filter::" << FilterMSL( desc.mipmapMode ) << ",\n"
			<< "  lod_clamp(" << ToString(desc.minLod) << ", " << ToString(desc.maxLod) << ")";

		if ( desc.compareOp.has_value() )
			str << ",\n  compare_func::" << CompareOpMSL( *desc.compareOp );

		if ( desc.HasAnisotropy() )
			str << ",\n  max_anisotropy(" << ToString( int(desc.maxAnisotropy) ) << ")";

		return str;
	}

/*
=================================================
	IsStd***
=================================================
*/
	ND_ static bool  IsStd140OrMetal (EStructLayout layout)
	{
		return AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140, EStructLayout::Metal );
	}

	ND_ static bool  IsStd430 (EStructLayout layout)
	{
		return AnyEqual( layout, EStructLayout::Compatible_Std430, EStructLayout::Std430 );
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	MSLBindings::operator ==
=================================================
*/
	bool  DescriptorSetLayout::MSLBindings::operator == (const MSLBindings &rhs) const
	{
		return	samplerIdx			== rhs.samplerIdx			and
				imtblSamplerCount	== rhs.imtblSamplerCount	and
				textureIdx			== rhs.textureIdx			and
				bufferIdx			== rhs.bufferIdx			and
				dsBinding			== rhs.dsBinding;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	DescriptorSetLayout::DescriptorSetLayout (const String &name) __Th___ :
		_features{ ObjectStorage::Instance()->GetDefaultFeatureSets() },
		_name{ name }
	{
		auto&	storage = *ObjectStorage::Instance();

		storage.AddName< DSLayoutName >( _name );
		CHECK_THROW_MSG( storage.dsLayouts.emplace( _name, DescriptorSetLayoutPtr{this} ).second );

		_dsLayout.name = DSLayoutName{_name};
	}

/*
=================================================
	ToGLSL
=================================================
*/
	void  DescriptorSetLayout::ToGLSL (EShaderStages stages, const uint dsBinding, INOUT String &outTypes, OUT String &outDecl, INOUT UniqueTypes_t &uniqueTypes) C_Th___
	{
		const auto	ArraySizeToStr = [] (ArraySize_t arraySize)
		{{
			return	arraySize == 0 ? " []"s	:
					arraySize == 1 ? ""s	: (" [" + ToString(arraySize) + ']');
		}};

		const auto	ImageToStr = [] (EImageType type, StringView typeName)
		{{
			String	str;
			switch ( type & EImageType::_ValMask )
			{
				case EImageType::Float :		break;
				case EImageType::Half :			break;
				case EImageType::SNorm :		break;
				case EImageType::UNorm :		break;
				case EImageType::Int :			str << "i";		break;
				case EImageType::UInt :			str << "u";		break;
				case EImageType::sRGB :			break;
				case EImageType::Depth :		break;
				case EImageType::Stencil :		break;
				case EImageType::DepthStencil :	break;
				case EImageType::SLong :		str << "i64";	break;
				case EImageType::ULong :		str << "u64";	break;
				default :						CHECK_MSG( false, "unknown image data type" );
			}
			str << typeName;
			switch ( type & EImageType::_TexMask )
			{
				case EImageType::Img1D :		str << "1D";		break;
				case EImageType::Img1DArray :	str << "1DArray";	break;
				case EImageType::Img2D :		str << "2D";		break;
				case EImageType::Img2DArray :	str << "2DArray";	break;
				case EImageType::Img2DMS :		str << "2DMS";		break;
				case EImageType::Img2DMSArray :	str << "2DMSArray";	break;
				case EImageType::ImgCube :		str << "Cube";		break;
				case EImageType::ImgCubeArray :	str << "CubeArray";	break;
				case EImageType::Img3D :		str << "3D";		break;
				case EImageType::Buffer :		str << "Buffer";	break;
				default :						CHECK_MSG( false, "unknown image dimension" );
			}
			switch ( type & EImageType::_QualMask )
			{
				case EImageType::Shadow :	str << "Shadow";	break;
				case EImageType::Unknown :	break;
				default :					CHECK_MSG( false, "unknown image qualifier type" );
			}
			return str;
		}};

		const auto	FormatToStr = [] (EPixelFormat fmt) -> StringView
		{{
			switch ( fmt )
			{
				case EPixelFormat::RGBA32F :		return "rgba32f";
				case EPixelFormat::RGBA16F :		return "rgba16f";
				case EPixelFormat::RG32F :			return "rg32f";
				case EPixelFormat::RG16F :			return "rg16f";
				case EPixelFormat::RGB_11_11_10F :	return "r11f_g11f_b10f";
				case EPixelFormat::R32F :			return "r32f";
				case EPixelFormat::R16F :			return "r16f";
				case EPixelFormat::RGBA16_UNorm :	return "rgba16";
				case EPixelFormat::RGB10_A2_UNorm :	return "rgb10_a2";
				case EPixelFormat::RGBA8_UNorm :	return "rgba8";
				case EPixelFormat::RG16_UNorm :		return "rg16";
				case EPixelFormat::RG8_UNorm :		return "rg8";
				case EPixelFormat::R16_UNorm :		return "r16";
				case EPixelFormat::R8_UNorm :		return "r8";
				case EPixelFormat::RGBA16_SNorm :	return "rgba16_snorm";
				case EPixelFormat::RGBA8_SNorm :	return "rgba8_snorm";
				case EPixelFormat::RG16_SNorm :		return "rg16_snorm";
				case EPixelFormat::RG8_SNorm :		return "rg8_snorm";
				case EPixelFormat::R16_SNorm :		return "r16_snorm";
				case EPixelFormat::R8_SNorm :		return "r8_snorm";
				case EPixelFormat::RGBA32I :		return "rgba32i";
				case EPixelFormat::RGBA16I :		return "rgba16i";
				case EPixelFormat::RGBA8I :			return "rgba8i";
				case EPixelFormat::RG32I :			return "rg32i";
				case EPixelFormat::RG16I :			return "rg16i";
				case EPixelFormat::RG8I :			return "rg8i";
				case EPixelFormat::R32I :			return "r32i";
				case EPixelFormat::R16I :			return "r16i";
				case EPixelFormat::R8I :			return "r8i";
				case EPixelFormat::RGBA32U :		return "rgba32ui";
				case EPixelFormat::RGBA16U :		return "rgba16ui";
				case EPixelFormat::RGBA8U :			return "rgba8ui";
				case EPixelFormat::RG32U :			return "rg32ui";
				case EPixelFormat::RG16U :			return "rg16ui";
				case EPixelFormat::RGB10_A2U :		return "rgb10_a2ui";
				case EPixelFormat::RG8U :			return "rg8ui";
				case EPixelFormat::R32U :			return "r32ui";
				case EPixelFormat::R16U :			return "r16ui";
				case EPixelFormat::R8U :			return "r8ui";
				case EPixelFormat::R64U :			return "r64ui";
				case EPixelFormat::R64I :			return "r64i";
			}
			CHECK_THROW_MSG( false, "unsupported pixel format" );
		}};

		CHECK_THROW_MSG( not _dsLayout.uniforms.empty() );
		CHECK_THROW_MSG( IsCompatibleWithVulkan() );

		if ( NoBits( _dsLayout.stages, stages ))
			return;

		auto&			storage			= *ObjectStorage::Instance();
		const String	ds_idx			= ToString( dsBinding );
		String&			str				= outDecl;
		EShaderStages	prev_stages		= Default;
		const bool		single_stage	= IsSingleBitSet( stages );

		for (auto& [name, un] : _dsLayout.uniforms)
		{
			const String	name_str = storage.GetName( name );

			CHECK_THROW_MSG( not name_str.empty() );
			CHECK_THROW_MSG( un.binding.IsVkDefined() );

			if ( NoBits( stages, un.stages ))
				continue;

			const String	idx_str		= ToString( un.binding.vkIndex );

			const AuxInfo*	aux_info	= null;
			auto			aux_info_it = _infoMap.find( name );
			if ( aux_info_it != _infoMap.end() )
				aux_info = &aux_info_it->second;

			ASSERT( un.stages != Default );
			if ( not single_stage and prev_stages != un.stages )
			{
				if ( prev_stages != Default )
					str << "#endif\n";
				str << "#if " << StagesToStr( un.stages ) << "\n";
				prev_stages = un.stages;
			}

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
				{
					CHECK_THROW_MSG( aux_info != null and aux_info->type );

					String	fields;
					aux_info->type->AddUsage( ShaderStructType::EUsage::BufferLayout );
					CHECK_THROW_MSG( aux_info->type->ToGLSL( true, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));

					str << "  // state: " << ToString( un.buffer.state )
						<< "\n  // size: " << ToString( un.buffer.staticSize );
					if ( un.buffer.HasDynamicOffset() )
						str << ", dynamic offset";
					str << "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ", std140) uniform AE_Type_" << aux_info->type->Typename() << " {\n"
						<< fields << "  } " << name_str << ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::StorageBuffer :
				{
					CHECK_THROW_MSG( aux_info != null and aux_info->type );

					String	fields;
					aux_info->type->AddUsage( ShaderStructType::EUsage::BufferLayout );
					CHECK_THROW_MSG( aux_info->type->ToGLSL( true, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));

					str << "  // state: " << ToString( un.buffer.state )
						<< "\n  // static size: " << ToString( un.buffer.staticSize )
						<< ", array stride: " << ToString( un.buffer.arrayStride );
					if ( un.buffer.HasDynamicOffset() )
						str << ", dynamic offset";
					str << "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ", std430) " << AccessToStr( aux_info->access, un.buffer.state )
						<< " buffer AE_Type_" << aux_info->type->Typename() << " {\n"
						<< fields << "  } " << name_str << ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::UniformTexelBuffer :
				{
					str << "  // state: " << ToString( un.texelBuffer.state )
						<< "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ") uniform " << ImageToStr( un.texelBuffer.type, "sampler" )
						<< ' ' << name_str << ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::StorageTexelBuffer :
				{
					CHECK_THROW_MSG( aux_info != null );
					str << "  // state: " << ToString( un.texelBuffer.state )
						<< "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ") "
						<< AccessToStr( aux_info->access, un.texelBuffer.state ) << " uniform " << ImageToStr( un.texelBuffer.type, "image" )
						<< ' ' << name_str << ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::StorageImage :
				{
					CHECK_THROW_MSG( aux_info != null );
					str << "  // state: " << ToString( un.image.state )
						<< "\n  layout(set=" << ds_idx << ", binding=" << idx_str;
					if ( un.image.format != Default )
						str << ", " << FormatToStr( un.image.format );
					str	<< ") " << AccessToStr( aux_info->access, un.image.state )
						<< " uniform " << ImageToStr( un.image.type, "image" ) << ' ' << name_str
						<< ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::SampledImage :
				{
					str << "  // state: " << ToString( un.image.state )
						<< "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ") uniform "
						<< ImageToStr( un.image.type, "texture" ) << ' ' << name_str
						<< ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::CombinedImage :
				case EDescriptorType::CombinedImage_ImmutableSampler :
				{
					str << "  // state: " << ToString( un.image.state );
					if ( un.type == EDescriptorType::CombinedImage_ImmutableSampler )
						str << ", immutable sampler";
					str	<< "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ") uniform "
						<< ImageToStr( un.image.type, "sampler" ) << ' ' << name_str
						<< ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::SubpassInput :
				{
					CHECK_THROW_MSG( un.image.subpassInputIdx != UMax, "'subpassInputIdx' is not valid" );
					String	tmp = ImageToStr( un.image.type, "_" );
					FindAndReplace( INOUT tmp, "_2D", "subpassInput" );

					str << "  // state: " << ToString( un.image.state )
						<< "\n  layout(set=" << ds_idx << ", binding=" << idx_str << ", input_attachment_index="
						<< ToString( un.image.subpassInputIdx ) << ") uniform " << tmp << ' ' << name_str
						<< ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::Sampler :
				case EDescriptorType::ImmutableSampler :
				{
					if ( un.type == EDescriptorType::ImmutableSampler )
						str << "  // immutable sampler\n";
					str << "  layout(set=" << ds_idx << ", binding=" << idx_str << ") uniform sampler "
						<< name_str << ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::RayTracingScene :
				{
					str	<< "  layout(set=" << ds_idx << ", binding=" << idx_str << ") uniform accelerationStructureEXT "
						<< name_str << ArraySizeToStr( un.arraySize ) << ";\n";
					break;
				}
				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					CHECK_THROW_MSG( false, "unknown descriptor type" );
			}
			switch_end
		}

		if ( not single_stage and prev_stages != Default )
			str << "#endif\n";
	}

/*
=================================================
	ToMSL
----
	https://developer.apple.com/documentation/metal/buffers/indexing_argument_buffers?language=objc
=================================================
*/
	void  DescriptorSetLayout::ToMSL (EShaderStages stages, INOUT MSLBindings &bindings, INOUT String &outTypes, OUT String &outDecl, INOUT UniqueTypes_t &uniqueTypes) C_Th___
	{
		CHECK_THROW_MSG( not _dsLayout.uniforms.empty() );
		CHECK_THROW_MSG( IsSingleBitSet( stages ));
		CHECK_THROW_MSG( IsCompatibleWithMetal() );

		if ( NoBits( _dsLayout.stages, stages ))
			return;

		const auto	ValTypeToStr = [] (EImageType type) -> StringView
		{{
			switch ( type & EImageType::_ValMask )
			{
				case EImageType::Depth :
				case EImageType::Float :
				case EImageType::sRGB :
				case EImageType::SNorm :
				case EImageType::UNorm :	return "float";
				case EImageType::Half :		return "half";
				case EImageType::Int :		return "int";
				case EImageType::UInt :		return "uint";
				default :					CHECK(false);
			}
			return "";
		}};

		const auto	ImageToStr = [] (EImageType type) -> StringView
		{{
			const bool	is_depth = ((type & EImageType::_QualMask) == EImageType::Shadow);
			switch ( type & EImageType::_TexMask )
			{
				case EImageType::Img1D :		CHECK(not is_depth);  return "texture1d";
				case EImageType::Img1DArray :	CHECK(not is_depth);  return "texture1d_array";
				case EImageType::Img2D :		return is_depth ? "depth2d"			: "texture2d";
				case EImageType::Img2DArray :	return is_depth ? "depth2d_array"	: "texture2d_array";
				case EImageType::Img2DMS :		return is_depth ? "depth2d_ms"		: "texture2d_ms";
				case EImageType::Img2DMSArray :	return is_depth ? "depth2d_ms_array": "texture2d_ms_array";
				case EImageType::ImgCube :		return is_depth ? "depthcube"		: "texturecube";
				case EImageType::ImgCubeArray :	return is_depth ? "depthcube_array"	: "texturecube_array";
				case EImageType::Img3D :		CHECK(not is_depth);  return "texture3d";
				case EImageType::Buffer :		CHECK(not is_depth);  return "texture_buffer";
				default :						CHECK(false);
			}
			return "";
		}};

		const auto	StateToAccessStr = [] (EResourceState state) -> StringView
		{{
			switch ( ToEResState( state )) {
				case _EResState::ShaderSample :			return "access::sample";
				case _EResState::ShaderStorage_Read :	return "access::read";
				case _EResState::ShaderStorage_Write :	return "access::write";
				case _EResState::ShaderStorage_RW :		return "access::read_write";
				default :								CHECK(false);
			}
			return "";
		}};

		auto&				storage			= *ObjectStorage::Instance();
		const MSLBindings	src_bindings	= bindings;
		const bool			is_argbuf		= AllBits( _dsLayout.usage, EDescSetUsage::ArgumentBuffer );
		String				s_decl;

		for (auto& [name, un] : _dsLayout.uniforms)
		{
			const String	name_str = storage.GetName( name );

			CHECK_THROW_MSG( un.arraySize > 0 );			// TODO
			CHECK_THROW_MSG( not name_str.empty() );
			CHECK_THROW_MSG( un.binding.IsMetalDefined() );
			ASSERT( un.stages != Default );

			const ubyte*	index_ptr = null;

			// argument buffer must have same layout
			if ( not is_argbuf )
			{
				if ( NoBits( stages, un.stages ))
					continue;

				index_ptr = un.binding.mtlPerStageIndex.PtrForShader( stages );
				CHECK_THROW_MSG( index_ptr != null, "unsupported shader stage for Metal" );
			}

			const String	idx_str		= "[[id("s << ToString( un.binding.mtlIndex ) << ")]]";

			const AuxInfo*	aux_info	= null;
			auto			aux_info_it = _infoMap.find( name );
			if ( aux_info_it != _infoMap.end() )
				aux_info = &aux_info_it->second;

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
				{
					CHECK_THROW_MSG( aux_info != null and aux_info->type );

					if ( uniqueTypes.insert( String{aux_info->type->Typename()} ).second )
					{
						aux_info->type->AddUsage( ShaderStructType::EUsage::BufferLayout );
						CHECK_THROW_MSG( aux_info->type->ToMSL( INOUT outTypes, INOUT uniqueTypes ));
					}
					s_decl	<< "  /* state: " << ToString( un.buffer.state ) << " */\n"
							<< "  /* size: " << ToString( un.buffer.staticSize );
					if ( is_argbuf )
					{
						CHECK_THROW_MSG( not un.buffer.HasDynamicOffset() );
						s_decl	<< " */\n"
								<< "  " << aux_info->type->Typename() << " " << name_str
								<< ' ' << idx_str << ' '
								<< (un.arraySize > 1 ? ('[' + ToString( un.arraySize ) + ']') : ""s) << ";\n";
					}
					else
					{
						if ( un.buffer.HasDynamicOffset() )
							s_decl << ", dynamic offset";
						s_decl	<< " */\n"
								<< "  constant " << aux_info->type->Typename() << " " << name_str << " [[buffer(" << ToString( src_bindings.bufferIdx + *index_ptr ) << ")]] "
								<< (un.arraySize > 1 ? ('[' + ToString( un.arraySize ) + ']') : ""s) << ",\n";
						bindings.bufferIdx += un.arraySize;
					}
					break;
				}
				case EDescriptorType::StorageBuffer :
				{
					CHECK_THROW_MSG( aux_info != null and aux_info->type );

					if ( uniqueTypes.insert( String{aux_info->type->Typename()} ).second )
					{
						aux_info->type->AddUsage( ShaderStructType::EUsage::BufferLayout );
						CHECK_THROW_MSG( aux_info->type->ToMSL( INOUT outTypes, INOUT uniqueTypes ));
					}
					s_decl	<< "  /* state: " << ToString( un.buffer.state ) << " */\n"
							<< "  /* static size: " << ToString( un.buffer.staticSize )
							<< ", array stride: " << ToString( un.buffer.arrayStride );
					if ( un.buffer.HasDynamicOffset() )
					{
						CHECK_THROW_MSG( not is_argbuf );
						s_decl << ", dynamic offset";
					}
					s_decl	<< " */\n"
							<< "  " << (EResourceState_IsReadOnly( un.buffer.state ) ? "const " : "") << "device " << aux_info->type->Typename()
							<< (un.arraySize == 0 ? "* " : " ") << name_str << ' '
							<< (is_argbuf ? idx_str : ("[[buffer(" + ToString( src_bindings.bufferIdx + *index_ptr ) + ")]]"))
							<< (un.arraySize > 1 ? (" ["s + ToString(un.arraySize) + "]") : ""s)
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.bufferIdx += un.arraySize;
					break;
				}
				case EDescriptorType::UniformTexelBuffer :
				{
					CHECK( EResourceState_IsReadOnly( un.texelBuffer.state ));
					s_decl	<< "  /* state: " << ToString( un.texelBuffer.state ) << " */\n"
							<< "  " << (un.arraySize > 1 ? "array< "s : ""s)
							<< "texture_buffer< " << ValTypeToStr( un.texelBuffer.type ) << ", access::read >"
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s)
							<< name_str << ' '
							<< (is_argbuf ? idx_str : ("[[texture(" + ToString( src_bindings.textureIdx + *index_ptr ) << ")]]"))
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.textureIdx += un.arraySize;
					break;
				}
				case EDescriptorType::StorageTexelBuffer :
				{
					CHECK_THROW_MSG( aux_info != null );
					s_decl	<< "  /* state: " << ToString( un.texelBuffer.state ) << " */\n"
							<< "  " << (un.arraySize > 1 ? "array< "s : ""s)
							<< "texture_buffer< " << ValTypeToStr( un.texelBuffer.type ) << ", " << StateToAccessStr( un.texelBuffer.state ) << " >"
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s) << name_str << ' '
							<< (is_argbuf ? idx_str : ("[[texture(" + ToString( src_bindings.textureIdx + *index_ptr ) << ")]]"))
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.textureIdx += un.arraySize;
					break;
				}
				case EDescriptorType::StorageImage :
				{
					CHECK_THROW_MSG( aux_info != null );
					s_decl	<< "  /* state: " << ToString( un.image.state ) << " */\n"
							<< "  " << (un.arraySize > 1 ? "array< "s : ""s)
							<< ImageToStr( un.image.type ) << "< " << ValTypeToStr( un.image.type ) << ", " << StateToAccessStr( un.image.state ) << " >"
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s) << name_str << ' '
							<< (is_argbuf ? idx_str : ("[[texture(" + ToString( src_bindings.textureIdx + *index_ptr ) << ")]]"))
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.textureIdx += un.arraySize;
					break;
				}
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage_ImmutableSampler :
				{
					s_decl	<< "  /* state: " << ToString( un.image.state ) << " */\n"
							<< "  " << (un.arraySize > 1 ? "array< "s : ""s)
							<< ImageToStr( un.image.type ) << "< " << ValTypeToStr( un.image.type ) << ", access::sample >"
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s) << name_str << ' '
							<< (is_argbuf ? idx_str : ("[[texture(" + ToString( src_bindings.textureIdx + *index_ptr ) << ")]]"))
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.textureIdx += un.arraySize;
					break;
				}
				case EDescriptorType::CombinedImage :
				{
					CHECK_THROW_MSG( is_argbuf, "combined image type is not supported in MSL outside of argument buffer" );

					s_decl	<< "  /* state: " << ToString( un.image.state ) << " */\n"
							<< "  " << (un.arraySize > 1 ? "array< "s : ""s)
							<< ImageToStr( un.image.type ) << "< " << ValTypeToStr( un.image.type ) << ", access::sample >"
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s) << name_str << ' '
							<<  idx_str << ";\n";

					s_decl	<< "  " << (un.arraySize > 1 ? "array< "s : ""s) << "sampler " << name_str
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s) << name_str << ' '
							<< "[[id("s << ToString( un.binding.mtlIndex + un.arraySize ) << ")]];\n";
					break;
				}
				case EDescriptorType::SubpassInput :
				{
					CHECK_THROW_MSG( false, "subpass input type is not supported in MSL" );	// TODO
					break;
				}
				case EDescriptorType::Sampler :
				{
					s_decl	<< "  " << (un.arraySize > 1 ? "array< "s : ""s) << "sampler " << name_str
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s) << name_str << ' '
							<< (is_argbuf ? idx_str : ("[[sampler(" + ToString( src_bindings.samplerIdx + *index_ptr ) << ")]]"))
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.samplerIdx += un.arraySize;
					break;
				}
				case EDescriptorType::ImmutableSampler :
				{
					CHECK_THROW_MSG( un.arraySize == 1 );
					CHECK_THROW_MSG( un.binding.mtlIndex == DescriptorSetLayoutDesc::UnassignedIdx );

					const String	samp_name	= storage.GetName( _dsLayout.samplerStorage[ un.immutableSampler.offsetInStorage ]);
					auto			iter		= storage.samplerMap.find( samp_name );
					CHECK_THROW_MSG( iter != storage.samplerMap.end(),
						"Sampler '"s << samp_name << "' is not exist in storage" );

					const auto&	samp = storage.samplerRefs[ iter->second ]->Desc();

					outTypes << "constexpr sampler " << name_str << " (\n"
							 << SamplerDescToMSL( samp ) << "\n);\n";

					bindings.imtblSamplerCount += un.arraySize;
					break;
				}
				case EDescriptorType::RayTracingScene :
				{
					s_decl	<< "  " << (un.arraySize > 1 ? "array< "s : ""s)
							<< "instance_acceleration_structure"				// TODO: primitive_acceleration_structure
							<< (un.arraySize > 1 ? (", "s + ToString(un.arraySize) + " > ") : " "s)
							<< name_str << ' '
							<< (is_argbuf ? idx_str : ("[[buffer(" + ToString( src_bindings.bufferIdx + *index_ptr ) << ")]]"))
							<< (is_argbuf ? ";" : ",") << "\n";
					if ( not is_argbuf ) bindings.bufferIdx += un.arraySize;
					break;
				}
				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					CHECK_THROW_MSG( false, "unknown descriptor type" );
			}
			switch_end
		}

		if ( is_argbuf )
		{
			// available in:
			//	iOS    11.0
			//	macOS  10.13
			//	tvOS   11.0

			const String	ab_type = "ArgBuf"s << _name << "Type";

			outTypes << "struct " << ab_type << "\n{\n" << s_decl << "};\n\n";
			outDecl  << "  " << (AllBits( _dsLayout.usage, EDescSetUsage::MutableArgBuffer ) ? "device" : "constant")
					 << " " << ab_type << "& descSet" << ToString(bindings.dsBinding) << " [[buffer(" << ToString(bindings.bufferIdx) << ")]],\n";
			++bindings.bufferIdx;
		}
		else
		{
			outDecl << s_decl;
		}
		++bindings.dsBinding;
	}

/*
=================================================
	CountMSLBindings
----
	descriptor calculation must be the same as in 'ToMSL()'
=================================================
*/
	bool  DescriptorSetLayout::CountMSLBindings (EShaderStages stages, INOUT MSLBindings &bindings) C_NE___
	{
		CHECK_ERR( not _dsLayout.uniforms.empty() );
		CHECK_ERR( IsSingleBitSet( stages ));
		CHECK_ERR( IsCompatibleWithMetal() );

		const bool	is_argbuf = AllBits( _dsLayout.usage, EDescSetUsage::ArgumentBuffer );

		for (auto& [name, un] : _dsLayout.uniforms)
		{
			CHECK_ERR( un.arraySize > 0 );			// TODO

			// argument buffer must have same layout
			if ( not is_argbuf and NoBits( stages, un.stages ))
				continue;

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
				{
					if ( is_argbuf )	CHECK_ERR( not un.buffer.HasDynamicOffset() )
					else				bindings.bufferIdx += un.arraySize;
					break;
				}
				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :
				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage_ImmutableSampler :
				{
					if ( not is_argbuf ) bindings.textureIdx += un.arraySize;
					break;
				}
				case EDescriptorType::CombinedImage :
				{
					CHECK_ERR_MSG( is_argbuf, "combined image type is not supported in MSL outside of argument buffer" );
					break;
				}
				case EDescriptorType::SubpassInput :
				{
					RETURN_ERR( "subpass input type is not supported in MSL" );	// TODO
					break;
				}
				case EDescriptorType::Sampler :
				{
					if ( not is_argbuf ) bindings.samplerIdx += un.arraySize;
					break;
				}
				case EDescriptorType::ImmutableSampler :
				{
					bindings.imtblSamplerCount += un.arraySize;
					break;
				}
				case EDescriptorType::RayTracingScene :
				{
					if ( not is_argbuf ) bindings.bufferIdx += un.arraySize;
					break;
				}
				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					RETURN_ERR( "unknown descriptor type" );
			}
			switch_end

			CHECK_ERR( un.binding.IsMetalDefined() );
		}

		if ( is_argbuf )
			++bindings.bufferIdx;

		++bindings.dsBinding;

		return true;
	}

/*
=================================================
	IsCompatibleWith***
=================================================
*/
	bool  DescriptorSetLayout::IsCompatibleWithVulkan () const
	{
		ASSERT( _dsLayout.stages != Default );
		return true;
	}

	bool  DescriptorSetLayout::IsCompatibleWithMetal () const
	{
		ASSERT( _dsLayout.stages != Default );
		return AnyBits( _dsLayout.stages, s_MetalStages );
	}

/*
=================================================
	Build
=================================================
*/
	bool  DescriptorSetLayout::Build () __NE___
	{
		if ( _uid.has_value() )
			return true;

		CHECK_ERR_MSG( not _dsLayout.uniforms.empty(),
			"DescriptorSetLayout '"s << _name << "' is empty" );

		ScriptFeatureSet::Minimize( INOUT _features );

		const bool						is_argbuf		= AllBits( _dsLayout.usage, EDescSetUsage::ArgumentBuffer );
		StaticArray< MSLBindings, 3 >	msl_per_stage	= {};
		usize							vk_binding		= 0;
		usize							msl_binding		= 0;
		auto&							storage			= *ObjectStorage::Instance();

		_dsLayout.stages = Default;
		for (auto& [name, un] : _dsLayout.uniforms)
		{
			_dsLayout.stages |= un.stages;
		}

		_dsLayout.features = storage.CopyFeatures( _features );

		// validate
		{
			const bool	is_compute		= AllBits( _dsLayout.stages, EShaderStages::Compute );
			const bool	is_tile			= AllBits( _dsLayout.stages, EShaderStages::Tile );
			const bool	is_graphics		= AnyBits( _dsLayout.stages, EShaderStages::AllGraphics );
			const bool	is_ray_tracing	= AnyBits( _dsLayout.stages, EShaderStages::AllRayTracing );

			CHECK_ERR( (is_compute + is_tile + is_graphics + is_ray_tracing) == 1 );
		}

		for (auto& [name, un] : _dsLayout.uniforms)
		{
			CHECK( not un.binding.IsVkDefined() );
			CHECK( not un.binding.IsMetalDefined() );

			// Vulkan
			if ( IsCompatibleWithVulkan() )
			{
				CHECK_ERR( vk_binding < MaxValue<BindingIndex_t>() );
				un.binding.vkIndex = BindingIndex_t(vk_binding++);
			}

			// Metal
			if ( IsCompatibleWithMetal() )
			{
				CHECK_ERR( un.arraySize != 0 );	// TODO: set uniform array size in runtime is not supported for Metal

				if ( is_argbuf )
				{
					CHECK_ERR( msl_binding < MaxValue<BindingIndex_t>() );

					un.binding.mtlIndex = BindingIndex_t(msl_binding);

					switch_enum( un.type )
					{
						case EDescriptorType::UniformBuffer :
						{
							auto	aux_info_it = _infoMap.find( name );
							CHECK_ERR( aux_info_it != _infoMap.end() );

							const AuxInfo*	aux_info = &aux_info_it->second;
							CHECK_ERR( aux_info->type );

							usize	fcount = 0;
							CHECK_ERR( aux_info->type->FieldCount( INOUT fcount ));

							msl_binding += fcount;
							break;
						}
						case EDescriptorType::StorageBuffer :
						case EDescriptorType::RayTracingScene :
						case EDescriptorType::UniformTexelBuffer :
						case EDescriptorType::StorageTexelBuffer :
						case EDescriptorType::StorageImage :
						case EDescriptorType::SampledImage :
						case EDescriptorType::CombinedImage_ImmutableSampler :
						case EDescriptorType::Sampler :
							msl_binding += un.arraySize;
							break;

						case EDescriptorType::ImmutableSampler :
							un.binding.mtlIndex = DescriptorSetLayoutDesc::UnassignedIdx;
							break;

						case EDescriptorType::CombinedImage :
							msl_binding += un.arraySize * 2;
							break;

						case EDescriptorType::SubpassInput :
							un.binding.mtlIndex = DescriptorSetLayoutDesc::InvalidIdx;
							break;	// skip

						case EDescriptorType::_Count :
						case EDescriptorType::Unknown :
						default :						RETURN_ERR( "unknown descriptor type" );
					}
					switch_end
				}
				else
				for (auto shader : BitIndexIterate<EShader>( un.stages & s_MetalStages ))
				{
					const int	idx = MetalBindingPerStage::ShaderToIndex( shader );
					CHECK_ERR_MSG( idx >= 0, "unsupported shader stage for Metal" );

					MSLBindings*	dst_binding	= &msl_per_stage[0];
					ubyte *			dst_index	= &un.binding.mtlPerStageIndex.GetRef(idx);

					switch_enum( un.type )
					{
						// buffer
						case EDescriptorType::UniformBuffer :
						case EDescriptorType::StorageBuffer :
						case EDescriptorType::RayTracingScene :
							CHECK_ERR( (dst_binding->bufferIdx + un.arraySize) <= MaxValue< decltype(*dst_index) >() );
							*dst_index = ubyte(dst_binding->bufferIdx);
							dst_binding->bufferIdx += un.arraySize;
							break;

						// texture
						case EDescriptorType::UniformTexelBuffer :
						case EDescriptorType::StorageTexelBuffer :
						case EDescriptorType::StorageImage :
						case EDescriptorType::SampledImage :
						case EDescriptorType::CombinedImage_ImmutableSampler :
							CHECK_ERR( (dst_binding->textureIdx + un.arraySize) <= MaxValue< decltype(*dst_index) >() );
							*dst_index = ubyte(dst_binding->textureIdx);
							dst_binding->textureIdx += un.arraySize;
							break;

						// sampler
						case EDescriptorType::Sampler :
							CHECK_ERR( (dst_binding->samplerIdx + un.arraySize) <= MaxValue< decltype(*dst_index) >() );
							*dst_index = ubyte(dst_binding->samplerIdx);
							dst_binding->samplerIdx += un.arraySize;
							break;

						case EDescriptorType::ImmutableSampler :
							un.binding.mtlIndex = DescriptorSetLayoutDesc::UnassignedIdx;
							break;	// skip

						case EDescriptorType::SubpassInput :
						case EDescriptorType::CombinedImage :
							un.binding.mtlIndex = DescriptorSetLayoutDesc::InvalidIdx;
							break;	// skip

						case EDescriptorType::_Count :
						case EDescriptorType::Unknown :
						default :									RETURN_ERR( "unknown descriptor type" );
					}
					switch_end
				}
			}
		}

		DescriptorCount		total		= {};
		PerStageDescCount_t	per_stage	= {};
		CountDescriptors( INOUT total, INOUT per_stage );
		CHECK_ERR( CheckDescriptorLimits( total, per_stage, _features, ("In DescriptorSetLayout '"s << _name << "'") ));

		_uid = storage.pplnStorage->AddDescriptorSetLayout( _dsLayout );
		return true;
	}

/*
=================================================
	CountDescriptors
=================================================
*/
	void  DescriptorCount::Add (const DescriptorSetLayoutDesc::Uniform &un)
	{
		const uint	count = Max( 1u, un.arraySize );	// can not count runtime sized array

		switch_enum( un.type )
		{
			case EDescriptorType::UniformBuffer :
				(un.buffer.HasDynamicOffset() ? dynamicUniformBuffers : uniformBuffers) += count;		break;

			case EDescriptorType::StorageBuffer :
				(un.buffer.HasDynamicOffset() ? dynamicStorageBuffers : storageBuffers) += count;		break;

			case EDescriptorType::StorageTexelBuffer :
			case EDescriptorType::StorageImage :
				storageImages += count;				break;

			case EDescriptorType::UniformTexelBuffer :
			case EDescriptorType::SampledImage :
			case EDescriptorType::CombinedImage :
				sampledImages += count;				break;

			case EDescriptorType::CombinedImage_ImmutableSampler :
				sampledImages	+= count;
				samplers		+= count;			break;

			case EDescriptorType::SubpassInput :
				subpassInputs += count;				break;

			case EDescriptorType::Sampler :
			case EDescriptorType::ImmutableSampler :
				samplers += count;					break;

			case EDescriptorType::RayTracingScene :
				rayTracingScenes += count;			break;

			case EDescriptorType::_Count :
			case EDescriptorType::Unknown :
				break;
		}
		switch_end
	}

	void  DescriptorSetLayout::CountDescriptors (INOUT DescriptorCount &total, INOUT PerStageDescCount_t &perStage) C_NE___
	{
		for (auto& [name, un] : _dsLayout.uniforms)
		{
			total.Add( un );

			for (auto shader : BitIndexIterate<EShader>( un.stages ))
			{
				perStage(shader).Add( un );
			}
		}
	}

/*
=================================================
	Bind
=================================================
*/
	void  DescriptorSetLayout::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<DescriptorSetLayout>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Create descriptor set layout.\n"
						"Name may be used in C++ code to create descriptor set." );
		binder.AddFactoryCtor( &DescriptorSetLayout_Ctor, {"name"} );

		binder.Comment( "Add FeatureSet to the descriptor set and all dependent resources." );
		binder.AddMethod( &DescriptorSetLayout::AddFeatureSet,					"AddFeatureSet",	{"fsName"} );

		binder.Comment( "Add macros which will be used in shader.\n"
						"Format: MACROS = value \\n DEF \\n ..." );
		binder.AddMethod( &DescriptorSetLayout::Define,							"Define",			{} );

		binder.Comment( "Set descriptor set usage (EDescSetUsage)." );
		binder.AddMethod( &DescriptorSetLayout::SetUsage,						"SetUsage",			{} );
		binder.AddMethod( &DescriptorSetLayout::SetUsage2,						"SetUsage",			{} );

		binder.Comment( "Add input attachment from render technique graphics pass." );
		binder.AddMethod( &DescriptorSetLayout::AddSubpassInputFromRenderTech,	"SubpassInputFromRenderTech", {"rtech", "gpass"} );

		binder.Comment( "Add input attachment from render pass subpass." );
		binder.AddMethod( &DescriptorSetLayout::AddSubpassInputFromRenderPass,	"SubpassInputFromRenderPass", {"compatRP", "subpass"} );

		binder.Comment( "Add uniform buffer." );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &)					>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &)								>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EResourceState)	>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EResourceState)				>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &)										>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const String &)												>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EResourceState)						>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EResourceState )								>( &DescriptorSetLayout::_AddUniformBuffer, "UniformBuffer", {"shaderStages", "uniform", "typeName", "state"} );

		binder.Comment( "Add dynamic uniform buffer." );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &)					>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &)								>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EResourceState)	>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EResourceState)				>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &)										>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const String &)												>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EResourceState)						>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EResourceState)								>( &DescriptorSetLayout::_AddUniformBufferDynamic, "UniformBufferDynamic", {"shaderStages", "uniform", "typeName", "state"} );

		binder.Comment( "Add storage buffer." );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &)								>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &)											>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EResourceState)				>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EResourceState)							>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &)													>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const String &)															>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EResourceState)									>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EResourceState)											>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EAccessType)					>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EAccessType)							>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EAccessType, EResourceState)	>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EAccessType, EResourceState)			>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "arraySize", "typeName", "access", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EAccessType)										>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EAccessType)												>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EAccessType, EResourceState)						>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EAccessType, EResourceState)								>( &DescriptorSetLayout::_AddStorageBuffer, "StorageBuffer", {"shaderStages", "uniform", "typeName", "access", "state"} );

		binder.Comment( "Add dynamic storage buffer." );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &)								>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &)											>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EResourceState)				>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EResourceState)							>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &)													>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (uint, const String &, const String &)															>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EResourceState)									>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EResourceState)											>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EAccessType)					>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EAccessType)							>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, const String &, EAccessType, EResourceState)	>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, const String &, EAccessType, EResourceState)			>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "arraySize", "typeName", "access", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EAccessType)										>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EAccessType)												>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &, EAccessType, EResourceState)						>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const String &, EAccessType, EResourceState)								>( &DescriptorSetLayout::_AddStorageBufferDynamic, "StorageBufferDynamic", {"shaderStages", "uniform", "typeName", "access", "state"} );

		binder.Comment( "Add uniform (sampled) texel buffer.\n"
						"'imageType' must be 'Buffer | Int/Uint/Float'" );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType)										>( &DescriptorSetLayout::_AddUniformTexelBuffer, "UniformTexelBuffer", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType)												>( &DescriptorSetLayout::_AddUniformTexelBuffer, "UniformTexelBuffer", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType)					>( &DescriptorSetLayout::_AddUniformTexelBuffer, "UniformTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType)								>( &DescriptorSetLayout::_AddUniformTexelBuffer, "UniformTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState)	>( &DescriptorSetLayout::_AddUniformTexelBuffer, "UniformTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState)				>( &DescriptorSetLayout::_AddUniformTexelBuffer, "UniformTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );

		binder.Comment( "Add storage texel buffer.\n"
						"'imageType' must be 'Buffer | Int/Uint/Float'.\n"
						"'format' must be included in 'storageImageFormats' in at least one of feature set." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EPixelFormat)													>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "format"} );
		binder.AddGenericMethod< void (uint, const String &, EPixelFormat)															>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "format"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EPixelFormat)								>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EPixelFormat)										>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EPixelFormat, EResourceState)				>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EPixelFormat, EResourceState)						>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EPixelFormat, EAccessType)									>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "format", "access"} );
		binder.AddGenericMethod< void (uint, const String &, EPixelFormat, EAccessType)												>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "format", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EPixelFormat, EAccessType)					>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EPixelFormat, EAccessType)							>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EPixelFormat, EAccessType, EResourceState)	>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EPixelFormat, EAccessType, EResourceState)			>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "format", "access", "state"} );

		binder.Comment( "Add storage texel buffer.\n"
						"'imageType' must be 'Buffer | Int/Uint/Float'.\n"
						"Requires 'shaderStorageImageReadWithoutFormat' or 'shaderStorageImageWriteWithoutFormat' feature." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType)													>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType)															>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType)								>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType)											>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState)				>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState)							>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EAccessType)										>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "imageType", "access"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EAccessType)												>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "imageType", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EAccessType)					>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EAccessType)							>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EAccessType, EResourceState)	>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EAccessType, EResourceState)			>( &DescriptorSetLayout::_AddStorageTexelBuffer, "StorageTexelBuffer", {"shaderStages", "uniform", "arraySize", "imageType", "access", "state"} );

		binder.Comment( "Add storage image.\n"
						"'format' must be included in 'storageImageFormats' in at least one of feature set." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EPixelFormat)													>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EPixelFormat)															>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EPixelFormat, EAccessType)									>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format", "access"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EPixelFormat, EAccessType)												>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EPixelFormat, EResourceState)									>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EPixelFormat, EResourceState)											>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EPixelFormat, EAccessType, EResourceState)					>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EPixelFormat, EAccessType, EResourceState)								>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "format", "access", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EPixelFormat)								>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EPixelFormat)										>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EPixelFormat, EAccessType)					>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EPixelFormat, EAccessType)							>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EPixelFormat, EResourceState)				>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EPixelFormat, EResourceState)						>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EPixelFormat, EAccessType, EResourceState)	>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EPixelFormat, EAccessType, EResourceState)			>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "format", "access", "state"} );

		binder.Comment( "Add storage image.\n"
						"Requires 'shaderStorageImageReadWithoutFormat' or 'shaderStorageImageWriteWithoutFormat' feature." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType)													>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType)															>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EAccessType)										>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "access"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EAccessType)												>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EResourceState)									>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EResourceState)											>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EAccessType, EResourceState)						>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EAccessType, EResourceState)								>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "imageType", "access", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType)								>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType)											>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EAccessType)					>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "access"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EAccessType)							>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "access"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState)				>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState)							>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EAccessType, EResourceState)	>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "access", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EAccessType, EResourceState)			>( &DescriptorSetLayout::_AddStorageImage, "StorageImage", {"shaderStages", "uniform", "arraySize", "imageType", "access", "state"} );

		binder.Comment( "Add sampled image (without sampler)." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType)										>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType)												>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType)					>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType)								>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EResourceState)						>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EResourceState)								>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState)	>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState)				>( &DescriptorSetLayout::_AddSampledImage, "SampledImage", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );

		binder.Comment( "Add sampled image with sampler." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType)										>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType)												>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType)					>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType)								>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EResourceState)						>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EResourceState)								>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState)	>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState)				>( &DescriptorSetLayout::_AddCombinedImage, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "state"} );

		binder.Comment( "Add sampled image with immutable sampler." );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, const String &)												>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, const String &)														>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, const String &)							>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, const String &)										>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EResourceState, const String &)								>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EResourceState, const String &)										>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState, const String &)			>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState, const String &)						>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, const ScriptArray<String> &)									>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, const ScriptArray<String> &)											>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, const ScriptArray<String> &)				>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, const ScriptArray<String> &)						>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EResourceState, const ScriptArray<String> &)					>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EResourceState, const ScriptArray<String> &)							>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &, EImageType, EResourceState, const ScriptArray<String>&)>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "state", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &, EImageType, EResourceState, const ScriptArray<String> &)		>( &DescriptorSetLayout::_AddCombinedImage_ImmutableSampler, "CombinedImage", {"shaderStages", "uniform", "arraySize", "imageType", "state", "samplerName"} );

		binder.Comment( "Add input attachment." );
		binder.AddGenericMethod< void (EShaderStages, const String &)									>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform"} );
		binder.AddGenericMethod< void (uint, const String &)											>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType)						>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType)								>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EResourceState)					>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EResourceState)							>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, EImageType, EResourceState)		>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, EImageType, EResourceState)				>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "imageType", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, uint)								>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex"} );
		binder.AddGenericMethod< void (uint, const String &, uint)										>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, uint, EImageType)					>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex", "imageType"} );
		binder.AddGenericMethod< void (uint, const String &, uint, EImageType)							>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex", "imageType"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, uint, EResourceState)				>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex", "state"} );
		binder.AddGenericMethod< void (uint, const String &, uint, EResourceState)						>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex", "state"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, uint, EImageType, EResourceState)	>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex", "imageType", "state"} );
		binder.AddGenericMethod< void (uint, const String &, uint, EImageType, EResourceState)			>( &DescriptorSetLayout::_AddSubpassInput, "SubpassInput", {"shaderStages", "uniform", "inputAttachmentIndex", "imageType", "state"} );

		binder.Comment( "Add separate sampler." );
		binder.AddGenericMethod< void (EShaderStages, const String &)						>( &DescriptorSetLayout::_AddSampler, "Sampler", {"shaderStages", "uniform"} );
		binder.AddGenericMethod< void (uint, const String &)								>( &DescriptorSetLayout::_AddSampler, "Sampler", {"shaderStages", "uniform"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &)	>( &DescriptorSetLayout::_AddSampler, "Sampler", {"shaderStages", "uniform", "arraySize"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &)				>( &DescriptorSetLayout::_AddSampler, "Sampler", {"shaderStages", "uniform", "arraySize"} );

		binder.Comment( "Add immutable sampler." );
		binder.AddGenericMethod< void (EShaderStages, const String &, const String &)				>( &DescriptorSetLayout::_AddImmutableSampler, "ImtblSampler", {"shaderStages", "uniform", "samplerName"} );
		binder.AddGenericMethod< void (uint, const String &, const String &)						>( &DescriptorSetLayout::_AddImmutableSampler, "ImtblSampler", {"shaderStages", "uniform", "samplerName"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ScriptArray<String> &)	>( &DescriptorSetLayout::_AddImmutableSampler, "ImtblSampler", {"shaderStages", "uniform", "samplerNames"} );
		binder.AddGenericMethod< void (uint, const String &, const ScriptArray<String> &)			>( &DescriptorSetLayout::_AddImmutableSampler, "ImtblSampler", {"shaderStages", "uniform", "samplerNames"} );

		binder.Comment( "Add ray tracing scene (top level acceleration structure)." );
		binder.AddGenericMethod< void (EShaderStages, const String &)						>( &DescriptorSetLayout::_AddRayTracingScene, "RayTracingScene", {"shaderStages", "uniform"} );
		binder.AddGenericMethod< void (uint, const String &)								>( &DescriptorSetLayout::_AddRayTracingScene, "RayTracingScene", {"shaderStages", "uniform"} );
		binder.AddGenericMethod< void (EShaderStages, const String &, const ArraySize &)	>( &DescriptorSetLayout::_AddRayTracingScene, "RayTracingScene", {"shaderStages", "uniform", "arraySize"} );
		binder.AddGenericMethod< void (uint, const String &, const ArraySize &)				>( &DescriptorSetLayout::_AddRayTracingScene, "RayTracingScene", {"shaderStages", "uniform", "arraySize"} );

		binder.Comment( "Check is image description is supported by feature set." );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, MultiSamples const&) >( &DescriptorSetLayout::_IsImageSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "samples"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, MultiSamples const&)					 >( &DescriptorSetLayout::_IsImageSupported,	"IsSupported",	{"format", "usage", "options", "samples"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&)						 >( &DescriptorSetLayout::_IsImageSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt)										 >( &DescriptorSetLayout::_IsImageSupported,	"IsSupported",	{"format", "usage", "options"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage)													 >( &DescriptorSetLayout::_IsImageSupported,	"IsSupported",	{"format", "usage"} );

		binder.Comment( "Check is image view description is supported by feature set." );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, MultiSamples const&, EImage, EPixelFormat, EImageUsage) >( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "samples", "imageType", "viewFormat", "viewUsage"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, MultiSamples const&, EImage, EPixelFormat)				>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "samples", "imageType", "viewFormat"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, MultiSamples const&, EImage)							>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "samples", "imageType"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, EImage, EPixelFormat, EImageUsage)						>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "imageType", "viewFormat", "viewUsage"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, EImage, EPixelFormat)									>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "imageType", "viewFormat"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, ImageLayer const&, EImage)													>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "arrayLayers", "imageType"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, EImage, EPixelFormat, EImageUsage)											>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "imageType", "viewFormat", "viewUsage"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, EImage, EPixelFormat)														>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "imageType", "viewFormat"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImageOpt, EImage)																	>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "options", "imageType"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImage, EPixelFormat, EImageUsage)													>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "imageType", "viewFormat", "viewUsage"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImage, EPixelFormat)																	>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "imageType", "viewFormat"} );
		binder.AddGenericMethod< bool (EPixelFormat, EImageUsage, EImage)																				>( &DescriptorSetLayout::_IsImageViewSupported,	"IsSupported",	{"format", "usage", "imageType"} );

		binder.Comment( "Check is buffer description is supported by feature set." );
		binder.AddGenericMethod< bool (EBufferUsage, EBufferOpt) >( &DescriptorSetLayout::_IsBufferSupported,	"IsSupported",	{"usage", "options"} );
		binder.AddGenericMethod< bool (EBufferUsage)			 >( &DescriptorSetLayout::_IsBufferSupported,	"IsSupported",	{"usage"} );

		binder.Comment( "Check is buffer view description is supported by feature set." );
		binder.AddGenericMethod< bool (EBufferUsage, EBufferOpt, EPixelFormat)	>( &DescriptorSetLayout::_IsBufferViewSupported,	"IsSupported",	{"usage", "options", "format"} );
		binder.AddGenericMethod< bool (EBufferUsage, EPixelFormat)				>( &DescriptorSetLayout::_IsBufferViewSupported,	"IsSupported",	{"usage", "format"} );
	}

/*
=================================================
	SetUsage
=================================================
*/
	void  DescriptorSetLayout::SetUsage (EDescSetUsage value) __Th___
	{
		// TODO: validate

		_dsLayout.usage = EDescSetUsage(value) | (_dsLayout.usage & ~EDescSetUsage::_PrivateMask);
	}

	void  DescriptorSetLayout::SetUsage2 (uint value) __Th___
	{
		SetUsage( EDescSetUsage(value) );
	}

/*
=================================================
	Define
=================================================
*/
	void  DescriptorSetLayout::Define (const String &value) __Th___
	{
		_defines << '\n' << value;
	}

/*
=================================================
	AddFeatureSet
=================================================
*/
	void  DescriptorSetLayout::AddFeatureSet (const String &name) __Th___
	{
		CHECK_THROW_MSG( _dsLayout.uniforms.empty(), "Add all feature sets before uniform declaration" );
		CHECK_THROW_MSG( not _uid.has_value() );

		auto&	storage = *ObjectStorage::Instance();
		auto	fs_it	= storage.featureSets.find( FeatureSetName{name} );
		CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
			"FeatureSet with name '"s << name << "' is not found" );

		_features.push_back( fs_it->second );
		ScriptFeatureSet::Minimize( INOUT _features );
	}

/*
=================================================
	_CheckUniformName
=================================================
*/
	void  DescriptorSetLayout::_CheckUniformName (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _uid.has_value() );
		ObjectStorage::Instance()->AddName<UniformName>( name );
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second,
			"uniform '"s << name << "' is already exists" );
	}

/*
=================================================
	_CheckArraySize
=================================================
*/
	void  DescriptorSetLayout::_CheckArraySize (uint size) C_Th___
	{
		CHECK_THROW_MSG( size <= MaxValue<ArraySize_t>() );

		// runtime sized array
		if ( size == 0 )
		{
			TEST_FEATURE( _features, runtimeDescriptorArray );
		}
	}

/*
=================================================
	_CheckSamplerName
=================================================
*/
	void  DescriptorSetLayout::_CheckSamplerName (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _uid.has_value() );

		auto&	storage	= *ObjectStorage::Instance();
		storage.AddName<SamplerName>( name );

		auto	it = storage.samplerMap.find( name );
		CHECK_THROW_MSG( it != storage.samplerMap.end(),
			"can't find sampler '"s << name << "'" );

		const auto	samp = storage.samplerRefs[ it->second ];

		for (auto fs : samp->GetFeatures()) {
			_features.push_back( fs );
		}
		ScriptFeatureSet::Minimize( INOUT _features );
	}

/*
=================================================
	_CheckAccessType
=================================================
*/
	void  DescriptorSetLayout::_CheckAccessType (INOUT EAccessType &access) C_Th___
	{
		CHECK_THROW_MSG( access < EAccessType::_Count );
		CHECK_THROW_MSG( access != EAccessType::_MemoryModel );

		if ( access == EAccessType::Unknown )
			access = EAccessType::Coherent;

		if ( access > EAccessType::_MemoryModel )
		{
			TEST_FEATURE( _features, vulkanMemoryModel );
		}
	}

/*
=================================================
	_CheckStateForStorage
=================================================
*/
	void  DescriptorSetLayout::_CheckStateForStorage (EResourceState state) C_Th___
	{
		switch ( ToEResState( state )) {
			case _EResState::ShaderStorage_Read :
			case _EResState::ShaderStorage_Write :
			case _EResState::ShaderStorage_RW :		break;
			default :								CHECK_THROW_MSG( false, "state must be ShaderStorage_***" );
		}
	}

/*
=================================================
	_CheckStorageFormat
=================================================
*/
	void  DescriptorSetLayout::_CheckStorageFormat (EPixelFormat format, bool isReadOnly) C_Th___
	{
		CHECK_THROW_MSG( format < EPixelFormat::_Count );

		if ( format == Default )
		{
			if ( isReadOnly ){
				TEST_FEATURE( _features, shaderStorageImageReadWithoutFormat );
			}else{
				TEST_FEATURE( _features, shaderStorageImageWriteWithoutFormat );
			}
		}
		else
		{
			TestFeature_PixelFormat( _features, &FeatureSet::storageImageFormats, format, "storageImageFormats" );
		}
	}

/*
=================================================
	AddDebugStorageBuffer
=================================================
*/
	void  DescriptorSetLayout::AddDebugStorageBuffer (const String &name, EShaderStages stages, Bytes staticSize, Bytes arraySize) __Th___
	{
		_CheckUniformName( name );

		Uniform			un;
		un.type			= EDescriptorType::StorageBuffer;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(1);

		un.buffer						= Default;
		un.buffer.state					= EResourceState::ShaderStorage_RW | EResourceState_FromShaders( stages );
		un.buffer.dynamicOffsetIndex	= UMax;
		un.buffer.staticSize			= Bytes32u{staticSize};
		un.buffer.arrayStride			= Bytes32u{arraySize};
		un.buffer.typeName				= ShaderStructName{"dbg_ShaderTraceStorage"};

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	_AddUniformBuffer
=================================================
*/
	void  DescriptorSetLayout::_AddUniformBuffer (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderUniform;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};
		String			type_name;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< String const& >(idx) )
			type_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required typename as 'String'" );

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddUniformBuffer( stages, uniform_name, array_size, type_name, res_state, False{} );
	}

/*
=================================================
	_AddStorageBuffer
=================================================
*/
	void  DescriptorSetLayout::_AddStorageBuffer (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderStorage_RW;
		EAccessType		access			= EAccessType::Coherent;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};
		String			type_name;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< String const& >(idx) )
			type_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required typename as 'String'" );

		if ( args.IsArg< EAccessType >(idx) )
			access = args.Arg< EAccessType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddStorageBuffer( stages, uniform_name, array_size, type_name, access, res_state, False{} );
	}

/*
=================================================
	_AddUniformBufferDynamic
=================================================
*/
	void  DescriptorSetLayout::_AddUniformBufferDynamic (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderUniform;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};
		String			type_name;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< String const& >(idx) )
			type_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required typename as 'String'" );

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddUniformBuffer( stages, uniform_name, array_size, type_name, res_state, True{} );
	}

/*
=================================================
	_AddStorageBufferDynamic
=================================================
*/
	void  DescriptorSetLayout::_AddStorageBufferDynamic (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderStorage_RW;
		EAccessType		access			= EAccessType::Coherent;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};
		String			type_name;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< String const& >(idx) )
			type_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required typename as 'String'" );

		if ( args.IsArg< EAccessType >(idx) )
			access = args.Arg< EAccessType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddStorageBuffer( stages, uniform_name, array_size, type_name, access, res_state, True{} );
	}

/*
=================================================
	_AddUniformTexelBuffer
=================================================
*/
	void  DescriptorSetLayout::_AddUniformTexelBuffer (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderSample;
		EImageType		image_type		= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddUniformTexelBuffer( stages, uniform_name, array_size, image_type, res_state );
	}

/*
=================================================
	_AddStorageTexelBuffer
=================================================
*/
	void  DescriptorSetLayout::_AddStorageTexelBuffer (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderStorage_RW;
		EImageType		image_type		= Default;
		EAccessType		access			= EAccessType::Coherent;
		EPixelFormat	format			= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EPixelFormat >(idx) )
			format = args.Arg< EPixelFormat >(idx++);

		if ( args.IsArg< EAccessType >(idx) )
			access = args.Arg< EAccessType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddStorageTexelBuffer( stages, uniform_name, array_size, image_type, format, access, res_state );
	}

/*
=================================================
	_AddStorageImage
=================================================
*/
	void  DescriptorSetLayout::_AddStorageImage (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderStorage_RW;
		EImageType		image_type		= Default;
		EAccessType		access			= EAccessType::Coherent;
		EPixelFormat	format			= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EPixelFormat >(idx) )
			format = args.Arg< EPixelFormat >(idx++);

		if ( args.IsArg< EAccessType >(idx) )
			access = args.Arg< EAccessType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddStorageImage( stages, uniform_name, array_size, image_type, format, access, res_state );
	}

/*
=================================================
	_AddSampledImage
=================================================
*/
	void  DescriptorSetLayout::_AddSampledImage (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderSample;
		EImageType		image_type		= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddSampledImage( stages, uniform_name, array_size, image_type, res_state );
	}

/*
=================================================
	_AddCombinedImage
=================================================
*/
	void  DescriptorSetLayout::_AddCombinedImage (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderSample;
		EImageType		image_type		= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddCombinedImage( stages, uniform_name, array_size, image_type, res_state );
	}

/*
=================================================
	_AddCombinedImage_ImmutableSampler
=================================================
*/
	void  DescriptorSetLayout::_AddCombinedImage_ImmutableSampler (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::ShaderSample;
		EImageType		image_type		= Default;
		String			uniform_name;
		Array<String>	sampler_names;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		if ( args.IsArg< ScriptArray<String> const& >(idx) )
		{
			auto&	arr = args.Arg< ScriptArray<String> const& >(idx++);
			for (auto& samp : arr) {
				sampler_names.push_back( samp );
			}
		}
		else
		if ( args.IsArg< String const& >(idx) )
			sampler_names.push_back( args.Arg< String const& >(idx++) );
		else
			CHECK_THROW_MSG( false, "Required sampler name as 'String' or 'Array<String>'" );

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddCombinedImage_ImmutableSampler( stages, uniform_name, image_type, res_state, sampler_names );
	}

/*
=================================================
	_AddSubpassInput
=================================================
*/
	void  DescriptorSetLayout::_AddSubpassInput (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		EResourceState	res_state		= EResourceState::InputColorAttachment;
		EImageType		image_type		= EImageType::Img2D | EImageType::Float;
		uint			sp_index		= UMax;
		String			uniform_name;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< uint >(idx) )
			sp_index = args.Arg< uint >(idx++);

		if ( args.IsArg< EImageType >(idx) )
			image_type = args.Arg< EImageType >(idx++);

		if ( args.IsArg< EResourceState >(idx) )
			res_state = args.Arg< EResourceState >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddSubpassInput( stages, uniform_name, sp_index, image_type, res_state );
	}

/*
=================================================
	_AddSampler
=================================================
*/
	void  DescriptorSetLayout::_AddSampler (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddSampler( stages, uniform_name, array_size );
	}

/*
=================================================
	_AddImmutableSampler
=================================================
*/
	void  DescriptorSetLayout::_AddImmutableSampler (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		String			uniform_name;
		Array<String>	sampler_names;

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ScriptArray<String> const& >(idx) )
		{
			auto&	arr = args.Arg< ScriptArray<String> const& >(idx++);
			for (auto& samp : arr) {
				sampler_names.push_back( samp );
			}
		}
		else
		if ( args.IsArg< String const& >(idx) )
			sampler_names.push_back( args.Arg< String const& >(idx++) );
		else
			CHECK_THROW_MSG( false, "Required sampler name as 'String' or 'Array<String>'" );

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddImmutableSampler( stages, uniform_name, sampler_names );
	}

/*
=================================================
	_AddRayTracingScene
=================================================
*/
	void  DescriptorSetLayout::_AddRayTracingScene (Scripting::ScriptArgList args) __Th___
	{
		uint			idx				= 0;
		EShaderStages	stages			= Default;
		String			uniform_name;
		ArraySize		array_size		= ArraySize{1};

		if ( args.IsArg< EShaderStages >(idx) )	stages = args.Arg< EShaderStages >(idx++);			else
		if ( args.IsArg< uint >(idx) )			stages = EShaderStages(args.Arg< uint >(idx++));	else
												CHECK_THROW_MSG( false, "Required 'EShaderStages' or 'uint'" );

		if ( args.IsArg< String const& >(idx) )
			uniform_name = args.Arg< String const& >(idx++);
		else
			CHECK_THROW_MSG( false, "Required uniform name as 'String'" );

		if ( args.IsArg< ArraySize const& >(idx) )
			array_size = args.Arg< ArraySize const& >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		args.GetObject< DescriptorSetLayout >()->AddRayTracingScene( stages, uniform_name, array_size );
	}

/*
=================================================
	AddUniformBuffer
=================================================
*/
	void  DescriptorSetLayout::AddUniformBuffer (EShaderStages stages, const String &name, const ArraySize &arraySize, const String &typeName, EResourceState state, Bool dynamic) __Th___
	{
		CHECK_THROW_MSG( ToEResState(state) == _EResState::ShaderUniform );
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );

		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );

		const auto&	st_map	= ObjectStorage::Instance()->structTypes;
		auto		st_it	= st_map.find( typeName );
		CHECK_THROW_MSG( st_it != st_map.end(),
			"ShaderStructType '"s << typeName << "' is not exists" );

		CHECK_THROW_MSG( not st_it->second->HasDynamicArray() );

		if ( IsStd430( st_it->second->Layout() ))
		{
			TEST_FEATURE( _features, scalarBlockLayout,
				", UniformBuffer '"s << name << "' with struct '" << st_it->second->Name() <<
				"' with Std430 layout requires 'scalarBlockLayout'" );
		}
		else
		{
			CHECK_THROW_MSG( IsStd140OrMetal( st_it->second->Layout() ),
				"UniformBuffer '"s << name << "' with struct '" << st_it->second->Name() << "' requires Std140 layout" );
		}

		auto&	aux_info	= _infoMap[ UniformName{name} ];
		aux_info.type		= st_it->second;

		Uniform			un;
		un.type			= EDescriptorType::UniformBuffer;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.buffer						= Default;
		un.buffer.state					= state;
		un.buffer.dynamicOffsetIndex	= DescriptorSetLayoutDesc::DynamicOffsetIdx_t(dynamic ? 0u : UMax);
		un.buffer.staticSize			= Bytes32u{ aux_info.type->StaticSize() };
		un.buffer.arrayStride			= Bytes32u{0u};
		un.buffer.typeName				= ShaderStructName{st_it->second->Typename()};

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddStorageBuffer
=================================================
*/
	void  DescriptorSetLayout::AddStorageBuffer (EShaderStages stages, const String &name, const ArraySize &arraySize, const String &typeName, EAccessType access, EResourceState state, Bool dynamic) __Th___
	{
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );

		_CheckStateForStorage( state );
		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );
		_CheckAccessType( INOUT access );

		const auto&	st_map	= ObjectStorage::Instance()->structTypes;
		auto		st_it	= st_map.find( typeName );
		CHECK_THROW_MSG( st_it != st_map.end(),
			"ShaderStructType '"s << typeName << "' is not exists" );

		auto&	aux_info	= _infoMap[ UniformName{name} ];
		aux_info.type		= st_it->second;
		aux_info.access		= access;

		Uniform			un;
		un.type			= EDescriptorType::StorageBuffer;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.buffer						= Default;
		un.buffer.state					= state;
		un.buffer.dynamicOffsetIndex	= DescriptorSetLayoutDesc::DynamicOffsetIdx_t(dynamic ? 0u : UMax);
		un.buffer.staticSize			= Bytes32u{ aux_info.type->StaticSize()  };
		un.buffer.arrayStride			= Bytes32u{ aux_info.type->ArrayStride() };
		un.buffer.typeName				= ShaderStructName{st_it->second->Typename()};

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddUniformTexelBuffer
=================================================
*/
	void  DescriptorSetLayout::AddUniformTexelBuffer (EShaderStages stages, const String &name, const ArraySize &arraySize, EImageType type, EResourceState state) __Th___
	{
		CHECK_THROW_MSG( ToEResState(state) == _EResState::ShaderSample );
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );
		CHECK_THROW_MSG( (type & EImageType::_TexMask) == EImageType::Buffer );
		CHECK_THROW_MSG( (type & EImageType::_ValMask) != Default );

		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );

		Uniform			un;
		un.type			= EDescriptorType::UniformTexelBuffer;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.texelBuffer			= Default;
		un.texelBuffer.state	= state;
		un.texelBuffer.type		= type;

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddStorageTexelBuffer
=================================================
*/
	void  DescriptorSetLayout::AddStorageTexelBuffer (EShaderStages stages, const String &name, const ArraySize &arraySize, EImageType type, EPixelFormat format, EAccessType access, EResourceState state) __Th___
	{
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );
		CHECK_THROW_MSG( (type & EImageType::_TexMask) == EImageType::Buffer );
		CHECK_THROW_MSG( (type & EImageType::_ValMask) != Default );

		_CheckStateForStorage( state );
		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );
		_CheckAccessType( INOUT access );
		_CheckStorageFormat( format, ToEResState(state) == _EResState::ShaderStorage_Read );

		Uniform			un;
		un.type			= EDescriptorType::StorageTexelBuffer;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.texelBuffer			= Default;
		un.texelBuffer.state	= state;
		un.texelBuffer.type		= type;
	//	un.texelBuffer.format	= format;

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );

		auto&	aux_info	= _infoMap[ UniformName{name} ];
		aux_info.access		= access;
	}

/*
=================================================
	AddStorageImage
=================================================
*/
	void  DescriptorSetLayout::AddStorageImage (EShaderStages stages, const String &name, const ArraySize &arraySize, EImageType type, EPixelFormat format, EAccessType access, EResourceState state) __Th___
	{
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );
		CHECK_THROW_MSG( (type & EImageType::_TexMask) != Default );

		_CheckStateForStorage( state );
		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );
		_CheckAccessType( INOUT access );
		_CheckStorageFormat( format, ToEResState(state) == _EResState::ShaderStorage_Read );

		const EImageType	val_flags = EImageType_FromPixelFormat( format );
		CHECK_THROW_MSG( val_flags != Default );

		if ( (type & EImageType::_ValMask) == Default )
			type |= val_flags;

		CHECK_THROW_MSG( EImageType_IsCompatible( (type & EImageType::_ValMask), val_flags ));

		Uniform			un;
		un.type			= EDescriptorType::StorageImage;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.image		= Default;
		un.image.state	= state;
		un.image.type	= type;
		un.image.format	= format;

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );

		auto&	aux_info	= _infoMap[ UniformName{name} ];
		aux_info.access		= access;
	}

/*
=================================================
	AddSampledImage
=================================================
*/
	void  DescriptorSetLayout::AddSampledImage (EShaderStages stages, const String &name, const ArraySize &arraySize, EImageType type, EResourceState state) __Th___
	{
		CHECK_THROW_MSG( ToEResState(state) == _EResState::ShaderSample );
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );
		CHECK_THROW_MSG( (type & EImageType::_TexMask) != Default );
		CHECK_THROW_MSG( (type & EImageType::_ValMask) != Default );

		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );

		Uniform			un;
		un.type			= EDescriptorType::SampledImage;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.image		= Default;
		un.image.state	= state;
		un.image.type	= type;
		un.image.format	= Default;

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddCombinedImage
=================================================
*/
	void  DescriptorSetLayout::AddCombinedImage (EShaderStages stages, const String &name, const ArraySize &arraySize, EImageType type, EResourceState state) __Th___
	{
		CHECK_THROW_MSG( ToEResState(state) == _EResState::ShaderSample );
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );
		CHECK_THROW_MSG( (type & EImageType::_TexMask) != Default );
		CHECK_THROW_MSG( (type & EImageType::_ValMask) != Default );

		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );

		Uniform			un;
		un.type			= EDescriptorType::CombinedImage;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		un.image		= Default;
		un.image.state	= state;
		un.image.type	= type;
		un.image.format	= Default;

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddCombinedImage_ImmutableSampler
=================================================
*/
	void  DescriptorSetLayout::AddCombinedImage_ImmutableSampler (EShaderStages stages, const String &name, EImageType type, EResourceState state, const String &samplerName) __Th___
	{
		AddCombinedImage_ImmutableSampler( stages, name, type, state, ArrayView<String>{&samplerName, 1} );
	}

	void  DescriptorSetLayout::AddCombinedImage_ImmutableSampler (EShaderStages stages, const String &name, EImageType type, EResourceState state, ArrayView<String> samplerNames) __Th___
	{
		CHECK_THROW_MSG( ToEResState(state) == _EResState::ShaderSample );
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );
		CHECK_THROW_MSG( (type & EImageType::_TexMask) != Default );
		CHECK_THROW_MSG( (type & EImageType::_ValMask) != Default );

		const uint	array_size = uint(samplerNames.size());

		_CheckUniformName( name );
		_CheckArraySize( array_size );

		Uniform			un;
		un.type			= EDescriptorType::CombinedImage_ImmutableSampler;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(array_size);

		un.image		= Default;
		un.image.state	= state;
		un.image.type	= type;
		un.image.format	= Default;

		CHECK_THROW_MSG( _dsLayout.samplerStorage.size() + array_size < MaxValue<DescriptorSetLayoutDesc::SamplerIdx_t>() );
		un.image.samplerOffsetInStorage	= DescriptorSetLayoutDesc::SamplerIdx_t(_dsLayout.samplerStorage.size());

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );

		for (auto& samp : samplerNames)
		{
			_CheckSamplerName( samp );
			_dsLayout.samplerStorage.push_back( SamplerName{samp} );
		}

		// TODO: cubemap supports only CLAMP sampler
	}

/*
=================================================
	AddSubpassInput
=================================================
*/
	void  DescriptorSetLayout::AddSubpassInput (EShaderStages stages, const String &name, uint index, EImageType type, EResourceState state) __Th___
	{
		switch ( ToEResState( state )) {
			case _EResState::InputColorAttachment :
			case _EResState::InputColorAttachment_RW :
			case _EResState::InputDepthStencilAttachment :
			case _EResState::InputDepthStencilAttachment_RW :	break;
			default :											CHECK_THROW_MSG( false, "state must be one of InputColorAttachment or InputDepthStencilAttachment" );
		}
		CHECK_THROW_MSG( stages != Default );
		state |= EResourceState_FromShaders( stages );

		CHECK_THROW_MSG( (type & EImageType::_TexMask) != Default );
		CHECK_THROW_MSG( (type & EImageType::_ValMask) != Default );

		_CheckUniformName( name );

		Uniform			un;
		un.type			= EDescriptorType::SubpassInput;
		un.stages		= stages;
		un.arraySize	= 1;

		un.image		= Default;
		un.image.state	= state;
		un.image.type	= type;
		un.image.format	= Default;

		un.image.subpassInputIdx = (index == UMax ? 0xFF : ubyte(index));

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddSubpassInputFromRenderTech
=================================================
*/
	void  DescriptorSetLayout::AddSubpassInputFromRenderTech (const String &renTechName, const String &passName) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		storage.AddName<RenderTechName>( renTechName );
		storage.AddName<RenderTechPassName>( passName );

		const auto&	rtech_map	= storage.rtechMap;
		auto		rt_it		= rtech_map.find( renTechName );
		CHECK_THROW_MSG( rt_it != rtech_map.end(),
			"RenderTechnique '"s << renTechName << "' is not exists" );

		auto	pass_ptr = rt_it->second->GetPass( passName );
		CHECK_THROW_MSG( pass_ptr,
			"Pass '"s << passName << "' is not exists in RenderTechnique '" << renTechName << "'" );

		auto*	gpass_ptr = DynCast<RTGraphicsPass>( pass_ptr.Get() );
		CHECK_THROW_MSG( gpass_ptr != null );

		return AddSubpassInputFromRenderPass( storage.GetName( gpass_ptr->GetRenderPassName() ),
											  storage.GetName( gpass_ptr->GetSubpassName() ));
	}

/*
=================================================
	AddSubpassInputFromRenderPass
=================================================
*/
	void  DescriptorSetLayout::AddSubpassInputFromRenderPass (const String &compatRPassName, const String &subpassName) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		storage.AddName<CompatRenderPassName>( compatRPassName );
		storage.AddName<SubpassName>( subpassName );

		auto	compat_it = storage.compatibleRPs.find( CompatRenderPassName{ compatRPassName });
		CHECK_THROW_MSG( compat_it != storage.compatibleRPs.end(),
			"CompatibleRenderPass '"s << compatRPassName << "' is not exists" );

		const auto&	subpass_map = compat_it->second->_subpassMap;
		auto		sp_it		= subpass_map.find( SubpassName{ subpassName });
		CHECK_THROW_MSG( sp_it != subpass_map.end(),
			"Subpass '"s << subpassName << "' is not exists in CompatibleRenderPass '" << compatRPassName << "'" );

		for (const auto& [name, att] : compat_it->second->_attachments)
		{
			auto	sp_att_it = att->usageMap.find( SubpassName{ subpassName });
			CHECK_THROW_MSG( sp_att_it != att->usageMap.end(),
				"Subpass '"s << subpassName << "' is not defined in Attachment '" << storage.GetName( name ) <<
				"' of the CompatibleRenderPass '" << compatRPassName << "'" );

			const auto&	usage	= sp_att_it->second;
			if ( not usage.input.IsDefined() )
				continue;

			CHECK_THROW_MSG( AnyEqual( usage.type, EAttachment::Input, EAttachment::ReadWrite ));

			EImageType	img_type = att->samples.Get() > 1 ? EImageType::Img2DMS : EImageType::Img2D;

			switch_enum( usage.input.type )
			{
				case EShaderIO::Int :			img_type |= EImageType::Int;			break;
				case EShaderIO::UInt :			img_type |= EImageType::UInt;			break;
				case EShaderIO::Float :			img_type |= EImageType::Float;			break;
				case EShaderIO::AnyColor :		img_type |= EImageType::Float;			break;
				case EShaderIO::UFloat :		img_type |= EImageType::Float;			break;
				case EShaderIO::Half :			img_type |= EImageType::Half;			break;
				case EShaderIO::UNorm :			img_type |= EImageType::UNorm;			break;
				case EShaderIO::SNorm :			img_type |= EImageType::SNorm;			break;
				case EShaderIO::sRGB :			img_type |= EImageType::sRGB;			break;
				case EShaderIO::Depth :			img_type |= EImageType::Depth;			break;
				case EShaderIO::Stencil :		img_type |= EImageType::Stencil;		break;
				case EShaderIO::DepthStencil:	img_type |= EImageType::DepthStencil;	break;
				case EShaderIO::Unknown :
				case EShaderIO::_Count :
				default :						CHECK_THROW_MSG( false, "unknown ShaderIO type" );
			}
			switch_end

			EResourceState	state =
				(usage.type == EAttachment::ReadWrite ? EResourceState::InputColorAttachment_RW : EResourceState::InputColorAttachment);

			if ( (img_type & EImageType::_ValMask) >= EImageType::Depth )
				state = (usage.type == EAttachment::ReadWrite ? EResourceState::InputDepthStencilAttachment_RW : EResourceState::InputDepthStencilAttachment);

			AddSubpassInput( EShaderStages::Fragment, storage.GetName( usage.input.name ), usage.input.index, img_type, state );
		}
	}

/*
=================================================
	AddSampler
=================================================
*/
	void  DescriptorSetLayout::AddSampler (EShaderStages stages, const String &name, const ArraySize &arraySize) __Th___
	{
		CHECK_THROW_MSG( stages != Default );

		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );

		Uniform		un;
		un.type			= EDescriptorType::Sampler;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	AddImmutableSampler
=================================================
*/
	void  DescriptorSetLayout::AddImmutableSampler (EShaderStages stages, const String &name, const String &samplerName) __Th___
	{
		AddImmutableSampler( stages, name, ArrayView<String>{ &samplerName, 1 });
	}

	void  DescriptorSetLayout::AddImmutableSampler (EShaderStages stages, const String &name, ArrayView<String> samplerNames) __Th___
	{
		CHECK_THROW_MSG( stages != Default );

		const uint	array_size = uint(samplerNames.size());

		_CheckUniformName( name );
		_CheckArraySize( array_size );

		Uniform		un;
		un.type			= EDescriptorType::ImmutableSampler;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(array_size);

		CHECK_THROW_MSG( _dsLayout.samplerStorage.size() + array_size < MaxValue<DescriptorSetLayoutDesc::SamplerIdx_t>() );
		un.immutableSampler.offsetInStorage = DescriptorSetLayoutDesc::SamplerIdx_t(_dsLayout.samplerStorage.size());

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );

		for (auto& samp : samplerNames)
		{
			_CheckSamplerName( samp );
			_dsLayout.samplerStorage.push_back( SamplerName{samp} );
		}
	}

/*
=================================================
	AddRayTracingScene
=================================================
*/
	void  DescriptorSetLayout::AddRayTracingScene (EShaderStages stages, const String &name, const ArraySize &arraySize) __Th___
	{
		CHECK_THROW_MSG( stages != Default );

		_CheckUniformName( name );
		_CheckArraySize( arraySize.value );

		Uniform		un;
		un.type			= EDescriptorType::RayTracingScene;
		un.stages		= stages;
		un.arraySize	= ArraySize_t(arraySize.value);

		_dsLayout.uniforms.emplace_back( UniformName{name}, un );
	}

/*
=================================================
	CheckDescriptorLimits
=================================================
*/
	bool  DescriptorSetLayout::CheckDescriptorLimits (const DescriptorCount &total, const PerStageDescCount_t &perStage,
													  ArrayView<ScriptFeatureSetPtr> features, StringView name)
	{
		using DT = EDescriptorType;

		bool	result = true;
		{
			#define CHECK_LIMIT( _lhs_, _rhs_, _msg_ )																						\
			{																																\
				const auto	rhs_val = GetMaxValueFromFeatures( features, &FeatureSet::perDescrSet, &FeatureSet::PerDescriptorSet::_rhs_ );	\
				if_unlikely( (_lhs_) > rhs_val ) {																							\
					result = false;																											\
					AE_LOGE( String{name} << ": number of " << (_msg_) << " (" << ToString(_lhs_) << ") exceeds the maximum allowed '" <<	\
							 AE_TOSTRING( _rhs_ ) << "' (" << ToString(rhs_val) << ")" );													\
				}																															\
			}
			CHECK_LIMIT( total.uniformBuffers,		maxUniformBuffers,		"uniform buffers per DS" );
			CHECK_LIMIT( total.storageBuffers,		maxStorageBuffers,		"storage buffers per DS" );
			CHECK_LIMIT( total.subpassInputs,		maxInputAttachments,	"input attachments per DS" );
			CHECK_LIMIT( total.rayTracingScenes,	maxAccelStructures,		"acceleration structures per DS" );
			CHECK_LIMIT( total.storageImages,		maxStorageImages,		"storage images per DS" );
			CHECK_LIMIT( total.sampledImages,		maxSampledImages,		"sampler images per DS" );
			CHECK_LIMIT( total.samplers,			maxSamplers,			"samplers per DS" );
			CHECK_LIMIT( total.TotalCount(),		maxTotalResources,		"total resources per DS" );
			#undef CHECK_LIMIT

			#define CHECK_LIMIT( _lhs_, _rhs_, _msg_ )																						\
			{																																\
				const auto	rhs_val = GetMaxValueFromFeatures( features, &FeatureSet::perDescrSet_##_rhs_ );								\
				if_unlikely( (_lhs_) > rhs_val ) {																							\
					result = false;																											\
					AE_LOGE( String{name} << ": number of " << (_msg_) << " (" << ToString(_lhs_) << ") exceeds the maximum allowed '" <<	\
							 AE_TOSTRING( _rhs_ ) << "' (" << ToString(rhs_val) << ")" );													\
				}																															\
			}
			CHECK_LIMIT( total.dynamicUniformBuffers,	maxUniformBuffersDynamic,	"dynamic uniform buffers per DS" );
			CHECK_LIMIT( total.dynamicStorageBuffers,	maxStorageBuffersDynamic,	"dynamic storage buffers per DS" );
			#undef CHECK_LIMIT
		}

		for (auto [stage, count] : perStage)
		{
			#define CHECK_LIMIT( _lhs_, _rhs_, _msg_ )																					\
			{																															\
				const auto	rhs_val = GetMaxValueFromFeatures( features, &FeatureSet::perStage, &FeatureSet::PerShaderStage::_rhs_ );	\
				if_unlikely( (_lhs_) > rhs_val ) {																						\
					result = false;																										\
					AE_LOGE( "Number of "s << (_msg_) << " (" << ToString(_lhs_) << ") exceeds the maximum allowed '" <<				\
							AE_TOSTRING( _rhs_ ) << "' (" << ToString(rhs_val) << ")" );												\
				}																														\
			}
			CHECK_LIMIT( count.uniformBuffers,		maxUniformBuffers,		"uniform buffers per stage" );
			CHECK_LIMIT( count.storageBuffers,		maxStorageBuffers,		"storage buffers per stage" );
			CHECK_LIMIT( count.subpassInputs,		maxInputAttachments,	"input attachments per stage" );
			CHECK_LIMIT( count.rayTracingScenes,	maxAccelStructures,		"acceleration structures per stage" );
			CHECK_LIMIT( count.storageImages,		maxStorageImages,		"storage images per stage" );
			CHECK_LIMIT( count.sampledImages,		maxSampledImages,		"sampler images per stage" );
			CHECK_LIMIT( count.samplers,			maxSamplers,			"samplers per stage" );
			CHECK_LIMIT( count.TotalCount(),		maxTotalResources,		"total resources per stage" );
			#undef CHECK_LIMIT
		}

		return result;
	}

/*
=================================================
	GetSampler
=================================================
*/
	SamplerName  DescriptorSetLayout::GetSampler (const Uniform &un) C_Th___
	{
		if ( un.type == EDescriptorType::CombinedImage_ImmutableSampler )
		{
			CHECK_THROW_MSG( un.image.samplerOffsetInStorage < _dsLayout.samplerStorage.size() );

			return _dsLayout.samplerStorage[ un.image.samplerOffsetInStorage ];
		}

		if ( un.type == EDescriptorType::ImmutableSampler )
		{
			CHECK_THROW_MSG( un.immutableSampler.offsetInStorage < _dsLayout.samplerStorage.size() );

			return _dsLayout.samplerStorage[ un.immutableSampler.offsetInStorage ];
		}

		CHECK_THROW_MSG( false,
			"'GetSampler()' can be used only for 'CombinedImage_ImmutableSampler' and 'ImmutableSampler'" );
	}

/*
=================================================
	GetSampler
=================================================
*/
	SamplerName  DescriptorSetLayout::GetSampler (uint indexInStorage) C_Th___
	{
		if ( indexInStorage < _dsLayout.samplerStorage.size() )
			return _dsLayout.samplerStorage[ indexInStorage ];

		return Default;
	}

/*
=================================================
	_IsImageSupported
=================================================
*/
	void  DescriptorSetLayout::_IsImageSupported (Scripting::ScriptArgList args) __Th___
	{
		ImageDesc	desc;
		uint		idx = 0;

		if ( args.IsArg< EPixelFormat >(idx) )			desc.format		 = args.Arg< EPixelFormat >(idx++);
		if ( args.IsArg< EImageUsage >(idx) )			desc.usage		 = args.Arg< EImageUsage >(idx++);
		if ( args.IsArg< EImageOpt >(idx) )				desc.options	 = args.Arg< EImageOpt >(idx++);
		if ( args.IsArg< ImageLayer const& >(idx) )		desc.arrayLayers = args.Arg< ImageLayer const& >(idx++);
		if ( args.IsArg< MultiSamples const& >(idx) )	desc.samples	 = args.Arg< MultiSamples const& >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );

		desc.imageDim = EImageDim_2D;
		desc.Validate();

		bool	result = false;
		for (auto& fs : args.GetObject< DescriptorSetLayout >()->_features) {
			result = result or fs->fs.IsSupported( desc );
		}
		args.Return( result );
	}

/*
=================================================
	_IsImageViewSupported
=================================================
*/
	void  DescriptorSetLayout::_IsImageViewSupported (Scripting::ScriptArgList args) __Th___
	{
		ImageDesc		img_desc;
		ImageViewDesc	view_desc;
		uint			idx = 0;

		if ( args.IsArg< EPixelFormat >(idx) )			img_desc.format		 = args.Arg< EPixelFormat >(idx++);
		if ( args.IsArg< EImageUsage >(idx) )			img_desc.usage		 = args.Arg< EImageUsage >(idx++);
		if ( args.IsArg< EImageOpt >(idx) )				img_desc.options	 = args.Arg< EImageOpt >(idx++);
		if ( args.IsArg< ImageLayer const& >(idx) )		img_desc.arrayLayers = args.Arg< ImageLayer const& >(idx++);
		if ( args.IsArg< MultiSamples const& >(idx) )	img_desc.samples	 = args.Arg< MultiSamples const& >(idx++);

		if ( args.IsArg< EImage >(idx) )				view_desc.viewType	 = args.Arg< EImage >(idx++);
		if ( args.IsArg< EPixelFormat >(idx) )			view_desc.format	 = args.Arg< EPixelFormat >(idx++);
		if ( args.IsArg< EImageUsage >(idx) )			view_desc.extUsage	 = args.Arg< EImageUsage >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );

		img_desc.Validate();
		view_desc.Validate( img_desc );

		bool	result = false;
		for (auto& fs : args.GetObject< DescriptorSetLayout >()->_features) {
			result = result or fs->fs.IsSupported( img_desc, view_desc );
		}
		args.Return( result );
	}

/*
=================================================
	_IsBufferSupported
=================================================
*/
	void  DescriptorSetLayout::_IsBufferSupported (Scripting::ScriptArgList args) __Th___
	{
		BufferDesc	desc;
		uint		idx = 0;

		if ( args.IsArg< EBufferUsage >(idx) )	desc.usage	 = args.Arg< EBufferUsage >(idx++);
		if ( args.IsArg< EBufferOpt >(idx) )	desc.options = args.Arg< EBufferOpt >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
		desc.Validate();

		bool	result = false;
		for (auto& fs : args.GetObject< DescriptorSetLayout >()->_features) {
			result = result or fs->fs.IsSupported( desc );
		}
		args.Return( result );
	}

/*
=================================================
	_IsBufferViewSupported
=================================================
*/
	void  DescriptorSetLayout::_IsBufferViewSupported (Scripting::ScriptArgList args) __Th___
	{
		BufferDesc		buf_desc;
		BufferViewDesc	view_desc;
		uint			idx = 0;

		if ( args.IsArg< EBufferUsage >(idx) )	buf_desc.usage		= args.Arg< EBufferUsage >(idx++);
		if ( args.IsArg< EBufferOpt >(idx) )	buf_desc.options	= args.Arg< EBufferOpt >(idx++);
		if ( args.IsArg< EPixelFormat >(idx) )	view_desc.format	= args.Arg< EPixelFormat >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );

		buf_desc.Validate();
		view_desc.Validate( buf_desc );

		bool	result = false;
		for (auto& fs : args.GetObject< DescriptorSetLayout >()->_features) {
			result = result or fs->fs.IsSupported( buf_desc, view_desc );
		}
		args.Return( result );
	}


} // AE::PipelineCompiler
