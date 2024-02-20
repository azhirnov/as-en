// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "SpirvCompiler.h"
#include "PrivateDefines.h"

// glslang includes
#ifdef AE_COMPILER_MSVC
#   pragma warning (push, 0)
#   pragma warning (disable: 4005)
#   pragma warning (disable: 4668)
#endif
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#ifdef AE_COMPILER_GCC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wundef"
#   pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif

#include "glslang/build_info.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/MachineIndependent/localintermediate.h"
#include "glslang/Include/intermediate.h"
#include "glslang/SPIRV/doc.h"
#include "glslang/SPIRV/disassemble.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/SPIRV/GLSL.std.450.h"

// SPIRV-Tools includes
#ifdef ENABLE_OPT
#   include "spirv-tools/optimizer.hpp"
#   include "spirv-tools/libspirv.h"
#else
#   pragma message("SPIRV-Tolls library is missing, SPIRV optimization will be disabled")
#endif

#ifdef AE_ENABLE_GLSL_TRACE
#   include "ShaderTrace.h"
#else
#   include "Packer/ShaderTraceDummy.h"
#   pragma message("GLSL-Trace library is missing, shader debugging and profiling will be disabled")
#endif

#if GLSLANG_VERSION_MAJOR != 14 or GLSLANG_VERSION_MINOR != 0 or GLSLANG_VERSION_PATCH != 0
#   error invalid glslang version
#endif

#ifdef AE_ENABLE_SPIRV_CROSS
#   include "spirv_cross/spirv_cross.hpp"
#   include "spirv_cross/spirv_glsl.hpp"
#endif

#ifdef AE_COMPILER_MSVC
#   pragma warning (pop)
#endif
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic pop
#endif
#ifdef AE_COMPILER_GCC
#   pragma GCC diagnostic pop
#endif

#include "ScriptObjects/ObjectStorage.h"


template <>
struct std::hash< AE::PipelineCompiler::SpirvCompiler::ShaderReflection::DescriptorSet > final :
    AE::Base::DefaultHasher_CalcHash< AE::PipelineCompiler::SpirvCompiler::ShaderReflection::DescriptorSet >
{};


namespace AE::PipelineCompiler
{

    //
    // Shader Includer
    //
    class SpirvCompiler::ShaderIncluder final : public glslang::TShader::Includer
    {
    // types
    private:
        struct IncludeResultImpl final : IncludeResult
        {
            const String    _data;

            IncludeResultImpl (String &&data, const String& headerName, void* userData = null) :
                IncludeResult{headerName, null, 0, userData}, _data{RVRef(data)}
            {
                const_cast<const char*&>(headerData) = _data.c_str();
                const_cast<usize&>(headerLength)    = _data.length();
            }

            ND_ StringView  GetSource () const  { return _data; }
        };

        using IncludeResultPtr_t    = Unique< IncludeResultImpl >;
        using IncludeResults_t      = Array< IncludeResultPtr_t >;
        using IncludedFiles_t       = HashMap< Path, Ptr<IncludeResultImpl>, PathHasher >;


    // variables
    private:
        IncludeResults_t        _results;
        IncludedFiles_t         _includedFiles;
        IncludeDirsRef_t        _directories;
        IShaderPreprocessor*    _preprocessor   = null;
        EShader                 _shaderType     = Default;


    // methods
    public:
        explicit ShaderIncluder (IncludeDirsRef_t dirs, IShaderPreprocessor* pp, EShader shaderType) :
            _directories{dirs}, _preprocessor{pp}, _shaderType{shaderType} {}

        ~ShaderIncluder () override {}

        ND_ IncludedFiles_t const&  GetIncludedFiles () const   { return _includedFiles; }

        // TShader::Includer //
        IncludeResult*  includeSystem (const char* headerName, const char* includerName, usize inclusionDepth) override;
        IncludeResult*  includeLocal (const char* headerName, const char* includerName, usize inclusionDepth) override;

        void  releaseInclude (IncludeResult *) override {}
    };


/*
=================================================
    includeSystem
=================================================
*/
    SpirvCompiler::ShaderIncluder::IncludeResult*
        SpirvCompiler::ShaderIncluder::includeSystem (const char*, const char *, usize)
    {
        return null;
    }

/*
=================================================
    includeLocal
=================================================
*/
    SpirvCompiler::ShaderIncluder::IncludeResult*
        SpirvCompiler::ShaderIncluder::includeLocal (const char* headerName, const char *, usize)
    {
        ASSERT( _directories.size() );

        for (auto& folder : _directories)
        {
            Path    path = Path( folder ) / headerName;

            if ( not FileSystem::IsFile( path ))
                continue;

            const Path  filename = FileSystem::ToAbsolute( path.make_preferred() );

            // prevent recursive include
            if ( _includedFiles.count( filename ))
                return _results.emplace_back(new IncludeResultImpl{ "// skip header\n", headerName }).get();

            String      data;
            const auto  ansi_path = ToString( filename );
            {
                FileRStream     file{ filename };

                CHECK_ERR_MSG( file.IsOpen(),
                    "Failed to open shader '"s << headerName << "', full path '" << ansi_path << "'" );

                CHECK_ERR_MSG( file.Read( usize(file.Size()), OUT data ),
                    "Failed to read shader '"s << headerName << "', full path '" << ansi_path << "'" );
            }

            if ( _preprocessor != null )
            {
                String  tmp;  std::swap( tmp, data );
                CHECK_ERR( _preprocessor->Process( _shaderType, PathAndLine{RVRef(path)}, 0, tmp, OUT data ));
            }

            auto*   result = _results.emplace_back(new IncludeResultImpl{ RVRef(data), headerName }).get();

            _includedFiles.insert_or_assign( filename, result );
            return result;
        }

        return null;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    operator ==
=================================================
*/
    bool  SpirvCompiler::ShaderReflection::DescriptorSet::operator == (const DescriptorSet &rhs) C_NE___
    {
        return  bindingIndex    == rhs.bindingIndex     and
                name            == rhs.name             and
                layout          == rhs.layout;
    }

/*
=================================================
    CalcHash
=================================================
*/
    HashVal  SpirvCompiler::ShaderReflection::DescriptorSet::CalcHash () C_NE___
    {
        return HashOf(bindingIndex) + HashOf(name) + HashOf(layout);
    }
//-----------------------------------------------------------------------------



/*
=================================================
    operator ==
=================================================
*/
    bool  SpirvCompiler::ShaderReflection::operator == (const ShaderReflection &rhs) C_NE___
    {
        return  layout.descrSets                == rhs.layout.descrSets                 and
                layout.pushConstants            == rhs.layout.pushConstants             and
                layout.specConstants            == rhs.layout.specConstants             and

                vertex.supportedTopology        == rhs.vertex.supportedTopology         and
                vertex.vertexAttribs            == rhs.vertex.vertexAttribs             and

                tessellation.patchControlPoints == rhs.tessellation.patchControlPoints  and

                fragment.fragmentIO             == rhs.fragment.fragmentIO              and
                fragment.earlyFragmentTests     == rhs.fragment.earlyFragmentTests      and

                All( compute.localGroupSize     == rhs.compute.localGroupSize )         and
                All( compute.localGroupSpec     == rhs.compute.localGroupSpec )         and

                All( mesh.taskGroupSize         == rhs.mesh.taskGroupSize )             and
                All( mesh.taskGroupSpec         == rhs.mesh.taskGroupSpec )             and
                All( mesh.meshGroupSize         == rhs.mesh.meshGroupSize )             and
                All( mesh.meshGroupSpec         == rhs.mesh.meshGroupSpec )             and
                mesh.topology                   == rhs.mesh.topology                    and
                mesh.maxPrimitives              == rhs.mesh.maxPrimitives               and
                mesh.maxIndices                 == rhs.mesh.maxIndices                  and
                mesh.maxVertices                == rhs.mesh.maxVertices;

    }

/*
=================================================
    CalcHash
=================================================
*/
    HashVal  SpirvCompiler::ShaderReflection::CalcHash () C_NE___
    {
        return  HashOf( layout.descrSets )                  +
                HashOf( layout.pushConstants )              +
                HashOf( layout.specConstants )              +

                HashOf( vertex.supportedTopology )          +
                HashOf( vertex.vertexAttribs )              +

                HashOf( tessellation.patchControlPoints )   +

                HashOf( fragment.fragmentIO )               +
                HashOf( fragment.earlyFragmentTests )       +

                HashOf( compute.localGroupSize )            +
                HashOf( compute.localGroupSpec )            +

                HashOf( mesh.taskGroupSize )                +
                HashOf( mesh.taskGroupSpec )                +
                HashOf( mesh.meshGroupSize )                +
                HashOf( mesh.meshGroupSpec )                +
                HashOf( mesh.topology )                     +
                HashOf( mesh.maxPrimitives )                +
                HashOf( mesh.maxIndices )                   +
                HashOf( mesh.maxVertices );
    }
//-----------------------------------------------------------------------------



    //
    // GLSLang Result
    //
    struct SpirvCompiler::GLSLangResult
    {
        glslang::TProgram               prog;
        Unique< glslang::TShader >      shader;
    };
//-----------------------------------------------------------------------------


/*
=================================================
    Output
=================================================
*/
    SpirvCompiler::Output::Output () {}
    SpirvCompiler::Output::~Output () {}
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    SpirvCompiler::SpirvCompiler (ArrayView<Path> includeDirs) __NE___
    {
        _directories.reserve( includeDirs.size() );

        for (auto& path : includeDirs)
        {
            CHECK( path.is_absolute() );
            _directories.push_back( path.string() );
        }

        glslang::InitializeProcess();

        _GenerateResources( OUT _builtinResource );
    }

/*
=================================================
    destructor
=================================================
*/
    SpirvCompiler::~SpirvCompiler ()
    {
        glslang::FinalizeProcess();
    }

/*
=================================================
    SetPreprocessor
=================================================
*/
    void  SpirvCompiler::SetPreprocessor (IShaderPreprocessor* value)
    {
        _preprocessor.reset( value );
    }

/*
=================================================
    BuildReflection
=================================================
*/
    bool  SpirvCompiler::BuildReflection (const Input &in, OUT ShaderReflection &outReflection, OUT String &log)
    {
        CHECK_ERR( in.shaderType != Default );

        log.clear();

        _currentStage   = EShaderStages_FromShader( in.shaderType );
        _currentShader  = in.shaderType;
        _intermediate   = null;

        ShaderIncluder  includer    { _directories, _preprocessor.get(), in.shaderType };
        GLSLangResult   glslang_data;

        if ( not _ParseGLSL( in, INOUT includer, OUT glslang_data, INOUT log ))
            return false;

        SpirvBytecode_t     spirv;
        CHECK_ERR( _CompileSPIRV( glslang_data, EShaderOpt::Unknown, OUT spirv, INOUT log ));

        CHECK_ERR( _BuildReflection( glslang_data, OUT outReflection ));
        return true;
    }

/*
=================================================
    Compile
=================================================
*/
    bool  SpirvCompiler::Compile (const Input &in, OUT Output &out)
    {
        CHECK_ERR( in.shaderType != Default );

        _currentStage   = EShaderStages_FromShader( in.shaderType );
        _currentShader  = in.shaderType;
        _intermediate   = null;

        const EShaderOpt    dbg_mode    = in.options & EShaderOpt::_ShaderTrace_Mask;
        ShaderIncluder      includer    { _directories, _preprocessor.get(), in.shaderType };
        GLSLangResult       glslang_data;

        if ( not _ParseGLSL( in, INOUT includer, OUT glslang_data, INOUT out.log ))
            return false;

    #ifdef AE_ENABLE_GLSL_TRACE
        if ( dbg_mode != Default )
        {
            out.trace.reset( new ShaderTrace{} );
            auto&   trace   = *out.trace.get();
            auto&   interm  = *glslang_data.prog.getIntermediate( glslang_data.shader->getStage() );

            trace.AddSource( in.header );
            trace.AddSource( in.fileLoc.path, in.fileLoc.line, in.source );

            for (auto& [path, info] : includer.GetIncludedFiles()) {
                trace.IncludeSource( info->headerName, path, info->GetSource() );
            }

            COMP_CHECK_LOG( BitCount( dbg_mode ) == 1, out.log, "only one debug mode is supported" );
            COMP_CHECK_LOG( in.dbgDescSetIdx != UMax, out.log, "debug descriptor set index is not defined" );

            switch ( dbg_mode )
            {
                case EShaderOpt::Trace :
                    COMP_CHECK_LOG( trace.InsertTraceRecording( interm, in.dbgDescSetIdx ), out.log );
                    break;

                case EShaderOpt::FnProfiling :
                    COMP_CHECK_LOG( trace.InsertFunctionProfiler( interm, in.dbgDescSetIdx, in.shaderSubgroupClock, in.shaderDeviceClock ), out.log );
                    break;

                case EShaderOpt::TimeHeatMap :
                    COMP_CHECK_LOG( trace.InsertShaderClockHeatmap( interm, in.dbgDescSetIdx ), out.log );
                    break;

                default :
                    AE_LOGI( "unsupported shader debug mode: 0x" + ToString<16>( dbg_mode ));
                    break;
            }
        }
    #else
        COMP_CHECK_LOG( dbg_mode == Default, out.log, "debug mode is not supported without GLSLTrace library" );
    #endif

        COMP_CHECK_LOG( _CompileSPIRV( glslang_data, in.options, OUT out.spirv, INOUT out.log ), out.log );

        COMP_CHECK_LOG( _BuildReflection( glslang_data, OUT out.reflection ), out.log );
        return true;
    }

/*
=================================================
    ConvertShaderType
=================================================
*/
    ND_ static EShLanguage  ConvertShaderType (EShader shaderType)
    {
        switch_enum( shaderType )
        {
            case EShader::Vertex :          return EShLangVertex;
            case EShader::TessControl :     return EShLangTessControl;
            case EShader::TessEvaluation :  return EShLangTessEvaluation;
            case EShader::Geometry :        return EShLangGeometry;
            case EShader::Fragment :        return EShLangFragment;
            case EShader::Compute :         return EShLangCompute;
            case EShader::MeshTask :        return EShLangTask;
            case EShader::Mesh :            return EShLangMesh;
            case EShader::RayGen :          return EShLangRayGen;
            case EShader::RayAnyHit :       return EShLangAnyHit;
            case EShader::RayClosestHit :   return EShLangClosestHit;
            case EShader::RayMiss :         return EShLangMiss;
            case EShader::RayIntersection:  return EShLangIntersect;
            case EShader::RayCallable :     return EShLangCallable;
            case EShader::Tile :
            case EShader::Unknown :
            case EShader::_Count :          break;
        }
        switch_end
        RETURN_ERR( "unknown shader type", EShLangCount );
    }

/*
=================================================
    _ParseGLSL
=================================================
*/
    bool SpirvCompiler::_ParseGLSL (const Input &in, INOUT ShaderIncluder &includer, OUT GLSLangResult &glslangData, OUT String &log)
    {
        using namespace glslang;

        CHECK_ERR( not in.source.empty() );
        CHECK_ERR( IsNullTerminated( in.source ));
        CHECK_ERR( not in.entry.empty() );
        CHECK_ERR( IsNullTerminated( in.entry ));

        String      tmp_source;
        StringView  source      = in.source;

        if ( _preprocessor )
        {
            COMP_CHECK_LOG( _preprocessor->Process( in.shaderType, in.fileLoc, 0, source, OUT tmp_source ), log );
            source = tmp_source;
        }

        const EShClient                 client          = EShClientVulkan;
              EshTargetClientVersion    client_version  = EShTargetVulkan_1_0;

        const EShTargetLanguage         target          = EShTargetSpv;
              EShTargetLanguageVersion  target_version  = EShTargetSpv_1_2;

              int                       vk_version      = 100;
        const int                       sh_version      = 460;
        const EProfile                  sh_profile      = ECoreProfile;
        const EShSource                 sh_source       = EShSourceGlsl;

        switch ( in.spirvVersion.To100() )
        {
            case 100 :
                _spirvTraget    = SPV_ENV_VULKAN_1_0;
                target_version  = EShTargetSpv_1_0;
                vk_version      = 100;
                client_version  = EShTargetVulkan_1_0;
                break;
            case 110 :
                _spirvTraget    = SPV_ENV_VULKAN_1_0;
                target_version  = EShTargetSpv_1_1;
                vk_version      = 100;
                client_version  = EShTargetVulkan_1_0;
                break;
            case 120 :
                _spirvTraget    = SPV_ENV_VULKAN_1_0;
                target_version  = EShTargetSpv_1_2;
                vk_version      = 100;
                client_version  = EShTargetVulkan_1_0;
                break;
            case 130 :
                _spirvTraget    = SPV_ENV_VULKAN_1_1;
                target_version  = EShTargetSpv_1_3;
                vk_version      = 110;
                client_version  = EShTargetVulkan_1_1;
                break;
            case 140 :
                _spirvTraget    = SPV_ENV_VULKAN_1_1_SPIRV_1_4;
                target_version  = EShTargetSpv_1_4;
                vk_version      = 110;
                client_version  = EShTargetVulkan_1_1;
                break;
            case 150 :
                _spirvTraget    = SPV_ENV_VULKAN_1_2;
                target_version  = EShTargetSpv_1_5;
                vk_version      = 120;
                client_version  = EShTargetVulkan_1_2;
                break;
            case 160 :
                _spirvTraget    = SPV_ENV_VULKAN_1_3;
                target_version  = EShTargetSpv_1_6;
                vk_version      = 130;
                client_version  = EShTargetVulkan_1_3;
                break;
            default :
                RETURN_ERR( "unsupported SPIRV version" );
        }

        EShMessages     messages    = EShMsgDefault;
        EShLanguage     stage       = ConvertShaderType( in.shaderType );
        auto&           shader      = glslangData.shader;
        const char *    sources[]   = { in.header.data(), source.data() };
        const int       src_lens[]  = { int(in.header.size()), int(source.size()) };

        shader.reset( new TShader( stage ));
        shader->setStringsWithLengths( sources, src_lens, int(CountOf( sources )) );
        shader->setEntryPoint( in.entry.data() );
        shader->setSourceEntryPoint( in.entry.data() );
        shader->setEnvInput( sh_source, stage, client, vk_version );
        shader->setEnvClient( client, client_version );
        shader->setEnvTarget( target, target_version );

        shader->setAutoMapLocations( false );
        shader->setAutoMapBindings( false );

        if ( not shader->parse( &_builtinResource, sh_version, sh_profile, false, true, messages, includer ))
        {
            log += shader->getInfoLog();
            _OnCompilationFailed( in, includer, INOUT log );
            return false;
        }

        glslangData.prog.addShader( shader.get() );

        if ( not glslangData.prog.link( messages ))
        {
            log += glslangData.prog.getInfoLog();
            _OnCompilationFailed( in, includer, INOUT log );
            return false;
        }

        if ( not glslangData.prog.mapIO() )     // TODO: use resolver
        {
            log += "mapIO - failed";
            return false;
        }

        return true;
    }

#ifdef ENABLE_OPT
/*
=================================================
    _DisassembleSPIRV
=================================================
*/
    bool  SpirvCompiler::_DisassembleSPIRV (const SpirvBytecode_t &spirv, OUT String &outDisasm) const
    {
        outDisasm.clear();

        spv_target_env  target_env = BitCast<spv_target_env>( _spirvTraget );

        spv_context     ctx = ::spvContextCreate( target_env );
        CHECK_ERR( ctx != null );

        spv_text        text        = null;
        spv_diagnostic  diagnostic  = null;
        bool            result      = false;

        if ( ::spvBinaryToText( ctx, spirv.data(), spirv.size(), 0, &text, &diagnostic ) == SPV_SUCCESS )
        {
            outDisasm   = String{ text->str, text->length };
            result      = true;
        }

        ::spvTextDestroy( text );
        ::spvDiagnosticDestroy( diagnostic );
        ::spvContextDestroy( ctx );

        return result;
    }

/*
=================================================
    _OptimizeSPIRV
=================================================
*/
    bool  SpirvCompiler::_OptimizeSPIRV (INOUT SpirvBytecode_t &spirv, INOUT String &log) const
    {
        spv_target_env  target_env = BitCast<spv_target_env>( _spirvTraget );

        spvtools::Optimizer optimizer{ target_env };
        optimizer.SetMessageConsumer(
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

        optimizer.RegisterLegalizationPasses();
        optimizer.RegisterSizePasses();
        optimizer.RegisterPerformancePasses();

        optimizer.RegisterPass( spvtools::CreateCompactIdsPass() );
        optimizer.RegisterPass( spvtools::CreateAggressiveDCEPass() );
        optimizer.RegisterPass( spvtools::CreateRemoveDuplicatesPass() );
        optimizer.RegisterPass( spvtools::CreateCFGCleanupPass() );

        spvtools::OptimizerOptions spvOptOptions;
        spvOptOptions.set_run_validator( false );

        optimizer.Run( spirv.data(), spirv.size(), &spirv, spvOptOptions );
        return true;
    }

/*
=================================================
    _ValidateSPIRV
=================================================
*/
    bool  SpirvCompiler::_ValidateSPIRV (const SpirvBytecode_t &spirv, INOUT String &log) const
    {
        spv_target_env  target_env = BitCast<spv_target_env>( _spirvTraget );

        spvtools::ValidatorOptions  options;

        spvtools::SpirvTools        tools{ target_env };
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
#endif  // ENABLE_OPT

/*
=================================================
    _CompileSPIRV
=================================================
*/
    bool  SpirvCompiler::_CompileSPIRV (const GLSLangResult &glslangData, EShaderOpt options, OUT SpirvBytecode_t &spirv, OUT String &log) const
    {
        using namespace glslang;

        const TIntermediate* intermediate = glslangData.prog.getIntermediate( glslangData.shader->getStage() );
        COMP_CHECK_ERR( intermediate );

        SpvOptions              spv_options;
        spv::SpvBuildLogger     logger;

        spv_options.generateDebugInfo               =     AllBits( options, EShaderOpt::DebugInfo );
        spv_options.stripDebugInfo                  = false;
        spv_options.disableOptimizer                = not AllBits( options, EShaderOpt::Optimize );
        spv_options.optimizeSize                    =     AllBits( options, EShaderOpt::OptimizeSize );
        spv_options.disassemble                     = false;
        spv_options.validate                        = false;
        spv_options.emitNonSemanticShaderDebugInfo  = spv_options.generateDebugInfo;
        spv_options.emitNonSemanticShaderDebugSource= spv_options.generateDebugInfo;

        GlslangToSpv( *intermediate, OUT spirv, &logger, &spv_options );
        log += logger.getAllMessages();

        if ( spirv.empty() )
            return false;

        if ( not _ValidateSPIRV( spirv, INOUT log ))
        {
            #ifdef AE_DEBUG
            if ( AnyBits( options, EShaderOpt::_ShaderTrace_Mask ))
            {
                #ifdef ENABLE_OPT
                {
                    String  disasm;
                    if ( _DisassembleSPIRV( spirv, OUT disasm ))
                        AE_LOGI( disasm );
                }
                #endif

                #ifdef AE_ENABLE_SPIRV_CROSS
                try{
                    spirv_cross::CompilerGLSL           compiler {spirv.data(), spirv.size()};
                    spirv_cross::CompilerGLSL::Options  opt = {};

                    opt.version                     = 460;
                    opt.es                          = false;
                    opt.vulkan_semantics            = true;
                    opt.separate_shader_objects     = true;
                    opt.enable_420pack_extension    = true;

                    opt.vertex.fixup_clipspace      = false;
                    opt.vertex.flip_vert_y          = false;
                    opt.vertex.support_nonzero_base_instance = true;

                    opt.fragment.default_float_precision    = spirv_cross::CompilerGLSL::Options::Precision::Highp;
                    opt.fragment.default_int_precision      = spirv_cross::CompilerGLSL::Options::Precision::Highp;

                    compiler.set_common_options( opt );

                    String  glsl_src = compiler.compile();  // throw
                    AE_LOGI( glsl_src );
                }
                catch (...)
                {}
                #endif
            }
            #endif
            return false;
        }

        #ifdef ENABLE_OPT
            if ( AllBits( options, EShaderOpt::StrongOptimization ))
                CHECK_ERR( _OptimizeSPIRV( INOUT spirv, OUT log ));
        #endif

        return true;
    }

/*
=================================================
    ParseGLSLError
=================================================
*/
    struct GLSLErrorInfo
    {
        StringView  description;
        StringView  fileName;
        uint        sourceIndex;
        usize       line;
        bool        isError;

        GLSLErrorInfo () : sourceIndex{0}, line{UMax}, isError{false} {}
    };

    static bool  ParseGLSLError (StringView line, OUT GLSLErrorInfo &info)
    {
        const StringView    c_error     = "error";
        const StringView    c_warning   = "warning";

        usize               pos = 0;

        const auto          ReadToken   = [&line, &pos] (OUT bool &isNumber)
        {{
                        isNumber= true;
            const usize start   = pos;

            for (; pos < line.length() and line[pos] != ':'; ++pos) {
                isNumber &= (line[pos] >= '0' and line[pos] <= '9');
            }
            return line.substr( start, pos - start );
        }};

        const auto          SkipSeparator = [&line, &pos] ()
        {{
            if ( pos+1 < line.length() and line[pos] == ':' and line[pos+1] == ' ' )
                pos += 2;
            else
            if ( pos < line.length() and line[pos] == ':' )
                pos += 1;
            else
                return false;

            return true;
        }};
        //---------------------------------------------------------------------------


        // parse error/warning/...
        if ( StartsWithIC( line, c_error ))
        {
            pos         += c_error.length();
            info.isError = true;
        }
        else
        if ( StartsWithIC( line, c_warning ))
        {
            pos         += c_warning.length();
            info.isError = false;
        }
        else
            return false;

        if ( not SkipSeparator() )
            return false;


        // parse source index or header name
        {
            bool                is_number;
            const StringView    src     = ReadToken( OUT is_number );

            if ( not SkipSeparator() )
                return false;

            if ( not is_number )
                info.fileName = src;
            else
                info.sourceIndex = StringToUInt( String{src} );
        }


        // parse line number
        {
            bool                is_number;
            const StringView    src     = ReadToken( OUT is_number );

            if ( not SkipSeparator() or not is_number )
                return false;

            info.line = StringToUInt( String{src} );
        }

        info.description = line.substr( pos );
        return true;
    }

/*
=================================================
    _OnCompilationFailed
=================================================
*/
    void  SpirvCompiler::_OnCompilationFailed (const Input &in, const ShaderIncluder &includer, INOUT String &log) const
    {
        // glslang errors format:
        // pattern: <error/warning>: <number>:<line>: <description>
        // pattern: <error/warning>: <file>:<line>: <description>

        usize               prev_line   = UMax;
        usize               pos         = 0;
        const uint          src_count   = 2;
        const usize         num_lines[] = { Parser::CalculateNumberOfLines( in.header ), Parser::CalculateNumberOfLines( in.source )};
        const StringView    sources[]   = { in.header, in.source };
        const String        path_str    = ToString( in.fileLoc.path );
        String              str;        str.reserve( log.length() );

        StaticAssert( CountOf(num_lines) == src_count );
        StaticAssert( CountOf(sources) == src_count );

        for (;;)
        {
            if_unlikely( pos >= log.size() )
                break;

            StringView  line;
            Parser::ReadLineToEnd( log, INOUT pos, OUT line );

            if_unlikely( line.empty() )
                continue;

            GLSLErrorInfo   error_info;
            bool            added = false;

            if ( ParseGLSLError( line, OUT error_info ))
            {
                // unite error in same source lines
                if ( prev_line == error_info.line )
                {
                    str << line << "\n";
                    continue;
                }

                prev_line = error_info.line;

                if ( error_info.fileName.empty() )
                {
                    // search in sources
                    StringView  cur_source  = error_info.sourceIndex < src_count ? sources  [error_info.sourceIndex] : "";
                    usize       lines_count = error_info.sourceIndex < src_count ? num_lines[error_info.sourceIndex] : 0;
                    CHECK( error_info.line <= lines_count );

                    usize   line_pos = 0;
                    CHECK( Parser::MoveToLine( cur_source, INOUT line_pos, error_info.line-1 ));

                    StringView  line_str;
                    Parser::ReadLineToEnd( cur_source, INOUT line_pos, OUT line_str );

                    if ( not path_str.empty() and error_info.line > in.fileLoc.line )
                        str << path_str << '(' << ToString(error_info.line - in.fileLoc.line) << "):\n";
                    else
                        str << "in source (" << ToString(error_info.sourceIndex) << ": " << ToString(error_info.line) << "):\n";

                    str << "\"" << line_str << "\"\n" << line << "\n";
                    added = true;
                }
                else
                {
                    for (auto& [path, src_ptr] : includer.GetIncludedFiles())
                    {
                        if ( HasSubString( path.string(), error_info.fileName ))
                        {
                            StringView      src         = src_ptr->GetSource();
                            const usize     lines_count = Parser::CalculateNumberOfLines( src ) + 1;
                            const usize     local_line  = error_info.line;
                            usize           line_pos    = 0;
                            StringView      line_str;

                            CHECK( local_line < lines_count );
                            CHECK( Parser::MoveToLine( src, INOUT line_pos, local_line-1 ));

                            Parser::ReadLineToEnd( src, INOUT line_pos, OUT line_str );

                            str << path.string() << '(' << ToString(local_line) << "):\n\"" << line_str << "\"\n" << line << "\n";
                            added = true;
                            break;
                        }
                    }
                }
            }

            if ( not added )
            {
                str << DEBUG_ONLY( "<unknown> " << ) line << "\n";
            }
        }

        std::swap( log, str );
    }

/*
=================================================
    _BuildReflection
=================================================
*/
    bool  SpirvCompiler::_BuildReflection (const GLSLangResult &glslangData, OUT ShaderReflection &result)
    {
        _intermediate = glslangData.prog.getIntermediate( glslangData.shader->getStage() );
        COMP_CHECK_ERR( _intermediate );

        // deserialize shader
        TIntermNode*    root = _intermediate->getTreeRoot();

        COMP_CHECK_ERR( _ProcessExternalObjects( null, root, OUT result ));
        COMP_CHECK_ERR( _ProcessShaderInfo( INOUT result ));

        for (auto& ds : result.layout.descrSets)
        {
            ds.layout.SortUniforms();
        }

        _intermediate = null;
        return true;
    }

/*
=================================================
    _ProcessExternalObjects
=================================================
*/
    bool SpirvCompiler::_ProcessExternalObjects (TIntermNode*, TIntermNode* node, INOUT ShaderReflection &result) const
    {
        using namespace glslang;

        TIntermAggregate* aggr = node->getAsAggregate();

        if ( aggr == null )
            return true;

        switch ( aggr->getOp() )
        {
            // continue deserializing
            case TOperator::EOpSequence :
            {
                for (auto& seq : aggr->getSequence())
                {
                    COMP_CHECK_ERR( _ProcessExternalObjects( aggr, seq, INOUT result ));
                }
                return true;
            }

            // uniforms, buffers, ...
            case TOperator::EOpLinkerObjects :
            {
                for (auto& seq : aggr->getSequence())
                {
                    COMP_CHECK_ERR( _DeserializeExternalObjects( seq, INOUT result ));
                }
                return true;
            }
        }
        return true;
    }

/*
=================================================
    GetArraySize
=================================================
*/
    ND_ static uint  GetArraySize (const glslang::TType &type)
    {
        auto*   sizes = type.getArraySizes();

        if ( not sizes or sizes->getNumDims() <= 0 )
            return 1;

        CHECK( sizes->getNumDims() == 1 );

        return sizes->getDimSize(0);
    }

/*
=================================================
    _GetDescriptorSet
=================================================
*/
    SpirvCompiler::ShaderReflection::DescriptorSet&  SpirvCompiler::_GetDescriptorSet (uint dsIndex, INOUT SpirvCompiler::ShaderReflection &reflection) const
    {
        reflection.layout.descrSets.resize( Max( dsIndex+1, reflection.layout.descrSets.size() ));

        const String    ds_name = ToString(dsIndex);

        auto&   ds      = reflection.layout.descrSets[dsIndex];
        ds.bindingIndex = dsIndex;
        ds.name         = DescriptorSetName( ds_name );

        ObjectStorage::Instance()->AddName<DescriptorSetName>( ds_name );

        return ds;
    }

/*
=================================================
    _ExtractImageType
=================================================
*/
    EImageType  SpirvCompiler::_ExtractImageType (const glslang::TType &type) const
    {
        using namespace glslang;

        if ( type.getBasicType() != TBasicType::EbtSampler )
            COMP_RETURN_ERR( "type is not image/sampler type!" );

        TSampler const& samp    = type.getSampler();
        EImageType      result  = Default;

        switch_enum( samp.getBasicType() )
        {
            case TBasicType::EbtFloat :
            case TBasicType::EbtDouble :
                result |= EImageType::Float;    // TODO: UNorm, SNorm, UFloat, sRGB
                break;

            case TBasicType::EbtFloat16 :
                result |= EImageType::Half;
                break;

            case TBasicType::EbtInt8 :
            case TBasicType::EbtInt16 :
            case TBasicType::EbtInt :
            case TBasicType::EbtInt64 :
                result |= EImageType::Int;
                break;

            case TBasicType::EbtUint8 :
            case TBasicType::EbtUint16 :
            case TBasicType::EbtUint :
            case TBasicType::EbtUint64 :
                result |= EImageType::UInt;
                break;

            case TBasicType::EbtVoid :
            case TBasicType::EbtBool :
            case TBasicType::EbtAtomicUint :
            case TBasicType::EbtSampler :
            case TBasicType::EbtStruct :
            case TBasicType::EbtBlock :
            case TBasicType::EbtAccStruct :
            case TBasicType::EbtRayQuery :
            case TBasicType::EbtReference :
            case TBasicType::EbtString :
            case TBasicType::EbtNumTypes :
            case TBasicType::EbtSpirvType :
            case TBasicType::EbtHitObjectNV :
            case TBasicType::EbtCoopmat :
            default :
                COMP_RETURN_ERR( "unknown basic type!" );
        }
        switch_end

        if ( samp.isShadow() )
        {
            CHECK( not samp.isMultiSample() );
            result |= EImageType::Shadow;

            CHECK( (result & EImageType::_ValMask) == EImageType::Float );
            result &= ~EImageType::Float;
            result |= EImageType::Depth;    // TODO: depth stencil
        }

        switch_enum( samp.dim )
        {
            case TSamplerDim::Esd1D :
            {
                if ( samp.isArrayed() )     result |= EImageType::Img1DArray;
                else                        result |= EImageType::Img1D;
                break;
            }
            case TSamplerDim::Esd2D :
            {
                if ( samp.isMultiSample() and samp.isArrayed() )    result |= EImageType::Img2DMSArray;     else
                if ( samp.isArrayed() )                             result |= EImageType::Img2DArray;       else
                if ( samp.isMultiSample() )                         result |= EImageType::Img2DMS;          else
                                                                    result |= EImageType::Img2D;
                break;
            }
            case TSamplerDim::Esd3D :
            {
                CHECK( not samp.isArrayed() );
                CHECK( not samp.isMultiSample() );
                CHECK( not samp.isShadow() );
                result |= EImageType::Img3D;
                break;
            }
            case TSamplerDim::EsdCube :
            {
                if ( samp.isArrayed() ) result |= EImageType::ImgCubeArray;
                else                    result |= EImageType::ImgCube;
                break;
            }

            case TSamplerDim::EsdSubpass :
            {
                COMP_CHECK_ERR( type.isSubpass() );
                COMP_CHECK_ERR( not samp.isArrayed() );
                if ( samp.isMultiSample() ) result |= EImageType::Img2DMS;  else
                                            result |= EImageType::Img2D;
                break;
            }

            case TSamplerDim::EsdBuffer :
            case TSamplerDim::EsdNone :
            case TSamplerDim::EsdRect :
            case TSamplerDim::EsdNumDims :
            case TSamplerDim::EsdAttachmentEXT :
            default :
                COMP_RETURN_ERR( "unknown sampler dimension type!" );
        }
        switch_end

        return result;
    }

/*
=================================================
    _ExtractImageFormat
=================================================
*/
    EPixelFormat  SpirvCompiler::_ExtractImageFormat (uint format) const
    {
        using namespace glslang;
        switch_enum( BitCast<TLayoutFormat>(format) )
        {
            case TLayoutFormat::ElfNone :           return EPixelFormat::Unknown;
            case TLayoutFormat::ElfRgba32f :        return EPixelFormat::RGBA32F;
            case TLayoutFormat::ElfRgba16f :        return EPixelFormat::RGBA16F;
            case TLayoutFormat::ElfR32f :           return EPixelFormat::R32F;
            case TLayoutFormat::ElfRgba8 :          return EPixelFormat::RGBA8_UNorm;
            case TLayoutFormat::ElfRgba8Snorm :     return EPixelFormat::RGBA8_SNorm;
            case TLayoutFormat::ElfRg32f :          return EPixelFormat::RG32F;
            case TLayoutFormat::ElfRg16f :          return EPixelFormat::RG16F;
            case TLayoutFormat::ElfR11fG11fB10f :   return EPixelFormat::RGB_11_11_10F;
            case TLayoutFormat::ElfR16f :           return EPixelFormat::R16F;
            case TLayoutFormat::ElfRgba16 :         return EPixelFormat::RGBA16_UNorm;
            case TLayoutFormat::ElfRgb10A2 :        return EPixelFormat::RGB10_A2_UNorm;
            case TLayoutFormat::ElfRg16 :           return EPixelFormat::RG16_UNorm;
            case TLayoutFormat::ElfRg8 :            return EPixelFormat::RG8_UNorm;
            case TLayoutFormat::ElfR16 :            return EPixelFormat::R16_UNorm;
            case TLayoutFormat::ElfR8 :             return EPixelFormat::R8_UNorm;
            case TLayoutFormat::ElfRgba16Snorm :    return EPixelFormat::RGBA16_SNorm;
            case TLayoutFormat::ElfRg16Snorm :      return EPixelFormat::RG16_SNorm;
            case TLayoutFormat::ElfRg8Snorm :       return EPixelFormat::RG8_SNorm;
            case TLayoutFormat::ElfR16Snorm :       return EPixelFormat::R16_SNorm;
            case TLayoutFormat::ElfR8Snorm :        return EPixelFormat::R8_SNorm;
            case TLayoutFormat::ElfRgba32i :        return EPixelFormat::RGBA32I;
            case TLayoutFormat::ElfRgba16i :        return EPixelFormat::RGBA16I;
            case TLayoutFormat::ElfRgba8i :         return EPixelFormat::RGBA8I;
            case TLayoutFormat::ElfR32i :           return EPixelFormat::R32I;
            case TLayoutFormat::ElfRg32i :          return EPixelFormat::RG32I;
            case TLayoutFormat::ElfRg16i :          return EPixelFormat::RG16I;
            case TLayoutFormat::ElfRg8i :           return EPixelFormat::RG8I;
            case TLayoutFormat::ElfR16i :           return EPixelFormat::R16I;
            case TLayoutFormat::ElfR8i :            return EPixelFormat::R8I;
            case TLayoutFormat::ElfRgba32ui :       return EPixelFormat::RGBA32U;
            case TLayoutFormat::ElfRgba16ui :       return EPixelFormat::RGBA16U;
            case TLayoutFormat::ElfRgba8ui :        return EPixelFormat::RGBA8U;
            case TLayoutFormat::ElfR32ui :          return EPixelFormat::R32U;
            case TLayoutFormat::ElfRg32ui :         return EPixelFormat::RG32U;
            case TLayoutFormat::ElfRg16ui :         return EPixelFormat::RG16U;
            case TLayoutFormat::ElfRgb10a2ui :      return EPixelFormat::RGB10_A2U;
            case TLayoutFormat::ElfRg8ui :          return EPixelFormat::RG8U;
            case TLayoutFormat::ElfR16ui :          return EPixelFormat::R16U;
            case TLayoutFormat::ElfR8ui :           return EPixelFormat::R8U;
            case TLayoutFormat::ElfR64i :           return EPixelFormat::R64I;
            case TLayoutFormat::ElfR64ui :          return EPixelFormat::R64U;

            case TLayoutFormat::ElfEsFloatGuard :
            case TLayoutFormat::ElfFloatGuard :
            case TLayoutFormat::ElfEsIntGuard :
            case TLayoutFormat::ElfIntGuard :
            case TLayoutFormat::ElfEsUintGuard :
            case TLayoutFormat::ElfExtSizeGuard :
            case TLayoutFormat::ElfSize1x8 :
            case TLayoutFormat::ElfSize1x16 :
            case TLayoutFormat::ElfSize1x32 :
            case TLayoutFormat::ElfSize2x32 :
            case TLayoutFormat::ElfSize4x32 :
            case TLayoutFormat::ElfCount :          break;
        }
        switch_end
        COMP_RETURN_ERR( "Unsupported image format!" );
    }

/*
=================================================
    ExtractShaderAccessType
=================================================
*/
    ND_ static EResourceState  ExtractShaderAccessType (const glslang::TQualifier &q)
    {
        if ( q.coherent or
             q.volatil  or
             q.restrict )
        {
            return EResourceState::ShaderStorage_RW;
        }

        if ( q.readonly )
            return EResourceState::ShaderStorage_Read;

        if ( q.writeonly )
            return EResourceState::ShaderStorage_Write;

        // default:
        return EResourceState::ShaderStorage_RW;
    }

/*
=================================================
    ExtractNodeName
=================================================
*/
    ND_ static String  ExtractNodeName (TIntermNode* node)
    {
        CHECK_ERR( node and node->getAsSymbolNode() );

        String              name    = node->getAsSymbolNode()->getName().c_str();
        const StringView    prefix  = "anon@";

        if ( not name.compare( 0, prefix.size(), prefix.data() ))
            name.clear();

        //CHECK( not name.empty() );

        return name;
    }

/*
=================================================
    Extract***ID
=================================================
*/
    ND_ static UniformName  ExtractUniformID (TIntermNode* node)
    {
        String  name = ExtractNodeName( node );
        ObjectStorage::Instance()->AddName<UniformName>( name );
        return UniformName{ name };
    }

    ND_ static ShaderIOName  ExtractShaderIOName (TIntermNode* node)
    {
        String  name = ExtractNodeName( node );
        ObjectStorage::Instance()->AddName<ShaderIOName>( name );
        return ShaderIOName{ name };
    }

    ND_ static SpecializationName  ExtractSpecializationID (TIntermNode* node)
    {
        String  name = ExtractNodeName( node );
        ObjectStorage::Instance()->AddName<SpecializationName>( name );
        return SpecializationName{ name };
    }

    ND_ static PushConstantName  ExtractPushConstant (TIntermNode* node)
    {
        String  name = ExtractNodeName( node );
        ObjectStorage::Instance()->AddName<PushConstantName>( name );
        return PushConstantName{ name };
    }

/*
=================================================
    GetShaderStructName
=================================================
*/
    ND_ static ShaderStructName  GetShaderStructName (const glslang::TString &str2)
    {
        StringView  str     { str2.c_str(), str2.length() };
        StringView  prefix  {"AE_Type_"};

        if ( StartsWith( str, prefix ))
            return ShaderStructName{ str.substr( prefix.size() )};
        else
            return ShaderStructName{ str };
    }


/*
=================================================
    _ExtractVertexType
=================================================
*/
    EVertexType  SpirvCompiler::_ExtractVertexType (const glslang::TType &type) const
    {
        using namespace glslang;

        EVertexType     result = EVertexType(0);

        COMP_CHECK_ERR( not type.isArray() );

        switch_enum( type.getBasicType() )
        {
            case TBasicType::EbtFloat :     result |= EVertexType::_Float;  break;
            case TBasicType::EbtDouble :    result |= EVertexType::_Double; break;
            case TBasicType::EbtFloat16 :   result |= EVertexType::_Half;   break;
            case TBasicType::EbtInt8 :      result |= EVertexType::_Byte;   break;
            case TBasicType::EbtUint8 :     result |= EVertexType::_UByte;  break;
            case TBasicType::EbtInt16 :     result |= EVertexType::_Short;  break;
            case TBasicType::EbtUint16 :    result |= EVertexType::_UShort; break;
            case TBasicType::EbtInt :       result |= EVertexType::_Int;    break;
            case TBasicType::EbtUint :      result |= EVertexType::_UInt;   break;
            case TBasicType::EbtInt64 :     result |= EVertexType::_Long;   break;
            case TBasicType::EbtUint64 :    result |= EVertexType::_ULong;  break;
            //case TBasicType::EbtBool :    result |= EVertexType::_Bool;   break;

            case TBasicType::EbtVoid :
            case TBasicType::EbtBool :
            case TBasicType::EbtAtomicUint :
            case TBasicType::EbtSampler :
            case TBasicType::EbtStruct :
            case TBasicType::EbtBlock :
            case TBasicType::EbtAccStruct :
            case TBasicType::EbtRayQuery :
            case TBasicType::EbtString :
            case TBasicType::EbtReference :
            case TBasicType::EbtNumTypes :
            case TBasicType::EbtSpirvType :
            case TBasicType::EbtHitObjectNV :
            case TBasicType::EbtCoopmat :
            default :                       COMP_RETURN_ERR( "unsupported basic type!" );
        }
        switch_end

        if ( type.isScalarOrVec1() )
            return result;

        if ( type.isVector() )
        {
            switch ( type.getVectorSize() )
            {
                case 1 :    break;
                case 2 :    result |= EVertexType::_Vec2;   break;
                case 3 :    result |= EVertexType::_Vec3;   break;
                case 4 :    result |= EVertexType::_Vec4;   break;
                default :   COMP_RETURN_ERR( "unsupported vector size!" );
            }
            return result;
        }

        if ( type.isMatrix() )
        {
            COMP_RETURN_ERR( "not supported, yet" );
        }

        COMP_RETURN_ERR( "unknown vertex type" );
    }

/*
=================================================
    _ExtractShaderIOType
=================================================
*/
    EShaderIO  SpirvCompiler::_ExtractShaderIOType (const glslang::TType &type) const
    {
        using namespace glslang;

        if ( type.isSubpass() )
        {
            StaticAssert( uint(EShaderIO::_Count) == 13 );
            switch( type.getSampler().type )
            {
                case TBasicType::EbtFloat :     return EShaderIO::Float;
                case TBasicType::EbtFloat16 :   return EShaderIO::Half;
                case TBasicType::EbtInt :       return EShaderIO::Int;
                case TBasicType::EbtUint :      return EShaderIO::UInt;
                // TODO: other types ?
            }
            COMP_RETURN_ERR( "unsupported subpass input" );
        }

        COMP_CHECK_ERR( type.getVectorSize() == 4 );

        StaticAssert( uint(EShaderIO::_Count) == 13 );
        switch ( type.getBasicType() )
        {
            case TBasicType::EbtFloat :     return EShaderIO::Float;
            case TBasicType::EbtFloat16 :   return EShaderIO::Half;
            case TBasicType::EbtInt :       return EShaderIO::Int;
            case TBasicType::EbtUint :      return EShaderIO::UInt;
            // TODO: other types ?
        }
        COMP_RETURN_ERR( "unsupported fragment output" );
    }

/*
=================================================
    _CalculateStructSize
----
    based on TParseContext::fixBlockUniformOffsets
=================================================
*/
    bool  SpirvCompiler::_CalculateStructSize (const glslang::TType &bufferType, OUT Bytes32u &staticSize, OUT Bytes32u &arrayStride, OUT Bytes32u &minOffset) const
    {
        using namespace glslang;

        staticSize = arrayStride = Bytes32u{0u};
        minOffset = Bytes32u::Max();

        COMP_CHECK_ERR( bufferType.isStruct() );
        COMP_CHECK_ERR( bufferType.getQualifier().isUniformOrBuffer() or bufferType.getQualifier().layoutPushConstant );
        COMP_CHECK_ERR( bufferType.getQualifier().layoutPacking == ElpStd140 or
                        bufferType.getQualifier().layoutPacking == ElpStd430 );

        int     member_size     = 0;
        int     offset          = 0;
        int     max_align       = 1;
        auto&   struct_fields   = *bufferType.getStruct();

        for (usize member = 0; member < struct_fields.size(); ++member)
        {
            const TType&        member_type         = *struct_fields[member].type;
            const TQualifier&   member_qualifier    = member_type.getQualifier();
            TLayoutMatrix       sub_matrix_layout   = member_qualifier.layoutMatrix;

            int dummy_stride;
            int member_alignment = _intermediate->getBaseAlignment( member_type, OUT member_size, OUT dummy_stride,
                                                                    bufferType.getQualifier().layoutPacking,
                                                                    sub_matrix_layout != ElmNone ? sub_matrix_layout == ElmRowMajor
                                                                        : bufferType.getQualifier().layoutMatrix == ElmRowMajor );

            if ( member_qualifier.hasOffset() )
            {
                COMP_CHECK_ERR( IsMultipleOfPow2( member_qualifier.layoutOffset, member_alignment ));

                if ( _intermediate->getSpv().spv == 0 )
                {
                    COMP_CHECK_ERR( member_qualifier.layoutOffset >= offset );

                    offset = Max( offset, member_qualifier.layoutOffset );
                }
                else {
                    offset = member_qualifier.layoutOffset;
                }
            }

            if ( member_qualifier.hasAlign() )
                member_alignment = Max( member_alignment, member_qualifier.layoutAlign );

            glslang::RoundToPow2( INOUT offset, member_alignment );

            minOffset   = Min( minOffset, uint(offset) );
            max_align   = Max( max_align, member_alignment );

            // for last member
            if ( member+1 == struct_fields.size() and member_type.isUnsizedArray() )
            {
                arrayStride = Bytes32u{uint( dummy_stride )};
            }else{
                offset += member_size;
            }
        }

        glslang::RoundToPow2( INOUT offset, max_align );
        staticSize = Bytes32u{uint( offset )};

        return true;
    }

/*
=================================================
    _DeserializeExternalObjects
=================================================
*/
    bool  SpirvCompiler::_DeserializeExternalObjects (TIntermNode* node, INOUT ShaderReflection &reflection) const
    {
        using namespace glslang;

        TIntermTyped*   tnode       = node->getAsTyped();
        auto const&     type        = tnode->getType();
        auto const&     qual        = type.getQualifier();
        const String    node_name   = ExtractNodeName( node );

        // skip builtin
        if ( type.isBuiltIn() )
            return true;

        // shared variables
        if ( qual.storage == EvqShared )
            return true;

        // shader input
        if ( qual.storage == TStorageQualifier::EvqVaryingIn )
        {
            if ( _currentStage != EShaderStages::Vertex )
                return true;    // skip

            COMP_CHECK_ERR( qual.hasLocation(), "location for '"s << node_name << "' is not defined" );

            VertexAttrib    attrib;
            attrib.index    = CheckCast<ubyte>(qual.layoutLocation);
            attrib.type     = _ExtractVertexType( type );

            //attrib.id     = ExtractVertexID( node );
            //COMP_CHECK_ERR( attrib.id.IsDefined() );

            reflection.vertex.vertexAttribs.push_back( RVRef(attrib) );
            return true;
        }

        // shader output
        if ( qual.storage == TStorageQualifier::EvqVaryingOut )
        {
            if ( _currentStage != EShaderStages::Fragment )
                return true;    // skip

            COMP_CHECK_ERR( qual.hasLocation(), "location for '"s << node_name << "' is not defined" );
            reflection.fragment.fragmentIO.colorAttachments.resize( Max( uint(qual.layoutLocation)+1, reflection.fragment.fragmentIO.colorAttachments.size() ));

            const auto  name = ShaderIOName::Optimized_t{ ExtractShaderIOName( node )};
            COMP_CHECK_ERR( name.IsDefined() );

            COMP_CHECK_ERR( not reflection.fragment.fragmentIO.colorAttachments.at<0>( uint(qual.layoutLocation) ).IsDefined() );
            reflection.fragment.fragmentIO.colorAttachments.set( uint(qual.layoutLocation), name, _ExtractShaderIOType( type ));
            return true;
        }

        // specialization constant
        if ( qual.storage == EvqConst and
             qual.layoutSpecConstantId != TQualifier::layoutSpecConstantIdEnd )
        {
            const auto  name = ExtractSpecializationID( node );
            COMP_CHECK_ERR( name.IsDefined() );

            COMP_CHECK_ERR( reflection.layout.specConstants.emplace( name, qual.layoutSpecConstantId ).second );
            return true;
        }

        // global variable or global constant
        if ( qual.storage == EvqGlobal or qual.storage == EvqConst )
            return true;

        // ray tracing shader types
        if ( AnyEqual( qual.storage, EvqPayload, EvqPayloadIn, EvqHitAttr, EvqCallableData, EvqCallableDataIn ))
            return true;

        if ( qual.layoutShaderRecord )
            return true;

        // task shader out / mesh shader in variables
        if ( qual.storage == EvqtaskPayloadSharedEXT )
            return true;

        // push constants
        if ( qual.layoutPushConstant )
        {
            Bytes32u    size, stride, offset;
            COMP_CHECK_ERR( _CalculateStructSize( type, OUT size, OUT stride, OUT offset ), "failed to calculate struct size for '"s << node_name << "'");
            size -= offset;

            const auto  name = ExtractPushConstant( node );
            COMP_CHECK_ERR( name.IsDefined() );

            const auto  type_name = GetShaderStructName( type.getTypeName() );

            COMP_CHECK_ERR( reflection.layout.pushConstants.items.insert_or_assign(
                                name,
                                PushConstant{ _currentShader, UMax, offset, size, type_name }).second,
                            "push constant for '"s << node_name << "' is already defined" );
            return true;
        }

        COMP_CHECK_ERR( qual.hasSet(), "descriptor set index for '"s << node_name << "' is not defined" );

        auto&   descriptor_set = _GetDescriptorSet( uint(qual.layoutSet), INOUT reflection );

        if ( type.getBasicType() == TBasicType::EbtSampler )
        {
            TSampler const& samp = type.getSampler();

            COMP_CHECK_ERR( not samp.isYuv() );
            COMP_CHECK_ERR( qual.hasBinding(), "binding index for '"s << node_name << "' is not defined" );

            // image
            if ( samp.isImage() )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::StorageImage;

                un.image            = Default;
                un.image.type       = _ExtractImageType( type );
                un.image.format     = _ExtractImageFormat( qual.layoutFormat );
                un.image.state      = ExtractShaderAccessType( qual ) | EResourceState_FromShaders( _currentStage );

                COMP_CHECK_ERR( name.IsDefined() );
                return true;
            }

            // subpass input
            if ( samp.isSubpass() )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::SubpassInput;

                un.image            = Default;
                un.image.type       = _ExtractImageType( type );
                un.image.state      = EResourceState::InputColorAttachment | EResourceState_FromShaders( _currentStage );   // TODO: depth attachment
                un.image.subpassInputIdx = CheckCast<ubyte>(qual.layoutAttachment);

                COMP_CHECK_ERR( name.IsDefined() );
                COMP_CHECK_ERR( qual.hasAttachment(), "attachment index for '"s << node_name << "' is not defined" );
                reflection.fragment.fragmentIO.inputAttachments.resize( Max( uint(qual.layoutAttachment)+1, reflection.fragment.fragmentIO.inputAttachments.size() ));

                const auto  io_name = ShaderIOName::Optimized_t{ ExtractShaderIOName( node )};
                COMP_CHECK_ERR( io_name.IsDefined() );

                COMP_CHECK_ERR( not reflection.fragment.fragmentIO.inputAttachments.at<0>( uint(qual.layoutAttachment) ).IsDefined() );
                reflection.fragment.fragmentIO.inputAttachments.set( uint(qual.layoutAttachment), io_name, _ExtractShaderIOType( type ));
                return true;
            }

            // sampler
            if ( samp.isPureSampler() )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::Sampler;

                COMP_CHECK_ERR( name.IsDefined() );
                return true;
            }

            // texel buffer
            if ( samp.isBuffer() )
            {
                COMP_CHECK_ERR( qual.storage == TStorageQualifier::EvqUniform or qual.storage == TStorageQualifier::EvqBuffer,
                                "buffer type for '"s << node_name << "' is not valid" );

                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = (qual.storage == TStorageQualifier::EvqUniform ? EDescriptorType::UniformTexelBuffer : EDescriptorType::StorageTexelBuffer);

                un.texelBuffer       = Default;
                un.texelBuffer.state = (qual.storage == TStorageQualifier::EvqUniform ? EResourceState::ShaderSample : ExtractShaderAccessType( qual )) |
                                        EResourceState_FromShaders( _currentStage );
                un.texelBuffer.type  = _ExtractImageType( type );

                COMP_CHECK_ERR( name.IsDefined() );
                return true;
            }

            // combined texture
            if ( samp.isCombined() )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::CombinedImage;

                un.image            = Default;
                un.image.state      = EResourceState::ShaderSample | EResourceState_FromShaders( _currentStage );
                un.image.type       = _ExtractImageType( type );

                COMP_CHECK_ERR( name.IsDefined() );
                return true;
            }

            // texture
            if ( samp.isTexture() )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::SampledImage;

                un.image            = Default;
                un.image.state      = EResourceState::ShaderSample | EResourceState_FromShaders( _currentStage );
                un.image.type       = _ExtractImageType( type );

                COMP_CHECK_ERR( name.IsDefined() );
                return true;
            }

            COMP_RETURN_ERR( "unknown sampler type!" );
        }

        // uniform buffer or storage buffer
        if ( type.getBasicType() == TBasicType::EbtBlock    and
             (qual.storage == TStorageQualifier::EvqUniform or qual.storage == TStorageQualifier::EvqBuffer) )
        {
            COMP_CHECK_ERR( type.isStruct() );
            COMP_CHECK_ERR( qual.hasBinding(), "binding index for '"s << node_name << "' is not defined" );

            // uniform block
            if ( qual.storage == TStorageQualifier::EvqUniform )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::UniformBuffer;

                un.buffer                   = Default;
                un.buffer.state             = EResourceState::ShaderUniform | EResourceState_FromShaders( _currentStage );
                un.buffer.arrayStride       = Bytes32u{0u};
                un.buffer.dynamicOffsetIndex= UMax;
                un.buffer.typeName          = GetShaderStructName( type.getTypeName() );

                COMP_CHECK_ERR( name.IsDefined() );

                Bytes32u    stride, offset;
                COMP_CHECK_ERR( _CalculateStructSize( type, OUT un.buffer.staticSize, OUT stride, OUT offset ));
                COMP_CHECK_ERR( offset == 0u );
                return true;
            }

            // storage block
            if ( qual.storage == TStorageQualifier::EvqBuffer )
            {
                auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
                name                = ExtractUniformID( node );
                un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
                un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
                un.stages           = _currentStage;
                un.type             = EDescriptorType::StorageBuffer;

                un.buffer                   = Default;
                un.buffer.state             = ExtractShaderAccessType( qual ) | EResourceState_FromShaders( _currentStage );
                un.buffer.dynamicOffsetIndex= UMax;
                un.buffer.typeName          = GetShaderStructName( type.getTypeName() );

                COMP_CHECK_ERR( name.IsDefined() );

                Bytes32u    offset;
                COMP_CHECK_ERR( _CalculateStructSize( type, OUT un.buffer.staticSize, OUT un.buffer.arrayStride, OUT offset ));
                COMP_CHECK_ERR( offset == 0u );
                return true;
            }

            COMP_RETURN_ERR( "unknown buffer type!" );
        }

        // acceleration structure
        if ( type.getBasicType() == TBasicType::EbtAccStruct )
        {
            COMP_CHECK_ERR( qual.hasBinding(), "binding index for '"s << node_name << "' is not defined" );

            auto& [name, un]    = descriptor_set.layout.uniforms.emplace_back();
            name                = ExtractUniformID( node );
            un.binding.vkIndex  = CheckCast<ushort>( qual.layoutBinding );
            un.arraySize        = CheckCast<ushort>( GetArraySize( type ));
            un.stages           = _currentStage;
            un.type             = EDescriptorType::RayTracingScene;
            //rt_scene.state    = EResourceState::_RayTracingShader | EResourceState::ShaderRead | EResourceState_FromShaders( _currentStage );
            COMP_CHECK_ERR( name.IsDefined() );
            return true;
        }

        // uniform
        if ( qual.storage == TStorageQualifier::EvqUniform )
        {
            COMP_RETURN_ERR( "uniform is not supported for Vulkan!" );
        }

        COMP_RETURN_ERR( "unknown external type '"s << node_name << "'" );
    }

/*
=================================================
    _MergeWithGeometryInputPrimitive
=================================================
*/
    void  SpirvCompiler::_MergeWithGeometryInputPrimitive (INOUT TopologyBits_t &topologyBits, uint type) const
    {
        using namespace glslang;

        switch_enum( BitCast<TLayoutGeometry>(type) )
        {
            case TLayoutGeometry::ElgPoints : {
                topologyBits.insert( EPrimitive::Point );
                return;
            }
            case TLayoutGeometry::ElgLines : {
                topologyBits.insert( EPrimitive::LineList );
                topologyBits.insert( EPrimitive::LineStrip );
                return;
            }
            case TLayoutGeometry::ElgLinesAdjacency : {
                topologyBits.insert( EPrimitive::LineListAdjacency );
                topologyBits.insert( EPrimitive::LineStripAdjacency );
                return;
            }
            case TLayoutGeometry::ElgTriangles : {
                topologyBits.insert( EPrimitive::TriangleList );
                topologyBits.insert( EPrimitive::TriangleStrip );
                topologyBits.insert( EPrimitive::TriangleFan );
                return;
            }
            case TLayoutGeometry::ElgTrianglesAdjacency : {
                topologyBits.insert( EPrimitive::TriangleListAdjacency );
                topologyBits.insert( EPrimitive::TriangleStripAdjacency );
                return;
            }
            case TLayoutGeometry::ElgNone :
            case TLayoutGeometry::ElgLineStrip :
            case TLayoutGeometry::ElgTriangleStrip :
            case TLayoutGeometry::ElgQuads :
            case TLayoutGeometry::ElgIsolines :
                break;
        }
        switch_end
        COMP_RETURN_ERR( "invalid geometry input primitive type!", void() );
    }

/*
=================================================
    _ProcessShaderInfo
=================================================
*/
    bool  SpirvCompiler::_ProcessShaderInfo (INOUT ShaderReflection &result) const
    {
        using namespace glslang;

        const auto  GetLocalSizeSpecId = [this] (int dim)
        {{
            int spec = _intermediate->getLocalSizeSpecId(dim);
            return spec != TQualifier::layoutNotSet ? uint(spec) : UMax;
        }};

        switch_enum( _intermediate->getStage() )
        {
            case EShLangVertex :
            {
                break;
            }

            case EShLangTessControl :
            {
                result.tessellation.patchControlPoints  = uint(_intermediate->getVertices());

                result.vertex.supportedTopology.insert( EPrimitive::Patch );
                break;
            }

            case EShLangTessEvaluation :
            {
                break;
            }

            case EShLangGeometry :
            {
                _MergeWithGeometryInputPrimitive( INOUT result.vertex.supportedTopology, _intermediate->getInputPrimitive() );
                break;
            }

            case EShLangFragment :
            {
                result.fragment.earlyFragmentTests = (_intermediate->getEarlyFragmentTests() or not _intermediate->isDepthReplacing());

                break;
            }

            case EShLangCompute :
            {
                if ( _intermediate->isLocalSizeSet() )
                {
                    result.compute.localGroupSize.x = _intermediate->getLocalSize(0);
                    result.compute.localGroupSize.y = _intermediate->getLocalSize(1);
                    result.compute.localGroupSize.z = _intermediate->getLocalSize(2);
                }

                result.compute.localGroupSpec.x = GetLocalSizeSpecId( 0 );
                result.compute.localGroupSpec.y = GetLocalSizeSpecId( 1 );
                result.compute.localGroupSpec.z = GetLocalSizeSpecId( 2 );
                break;
            }

            case EShLangRayGen :
            {
                break;
            }
            case EShLangIntersect :
            {
                break;
            }
            case EShLangAnyHit :
            {
                break;
            }
            case EShLangClosestHit :
            {
                break;
            }
            case EShLangMiss :
            {
                break;
            }
            case EShLangCallable :
            {
                break;
            }

            case EShLangTask :
            {
                if ( _intermediate->isLocalSizeSet() )
                {
                    result.mesh.taskGroupSize.x = _intermediate->getLocalSize(0);
                    result.mesh.taskGroupSize.y = _intermediate->getLocalSize(1);
                    result.mesh.taskGroupSize.z = _intermediate->getLocalSize(2);
                }

                result.mesh.taskGroupSpec.x = GetLocalSizeSpecId( 0 );
                result.mesh.taskGroupSpec.y = GetLocalSizeSpecId( 1 );
                result.mesh.taskGroupSpec.z = GetLocalSizeSpecId( 2 );
                break;
            }
            case EShLangMesh :
            {
                result.mesh.maxVertices     = uint(_intermediate->getVertices());
                result.mesh.maxPrimitives   = uint(_intermediate->getPrimitives());
                result.mesh.maxIndices      = result.mesh.maxPrimitives;

                AE_END_ENUM_CHECKS();
                switch ( _intermediate->getOutputPrimitive() )
                {
                    case TLayoutGeometry::ElgPoints :
                        result.mesh.topology    = EPrimitive::Point;
                        result.mesh.maxIndices *= 1;
                        break;

                    case TLayoutGeometry::ElgLines :
                        result.mesh.topology    = EPrimitive::LineList;
                        result.mesh.maxIndices *= 2;
                        break;

                    case TLayoutGeometry::ElgTriangles :
                        result.mesh.topology    = EPrimitive::TriangleList;
                        result.mesh.maxIndices *= 3;
                        break;

                    default :
                        CHECK(false);
                        break;
                }
                AE_BEGIN_ENUM_CHECKS();

                if ( _intermediate->isLocalSizeSet() )
                {
                    result.mesh.meshGroupSize.x = _intermediate->getLocalSize(0);
                    result.mesh.meshGroupSize.y = _intermediate->getLocalSize(1);
                    result.mesh.meshGroupSize.z = _intermediate->getLocalSize(2);
                }

                result.mesh.meshGroupSpec.x = GetLocalSizeSpecId( 0 );
                result.mesh.meshGroupSpec.y = GetLocalSizeSpecId( 1 );
                result.mesh.meshGroupSpec.z = GetLocalSizeSpecId( 2 );
                break;
            }

            case EShLangCount : break;
        }
        switch_end
        return true;
    }

} // AE::PipelineCompiler
