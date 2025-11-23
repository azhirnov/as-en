// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_SLANG

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wcast-qual"
#	pragma clang diagnostic ignored "-Wdouble-promotion"
# endif

# include "slang.h"

// SPIRV-Tools includes
#ifdef AE_ENABLE_SPIRV_TOOLS
#	include "spirv-tools/optimizer.hpp"
#	include "spirv-tools/libspirv.h"
#endif

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif

# include "res_pack/pipeline_compiler/Compiler/SlangCompiler.h"
# include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

# define PRIVATE_SLANG_RETURN_ERR( _text_ )							\
		{if( not _quietWarnings ) {									\
			AE_LOGE( _text_ );										\
		}else{														\
			AE_LOGI( _text_ );										\
		}return Default;											\
		}

# define PRIVATE_SLANG_CHECK_LOG( _fn_, _text_, _log_ )				\
		{if_likely( SLANG_SUCCEEDED( _fn_ )) {}						\
		 else{														\
			(_log_) << _text_;										\
			return Default;											\
		}}

# define SLANG_CHECK_ERR( _fn_ )									\
		{if_likely( SLANG_SUCCEEDED( _fn_ )) {}						\
		 else														\
			PRIVATE_SLANG_RETURN_ERR( AE_TOSTRING( _fn_ ))			\
		}

# define SLANG_CHECK_ERR_MSG( _fn_, _msg_ )							\
		{if_likely( SLANG_SUCCEEDED( _fn_ )) {}						\
		 else														\
			PRIVATE_SLANG_RETURN_ERR( _msg_ )						\
		}

# define SLANG_CHECK_LOG( /* expr, message, log */... )																	\
		PRIVATE_SLANG_CHECK_LOG( AE_PRIVATE_GETARG_0( __VA_ARGS__, ),													\
								 AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__, )), ),\
								 AE_PRIVATE_GETARG_2( __VA_ARGS__, out.log, out.log, ))

//---------------------------------------------------------
# define SLANG_FNS( _visitor_ )\
		_visitor_( spReflection_GetParameterCount )\
		_visitor_( spReflection_GetTypeParameterByIndex )\
		_visitor_( spReflection_getGlobalParamsVarLayout )\
		_visitor_( spReflection_ToJson )\
		_visitor_( spReflection_GetParameterByIndex )\
		_visitor_( spReflection_getEntryPointCount )\
		_visitor_( spReflection_getEntryPointByIndex )\
		_visitor_( spReflectionTypeParameter_GetName )\
		_visitor_( spReflectionVariableLayout_GetTypeLayout )\
		_visitor_( spReflectionVariableLayout_GetVariable )\
		_visitor_( spReflectionVariableLayout_GetOffset )\
		_visitor_( spReflectionVariableLayout_GetSpace )\
		_visitor_( spReflectionVariableLayout_GetImageFormat )\
		_visitor_( spReflectionType_GetName )\
		_visitor_( spReflectionType_GetResourceShape )\
		_visitor_( spReflectionType_GetKind )\
		_visitor_( spReflectionType_GetElementType )\
		_visitor_( spReflectionType_GetScalarType )\
		_visitor_( spReflectionType_GetResourceAccess )\
		_visitor_( spReflectionType_GetResourceResultType )\
		_visitor_( spReflectionTypeLayout_getKind )\
		_visitor_( spReflectionTypeLayout_GetType )\
		_visitor_( spReflectionTypeLayout_GetFieldCount )\
		_visitor_( spReflectionTypeLayout_GetFieldByIndex )\
		_visitor_( spReflectionTypeLayout_GetElementVarLayout )\
		_visitor_( spReflectionTypeLayout_getContainerVarLayout )\
		_visitor_( spReflectionTypeLayout_GetSize )\
		_visitor_( spReflectionTypeLayout_GetElementStride )\
		_visitor_( spReflectionTypeLayout_GetElementTypeLayout )\
		_visitor_( spReflectionTypeLayout_GetCategoryCount )\
		_visitor_( spReflectionTypeLayout_GetCategoryByIndex )\
		_visitor_( spReflectionVariable_GetName )\
		_visitor_( spReflectionEntryPoint_getStage )\
		_visitor_( spReflectionEntryPoint_getComputeThreadGroupSize )\

// TODO: don't use global
namespace SlangFunctions
{
	#define SLANG_FN_VISIT( _name_ )	static decltype(&::_name_)		_var_##_name_;
	SLANG_FNS( SLANG_FN_VISIT )
	#undef SLANG_FN_VISIT

} // SlangFunctions
//---------------------------------------------------------

namespace AE::PipelineCompiler
{
	using namespace slang;

namespace
{
/*
=================================================
	EnumCast (EShader)
=================================================
*/
	ND_ static SlangStage  EnumCast (EShader type) __NE___
	{
		switch_enum( type )
		{
			case EShader::Vertex :				return SLANG_STAGE_VERTEX;
			case EShader::TessControl :			return SLANG_STAGE_HULL;
			case EShader::TessEvaluation :		return SLANG_STAGE_DOMAIN;
			case EShader::Geometry :			return SLANG_STAGE_GEOMETRY;
			case EShader::Fragment :			return SLANG_STAGE_FRAGMENT;
			case EShader::Compute :				return SLANG_STAGE_COMPUTE;
			case EShader::MeshTask :			return SLANG_STAGE_AMPLIFICATION;
			case EShader::Mesh :				return SLANG_STAGE_MESH;
			case EShader::RayGen :				return SLANG_STAGE_RAY_GENERATION;
			case EShader::RayAnyHit :			return SLANG_STAGE_ANY_HIT;
			case EShader::RayClosestHit :		return SLANG_STAGE_CLOSEST_HIT;
			case EShader::RayMiss :				return SLANG_STAGE_MISS;
			case EShader::RayIntersection :		return SLANG_STAGE_INTERSECTION;
			case EShader::RayCallable :			return SLANG_STAGE_CALLABLE;
			case EShader::Tile :
			case EShader::_Count :				break;
		}
		switch_end
		return SLANG_STAGE_NONE;
	}
	
/*
=================================================
	EnumCast (SlangStage)
=================================================
*/
	ND_ static EShader  EnumCast (SlangStage type) __NE___
	{
		switch_enum( type )
		{
			case SLANG_STAGE_VERTEX :			return EShader::Vertex;
			case SLANG_STAGE_HULL :				return EShader::TessControl;
			case SLANG_STAGE_DOMAIN :			return EShader::TessEvaluation;
			case SLANG_STAGE_GEOMETRY :			return EShader::Geometry;
			case SLANG_STAGE_FRAGMENT :			return EShader::Fragment;
			case SLANG_STAGE_COMPUTE :			return EShader::Compute;
			case SLANG_STAGE_AMPLIFICATION :	return EShader::MeshTask;
			case SLANG_STAGE_MESH :				return EShader::Mesh;
			case SLANG_STAGE_RAY_GENERATION :	return EShader::RayGen;
			case SLANG_STAGE_ANY_HIT :			return EShader::RayAnyHit;
			case SLANG_STAGE_CLOSEST_HIT :		return EShader::RayClosestHit;
			case SLANG_STAGE_MISS :				return EShader::RayMiss;
			case SLANG_STAGE_INTERSECTION :		return EShader::RayIntersection;
			case SLANG_STAGE_CALLABLE :			return EShader::RayCallable;
			case SLANG_STAGE_NONE :
			case SLANG_STAGE_DISPATCH :
			case SLANG_STAGE_COUNT :			break;
		}
		switch_end
		return Default;
	}
	
/*
=================================================
	ValidateSPIRV
=================================================
*/
# ifdef AE_ENABLE_SPIRV_TOOLS
	ND_ static bool  ValidateSPIRV (const SpirvBytecode_t &spirv, EShaderVersion shVer, INOUT String &log)
	{
		const uint		ver			= EShaderVersion_Ver2( shVer ).To100();
		spv_target_env	target_env	= SPV_ENV_MAX;
		switch ( ver )
		{
			case 100 :
			case 110 :
			case 120 : target_env = SPV_ENV_VULKAN_1_0;				break;
			case 130 : target_env = SPV_ENV_VULKAN_1_1;				break;
			case 140 : target_env = SPV_ENV_VULKAN_1_1_SPIRV_1_4;	break;
			case 150 : target_env = SPV_ENV_VULKAN_1_2;				break;
			case 160 : target_env = SPV_ENV_VULKAN_1_3;				break;
		}
		CHECK_ERR( target_env != SPV_ENV_MAX );

		spvtools::ValidatorOptions	options;
		spvtools::SpirvTools		tools{ target_env };

		tools.SetMessageConsumer(
			[&log] (spv_message_level_t level, const char *source, const spv_position_t &position, const char *message) {
				switch ( level )
				{
					case SPV_MSG_FATAL:
					case SPV_MSG_INTERNAL_ERROR:
					case SPV_MSG_ERROR:
						log << "error: ";
						break;
					case SPV_MSG_WARNING:
						log << "warning: ";
						break;
					case SPV_MSG_INFO:
					case SPV_MSG_DEBUG:
						log << "info: ";
						break;
				}

				if ( source )
					log << source << ":";

				log << ToString(position.line) << ":" << ToString(position.column) << ":" << ToString(position.index) << ":";
				if ( message )
					log << " " << message;
			});

		return tools.Validate( spirv.data(), spirv.size(), options );
	}
# endif // AE_ENABLE_SPIRV_TOOLS

} // namespace


/*
=================================================
	constructor
=================================================
*/
	SLangCompiler::SLangCompiler (ArrayView<Path> includeDirs) __NE___
	{
		bool	res = _Initialize( includeDirs );
		if ( not res )
			_Deinitialize();
	}

/*
=================================================
	destructor
=================================================
*/
	SLangCompiler::~SLangCompiler () __NE___
	{
		_Deinitialize();
	}
	
/*
=================================================
	_Initialize
=================================================
*/
	bool  SLangCompiler::_Initialize (ArrayView<Path> includeDirs)
	{
		CHECK_ERR( not _lib );
		CHECK_ERR( _globalSession == null );
		CHECK_ERR( _session == null );

		// can not link with static library, so use dynamic
		#ifdef AE_PLATFORM_WINDOWS
		# ifdef AE_CFG_DEBUG
			Unused( _lib.Load( "slangd.dll" ));
		# endif
			if ( not _lib )
				Unused( _lib.Load( "slang.dll" ));
		#endif

		#ifdef AE_PLATFORM_LINUX
		# ifdef AE_CFG_DEBUG
			Unused( _lib.Load( "./libslangd.so" ));
		# endif
			if ( not _lib )
				Unused( _lib.Load( "./libslang.so" ));
		#endif

		#ifdef AE_PLATFORM_APPLE
		# ifdef AE_CFG_DEBUG
			Unused( _lib.Load( "slangd.dylib" ));
		# endif
			if ( not _lib )
				Unused( _lib.Load( "slang.dylib" ));
		#endif

		if ( not _lib )
			return false;

		decltype(&slang_createGlobalSession2)	fnCreateGlobalSession = null;
		CHECK_ERR( _lib.GetProcAddr( "slang_createGlobalSession2", OUT fnCreateGlobalSession ));
		
		decltype(&slang_shutdown)	fnShutdown = null;
		CHECK_ERR( _lib.GetProcAddr( "slang_shutdown", OUT fnShutdown ));

		_shutdown = fnShutdown;

		// load functions
		{
			bool	loaded = true;

			#define SLANG_FN_VISIT( _name_ )	loaded &= _lib.GetProcAddr( AE_TOSTRING(_name_), SlangFunctions::_var_##_name_ );
			SLANG_FNS( SLANG_FN_VISIT )
			#undef SLANG_FN_VISIT

			CHECK_ERR( loaded );
		}

		SlangGlobalSessionDesc	global_desc = {};
		SLANG_CHECK_ERR( fnCreateGlobalSession( &global_desc, OUT &_globalSession ));
		
		_includeDirs.resize( includeDirs.size() );
		for (usize i = 0; i < includeDirs.size(); ++i)
		{
			_includeDirs[i]	= ToString( includeDirs[i] );
		}
		return true;
	}
	
/*
=================================================
	_Deinitialize
=================================================
*/
	void  SLangCompiler::_Deinitialize ()
	{
		_EndSession();

		if ( _globalSession != null )
		{
			_globalSession->release();
			_globalSession = null;
		}

		if ( _shutdown != null )
		{
			_shutdown();
			_shutdown = null;
		}

		_lib.Unload();
	}
	
/*
=================================================
	_BeginSession
=================================================
*/
	bool  SLangCompiler::_BeginSession (const Input &in)
	{
		CHECK_ERR( _globalSession != null );
		CHECK_ERR( in.dstVersion != Default );

		_EndSession();

		Array<CompilerOptionEntry>	options;
		EShaderVersion				dst_type	= in.dstVersion & EShaderVersion::_Mask;
		const bool					is_spirv	= AnyEqual( dst_type, EShaderVersion::_GLSL_SPIRV, EShaderVersion::_Slang_SPIRV );
		const bool					is_metal	= AnyEqual( dst_type, EShaderVersion::_Metal, EShaderVersion::_Metal_iOS, EShaderVersion::_Metal_Mac );
		const bool					is_cxx		= (dst_type == EShaderVersion::_Slang_CXX);
		const bool					is_glsl		= (dst_type == EShaderVersion::_Slang_GLSL);

	#if 1
		const auto	AddIntOption = [&options] (CompilerOptionName name, int value)
		{{
			auto&	dst = options.emplace_back();
			dst.name			= name;
			dst.value.kind		= CompilerOptionValueKind::Int;
			dst.value.intValue0	= value;
		}};

		AddIntOption( CompilerOptionName::Stage, EnumCast( in.shaderType ));

		for (EShaderOpt opt : BitfieldIterate( in.options ))
		{
			switch_enum( opt )
			{
				case EShaderOpt::Trace :
				case EShaderOpt::FnProfiling :
				case EShaderOpt::TimeHeatMap :
					RETURN_ERR( "not supported" );

				case EShaderOpt::DebugInfo :
				{
					AddIntOption( CompilerOptionName::DebugInformation, SLANG_DEBUG_INFO_LEVEL_MAXIMAL );
					AddIntOption( CompilerOptionName::MinimumSlangOptimization, 1 );
					break;
				}

				case EShaderOpt::Optimize :
				case EShaderOpt::OptimizeSize :
				case EShaderOpt::StrongOptimization :
				{
					AddIntOption( CompilerOptionName::Optimization, 3 );	// O3
					AddIntOption( CompilerOptionName::Obfuscate, 1 );
					break;
				}

				case EShaderOpt::WarnAsError :
				{
					AddIntOption( CompilerOptionName::WarningsAsErrors, 1 );
					break;
				}

				case EShaderOpt::_ShaderTrace_Mask :
				case EShaderOpt::_Last :
				case EShaderOpt::All :
				case EShaderOpt::Unknown :	break;
			}
			switch_end
		}
		
		if ( is_spirv )
		{
			// use 'Main' instead of default 'main'
			AddIntOption( CompilerOptionName::VulkanUseEntryPointName, 1 );
			
			if ( AllBits( in.options, EShaderOpt::DebugInfo ))
			{
				AddIntOption( CompilerOptionName::VulkanEmitReflection, 1 );
			}

		  #ifdef AE_ENABLE_SPIRV_TOOLS
			AddIntOption( CompilerOptionName::SkipSPIRVValidation, 1 );
		  #endif
		}

	//	options.emplace_back( CompilerOptionName::DisableWarnings );
	//	options.emplace_back( CompilerOptionName::EnableWarning );
	//	options.emplace_back( CompilerOptionName::DisableWarning );

	//	options.emplace_back( CompilerOptionName::SourceEmbedStyle );
	//	options.emplace_back( CompilerOptionName::SourceEmbedName );
	//	options.emplace_back( CompilerOptionName::SourceEmbedLanguage );

	//	options.emplace_back( CompilerOptionName::Capability );
	//	options.emplace_back( CompilerOptionName::FloatingPointMode );
	//	options.emplace_back( CompilerOptionName::LineDirectiveMode );
	//	options.emplace_back( CompilerOptionName::DebugInformationFormat );
	#endif

		TargetDesc	target_desc [1]		= {};
		target_desc[0].flags					= 0;
		target_desc[0].compilerOptionEntries	= options.data();
		target_desc[0].compilerOptionEntryCount	= uint(options.size());

		if ( is_spirv )
		{
			Version2	ver = EShaderVersion_Ver2( in.dstVersion );
			String		ver_str {"spirv_"};

			ver_str << char('0' + ver.major) << '_' << char('0' + ver.minor);

			target_desc[0].format	= SLANG_SPIRV;
			target_desc[0].profile	= _globalSession->findProfile( ver_str.c_str() );

			target_desc[0].lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_GLSL;

			CHECK_ERR( target_desc[0].profile != SLANG_PROFILE_UNKNOWN );
		}
		else
		if ( is_metal )
		{
			// TODO
			target_desc[0].format	= SLANG_METAL_LIB;
		}
		else
		if ( is_cxx )
		{
			target_desc[0].format	= SLANG_CPP_SOURCE;
		}
		else
		if ( is_glsl )
		{
			target_desc[0].format	= SLANG_GLSL;
		}
		else
		{
			RETURN_ERR( "unsupported shader type or version" );
		}

		Array<const char*>	include_dirs;
		include_dirs.resize( _includeDirs.size() );

		for (usize i = 0; i < _includeDirs.size(); ++i) {
			include_dirs[i] = _includeDirs[i].c_str();
		}

		SessionDesc	session_desc = {};
		session_desc.defaultMatrixLayoutMode	= SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
		session_desc.targetCount				= CountOf(target_desc);
		session_desc.targets					= target_desc;

		session_desc.searchPathCount			= include_dirs.size();
		session_desc.searchPaths				= include_dirs.data();

		SLANG_CHECK_ERR( _globalSession->createSession( session_desc, OUT &_session ));
		return true;
	}
	
/*
=================================================
	_EndSession
=================================================
*/
	void  SLangCompiler::_EndSession ()
	{
		if ( _session != null )
		{
			_session->release();
			_session = null;
		}
	}

/*
=================================================
	Compile
=================================================
*/
	bool  SLangCompiler::Compile (const Input &in, OUT Output &out) __NE___
	{
		try{
			return _CompileImpl( in, OUT out );
		}
		catch(...){
			_EndSession();
			return false;
		}
	}
		
/*
=================================================
	_CompileImpl
=================================================
*/
	bool  SLangCompiler::_CompileImpl (const Input &in, OUT Output &out) __Th___
	{
		CHECK_ERR( _BeginSession( in ));

		ComPtr<IModule>	src_module;
		{
			ComPtr<IBlob>	diagnostic_blob;
			src_module = _session->loadModuleFromSourceString(
											"name",
											ToString( in.fileLoc.path ).c_str(),
											("#line 0 \"header\"\n"s << in.header
											 << "\n#line 0 \"source\"\n" << in.source).c_str(),
											OUT &diagnostic_blob );

			if ( diagnostic_blob )
				out.log << Cast<char>(diagnostic_blob->getBufferPointer()) << "\n\n";

			if ( not src_module )
				return false;
		}
		
		ComPtr<IEntryPoint>		entry_point;
		SLANG_CHECK_LOG( src_module->findEntryPointByName( NtStringView{in.entry}.c_str(), OUT &entry_point ));

		Array<IComponentType*>	component_types;
		component_types.push_back( src_module );
		component_types.push_back( entry_point );
		
		ComPtr<IComponentType>	composed_program;
		{
			ComPtr<IBlob>	diagnostic_blob;
			SlangResult		result = _session->createCompositeComponentType(
												component_types.data(),
												component_types.size(),
												OUT &composed_program,
												OUT &diagnostic_blob );
			if ( diagnostic_blob )
				out.log << Cast<char>(diagnostic_blob->getBufferPointer()) << "\n\n";

			SLANG_CHECK_LOG( result, "createCompositeComponentType()" );
		}

		const uint		target_idx		= 0;
		const uint		entry_point_idx	= 0;
		ComPtr<IBlob>	shader_bytecode;
		{
			ComPtr<IBlob>	diagnostic_blob;

			SlangResult	result = composed_program->getEntryPointCode(
												entry_point_idx,
												target_idx,
												OUT &shader_bytecode,
												OUT &diagnostic_blob );
			if ( diagnostic_blob )
				out.log << Cast<char>(diagnostic_blob->getBufferPointer()) << "\n\n";

			SLANG_CHECK_LOG( result, "getEntryPointCode()" );
		}

		// reflection
		{
			ProgramLayout* layout = composed_program->getLayout( target_idx, null );
			CHECK_ERR( layout != null );

			CHECK_ERR( _ParseReflection( *layout, OUT out.reflection ));  // throw
		}

		// copy shader bytecode
		{
			EShaderVersion	dst_type	= in.dstVersion & EShaderVersion::_Mask;
			const bool		is_spirv	= AnyEqual( dst_type, EShaderVersion::_GLSL_SPIRV, EShaderVersion::_Slang_SPIRV );
			const bool		is_metal	= AnyEqual( dst_type, EShaderVersion::_Metal, EShaderVersion::_Metal_iOS, EShaderVersion::_Metal_Mac );
			const bool		is_source	= AnyEqual( dst_type, EShaderVersion::_Slang_CXX, EShaderVersion::_Slang_GLSL );

			if ( is_spirv )
			{
				out.spirv.assign( Cast<uint>(shader_bytecode->getBufferPointer()), Cast<uint>(shader_bytecode->getBufferPointer()) + Bytes{shader_bytecode->getBufferSize()} );  // throw
				CHECK( ArraySizeOf(out.spirv) == shader_bytecode->getBufferSize() );
			}else
			if ( is_metal )
			{
				out.metal.assign( Cast<ubyte>(shader_bytecode->getBufferPointer()), Cast<ubyte>(shader_bytecode->getBufferPointer()) + Bytes{shader_bytecode->getBufferSize()} );  // throw
				CHECK( ArraySizeOf(out.metal) == shader_bytecode->getBufferSize() );
			}else
			if ( is_source )
			{
				out.source.assign( Cast<char>(shader_bytecode->getBufferPointer()), Cast<char>(shader_bytecode->getBufferPointer()) + Bytes{shader_bytecode->getBufferSize()} );  // throw
				CHECK( StringSizeOf(out.source) == shader_bytecode->getBufferSize() );
			}else
			{
				RETURN_ERR( "unsupported output type" );
			}
		}

		_EndSession();

	  #ifdef AE_ENABLE_SPIRV_TOOLS
		if ( not ValidateSPIRV( out.spirv, in.dstVersion, INOUT out.log ))
			return false;
	  #endif

		return true;
	}
//-----------------------------------------------------------------------------


namespace {
/*
=================================================
	AddUniform
=================================================
*/
	ND_ static DescriptorSetLayoutDesc::Uniform*  AddUniform (VariableLayoutReflection* param, INOUT SLangCompiler::ShaderReflection &result)
	{
		CHECK_ERR( param != null );

		const char*				name			= param->getName();
		const uint				category_count	= param->getCategoryCount();
		TypeLayoutReflection*	layout			= param->getTypeLayout();
		
		CHECK_ERR( category_count == 1 );
		CHECK_ERR( name != null );

		const auto	category = SlangParameterCategory(param->getCategoryByIndex( 0 ));
		CHECK_ERR( category == SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT );

		usize	index	= param->getOffset( category );
		usize	space	= param->getBindingSpace( category );
		usize	count	= layout->getSize( category );

		result.layout.descrSets.resize( space+1 );
		result.layout.descrSets[ space ].bindingIndex = uint(space);

		auto&	ds = result.layout.descrSets[ space ];
		auto&	[un_name, uniform] = ds.layout.uniforms.emplace_back();

		un_name = UniformName{name};
		uniform.binding.vkIndex = uint(index);

		ASSERT( count <= 1 );
		uniform.arraySize = CheckCast{ count };

		return &uniform;
	}
	
/*
=================================================
	InitBufferFromStructLayout
=================================================
*/
	ND_ static bool  InitBufferFromStructLayout (TypeLayoutReflection* structLayout, uint arraySize, OUT DescriptorSetLayoutDesc::Buffer &buf)
	{
		CHECK_ERR( structLayout != null );
		CHECK_ERR( structLayout->getKind() == TypeReflection::Kind::Struct );

		const char*		type_name = structLayout->getName();
		CHECK_ERR( type_name != null );

		buf				= {};
		buf.typeName	= ShaderStructName{type_name};
		buf.staticSize	= 0_b;
		buf.arrayStride	= 0_b;

		const uint	count = structLayout->getFieldCount();
		for (uint i = 0; i < count; ++i)
		{
			VariableLayoutReflection*	field			= structLayout->getFieldByIndex( i );
			TypeLayoutReflection*		field_layout	= field->getTypeLayout();
			const uint					category_count	= field->getCategoryCount();
			CHECK_ERR( category_count == 1 );

			const auto					category		= SlangParameterCategory(field->getCategoryByIndex( 0 ));
			CHECK_ERR( category == SLANG_PARAMETER_CATEGORY_UNIFORM );
			
			usize	offset			= field->getOffset( category );
			usize	size			= field_layout->getSize( category );
			usize	element_stride	= field_layout->getElementStride( category );

			buf.staticSize	= Max( buf.staticSize, Bytes{offset + size} );
			buf.arrayStride	= Max( buf.arrayStride, Bytes{element_stride} );
		}

		if ( arraySize <= 1 )
		{
			CHECK( buf.staticSize > 0 );
			buf.arrayStride = 0_b;
		}
		else
		{
			CHECK( buf.arrayStride > 0 );
		}
		return true;
	}
	
/*
=================================================
	EnumCast (SlangImageFormat)
=================================================
*/
	ND_ static EPixelFormat  EnumCast (SlangImageFormat fmt)
	{
		switch_enum( fmt )
		{
			case SLANG_IMAGE_FORMAT_rgba32f :			return EPixelFormat::RGBA32F;
			case SLANG_IMAGE_FORMAT_rgba16f :			return EPixelFormat::RGBA16F;
			case SLANG_IMAGE_FORMAT_rg32f :				return EPixelFormat::RG32F;
			case SLANG_IMAGE_FORMAT_rg16f :				return EPixelFormat::RG16F;
			case SLANG_IMAGE_FORMAT_r11f_g11f_b10f :	return EPixelFormat::R11G11B10F;
			case SLANG_IMAGE_FORMAT_r32f :				return EPixelFormat::R32F;
			case SLANG_IMAGE_FORMAT_r16f :				return EPixelFormat::R16F;
			case SLANG_IMAGE_FORMAT_rgba16 :			return EPixelFormat::RGBA16_UNorm;
			case SLANG_IMAGE_FORMAT_rgb10_a2 :			return EPixelFormat::RGB10_A2_UNorm;
			case SLANG_IMAGE_FORMAT_rgba8 :				return EPixelFormat::RGBA8_UNorm;
			case SLANG_IMAGE_FORMAT_rg16 :				return EPixelFormat::RG16_UNorm;
			case SLANG_IMAGE_FORMAT_rg8 :				return EPixelFormat::RG8_UNorm;
			case SLANG_IMAGE_FORMAT_r16 :				return EPixelFormat::R16_UNorm;
			case SLANG_IMAGE_FORMAT_r8 :				return EPixelFormat::R8_UNorm;
			case SLANG_IMAGE_FORMAT_rgba16_snorm :		return EPixelFormat::RGBA16_SNorm;
			case SLANG_IMAGE_FORMAT_rgba8_snorm :		return EPixelFormat::RGBA8_SNorm;
			case SLANG_IMAGE_FORMAT_rg16_snorm :		return EPixelFormat::RG16_SNorm;
			case SLANG_IMAGE_FORMAT_rg8_snorm :			return EPixelFormat::RG8_SNorm;
			case SLANG_IMAGE_FORMAT_r16_snorm :			return EPixelFormat::R16_SNorm;
			case SLANG_IMAGE_FORMAT_r8_snorm :			return EPixelFormat::R8_SNorm;
			case SLANG_IMAGE_FORMAT_rgba32i :			return EPixelFormat::RGBA32I;
			case SLANG_IMAGE_FORMAT_rgba16i :			return EPixelFormat::RGBA16I;
			case SLANG_IMAGE_FORMAT_rgba8i :			return EPixelFormat::RGBA8I;
			case SLANG_IMAGE_FORMAT_rg32i :				return EPixelFormat::RG32I;
			case SLANG_IMAGE_FORMAT_rg16i :				return EPixelFormat::RG16I;
			case SLANG_IMAGE_FORMAT_rg8i :				return EPixelFormat::RG8I;
			case SLANG_IMAGE_FORMAT_r32i :				return EPixelFormat::R32I;
			case SLANG_IMAGE_FORMAT_r16i :				return EPixelFormat::R16I;
			case SLANG_IMAGE_FORMAT_r8i :				return EPixelFormat::R8I;
			case SLANG_IMAGE_FORMAT_rgba32ui :			return EPixelFormat::RGBA32U;
			case SLANG_IMAGE_FORMAT_rgba16ui :			return EPixelFormat::RGBA16U;
			case SLANG_IMAGE_FORMAT_rgb10_a2ui :		return EPixelFormat::RGB10_A2U;
			case SLANG_IMAGE_FORMAT_rgba8ui :			return EPixelFormat::RGBA8U;
			case SLANG_IMAGE_FORMAT_rg32ui :			return EPixelFormat::RG32U;
			case SLANG_IMAGE_FORMAT_rg16ui :			return EPixelFormat::RG16U;
			case SLANG_IMAGE_FORMAT_rg8ui :				return EPixelFormat::RG8U;
			case SLANG_IMAGE_FORMAT_r32ui :				return EPixelFormat::R32U;
			case SLANG_IMAGE_FORMAT_r16ui :				return EPixelFormat::R16U;
			case SLANG_IMAGE_FORMAT_r8ui :				return EPixelFormat::R8U;
			case SLANG_IMAGE_FORMAT_r64ui :				return EPixelFormat::R64U;
			case SLANG_IMAGE_FORMAT_r64i :				return EPixelFormat::R64I;
			case SLANG_IMAGE_FORMAT_bgra8 :				return EPixelFormat::BGRA8_UNorm;
			case SLANG_IMAGE_FORMAT_unknown :
			default :									break;
		}
		switch_end
		RETURN_ERR( "unknown format" );
	}

/*
=================================================
	InitImage
=================================================
*/
	ND_ static bool  InitImage (VariableLayoutReflection* param, TypeReflection* type, OUT EDescriptorType &descType, OUT DescriptorSetLayoutDesc::Image &image)
	{
		image = {};

		SlangResourceAccess		access	= type->getResourceAccess();
		SlangResourceShape		shape	= type->getResourceShape();

		descType =	shape & SLANG_TEXTURE_COMBINED_FLAG ?	EDescriptorType::CombinedImage :
					(access != 0 ?							EDescriptorType::StorageImage
					 :										EDescriptorType::SampledImage);

		const bool	is_array	= AllBits( shape, SLANG_TEXTURE_ARRAY_FLAG );
		const bool	is_ms		= AllBits( shape, SLANG_TEXTURE_MULTISAMPLE_FLAG );

		switch ( shape & SLANG_RESOURCE_BASE_SHAPE_MASK )
		{
			case SLANG_TEXTURE_1D :
				image.type = is_array ? EImageType::Dim1DArray : EImageType::Dim1D;
				break;

			case SLANG_TEXTURE_2D :
				image.type = is_ms ?
						(is_array ? EImageType::Dim2DMSArray : EImageType::Dim2DMS) :
						(is_array ? EImageType::Dim2DArray : EImageType::Dim2D);
				break;

			case SLANG_TEXTURE_3D :
				image.type = EImageType::Dim3D;
				break;

			case SLANG_TEXTURE_CUBE :
				image.type = is_array ? EImageType::DimCubeArray : EImageType::DimCube;
				break;
		}

		if ( AllBits( shape, SLANG_TEXTURE_SHADOW_FLAG ))
			image.type |= EImageType::Shadow;

		TypeReflection*		result_type	= type->getResourceResultType();
		CHECK_ERR( result_type != null );
		CHECK_ERR( result_type->getKind() == slang::TypeReflection::Kind::Vector );

		//result_type->getElementCount();

		TypeReflection*		elem_type	= result_type->getElementType();
		CHECK_ERR( elem_type != null );
		CHECK_ERR( elem_type->getKind() == slang::TypeReflection::Kind::Scalar );
		
		SlangScalarType		scalar_type	= SlangScalarType(elem_type->getScalarType());
		switch_enum( scalar_type )
		{
			case SLANG_SCALAR_TYPE_INT8 :
			case SLANG_SCALAR_TYPE_INT16 :
			case SLANG_SCALAR_TYPE_INTPTR :
			case SLANG_SCALAR_TYPE_INT32 :		image.type |= EImageType::Int;		break;
			case SLANG_SCALAR_TYPE_UINT8 :
			case SLANG_SCALAR_TYPE_UINT16 :
			case SLANG_SCALAR_TYPE_UINTPTR :
			case SLANG_SCALAR_TYPE_UINT32 :		image.type |= EImageType::UInt;		break;

			case SLANG_SCALAR_TYPE_INT64 :		image.type |= EImageType::SLong;	break;
			case SLANG_SCALAR_TYPE_UINT64 :		image.type |= EImageType::ULong;	break;
			case SLANG_SCALAR_TYPE_FLOAT16 :	image.type |= EImageType::Half;		break;
			case SLANG_SCALAR_TYPE_FLOAT32 :	image.type |= EImageType::Float;	break;

			case SLANG_SCALAR_TYPE_NONE :
			case SLANG_SCALAR_TYPE_VOID :
			case SLANG_SCALAR_TYPE_BOOL :
			case SLANG_SCALAR_TYPE_FLOAT64 :
			default :							RETURN_ERR( "unsupported scalar type" );
		}
		switch_end

		if ( descType == EDescriptorType::StorageImage )
		{
			image.format = EnumCast( param->getImageFormat() );
			CHECK_ERR( image.format != Default );

			switch_enum( access )
			{
				case SLANG_RESOURCE_ACCESS_READ :		image.state = EResourceState::ShaderStorage_Read;	break;
				case SLANG_RESOURCE_ACCESS_READ_WRITE :	image.state = EResourceState::ShaderStorage_RW;		break;
				case SLANG_RESOURCE_ACCESS_WRITE :		image.state = EResourceState::ShaderStorage_Write;	break;

				case SLANG_RESOURCE_ACCESS_RASTER_ORDERED :
				case SLANG_RESOURCE_ACCESS_APPEND :
				case SLANG_RESOURCE_ACCESS_CONSUME :
				case SLANG_RESOURCE_ACCESS_NONE :
				case SLANG_RESOURCE_ACCESS_FEEDBACK :
				case SLANG_RESOURCE_ACCESS_UNKNOWN :
				default :								RETURN_ERR( "unsupported resource access" );
			}
			switch_end
		}
		return true;
	}

/*
=================================================
	AddResourceReflection
=================================================
*/
	ND_ static bool  AddResourceReflection (VariableLayoutReflection* param, INOUT SLangCompiler::ShaderReflection &result)
	{
		CHECK_ERR( param != null );

		TypeLayoutReflection*	layout		= param->getTypeLayout();
		TypeReflection*			type		= layout->getType();
		SlangResourceShape		base_type	= SlangResourceShape(type->getResourceShape() & SLANG_RESOURCE_BASE_SHAPE_MASK);

		auto*	uniform = AddUniform( param, INOUT result );
		CHECK_ERR( uniform != null );

		if ( base_type == SLANG_STRUCTURED_BUFFER )
		{
			uniform->type = EDescriptorType::StorageBuffer;

			CHECK_ERR( InitBufferFromStructLayout( layout->getElementTypeLayout(), uniform->arraySize, OUT uniform->buffer ));
			return true;
		}

		if ( base_type >= SLANG_TEXTURE_1D and base_type <= SLANG_TEXTURE_CUBE )
		{
			CHECK_ERR( InitImage( param, type, OUT uniform->type, OUT uniform->image ));
			return true;
		}

		if ( base_type == SLANG_TEXTURE_SUBPASS )
		{
			uniform->type = EDescriptorType::SubpassInput;

			TODO("");
			return true;
		}

		if ( base_type == SLANG_ACCELERATION_STRUCTURE )
		{
			uniform->type = EDescriptorType::RayTracingScene;

			TODO("");
			return true;
		}

		if ( base_type == SLANG_TEXTURE_BUFFER or base_type == SLANG_BYTE_ADDRESS_BUFFER )
		{
			uniform->type = EDescriptorType::UniformTexelBuffer;

			TODO("");
			return true;
		}

		return false;
	}
	
/*
=================================================
	AddPushConstant
=================================================
*/
	ND_ static bool  AddPushConstant (VariableLayoutReflection* param, INOUT SLangCompiler::ShaderReflection &result)
	{
		CHECK_ERR( param != null );

		uint	category_count = param->getCategoryCount();
		if ( category_count != 1 )
			return false;

		auto	category = SlangParameterCategory(param->getCategoryByIndex( 0 ));
		if ( category != SLANG_PARAMETER_CATEGORY_PUSH_CONSTANT_BUFFER )
			return false;
		
		TypeLayoutReflection*	layout = param->getTypeLayout();
		CHECK_ERR( layout != null );

		VariableLayoutReflection*	struct_var		= layout->getElementVarLayout();
		CHECK_ERR( struct_var != null );
		
		TypeLayoutReflection*		struct_layout	= struct_var->getTypeLayout();
		DescriptorSetLayoutDesc::Buffer	buf;
		CHECK_ERR( InitBufferFromStructLayout( struct_layout, 0, OUT buf ));
		
		category_count	= struct_var->getCategoryCount();
		CHECK_ERR( category_count == 1 );

		category		= SlangParameterCategory(struct_var->getCategoryByIndex( 0 ));
		CHECK_ERR( category == SLANG_PARAMETER_CATEGORY_UNIFORM );
		
		const char*		name			= param->getName();
		CHECK_ERR( name != null );

		PushConstants::PushConst	pc;
		pc.typeName		= buf.typeName;
		pc.vulkanOffset	= CheckCast{ struct_var->getOffset( category )};
		pc.size			= CheckCast{ struct_layout->getSize( category )};

		ASSERT( buf.staticSize <= pc.size );

		result.layout.pushConstants.items.emplace( PushConstantName{name}, pc );
		return true;
	}

/*
=================================================
	AddConstantBuffer
=================================================
*/
	ND_ static bool  AddConstantBuffer (VariableLayoutReflection* param, INOUT SLangCompiler::ShaderReflection &result)
	{
		CHECK_ERR( param != null );

		if ( AddPushConstant( param, INOUT result ))
			return true;

		TypeLayoutReflection*	layout = param->getTypeLayout();
		CHECK_ERR( layout != null );

		auto*	uniform = AddUniform( param, INOUT result );
		CHECK_ERR( uniform != null );

		uniform->type = EDescriptorType::UniformBuffer;
		
		VariableLayoutReflection*	struct_var		= layout->getElementVarLayout();
		CHECK_ERR( struct_var != null );
		
		TypeLayoutReflection*		struct_layout	= struct_var->getTypeLayout();
		CHECK_ERR( InitBufferFromStructLayout( struct_layout, uniform->arraySize, OUT uniform->buffer ));
		
		const uint		category_count	= struct_var->getCategoryCount();
		CHECK_ERR( category_count == 1 );

		const auto		category		= SlangParameterCategory(struct_var->getCategoryByIndex( 0 ));
		CHECK_ERR( category == SLANG_PARAMETER_CATEGORY_UNIFORM );
			
		usize	offset	= struct_var->getOffset( category );
		usize	size	= struct_layout->getSize( category );
		
		CHECK_Eq( AlignUp( uniform->buffer.staticSize, 16_b ), Bytes{offset + size} );
		CHECK( uniform->buffer.arrayStride == 0 );

		return true;
	}
	
/*
=================================================
	ReadEntryPointAttribs
=================================================
*/
	ND_ static bool  ReadEntryPointAttribs (slang::ShaderReflection &mainLayout, OUT SLangCompiler::ShaderReflection &result)
	{
		ulong	entry_count = mainLayout.getEntryPointCount();
		CHECK_ERR( entry_count == 1 );

		EntryPointReflection*	entry = mainLayout.getEntryPointByIndex( 0 );
		CHECK_ERR( entry != null );

		EShader			sh_type	= EnumCast( entry->getStage() );
		EShaderStages	stage	= EShaderStages(0) | sh_type;
		
		// set stage
		for (auto& ds : result.layout.descrSets)
		{
			for (auto& [name, un] : ds.layout.uniforms)
			{
				un.stages = stage;
			}
		}

		for (auto [name, pc] : result.layout.pushConstants.items)
		{
			pc.stage = sh_type;
		}

		// entry->usesAnySampleRateInput()

		if ( sh_type == EShader::Compute )
		{
			SlangUInt	thread_group_size [3];
			entry->getComputeThreadGroupSize( 3, OUT thread_group_size );

			result.compute.localGroupSize = uint3{ uint(thread_group_size[0]), uint(thread_group_size[1]), uint(thread_group_size[2]) };
		}

		return true;
	}

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	_ParseReflection
----
	https://shader-slang.org/slang/user-guide/reflection.html
	based on 'spReflection_ToJson' implementation
=================================================
*/
	bool  SLangCompiler::_ParseReflection (slang::ShaderReflection &mainLayout, OUT ShaderReflection &result) __Th___
	{
	#if 0 // for debugging
		ComPtr<IBlob>	json_blob;
		SlangResult	res = mainLayout.toJson( OUT &json_blob );

		SLANG_CHECK_ERR_MSG( res, "ShaderReflection::toJson()" );

		AE_LOGI( Cast<char>(json_blob->getBufferPointer()) );
	#endif

		const uint	parameter_count = mainLayout.getParameterCount();
		for (uint i = 0; i < parameter_count; ++i)
		{
			VariableLayoutReflection*	parameter	= mainLayout.getParameterByIndex( i );
			TypeLayoutReflection*		layout		= parameter->getTypeLayout();

			switch ( layout->getKind() )
			{
				case TypeReflection::Kind::Resource :			CHECK_ERR( AddResourceReflection( parameter, INOUT result ));	break;
				case TypeReflection::Kind::ConstantBuffer :		CHECK_ERR( AddConstantBuffer( parameter, INOUT result ));		break;
			}
		}
		CHECK_ERR( ReadEntryPointAttribs( mainLayout, INOUT result ));

		// remove empty DS
		for (usize i = 0; i < result.layout.descrSets.size();)
		{
			if ( result.layout.descrSets[i].bindingIndex == UMax )
				result.layout.descrSets.erase( i );
			else
				++i;
		}
		return true;
	}

} // AE::PipelineCompiler
//-----------------------------------------------------------------------------

extern "C"
{
	unsigned						spReflection_GetParameterCount (SlangReflection* reflection)												{ return SlangFunctions::_var_spReflection_GetParameterCount( reflection ); }
	SlangReflectionTypeParameter*	spReflection_GetTypeParameterByIndex (SlangReflection* reflection, unsigned int index)						{ return SlangFunctions::_var_spReflection_GetTypeParameterByIndex( reflection, index ); }
	SlangReflectionParameter*		spReflection_GetParameterByIndex (SlangReflection* reflection, unsigned index)								{ return SlangFunctions::_var_spReflection_GetParameterByIndex( reflection, index ); }
	SlangReflectionVariableLayout*	spReflection_getGlobalParamsVarLayout (SlangReflection* reflection)											{ return SlangFunctions::_var_spReflection_getGlobalParamsVarLayout( reflection ); }
	SlangResult						spReflection_ToJson (SlangReflection* reflection, SlangCompileRequest* request, ISlangBlob** outBlob)		{ return SlangFunctions::_var_spReflection_ToJson( reflection, request, outBlob ); }
	SlangUInt						spReflection_getEntryPointCount (SlangReflection* reflection)												{ return SlangFunctions::_var_spReflection_getEntryPointCount( reflection ); }
	SlangReflectionEntryPoint*		spReflection_getEntryPointByIndex (SlangReflection* reflection, SlangUInt index)							{ return SlangFunctions::_var_spReflection_getEntryPointByIndex( reflection, index ); }

	char const*						spReflectionTypeParameter_GetName (SlangReflectionTypeParameter* typeParam)									{ return SlangFunctions::_var_spReflectionTypeParameter_GetName( typeParam ); }
	
	SlangReflectionTypeLayout*		spReflectionVariableLayout_GetTypeLayout (SlangReflectionVariableLayout* var)								{ return SlangFunctions::_var_spReflectionVariableLayout_GetTypeLayout( var ); }
	SlangReflectionVariable*		spReflectionVariableLayout_GetVariable (SlangReflectionVariableLayout* var)									{ return SlangFunctions::_var_spReflectionVariableLayout_GetVariable( var ); }
	size_t							spReflectionVariableLayout_GetOffset (SlangReflectionVariableLayout* var, SlangParameterCategory category)	{ return SlangFunctions::_var_spReflectionVariableLayout_GetOffset( var, category ); }
	size_t							spReflectionVariableLayout_GetSpace (SlangReflectionVariableLayout* var, SlangParameterCategory category)	{ return SlangFunctions::_var_spReflectionVariableLayout_GetSpace( var, category ); }
	SlangImageFormat				spReflectionVariableLayout_GetImageFormat (SlangReflectionVariableLayout* var)								{ return SlangFunctions::_var_spReflectionVariableLayout_GetImageFormat( var ); }
	
	char const*						spReflectionType_GetName (SlangReflectionType* type)														{ return SlangFunctions::_var_spReflectionType_GetName( type ); }
	SlangResourceShape				spReflectionType_GetResourceShape (SlangReflectionType* type)												{ return SlangFunctions::_var_spReflectionType_GetResourceShape( type ); }
	SlangTypeKind					spReflectionType_GetKind (SlangReflectionType* type)														{ return SlangFunctions::_var_spReflectionType_GetKind( type ); }
	SlangReflectionType*			spReflectionType_GetElementType (SlangReflectionType* type)													{ return SlangFunctions::_var_spReflectionType_GetElementType( type ); }
	SlangScalarType					spReflectionType_GetScalarType (SlangReflectionType* type)													{ return SlangFunctions::_var_spReflectionType_GetScalarType( type ); }
	SlangResourceAccess				spReflectionType_GetResourceAccess (SlangReflectionType* type)												{ return SlangFunctions::_var_spReflectionType_GetResourceAccess( type ); }
	SlangReflectionType*			spReflectionType_GetResourceResultType (SlangReflectionType* type)											{ return SlangFunctions::_var_spReflectionType_GetResourceResultType( type ); }

	SlangTypeKind					spReflectionTypeLayout_getKind (SlangReflectionTypeLayout* type)											{ return SlangFunctions::_var_spReflectionTypeLayout_getKind( type ); }
	SlangReflectionType*			spReflectionTypeLayout_GetType (SlangReflectionTypeLayout* type)											{ return SlangFunctions::_var_spReflectionTypeLayout_GetType( type ); }
	uint32_t						spReflectionTypeLayout_GetFieldCount (SlangReflectionTypeLayout* type)										{ return SlangFunctions::_var_spReflectionTypeLayout_GetFieldCount( type ); }
	SlangReflectionVariableLayout*  spReflectionTypeLayout_GetFieldByIndex (SlangReflectionTypeLayout* type, unsigned index)					{ return SlangFunctions::_var_spReflectionTypeLayout_GetFieldByIndex( type, index ); }
	SlangReflectionVariableLayout*  spReflectionTypeLayout_GetElementVarLayout (SlangReflectionTypeLayout* type)								{ return SlangFunctions::_var_spReflectionTypeLayout_GetElementVarLayout( type ); }
	SlangReflectionVariableLayout*  spReflectionTypeLayout_getContainerVarLayout (SlangReflectionTypeLayout* type)								{ return SlangFunctions::_var_spReflectionTypeLayout_getContainerVarLayout( type ); }
	size_t							spReflectionTypeLayout_GetSize (SlangReflectionTypeLayout* type, SlangParameterCategory category)			{ return SlangFunctions::_var_spReflectionTypeLayout_GetSize( type, category ); }
	size_t							spReflectionTypeLayout_GetElementStride (SlangReflectionTypeLayout* type, SlangParameterCategory category)	{ return SlangFunctions::_var_spReflectionTypeLayout_GetElementStride( type, category ); }
	SlangReflectionTypeLayout*		spReflectionTypeLayout_GetElementTypeLayout (SlangReflectionTypeLayout* type)								{ return SlangFunctions::_var_spReflectionTypeLayout_GetElementTypeLayout( type ); }
	unsigned						spReflectionTypeLayout_GetCategoryCount (SlangReflectionTypeLayout* type)									{ return SlangFunctions::_var_spReflectionTypeLayout_GetCategoryCount( type ); }
	SlangParameterCategory			spReflectionTypeLayout_GetCategoryByIndex (SlangReflectionTypeLayout* type, unsigned index)					{ return SlangFunctions::_var_spReflectionTypeLayout_GetCategoryByIndex( type, index ); }

	char const*						spReflectionVariable_GetName (SlangReflectionVariable* var)													{ return SlangFunctions::_var_spReflectionVariable_GetName( var ); }
	
	SlangStage						spReflectionEntryPoint_getStage (SlangReflectionEntryPoint* entryPoint)										{ return SlangFunctions::_var_spReflectionEntryPoint_getStage( entryPoint ); }
	void							spReflectionEntryPoint_getComputeThreadGroupSize (SlangReflectionEntryPoint* entryPoint, SlangUInt axisCount, SlangUInt* outSizeAlongAxis)	{ return SlangFunctions::_var_spReflectionEntryPoint_getComputeThreadGroupSize( entryPoint, axisCount, outSizeAlongAxis ); }

} // extern "C"
//-----------------------------------------------------------------------------

#else

# include "SlangCompiler.h"

namespace AE::PipelineCompiler
{
	SLangCompiler::SLangCompiler (ArrayView<Path>)				__NE___	{}
	SLangCompiler::~SLangCompiler ()							__NE___	{}
	bool  SLangCompiler::Compile (const Input &, OUT Output &)	__NE___	{ return false; }

} // AE::PipelineCompiler

#endif // AE_ENABLE_SLANG
