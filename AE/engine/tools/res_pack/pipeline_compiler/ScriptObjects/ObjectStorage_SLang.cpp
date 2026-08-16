// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{

/*
=================================================
	CompileShaderSLang
=================================================
*/
	void  ObjectStorage::CompileShaderSLang (OUT CompiledShaderPtr &outShader, const ScriptShaderPtr &inShader, EShaderVersion version,
											 const String &defines, const String &resources, ArrayView<String> include, ArrayView<ScriptFeatureSetPtr> features) __Th___
	{
		CHECK_THROW_MSG( not outShader );
		CHECK_THROW_MSG( inShader );

		ShaderSrcKey	key;
		key.source		= inShader->GetSource();
		key.resources	= resources;
		key.type		= inShader->type;
		key.version		= version;
		key.options		= inShader->options;
		key.include		= Array<String>{ include };
		_SetAndSortDefines( OUT key.defines, String{defines} << inShader->GetDefines() << this->defaultShaderDefines );

		// find in existing shader source
		{
			auto	iter = _shaderSrcMap.find( key );
			if ( iter != _shaderSrcMap.end() )
			{
				outShader = iter->second;
				return;
			}
		}

		// compile shader
		CompiledShader	compiled;
		_CompileShaderSLang( key, features, inShader->GetPath(), inShader->GetEntry(), OUT compiled );

		auto	cs_iter	= _compiledShaders.insert( RVRef(compiled) ).first;
		auto*	spirv	= UnionGet<SpirvBytecode_t>( cs_iter->data );

		CHECK_THROW_MSG( spirv != null );

		cs_iter->uid = pplnStorage->AddSpirvShader( *spirv, cs_iter->reflection.layout.specConstants );

		auto [src_it, src_inserted] = _shaderSrcMap.emplace( RVRef(key), &(*cs_iter) );
		CHECK( src_inserted );

		outShader = src_it->second;
	}

/*
=================================================
	_CompileShaderSLang
=================================================
*/
	void  ObjectStorage::_CompileShaderSLang (const ShaderSrcKey &info, ArrayView<ScriptFeatureSetPtr> features,
											  const PathAndLine &shaderPath, const String &entry, OUT CompiledShader &compiled) __Th___
	{
		CHECK_THROW_MSG( slangCompiler );

		// check SPIRV version
		{
			const uint	ver			= GetMaxValueFromFeatures( features, &FeatureSet::maxShaderVersion ).spirv;
			Version2	max_spv_ver = Version2::From100( ver );
			CHECK_THROW_MSG(( max_spv_ver >= Version2{1,0} ));

			Version2	req_spv_ver = EShaderVersion_Ver2( info.version );
			CHECK_THROW_MSG( req_spv_ver <= max_spv_ver );
		}

		String	header;

		// add header
		{
			header << "\n#define " << ShaderToStr( info.type ) << " 1\n";
			header << "#define ND_\n"
					  "#define or ||\n"
					  "#define and &&\n"
					  "#define SLANG 1\n";

			for (auto& def : info.defines) {
				header << def << '\n';
			}
			header << '\n';

			for (auto& inc : info.include) {
				header << "#include \"" << inc << "\"\n";
			}
			header << '\n';

			header << info.resources;
		}

		SLangCompiler::Output	out;
		SLangCompiler::Input	in;
		in.shaderType			= info.type;
		in.options				= info.options;
		in.dstVersion			= info.version;
		in.entry				= entry.c_str();
		in.header				= header;
		in.source				= info.source;
		in.fileLoc				= shaderPath;

		if_unlikely( not slangCompiler->Compile( in, OUT out ))
		{
			AE_LOGI( "Shader source:\n"s << in.header << '\n' << in.source );
			CHECK_THROW_MSG( false, "Failed to compile shader:\n"s << out.log );
		}

		if ( not out.log.empty() )
		{
			AE_LOG_DBG( "Shader compiled with warnings:\n"s << out.log );
		}

		compiled.version	= info.version;
		compiled.type		= info.type;
		compiled.reflection	= RVRef(out.reflection);
		compiled.data		= RVRef(out.spirv);
	}

} // AE::PipelineCompiler
