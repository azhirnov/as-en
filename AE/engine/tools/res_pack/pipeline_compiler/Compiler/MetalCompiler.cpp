// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_SPIRV_CROSS

# ifdef AE_COMPILER_MSVC
#   pragma warning (push, 0)
#   pragma warning (disable: 4266)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdouble-promotion"
# endif
# ifdef AE_COMPILER_GCC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdouble-promotion"
# endif

# include "spirv_cross/spirv_msl.hpp"

# ifdef AE_COMPILER_MSVC
#   pragma warning (pop)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic pop
# endif
# ifdef AE_COMPILER_GCC
#   pragma GCC diagnostic pop
# endif

# include "Compiler/MetalCompiler.h"

namespace AE::PipelineCompiler
{
namespace
{
/*
=================================================
    MetalToMacOSVersion
----
    see '1.5.8 Compiler Options Controlling the Language Version'
=================================================
*/
    ND_ static Version2  MetalToMacOSVersion (Version2 ver)
    {
        StaticAssert( Graphics::FeatureSet::MaxMetalVersion == 310 );
        switch ( ver.To10() )
        {
            case 20 :   return {10, 13};
            case 21 :   return {10, 14};
            case 22 :   return {10, 15};
            case 23 :   return {11, 0};
            case 24 :   return {12, 0};
            case 30 :   return {13, 0};
            case 31 :   return {14, 0};
        }
        RETURN_ERR( "unsupported metal version" );
    }

/*
=================================================
    MetalToiOSVersion
=================================================
*/
    ND_ static Version2  MetalToiOSVersion (Version2 ver)
    {
        StaticAssert( Graphics::FeatureSet::MaxMetalVersion == 310 );
        switch ( ver.To10() )
        {
            case 20 :   return {11, 0};
            case 21 :   return {12, 0};
            case 22 :   return {13, 0};
            case 23 :   return {14, 0};
            case 24 :   return {15, 0};
            case 30 :   return {16, 0};
            case 31 :   return {17, 0};
        }
        RETURN_ERR( "unsupported metal version" );
    }
}

/*
=================================================
    constructor
=================================================
*/
    MetalCompiler::MetalCompiler (ArrayView<Path> includeDirs) __NE___
    {
        _directories.reserve( includeDirs.size() );

        for (auto& path : includeDirs)
        {
            CHECK( path.is_absolute() );
            _directories.push_back( path.string() );
        }
    }

/*
=================================================
    SpirvToMsl
----
    warning: has false positive memleak because of 'static string'
=================================================
*/
    bool  MetalCompiler::SpirvToMsl (const SpirvToMslConfig &cfg, SpirvBytecode_t spirv,
                                     INOUT SpirvCompiler::ShaderReflection &reflection, OUT String &src) const
    {
        using namespace spirv_cross;
        try
        {
            using EPlatform = CompilerMSL::Options::Platform;

            CompilerMSL::Options options;

            const auto  msl     = EShaderVersion_Ver2( cfg.version );
            const bool  is_mac  = AllBits( cfg.version, EShaderVersion::_Metal_Mac, EShaderVersion::_Mask );
            const bool  is_ios  = AllBits( cfg.version, EShaderVersion::_Metal_iOS, EShaderVersion::_Mask );

            CHECK_ERR(( msl >= Version2{2,0} ));
            CHECK_ERR( is_mac != is_ios );

            options.set_msl_version( msl.major, msl.minor );
            options.platform    = is_mac ?  EPlatform::macOS :
                                  is_ios ?  EPlatform::iOS   :
                                            EPlatform(0xFF);

            options.texture_buffer_native                   = (msl >= Version2{2,1});
            options.argument_buffers                        = cfg.useArgBuffer;
            options.use_framebuffer_fetch_subpasses         = true;
            options.force_active_argument_buffer_resources  = true;

            CompilerMSL comp { RVRef(spirv) };
            comp.set_msl_options( options );

            src = comp.compile();           // throw
            CHECK_ERR( not src.empty() );

            {
                Input   in;
                in.shaderType   = cfg.shaderType;
                in.source       = src;

                String  log;
                CHECK_ERR( BuildReflection( in, INOUT reflection, OUT log ));
            }
            return true;
        }
        catch (...)
        {
            src.clear();
            return false;
        }
    }

/*
=================================================
    Compile
----
    USAGE: metal.exe [options] <inputs>

    -o <file>                           Write output to <file>
    -mios-version-min=<value>           Set iOS deployment target
    -mmacosx-version-min=<value>        Set Mac OS X deployment target
    -ftexture-write-rounding
    -fpreserve-invariance
    -ffast-math  -fno-fast-math
    -Werror
    -O2                                 Optimize for performance (default).
    -Os                                 Like -O2 with extra optimizations to reduce code size.
    -frecord-sources                    Enable the compiler to store source information into the AIR or Metal library file (.metallib).

    TODO:
        -ftexture-write-rounding-mode

        1.5.12 Options for Compiling to GPU binaries
=================================================
*/
    bool  MetalCompiler::Compile (const Input &in, OUT MetalBytecode_t &bytecode, OUT String &log) const
    {
        bytecode.clear();
        log.clear();

    #ifdef AE_METAL_TOOLS
        FileSystem::SetCurrentPath( Path{AE_METAL_TOOLS} );

        const String    tmp_shader_name = "temp-"s << ToString<16>( usize(this) ) << ".metal";
        const String    out_name        = "bytecode-"s << ToString<16>( usize(this) ) << ".metallib";
        const Version2  msl_ver         = EShaderVersion_Ver2( in.version );

        FileSystem::Remove( Path{out_name} );
        FileSystem::Remove( Path{tmp_shader_name} );

        // file to temp file
        {
            String      tmp_source;
            StringView  source      = in.source;

            if ( in.enablePreprocessing and _preprocessor )
            {
                CHECK_ERR( _preprocessor->Process( in.shaderType, in.fileLoc, in.headerLines, source, OUT tmp_source ));
                source = tmp_source;
            }

            FileWStream     file{ Path{tmp_shader_name} };
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( file.Write( source ));
        }

        String  cmd;
        cmd << "metal.exe";

        for (auto& dir : _directories)
        {
            CHECK_ERR( FileSystem::IsDirectory( Path{dir} ));
            cmd << " -I \"" << dir << '"';
        }

        // shader options
        {
            if ( AllBits( in.options, EShaderOpt::DebugInfo ))
                cmd << " -frecord-sources";

            if ( AllBits( in.options, EShaderOpt::WarnAsError ))
                cmd << " -Werror";

            if ( AnyBits( in.options, EShaderOpt::Optimize | EShaderOpt::StrongOptimization ))
                cmd << " -ffast-math -Os";  // or -O2
            else
                cmd << " -fno-fast-math";
        }

        // choose version
        {
            if ( msl_ver >= Version2{3,0} )
            {
                cmd << " -std=metal" << ToString( msl_ver.major ) << '.' << ToString( msl_ver.minor );
            }
            else
            if ( in.target == ECompilationTarget::Metal_iOS )
            {
                const auto  ios_ver = MetalToiOSVersion( msl_ver );
                cmd << " -std=ios-metal" << ToString( msl_ver.major ) << '.' << ToString( msl_ver.minor )
                    << " -mios-version-min=" << ToString( ios_ver.major ) << '.' << ToString( ios_ver.minor );
            }
            else
            if ( in.target == ECompilationTarget::Metal_Mac )
            {
                const auto  mac_ver = MetalToMacOSVersion( msl_ver );
                cmd << " -std=macos-metal" << ToString( msl_ver.major ) << '.' << ToString( msl_ver.minor )
                    << " -mmacosx-version-min=" << ToString( mac_ver.major ) << '.' << ToString( mac_ver.minor );
            }
            else
                RETURN_ERR( "unsupported target version" );
        }

        cmd << " -o \"" << out_name << "\" \""
            << tmp_shader_name << '"';

        // compile shader
        {
            String          output;
            WindowsProcess  proc;
            CHECK_ERR( proc.ExecuteAsync( cmd, WindowsProcess::EFlags::ReadOutput | WindowsProcess::EFlags::NoWindow ));
            CHECK_ERR( proc.WaitAndClose( OUT output, null, seconds{60*10} ));

            if ( not output.empty() )
            {
                if ( not _ParseOutput( in, output, tmp_shader_name, OUT log ) or not FileSystem::IsFile( out_name ))
                    return false;
            }
        }

        // read bytecode
        {
            FileRStream     file{ out_name };
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( file.Read( file.RemainingSize(), OUT bytecode ));
        }

        FileSystem::Remove( Path{tmp_shader_name} );
        FileSystem::Remove( Path{out_name} );

        return true;
    #else

        RETURN_ERR( "Metal tools is not supported" );
    #endif
    }

/*
=================================================
    _ParseOutput
----
    pattern:
        <filename>:<line>:<column>: <type>:

    types:
        'fatal error', 'error', 'note', 'warning'
=================================================
*/
    bool  MetalCompiler::_ParseOutput (const Input &in, StringView output, StringView tempShaderName, OUT String &log) const
    {
        usize               pos         = 0;
        Array<StringView>   tokens;
        bool                has_errors  = false;
        const String        path_str    = in.fileLoc.path.string();

        const auto  Tokenize = [](StringView str , OUT Array<StringView> &outTokens)
        {{
            outTokens.clear();

            usize   begin = 0;
            for (usize i = 1; i < str.length(); ++i)
            {
                const char  c = str[i-1];
                const char  n = str[i];

                if ( (c == ':') and (n != '/') and (n != '\\') )
                {
                    for (usize j = begin; j < str.length(); ++j)
                    {
                        const char  k = str[j];
                        if ( not ((k == ' ') or (k == '\t')) )
                        {
                            begin = j;
                            break;
                        }
                    }
                    outTokens.push_back( StringView{ str.data() + begin, i-1 - begin });
                    begin = i;

                }
            }
            outTokens.push_back( StringView{ str.data() + begin, str.length() - begin });
        }};

        for (;;)
        {
            if_unlikely( pos >= output.size() )
                break;

            StringView  line;
            Parser::ReadLineToEnd( output, INOUT pos, OUT line );

            if_unlikely( line.empty() )
                continue;

            // skip: ':', spaces
            Tokenize( line, OUT tokens );

            if ( tokens.size() < 4 )
            {
                // console output format: <type>: <message>
                if ( tokens.size() and tokens[0] == "error" )
                    has_errors = true;

                log << line << '\n';
                continue;
            }

            // compiler output format:  <filename>:<line>:<column>: <type>:

            const uint  line_idx = StringToInt( tokens[1] );
            const auto  type     = tokens[3];
            const auto  message  = line.substr( usize(Bytes{type.data() + type.size()} - Bytes{line.data()}) + 1 );

            if ( type == "fatal error" or type == "error" )
                has_errors = true;
            else
            if ( type == "note" ) {}
            else
            if ( type == "warning" ) {}
            else
            {
                CHECK(false);   // unknown message type
            }

            // current file
            if ( tokens[0] == tempShaderName )
            {
                if ( not path_str.empty() and line_idx >= in.headerLines )
                    log << path_str << '(' << ToString( line_idx - in.headerLines ) << "):\n" << message << '\n';
                else
                    log << line << '\n';
            }
            else
            // find in include directories
            {
                ASSERT( FileSystem::IsFile( Path{tokens[0]} ));
                ASSERT( Path{tokens[0]}.is_absolute() );

                log << tokens[0] << '(' << tokens[1] << "):\n" << message << '\n';
            }
        }

        if ( not path_str.empty() )
            FindAndReplace( INOUT log, tempShaderName, path_str );

        return not has_errors;
    }

/*
=================================================
    BuildReflection
=================================================
*/
    bool  MetalCompiler::BuildReflection (const Input &in, INOUT ShaderReflection &reflection, OUT String &log) const
    {
        log.clear();

        CHECK_ERR( not in.source.empty() );
        CHECK_ERR( in.shaderType != Default );

        switch ( in.shaderType )
        {
            case EShader::Vertex :      break;
            case EShader::Fragment :    break;
            case EShader::Compute :     break;
            case EShader::Tile :        break;
            case EShader::MeshTask :    break;
            case EShader::Mesh :        CHECK_ERR( _BuildReflectionForMeshShader( in.source, INOUT reflection, INOUT log ));    break;
            default :
                RETURN_ERR( "unsupported shader type" );
        }

        return true;
    }

/*
=================================================
    _BuildReflectionForMeshShader
=================================================
*/
    bool  MetalCompiler::_BuildReflectionForMeshShader (StringView src, INOUT ShaderReflection &reflection, INOUT String &log) const
    {
        Unused( log );

        const String    suffix      = "metal::topology::";
        EPrimitive      topology    = Default;

        for (usize i = 0; i < src.size();)
        {
            usize   pos = src.find( suffix, i );
            if ( pos == StringView::npos )
                break;

            pos += suffix.size();
            const usize begin = pos;

            for (; pos < src.size(); ++pos)
            {
                const char  c = src[pos];
                if ( not ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z')) )
                    break;
            }

            StringView  topo        = src.substr( begin, pos - begin );
            EPrimitive  new_topo    = Default;

            if ( topo == "triangle" )   new_topo = EPrimitive::TriangleList;
            if ( topo == "line" )       new_topo = EPrimitive::LineList;
            if ( topo == "point" )      new_topo = EPrimitive::Point;
            CHECK_ERR( new_topo != Default );

            // merge with prev topology
            CHECK_ERR( topology == Default or topology == new_topo );
            topology = new_topo;

            i = pos;
        }

        // topology must be defined in mesh shader
        CHECK_ERR( topology != Default );
        reflection.mesh.topology = topology;

        return true;
    }


} // AE::PipelineCompiler
//-----------------------------------------------------------------------------

#else

# include "Compiler/MetalCompiler.h"

namespace AE::PipelineCompiler
{

    MetalCompiler::MetalCompiler (ArrayView<Path>) __NE___
    {}

    bool  MetalCompiler::SpirvToMsl (const SpirvToMslConfig &, SpirvBytecode_t, INOUT SpirvCompiler::ShaderReflection &, OUT String &) const
    {
        return false;
    }

    bool  MetalCompiler::Compile (const Input &, OUT MetalBytecode_t &, OUT String &) const
    {
        return false;
    }

    bool  MetalCompiler::BuildReflection (const Input &, INOUT ShaderReflection &, OUT String &) const
    {
        return false;
    }

} // AE::PipelineCompiler
#endif // AE_ENABLE_SPIRV_CROSS
