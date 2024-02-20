// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Compiler/MetalCompiler.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{

/*
=================================================
    GetShaderExtensionsMSL
=================================================
*/
    String  ObjectStorage::GetShaderExtensionsMSL (INOUT Version2 &metalVer, const EShaderStages stage, ArrayView<ScriptFeatureSetPtr> features) __Th___
    {
        // Metal version
        {
            const uint  ver = Max( metalVer.To100(), GetMaxValueFromFeatures( features, &FeatureSet::maxShaderVersion ).metal );
            metalVer = Version2::From100( ver );
            CHECK_THROW_MSG(( metalVer == Version2{0,0} or metalVer >= Version2{2,0} ));
        }

        String  str;
        str << "#include <metal_stdlib>\n"
            << "#include <simd/simd.h>\n"
            << "using namespace metal;\n\n";

        // ray tracing
        {
            FeatureSetCounter   rt_supported;
            EShaderStages       rt_stages   = Default;

            for (auto& ptr : features)
            {
                rt_supported.Add( ptr->fs.rayQuery );
                rt_stages |= ptr->fs.rayQueryStages;
            }

            if ( rt_supported.IsEnable() and AnyBits( rt_stages, stage ))
            {
                CHECK_THROW_MSG( AnyEqual( stage, EShaderStages::Compute, EShaderStages::Fragment, EShaderStages::Tile ));

                str << "#include <metal_raytracing>\n"
                    << "using namespace raytracing;\n\n";
            }
        }

        // atomic
        {
            FeatureSetCounter   has_atomics;

            for (auto& ptr : features)
            {
                if ( stage != Default and AllBits( EShaderStages::Fragment, stage ))
                    has_atomics.Add( ptr->fs.fragmentStoresAndAtomics );

                if ( stage != Default and AllBits( EShaderStages::Vertex, stage ))
                    has_atomics.Add( ptr->fs.vertexPipelineStoresAndAtomics );
            }

            if ( has_atomics.IsEnable() ) {
                str << "#include <metal_atomic>\n"
                    << "#define AE_HAS_ATOMICS 1\n";
            }
        }

        str << "\n";
        return str;
    }

/*
=================================================
    _CompileShaderMSL
=================================================
*/
    void  ObjectStorage::_CompileShaderMSL (const ShaderSrcKey &info, ArrayView<ScriptFeatureSetPtr> features, const PathAndLine &shaderPath, OUT CompiledShader &compiled) __Th___
    {
        CHECK_THROW_MSG( metalCompiler != null );

        String  source      = info.source;
        usize   hdr_lines   = 0;

        // add header
        {
            Version2    max_msl_ver;
            String      header = GetShaderExtensionsMSL( INOUT max_msl_ver, EShaderStages::Unknown | info.type, features );

            CHECK_THROW_MSG( EShaderVersion_Ver2( info.version ) <= max_msl_ver );

            header << "\n#define " << ShaderToStr( info.type ) << " 1\n";
            header << "#define ND_\n";

            for (auto& def : info.defines) {
                header << def << '\n';
            }
            header << '\n';

            for (auto& inc : info.include) {
                header << "#include \"" << inc << "\"\n";
            }
            header << '\n';

            header << info.resources;

            source.insert( 0, header.c_str() );
            hdr_lines = Parser::CalculateNumberOfLines( header );
        }

        MetalCompiler::Input    in;
        in.target               = target;
        in.options              = info.options;
        in.version              = info.version;
        in.shaderType           = info.type;
        in.source               = source;
        in.headerLines          = hdr_lines;
        in.fileLoc              = shaderPath;
        in.enablePreprocessing  = true;

        MetalBytecode_t bytecode;
        String          log;

        if ( not metalCompiler->Compile( in, OUT bytecode, OUT log ))
        {
            CHECK_THROW_MSG( false, "Failed to compile shader:\n"s << log << "\nsource:\n" << source );
        }

        if ( not log.empty() )
        {
            AE_LOG_DBG( "Shader compiled with warnings:\n"s << log );
        }

        if ( not metalCompiler->BuildReflection( in, INOUT compiled.reflection, OUT log ))
        {
            CHECK_THROW_MSG( false, "Failed to build reflection:\n"s << log );
        }

        compiled.version    = info.version;
        compiled.type       = info.type;
        compiled.data       = RVRef(bytecode);
    }

/*
=================================================
    CompileShaderMSL
=================================================
*/
    void  ObjectStorage::CompileShaderMSL (INOUT CompiledShaderPtr &outShader, const ScriptShaderPtr &inShader, EShaderVersion version,
                                            const String &defines, const String &resources, ArrayView<String> include, ArrayView<ScriptFeatureSetPtr> features) __Th___
    {
        CHECK_THROW_MSG( not outShader );
        CHECK_THROW_MSG( target == ECompilationTarget::Metal_iOS or target == ECompilationTarget::Metal_Mac );
        CHECK_THROW_MSG( AllBits( version, EShaderVersion::_Metal )     or
                         AllBits( version, EShaderVersion::_Metal_iOS ) or
                         AllBits( version, EShaderVersion::_Metal_Mac ));

        // validate options
        for (auto opt : BitfieldIterate( inShader->options ))
        {
            switch_enum( opt )
            {
                case EShaderOpt::DebugInfo :
                case EShaderOpt::Optimize :
                case EShaderOpt::OptimizeSize :
                case EShaderOpt::StrongOptimization :
                case EShaderOpt::WarnAsError :          break;  // ok

                case EShaderOpt::Trace :
                case EShaderOpt::FnProfiling :
                case EShaderOpt::TimeHeatMap :
                case EShaderOpt::Unknown :
                case EShaderOpt::_ShaderTrace_Mask :
                case EShaderOpt::_Last :
                case EShaderOpt::All :
                default :                               CHECK_THROW_MSG( false, "unknown shader option" );
            }
            switch_end
        }

        ShaderSrcKey    key;
        key.source      = inShader->GetSource();
        key.resources   = resources;
        key.type        = inShader->type;
        key.version     = version;
        key.options     = inShader->options;
        key.include     = Array<String>{ include };
        _SetAndSortDefines( OUT key.defines, String{defines} << inShader->GetDefines() );

        // find in existing shader source
        {
            auto    iter = _shaderSrcMap.find( key );
            if ( iter != _shaderSrcMap.end() )
            {
                outShader = iter->second;
                return;
            }
        }

        // compile shader
        CompiledShader  compiled;
        _CompileShaderMSL( key, features, inShader->GetPath(), OUT compiled );

        compiled.reflection.layout.specConstants = inShader->GetSpec();

        const auto  AddMetalBytecode = [&] (ShaderUID (PipelineStorage::*fn) (MetalBytecode_t, const ShaderBytecode::SpecConstants_t &)) -> CompiledShaderPtr
        {{
            auto    cs_iter     = _compiledShaders.insert( RVRef(compiled) ).first;
            auto*   mtbc_ptr    = UnionGet<MetalBytecode_t>( cs_iter->data );
            CHECK_THROW_MSG( mtbc_ptr != null );

            cs_iter->uid = ((*pplnStorage).*fn)( *mtbc_ptr, cs_iter->reflection.layout.specConstants ); // TODO

            auto [src_it, src_inserted] = _shaderSrcMap.emplace( RVRef(key), &(*cs_iter) );
            CHECK( src_inserted );

            return src_it->second;
        }};

        if ( target == ECompilationTarget::Metal_iOS )
        {
            outShader = AddMetalBytecode( &PipelineStorage::AddMsliOSShader );
        }
        else
        if ( target == ECompilationTarget::Metal_Mac )
        {
            outShader = AddMetalBytecode( &PipelineStorage::AddMslMacShader );
        }
        else
        {
            CHECK_THROW_MSG( false, "unsupported compilation target" );
        }
    }

} // AE::PipelineCompiler
