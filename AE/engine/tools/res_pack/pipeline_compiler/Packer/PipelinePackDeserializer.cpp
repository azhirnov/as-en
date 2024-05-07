// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "PipelinePack.h"
#include "graphics/Private/EnumUtils.h"

#ifdef AE_TEST_PIPELINE_COMPILER
# include "graphics/Private/EnumToString.h"
#endif

#ifdef AE_ENABLE_GLSL_TRACE
# include "ShaderTrace.h"
#else
# include "Packer/ShaderTraceDummy.h"
#endif

namespace AE::PipelineCompiler
{
/*
=================================================
	SpirvWithTrace
=================================================
*/
	SpirvWithTrace::SpirvWithTrace () __NE___ {}

	SpirvWithTrace::SpirvWithTrace (SpirvWithTrace &&other) __NE___ :
		bytecode{ RVRef(other.bytecode) },
		trace{ RVRef(other.trace) }
	{}

	SpirvWithTrace::~SpirvWithTrace () __NE___ {}

	SpirvWithTrace::SpirvWithTrace (const SpirvWithTrace &other) :
		bytecode{other.bytecode},
		trace{other.trace ? other.trace->Clone() : Default}
	{}

	SpirvWithTrace&  SpirvWithTrace::operator = (SpirvWithTrace &&rhs) __NE___
	{
		this->bytecode	= RVRef(rhs.bytecode);
		this->trace		= RVRef(rhs.trace);
		return *this;
	}

	SpirvWithTrace&  SpirvWithTrace::operator = (const SpirvWithTrace &rhs)
	{
		this->bytecode	= rhs.bytecode;
		this->trace		= rhs.trace ? rhs.trace->Clone() : Default;
		return *this;
	}

	bool  SpirvWithTrace::operator == (const SpirvWithTrace &rhs) C_NE___
	{
		return	(bytecode == rhs.bytecode)						and
				(trace != null) == (rhs.trace != null)			and
				(trace != null ? (*trace == *rhs.trace) : true);
	}
//-----------------------------------------------------------------------------



/*
=================================================
	EImageType_IsCompatible
=================================================
*/
	bool  EImageType_IsCompatible (EImageType lhs, EImageType rhs) __NE___
	{
		if_unlikely( (lhs & EImageType::_TexMask) != (rhs & EImageType::_TexMask) )
			return false;

		EImageType	lhs_val	= (lhs & EImageType::_ValMask);
		EImageType	rhs_val	= (lhs & EImageType::_ValMask);

		switch ( lhs_val ) {
			case EImageType::Half :
			case EImageType::SNorm :
			case EImageType::UNorm :
			case EImageType::sRGB :		lhs_val = EImageType::Float;
		}
		switch ( rhs_val ) {
			case EImageType::Half :
			case EImageType::SNorm :
			case EImageType::UNorm :
			case EImageType::sRGB :		rhs_val = EImageType::Float;
		}

		if_unlikely( lhs_val != rhs_val )
			return false;

		if_unlikely( (lhs & EImageType::_QualMask) != (rhs & EImageType::_QualMask) )
			return false;

		return true;
	}

/*
=================================================
	EImageType_FromPixelFormat
=================================================
*/
	EImageType  EImageType_FromPixelFormat (EPixelFormat fmt) __NE___
	{
		using EType = PixelFormatInfo::EType;

		const auto&		info = EPixelFormat_GetInfo( fmt );

		switch ( info.valueType & EType::_ValueMask )
		{
			case EType::SFloat :
			case EType::UFloat :
			{
				if ( not info.IsCompressed() and info.BitsPerChannel() == 16 )
					return EImageType::Half;
				else
					return EImageType::Float;
			}
			case EType::UNorm :
			{
				if ( AllBits( info.valueType, EType::sRGB ))
					return EImageType::sRGB;
				else
					return EImageType::UNorm;
			}
			case EType::SNorm :	return EImageType::SNorm;
			case EType::Int :	return EImageType::Int;
			case EType::UInt :	return EImageType::UInt;
		}

		if ( AllBits( info.valueType, EType::DepthStencil ))	return EImageType::DepthStencil;
		if ( AllBits( info.valueType, EType::Depth ))			return EImageType::Depth;
		if ( AllBits( info.valueType, EType::Stencil ))			return EImageType::Stencil;

		RETURN_ERR( "unknown pixel format" );
	}

/*
=================================================
	EImageType_FromPixelFormatRelaxed
=================================================
*/
	EImageType  EImageType_FromPixelFormatRelaxed (EPixelFormat fmt) __NE___
	{
		using EType = PixelFormatInfo::EType;

		const auto&		info = EPixelFormat_GetInfo( fmt );

		switch ( info.valueType & EType::_ValueMask )
		{
			case EType::SFloat :
			case EType::UFloat :
			case EType::UNorm :
			case EType::SNorm :	return EImageType::Float;

			case EType::Int :	return EImageType::Int;
			case EType::UInt :	return EImageType::UInt;
		}

		if ( AllBits( info.valueType, EType::DepthStencil ))	return EImageType::DepthStencil;
		if ( AllBits( info.valueType, EType::Depth ))			return EImageType::Depth;
		if ( AllBits( info.valueType, EType::Stencil ))			return EImageType::Stencil;

		RETURN_ERR( "unknown pixel format" );
	}

/*
=================================================
	EImageType_FromImage
=================================================
*/
	EImageType  EImageType_FromImage (EImage type, bool ms) __NE___
	{
		switch_enum( type )
		{
			case EImage_1D :			ASSERT( not ms );	return EImageType::Img1D;
			case EImage_2D :								return ms ? EImageType::Img2DMS : EImageType::Img2D;
			case EImage_3D :			ASSERT( not ms );	return EImageType::Img3D;
			case EImage_1DArray :		ASSERT( not ms );	return EImageType::Img1DArray;
			case EImage_2DArray :							return ms ? EImageType::Img2DMSArray : EImageType::Img2DArray;
			case EImage_Cube :			ASSERT( not ms );	return EImageType::ImgCube;
			case EImage_CubeArray :		ASSERT( not ms );	return EImageType::ImgCubeArray;
			case EImage::Unknown :
			case EImage::_Count :
			default :					ASSERT( not ms );	break;
		}
		switch_end
		RETURN_ERR( "unknown image type" );
	}

/*
=================================================
	EImageType_FromImage
=================================================
*/
	String  EImageType_ToString (EImageType type) __Th___
	{
		String	str;
		switch ( type & EImageType::_ValMask )
		{
			case EImageType::sRGB :			str << "F";		break;
			case EImageType::Float :		str << "F";		break;
			case EImageType::SNorm :		str << "SN";	break;
			case EImageType::UNorm :		str << "UN";	break;
			case EImageType::Half :			str << "H";		break;
			case EImageType::Int :			str << "I";		break;
			case EImageType::UInt :			str << "U";		break;
			case EImageType::Depth :		break;
			case EImageType::Stencil :		break;
			case EImageType::DepthStencil :	break;
			default :						RETURN_ERR( "unknown image type" );
		}
		switch ( type & EImageType::_TexMask )
		{
			case EImageType::Img1D :		str << "Image1D";			break;
			case EImageType::Img1DArray :	str << "Image1DArray";		break;
			case EImageType::Img2D :		str << "Image2D";			break;
			case EImageType::Img2DArray :	str << "Image2DArray";		break;
			case EImageType::Img2DMS :		str << "Image2DMS";			break;
			case EImageType::Img2DMSArray :	str << "Image2DMSArray";	break;
			case EImageType::ImgCube :		str << "ImageCube";			break;
			case EImageType::ImgCubeArray :	str << "ImageCubeArray";	break;
			case EImageType::Img3D :		str << "Image3D";			break;
			default :						RETURN_ERR( "unknown image type" );
		}
		switch ( type & EImageType::_ValMask )
		{
			case EImageType::sRGB :			str << "_sRGB";		break;
		}
		switch ( type & EImageType::_QualMask )
		{
			case EImageType::Shadow :		str << "Shadow";	break;
			default :						break;
		}
		return str;
	}

/*
=================================================
	EImageType_ToShaderIO
=================================================
*/
	EShaderIO  EImageType_ToShaderIO (EImageType type) __NE___
	{
		switch ( type & EImageType::_ValMask )
		{
			case EImageType::sRGB :			return EShaderIO::sRGB;
			case EImageType::Float :		return EShaderIO::Float;
			case EImageType::SNorm :		return EShaderIO::SNorm;
			case EImageType::UNorm :		return EShaderIO::UNorm;
			case EImageType::Half :			return EShaderIO::Half;
			case EImageType::Int :			return EShaderIO::Int;
			case EImageType::UInt :			return EShaderIO::UInt;
			case EImageType::Depth :		return EShaderIO::Depth;
			case EImageType::Stencil :		return EShaderIO::Stencil;
			case EImageType::DepthStencil :	return EShaderIO::DepthStencil;
		}
		RETURN_ERR( "unknown image type" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize_Uniform
=================================================
*/
	bool  Deserialize_Uniform (Serializing::Deserializer& des, uint samplerStorageSize, OUT DescriptorSetLayoutDesc::Uniform &un)
	{
		const auto	Deserialize_Image = [&des] (OUT DescriptorSetLayoutDesc::Image &img)
		{{
			return des( OUT img.state, OUT img.type, OUT img.format, OUT img.subpassInputIdx, OUT img.samplerOffsetInStorage );
		}};

		bool	result = true;
		result &= des( OUT un.type, OUT un.stages, OUT un.binding.vkIndex, OUT un.binding.mtlIndex, OUT un.arraySize );

		switch_enum( un.type )
		{
			case EDescriptorType::UniformBuffer :
			case EDescriptorType::StorageBuffer :
				result &= des( OUT un.buffer.state, OUT un.buffer.dynamicOffsetIndex, OUT un.buffer.staticSize,
							   OUT un.buffer.arrayStride, OUT un.buffer.typeName );
				break;

			case EDescriptorType::UniformTexelBuffer :
			case EDescriptorType::StorageTexelBuffer :
				result &= des( OUT un.texelBuffer.state, OUT un.texelBuffer.type );
				break;

			case EDescriptorType::StorageImage :
			case EDescriptorType::SampledImage :
			case EDescriptorType::CombinedImage :
				result &= Deserialize_Image( OUT un.image );
				CHECK_ERR( un.image.samplerOffsetInStorage == UMax );
				CHECK_ERR( un.image.subpassInputIdx == UMax );
				break;

			case EDescriptorType::SubpassInput :
				result &= Deserialize_Image( OUT un.image );
				CHECK_ERR( un.image.samplerOffsetInStorage == UMax );
				CHECK_ERR( un.image.subpassInputIdx != UMax );
				break;

			case EDescriptorType::CombinedImage_ImmutableSampler :
				result &= Deserialize_Image( OUT un.image );
				CHECK_ERR( un.image.samplerOffsetInStorage + un.arraySize <= samplerStorageSize );
				CHECK_ERR( un.image.subpassInputIdx == UMax );
				break;

			case EDescriptorType::Sampler :
				break;

			case EDescriptorType::ImmutableSampler :
				result &= des( OUT un.immutableSampler.offsetInStorage );
				CHECK_ERR( un.immutableSampler.offsetInStorage + un.arraySize <= samplerStorageSize );
				break;

			case EDescriptorType::RayTracingScene :
				break;

			case EDescriptorType::Unknown :
			case EDescriptorType::_Count :
			default :
				RETURN_ERR( "unknown descriptor type" );
		}
		switch_end

		return result;
	}

/*
=================================================
	Deserialize (DescriptorSetLayoutDesc)
=================================================
*/
	bool  DescriptorSetLayoutDesc::Deserialize (Serializing::Deserializer& des) __NE___
	{
		try {
			uniforms.clear();
			samplerStorage.clear();

			UniformOffsets_t	offsets		= {};
			uint				samp_count	= 0;

			CHECK_ERR( des( OUT name, OUT usage, OUT stages, OUT features, OUT offsets, OUT samp_count ));

			const uint	un_count = offsets.back();
			CHECK_ERR( un_count <= MaxUniforms );
			CHECK_ERR( samp_count <= MaxSamplers );

			if ( samp_count )
			{
				samplerStorage.resize( samp_count );	// throw

				bool	result = true;
				for (auto& id : samplerStorage)
				{
					result &= des( OUT id );
				}
				CHECK_ERR( result );
			}

			uniforms.resize( un_count );	// throw

			bool	result = true;
			for (auto& [un_name, un] : uniforms)
			{
				result &= des( OUT un_name );
			}
			CHECK_ERR( result );

			for (auto& [un_name, un] : uniforms)
			{
				result &= Deserialize_Uniform( des, samp_count, OUT un );

				if ( not result )
					break;
			}
			CHECK_ERR( result );
			return true;
		}
		catch (...) {
			return false;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (PushConstants)
=================================================
*/
	bool  PushConstants::Deserialize (Serializing::Deserializer& des) __NE___
	{
		try {
			bool	result	= true;
			uint	count	= 0;

			items.clear();

			result &= des( OUT count );

			for (uint i = 0; i < count; ++i)
			{
				PushConstantName	name;
				PushConst			data;

				result &= des( OUT name, OUT data.typeName, OUT data.stage, OUT data.metalBufferId, OUT data.vulkanOffset, OUT data.size );

				items.emplace( name, data );
			}
			return result;
		}
		catch (...) {
			return false;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (PipelineLayoutDesc)
=================================================
*/
	bool  PipelineLayoutDesc::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT descrSets, OUT pushConstants );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableRenderState)
=================================================
*/
	bool  SerializableRenderState::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT rs );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableDepthStencilState)
=================================================
*/
	bool  SerializableDepthStencilState::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT ds );
	}
//-----------------------------------------------------------------------------



namespace {
/*
=================================================
	Deserialize_BasePipelineDesc
=================================================
*/
	static bool  Deserialize_BasePipelineDesc (Serializing::Deserializer& des, OUT BasePipelineDesc &base)
	{
		return des( OUT base.specialization, OUT base.options, OUT base.dynamicState );
	}

} // namespace

/*
=================================================
	Deserialize (SerializableGraphicsPipeline)
=================================================
*/
	bool  SerializableGraphicsPipeline::Deserialize (Serializing::Deserializer& des) __NE___
	{
		bool	result = true;
		result &= des( OUT features, OUT layout, OUT shaders, OUT supportedTopology );
		result &= des( OUT vertexAttribs );
		result &= des( OUT patchControlPoints, OUT earlyFragmentTests );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableGraphicsPipelineSpec)
=================================================
*/
	bool  SerializableGraphicsPipelineSpec::Deserialize (Serializing::Deserializer &des) __NE___
	{
		StaticAssert64( (sizeof(desc) - sizeof(BasePipelineDesc)) == AlignUp( 8+4+4+16+16+1, 8 ));

		bool	result = true;
		result &= des( OUT templUID, OUT rStateUID, OUT dsStateUID );
		result &= Deserialize_BasePipelineDesc( des, OUT desc );
		// skip renderStatePtr
		result &= des( OUT desc.renderPass, OUT desc.subpass );
		result &= des( OUT desc.vertexInput, OUT desc.vertexBuffers, OUT desc.viewportCount );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableComputePipeline)
=================================================
*/
	bool  SerializableComputePipeline::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT features, OUT layout, OUT shader, OUT defaultLocalSize, OUT localSizeSpec );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableComputePipelineSpec)
=================================================
*/
	bool  SerializableComputePipelineSpec::Deserialize (Serializing::Deserializer &des) __NE___
	{
		StaticAssert64( (sizeof(desc) - sizeof(BasePipelineDesc)) == AlignUp( 8, 8 ));

		bool	result = true;
		result &= des( OUT templUID );
		result &= Deserialize_BasePipelineDesc( des, OUT desc );
		result &= des( OUT desc.localSize );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableTilePipeline)
=================================================
*/
	bool  SerializableTilePipeline::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT features, OUT layout, OUT shader, OUT defaultLocalSize, OUT localSizeSpec );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableTilePipelineSpec)
=================================================
*/
	bool  SerializableTilePipelineSpec::Deserialize (Serializing::Deserializer &des) __NE___
	{
		StaticAssert64( (sizeof(desc) - sizeof(BasePipelineDesc)) == AlignUp( 4+4+4, 8 ));

		bool	result = true;
		result &= des( OUT templUID );
		result &= des( OUT desc.renderPass, OUT desc.subpass, OUT desc.localSize );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableMeshPipeline)
=================================================
*/
	bool  SerializableMeshPipeline::Deserialize (Serializing::Deserializer& des) __NE___
	{
		bool	result = true;
		result &= des( OUT features, OUT layout, OUT shaders, OUT outputTopology, OUT maxVertices, OUT maxIndices );
		result &= des( OUT taskDefaultLocalSize, OUT taskLocalSizeSpec, OUT meshDefaultLocalSize, OUT meshLocalSizeSpec, OUT earlyFragmentTests );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableMeshPipelineSpec)
=================================================
*/
	bool  SerializableMeshPipelineSpec::Deserialize (Serializing::Deserializer &des) __NE___
	{
		StaticAssert64( (sizeof(desc) - sizeof(BasePipelineDesc)) == AlignUp( 8+4+4+2+6+6, 8 ));

		bool	result = true;
		result &= des( OUT templUID, OUT rStateUID, OUT dsStateUID );
		result &= Deserialize_BasePipelineDesc( des, OUT desc );
		// skip renderStatePtr
		result &= des( OUT desc.renderPass, OUT desc.subpass, OUT desc.viewportCount );
		result &= des( OUT desc.taskLocalSize, OUT desc.meshLocalSize );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableRayTracingPipeline)
=================================================
*/
	bool  SerializableRayTracingPipeline::Deserialize (Serializing::Deserializer& des) __NE___
	{
		bool	result = true;
		result &= des( OUT features, OUT layout, OUT shaderArr );
		result &= des( OUT generalShaders, OUT triangleGroups, OUT proceduralGroups );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableRayTracingPipelineSpec)
=================================================
*/
	bool  SerializableRayTracingPipelineSpec::Deserialize (Serializing::Deserializer &des) __NE___
	{
		StaticAssert64( (sizeof(desc) - sizeof(BasePipelineDesc)) == AlignUp( 4+4+4, 8 ));

		bool	result = true;
		result &= des( OUT templUID );
		result &= Deserialize_BasePipelineDesc( des, OUT desc );
		result &= des( OUT desc.maxRecursionDepth, OUT desc.maxPipelineRayPayloadSize, OUT desc.maxPipelineRayHitAttributeSize );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableRenderTechnique)
=================================================
*/
	bool  SerializableRenderTechnique::Pass::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT name, OUT dsLayout, OUT renderPass, OUT subpass );
	}

	bool  SerializableRenderTechnique::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT name, OUT features, OUT passes, OUT pipelines, OUT rtSBTs );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (SerializableRTShaderBindingTable)
=================================================
*/
	bool  SerializableRTShaderBindingTable::Deserialize (Serializing::Deserializer& des) __NE___
	{
		return des( OUT pplnName, OUT raygen, OUT miss, OUT hit, OUT callable, OUT numRayTypes );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize (ShaderBytecode)
=================================================
*/
	bool  ShaderBytecode::Deserialize (Serializing::Deserializer& des) __NE___
	{
		uint	off = 0;

		if ( des( OUT off, OUT dataSize, OUT data2Size, OUT typeIdx ))
		{
			offset = Bytes{off};
			return true;
		}
		return false;
	}

/*
=================================================
	ReadData
=================================================
*/
	bool  ShaderBytecode::ReadData (RStream &stream) __NE___
	{
		using Types_t = TypeList< Bytecode_t >;	// union to typelist

		CHECK_ERR( dataSize > 0 );
		CHECK_ERR( stream.SeekSet( offset ));

		switch ( typeIdx )
		{
			case Types_t::Index< SpirvBytecode_t > :
			{
				SpirvBytecode_t		spirv;
				CHECK_ERR( data2Size == 0 );
				CHECK_ERR( _ReadSpirvData( dataSize, stream, OUT spirv ));
				code = RVRef(spirv);
				break;
			}
			case Types_t::Index< MetalBytecode_t > :
			{
				MetalBytecode_t		mtbc;
				CHECK_ERR( data2Size == 0 );
				CHECK_ERR( _ReadMetalData( stream, OUT mtbc ));
				code = RVRef(mtbc);
				break;
			}
			case Types_t::Index< SpirvWithTrace > :
			{
				SpirvWithTrace		dbg_spirv;
				CHECK_ERR( data2Size > 0 );
				CHECK_ERR( _ReadDbgSpirvData( stream, OUT dbg_spirv ));
				code = RVRef(dbg_spirv);
				break;
			}
			default :
				RETURN_ERR( "unsupported type index" );
		}
		return true;
	}

/*
=================================================
	_ReadSpirvData
=================================================
*/
	bool  ShaderBytecode::_ReadSpirvData (Bytes spvDataSize, RStream &stream, OUT SpirvBytecode_t &outCode) __NE___
	{
		CHECK_ERR( stream.Read( spvDataSize, OUT outCode ));

		const uint	spec_count	= outCode.back();
		CHECK_ERR( spec_count <= GraphicsConfig::MaxSpecConstants );
		CHECK_ERR( outCode.size() > (spec_count*2 + 1) );

		const uint*	ptr = &outCode[ outCode.size() - spec_count*2 - 1 ];

		for (uint i = 0; i < spec_count; ++i, ptr += 2)
		{
			spec.emplace( SpecializationName{HashVal32{ptr[0]}}, ptr[1] );
		}

		NOTHROW_ERR( outCode.resize( outCode.size() - spec_count*2 - 1 ));
		return true;
	}

/*
=================================================
	_ReadDbgSpirvData
=================================================
*/
	bool  ShaderBytecode::_ReadDbgSpirvData (RStream &stream, OUT SpirvWithTrace &outCode) __NE___
	{
		try{
			CHECK_ERR( _ReadSpirvData( dataSize, stream, OUT outCode.bytecode ));

			const Bytes	start		= stream.Position();
			auto		buf_stream	= MakeRC<BufferedRStream>( stream.GetRC<RStream>() );
			{
				Serializing::Deserializer	des{ buf_stream };
				outCode.trace = MakeUnique<ShaderTrace>();

				if_unlikely( not outCode.trace->Deserialize( des ))
				{
					outCode.trace.reset();
					return true;
				}

				const Bytes	trace_size = (buf_stream->Position() - des.stream.RemainingSize()) - start;
				CHECK_ERR( Bytes{data2Size} == trace_size );
			}
			return true;
		}
		catch(...) {
			RETURN_ERR( "Failed to load shader trace" );
		}
	}

/*
=================================================
	_ReadMetalData
=================================================
*/
	bool  ShaderBytecode::_ReadMetalData (RStream &stream, OUT MetalBytecode_t &outCode) __NE___
	{
		CHECK_ERR( stream.Read( dataSize, OUT outCode ));

		const auto	ucode = ArrayView<ubyte>{ outCode }.Cast<uint>();

		const uint	spec_count	= ucode.back();
		CHECK_ERR( spec_count <= GraphicsConfig::MaxSpecConstants );

		const Bytes	spec_size	{/*count*/sizeof(uint) + (sizeof(uint) * spec_count*2)};
		CHECK_ERR( spec_size < Bytes{dataSize} );

		const uint*	ptr			= &ucode[ ucode.size() - spec_count*2 - 1 ];

		for (uint i = 0; i < spec_count; ++i, ptr += 2)
		{
			spec.emplace( SpecializationName{HashVal32{ptr[0]}}, ptr[1] );
		}

		NOTHROW_ERR( outCode.resize( outCode.size() - usize{spec_size} ));
		return true;
	}
//-----------------------------------------------------------------------------


# ifndef AE_BUILD_PIPELINE_COMPILER

	bool  DescriptorSetLayoutDesc::Serialize (Serializing::Serializer &)			C_NE___ { return false; }

	bool  PushConstants::Serialize (Serializing::Serializer &)						C_NE___ { return false; }

	bool  PipelineLayoutDesc::Serialize (Serializing::Serializer &)					C_NE___ { return false; }

	bool  SerializableRenderState::Serialize (Serializing::Serializer &)			C_NE___ { return false; }

	bool  SerializableDepthStencilState::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  SerializableGraphicsPipeline::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  SerializableComputePipeline::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  SerializableTilePipeline::Serialize (Serializing::Serializer &)			C_NE___ { return false; }

	bool  SerializableMeshPipeline::Serialize (Serializing::Serializer &)			C_NE___ { return false; }

	bool  SerializableRayTracingPipeline::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  SerializableGraphicsPipelineSpec::Serialize (Serializing::Serializer &)	C_NE___ { return false; }

	bool  SerializableComputePipelineSpec::Serialize (Serializing::Serializer &)	C_NE___ { return false; }

	bool  SerializableTilePipelineSpec::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  SerializableMeshPipelineSpec::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  SerializableRayTracingPipelineSpec::Serialize (Serializing::Serializer &)	C_NE___ { return false; }

	bool  SerializableRTShaderBindingTable::Serialize (Serializing::Serializer &)	C_NE___ { return false; }

	bool  SerializableRenderTechnique::Pass::Serialize (Serializing::Serializer &)	C_NE___ { return false; }

	bool  SerializableRenderTechnique::Serialize (Serializing::Serializer &)		C_NE___ { return false; }

	bool  ShaderBytecode::Serialize (Serializing::Serializer &)						C_NE___ { return false; }

# endif // AE_BUILD_PIPELINE_COMPILER
//-----------------------------------------------------------------------------



# ifdef AE_TEST_PIPELINE_COMPILER
/*
=================================================
	EDescriptorTypeToString
=================================================
*/
	ND_ inline StringView  EDescriptorTypeToString (EDescriptorType type)
	{
		switch_enum( type )
		{
			case EDescriptorType::UniformBuffer :					return "UniformBuffer";
			case EDescriptorType::StorageBuffer :					return "StorageBuffer";
			case EDescriptorType::UniformTexelBuffer :				return "UniformTexelBuffer";
			case EDescriptorType::StorageTexelBuffer :				return "StorageTexelBuffer";
			case EDescriptorType::StorageImage :					return "StorageImage";
			case EDescriptorType::SampledImage :					return "SampledImage";
			case EDescriptorType::CombinedImage :					return "CombinedImage";
			case EDescriptorType::SubpassInput :					return "SubpassInput";
			case EDescriptorType::CombinedImage_ImmutableSampler:	return "CombinedImage_ImmutableSampler";
			case EDescriptorType::Sampler :							return "Sampler";
			case EDescriptorType::ImmutableSampler :				return "ImmutableSampler";
			case EDescriptorType::RayTracingScene :					return "RayTracingScene";
			case EDescriptorType::Unknown :
			case EDescriptorType::_Count :
			default :												break;
		}
		switch_end
		RETURN_ERR( "unknown descriptor type" );
	}

/*
=================================================
	ToString (DescriptorSetLayoutDesc)
=================================================
*/
	String  DescriptorSetLayoutDesc::ToString (const HashToName &nameMap) const
	{
		const bool	is_argbuf = AllBits( usage, EDescSetUsage::ArgumentBuffer );

		String	str;
		str << "\n    name   = '" << nameMap( name ) << "'"
			<< "\n    usage  = " << Base::ToString( usage )
			<< "\n    stages = " << Base::ToString( stages );

		usize	idx = 0;
		for (auto& [un_name, un] : uniforms)
		{
			str << "\n    [" << Base::ToString(idx++) << "] " << EDescriptorTypeToString( un.type ) << " {"
				<< "\n      name         = '" << nameMap( un_name ) << "'";

			if ( un.binding.vkIndex != UMax )
				str << "\n      vkIndex      = " << Base::ToString( un.binding.vkIndex );

			if ( un.binding.IsMetalDefined() )
			{
				if ( un.type == EDescriptorType::ImmutableSampler and un.binding.mtlIndex == UnassignedIdx )
				{}
				else
				{
					str << "\n      mtlIndex     = ";
					if ( is_argbuf )
						str << Base::ToString( un.binding.mtlIndex );
					else
					{
						EShaderStages	tmp_stages = un.stages;
						while ( tmp_stages != Default )
						{
							EShaderStages	stage	= ExtractBit( INOUT tmp_stages );

							str << Base::ToString( stage ) << ": " << Base::ToString( un.binding.mtlPerStageIndex.ForShader( stage ));

							if ( tmp_stages != Default )
								str << ", ";
						}
					}
				}
			}

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
					str << "\n      state        = " << Base::ToString( un.buffer.state );
					if ( un.buffer.HasDynamicOffset() )
						str << "\n      dynamicOffsetIndex  = " << Base::ToString( un.buffer.dynamicOffsetIndex );
					str	<< "\n      staticSize   = " << Base::ToString( un.buffer.staticSize )
						<< "\n      arrayStride  = " << Base::ToString( un.buffer.arrayStride )
						<< "\n      typeName     = " << nameMap( un.buffer.typeName );
					break;

				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :
					str << "\n      state        = " << Base::ToString( un.texelBuffer.state );
					break;

				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage :
					str << "\n      state        = " << Base::ToString( un.image.state )
						<< "\n      type         = " << EImageType_ToString( un.image.type )
						<< "\n      format       = " << Base::ToString( un.image.format );
					break;

				case EDescriptorType::SubpassInput :
					str << "\n      state        = " << Base::ToString( un.image.state )
						<< "\n      type         = " << EImageType_ToString( un.image.type )
						<< "\n      format       = " << Base::ToString( un.image.format )
						<< "\n      index        = " << Base::ToString( un.image.subpassInputIdx );
					break;

				case EDescriptorType::CombinedImage_ImmutableSampler :
					CHECK_ERR( un.image.samplerOffsetInStorage + un.arraySize <= samplerStorage.size() );

					str << "\n      state        = " << Base::ToString( un.image.state )
						<< "\n      type         = " << EImageType_ToString( un.image.type )
						<< "\n      format       = " << Base::ToString( un.image.format )
						<< "\n      sampler      = { ";
					for (usize i = 0; i < un.arraySize; ++i) {
						if ( i > 0 ) str << ", ";
						str << "'" << nameMap( samplerStorage[ un.image.samplerOffsetInStorage + i ]) << "'";
					}
					str << " }";
					break;

				case EDescriptorType::Sampler :
					break;

				case EDescriptorType::ImmutableSampler :
					CHECK_ERR( un.immutableSampler.offsetInStorage + un.arraySize <= samplerStorage.size() );

					str << "\n      sampler      = { ";
					for (usize i = 0; i < un.arraySize; ++i) {
						if ( i > 0 ) str << ", ";
						str << "'" << nameMap( samplerStorage[ un.immutableSampler.offsetInStorage + i ]) << "'";
					}
					str << " }";
					break;

				case EDescriptorType::RayTracingScene :
					break;

				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					RETURN_ERR( "unknown descriptor type" );
			}
			switch_end
			str << "\n    }";
		}
		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (PushConstants)
=================================================
*/
	String  PushConstants::ToString (const HashToName &nameMap) const
	{
		String	str;
		usize	idx = 0;
		for (auto el : items)
		{
			str << "\n    [" << Base::ToString(idx++) << "] PushConst {"
				<< "\n      name     = '" << nameMap( el.first ) << "'"
				<< "\n      typename = '" << nameMap( el.second.typeName ) << "'"
				<< "\n      stage    = " << Base::ToString( el.second.stage )
				<< "\n      bufferId = " << Base::ToString( el.second.metalBufferId )
				<< "\n      offset   = " << Base::ToString( el.second.vulkanOffset )
				<< "\n      size     = " << Base::ToString( el.second.size )
				<< "\n    }";
		}
		return str;
	}

/*
=================================================
	ToString (PipelineLayoutDesc)
=================================================
*/
	String  PipelineLayoutDesc::ToString (const HashToName &nameMap) const
	{
		String	str;
		usize	idx = 0;
		for (auto ds : descrSets)
		{
			str << "\n    [" << Base::ToString(idx++) << "] DescrSet {"
				<< "\n      name     = '" << nameMap( ds.first ) << "'"
				<< "\n      uid      = " << Base::ToString<16>(usize(ds.second.uid));

			if ( ds.second.vkIndex != UMax )
				str << "\n      vkIndex  = " << Base::ToString(ds.second.vkIndex);

			if ( ds.second.mtlIndex.IsDefined() )
			{
				str << "\n      mtlIndex =";
				if ( ds.second.mtlIndex.Has(0) )					str << " v/t/c/m: " << Base::ToString(ds.second.mtlIndex.Get(0));
				if ( ds.second.mtlIndex.Has(1) )					str << " f: " << Base::ToString(ds.second.mtlIndex.Get(1));
				if ( ds.second.mtlIndex.Has(2) )					str << " mt: " << Base::ToString(ds.second.mtlIndex.Get(2));
				if ( ds.second.mtlIndex.BindingIndex() != UMax )	str << " idx: " << Base::ToString(ds.second.mtlIndex.BindingIndex());
			}
			str << "\n    }";
		}

		str << pushConstants.ToString( nameMap );
		str << "\n  ----------------------";

		return str;
	}

/*
=================================================
	DepthBufferState_ToString
=================================================
*/
	static String  DepthBufferState_ToString (const RenderState::DepthBufferState &d)
	{
		String	str;
		str << "\n      depth = {";

		if ( d.bounds )
		{
			str << "\n        bounds    = " << Base::ToString( d.bounds )
				<< "\n        minBounds = " << Base::ToString( d.minBounds, 2 )
				<< "\n        maxBounds = " << Base::ToString( d.maxBounds, 2 );
		}
		str << "\n        compareOp = " << Base::ToString( d.compareOp )
			<< "\n        write     = " << Base::ToString( d.write )
			<< "\n        test      = " << Base::ToString( d.test );
		str << "\n      }";

		return str;
	}

/*
=================================================
	StencilBufferState_ToString
=================================================
*/
	static String  StencilBufferState_ToString (const RenderState::StencilBufferState &s)
	{
		String		str;
		if ( s.enabled )
		{
			const auto	FaceToStr = [] (StringView prefix, const RenderState::StencilFaceState &sf)
			{{
				String	res;
				res << prefix << "failOp      = " << Base::ToString( sf.failOp )
					<< prefix << "depthFailOp = " << Base::ToString( sf.depthFailOp )
					<< prefix << "passOp      = " << Base::ToString( sf.passOp )
					<< prefix << "compareOp   = " << Base::ToString( sf.compareOp )
					<< prefix << "reference   = " << Base::ToString( uint(sf.reference) )
					<< prefix << "writeMask   = " << Base::ToString( uint(sf.writeMask) )
					<< prefix << "compareMask = " << Base::ToString( uint(sf.compareMask) );
				return res;
			}};

			str << "\n      stencil = {";
			if ( s.front == s.back )
			{
				str << FaceToStr( "\n        ", s.front );
			}
			else
			{
				str << "\n        front = {"
					<< FaceToStr( "\n          ", s.front )
					<< "\n        }"
					<< "\n        back = {"
					<< FaceToStr( "\n          ", s.back )
					<< "\n        }";
			}
			str << "\n      }";
		}
		return str;
	}

/*
=================================================
	RenderState_ToString
=================================================
*/
	static String  RenderState_ToString (const RenderState &rs)
	{
		String	str;
		str << "\n    renderState = {";

		if ( rs.color != RenderState::ColorBuffersState{} )
		{
			const auto&	cb = rs.color;

			str << "\n      color = {"
				<< "\n        logicOp    = " << Base::ToString( cb.logicOp )
				<< "\n        blendColor = " << Base::ToString( cb.blendColor );

			for (usize i = 0; i < cb.buffers.size(); ++i)
			{
				const auto&	c = cb.buffers[i];
				if ( not c.blend and c.colorMask.All() )
					continue;

				str << "\n        [" << Base::ToString( i ) << "]";
				if ( c.blend )
				{
					str << "\n          srcBlendFactor = ";
					if ( c.srcBlendFactor.color != c.srcBlendFactor.alpha )
						str << "{ " << Base::ToString( c.srcBlendFactor.color ) << ", " << Base::ToString( c.srcBlendFactor.alpha ) << " }";
					else
						str << Base::ToString( c.srcBlendFactor.color );

					str << "\n          dstBlendFactor = ";
					if ( c.dstBlendFactor.color != c.dstBlendFactor.alpha )
						str << "{ " << Base::ToString( c.dstBlendFactor.color ) << ", " << Base::ToString( c.dstBlendFactor.alpha ) << " }";
					else
						str << Base::ToString( c.dstBlendFactor.color );

					str << "\n          blendOp        = ";
					if ( c.blendOp.color != c.blendOp.alpha )
						str << "{  " << Base::ToString( c.blendOp.color ) << ", " << Base::ToString( c.blendOp.alpha ) << " }";
					else
						str << Base::ToString( c.blendOp.color );
				}

				if ( not c.colorMask.All() )
				{
					str << "\n          colorMask = { ";
					if ( c.colorMask.r )	str << "R |";
					if ( c.colorMask.g )	str << "G |";
					if ( c.colorMask.b )	str << "B |";
					if ( c.colorMask.a )	str << "A |";

					if ( c.colorMask.Any() ) {
						str.pop_back();
						str.pop_back();
					}
					str << " }";
				}
			}
			str << "\n      }";
		}

		//if ( rs.depth != RenderState::DepthBufferState{} )
			str << DepthBufferState_ToString( rs.depth );

		if ( rs.stencil != RenderState::StencilBufferState{} )
			str << StencilBufferState_ToString( rs.stencil );

		if ( rs.inputAssembly != RenderState::InputAssemblyState{} )
		{
			const auto&	ia = rs.inputAssembly;
			str << "\n      inputAssembly = {"
				<< "\n        topology         = " << Base::ToString( ia.topology )
				<< "\n        primitiveRestart = " << Base::ToString( ia.primitiveRestart )
				<< "\n      }";
		}

		if ( rs.rasterization != RenderState::RasterizationState{} )
		{
			const auto&	r = rs.rasterization;
			str << "\n      rasterization = {"
				<< "\n        depthBiasConstFactor = " << Base::ToString( r.depthBiasConstFactor, 2 )
				<< "\n        depthBiasClamp       = " << Base::ToString( r.depthBiasClamp, 2 )
				<< "\n        depthBiasSlopeFactor = " << Base::ToString( r.depthBiasSlopeFactor, 2 )
				<< "\n        depthBias            = " << Base::ToString( r.depthBias )
				<< "\n        polygonMode          = " << Base::ToString( r.polygonMode )
				<< "\n        depthClamp           = " << Base::ToString( r.depthClamp )
				<< "\n        rasterizerDiscard    = " << Base::ToString( r.rasterizerDiscard )
				<< "\n        frontFaceCCW         = " << Base::ToString( r.frontFaceCCW )
				<< "\n        cullMode             = " << Base::ToString( r.cullMode )
				<< "\n      }";
		}

		if ( rs.multisample != RenderState::MultisampleState{} )
		{
			const auto&	ms = rs.multisample;
			str << "\n      multisample = {"
				<< "\n        sampleMask       = " << Base::ToString<16>( ms.sampleMask )
				<< "\n        minSampleShading = " << Base::ToString( ms.minSampleShading )
				<< "\n        samples          = " << Base::ToString( ms.samples.Get() )
				<< "\n        sampleShading    = " << Base::ToString( ms.sampleShading )
				<< "\n        alphaToCoverage  = " << Base::ToString( ms.alphaToCoverage )
				<< "\n        alphaToOne       = " << Base::ToString( ms.alphaToOne )
				<< "\n      }";
		}
		str << "\n    }";
		return str;
	}

/*
=================================================
	ToString (SerializableRenderState)
=================================================
*/
	String  SerializableRenderState::ToString (const HashToName &) const
	{
		return RenderState_ToString( rs );
	}

/*
=================================================
	ToString (SerializableDepthStencilState)
=================================================
*/
	String  SerializableDepthStencilState::ToString (const HashToName &) const
	{
		String	str;
		str << "\n    depthStencilState = {";

		//if ( ds.depth != RenderState::DepthBufferState{} )
			str << DepthBufferState_ToString( ds.depth );

		if ( ds.stencil != RenderState::StencilBufferState{} )
			str << StencilBufferState_ToString( ds.stencil );

		str << "\n    }";
		return str;
	}

/*
=================================================
	TopologyBitsToString
=================================================
*/
	ND_ inline String  TopologyBitsToString (const SerializableGraphicsPipeline::TopologyBits_t topology)
	{
		String	str;
		for (auto value : BitIndexIterate<EPrimitive>( topology.AsBits() ))
		{
			if ( not str.empty() )
				str << " | ";

			str << ToString( value );
		}
		return str;
	}

/*
=================================================
	ToString (SerializableGraphicsPipeline)
=================================================
*/
	String  SerializableGraphicsPipeline::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    layout  = " << Base::ToString(usize( layout ))
			<< "\n    shaders = {";

		for (auto sh : shaders) {
			str << "\n      " << Base::ToString( sh.first ) << " = " << Base::ToString<16>(usize( sh.second ));
		}
		str << "\n    }"
			<< "\n    topology      = " << TopologyBitsToString( supportedTopology );

		if ( not vertexAttribs.empty() )
		{
			str << "\n    vertexAttribs = {";
			for (usize i = 0; i < vertexAttribs.size(); ++i)
			{
				const auto&	va = vertexAttribs[i];

				if ( i > 0 )
					str << "\n      -------------";

				str << "\n      type   = " << Base::ToString( va.type )
					<< "\n      index  = " << Base::ToString(uint( va.index ));
			}
			str << "\n    }";
		}

		str << "\n    patchControlPoints = " << Base::ToString( patchControlPoints )
			<< "\n    earlyFragmentTests = " << Base::ToString( earlyFragmentTests );

		if ( not features.empty() )
		{
			str << "\n    features = { ";
			for (auto feat : features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (SerializableMeshPipeline)
=================================================
*/
	String  SerializableMeshPipeline::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    layout  = " << Base::ToString(usize( layout ))
			<< "\n    shaders = {";

		for (auto sh : shaders) {
			str << "\n      " << Base::ToString( sh.first ) << " = " << Base::ToString<16>(usize( sh.second ));
		}
		str << "\n    }"
			<< "\n    outputTopology       = " << Base::ToString( outputTopology )
			<< "\n    maxVertices          = " << Base::ToString( maxVertices )
			<< "\n    maxIndices           = " << Base::ToString( maxIndices )
			<< "\n    earlyFragmentTests   = " << Base::ToString( earlyFragmentTests );

		if ( Any( taskDefaultLocalSize != Zero ))
		{
			str << "\n    taskDefaultLocalSize = { "
				<< (taskDefaultLocalSize.x == 0 ? "Undefined"s : Base::ToString( taskDefaultLocalSize.x )) << ", "
				<< (taskDefaultLocalSize.y == 0 ? "Undefined"s : Base::ToString( taskDefaultLocalSize.y )) << ", "
				<< (taskDefaultLocalSize.z == 0 ? "Undefined"s : Base::ToString( taskDefaultLocalSize.z )) << " }";
		}
		if ( Any( taskLocalSizeSpec != UMax ))
		{
			str << "\n    taskLocalSizeSpec    = { "
					<< (taskLocalSizeSpec.x == UMax ? "Undefined"s : Base::ToString( taskLocalSizeSpec.x )) << ", "
					<< (taskLocalSizeSpec.y == UMax ? "Undefined"s : Base::ToString( taskLocalSizeSpec.y )) << ", "
					<< (taskLocalSizeSpec.z == UMax ? "Undefined"s : Base::ToString( taskLocalSizeSpec.z )) << " }";
		}
		if ( Any( meshDefaultLocalSize != Zero ))
		{
			str << "\n    meshDefaultLocalSize = { "
					<< (meshDefaultLocalSize.x == 0 ? "Undefined"s : Base::ToString( meshDefaultLocalSize.x )) << ", "
					<< (meshDefaultLocalSize.y == 0 ? "Undefined"s : Base::ToString( meshDefaultLocalSize.y )) << ", "
					<< (meshDefaultLocalSize.z == 0 ? "Undefined"s : Base::ToString( meshDefaultLocalSize.z )) << " }";
		}
		if ( Any( meshLocalSizeSpec != UMax ))
		{
			str << "\n    meshLocalSizeSpec    = { "
					<< (meshLocalSizeSpec.x == UMax ? "Undefined"s : Base::ToString( meshLocalSizeSpec.x )) << ", "
					<< (meshLocalSizeSpec.y == UMax ? "Undefined"s : Base::ToString( meshLocalSizeSpec.y )) << ", "
					<< (meshLocalSizeSpec.z == UMax ? "Undefined"s : Base::ToString( meshLocalSizeSpec.z )) << " }";
		}

		if ( not features.empty() )
		{
			str << "\n    features          = { ";
			for (auto feat : features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (SerializableComputePipeline)
=================================================
*/
	String  SerializableComputePipeline::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    layout  = " << Base::ToString(usize( layout ))
			<< "\n    shader  = " << Base::ToString<16>(usize( shader ));

		if ( Any( defaultLocalSize != Zero ))
		{
			str << "\n    defaultLocalSize = { "
					<< (defaultLocalSize.x == 0 ? "Undefined"s : Base::ToString( defaultLocalSize.x )) << ", "
					<< (defaultLocalSize.y == 0 ? "Undefined"s : Base::ToString( defaultLocalSize.y )) << ", "
					<< (defaultLocalSize.z == 0 ? "Undefined"s : Base::ToString( defaultLocalSize.z )) << " }";
		}
		if ( Any( localSizeSpec != UMax ))
		{
			str << "\n    localSizeSpec = { "
					<< (localSizeSpec.x == UMax ? "Undefined"s : Base::ToString( localSizeSpec.x )) << ", "
					<< (localSizeSpec.y == UMax ? "Undefined"s : Base::ToString( localSizeSpec.y )) << ", "
					<< (localSizeSpec.z == UMax ? "Undefined"s : Base::ToString( localSizeSpec.z )) << " }";
		}

		if ( not features.empty() )
		{
			str << "\n    features = { ";
			for (auto feat : features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (SerializableTilePipeline)
=================================================
*/
	String  SerializableTilePipeline::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    layout  = " << Base::ToString(usize( layout ))
			<< "\n    shader  = " << Base::ToString<16>(usize( shader ));

		if ( Any( defaultLocalSize != Zero ))
		{
			str << "\n    defaultLocalSize = { "
				<< (defaultLocalSize.x == 0 ? "Undefined"s : Base::ToString( defaultLocalSize.x )) << ", "
				<< (defaultLocalSize.y == 0 ? "Undefined"s : Base::ToString( defaultLocalSize.y )) << " }";
		}
		if ( Any( localSizeSpec != UMax ))
		{
			str << "\n    localSizeSpec = { "
				<< (localSizeSpec.x == UMax ? "Undefined"s : Base::ToString( localSizeSpec.x )) << ", "
				<< (localSizeSpec.y == UMax ? "Undefined"s : Base::ToString( localSizeSpec.y )) << " }";
		}

		if ( not features.empty() )
		{
			str << "\n    features = { ";
			for (auto feat : features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (SerializableRayTracingPipeline)
=================================================
*/
	String  SerializableRayTracingPipeline::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    layout  = " << Base::ToString(usize( layout ));

		if ( not features.empty() )
		{
			str << "\n    features = { ";
			for (auto feat : features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}

		if ( not generalShaders.empty() )
		{
			str << "\n    generalShaders = {";
			uint	idx = 0;
			for (auto& group : generalShaders)
			{
				str << "\n      [" << Base::ToString(idx++) << "] {"
					<< "\n        name   = '" << nameMap( group.name ) << "'"
					<< "\n        shader = " << (group.shader < shaderArr.size() ? Base::ToString<16>( shaderArr[group.shader].Get<0>() ) : "none"s)
					<< "\n      }";
			}
			str << "\n    }";
		}

		if ( not triangleGroups.empty() )
		{
			str << "\n    triangleGroups = {";
			uint	idx = 0;
			for (auto& group : triangleGroups)
			{
				str << "\n      [" << Base::ToString(idx++) << "] {"
					<< "\n        name       = '" << nameMap( group.name ) << "'"
					<< "\n        closestHit = " << (group.closestHit < shaderArr.size() ? Base::ToString<16>( shaderArr[group.closestHit].Get<0>() ) : "none"s)
					<< "\n        anyHit     = " << (group.anyHit     < shaderArr.size() ? Base::ToString<16>( shaderArr[group.anyHit].Get<0>()     ) : "none"s)
					<< "\n      }";
			}
			str << "\n    }";
		}

		if ( not proceduralGroups.empty() )
		{
			str << "\n    proceduralGroups = {";
			uint	idx = 0;
			for (auto& group : proceduralGroups)
			{
				str << "\n      [" << Base::ToString(idx++) << "] {"
					<< "\n        name         = '" << nameMap( group.name ) << "'"
					<< "\n        intersection = " << (group.intersection < shaderArr.size() ? Base::ToString<16>( shaderArr[group.intersection].Get<0>() ) : "none"s)
					<< "\n        closestHit   = " << (group.closestHit   < shaderArr.size() ? Base::ToString<16>( shaderArr[group.closestHit].Get<0>()   ) : "none"s)
					<< "\n        anyHit       = " << (group.anyHit       < shaderArr.size() ? Base::ToString<16>( shaderArr[group.anyHit].Get<0>()       ) : "none"s)
					<< "\n      }";
			}
			str << "\n    }";
		}

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (SerializableRenderTechnique)
=================================================
*/
	String  SerializableRenderTechnique::ToString (const HashToName &nameMap) const
	{
		String	str;

		str << "\n    name     = '" << nameMap( name ) << "'"
			<< "\n    features = {";
		if ( not features.empty() )
		{
			str << "\n      ";
			for (auto& fs : features) {
				str << "'" << nameMap( fs ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << "\n    ";
		}
		str << "}";
		str << "\n    passes = {";
		if ( not passes.empty() )
		{
			uint	idx = 0;
			for (auto& pass : passes)
			{
				str << "\n      [" << Base::ToString(idx++) << "] Pass {"
					<< "\n        name       = '" << nameMap( pass.name ) << "'";

				if ( pass.dsLayout != Default )
					str << "\n        dsLayout   = " << Base::ToString( usize(pass.dsLayout) );

				if ( pass.IsGraphics() )
				{
					str << "\n        renderPass = '" << nameMap( pass.renderPass ) << "'"
						<< "\n        subpass    = '" << nameMap( pass.subpass ) << "'";
				}
				str << "\n      }";
			}
			str << "\n    ";
		}
		str << "}";
		str << "\n    pipelines = {";
		if ( not pipelines.empty() )
		{
			Array<Pair< PipelineName, PipelineSpecUID >>	temp {pipelines};
			std::sort( temp.begin(), temp.end(), [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; });

			uint	idx = 0;
			for (auto& ppln : temp) {
				str << "\n      [" << Base::ToString(idx++) << "]  '" << nameMap( ppln.first ) << "', " << Base::ToString<16>( usize(ppln.second) );
			}
			str << "\n    ";
		}
		str << "}";

		str << "\n    shader binding tables = {";
		if ( not rtSBTs.empty() )
		{
			Array<Pair< RTShaderBindingName, RTShaderBindingUID >>	temp {rtSBTs};
			std::sort( temp.begin(), temp.end(), [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; });

			uint	idx = 0;
			for (auto& sbt : temp) {
				str << "\n      [" << Base::ToString(idx++) << "]  '" << nameMap( sbt.first ) << "', " << Base::ToString<16>( usize(sbt.second) );
			}
			str << "\n    ";
		}
		str << "}";

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (SerializableRTShaderBindingTable)
=================================================
*/
	String  SerializableRTShaderBindingTable::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    ppln name:        " << nameMap( pplnName );
		str << "\n    ray gen:          " << Base::ToString( raygen.index );
		str << "\n    ray type count:   " << Base::ToString( numRayTypes );

		if ( miss.size() )
		{
			str << "\n    miss shaders = {\n        ";
			for (auto& sh : miss) {
				str << (&sh == miss.data() ? "" : ", ") << Base::ToString( sh.index );
			}
			str << "\n    }";
		}

		if ( hit.size() )
		{
			str << "\n    hit groups = {\n        ";
			for (auto& sh : hit) {
				str << (&sh == hit.data() ? "" : ", ") << Base::ToString( sh.index );
			}
			str << "\n    }";
		}

		if ( callable.size() )
		{
			str << "\n    callable shaders = {\n        ";
			for (auto& sh : callable) {
				str << (&sh == callable.data() ? "" : ", ") << Base::ToString( sh.index );
			}
			str << "\n    }";
		}

		str << "\n  ----------------------";
		return str;
	}

/*
=================================================
	ToString (ShaderBytecode)
=================================================
*/
	String  ShaderBytecode::ToString (const HashToName &) const
	{
		String	str;
		str << "  offset = " << Base::ToString( offset );
		return str;
	}

	String  ShaderBytecode::ToString2 (const HashToName &nameMap) const
	{
		String	str;

		Visit( code,
			[&str] (const NullUnion &) {
				str << "\n    unknown type";
			},
			[&str] (const SpirvBytecode_t &spirv) {
				str << "\n    codeSize = " << Base::ToString( ArraySizeOf( spirv ))
					<< "\n    codeHash = " << Base::ToString<16>( usize(HashOf(spirv)) );
			},
			[&str] (const MetalBytecode_t &mtbc) {
				str << "\n    codeSize = " << Base::ToString( ArraySizeOf( mtbc ));
					//<< "\n    codeHash = " << Base::ToString<16>( usize(HashOf(mtbc)) );
			},
			[&str] (const SpirvWithTrace &dbgSpirv) {
				str << "\n    codeSize = " << Base::ToString( ArraySizeOf( dbgSpirv.bytecode ));
					//<< "\n    codeHash = " << Base::ToString<16>( usize(HashOf( dbgSpirv.bytecode ))); // not stable
				if ( dbgSpirv.trace )
					str << "\n    withTrace";
			}
		);

		if ( not spec.empty() )
		{
			str << "\n    spec     = {";
			usize	idx = 0;
			for (auto s : spec)
			{
				str << "\n      [" << Base::ToString(idx++) << "] '"
					<< nameMap( s.first ) << "', "
					<< Base::ToString(s.second);
			}
			str << "\n    }";
		}
		return str;
	}

/*
=================================================
	SpecValues_ToString
=================================================
*/
	static String  SpecValues_ToString (const HashToName &nameMap, const GraphicsPipelineDesc::SpecValues_t &spec)
	{
		String	str;
		str << "\n    specialization = {";

		uint	idx = 0;
		for (auto [name, val] : spec)
		{
			str << "\n      [" << ToString(idx++) << "] '"
				<< nameMap( name ) << "' : "
				<< ToString( val ) << " (0x" << ToString<16>( val ) << ")";
		}

		str << "\n    }";
		return str;
	}

/*
=================================================
	ToString (SerializableGraphicsPipelineSpec)
=================================================
*/
	String  SerializableGraphicsPipelineSpec::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    templUID      = " << Base::ToString<16>( usize(templUID) )
			<< "\n    rStateUID     = " << Base::ToString<16>( usize(rStateUID) )
			<< "\n    dsStateUID    = " << Base::ToString<16>( usize(dsStateUID) )
			<< "\n    renderPass    = '" << nameMap( desc.renderPass ) << "'"
			<< "\n    subpass       = '" << nameMap( desc.subpass ) << "'"
			<< "\n    viewportCount = " << Base::ToString( desc.viewportCount )
			<< "\n    dynamicState  = " << Base::ToString( desc.dynamicState )
			<< "\n    options       = " << Base::ToString( desc.options );

		CHECK( desc.vertexBuffers.empty() == desc.vertexInput.empty() );
		if ( not desc.vertexBuffers.empty() and not desc.vertexInput.empty() )
		{
			usize	num_vi = 0;

			str << "\n    vertexBuffers = {";
			for (usize i = 0; i < desc.vertexBuffers.size(); ++i)
			{
				const auto&	vb = desc.vertexBuffers[i];
				str << "\n      [" << Base::ToString(i) << "] {"
					<< "\n        name     = '" << nameMap( vb.name ) << "'"
					<< "\n        typeName = '" << nameMap( vb.typeName ) << "'"
					<< "\n        rate     = " << Base::ToString( vb.rate )
					<< "\n        index    = " << Base::ToString(uint( vb.index ))
					<< "\n        stride   = " << Base::ToString( vb.stride )
					<< "\n        divisor  = " << Base::ToString( vb.divisor )
					<< "\n        attribs  = {";

				for ( auto&	vi : desc.vertexInput)
				{
					if ( vi.bufferBinding == i )
					{
						++num_vi;
						str << "\n          type   = " << Base::ToString( vi.type )
							<< "\n          offset = " << Base::ToString( vi.offset )
							<< "\n          index  = " << Base::ToString(uint( vi.index ))
							<< "\n          -------------";
					}
				}
				str << "\n      }";
			}
			CHECK( num_vi == desc.vertexInput.size() );
			str << "\n    }";
		}

		if ( not desc.specialization.empty() )
			str << SpecValues_ToString( nameMap, desc.specialization );

		return str;
	}

/*
=================================================
	ToString (SerializableMeshPipelineSpec)
=================================================
*/
	String  SerializableMeshPipelineSpec::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    templUID      = " << Base::ToString<16>( usize(templUID) )
			<< "\n    rStateUID     = " << Base::ToString<16>( usize(rStateUID) )
			<< "\n    dsStateUID    = " << Base::ToString<16>( usize(dsStateUID) )
			<< "\n    renderPass    = '" << nameMap( desc.renderPass ) << "'"
			<< "\n    subpass       = '" << nameMap( desc.subpass ) << "'"
			<< "\n    viewportCount = " << Base::ToString( desc.viewportCount )
			<< "\n    dynamicState  = " << Base::ToString( desc.dynamicState )
			<< "\n    options       = " << Base::ToString( desc.options );

		if ( Any( desc.taskLocalSize != UMax ))
		{
			str << "\n    taskLocalSize = { "
				<< (desc.taskLocalSize.x != UMax ? Base::ToString( desc.taskLocalSize.x ) : "Default"s) << ", "
				<< (desc.taskLocalSize.y != UMax ? Base::ToString( desc.taskLocalSize.y ) : "Default"s) << ", "
				<< (desc.taskLocalSize.z != UMax ? Base::ToString( desc.taskLocalSize.z ) : "Default"s) << " }";
		}
		if ( Any( desc.meshLocalSize != UMax ))
		{
			str << "\n    meshLocalSize = { "
				<< (desc.meshLocalSize.x != UMax ? Base::ToString( desc.meshLocalSize.x ) : "Default"s) << ", "
				<< (desc.meshLocalSize.y != UMax ? Base::ToString( desc.meshLocalSize.y ) : "Default"s) << ", "
				<< (desc.meshLocalSize.z != UMax ? Base::ToString( desc.meshLocalSize.z ) : "Default"s) << " }";
		}

		if ( not desc.specialization.empty() )
			str << SpecValues_ToString( nameMap, desc.specialization );

		return str;
	}

/*
=================================================
	ToString (SerializableComputePipelineSpec)
=================================================
*/
	String  SerializableComputePipelineSpec::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    templUID     = " << Base::ToString<16>( usize(templUID) )
			<< "\n    dynamicState = " << Base::ToString( desc.dynamicState )
			<< "\n    options      = " << Base::ToString( desc.options );

		if ( Any( desc.localSize != UMax ))
		{
			str << "\n    localSize    = { "
				<< (desc.localSize.x != UMax ? Base::ToString( desc.localSize.x ) : "Default"s) << ", "
				<< (desc.localSize.y != UMax ? Base::ToString( desc.localSize.y ) : "Default"s) << ", "
				<< (desc.localSize.z != UMax ? Base::ToString( desc.localSize.z ) : "Default"s) << " }";
		}

		if ( not desc.specialization.empty() )
			str << SpecValues_ToString( nameMap, desc.specialization );

		return str;
	}

/*
=================================================
	ToString (SerializableTilePipelineSpec)
=================================================
*/
	String  SerializableTilePipelineSpec::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    templUID     = " << Base::ToString<16>( usize(templUID) )
			<< "\n    dynamicState = " << Base::ToString( desc.dynamicState )
			<< "\n    renderPass   = '" << nameMap( desc.renderPass ) << "'"
			<< "\n    subpass      = '" << nameMap( desc.subpass ) << "'"
			<< "\n    options      = " << Base::ToString( desc.options );

		if ( Any( desc.localSize != UMax ))
		{
			str << "\n    localSize    = { "
				<< (desc.localSize.x != UMax ? Base::ToString( desc.localSize.x ) : "Default"s) << ", "
				<< (desc.localSize.y != UMax ? Base::ToString( desc.localSize.y ) : "Default"s) << " }";
		}

		if ( not desc.specialization.empty() )
			str << SpecValues_ToString( nameMap, desc.specialization );

		return str;
	}

/*
=================================================
	ToString (SerializableRayTracingPipelineSpec)
=================================================
*/
	String  SerializableRayTracingPipelineSpec::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\n    templUID       = " << Base::ToString<16>( usize(templUID) )
			<< "\n    dynamicState   = " << Base::ToString( desc.dynamicState )
			<< "\n    options        = " << Base::ToString( desc.options )
			<< "\n    maxRecursionDepth              = " << Base::ToString( desc.maxRecursionDepth )
			<< "\n    maxPipelineRayPayloadSize      = " << Base::ToString( desc.maxPipelineRayPayloadSize )
			<< "\n    maxPipelineRayHitAttributeSize = " << Base::ToString( desc.maxPipelineRayHitAttributeSize );

		if ( not desc.specialization.empty() )
			str << SpecValues_ToString( nameMap, desc.specialization );

		return str;
	}

# endif // AE_TEST_PIPELINE_COMPILER
//-----------------------------------------------------------------------------

} // AE::PipelineCompiler
