// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'


	//
	// Present Pass
	//
	class ScriptExe::ScriptPresent final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		rt;
		ImageLayer			layer;
		MipmapLevel			mipmap;
		RC<DynamicDim>		dynSize;

	public:
		ScriptPresent (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, RC<DynamicDim> dynSize) :
			rt{rt}, layer{layer}, mipmap{mipmap}, dynSize{dynSize} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV;
	};

/*
=================================================
	ScriptPresent::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptPresent::ToPass () __Th___
	{
		Array< RC<Image> >	src;

		ImageViewDesc	desc;
		desc.baseLayer	= layer;
		desc.baseMipmap	= mipmap;

		RC<Image>	img = rt->ToResource();
		CHECK_THROW( img );

		img = img->CreateView( desc, "PresentSrc" );
		CHECK_THROW( img );

		src.push_back( img );

		return MakeRCTh<ResEditor::Present>( RVRef(src), "Present", dynSize );
	}
//-----------------------------------------------------------------------------



	//
	// Dbg View Pass
	//
	class ScriptExe::ScriptDbgView final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		rt;
		ImageLayer			layer;
		MipmapLevel			mipmap;
		DebugView::EFlags	flags;
		uint				index;

	public:
		ScriptDbgView (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags, uint idx) :
			rt{rt}, layer{layer}, mipmap{mipmap}, flags{flags}, index{idx} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::DebugView>( rt->ToResource(), index, flags, layer, mipmap,
												   s_scriptExe->_GetRenderer(), "DbgView" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Generate Mipmaps Pass
	//
	class ScriptExe::ScriptGenMipmaps final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		rt;

	public:
		ScriptGenMipmaps (const ScriptImagePtr &rt) : rt{rt} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::GenerateMipmapsPass>( rt->ToResource(), "GenMipmaps" );
		}
	};
//-----------------------------------------------------------------------------



	Nd__In String  EValueType_ToStr (PipelineCompiler::EValueType type, uint rows)
	{
		ASSERT( rows >= 1 );
		ASSERT( rows <= 4 );
		rows = Clamp( rows, 1u, 4u ) - 1;
		const char  rows_str[] = { '\0', '2', '3', '4' };
		switch ( type ) {
			case PipelineCompiler::EValueType::Int32 :		return "int"s + rows_str[rows];
			case PipelineCompiler::EValueType::UInt32 :		return "uint"s + rows_str[rows];
			case PipelineCompiler::EValueType::Float32 :	return "float"s + rows_str[rows];
		}
		return "<unknown>";
	}


	//
	// Read Buffer Value
	//
	class ScriptExe::ScriptReadBufferValue final : public ScriptBasePass
	{
	private:
		ScriptBufferPtr						buffer;
		const String						fieldName;
		AnyDynVecOrScalar_t					dst;
		const PipelineCompiler::EValueType	type;
		const ubyte							rows;

	public:
		ScriptReadBufferValue (ScriptBufferPtr buf, const String &field, AnyDynVecOrScalar_t dst, PipelineCompiler::EValueType type, uint count) :
			buffer{buf}, fieldName{field}, dst{RVRef(dst)}, type{type}, rows{ubyte(count)} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			RC<Buffer>	buf;
			Bytes		offset;
			Bytes		size;

			s_scriptExe->_RunWithPipelineCompiler(
				[&] () {
					buf = buffer->ToResource();
					CHECK_THROW( buf );

					buffer->AddLayoutReflection();

					Array<StringView>	tokens;
					Parser::Tokenize( fieldName, '.', OUT tokens );

					CHECK_THROW( tokens.size() >= 1 );  // internal error

					auto*	field = buffer->GetField( String{tokens[0]} ).GetIf< PipelineCompiler::ShaderStructType::Field >();
					CHECK_THROW_MSG( field != null,
						"Field '"s << tokens[0] << "' is not exist in buffer '" << buffer->GetName() << "'." );

					for (usize i = 1; i < tokens.size(); ++i)
					{
						CHECK_THROW_MSG( field->stType,
							"Field '"s << tokens[i-1] << "' is not a structure. Error when searching fields '" << fieldName <<
							"' in buffer '" << buffer->GetName() << "'." );

						offset += field->offset;

						PipelineCompiler::ShaderStructType::Field const*	new_field = null;

						for (auto& f : field->stType->Fields())
						{
							if ( f.name == tokens[i] )
							{
								new_field = &f;
								break;
							}
						}

						CHECK_THROW_MSG( new_field != null,
							"Field '"s << tokens[i] << "' is not found in structure '" << field->stType->Name() <<
							"'. Error when searching fields '" << fieldName << "' in buffer '" << buffer->GetName() << "'." );

						field = new_field;
					}

					CHECK_THROW_MSG( field->type == type and field->rows == rows,
						"Field '"s << fieldName << "' in buffer '" << buffer->GetName() << "' has '" << EValueType_ToStr( field->type, field->rows ) <<
						"'type, but destination type is '" << EValueType_ToStr( type, rows ) << "'" );

					CHECK_THROW_MSG( field->cols == 1 );  // matrix type is not supported

					offset	+= field->offset;
					size	= field->size;
				});

			return MakeRCTh<ResEditor::ReadBufferValuePass>( buf, offset, size, dst );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Compress Image Pass
	//
	class ScriptExe::ScriptCompressImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		src;
		ScriptImagePtr		dst;
		EPixelFormat		dstFormat;

	public:
		ScriptCompressImage (const ScriptImagePtr &src, const ScriptImagePtr &dst, EPixelFormat dstFormat) :
			src{src}, dst{dst}, dstFormat{dstFormat} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::ImageCompressionPass>( src->ToResource(), dst->ToResource(), dstFormat, "CompressImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Copy Image Pass
	//
	class ScriptExe::ScriptCopyImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		src;
		ScriptImagePtr		dst;

	public:
		ScriptCopyImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) :
			src{src}, dst{dst} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::CopyImagePass>( src->ToResource(), dst->ToResource(), "CopyImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Copy Image Pass v2
	//
	class ScriptExe::ScriptCopyImage2 final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		src;
		ScriptImagePtr		dst;

	public:
		ScriptCopyImage2 (const ScriptImagePtr &src, const ScriptImagePtr &dst) :
			src{src}, dst{dst} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &)	C_Th_OV {}
		RC<IPass>	ToPass ()										__Th_OV;

	private:
		auto		_CompilePipeline ()								C_Th___;
		void		_CompilePipeline2 ()							C_Th___;
	};

/*
=================================================
	_CompilePipeline
=================================================
*/
	auto  ScriptExe::ScriptCopyImage2::_CompilePipeline () C_Th___
	{
		return ScriptExe::ScriptPassApi::ConvertAndLoad(
					[this] (ScriptEnginePtr) {
						_CompilePipeline2();	// throw
					},
					_baseFlags );
	}

	void  ScriptExe::ScriptCopyImage2::_CompilePipeline2 () C_Th___
	{
		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}{
			DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
			ds_layout->AddCombinedImage_ImmutableSampler( EShaderStages::Compute, "un_InImage", EImageType(src->ImageType()), EResourceState::ShaderSample, "NearestClamp"s );
			ds_layout->AddStorageImage( EShaderStages::Compute, "un_OutImage", ArraySize{1}, EImageType(dst->ImageType()), dst->PixelFormat(), EAccessType::Restrict, EResourceState::ShaderStorage_Write );
		}

		String	cs;
		{
			cs << R"#(
				#include "InvocationID.glsl"

				void  Main ()
				{
					float4	c = gl.texture.Fetch( un_InImage, GetGlobalCoord().xy, 0 );
					gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
				}
			)#";
		}

		PipelineLayoutPtr		ppln_layout{ new PipelineLayout{ "copyimage.pl" }};
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );

		ComputePipelinePtr		ppln_templ{ new ComputePipelineScriptBinding{ "copyimage" }};
		ppln_templ->Disable();
		ppln_templ->SetLayout2( ppln_layout );

		{
			const uint	local_size = RelaxedCopyImagePass::localSize;

			ScriptShaderPtr	sh{ new ScriptShader{}};
			sh->SetSource( EShader::Compute, cs );
			sh->options = EShaderOpt::Optimize;
			sh->SetComputeLocalSize2( local_size, local_size );

			ppln_templ->SetShader( sh );
		}
		{
			ComputePipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( "copyimage" );
			ppln_spec->Disable();
			ppln_spec->AddToRenderTech( "rtech", "Compute" );
			ppln_spec->SetOptions( EPipelineOpt::Optimize );

			// if successfully compiled
			ppln_spec->Enable();
		}
	}

/*
=================================================
	ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptCopyImage2::ToPass () __Th___
	{
		auto	pack	= _CompilePipeline();  // throw
		auto	ppln	= pack.rtech->GetComputePipeline( PipelineName{"copyimage"} );
		CHECK_THROW_MSG( ppln );

		return MakeRCTh<ResEditor::RelaxedCopyImagePass>(
							src->ToResource(), dst->ToResource(),
							RVRef(pack), ppln,
							"CopyImageRelaxed" );
	}
//-----------------------------------------------------------------------------



	//
	// Blit Image Pass
	//
	class ScriptExe::ScriptBlitImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		src;
		ScriptImagePtr		dst;

	public:
		ScriptBlitImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) :
			src{src}, dst{dst} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::BlitImagePass>( src->ToResource(), dst->ToResource(), "BlitImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Resolve Image Pass
	//
	class ScriptExe::ScriptResolveImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		src;
		ScriptImagePtr		dst;

	public:
		ScriptResolveImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) :
			src{src}, dst{dst} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::ResolveImagePass>( src->ToResource(), dst->ToResource(), "ResolveImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Clear Image Pass
	//
	class ScriptExe::ScriptClearImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr					image;
		ClearImagePass::ClearValue_t	value;

	public:
		ScriptClearImage (const ScriptImagePtr &image, ClearImagePass::ClearValue_t value) :
			image{image}, value{value} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::ClearImagePass>( image->ToResource(), value, "ClearImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Clear Buffer Pass
	//
	class ScriptExe::ScriptClearBuffer final : public ScriptBasePass
	{
	private:
		ScriptBufferPtr		buffer;
		Bytes				offset;
		Bytes				size	= UMax;
		uint				value;

	public:
		ScriptClearBuffer (const ScriptBufferPtr &buffer, uint value) :
			buffer{buffer}, value{value} {}

		ScriptClearBuffer (const ScriptBufferPtr &buffer, Bytes offset, Bytes size, uint value) :
			buffer{buffer}, offset{offset}, size{size}, value{value} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			RC<Buffer>	buf;

			s_scriptExe->_RunWithPipelineCompiler(
				[&] () {
					buf = buffer->ToResource();
					CHECK_THROW( buf );
				});

			return MakeRCTh<ResEditor::ClearBufferPass>( buf, offset, size, value, "ClearBuffer" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Convert Cooperative Vector Matrix Pass
	//
	class ScriptExe::ScriptConvertCooperativeVectorMatrix final : public ScriptBasePass
	{
	private:
		uint						_numRows;
		uint						_numColumns;

		ECoopMatrixComponentType	_srcType;
		ScriptBufferPtr				_srcBuffer;
		uint						_srcOffset;
		uint						_srcSize;
		uint						_srcStride;
		ECoopVecMatrixLayout		_srcLayout;

		ECoopMatrixComponentType	_dstType;
		ScriptBufferPtr				_dstBuffer;
		uint						_dstOffset;
		uint						_dstSize;
		uint						_dstStride;
		ECoopVecMatrixLayout		_dstLayout;

	public:
		ScriptConvertCooperativeVectorMatrix (
			uint numRows, uint numColumns,
			ECoopMatrixComponentType srcType, const ScriptBufferPtr &srcBuffer,
			uint srcOffset, uint srcSize,  uint srcStride, ECoopVecMatrixLayout srcLayout,
			ECoopMatrixComponentType dstType, const ScriptBufferPtr &dstBuffer,
			uint dstOffset, uint dstSize,  uint dstStride, ECoopVecMatrixLayout dstLayout) __Th___ :
			_numRows{numRows}, _numColumns{numColumns},
			_srcType{srcType}, _srcBuffer{srcBuffer}, _srcOffset{srcOffset}, _srcSize{srcSize}, _srcStride{srcStride}, _srcLayout{srcLayout},
			_dstType{dstType}, _dstBuffer{dstBuffer}, _dstOffset{dstOffset}, _dstSize{dstSize}, _dstStride{dstStride}, _dstLayout{dstLayout}
		{}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			RC<Buffer>	src_buf;
			RC<Buffer>	dst_buf;

			s_scriptExe->_RunWithPipelineCompiler(
				[&] () {
					src_buf = _srcBuffer->ToResource();
					CHECK_THROW( src_buf );

					dst_buf = _dstBuffer->ToResource();
					CHECK_THROW( dst_buf );
				});

			return MakeRCTh<ResEditor::ConvertCooperativeVectorMatrixPass>(
						_numRows, _numColumns,
						_srcType, src_buf, Bytes{_srcOffset}, Bytes{_srcSize}, Bytes{_srcStride}, _srcLayout,
						_dstType, dst_buf, Bytes{_dstOffset}, Bytes{_dstSize}, Bytes{_dstStride}, _dstLayout
					);
		}
	};
//-----------------------------------------------------------------------------



	//
	// Reset Unused Timers
	//
	class ScriptExe::ScriptResetUnusedTimers final : public ScriptBasePass
	{
	private:
		Array< RC<DynamicFloat> >		_arr;

	public:
		ScriptResetUnusedTimers (const ScriptArray< ScriptDynamicFloatPtr > &arr) __Th___
		{
			_arr.reserve( arr.size() );

			for (auto& dyn : arr)
			{
				CHECK_THROW( dyn and dyn->Get() );
				_arr.push_back( dyn->Get() );
			}
		}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::ResetUnusedTimersPass>( _arr );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Export Image
	//
	class ScriptExe::ScriptExportImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		image;
		String				prefix;

	public:
		ScriptExportImage (const ScriptImagePtr &image, const String &prefix) :
			image{image}, prefix{prefix} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV
		{
			return MakeRCTh<ResEditor::ExportImage>( image->ToResource(), RVRef(prefix) );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Export Buffer
	//
	class ScriptExe::ScriptExportBuffer final : public ScriptBasePass
	{
	public:
		enum class EMode {
			Structured,
			Binary,
		};
	private:
		struct _Utils;

	private:
		ScriptBufferPtr		buffer;
		String				prefix;
		Bytes				offset;
		Bytes				size;
		EMode				mode;

	public:
		ScriptExportBuffer (const ScriptBufferPtr &buffer, const String &prefix, EMode mode) :
			buffer{buffer}, prefix{prefix}, offset{0_b}, size{UMax}, mode{mode} {}

		ScriptExportBuffer (const ScriptBufferPtr &buffer, const String &prefix, Bytes offset, Bytes size, EMode mode) :
			buffer{buffer}, prefix{prefix}, offset{offset}, size{size}, mode{mode} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV;
	};

/*
=================================================
	ScriptExportBuffer::_Utils
=================================================
*/
	struct ScriptExe::ScriptExportBuffer::_Utils
	{
	private:
		ShaderStructType &		_bufferType;
		BufferMemView const&	_memView;

		String					str;

	public:
		explicit _Utils (ShaderStructType &stType, const BufferMemView &memView) :
			_bufferType{stType}, _memView{memView}
		{}

		void  Parse (WStream &stream);

	private:
		void  _AddTypeName (const ShaderStructType::Field &field);

		ND_ bool  _Parse (const ShaderStructType &stType, Bytes baseOffset, uint depth);

		template <typename T>
		ND_ bool  _ParseVal (const ShaderStructType::Field &field, Bytes baseOffset, ulong arraySize, uint depth);
	};


/*
=================================================
	ScriptExportBuffer::_Utils::Parse
=================================================
*/
	void  ScriptExe::ScriptExportBuffer::_Utils::Parse (WStream &stream)
	{
		str.reserve( 1024 );

		str << "buffer ";
		switch_enum( _bufferType.Layout() )
		{
			case EStructLayout::Compatible_Std140 :
			case EStructLayout::Std140 :			str << "(std140) ";	break;
			case EStructLayout::Compatible_Std430 :
			case EStructLayout::Std430 :			str << "(std430) ";	break;
			case EStructLayout::Metal :				str << "(metal) ";	break;
			case EStructLayout::HLSL_Const :		str << "(cbuffer)";	break;
			case EStructLayout::HLSL_Struct :		str << "(hlsl)";	break;
			case EStructLayout::InternalIO :		break;
			case EStructLayout::_Count :
			case EStructLayout::Unknown :			break;
		}
		switch_end
		str << _bufferType.Typename() << " {\n";

		CHECK( _Parse( _bufferType, 0_b, 1 ));

		str << "} // " << _bufferType.Typename() << "\n";

		CHECK( stream.Write( str ));
	}

/*
=================================================
	ScriptExportBuffer::_Utils::_AddTypeName
=================================================
*/
	void  ScriptExe::ScriptExportBuffer::_Utils::_AddTypeName (const ShaderStructType::Field &field)
	{
		if ( field.IsStruct() )
		{
			str << field.stType->Typename();
		}
		else
		{
			switch_enum( field.type )
			{
				case EValueType::Bool8 :		str << "bool";			break;
				case EValueType::Bool32 :		str << "bool";			break;
				case EValueType::Int8 :			str << "byte";			break;
				case EValueType::Int16 :		str << "short";			break;
				case EValueType::Int32 :		str << "int";			break;
				case EValueType::Int64 :		str << "long";			break;
				case EValueType::UInt8 :		str << "ubyte";			break;
				case EValueType::UInt16 :		str << "ushort";		break;
				case EValueType::UInt32 :		str << "uint";			break;
				case EValueType::DeviceAddress:	str << "DeviceAddress";	break;
				case EValueType::UInt64 :		str << "ulong";			break;
				case EValueType::Float16 :		str << "half";			break;
				case EValueType::Float32 :		str << "float";			break;
				case EValueType::Float64 :		str << "double";		break;
				case EValueType::Int8_Norm :
				case EValueType::Int16_Norm :
				case EValueType::UInt8_Norm :
				case EValueType::UInt16_Norm :
				case EValueType::Unknown :
				case EValueType::_Count :		str << "<unknown>";		break;
			}
			switch_end

			if ( field.IsVec() )	str << ToString( field.rows );
			if ( field.IsMat() )	str << ToString( field.cols ) << 'x' << ToString( field.rows );
		}
	}

/*
=================================================
	ScriptExportBuffer::_Utils::_Parse
=================================================
*/
	bool  ScriptExe::ScriptExportBuffer::_Utils::_Parse (const ShaderStructType &stType, const Bytes baseOffset, const uint depth)
	{
		for (auto& field : stType.Fields())
		{
			if ( field.IsAnyPadding() )
				continue;

			ulong	array_size	= field.IsStaticArray() ? field.arraySize : 0;

			if ( field.IsDynamicArray() )
			{
				Bytes	remain_size = _memView.DataSize() - baseOffset - field.offset;
				CHECK_ERR( IsMultipleOf( remain_size, field.size ));

				array_size	= ulong{remain_size / field.size};
			}

			AppendToString( INOUT str, depth*2, ' ' );
			_AddTypeName( field );
			str << "  " << field.name;

			if ( array_size > 0 )	str << " [" << ToString( array_size ) << "] = {\n";
			else					str << " = ";

			if ( field.IsStruct() )
			{
				const uint	depth2 = depth+1 + uint(array_size > 0);

				for (ulong i = 0, cnt = Max( array_size, 1u ); i < cnt; ++i)
				{
					if ( array_size > 0 ) {
						AppendToString( INOUT str, (depth+1)*2, ' ' );
						str << field.stType->Typename() << " { // [" << ToString(i) << "]\n";
					}else{
						str << field.stType->Typename() << " {\n";
					}

					Bytes	off = baseOffset + field.offset + AlignUp( field.size, field.align ) * i;
					ASSERT( off + field.size <= _memView.DataSize() );

					CHECK_ERR( _Parse( *field.stType, off, depth2 ));

					AppendToString( INOUT str, (depth+1)*2, ' ' );

					if ( array_size > 0 ) {
						str << "}" << (i+1 != cnt ? "," : " ") << " // " << field.stType->Typename() << "\n";
					}else{
						str << "} // " << field.stType->Typename();
					}
				}
			}
			else
			{
				switch_enum( field.type )
				{
					case EValueType::Bool8 :		CHECK_ERR( _ParseVal< bool   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Bool32 :		CHECK_ERR( _ParseVal< lbool  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int8 :			CHECK_ERR( _ParseVal< sbyte  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int16 :		CHECK_ERR( _ParseVal< sshort >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int32 :		CHECK_ERR( _ParseVal< sint   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int64 :		CHECK_ERR( _ParseVal< slong  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt8 :		CHECK_ERR( _ParseVal< ubyte  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt16 :		CHECK_ERR( _ParseVal< ushort >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt32 :		CHECK_ERR( _ParseVal< uint   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::DeviceAddress:	CHECK_ERR( _ParseVal<DeviceAddress>( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt64 :		CHECK_ERR( _ParseVal< ulong  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Float16 :		CHECK_ERR( _ParseVal< half   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Float32 :		CHECK_ERR( _ParseVal< float  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Float64 :		CHECK_ERR( _ParseVal< double >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int8_Norm :
					case EValueType::Int16_Norm :
					case EValueType::UInt8_Norm :
					case EValueType::UInt16_Norm :
					case EValueType::Unknown :
					case EValueType::_Count :
					default :						RETURN_ERR( "unsupported value type" );
				}
				switch_end
			}

			if ( array_size > 0 )	{ AppendToString( INOUT str, depth*2, ' ' );  str << "} // " << field.name << "[]\n"; }
			else					str << "\n";
		}

		return true;
	}

/*
=================================================
	ScriptExportBuffer::_Utils::_ParseVal
=================================================
*/
	template <typename T>
	bool  ScriptExe::ScriptExportBuffer::_Utils::_ParseVal (const ShaderStructType::Field &field, Bytes baseOffset,
															const ulong arraySize, const uint depth)
	{
		const bool	is_array	= arraySize > 0;
		const Bytes	elem_size	= field.size / Max( arraySize, 1u );

		baseOffset += field.offset;
		ASSERT( baseOffset + field.size <= _memView.DataSize() );

		ASSERT( SizeOf<T> * field.rows * field.cols == elem_size or
				SizeOf<T> * 4 * field.cols == elem_size );

		for (ulong i = 0, cnt = Max( arraySize, 1u ); i < cnt; ++i)
		{
			Bytes	off  = baseOffset + elem_size * i;
			auto	data = _memView.GetRange( off, elem_size );

			CHECK_ERR( data.size == elem_size );

			if ( is_array )  AppendToString( INOUT str, depth*2, ' ' );

			if ( field.rows > 1 ) str << "{ ";

			for (uint r = 0, rows = field.rows; r < rows; ++r)
			{
				if constexpr( IsSame< T, DeviceAddress >)
					str << "0x" << ToString<16>( Cast<T>(data.ptr)[r] );
				else
				if constexpr( IsSame< T, half >)
					str << ToString( float{Cast<T>(data.ptr)[r]}, 5, True{"exp"} );
				else
				if constexpr( IsFloatPoint<T> )
					str << ToString( Cast<T>(data.ptr)[r], 5, True{"exp"} );
				else
					str << ToString( Cast<T>(data.ptr)[r] );

				if ( r+1 != rows )  str << ", ";
			}

			if ( field.rows > 1 ) str << " }";

			if ( is_array ) str << (i+1 != arraySize ? "," : " ") << "  // [" << ToString(i) << "]\n";
		}
		return true;
	}

/*
=================================================
	ScriptExportBuffer::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptExportBuffer::ToPass () __Th___
	{
		ShaderStructTypePtr	st_type;
		RC<Buffer>			buf;

		s_scriptExe->_RunWithPipelineCompiler(
			[&] () {
				auto	storage = ObjectStorage::Instance();
				CHECK_THROW( storage );

				buffer->AddLayoutReflection();
				buf = buffer->ToResource();
				CHECK_THROW( buf );

				if ( mode == EMode::Structured )
				{
					CHECK_THROW( buffer->HasLayout() );

					auto&	st_types	= storage->structTypes;
					auto	it			= st_types.find( buffer->GetTypeName() );

					CHECK_THROW( it != st_types.end() );
					st_type = it->second;
				}
			});

		switch_enum( mode )
		{
			case EMode::Structured :
				return MakeRCTh<ResEditor::ExportBuffer>( buf, RVRef(prefix),
								[st_type] (const BufferMemView &memView, WStream &stream) {
									_Utils{ *st_type, memView }.Parse( stream );
								});

			case EMode::Binary :
				return MakeRCTh<ResEditor::ExportBuffer>( buf, RVRef(prefix), offset, size );

			default :
				CHECK_THROW_MSG( false, "unsupported buffer export mode" );
		}
		switch_end
	}
//-----------------------------------------------------------------------------



	//
	// Build Ray Tracing Geometry Pass
	//
	class ScriptExe::ScriptBuildRTGeometry final : public ScriptBasePass
	{
	private:
		ScriptRTGeometryPtr	_dstGeometry;
		const bool			_indirect;


	public:
		ScriptBuildRTGeometry (ScriptRTGeometryPtr	dstGeometry,
							   bool					indirect) :
			_dstGeometry{ dstGeometry },
			_indirect{ indirect }
		{}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV;
	};

/*
=================================================
	ScriptBuildRTGeometry::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptBuildRTGeometry::ToPass () __Th___
	{
		CHECK_THROW( s_scriptExe != null );

		RC<RTGeometry>	dst_geom;
		s_scriptExe->_RunWithPipelineCompiler(
			[&] () {
				dst_geom = _dstGeometry->ToResource();
			});

		return MakeRCTh<ResEditor::BuildRTGeometry>(
					dst_geom, _indirect,
					"BuildRTGeometry" );
	}
//-----------------------------------------------------------------------------



	//
	// Build Ray Tracing Scene Pass
	//
	class ScriptExe::ScriptBuildRTScene final : public ScriptBasePass
	{
	private:
		ScriptRTScenePtr	_dstScene;
		const bool			_indirect;


	public:
		ScriptBuildRTScene (ScriptRTScenePtr	dstScene,
							bool				indirect) :
			_dstScene{ dstScene },
			_indirect{ indirect }
		{}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () __Th_OV;
	};

/*
=================================================
	ScriptBuildRTScene::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptBuildRTScene::ToPass () __Th___
	{
		CHECK_THROW( s_scriptExe != null );

		RC<RTScene>		dst_scene;
		s_scriptExe->_RunWithPipelineCompiler(
			[&] () {
				dst_scene = _dstScene->ToResource();
			});

		return MakeRCTh<ResEditor::BuildRTScene>(
					dst_scene, _indirect,
					"BuildRTScene" );
	}
//-----------------------------------------------------------------------------



	//
	// Pass Group
	//
	class ScriptExe::ScriptPassGroup final : public ScriptBasePass
	{
	private:
		Array< ScriptBasePassPtr >	_passes;
		const PassGroup::EFlags		_flags;
		RC<Renderer>				_renderer;
		RC<IPass>					_result;

	public:
		ScriptPassGroup (PassGroup::EFlags flags, RC<Renderer> renderer) :
			_flags{flags}, _renderer{RVRef(renderer)}
		{}

			void  Add (ScriptBasePassPtr pass)							{ _passes.push_back( RVRef(pass) ); }
			void  _OnAddArg (INOUT ScriptPassArgs::Argument &)	C_Th_OV	{}

		ND_ ArrayView<ScriptBasePassPtr>	GetPasses ()		const	{ return _passes; }
		ND_ RC<IPass>						ToPass ()			__Th_OV;
	};

/*
=================================================
	ScriptPassGroup::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptPassGroup::ToPass () __Th___
	{
		if ( _result )
			return _result;

		CHECK_THROW( not _passes.empty() );

		RC<PassGroup>	pg = MakeRC<PassGroup>( _flags, _renderer->GetDataTransferQueue() );

		for (auto& script_pass : _passes) {
			pg->AddPass( script_pass->ToPass() );  // throw
		}

		_result = pg;
		return pg;
	}
//-----------------------------------------------------------------------------

