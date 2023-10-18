// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptShader.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
/*
=================================================
    ETessPatch_ToString
=================================================
*/
    ND_ static StringView  ETessPatch_ToString (ETessPatch value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case ETessPatch::Points :       return "point_mode";
            case ETessPatch::Isolines :     return "isolines";
            case ETessPatch::Triangles :    return "triangles";
            case ETessPatch::Quads :        return "quads";

            case ETessPatch::Unknown :
            case ETessPatch::_Count :       break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unsupported tessellation patch mode" );
    }

/*
=================================================
    ETessSpacing_ToString
=================================================
*/
    ND_ static StringView  ETessSpacing_ToString (ETessSpacing value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case ETessSpacing::Equal :          return "equal_spacing";
            case ETessSpacing::FractionalEven : return "fractional_even_spacing";
            case ETessSpacing::FractionalOdd :  return "fractional_odd_spacing";

            case ETessSpacing::Unknown :
            case ETessSpacing::_Count :         break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unsupported tessellation patch mode" );
    }

} // namespace


/*
=================================================
    constructor
=================================================
*/
    ScriptShader::ScriptShader () :
        options{ ObjectStorage::Instance()->shaderOptions }
    {}

/*
=================================================
    AddSpec
=================================================
*/
    void  ScriptShader::AddSpec (EValueType valType, const String &name) __Th___
    {
        BEGIN_ENUM_CHECKS();
        switch ( valType )
        {
            case EValueType::Bool8 :
            case EValueType::Bool32 :
            case EValueType::Int8 :
            case EValueType::Int16 :
            case EValueType::Int32 :
            case EValueType::UInt8 :
            case EValueType::UInt16 :
            case EValueType::UInt32 :
            case EValueType::Float16 :
            case EValueType::Float32 :
            case EValueType::Int8_Norm :
            case EValueType::Int16_Norm :
            case EValueType::UInt8_Norm :
            case EValueType::UInt16_Norm :  break;

            case EValueType::Int64 :
            case EValueType::UInt64 :
            case EValueType::DeviceAddress :
            case EValueType::Float64 :
            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       CHECK_THROW_MSG( false, "unsupported value type" );
        }
        END_ENUM_CHECKS();

        ObjectStorage::Instance()->AddName<SpecializationName>( name );
        CHECK_THROW_MSG( _spec.size() < _spec.capacity() );

        _spec.emplace( SpecializationName{name}, SpecInfo{ valType, _specId });
        _specConst.emplace( SpecializationName{name}, _specId );
        _specId++;
    }

/*
=================================================
    SetComputeSpec
=================================================
*/
    void  ScriptShader::SetComputeSpec1 () __Th___
    {
        return SetComputeSpecAndDefault1( 0 );
    }

    void  ScriptShader::SetComputeSpec2 () __Th___
    {
        return SetComputeSpecAndDefault2( 0, 0 );
    }

    void  ScriptShader::SetComputeSpec3 () __Th___
    {
        return SetComputeSpecAndDefault3( 0, 0, 0 );
    }

    void  ScriptShader::SetComputeSpecAndDefault1 (uint x) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _localSizeSpec.x    = _specId++;
        _localSizeSpec.y    = UMax;
        _localSizeSpec.z    = UMax;

        _defaultLocalSize   = { x, 1u, 1u };
        _requiredStages     = EShaderStages::Compute;
    }

    void  ScriptShader::SetComputeSpecAndDefault2 (uint x, uint y) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _localSizeSpec.x    = _specId++;
        _localSizeSpec.y    = _specId++;
        _localSizeSpec.z    = UMax;

        _defaultLocalSize   = { x, y, 1u };
        _requiredStages     = EShaderStages::Compute;
    }

    void  ScriptShader::SetComputeSpecAndDefault3 (uint x, uint y, uint z) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _localSizeSpec.x    = _specId++;
        _localSizeSpec.y    = _specId++;
        _localSizeSpec.z    = _specId++;

        _defaultLocalSize   = { x, y, z };
        _requiredStages     = EShaderStages::Compute;
    }

/*
=================================================
    SetComputeLocalSize
=================================================
*/
    void  ScriptShader::SetComputeLocalSize1 (uint x) __Th___
    {
        return SetComputeLocalSize3( x, 1, 1 );
    }

    void  ScriptShader::SetComputeLocalSize2 (uint x, uint y) __Th___
    {
        return SetComputeLocalSize3( x, y, 1 );
    }

    void  ScriptShader::SetComputeLocalSize3 (uint x, uint y, uint z) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _defaultLocalSize   = { x, y, z };
        _requiredStages     = EShaderStages::Compute;
    }

/*
=================================================
    SetMeshSpec
=================================================
*/
    void  ScriptShader::SetMeshSpecAndDefault1 (uint x) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _localSizeSpec.x    = _specId++;
        _localSizeSpec.y    = UMax;
        _localSizeSpec.z    = UMax;

        _defaultLocalSize   = { x, 1u, 1u };
        _requiredStages     = EShaderStages::Mesh | EShaderStages::MeshTask;
    }

    void  ScriptShader::SetMeshSpecAndDefault2 (uint x, uint y) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _localSizeSpec.x    = _specId++;
        _localSizeSpec.y    = _specId++;
        _localSizeSpec.z    = UMax;

        _defaultLocalSize   = { x, y, 1u };
        _requiredStages     = EShaderStages::Mesh | EShaderStages::MeshTask;
    }

    void  ScriptShader::SetMeshSpecAndDefault3 (uint x, uint y, uint z) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _localSizeSpec.x    = _specId++;
        _localSizeSpec.y    = _specId++;
        _localSizeSpec.z    = _specId++;

        _defaultLocalSize   = { x, y, z };
        _requiredStages     = EShaderStages::Mesh | EShaderStages::MeshTask;
    }

    void  ScriptShader::SetMeshSpec1 () __Th___
    {
        return SetMeshSpecAndDefault1( 0 );
    }

    void  ScriptShader::SetMeshSpec2 () __Th___
    {
        return SetMeshSpecAndDefault2( 0, 0 );
    }

    void  ScriptShader::SetMeshSpec3 () __Th___
    {
        return SetMeshSpecAndDefault3( 0, 0, 0 );
    }

/*
=================================================
    SetMeshLocalSize
=================================================
*/
    void  ScriptShader::SetMeshLocalSize1 (uint x) __Th___
    {
        return SetMeshLocalSize3( x, 1, 1 );
    }

    void  ScriptShader::SetMeshLocalSize2 (uint x, uint y) __Th___
    {
        return SetMeshLocalSize3( x, y, 1 );
    }

    void  ScriptShader::SetMeshLocalSize3 (uint x, uint y, uint z) __Th___
    {
        CHECK_THROW_MSG( _requiredStages == Default );

        _defaultLocalSize   = { x, y, z };
        _requiredStages     = EShaderStages::Mesh | EShaderStages::MeshTask;
    }

/*
=================================================
    SetMeshOutput
=================================================
*/
    void  ScriptShader::SetMeshOutput (uint maxVertices, uint maxPrimitives, EPrimitive topology) __Th___
    {
        CHECK_THROW_MSG( maxVertices > 0 );
        CHECK_THROW_MSG( maxPrimitives > 0 );
        CHECK_THROW_MSG( AnyEqual( topology, EPrimitive::TriangleList ),
            "Unsupported topology for mesh shader output, supported only: 'TriangleList', ..." );   // TODO

        _mashMaxVertices    = maxVertices;
        _meshMaxPrimitives  = maxPrimitives;
        _meshTopology       = topology;
    }

/*
=================================================
    SetTessPatchSize
=================================================
*/
    void  ScriptShader::SetTessPatchSize (uint count) __Th___
    {
        CHECK_THROW_MSG( count > 0 );
        CHECK_THROW_MSG( _tcsPatchSize == 0, "already specified" );

        _tcsPatchSize = count;
    }

/*
=================================================
    SetTessPatchMode
=================================================
*/
    void  ScriptShader::SetTessPatchMode (ETessPatch mode, ETessSpacing spacing, bool ccw) __Th___
    {
        CHECK_THROW_MSG( mode > ETessPatch::Unknown and mode < ETessPatch::_Count );
        CHECK_THROW_MSG( spacing > ETessSpacing::Unknown and spacing < ETessSpacing::_Count );
        CHECK_THROW_MSG( _tesPatchMode == Default and _tesPatchSpacing == Default, "already specified" );

        _tesPatchMode       = mode;
        _tesPatchSpacing    = spacing;
        _tesFrontFaceCCW    = ccw;
    }

/*
=================================================
    Define
=================================================
*/
    void  ScriptShader::Define (const String &value) __Th___ 
    {
        _defines << '\n' << value;
    }

/*
=================================================
    LoadSelf
=================================================
*/
    void  ScriptShader::LoadSelf () __Th___
    {
        CHECK_THROW_MSG( _filename.empty() );
        CHECK_THROW_MSG( _source.empty() );
        CHECK_THROW_MSG( _absolutePath.path.empty() );

        _absolutePath.path  = ObjectStorage::Instance()->pipelineFilename;
        _absolutePath.line  = 0;
        _filename           = _absolutePath.path.filename().string();

        CHECK_THROW_MSG( not _absolutePath.path.empty() );
    }

/*
=================================================
    SetSource
=================================================
*/
    void  ScriptShader::SetSource (EShader shaderType, String src)
    {
        type    = shaderType;
        _source = RVRef(src);
    }

    void  ScriptShader::SetSource2 (EShader shaderType, String src, PathAndLine path)
    {
        type            = shaderType;
        _source         = RVRef(src);
        _absolutePath   = RVRef(path);
    }

/*
=================================================
    _UpdatePath
=================================================
*/
    void  ScriptShader::_UpdatePath () C_Th___
    {
        if ( not _absolutePath.path.empty() )
            return;

        Path    fpath;
        for (auto& folder : ObjectStorage::Instance()->shaderFolders)
        {
            Path    sh_path = (Path{ folder }.append( _filename ));

            if ( FileSystem::IsFile( sh_path ))
            {
                fpath = FileSystem::ToAbsolute( sh_path );
                break;
            }
        }
        CHECK_THROW_MSG( not fpath.empty(), "Can't find shader: '"s << _filename << "'" );

        _absolutePath.path  = RVRef(fpath);
        _absolutePath.line  = 0;
    }

/*
=================================================
    GetSource
=================================================
*/
    String  ScriptShader::GetSource () C_Th___
    {
        CHECK_THROW_MSG( _source.empty() != _filename.empty(),
            "only one of the properties 'source' and 'filename' must be specified" );

        if ( not _source.empty() )
            return _source;

        _UpdatePath();

        FileRStream     file{ _absolutePath.path };
        if ( not file.IsOpen() )
        {
            CHECK_THROW_MSG( false, "Failed to open shader file: '"s << _filename << "'" );
        }

        String  str;
        if ( not file.Read( file.RemainingSize(), OUT str ))
        {
            CHECK_THROW_MSG( false, "Failed to read shader file: '"s << _filename << "'" );
        }
        return str;
    }

/*
=================================================
    GetPath
=================================================
*/
    PathAndLine  ScriptShader::GetPath () C_Th___
    {
        if ( not _absolutePath.path.empty() )
            return _absolutePath;

        if ( _filename.empty() )
            return Default;

        _UpdatePath();
        return _absolutePath;
    }

/*
=================================================
    GetEntry
=================================================
*/
    String const&  ScriptShader::GetEntry () const
    {
        if_unlikely( _entry.empty() )
        {
            _entry = "Main";
        }

        return _entry;
    }

/*
=================================================
    _Validate
=================================================
*/
    void  ScriptShader::_Validate () C_Th___
    {
        if ( AnyEqual( type, EShader::Compute, EShader::MeshTask, EShader::Mesh, EShader::Tile ))
        {
            const bool3     has_def     = (_defaultLocalSize != ~0u);
            const bool3     has_spec    = (_localSizeSpec != ~0u);

            CHECK_THROW_MSG( All(has_def | has_spec),
                "local_size or local_size_id must be defined" );
        }

        if ( type == EShader::Mesh )
        {
            CHECK_THROW_MSG( _mashMaxVertices   != 0    and
                             _meshMaxPrimitives != 0    and
                             _meshTopology      != Default,
                "use 'MeshOutput()' to set mesh shader output size and type" );
        }else{
            CHECK_THROW_MSG( _mashMaxVertices   == 0    and
                             _meshMaxPrimitives == 0    and
                             _meshTopology      == Default,
                "use 'MeshOutput()' only for Mesh shader" );
        }

        if ( type == EShader::TessControl ){
            CHECK_THROW_MSG( _tcsPatchSize != 0,
                "use 'TessPatchSize()' to set tessellation patch size" );
        }else{
            CHECK_THROW_MSG( _tcsPatchSize == 0,
                "use 'TessPatchSize()' only for TessControl shader" );
        }

        if ( type == EShader::TessEvaluation ){
            CHECK_THROW_MSG( _tesPatchMode != Default and _tesPatchSpacing != Default,
                "use 'TessPatchMode()' to set tessellation patch mode" );
        }else{
            CHECK_THROW_MSG( _tesPatchMode == Default and _tesPatchSpacing == Default,
                "use 'TessPatchMode()' only for TessEvaluation shader" );
        }
    }

/*
=================================================
    SpecToGLSL
=================================================
*/
    String  ScriptShader::SpecToGLSL () C_Th___
    {
        _Validate();  // throw

        auto&   storage = *ObjectStorage::Instance();
        String  str;

        const auto  AddLocalSize = [&str] (const uint3 &size)
        {{
            if ( Any( size > uint3(0) ))
            {
                str << "layout (";
                if ( size.x > 0 )   str << "local_size_x = " << ToString( size.x ) << ", ";
                if ( size.y > 0 )   str << "local_size_y = " << ToString( size.y ) << ", ";
                if ( size.z > 0 )   str << "local_size_z = " << ToString( size.z ) << ", ";

                str.pop_back();
                str.pop_back();
                str << ") in;\n";
            }
        }};
        const auto  AddLocalSizeSpec = [&str] (const uint3 &spec)
        {{
            str << "layout (";
            if ( spec.x != UMax )   str << "local_size_x_id = " << ToString( spec.x ) << ", ";
            if ( spec.y != UMax )   str << "local_size_y_id = " << ToString( spec.y ) << ", ";
            if ( spec.z != UMax )   str << "local_size_z_id = " << ToString( spec.z ) << ", ";

            str.pop_back();
            str.pop_back();
            str << ") in;\n";
        }};

        if ( _specId == 0 )
        {
            if ( AnyBits( _requiredStages, EShaderStages::Compute | EShaderStages::Mesh | EShaderStages::MeshTask ))
            {
                CHECK_THROW_MSG( AnyEqual( type, EShader::Compute, EShader::Mesh, EShader::MeshTask ));

                AddLocalSize( _defaultLocalSize );
            }
        }
        else
        {
            str << "//---- specialization ----\n";

            if ( AnyBits( _requiredStages, EShaderStages::Compute | EShaderStages::Mesh | EShaderStages::MeshTask ))
            {
                CHECK_THROW_MSG( AnyEqual( type, EShader::Compute, EShader::Mesh, EShader::MeshTask ));
                CHECK_THROW_MSG( Any( _localSizeSpec != uint3{~0u} ));

                AddLocalSizeSpec( _localSizeSpec );
                AddLocalSize( _defaultLocalSize );
                str << '\n';
            }

            for (auto [name, spec] : _spec)
            {
                str << "layout (constant_id = " << ToString( spec.index ) << ") const ";

                String  def_val;

                BEGIN_ENUM_CHECKS();
                switch ( spec.type )
                {
                    case EValueType::Bool8 :
                    case EValueType::Bool32 :   str << "bool";      def_val = "false";  break;
                    case EValueType::Int8 :     str << "int8_t";    def_val = "0";      break;
                    case EValueType::Int16 :    str << "int16_t";   def_val = "0";      break;
                    case EValueType::Int32 :    str << "int";       def_val = "0";      break;
                    case EValueType::UInt8 :    str << "uint8_t";   def_val = "0";      break;
                    case EValueType::UInt16 :   str << "uint16_t";  def_val = "0";      break;
                    case EValueType::UInt32 :   str << "uint";      def_val = "0";      break;
                    case EValueType::Float16 :  str << "float16_t"; def_val = "0.0";    break;
                    case EValueType::Float32 :  str << "float";     def_val = "0.0";    break;

                    case EValueType::Int8_Norm :
                    case EValueType::Int16_Norm :
                    case EValueType::UInt8_Norm :
                    case EValueType::UInt16_Norm :
                    case EValueType::Int64 :
                    case EValueType::UInt64 :
                    case EValueType::DeviceAddress :
                    case EValueType::Float64 :
                    case EValueType::Unknown :
                    case EValueType::_Count :
                    default :                   CHECK_THROW_MSG( false, "unsupported value type" );
                }
                END_ENUM_CHECKS();

                str << "  " << storage.GetName( name ) << " = " << def_val << ";\n";
            }
        }

        if ( type == EShader::Mesh )
        {
            str << "layout(max_vertices=" << ToString( _mashMaxVertices ) << ", max_primitives=" << ToString( _meshMaxPrimitives ) << ") out;\n";
            str << "layout(";
            switch ( _meshTopology )
            {
                case EPrimitive::Point :            str << "points";    break;
                case EPrimitive::LineList :         str << "lines";     break;
                case EPrimitive::TriangleList :     str << "triangles"; break;
                default :                           CHECK_THROW_MSG( false, "unsupported topology" );
            }
            str << ") out;\n"
                << "#define AE_maxVertices " << ToString( _mashMaxVertices ) << "\n"
                << "#define AE_maxPrimitives " << ToString( _meshMaxPrimitives ) << "\n\n";
        }

        if ( type == EShader::TessControl )
            str << "layout(vertices = " << ToString( _tcsPatchSize ) << ") out;\n";

        if ( type == EShader::TessEvaluation ) {
            str << "layout(" << ETessPatch_ToString( _tesPatchMode ) << ", "
                << ETessSpacing_ToString( _tesPatchSpacing ) << ", " 
                << (_tesFrontFaceCCW ? "ccw" : "cw") << ") in;\n";
        }

        str << "//------------------------\n\n";
        return str;
    }

/*
=================================================
    SpecToMSL
=================================================
*/
    String  ScriptShader::SpecToMSL () C_Th___
    {
        _Validate();  // throw

        String  str;

        if ( type == EShader::Mesh )
        {
            str << "#define AE_maxVertices " << ToString( _mashMaxVertices ) << "\n"
                << "#define AE_maxPrimitives " << ToString( _meshMaxPrimitives ) << "\n\n";
        }

        if ( _specId == 0 )
            return str;

        CHECK_THROW_MSG( _specId < 65535 ); // from specs

        auto&   storage = *ObjectStorage::Instance();

        str << "//---- specialization ----\n";

        if ( _requiredStages == EShaderStages::Compute )
        {
            CHECK_THROW_MSG( type == EShader::Compute );
            CHECK_THROW_MSG( Any( _localSizeSpec != uint3{~0u} ));
            CHECK_THROW_MSG( All( _defaultLocalSize > uint3(0) ));

            if ( _localSizeSpec.x != UMax )
                str << "constant uint  spec_LocalSizeX [[function_constant(" << ToString(_localSizeSpec.x) << ")]];\n";
            if ( _localSizeSpec.y != UMax )
                str << "constant uint  spec_LocalSizeY [[function_constant(" << ToString(_localSizeSpec.y) << ")]];\n";
            if ( _localSizeSpec.z != UMax )
                str << "constant uint  spec_LocalSizeZ [[function_constant(" << ToString(_localSizeSpec.z) << ")]];\n";

            str << "constant uint3  gl_WorkGroupSize = uint3( ";
            if ( _localSizeSpec.x == UMax ) str << ToString( _defaultLocalSize.x ) << "u, ";
            else                            str << "(is_function_constant_defined(spec_LocalSizeX) ? spec_LocalSizeX : " << ToString( _defaultLocalSize.x ) << "u), ";

            if ( _localSizeSpec.y == UMax ) str << ToString( _defaultLocalSize.y ) << "u, ";
            else                            str << "(is_function_constant_defined(spec_LocalSizeY) ? spec_LocalSizeY : " << ToString( _defaultLocalSize.y ) << "u), ";

            if ( _localSizeSpec.z == UMax ) str << ToString( _defaultLocalSize.z ) << "u";
            else                            str << "(is_function_constant_defined(spec_LocalSizeZ) ? spec_LocalSizeZ : " << ToString( _defaultLocalSize.z ) << "u)";
            str << " );\n";
        }

        for (auto [name, spec] : _spec)
        {
            str << "constant ";

            BEGIN_ENUM_CHECKS();
            switch ( spec.type )
            {
                case EValueType::Bool8 :
                case EValueType::Bool32 :       str << "bool";      break;
                case EValueType::Int8 :
                case EValueType::Int8_Norm :    str << "int8_t";    break;
                case EValueType::Int16 :
                case EValueType::Int16_Norm :   str << "int16_t";   break;
                case EValueType::Int32 :        str << "int";       break;
                case EValueType::UInt8 :
                case EValueType::UInt8_Norm :   str << "uint8_t";   break;
                case EValueType::UInt16 :
                case EValueType::UInt16_Norm :  str << "uint16_t";  break;
                case EValueType::UInt32 :       str << "uint";      break;
                case EValueType::Float16 :      str << "half";      break;
                case EValueType::Float32 :      str << "float";     break;

                case EValueType::Int64 :
                case EValueType::UInt64 :
                case EValueType::DeviceAddress :
                case EValueType::Float64 :
                case EValueType::Unknown :
                case EValueType::_Count :
                default :                       CHECK_THROW_MSG( false, "unsupported value type" );
            }
            END_ENUM_CHECKS();

            str << "  " << storage.GetName( name ) << " [[function_constant(" << ToString( spec.index ) << ")]];\n";
        }

        str << "//------------------------\n\n";
        return str;
    }

/*
=================================================
    InputToMSL
=================================================
*/
    String  ScriptShader::InputToMSL () C_Th___
    {
        if ( not AnyEqual( type, EShader::Compute, EShader::Mesh, EShader::MeshTask ))
            return "";

        String  str;
        str << "  /* compute builtin */\n"
            << "  uint3  gl_LocalInvocationID  [[thread_position_in_threadgroup]],\n"
            << "  uint3  gl_GlobalInvocationID [[thread_position_in_grid]],\n";

        if ( type == EShader::Mesh )
        {
            str << "  AE_MeshType  Out,\n";
        }

        return str;
    }

/*
=================================================
    MeshOutToMSL
=================================================
*/
    String  ScriptShader::MeshOutToMSL (String vertex, String primitive) C_Th___
    {
        CHECK_THROW_MSG( type == EShader::Mesh, "Only for mesh shader" );
        CHECK_THROW_MSG( not vertex.empty() );

        if ( primitive.empty() )
            primitive = "void";

        String  str;
        str <<  "using AE_MeshType = metal::mesh<\n"
            <<  "\t\t" << vertex << ",\n"                           // vertex type
                "\t\t" << primitive << ",\n"                        // primitive type
                "\t\t" << ToString( _mashMaxVertices ) << ",\n"     // maximum number of vertices,      GLSL: layout(max_vertices=X) out;
                "\t\t" << ToString( _meshMaxPrimitives ) << ",\n"   // maximum number of primitives,    GLSL: layout(max_primitives=X) out;
                "\t\t";

        switch ( _meshTopology )
        {
            case EPrimitive::Point :        str << "metal::topology::point";    break;
            case EPrimitive::LineList :     str << "metal::topology::line";     break;
            case EPrimitive::TriangleList : str << "metal::topology::triangle"; break;
            default :                       CHECK_THROW_MSG( false, "unsupported topology" );
        }

        str <<  "\n"    // topology of the mesh,            GLSL: layout(triangles) out;
                "\t>;\n";
        return str;
    }

/*
=================================================
    ThreadgroupsMSL
=================================================
*/
    String  ScriptShader::ThreadgroupsMSL (ArrayView<ScriptFeatureSetPtr> features) C_Th___
    {
        String  str;

        // TODO
        //if ( type == EShader::MeshTask )
        //{
        //  str << "[[max_total_threadgroups_per_mesh_grid(" << ToString() << ")]] \n";
        //}

        if ( AnyEqual( type, EShader::Compute, EShader::Tile, EShader::MeshTask, EShader::Mesh ))
        {
            uint    count = GetMaxValueFromFeatures( features, &FeatureSet::maxComputeWorkGroupInvocations );

            if ( All( _defaultLocalSize != ~0u ))
                count = _defaultLocalSize.x * _defaultLocalSize.y * _defaultLocalSize.z;

            str << "[[max_total_threads_per_threadgroup(" << ToString( count ) << ")]] \n";
        }
        return str;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptShader::Bind (const ScriptEnginePtr &se) __Th___
    {
        {
            EnumBinder<ETessPatch>  binder{ se };
            binder.Create();
            binder.AddValue( "Points",      ETessPatch::Points );
            binder.AddValue( "Isolines",    ETessPatch::Isolines );
            binder.AddValue( "Triangles",   ETessPatch::Triangles );
            binder.AddValue( "Quads",       ETessPatch::Quads );
            STATIC_ASSERT( uint(ETessPatch::_Count) == 5 );
        }{
            EnumBinder<ETessSpacing>    binder{ se };
            binder.Create();
            binder.AddValue( "Equal",           ETessSpacing::Equal );
            binder.AddValue( "FractionalEven",  ETessSpacing::FractionalEven );
            binder.AddValue( "FractionalOdd",   ETessSpacing::FractionalOdd );
            STATIC_ASSERT( uint(ETessSpacing::_Count) == 4 );
        }

        ClassBinder<ScriptShader>   binder{ se };
        binder.CreateRef();

        binder.Comment( "Set shader source in script." );
        binder.AddProperty( &ScriptShader::_source,     "source"    );

        binder.Comment( "Load shader source from file." );
        binder.AddProperty( &ScriptShader::_filename,   "file"      );
    //  binder.AddProperty( &ScriptShader::_entry,      "entry"     );
    //  binder.AddProperty( &ScriptShader::_defines,    "defines"   );

        binder.Comment( "Set custom shader version.\n"
                        "By default used value from 'GlobalConfig::SetShaderVersion()'." );
        binder.AddProperty( &ScriptShader::version,     "version"   );

        binder.Comment( "Set shader options.\n"
                        "By default used value from 'GlobalConfig::SetShaderOptions()'." );
        binder.AddProperty( &ScriptShader::options,     "options"   );

        binder.Comment( "Set shader type.\n"
                        "Optional for compute/graphics/mesh pipelines, required for ray tracing pipeline." );
        binder.AddProperty( &ScriptShader::type,        "type"      );

        binder.Comment( "Add specialization constant.\n" );
        binder.AddMethod( &ScriptShader::AddSpec,                   "AddSpec",                  {"valueType", "name"} );

        binder.Comment( "Add macros which will be used in shader.\n"
                        "Format: MACROS = value; DEF" );
        binder.AddMethod( &ScriptShader::Define,                    "Define",                   {} );

        binder.Comment( "Load shader source from current file." );
        binder.AddMethod( &ScriptShader::LoadSelf,                  "LoadSelf",                 {} );

        binder.Comment( "Add specialization constant for dynamic workgroup size.\n"
                        "Only for compute shader. Must be explicitly specialized by 'ComputePipelineSpec::SetLocalSize()'." );
        binder.AddMethod( &ScriptShader::SetComputeSpec1,           "ComputeSpec1",             {} );
        binder.AddMethod( &ScriptShader::SetComputeSpec2,           "ComputeSpec2",             {} );
        binder.AddMethod( &ScriptShader::SetComputeSpec3,           "ComputeSpec3",             {} );

        binder.Comment( "Add specialization constant for dynamic workgroup size.\n"
                        "Only for compute shader. Can be explicitly specialized by 'ComputePipelineSpec::SetLocalSize()', otherwise default value will be used" );
        binder.AddMethod( &ScriptShader::SetComputeSpecAndDefault1, "ComputeSpecAndDefault",    {"x"} );
        binder.AddMethod( &ScriptShader::SetComputeSpecAndDefault2, "ComputeSpecAndDefault",    {"x", "y"} );
        binder.AddMethod( &ScriptShader::SetComputeSpecAndDefault3, "ComputeSpecAndDefault",    {"x", "y", "z"} );

        binder.Comment( "Set constant workgroup size. Only for compute shader." );
        binder.AddMethod( &ScriptShader::SetComputeLocalSize1,      "ComputeLocalSize",         {"x"} );
        binder.AddMethod( &ScriptShader::SetComputeLocalSize2,      "ComputeLocalSize",         {"x", "y"} );
        binder.AddMethod( &ScriptShader::SetComputeLocalSize3,      "ComputeLocalSize",         {"x", "y", "z"} );

        binder.Comment( "Add specialization constant for dynamic workgroup size.\n"
                        "Only for mesh/task shader. Must be explicitly specialized by 'MeshPipelineSpec::SetLocalSize()'." );
        binder.AddMethod( &ScriptShader::SetMeshSpec1,              "MeshSpec1",                {} );
        binder.AddMethod( &ScriptShader::SetMeshSpec2,              "MeshSpec2",                {} );
        binder.AddMethod( &ScriptShader::SetMeshSpec3,              "MeshSpec3",                {} );

        binder.Comment( "Add specialization constant for dynamic workgroup size.\n"
                        "Only for mesh/task shader. Can be explicitly specialized by 'MeshPipelineSpec::SetLocalSize()', otherwise default value will be used" );
        binder.AddMethod( &ScriptShader::SetMeshSpecAndDefault1,    "MeshSpecAndDefault",       {"x"} );
        binder.AddMethod( &ScriptShader::SetMeshSpecAndDefault2,    "MeshSpecAndDefault",       {"x", "y"} );
        binder.AddMethod( &ScriptShader::SetMeshSpecAndDefault3,    "MeshSpecAndDefault",       {"x", "y", "z"} );

        binder.Comment( "Set constant workgroup size. Only for mesh/task shader." );
        binder.AddMethod( &ScriptShader::SetMeshLocalSize1,         "MeshLocalSize",            {"x"} );
        binder.AddMethod( &ScriptShader::SetMeshLocalSize2,         "MeshLocalSize",            {"x", "y"} );
        binder.AddMethod( &ScriptShader::SetMeshLocalSize3,         "MeshLocalSize",            {"x", "y", "z"} );

        binder.Comment( "Set size and topology for mesh shader output." );
        binder.AddMethod( &ScriptShader::SetMeshOutput,             "MeshOutput",               {"maxVertices", "maxPrimitives", "primitive"} );

        binder.Comment( "Set number of vertices in tessellation patch.\n"
                        "Only for graphics pipeline with tessellation shader." );
        binder.AddMethod( &ScriptShader::SetTessPatchSize,          "TessPatchSize",            {"vertexCount"} );

        binder.Comment( "Set tessellation mode.\n"
                        "Only for graphics pipeline with tessellation shader." );
        binder.AddMethod( &ScriptShader::SetTessPatchMode,          "TessPatchMode",            {"mode", "spacing", "ccw"} );
    }

} // AE::PipelineCompiler
