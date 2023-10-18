// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Alignment rules:
        std140  - https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
        std430  - https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shader_storage_buffer_object.txt
*/

#include "ScriptObjects/ShaderStructType.h"
#include "ScriptObjects/Common.inl.h"
#include "Compiler/MetalCompiler.h"

namespace AE::PipelineCompiler
{
namespace
{
    static void  ArraySize_Ctor (OUT void* mem, uint value) {
        PlacementNew<ArraySize>( OUT mem, value );
    }

    static void  Align_Ctor (OUT void* mem, uint value) {
        PlacementNew<Align>( OUT mem, value );
    }

    static ShaderStructType*  ShaderStructType_Ctor (const String &name) {
        return ShaderStructTypePtr{ new ShaderStructType{ name }}.Detach();
    }

    ND_ static String  ValidateTypeName (const String &name)
    {
        String  str = name;
        FindAndReplace( INOUT str, ".", "_" );
        FindAndReplace( INOUT str, ":", "_" );
        FindAndReplace( INOUT str, "-", "_" );
        FindAndReplace( INOUT str, "/", "_" );
        return str;
    }

/*
=================================================
    IsStd***
=================================================
*/
    ND_ static bool  IsStd140 (EStructLayout layout)
    {
        return AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140 );
    }

    ND_ static bool  IsStd430 (EStructLayout layout)
    {
        return AnyEqual( layout, EStructLayout::Compatible_Std430, EStructLayout::Std430 );
    }

/*
=================================================
    IsGLSLCompatible
=================================================
*/
    ND_ static bool  IsGLSLCompatible (EStructLayout layout)
    {
        return AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430, EStructLayout::Std140, EStructLayout::Std430 );
    }

/*
=================================================
    IsMSLCompatible
=================================================
*/
    ND_ static bool  IsMSLCompatible (EStructLayout layout)
    {
        return AnyEqual( layout, EStructLayout::Metal, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430 );
    }

/*
=================================================
    IsSameLayouts
=================================================
*/
    ND_ static bool  IsSameLayouts (EStructLayout lhs, EStructLayout rhs)
    {
        return  IsStd140( lhs ) == IsStd140( rhs )  or
                IsStd430( lhs ) == IsStd430( rhs )  or
                (lhs == EStructLayout::InternalIO) == (rhs == EStructLayout::InternalIO);
    }

/*
=================================================
    EValueType_ToString
=================================================
*/
    ND_ static StringView  EValueType_ToString (EValueType type)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EValueType::Bool8 :        return "Bool8";
            case EValueType::Bool32 :       return "Bool32";
            case EValueType::Int8 :         return "Int8";
            case EValueType::Int16 :        return "Int16";
            case EValueType::Int32 :        return "Int32";
            case EValueType::Int64 :        return "Int64";
            case EValueType::UInt8 :        return "UInt8";
            case EValueType::UInt16 :       return "UInt16";
            case EValueType::UInt32 :       return "UInt32";
            case EValueType::UInt64 :       return "UInt64";
            case EValueType::Float16 :      return "Float16";
            case EValueType::Float32 :      return "Float32";
            case EValueType::Float64 :      return "Float64";
            case EValueType::Int8_Norm :    return "Int8_Norm";
            case EValueType::Int16_Norm :   return "Int16_Norm";
            case EValueType::UInt8_Norm :   return "UInt8_Norm";
            case EValueType::UInt16_Norm :  return "UInt16_Norm";
            case EValueType::DeviceAddress: return "DeviceAddress";
            case EValueType::Unknown :
            case EValueType::_Count :       break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown EValueType" );
    }

/*
=================================================
    EStructLayout_ToString
=================================================
*/
    ND_ static StringView  EStructLayout_ToString (EStructLayout type)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EStructLayout::Compatible_Std140 : return "Compatible_Std140";
            case EStructLayout::Compatible_Std430 : return "Compatible_Std430";
            case EStructLayout::Metal :             return "Metal";
            case EStructLayout::Std140 :            return "Std140";
            case EStructLayout::Std430 :            return "Std430";
            case EStructLayout::InternalIO :        return "InternalIO";
            case EStructLayout::_Count :
            case EStructLayout::Unknown :           break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown EStructLayout" );
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  ArraySize::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ArraySize>  binder{ se };
        binder.CreateClassValue();
        binder.AddConstructor( &ArraySize_Ctor, {} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Bind
=================================================
*/
    void  Align::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<Align>  binder{ se };
        binder.CreateClassValue();
        binder.AddConstructor( &Align_Ctor, {} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Field::operator ==
=================================================
*/
    bool  ShaderStructType::Field::operator == (const Field &rhs) const
    {
        return  name        == rhs.name         and
                type        == rhs.type         and
                stType      == rhs.stType       and
                arraySize   == rhs.arraySize    and
                rows        == rhs.rows         and
                cols        == rhs.cols         and
                packed      == rhs.packed       and
                size        == rhs.size         and
                align       == rhs.align        and
                offset      == rhs.offset;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ShaderStructType::Constants
=================================================
*/
    ShaderStructType::Constants::Constants () :
        typeNames{
            { "bool",           { EValueType::Bool8,            1,  1 }},
            { "lbool",          { EValueType::Bool32,           4,  4 }},
            { "sbyte",          { EValueType::Int8,             1,  1 }},
            { "ubyte",          { EValueType::UInt8,            1,  1 }},
            { "sshort",         { EValueType::Int16,            2,  2 }},
            { "ushort",         { EValueType::UInt16,           2,  2 }},
            { "sint",           { EValueType::Int32,            4,  4 }},
            { "uint",           { EValueType::UInt32,           4,  4 }},
            { "slong",          { EValueType::Int64,            8,  8 }},
            { "ulong",          { EValueType::UInt64,           8,  8 }},
            { "DeviceAddress",  { EValueType::DeviceAddress,    8,  8 }},   // uvec2
            { "half",           { EValueType::Float16,          2,  2 }},
            { "float",          { EValueType::Float32,          4,  4 }},
            { "double",         { EValueType::Float64,          8,  8 }},
            { "sbyte_norm",     { EValueType::Int8_Norm,        1,  1 }},
            { "ubyte_norm",     { EValueType::UInt8_Norm,       1,  1 }},
            { "sshort_norm",    { EValueType::Int16_Norm,       2,  2 }},
            { "ushort_norm",    { EValueType::UInt16_Norm,      2,  2 }},
        },
        renameMap{
            {"int8_t",      "sbyte"},   {"byte",    "sbyte"},   {"char",    "sbyte"},
            {"uint8_t",     "ubyte"},   {"uchar",   "ubyte"},
            {"int16_t",     "sshort"},  {"short",   "sshort"},
            {"uint16_t",    "ushort"},
            {"int32_t",     "sint"},    {"int",     "sint"},
            {"uint32_t",    "uint"},
            {"int64_t",     "slong"},   {"long",    "slong"},   {"ssize",   "slong"},   {"ptrdiff_t",   "slong"},
            {"uint64_t",    "ulong"},   {"usize",   "ulong"},   {"size_t",  "ulong"},
            {"float16_t",   "half"},
            {"float32_t",   "float"},
            {"float64_t",   "double"},
            {"ivec",        "sint"},    {"uvec",    "uint"},
            {"i8vec",       "sbyte"},   {"u8vec",   "ubyte"},
            {"i16vec",      "sshort"},  {"u16vec",  "ushort"},
            {"i32vec",      "sint"},    {"u32vec",  "uint"},
            {"i64vec",      "slong"},   {"u64vec",  "ulong"},
            {"f16vec",      "half"},    {"f16mat",  "half"},
            {"f32vec",      "float"},   {"f32mat",  "float"},   {"vec",     "float"},   {"mat",     "float"},
            {"f64vec",      "double"},  {"f64mat",  "double"},  {"dvec",    "double"},  {"dmat",    "double"},

            // _norm
            {"char_norm",   "sbyte_norm"},
            {"uchar_norm",  "ubyte_norm"},
            {"short_norm",  "sshort_norm"},
            // norm_
            {"norm_sbyte",  "sbyte_norm"},  {"norm_byte",   "sbyte_norm"},  {"norm_char",   "sbyte_norm"},
            {"norm_ubyte",  "ubyte_norm"},  {"norm_uchar",  "ubyte_norm"},
            {"norm_sshort", "sshort_norm"}, {"norm_short",  "sshort_norm"},
            {"norm_ushort", "ushort_norm"},
        }
    {}

/*
=================================================
    _ParseFields
=================================================
*/
    void  ShaderStructType::_ParseFields (const String &fields, OUT Array<Field> &outFields) __Th___
    {
        const auto&         const_ptr   = ObjectStorage::Instance()->_structTypeConstPtr;
        const auto&         c_typeNames = const_ptr->typeNames;
        const auto&         c_renameMap = const_ptr->renameMap;
        const StringView    c_packed    = "packed_";
        const Bytes         c_ptrSize   = 8_b;
        const Bytes         c_ptrAlign  = 8_b;

        const auto  IsTypeNameStart = [] (char c) -> bool
        {{
            return ((c >= 'a') & (c <= 'z')) | ((c >= 'A') & (c <= 'Z')) | (c == '_');
        }};
        const auto  IsNumber = [] (char c) -> bool
        {{
            return (c >= '0') & (c <= '9');
        }};

        Array<StringView>   tokens;

        const auto  ReadNameAndArray = [&tokens] (Array<StringView>::iterator &it, INOUT Field &field) __Th___
        {{
            field.name = String{*it};

            for (auto c : field.name) {
                if ( not ( ((c >= 'a') & (c <= 'z')) | ((c >= 'A') & (c <= 'Z')) | ((c >= '0') & (c <= '9')) | (c == '_') ))
                    CHECK_THROW_MSG( false, "invalid name: '"s << field.name << "'" );
            }
            ++it;

            // non-array
            if ( it == tokens.end() )
            {
                field.arraySize = 0;
                return;
            }

            if ( *it == "[" )
            {
                ++it;
                CHECK_THROW_MSG( it != tokens.end(), "failed to parse ShaderStructType" );

                // dynamic array
                if ( *it == "]" )
                {
                    field.arraySize = UMax;
                    ++it;
                    CHECK_THROW_MSG( it == tokens.end(), "failed to parse ShaderStructType" );
                    return;
                }

                for (auto c : *it) {
                    if ( not ( (c >= '0') & (c <= '9') ))
                        CHECK_THROW_MSG( false, "invalid array size: '"s << *it << "'" );
                }

                field.arraySize = StringToUInt( *it );
                ASSERT( field.arraySize != 0 and field.arraySize != UMax );

                ++it;
                CHECK_THROW_MSG( it != tokens.end(), "failed to parse ShaderStructType" );
                CHECK_THROW_MSG( *it == "]", "failed to parse ShaderStructType" );
                ++it;
                CHECK_THROW_MSG( it == tokens.end(), "failed to parse ShaderStructType" );
                return;
            }

            CHECK_THROW_MSG( false, "can't parse token: '"s << *it << "'" );
        }};

        for (usize pos = 0;;)
        {
            StringView  line;
            {
                const usize     begin       = pos;
                const usize     semicolon   = fields.find( ';',  begin );
                const usize     comment     = fields.find( "//", begin );

                if ( semicolon == String::npos and comment == String::npos )
                    break;

                pos     = Min( semicolon, comment );
                line    = StringView{fields}.substr( begin, pos - begin );

                if ( pos == semicolon )     pos++;      else
                if ( pos == comment )       Parser::ToNextLine( fields, INOUT pos );
            }

            Parser::DivideString_CPP( line, OUT tokens );
            if ( tokens.empty() )
                continue;

            auto it = tokens.begin();
            if ( IsTypeNameStart( it->front() ))
            {
                StringView  type_name   = *it;
                bool        is_packed   = false;

                if ( StartsWith( type_name, c_packed ))
                {
                    is_packed = true;
                    type_name = type_name.substr( c_packed.length() );
                }

                uint    rows        = 1;
                uint    cols        = 1;
                bool    has_rows    = false;
                bool    has_cols    = false;

                // vector or matrix
                if ( IsNumber( type_name.back() ))
                {
                    rows        = uint(type_name.back() - '0');
                    type_name   = type_name.substr( 0, type_name.size()-1 );
                    has_rows    = true;
                }

                // matrix
                if ( has_rows and type_name.back() == 'x' and IsNumber( type_name[type_name.size()-2] ))
                {
                    cols        = uint(type_name[type_name.size()-2] - '0');
                    type_name   = type_name.substr( 0, type_name.size()-2 );
                    has_cols    = true;
                }

                // remap type
                {
                    auto    rm_it = c_renameMap.find( type_name );
                    if ( rm_it != c_renameMap.end() )
                        type_name = rm_it->second;
                }

                // find in standard types
                {
                    auto    tn_it = c_typeNames.find( type_name );
                    if ( tn_it != c_typeNames.end() )
                    {
                        if ( is_packed ) {
                            CHECK_THROW_MSG( rows > 1, "packed type is not supported for scalar" );
                        }
                        if ( has_rows ) {
                            CHECK_THROW_MSG( rows >= 2 and rows <= 4, "failed to parse ShaderStructType" );
                        }
                        if ( has_cols ) {
                            CHECK_THROW_MSG( rows >= 2 and rows <= 4, "failed to parse ShaderStructType" );
                            CHECK_THROW_MSG( cols >= 2 and cols <= 4, "failed to parse ShaderStructType" );
                        }
                        ++it;

                        Field&  field   = outFields.emplace_back();
                        field.type      = tn_it->second.type;
                        field.align     = Bytes{tn_it->second.align};
                        field.size      = Bytes{tn_it->second.size};
                        field.rows      = ubyte(rows);
                        field.cols      = ubyte(cols);
                        field.packed    = is_packed;
                        field.pointer   = (*it == "*");
                        field.address   = field.pointer;

                        if ( field.pointer )
                        {
                            ++it;
                            field.align = c_ptrSize;
                            field.size  = c_ptrAlign;
                        }

                        ReadNameAndArray( it, INOUT field );
                        CHECK_THROW_MSG( it == tokens.end(), "failed to parse ShaderStructType" );
                        continue;
                    }
                }

                // TODO: atomic<>
                // TODO: simdgroup_float8x8
                // TODO: r8unorm<>, ...

                // custom type
                type_name = *it;
                ++it;

                const bool  is_ref       = (*it == "&");
                const auto& struct_types = ObjectStorage::Instance()->structTypes;
                auto        st_it        = struct_types.find( String{type_name} );
                CHECK_THROW_MSG( st_it != struct_types.end(),
                    "unknown typename: '"s << type_name << "'" );

                Field&  field   = outFields.emplace_back();

                if ( is_ref )
                {
                    ++it;
                    st_it->second->AddUsage( EUsage::BufferReference );

                    CHECK_THROW_MSG( not st_it->second->HasDynamicArray(),
                        "buffer reference with dynamic array is not supported, use pointer to <dynamic_array_element_type> instead" );

                    field.stType    = st_it->second;
                    field.align     = c_ptrSize;
                    field.size      = c_ptrAlign;
                    field.type      = EValueType::DeviceAddress;
                    field.address   = true;
                }
                else
                {
                    CHECK_THROW_MSG( not st_it->second->HasDynamicArray(),
                        "struct field with dynamic array is not supported" );

                    field.stType    = st_it->second;
                    field.align     = field.stType->_align;
                    field.size      = field.stType->_size;
                    field.pointer   = (*it == "*");
                    field.address   = field.pointer;

                    if ( field.pointer )
                    {
                        ++it;
                        field.align = c_ptrSize;
                        field.size  = c_ptrAlign;
                    }
                }

                ReadNameAndArray( it, INOUT field );
                CHECK_THROW_MSG( it == tokens.end(),
                    "failed to parse ShaderStructType, line: '"s << line << "'" );
                continue;
            }

            if ( *it == "//" )
            {
                Parser::ToNextLine( fields, INOUT pos );
                continue;
            }

            CHECK_THROW_MSG( false, "failed to parse token: '"s << *it << "'" );
        }
    }

/*
=================================================
    _CalcOffsets
=================================================
*/
    void  ShaderStructType::_CalcOffsets (StringView stName, EStructLayout layout, INOUT Array<Field> &fields,
                                          OUT Bytes &maxAlign, OUT Bytes &structAlign, OUT Bytes &totalSize) __Th___
    {
        maxAlign    = 0_b;
        structAlign = 0_b;
        totalSize   = 0_b;

        for (usize i = 0; i < fields.size(); ++i)
        {
            auto&   field = fields[i];

            if ( i+1 < fields.size() ) {
                CHECK_THROW_MSG( not field.IsDynamicArray(),
                    "In Struct '"s << stName << "', field '" << field.name << "': only last field can be dynamic array" );
            }

            if ( field.IsDeviceAddress() )
            {
                // skip
            }
            else
            if ( field.IsStruct() )
            {
                BEGIN_ENUM_CHECKS();
                switch ( layout )
                {
                    case EStructLayout::Compatible_Std140 :
                    {
                        // Metal does not support custom align
                        if ( field.IsArray() ) {
                            CHECK_THROW_MSG( IsMultipleOf( field.align, 16 ) or IsMultipleOf( field.size, 16 ),
                                "In Struct '"s << stName << "', field '" << field.name << "': align (" << ToString(uint(field.align)) <<
                                ") and size (" << ToString(uint(field.size)) << ") must be aligned to 16 bytes" );
                        }
                        break;
                    }
                    case EStructLayout::Std140 :
                    {
                        if ( field.IsArray() )
                            field.align = Max( field.align, 16_b );
                        break;
                    }
                    case EStructLayout::Compatible_Std430 : break;
                    case EStructLayout::Metal :             break;
                    case EStructLayout::Std430 :            break;
                    case EStructLayout::InternalIO :        break;
                    case EStructLayout::_Count :
                    case EStructLayout::Unknown :
                    default :                               CHECK_THROW_MSG( false, "unknown layout type" );
                }
                END_ENUM_CHECKS();
            }
            else
            {
                field.size *= field.rows;

                BEGIN_ENUM_CHECKS();
                switch ( layout )
                {
                    case EStructLayout::Compatible_Std140 :
                    {
                        CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool8, EValueType::Bool32, EValueType::Float64 ),
                            "In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
                            "must not be Bool8/Bool32/Float64" );

                        if ( field.IsMat() and not IsMultipleOf( field.size, 16 ))
                            field.packed = true;    // set 'packed' for compatibility with Metal

                        if ( not field.packed and field.rows > 1 )
                            field.align *= (field.rows == 3 or field.IsMat() ? 4 : field.rows);

                        field.size = AlignUp( field.size, field.align );

                        if ( field.IsMat() )
                            field.size *= field.cols;

                        // Metal does not support custom align for arrays
                        if ( field.IsArray() )
                        {
                            CHECK_THROW_MSG( IsMultipleOf( field.align, 16 ) or IsMultipleOf( field.size, 16 ),
                                "In Struct '"s << stName << "', field '" << field.name << "': align (" << ToString(uint(field.align)) <<
                                ") and size (" << ToString(uint(field.size)) << ") must be aligned to 16 bytes" );
                            field.align = Max( field.align, 16_b );
                        }
                        break;
                    }

                    case EStructLayout::Compatible_Std430 :
                    case EStructLayout::InternalIO :
                    {
                        CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool8, EValueType::Bool32, EValueType::Float64 ),
                            "In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
                            "must not be Bool8/Bool32/Float64" );

                        if ( not field.packed and field.rows > 1 )
                            field.align *= (field.rows == 3 ? 4 : field.rows);

                        field.size = AlignUp( field.size, field.align );

                        if ( field.IsMat() )
                            field.size *= field.cols;
                        break;
                    }

                    case EStructLayout::Metal :
                    {
                        CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool32, EValueType::Float64 ),
                            "In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
                            "must not be Bool32/Float64" );

                        if ( not field.packed and field.rows > 1 )
                            field.align *= (field.rows == 3 ? 4 : field.rows);

                        field.size = AlignUp( field.size, field.align );

                        if ( field.IsMat() )
                            field.size *= field.cols;
                        break;
                    }

                    case EStructLayout::Std140 :
                    {
                        CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool8 ),
                            "In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
                            "must not be Bool8" );

                        if ( not field.packed and field.rows > 1 )
                            field.align *= (field.rows == 3 or field.IsMat() ? 4 : field.rows);

                        if ( field.IsMat() or field.IsArray() )
                            field.align = Max( field.align, 16_b );

                        field.size = AlignUp( field.size, field.align );

                        if ( field.IsMat() )
                            field.size *= field.cols;
                        break;
                    }

                    case EStructLayout::Std430 :
                    {
                        CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool8 ),
                            "In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
                            "must not be Bool8" );

                        if ( not field.packed and field.rows > 1 )
                            field.align *= (field.rows == 3 ? 4 : field.rows);

                        field.size = AlignUp( field.size, field.align );

                        if ( field.IsMat() )
                            field.size *= field.cols;
                        break;
                    }

                    case EStructLayout::_Count :
                    case EStructLayout::Unknown :
                    default :
                        CHECK_THROW_MSG( false, "unknown layout type" );
                }
                END_ENUM_CHECKS();
            }

            if ( field.IsArray() )
                field.size = AlignUp( field.size, field.align );

            if ( field.IsStaticArray() )
                field.size *= field.arraySize;

            maxAlign        = Max( maxAlign, field.align );
            field.offset    = AlignUp( totalSize, field.align );

            if ( field.IsDynamicArray() ){
                CHECK_THROW_MSG( i+1 == fields.size() );
            }else{
                totalSize = field.offset + field.size;
            }
            CHECK_THROW_MSG( IsPowerOfTwo( maxAlign ));
        }

        structAlign = maxAlign;

        if ( IsStd140( layout ))
        {
            // structure aligned to 16 bytes
            structAlign = Max( structAlign, 16_b );
        }
    }

/*
=================================================
    _GetCPPSizeAndAlign
=================================================
*/
    SizeAndAlign  ShaderStructType::_GetCPPSizeAndAlign2 (const Field &field) __Th___
    {
    #define SWITCH_TYPE( _prefix_, _suffix_, ... )                                                      \
        {                                                                                               \
            BEGIN_ENUM_CHECKS();                                                                        \
            switch ( field.type )                                                                       \
            {                                                                                           \
                case EValueType::Bool32 :       return SizeAndAlignOf<_prefix_ ## lbool  ## _suffix_>;  \
                case EValueType::Int32 :        return SizeAndAlignOf<_prefix_ ## int    ## _suffix_>;  \
                case EValueType::UInt32 :       return SizeAndAlignOf<_prefix_ ## uint   ## _suffix_>;  \
                case EValueType::Float32 :      return SizeAndAlignOf<_prefix_ ## float  ## _suffix_>;  \
                case EValueType::Int64 :        return SizeAndAlignOf<_prefix_ ## slong  ## _suffix_>;  \
                case EValueType::UInt64 :       return SizeAndAlignOf<_prefix_ ## ulong  ## _suffix_>;  \
                case EValueType::Float64 :      return SizeAndAlignOf<_prefix_ ## double ## _suffix_>;  \
                case EValueType::Bool8 :        return SizeAndAlignOf<_prefix_ ## bool   ## _suffix_>;  \
                case EValueType::Int8 :                                                                 \
                case EValueType::Int8_Norm :    return SizeAndAlignOf<_prefix_ ## sbyte  ## _suffix_>;  \
                case EValueType::UInt8 :                                                                \
                case EValueType::UInt8_Norm :   return SizeAndAlignOf<_prefix_ ## ubyte  ## _suffix_>;  \
                case EValueType::Int16 :                                                                \
                case EValueType::Int16_Norm :   return SizeAndAlignOf<_prefix_ ## short  ## _suffix_>;  \
                case EValueType::UInt16 :                                                               \
                case EValueType::UInt16_Norm :  return SizeAndAlignOf<_prefix_ ## ushort ## _suffix_>;  \
                case EValueType::Float16 :      return SizeAndAlignOf<_prefix_ ## half   ## _suffix_>;  \
                case EValueType::Unknown :                                                              \
                case EValueType::_Count :                                                               \
                __VA_ARGS__                                                                             \
                default :                       CHECK_THROW_MSG( false, "unknown value type" );         \
            }                                                                                           \
            END_ENUM_CHECKS();                                                                          \
        }
    #define SWITCH_MAT_TYPE( _prefix_, _suffix_ )                                                       \
        {                                                                                               \
            BEGIN_ENUM_CHECKS();                                                                        \
            switch ( field.type )                                                                       \
            {                                                                                           \
                case EValueType::Float32 :      return SizeAndAlignOf<_prefix_ ## float  ## _suffix_>;  \
                case EValueType::Float64 :      return SizeAndAlignOf<_prefix_ ## double ## _suffix_>;  \
                case EValueType::Float16 :      return SizeAndAlignOf<_prefix_ ## half   ## _suffix_>;  \
                case EValueType::Bool32 :                                                               \
                case EValueType::Int32 :                                                                \
                case EValueType::UInt32 :                                                               \
                case EValueType::Int64 :                                                                \
                case EValueType::DeviceAddress:                                                         \
                case EValueType::UInt64 :                                                               \
                case EValueType::Bool8 :                                                                \
                case EValueType::Int8 :                                                                 \
                case EValueType::UInt8 :                                                                \
                case EValueType::Int16 :                                                                \
                case EValueType::UInt16 :                                                               \
                case EValueType::Int8_Norm :                                                            \
                case EValueType::UInt8_Norm :                                                           \
                case EValueType::Int16_Norm :                                                           \
                case EValueType::UInt16_Norm :  CHECK_THROW_MSG( false, "unsupported value type" );     \
                case EValueType::Unknown :                                                              \
                case EValueType::_Count :                                                               \
                default :                       CHECK_THROW_MSG( false, "unknown value type" );         \
            }                                                                                           \
            END_ENUM_CHECKS();                                                                          \
        }

        CHECK_THROW_MSG( not (field.IsStruct() or field.IsDeviceAddress()) );

        if ( field.IsScalar() )
        {
            SWITCH_TYPE( , ,
                case EValueType::DeviceAddress : return SizeAndAlignOf< Graphics::DeviceAddress >;
            );
        }
        else
        if ( field.IsVec() and field.packed )
        {
            switch ( field.rows )
            {
                case 2 :    SWITCH_TYPE( packed_, 2,  case EValueType::DeviceAddress: );    break;
                case 3 :    SWITCH_TYPE( packed_, 3,  case EValueType::DeviceAddress: );    break;
                case 4 :    SWITCH_TYPE( packed_, 4,  case EValueType::DeviceAddress: );    break;
                default :   CHECK_THROW_MSG( false, "unsupported vector size" );
            }
        }
        else
        if ( field.IsVec() and not field.packed )
        {
            switch ( field.rows )
            {
                case 2 :    SWITCH_TYPE( , 2,  case EValueType::DeviceAddress: );   break;
                case 3 :    SWITCH_TYPE( , 3,  case EValueType::DeviceAddress: );   break;
                case 4 :    SWITCH_TYPE( , 4,  case EValueType::DeviceAddress: );   break;
                default :   CHECK_THROW_MSG( false, "unsupported vector size" );
            }
        }
        else
        if ( field.IsMat() and field.packed )
        {
            switch ( uint(field.cols)*10 + uint(field.rows) )
            {
                case 22 :   SWITCH_MAT_TYPE( packed_, 2x2_storage );    break;
                case 23 :   SWITCH_MAT_TYPE( packed_, 2x3_storage );    break;
                case 24 :   SWITCH_MAT_TYPE( packed_, 2x4_storage );    break;
                case 32 :   SWITCH_MAT_TYPE( packed_, 3x2_storage );    break;
                case 33 :   SWITCH_MAT_TYPE( packed_, 3x3_storage );    break;
                case 34 :   SWITCH_MAT_TYPE( packed_, 3x4_storage );    break;
                case 42 :   SWITCH_MAT_TYPE( packed_, 4x2_storage );    break;
                case 43 :   SWITCH_MAT_TYPE( packed_, 4x3_storage );    break;
                case 44 :   SWITCH_MAT_TYPE( packed_, 4x4_storage );    break;
                default :   CHECK_THROW_MSG( false, "unsupported matrix size" );
            }
        }
        else
        if ( field.IsMat() and not field.packed )
        {
            switch ( uint(field.cols)*10 + uint(field.rows) )
            {
                case 22 :   SWITCH_MAT_TYPE( , 2x2_storage );   break;
                case 23 :   SWITCH_MAT_TYPE( , 2x3_storage );   break;
                case 24 :   SWITCH_MAT_TYPE( , 2x4_storage );   break;
                case 32 :   SWITCH_MAT_TYPE( , 3x2_storage );   break;
                case 33 :   SWITCH_MAT_TYPE( , 3x3_storage );   break;
                case 34 :   SWITCH_MAT_TYPE( , 3x4_storage );   break;
                case 42 :   SWITCH_MAT_TYPE( , 4x2_storage );   break;
                case 43 :   SWITCH_MAT_TYPE( , 4x3_storage );   break;
                case 44 :   SWITCH_MAT_TYPE( , 4x4_storage );   break;
                default :   CHECK_THROW_MSG( false, "unsupported matrix size" );
            }
        }

        CHECK_THROW_MSG( false, "unknown field type" );

    #undef SWITCH_MAT_TYPE
    #undef SWITCH_TYPE
    }

    SizeAndAlign  ShaderStructType::_GetCPPSizeAndAlign (const Field &field, EStructLayout layout) __Th___
    {
        auto [size, align] = _GetCPPSizeAndAlign2( field );

        if ( field.IsDynamicArray() )
            size = 0_b;

        size = AlignUp( size, align );

        if ( IsStd140( layout ))
        {
            if ( field.IsStaticArray() )
            {
                align = Max( align, 16_b );
                size  = AlignUp( size, align );
                size  = AlignUp( size * field.arraySize, align );
            }
        }
        else
        {
            if ( field.IsStaticArray() )
                size = AlignUp( size * field.arraySize, align );
        }

        return SizeAndAlign{ size, align };
    }

/*
=================================================
    _GetMSLSizeAndAlign
=================================================
*/
    SizeAndAlign  ShaderStructType::_GetMSLSizeAndAlign2 (const Field &field) __Th___
    {
        if ( field.IsScalar() or (field.IsVec() and field.packed) or (field.IsMat() and field.packed) )
        {
            CHECK_THROW_MSG( field.rows >= 1 and field.rows <= 4 );
            CHECK_THROW_MSG( field.cols >= 1 and field.cols <= 4 );
            BEGIN_ENUM_CHECKS();
            switch ( field.type )
            {
                case EValueType::Bool8 :        return SizeAndAlign{ 1_b * field.rows * field.cols,  1_b };
                case EValueType::Int8 :
                case EValueType::Int8_Norm :    return SizeAndAlign{ 1_b * field.rows * field.cols,  1_b };
                case EValueType::UInt8 :
                case EValueType::UInt8_Norm :   return SizeAndAlign{ 1_b * field.rows * field.cols,  1_b };
                case EValueType::Int16 :
                case EValueType::Int16_Norm :   return SizeAndAlign{ 2_b * field.rows * field.cols,  2_b };
                case EValueType::UInt16 :
                case EValueType::UInt16_Norm :  return SizeAndAlign{ 2_b * field.rows * field.cols,  2_b };
                case EValueType::Float16 :      return SizeAndAlign{ 2_b * field.rows * field.cols,  2_b };
                case EValueType::Int32 :        return SizeAndAlign{ 4_b * field.rows * field.cols,  4_b };
                case EValueType::UInt32 :       return SizeAndAlign{ 4_b * field.rows * field.cols,  4_b };
                case EValueType::Float32 :      return SizeAndAlign{ 4_b * field.rows * field.cols,  4_b };
                case EValueType::Int64 :        return SizeAndAlign{ 8_b * field.rows * field.cols,  8_b };
                case EValueType::DeviceAddress: return SizeAndAlign{ 8_b * field.rows * field.cols,  8_b };
                case EValueType::UInt64 :       return SizeAndAlign{ 8_b * field.rows * field.cols,  8_b };
                case EValueType::Bool32 :
                case EValueType::Float64 :      CHECK_THROW_MSG( false, "unsupported value type" );
                case EValueType::Unknown :
                case EValueType::_Count :
                default :                       CHECK_THROW_MSG( false, "unknown value type" );
            }
            END_ENUM_CHECKS();
        }
        else
        if ( field.IsVec() or field.IsMat() )
        {
            CHECK_THROW_MSG( not field.packed );
            CHECK_THROW_MSG( field.rows >= 2 and field.rows <= 4 );
            CHECK_THROW_MSG( field.cols >= 1 and field.cols <= 4 );
            const uint  rows    = field.rows == 3 ? 4 : field.rows;
            const uint  count   = rows * field.cols;

            BEGIN_ENUM_CHECKS();
            switch ( field.type )
            {
                case EValueType::Bool8 :        return SizeAndAlign{ 1_b * count,  1_b * rows };
                case EValueType::Int8 :
                case EValueType::Int8_Norm :    return SizeAndAlign{ 1_b * count,  1_b * rows };
                case EValueType::UInt8 :
                case EValueType::UInt8_Norm :   return SizeAndAlign{ 1_b * count,  1_b * rows };
                case EValueType::Int16 :
                case EValueType::Int16_Norm :   return SizeAndAlign{ 2_b * count,  2_b * rows };
                case EValueType::UInt16 :
                case EValueType::UInt16_Norm :  return SizeAndAlign{ 2_b * count,  2_b * rows };
                case EValueType::Float16 :      return SizeAndAlign{ 2_b * count,  2_b * rows };
                case EValueType::Int32 :        return SizeAndAlign{ 4_b * count,  4_b * rows };
                case EValueType::UInt32 :       return SizeAndAlign{ 4_b * count,  4_b * rows };
                case EValueType::Float32 :      return SizeAndAlign{ 4_b * count,  4_b * rows };
                case EValueType::Int64 :        return SizeAndAlign{ 8_b * count,  8_b * rows };
                case EValueType::UInt64 :       return SizeAndAlign{ 8_b * count,  8_b * rows };
                case EValueType::Bool32 :
                case EValueType::Float64 :
                case EValueType::DeviceAddress: CHECK_THROW_MSG( false, "unsupported value type" );
                case EValueType::Unknown :
                case EValueType::_Count :
                default :                       CHECK_THROW_MSG( false, "unknown value type" );
            }
            END_ENUM_CHECKS();
        }

        CHECK_THROW_MSG( false, "unknown field type" );
    }

    SizeAndAlign  ShaderStructType::_GetMSLSizeAndAlign (const Field &field, EStructLayout layout) __Th___
    {
        auto [size, align] = _GetMSLSizeAndAlign2( field );

        if ( field.IsDynamicArray() )
            size = 0_b;

        size = AlignUp( size, align );

        if ( field.IsStaticArray() )
        {
            if ( IsStd140( layout ))
            {
                align = Max( align, 16_b );
                size  = AlignUp( size, align );
            }
            size = AlignUp( size * field.arraySize, align );
        }
        return SizeAndAlign{ size, align };
    }

/*
=================================================
    _GetGLSLSizeAndAlign
=================================================
*/
    SizeAndAlign  ShaderStructType::_GetGLSLSizeAndAlign2 (const Field &field) __Th___
    {
        BEGIN_ENUM_CHECKS();
        switch ( field.type )
        {
            case EValueType::Bool8 :        return SizeAndAlign{ 1_b,  1_b };
            case EValueType::Int8 :
            case EValueType::Int8_Norm :    return SizeAndAlign{ 1_b,  1_b };
            case EValueType::UInt8 :
            case EValueType::UInt8_Norm :   return SizeAndAlign{ 1_b,  1_b };
            case EValueType::Int16 :
            case EValueType::Int16_Norm :   return SizeAndAlign{ 2_b,  2_b };
            case EValueType::UInt16 :
            case EValueType::UInt16_Norm :  return SizeAndAlign{ 2_b,  2_b };
            case EValueType::Float16 :      return SizeAndAlign{ 2_b,  2_b };
            case EValueType::Int32 :        return SizeAndAlign{ 4_b,  4_b };
            case EValueType::UInt32 :       return SizeAndAlign{ 4_b,  4_b };
            case EValueType::Float32 :      return SizeAndAlign{ 4_b,  4_b };
            case EValueType::Bool32 :       return SizeAndAlign{ 4_b,  4_b };
            case EValueType::Int64 :        return SizeAndAlign{ 8_b,  8_b };
            case EValueType::DeviceAddress: return SizeAndAlign{ 8_b,  8_b };
            case EValueType::UInt64 :       return SizeAndAlign{ 8_b,  8_b };
            case EValueType::Float64 :      return SizeAndAlign{ 8_b,  8_b };
            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       CHECK_THROW_MSG( false, "unknown value type" );
        }
        END_ENUM_CHECKS();
    }

    SizeAndAlign  ShaderStructType::_GetGLSLSizeAndAlign (const Field &field, EStructLayout layout) __Th___
    {
        auto [size, align] = _GetGLSLSizeAndAlign2( field );

        if ( field.IsDynamicArray() )
            size = 0_b;

        if ( not field.packed )
            align *= (field.rows == 3 ? 4 : field.rows);

        size = AlignUp( size * field.rows, align );

        if ( IsStd140( layout ))
        {
            if ( not field.packed and field.IsMat() )
                align = Max( align, 16_b );

            size = AlignUp( size * field.cols, align );

            if ( field.IsStaticArray() )
            {
                align = Max( align, 16_b );
                size  = AlignUp( size, align );
                size  = AlignUp( size * field.arraySize, align );
            }
        }
        else
        {
            size = AlignUp( size * field.cols, align );

            if ( field.IsStaticArray() )
                size = AlignUp( size * field.arraySize, align );
        }

        return SizeAndAlign{ size, align };
    }

/*
=================================================
    _ValidateOffsets
=================================================
*/
    void  ShaderStructType::_ValidateOffsets (const ValidationData &data, Bytes offset) __Th___
    {
        offset += data.baseOffset;

        BEGIN_ENUM_CHECKS();
        switch ( data.layout )
        {
            case EStructLayout::Compatible_Std140 :
            case EStructLayout::Compatible_Std430 :
                CHECK_THROW_MSG( data.mslOffset  == offset );
                CHECK_THROW_MSG( data.glslOffset == offset );
                CHECK_THROW_MSG( data.cppOffset  == offset );
                break;
            case EStructLayout::Metal :
                CHECK_THROW_MSG( data.mslOffset == offset );
                CHECK_THROW_MSG( data.cppOffset == offset );
                break;
            case EStructLayout::Std140 :
            case EStructLayout::Std430 :
                CHECK_THROW_MSG( data.glslOffset == offset );
                CHECK_THROW_MSG( data.cppOffset  == offset );
                break;
            case EStructLayout::InternalIO :
                // ignore offsets
                break;
            case EStructLayout::_Count :
            case EStructLayout::Unknown :
            default :               CHECK_THROW_MSG( false );
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    _Validate
=================================================
*/
    void  ShaderStructType::_Validate (StringView prefix, StringView stName, ArrayView<Field> fields, INOUT ValidationData &data) __Th___
    {
        const bool  is_internal_io  = data.layout == EStructLayout::InternalIO;
        const bool  glsl_compat     = IsGLSLCompatible( data.layout );
        const bool  msl_compat      = IsMSLCompatible( data.layout );

        for (auto& field : fields)
        {
            if ( field.padding )
                continue;

            CHECK( field.IsDeviceAddress() == (field.IsBufferRef() or field.IsPointer() or field.IsUntypedDeviceAddress()) );

            if ( field.IsDeviceAddress() )
            {
                TEST_FEATURE( data.features, bufferDeviceAddress, ", required for field '"s << field.name << "'" );

                const Bytes     ptr_size    = 8_b;
                const Bytes     ptr_align   = 8_b;

                data.mslOffset  = AlignUp( data.mslOffset,  ptr_align );
                data.glslOffset = AlignUp( data.glslOffset, ptr_align );
                data.cppOffset  = AlignUp( data.cppOffset,  ptr_align );

                _ValidateOffsets( data, field.offset );

                const uint  arr_size = field.IsStaticArray() ? field.arraySize : 1;

                data.mslOffset  += ptr_size * arr_size;
                data.glslOffset += ptr_size * arr_size;
                data.cppOffset  += ptr_size * arr_size;
            }
            else
            if ( field.IsStruct() )
            {
                CHECK_THROW_MSG( IsSameLayouts( data.layout, field.stType->Layout() ),
                    "Struct '"s << stName << "' field '" << field.name << "' uses Struct '" << field.stType->Name() << "' with layout '" <<
                    EStructLayout_ToString(field.stType->Layout()) << "' is not compatible with layout '" << EStructLayout_ToString(data.layout) << "'" );

                const Bytes     st_align = field.stType->_structAlign;
                data.mslOffset  = AlignUp( data.mslOffset,  st_align );
                data.glslOffset = AlignUp( data.glslOffset, st_align );
                data.cppOffset  = AlignUp( data.cppOffset,  st_align );

                _ValidateOffsets( data, field.offset );

                String  str{prefix};
                if ( not str.empty() )  str << '.';

                ValidationData  data2 = data;
                data2.baseOffset = field.offset;

                _Validate( (str << field.name), field.stType->Name(), field.stType->_fields, INOUT data2 );

                if ( not field.IsDynamicArray() )
                {
                    const uint  arr_size = field.IsStaticArray() ? field.arraySize : 1;
                    data.mslOffset  += AlignUp( data2.mslOffset  - data.mslOffset,  st_align ) * arr_size;
                    data.glslOffset += AlignUp( data2.glslOffset - data.glslOffset, st_align ) * arr_size;
                    data.cppOffset  += AlignUp( data2.cppOffset  - data.cppOffset,  st_align ) * arr_size;
                }
            }
            else
            {
                BEGIN_ENUM_CHECKS();
                switch ( field.type )
                {
                    case EValueType::Bool32 :
                    case EValueType::Int32 :
                    case EValueType::UInt32 :
                    case EValueType::Float32 :
                        break;  // default types

                    case EValueType::Int8_Norm :
                    case EValueType::UInt8_Norm :
                    case EValueType::Int16_Norm :
                    case EValueType::UInt16_Norm :
                        break;  // same as float

                    case EValueType::Int64 :
                    case EValueType::UInt64 :
                        TEST_FEATURE( data.features, shaderInt64, ", required for field '"s << field.name << "'" );
                        break;

                    case EValueType::Float64 :
                        TEST_FEATURE( data.features, shaderFloat64, ", required for field '"s << field.name << "'" );
                        break;

                    case EValueType::Float16 :
                        if ( not is_internal_io ) {
                            TEST_FEATURE( data.features, shaderFloat16, ", required for field '"s << field.name << "'" );
                        }
                        break;

                    case EValueType::Bool8 :
                    case EValueType::Int8 :
                    case EValueType::UInt8 :
                        if ( not is_internal_io ) {
                            TEST_FEATURE( data.features, shaderInt8, ", required for field '"s << field.name << "'" );
                        }
                        break;

                    case EValueType::Int16 :
                    case EValueType::UInt16 :
                        if ( not is_internal_io ) {
                            TEST_FEATURE( data.features, shaderInt16, ", required for field '"s << field.name << "'" );
                        }
                        break;

                    case EValueType::DeviceAddress :
                        TEST_FEATURE( data.features, bufferDeviceAddress, ", required for field '"s << field.name << "'" );
                        break;

                    case EValueType::Unknown :
                    case EValueType::_Count :
                    default :                   CHECK_THROW_MSG( false, "unknown value type" );
                }
                END_ENUM_CHECKS();

                Bytes   msl_offset  = data.mslOffset;
                Bytes   glsl_offset = data.glslOffset;
                Bytes   cpp_offset  = data.cppOffset;

                // Metal
                if ( msl_compat )
                {
                    auto [size, align] = _GetMSLSizeAndAlign( field, data.layout );

                    CHECK_THROW_MSG( field.IsDynamicArray() == (size == 0_b) );  // internal error
                    CHECK_THROW_MSG( size == 0_b or size == field.size,
                        "Struct '"s << stName << "' field '" << field.name << "' size mismatch for Metal backend: (" <<
                        ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

                    data.mslOffset  = AlignUp( data.mslOffset, align );
                    msl_offset      = data.mslOffset + size;
                }

                // GLSL
                if ( glsl_compat )
                {
                    auto [size, align] = _GetGLSLSizeAndAlign( field, data.layout );

                    CHECK_THROW_MSG( field.IsDynamicArray() == (size == 0_b) );  // internal error
                    CHECK_THROW_MSG( size == 0_b or size == field.size,
                        "Struct '"s << stName << "' field '" << field.name << "' size mismatch for GLSL backend: (" <<
                        ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

                    data.glslOffset = AlignUp( data.glslOffset, align );
                    glsl_offset     = data.glslOffset + size;
                }

                // CPP
                {
                    auto [size, align] = _GetCPPSizeAndAlign( field, data.layout );

                    CHECK_THROW_MSG( field.IsDynamicArray() == (size == 0_b) );  // internal error
                    CHECK_THROW_MSG( size == 0_b or size == field.size,
                        "Struct '"s << stName << "' field '" << field.name << "' size mismatch for C++ backend: (" <<
                        ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

                    data.cppOffset  = AlignUp( data.cppOffset, align );
                    cpp_offset      = data.cppOffset + size;
                }

                _ValidateOffsets( data, field.offset );

                data.mslOffset  = msl_offset;
                data.glslOffset = glsl_offset;
                data.cppOffset  = cpp_offset;
            }
        }
    }

/*
=================================================
    _AddPadding
=================================================
*/
    void  ShaderStructType::_AddPadding (const EStructLayout layout, INOUT Array<Field> &fields) __Th___
    {
        BEGIN_ENUM_CHECKS();
        switch ( layout )
        {
            case EStructLayout::Compatible_Std430 :
            case EStructLayout::Std430 :
            case EStructLayout::Compatible_Std140 :
            case EStructLayout::Std140 :
                for (usize i = 0; i < fields.size(); ++i)
                {
                    auto&   field = fields[i];

                    if ( field.IsArray() )  continue;

                    // add padding for vec3
                    if ( field.IsVec() and field.rows == 3 and not field.packed )
                    {
                        Field   pad = field;

                        pad.name    = "_"s + field.name + "_padding_w";
                        pad.rows    = 1;
                        pad.size    = field.size / 4;
                        pad.offset  = field.offset + (field.size - pad.size);
                        pad.align   = pad.size;
                        pad.padding = true;
                        ASSERT( pad.IsScalar() );

                        ++i;
                        fields.insert( fields.begin()+i, RVRef(pad) );
                    }
                }
                break;

            case EStructLayout::Metal :
            case EStructLayout::InternalIO :
            case EStructLayout::_Count :
            case EStructLayout::Unknown :   break;
        }
        END_ENUM_CHECKS();

    }

/*
=================================================
    constructor
=================================================
*/
    ShaderStructType::ShaderStructType (const String &name) __Th___ :
        _originName{ name },
        _typeName{ ValidateTypeName( name )},
        _features{ ObjectStorage::Instance()->GetDefaultFeatureSets() }
    {
        ObjectStorage::Instance()->AddName<ShaderStructName>( name );
        CHECK_THROW_MSG( ObjectStorage::Instance()->structTypes.emplace( name, ShaderStructTypePtr{this} ).second,
            "StructureType with name '"s << name << "' is already defined" );
    }

/*
=================================================
    AddFeatureSet
=================================================
*/
    void  ShaderStructType::AddFeatureSet (const String &name) __Th___
    {
        CHECK_THROW_MSG( _fields.empty(),
            "StructureType '"s << name << "' doesn't define any field" );

        auto&   storage = *ObjectStorage::Instance();
        auto    fs_it   = storage.featureSets.find( FeatureSetName{name} );
        CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
            "FeatureSet with name '"s << name << "' is not found" );

        _features.push_back( fs_it->second );
    }

/*
=================================================
    Set
=================================================
*/
    void  ShaderStructType::Set2 (const String &fields) __Th___
    {
        return Set( ObjectStorage::Instance()->defaultLayout, fields );
    }

    void  ShaderStructType::Set (const EStructLayout layout, const String &fields) __Th___
    {
        CHECK_THROW_MSG( not fields.empty() );
        CHECK_THROW_MSG( _fields.empty() );

        _ParseFields( fields, OUT _fields );
        CHECK_THROW_MSG( not _fields.empty() );

        _CalcOffsets( Name(), layout, INOUT _fields, OUT _align, OUT _structAlign, OUT _size );
        CHECK_THROW_MSG( _align > 0 );
        CHECK_THROW_MSG( HasDynamicArray() or _size > 0 );

        _layout = layout;

        for (auto& field : _fields)
        {
            if ( not field.stType )
                continue;

            for (auto& feat : field.stType->_features) {
                _features.push_back( feat );
            }
        }
        ScriptFeatureSet::Minimize( INOUT _features );

        {
            ValidationData  data{ _features, _layout };
            _Validate( "", Name(), _fields, INOUT data );
            _ValidateOffsets( data, HasDynamicArray() ? StaticSize() : _size );
        }

        _AddPadding( _layout, INOUT _fields );

        auto&   storage = *ObjectStorage::Instance();
        if ( storage.spirvCompiler != null and IsGLSLCompatible( _layout ))
        {
            String  dsl_hdr;
            String  dsl_src;
            CHECK_THROW_MSG( ToGLSL( true, OUT dsl_hdr, OUT dsl_src ));

            Version2    spv_ver {1,0};
            String      header, source;

            header  << storage.GetShaderExtensionsGLSL( INOUT spv_ver, EShaderStages::Fragment, _features )
                    << dsl_hdr
                    << "layout(set=0, binding=0, " 
                    << (IsStd430( _layout ) ? "std430" : "std140")
                    << ") buffer BufType {\n"
                    << dsl_src
                    << "} buf;\n\n";
            source << "void Main () {}";

            SpirvCompiler::Input    in;
            in.shaderType   = EShader::Fragment;
            in.spirvVersion = spv_ver;
            in.entry        = "Main";
            in.header       = header;
            in.source       = source;

            SpirvCompiler::ShaderReflection refl;
            String                          log;

            if ( not storage.spirvCompiler->BuildReflection( in, OUT refl, OUT log ))
            {
                CHECK_THROW_MSG( false, "Failed to compile temp shader:\n"s << log );
            }

            CHECK_THROW_MSG( refl.layout.descrSets.size() == 1, "internal error" );
            CHECK_THROW_MSG( refl.layout.descrSets.front().layout.uniforms.size() == 1, "internal error" );

            auto&   un = refl.layout.descrSets.front().layout.uniforms[0];
            ASSERT( un.first == UniformName{"buf"} );
            CHECK_THROW_MSG( un.second.type == EDescriptorType::UniformBuffer or
                             un.second.type == EDescriptorType::StorageBuffer,
                             "internal error" );

            CHECK_THROW_MSG( (_size == Bytes{un.second.buffer.staticSize}) or
                             (AlignUp( _size, _align ) == Bytes{un.second.buffer.staticSize}) or
                             (AlignUp( _size, _structAlign ) == Bytes{un.second.buffer.staticSize}) );
            CHECK_THROW_MSG( ArrayStride() == Bytes{un.second.buffer.arrayStride} );
        }

        if ( storage.metalCompiler != null and IsMSLCompatible( _layout ))
        {
            String  dsl_src;
            CHECK_THROW_MSG( ToMSL( OUT dsl_src ));

            Version2    msl_ver {2,0};
            String      str;
            str << storage.GetShaderExtensionsMSL( INOUT msl_ver, EShaderStages::Fragment, _features )
                << dsl_src
                << "void Main (constant " << Typename() << "& buf [[buffer(0)]]) {}";

            MetalCompiler::Input    in;
            in.target               = ECompilationTarget::Metal_Mac;
            in.options              = Default;
            in.version              = EShaderVersion::Metal_Mac_2_3;
            in.source               = str;
            in.enablePreprocessing  = false;

            MetalBytecode_t bytecode;
            String          log;
            CHECK_THROW_MSG( storage.metalCompiler->Compile( in, OUT bytecode, OUT log ),
                "MSL shader struct type validation failed:\n"s << log );

            CHECK_THROW( not bytecode.empty() ); // internal error
        }
    }

/*
=================================================
    _CreatePackedTypeGLSL1
=================================================
*/
    bool  ShaderStructType::_CreatePackedTypeGLSL1 (INOUT String &str, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &field)
    {
        CHECK_ERR_MSG( not (field.IsScalar() or field.IsStruct() or field.IsDeviceAddress()),
            "Field '"s << field.name << "' has unsupported type" );

        const char  vec_field_names[] = "xyzw";

        str << "// size: " << ToString( field.size ) << ", align: " << ToString( field.align ) << "\n"
            << "#define " << packedTypeName << "( _name_ ) \\\n";

        if ( field.IsVec() )
        {
            for (ubyte r = 0; r < field.rows; ++r) {
                str << "\t" << memberTypeName << "  _name_ ## _" << vec_field_names[r] << (r+1 == field.rows ? "\n" : "; \\\n");
            }
        }else
        if ( field.IsMat() )
        {
            for (ubyte c = 0; c < field.cols; ++c) {
                str << "\t" << memberTypeName << "( _name_ ## _c" << ToString( uint(c) ) << " )" << (c+1 == field.cols ? "\n" : "; \\\n");
            }
        }

        // example: '#define /*float3*/ GetInplaceFloat3( /*inplace_float3*/ _fieldName_ ) ...'
        String  short_name {packedTypeName.substr( packedTypeName.find( '_' )+1 )};  // remove 'inplace_'
        CHECK_ERR( not short_name.empty() );
        short_name[0] = ToUpperCase( short_name[0] );

        String  mat_field = short_name.substr( 0, short_name.size()-3 ) + short_name.substr( short_name.size()-1 );

        str << "#define GetInplace" << short_name << "( _fieldName_ )  " << dstType << "( ";

        if ( field.IsVec() )
        {
            for (ubyte r = 0; r < field.rows; ++r) {
                str << (r ? ", " : "") << "(_fieldName_ ## _" << vec_field_names[r] << ")"; 
            }
        }else
        if ( field.IsMat() )
        {
            UNTESTED;
            for (ubyte c = 0; c < field.cols; ++c) {
                str << (c ? ", " : "") << "GetInplace" << mat_field << "(_fieldName_ ## _c" << ToString( uint(c) ) << ")";
            }
        }
        str << " )\n";

        // example: '#define /*inplace_float3*/ SetInplaceFloat3( _fieldName_, /*float3*/_src_ ) ...'
        str << "#define SetInplace" << short_name << "( _fieldName_, _src_ )  {";
        if ( field.IsVec() )
        {
            for (ubyte r = 0; r < field.rows; ++r) {
                str << (r ? ", " : "") << "(_fieldName_ ## _" << vec_field_names[r] << " = (_src_)." << vec_field_names[r] << ")"; 
            }
        }else
        if ( field.IsMat() )
        {
            for (ubyte c = 0; c < field.cols; ++c) {
                str << (c ? ", " : "") << "SetInplace" << mat_field << "(_fieldName_ ## _c" << ToString( uint(c) ) << ")";
            }
        }
        str << "}\n\n";
        return true;
    }

/*
=================================================
    _CreatePackedTypeGLSL2
=================================================
*/
    bool  ShaderStructType::_CreatePackedTypeGLSL2 (INOUT String &str, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &field)
    {
        CHECK_ERR_MSG( not (field.IsScalar() or field.IsStruct() or field.IsUntypedDeviceAddress()),
            "Field '"s << field.name << "' has unsupported type" );

        const char  vec_field_names[] = "xyzw";

        str << "// size: " << ToString( field.size ) << ", align: " << ToString( field.align ) << "\n"
            << "struct " << packedTypeName << "\n{\n";

        if ( field.IsVec() )
        {
            for (ubyte r = 0; r < field.rows; ++r) {
                str << "\t" << memberTypeName << "  " << vec_field_names[r] << ";\n";
            }
        }else
        if ( field.IsMat() )
        {
            for (ubyte c = 0; c < field.cols; ++c) {
                str << "\t" << memberTypeName << "  c" << ToString( uint(c) ) << ";\n";
            }
        }
        str << "};\n";

        // example: 'float3  Cast (const packed_float3 src) ...'
        str << dstType << "  Cast" << " (const " << packedTypeName << " src) { return " << dstType << "( ";
        if ( field.IsVec() )
        {
            for (ubyte r = 0; r < field.rows; ++r) {
                str << (r ? ", " : "") << "src." << vec_field_names[r]; 
            }
        }else
        if ( field.IsMat() )
        {
            for (ubyte c = 0; c < field.cols; ++c) {
                str << (c ? ", " : "") << "Cast(src.c" << ToString( uint(c) ) << ")";
            }
        }
        str << " ); }\n";

        // example: 'packed_float3  Cast (const float3 src) ...'
        str << packedTypeName << "  Cast" << " (const " << dstType << " src) { return " << packedTypeName << "( ";
        if ( field.IsVec() )
        {
            for (ubyte r = 0; r < field.rows; ++r) {
                str << (r ? ", " : "") << "src." << vec_field_names[r]; 
            }
        }else
        if ( field.IsMat() )
        {
            for (ubyte c = 0; c < field.cols; ++c) {
                str << (c ? ", " : "") << "Cast(src[" << ToString( uint(c) ) << "])";
            }
        }
        str << " ); }\n\n";

        return true;
    }

/*
=================================================
    ValueTypeToStrGLSL
=================================================
*/
namespace {
    ND_ static bool  ValueTypeToStrGLSL (EValueType type, OUT StringView &s_name, OUT StringView &v_name, OUT StringView &m_name)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EValueType::Int8 :
            case EValueType::Int8_Norm :    s_name = "int8_t";      v_name = "i8vec";   break;
            case EValueType::UInt8 :
            case EValueType::UInt8_Norm :   s_name = "uint8_t";     v_name = "u8vec";   break;
            case EValueType::Int16 :
            case EValueType::Int16_Norm :   s_name = "int16_t";     v_name = "i16vec";  break;
            case EValueType::UInt16 :
            case EValueType::UInt16_Norm :  s_name = "uint16_t";    v_name = "u16vec";  break;
            case EValueType::Bool32 :       s_name = "bool";        v_name = "bvec";    break;
            case EValueType::Int32 :        s_name = "int";         v_name = "ivec";    break;
            case EValueType::UInt32 :       s_name = "uint";        v_name = "uvec";    break;
            case EValueType::Int64 :        s_name = "int64_t";     v_name = "i64vec";  break;
            case EValueType::DeviceAddress: s_name = "uvec2";                           break;
            case EValueType::UInt64 :       s_name = "uint64_t";    v_name = "u64vec";  break;
            case EValueType::Float16 :      s_name = "float16_t";   v_name = "f16vec";  m_name = "f16mat";  break;
            case EValueType::Float32 :      s_name = "float";       v_name = "vec";     m_name = "mat";     break;
            case EValueType::Float64 :      s_name = "double";      v_name = "dvec";    m_name = "dmat";    break;

            case EValueType::Bool8 :        // TODO
            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       RETURN_ERR( "unknown value type" );
        }
        END_ENUM_CHECKS();
        return true;
    }
}
/*
=================================================
    ValueTypeToStrGLSL_WithPrecision
----
            |   fp range   |  fp magnitude  | fp precision |   int range   |
    --------|--------------|----------------|--------------|---------------|
    highp   | -2^62; 2^62  |  2^-62;  2^62  |     2^-16    | -2^16;  2^16  |
    mediump | -2^14; 2^14  |  2^-14;  2^14  |     2^-10    | -2^10;  2^10  |
    lowp    | -2;    2     |  2^-8;   2     |     2^-8     | -2^8;   2^8   |
=================================================
*/
namespace {
    ND_ static bool  ValueTypeToStrGLSL_WithPrecision (EValueType type, OUT StringView &s_name, OUT StringView &v_name, OUT StringView &m_name)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            // integer
            case EValueType::Int8 :         s_name = "lowp int";        v_name = "lowp ivec";       break;
            case EValueType::UInt8 :        s_name = "lowp uint";       v_name = "lowp uvec";       break;
            case EValueType::Int16 :        s_name = "mediump int";     v_name = "mediump ivec";    break;
            case EValueType::UInt16 :       s_name = "mediump uint";    v_name = "mediump uvec";    break;
            case EValueType::Bool32 :       s_name = "bool";            v_name = "bvec";            break;
            case EValueType::Int32 :        s_name = "int";             v_name = "ivec";            break;
            case EValueType::UInt32 :       s_name = "uint";            v_name = "uvec";            break;
            case EValueType::Int64 :        s_name = "int64_t";         v_name = "i64vec";          break;
            case EValueType::UInt64 :       s_name = "uint64_t";        v_name = "u64vec";          break;

            // float point
            case EValueType::Int8_Norm :    s_name = "lowp float";      v_name = "lowp vec";        break;
            case EValueType::UInt8_Norm :   s_name = "lowp float";      v_name = "lowp vec";        break;
            case EValueType::Int16_Norm :   s_name = "mediump float";   v_name = "mediump vec";     break;
            case EValueType::UInt16_Norm :  s_name = "mediump float";   v_name = "mediump vec";     break;
            case EValueType::Float16 :      s_name = "mediump float";   v_name = "mediump vec"; m_name = "mediump mat"; break;
            case EValueType::Float32 :      s_name = "float";           v_name = "vec";         m_name = "mat";         break;
            case EValueType::Float64 :      s_name = "double";          v_name = "dvec";        m_name = "dmat";        break;

            case EValueType::Bool8 :        // TODO
            case EValueType::DeviceAddress:
            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       RETURN_ERR( "unknown value type" );
        }
        END_ENUM_CHECKS();
        return true;
    }
}

/*
=================================================
    ValueTypeToStrCPP
=================================================
*/
namespace {
    ND_ static bool  ValueTypeToStrCPP (EValueType type, INOUT String &src)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EValueType::Bool8 :        src << "bool";          break;
            case EValueType::Bool32 :       src << "lbool";         break;
            case EValueType::Int8 :
            case EValueType::Int8_Norm :    src << "sbyte";         break;
            case EValueType::UInt8 :
            case EValueType::UInt8_Norm :   src << "ubyte";         break;
            case EValueType::Int16 :
            case EValueType::Int16_Norm :   src << "short";         break;
            case EValueType::UInt16 :
            case EValueType::UInt16_Norm :  src << "ushort";        break;
            case EValueType::Int32 :        src << "int";           break;
            case EValueType::UInt32 :       src << "uint";          break;
            case EValueType::Int64 :        src << "slong";         break;
            case EValueType::UInt64 :       src << "ulong";         break;
            case EValueType::Float16 :      src << "half";          break;
            case EValueType::Float32 :      src << "float";         break;
            case EValueType::Float64 :      src << "double";        break;
            case EValueType::DeviceAddress: src << "DeviceAddress"; break;

            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       RETURN_ERR( "unknown value type" );
        }
        END_ENUM_CHECKS();
        return true;
    }
}
/*
=================================================
    ToGLSL
=================================================
*/
    bool  ShaderStructType::ToGLSL (const bool withOffsets, INOUT String &outTypes, INOUT String &outFields, INOUT UniqueTypes_t &uniqueTypes, Bytes baseOffset) const
    {
        const bool  is_std140 = IsStd140( _layout );

        const auto  GetScalarName = [&uniqueTypes, &outTypes] (bool isStd140, const Field &field, INOUT String &str) -> bool
        {{
            StringView  s_name, v_name, m_name;
            CHECK_ERR( ValueTypeToStrGLSL( field.type, OUT s_name, OUT v_name, OUT m_name ));

            String  tname;
            if ( field.IsScalar() ) tname << s_name;
            if ( field.IsVec() )    tname << v_name << ToString( field.rows );
            if ( field.IsMat() )    tname << m_name << ToString( field.cols ) << 'x' << ToString( field.rows );
            CHECK_ERR( not tname.empty() );

            if ( field.packed )
            {
                String  public_name;
                CHECK_ERR( ValueTypeToStrCPP( field.type, OUT public_name ));

                // Prefix 'inplace_' - packed in the same place, because in 'std140' struct aligned to 16 bytes

                String  packed  = (isStd140 ? "inplace_"s : "packed_"s) << public_name;
                String  memt    {s_name};

                if ( field.IsVec() )
                    packed << ToString( field.rows );

                if ( field.IsMat() )
                {
                    memt = (isStd140 ? "inplace_"s : "packed_"s) << public_name << ToString( field.rows );
                    packed << ToString( field.cols ) << 'x' << ToString( field.rows );

                    // add packed vec type
                    if ( uniqueTypes.insert( memt ).second )
                    {
                        Field   f;
                        f.type      = field.type;
                        f.rows      = field.rows;
                        f.cols      = 1;
                        f.arraySize = 0;    // non-array
                        f.packed    = true;
                        f.size      = field.size / field.cols;
                        f.align     = field.align;

                        if ( isStd140 ) {
                            CHECK_ERR( _CreatePackedTypeGLSL1( INOUT outTypes, memt, s_name, (String{v_name} << ToString( field.rows )), f ));
                        }else{
                            CHECK_ERR( _CreatePackedTypeGLSL2( INOUT outTypes, memt, s_name, (String{v_name} << ToString( field.rows )), f ));
                        }
                    }
                }

                if ( uniqueTypes.insert( packed ).second )
                {
                    Field   field2 = field;
                    field2.pointer = false;

                    if ( isStd140 ) {
                        CHECK_ERR( _CreatePackedTypeGLSL1( INOUT outTypes, packed, memt, tname, field2 ));
                    }else{
                        CHECK_ERR( _CreatePackedTypeGLSL2( INOUT outTypes, packed, memt, tname, field2 ));
                    }
                }
                str << packed;
                return true;
            }
            else
            {
                str << tname;
                return true;
            }
            return true;
        }};

        const auto  GetBufferRefName = [withOffsets, &uniqueTypes, &outTypes] (const Field &field, OUT String &type_name) -> bool
        {{
            type_name = String{field.stType->Typename()} + "_AERef";

            if ( uniqueTypes.insert( type_name ).second )
            {
                const bool  is_std140_2 = IsStd140( field.stType->Layout() );

                String  fields;
                CHECK_ERR( field.stType->ToGLSL( withOffsets, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));
                outTypes
                    << "layout(" << (is_std140_2 ? "std140" : "std430") << ", buffer_reference, buffer_reference_align="
                    << ToString( ulong(field.stType->Align()) ) << ") buffer " << type_name << "\n{\n" << fields << "};\n\n";
            }
            return true;
        }};


        for (auto& field : _fields)
        {
            // padding needed for structure declaration, buffer declaration has explicit offsets
            if ( withOffsets and field.padding )
                continue;

            outFields << "\t";
            if ( withOffsets )
                outFields << "layout(offset=" << ToString( usize(field.offset + baseOffset) ) << ", align=" << ToString( usize(field.align) ) << ") ";

            if ( field.IsBufferRef() and field.IsPointer() )
            {
                String  type_name;
                CHECK_ERR( GetBufferRefName( field, OUT type_name ));

                const String    ptr_name = String{field.stType->Typename()} + "_AEPtr";

                if ( uniqueTypes.insert( ptr_name ).second )
                {
                    String  fields;
                    CHECK_ERR( field.stType->ToGLSL( withOffsets, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));
                    outTypes
                        << "layout(std430, buffer_reference, buffer_reference_align="
                        << ToString( ulong(field.stType->Align()) ) << ") buffer " << ptr_name << "\n{\n\t"
                        << type_name << "  data [];\n"
                        << "};\n\n";
                }
                outFields << ptr_name << "  " << field.name;
            }
            else
            if ( field.IsBufferRef() )
            {
                String  type_name;
                CHECK_ERR( GetBufferRefName( field, OUT type_name ));
                outFields << type_name << "  " << field.name;
            }
            else
            if ( field.IsPointer() )
            {
                StringView  s_name, v_name, m_name;
                CHECK_ERR( ValueTypeToStrGLSL( field.type, OUT s_name, OUT v_name, OUT m_name ));

                String      elem_name;
                CHECK_ERR( GetScalarName( false, field, INOUT elem_name ));

                String      type_name;
                CHECK_ERR( ValueTypeToStrCPP( field.type, OUT type_name ));
                if ( field.packed )     "packed_" >> type_name;
                if ( field.IsVec() )    type_name << ToString( field.rows );
                if ( field.IsMat() )    type_name << ToString( field.cols ) << 'x' << ToString( field.rows );
                CHECK_ERR( not type_name.empty() );
                type_name << "_AEPtr";

                const SizeAndAlign  size_align  = _GetGLSLSizeAndAlign( field, EStructLayout::Std430 );

                if ( uniqueTypes.insert( type_name ).second )
                {
                    outTypes
                        << "layout(std430, buffer_reference, buffer_reference_align="
                        << ToString( ulong(size_align.align) ) << ") buffer " << type_name << "\n{\n\t"
                        << elem_name << "  data [];\n"
                        << "};\n\n";
                }

                outFields << type_name << "  " << field.name;
            }
            else
            if ( field.IsStruct() )
            {
                const StringView    type_name = field.stType->Typename();

                if ( uniqueTypes.insert( String{type_name} ).second )
                {
                    String  fields;
                    CHECK_ERR( field.stType->ToGLSL( false, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));
                    outTypes
                        << "#define " << type_name << "_defined\n"
                        << "struct " << type_name << "\n{\n" << fields << "};\n\n";
                }

                outFields << type_name << "  " << field.name;
            }
            else
            {
                CHECK_ERR( GetScalarName( is_std140, field, INOUT outFields ));

                if ( field.packed )
                    outFields << ( is_std140 ? ("( "s << field.name << " )") : ("  "s << field.name) );
                else
                    outFields << "  " << field.name;
            }

            outFields
                << (field.IsDynamicArray() ? " []"s : (field.IsStaticArray() ? (" ["s << ToString(field.arraySize) << "]") : ""s)) << ";";

            if ( withOffsets )
                outFields << "\n";
            else
                outFields << "  // offset: " << ToString( usize(field.offset) ) << "\n";
        }
        return true;
    }

    bool  ShaderStructType::ToGLSL (bool withOffsets, INOUT String &types, INOUT String &fields, Bytes baseOffset) const
    {
        UniqueTypes_t   uniqueTypes;
        return ToGLSL( withOffsets, INOUT types, INOUT fields, uniqueTypes, baseOffset );
    }

/*
=================================================
    StructToGLSL
=================================================
*/
    bool  ShaderStructType::StructToGLSL (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes) const
    {
        if ( uniqueTypes.insert( String{Typename()} ).second )
        {
            String  fields;
            fields << "#define " << Typename() << "_defined\n";
            fields << "struct "s << Typename() << "\n{\n";
            CHECK_ERR( ToGLSL( false, INOUT types, INOUT fields, uniqueTypes ));
            fields << "};\n\n";
            types << fields;
        }
        return true;
    }

/*
=================================================
    _CreatePackedTypeMSL
=================================================
*/
    bool  ShaderStructType::_CreatePackedTypeMSL (INOUT String &str, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &field)
    {
        CHECK_ERR( field.IsMat() );

        str << "// size: " << ToString( field.size ) << ", align: " << ToString( field.align ) << "\n"
            << "struct " << packedTypeName << "\n{\n";

        for (ubyte c = 0; c < field.cols; ++c) {
            str << "\tpacked_" << memberTypeName << "  c" << ToString( uint(c) ) << ";\n";
        }

        str << "\n\t" << dstType << " cast () const { return " << dstType << "( ";

        for (ubyte c = 0; c < field.cols; ++c) {
            str << (c ? ", " : "") << memberTypeName << "(c" << ToString( uint(c) ) << ")";
        }

        str << " ); }\n"
            << "};\n";
        return true;
    }

/*
=================================================
    ValueTypeToStrMSL
=================================================
*/
namespace {
    ND_ static bool  ValueTypeToStrMSL (EValueType type, INOUT String &src)
    {
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EValueType::Bool8 :        src << "bool";      break;
            case EValueType::Int8 :
            case EValueType::Int8_Norm :    src << "char";      break;
            case EValueType::UInt8 :
            case EValueType::UInt8_Norm :   src << "uchar";     break;
            case EValueType::Int16 :
            case EValueType::Int16_Norm :   src << "short";     break;
            case EValueType::UInt16 :
            case EValueType::UInt16_Norm :  src << "ushort";    break;
            case EValueType::Int32 :        src << "int";       break;
            case EValueType::UInt32 :       src << "uint";      break;
            case EValueType::Int64 :        src << "long";      break;
            case EValueType::UInt64 :       src << "ulong";     break;
            case EValueType::Float16 :      src << "half";      break;
            case EValueType::Float32 :      src << "float";     break;
            case EValueType::DeviceAddress: src << "void";      break;

            case EValueType::Bool32 :       // TODO
            case EValueType::Float64 :
            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       RETURN_ERR( "unknown value type" );
        }
        END_ENUM_CHECKS();
        return true;
    }
}
/*
=================================================
    ValueTypeToStrMSL_WithNorm
=================================================
*/
namespace {
    ND_ static bool  ValueTypeToStrMSL_WithNorm (EValueType type, uint rows, INOUT String &src)
    {
        // normalized types
        switch ( type )
        {
            case EValueType::Int8_Norm :
                switch ( rows ) {
                    case 1 :    src << "r8snorm<half>";         return true;
                    case 2 :    src << "rg8snorm<half2>";       return true;
                    case 4 :    src << "rgba8snorm<half4>";     return true;
                }
                break;
            case EValueType::UInt8_Norm :
                switch ( rows ) {
                    case 1 :    src << "r8unorm<half>";         return true;
                    case 2 :    src << "rg8unorm<half3>";       return true;
                    case 4 :    src << "rgba8unorm<half4>";     return true;
                }
                break;
            case EValueType::Int16_Norm :
                switch ( rows ) {
                    case 1 :    src << "r16snorm<float>";       return true;
                    case 2 :    src << "rg16snorm<float2>";     return true;
                    case 4 :    src << "rgba16snorm<float4>";   return true;
                }
                break;
            case EValueType::UInt16_Norm :
                switch ( rows ) {
                    case 1 :    src << "r16unorm<float>";       return true;
                    case 2 :    src << "rg16unorm<float>";      return true;
                    case 4 :    src << "rgba16unorm<float4>";   return true;
                }
                break;
        }

        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EValueType::Bool8 :        src << "bool";      break;
            case EValueType::Int8 :
            case EValueType::Int8_Norm :    src << "char";      break;
            case EValueType::UInt8 :
            case EValueType::UInt8_Norm :   src << "uchar";     break;
            case EValueType::Int16 :
            case EValueType::Int16_Norm :   src << "short";     break;
            case EValueType::UInt16 :
            case EValueType::UInt16_Norm :  src << "ushort";    break;
            case EValueType::Int32 :        src << "int";       break;
            case EValueType::UInt32 :       src << "uint";      break;
            case EValueType::Int64 :        src << "long";      break;
            case EValueType::UInt64 :       src << "ulong";     break;
            case EValueType::Float16 :      src << "half";      break;
            case EValueType::Float32 :      src << "float";     break;

            case EValueType::Bool32 :       // TODO
            case EValueType::Float64 :
            case EValueType::DeviceAddress:
            case EValueType::Unknown :
            case EValueType::_Count :
            default :                       RETURN_ERR( "unknown value type" );
        }
        END_ENUM_CHECKS();

        if ( rows > 1 )
            src << ToString(rows);

        return true;
    }
}
/*
=================================================
    ToMSL
=================================================
*/
    bool  ShaderStructType::ToMSL (INOUT String &outTypes, INOUT UniqueTypes_t &uniqueTypes) const
    {
        const auto  TypeToStr = [&uniqueTypes, &outTypes] (const Field &field, INOUT String &src) -> bool
        {{
            if ( field.stType )
            {
                if ( uniqueTypes.insert( String{field.stType->Typename()} ).second )
                    CHECK_ERR( field.stType->ToMSL( INOUT outTypes, INOUT uniqueTypes ));

                src << field.stType->Typename();
                return true;
            }
            else
            if ( field.IsScalar() )
            {
                CHECK_ERR( ValueTypeToStrMSL( field.type, INOUT src ));
                return true;
            }
            else
            if ( field.IsVec() )
            {
                if ( field.packed )
                    src << "packed_";

                CHECK_ERR( ValueTypeToStrMSL( field.type, INOUT src ));

                src << ToString( field.rows );
                return true;
            }
            else
            if ( field.IsMat() )
            {
                StringView  s_name;
                switch ( field.type )
                {
                    case EValueType::Float16 :  s_name = "half";    break;
                    case EValueType::Float32 :  s_name = "float";   break;
                    default :                   RETURN_ERR( "unsupported value type for matrix" );
                }

                String  tname {s_name};
                tname << ToString( field.cols ) << 'x' << ToString( field.rows );

                if ( field.packed )
                {
                    const String    memt    = String{s_name} << ToString( field.rows );
                    const String    packed  = "packed_"s << tname;

                    if ( uniqueTypes.insert( packed ).second )
                        CHECK_ERR( _CreatePackedTypeMSL( INOUT outTypes, packed, memt, tname, field ));

                    tname = packed;
                }

                src << tname;
                return true;
            }
            else
                return false;
        }};

        CHECK_ERR( IsMSLCompatible( Layout() ));

        String  src;
        src << "struct " << Typename() << "\n{\n";

        for (auto& field : _fields)
        {
            if ( field.padding )
                continue;

            src << "\t";

            if ( field.IsDynamicArray() )
                src << "device ";

            if ( field.IsDeviceAddress() )
            {
                Field   f2 = field;     f2.pointer = false;

                String  type_name;
                CHECK_ERR( TypeToStr( f2, INOUT type_name ));

                src << "device " << type_name << "*";
            }
            else
            if ( field.IsStruct() or field.IsScalar() or field.IsVec() or field.IsMat() )
            {
                CHECK_ERR( TypeToStr( field, INOUT src ));
            }
            else
            {
                RETURN_ERR( "unknown field type" );
            }

            if ( field.IsDynamicArray() )
                src << "*";

            src << "  " << field.name
                << (field.IsStaticArray() ? (" ["s << ToString(field.arraySize) << "]") : ""s) << ";"
                << "  // offset: " << ToString( usize(field.offset) ) << "\n";
        }
        src << "};\n";

        if ( not HasDynamicArray() )
        {
            src << "static_assert( sizeof(" << Typename() << ") == " << ToString(usize( AlignUp( _size, _align )))
                << ", \"size mismatch\" );\n";
        }
        src << "\n";

        outTypes << src;
        return true;
    }

    bool  ShaderStructType::ToMSL (INOUT String &types) const
    {
        UniqueTypes_t   uniqueTypes;
        return ToMSL( INOUT types, uniqueTypes );
    }
/*
=================================================
    ToCPP
=================================================
*/
    bool  ShaderStructType::ToCPP (INOUT String &outTypes, INOUT UniqueTypes_t &uniqueTypes) const
    {
        if ( not uniqueTypes.insert( String{Typename()} ).second )
            return true;  // already exists

        String  src;
        String  test;

        const auto  TypeToStr = [&uniqueTypes, &outTypes] (const Field &field, INOUT String &src) -> bool
        {{
            if ( field.stType )
            {
                CHECK_ERR( field.stType->ToCPP( INOUT outTypes, INOUT uniqueTypes ));
                src << field.stType->Typename();
                return true;
            }
            else
            if ( field.IsVec() or field.IsScalar() )
            {
                if ( field.IsVec() and field.packed )
                    src << "packed_";

                CHECK_ERR( ValueTypeToStrCPP( field.type, INOUT src ));

                if ( field.IsVec() )
                    src << ToString( field.rows );
                return true;
            }
            else
            if ( field.IsMat() )
            {
                if ( field.packed )
                    src << "packed_";

                switch ( field.type )
                {
                    case EValueType::Float16 :  src << "half";      break;
                    case EValueType::Float32 :  src << "float";     break;
                    case EValueType::Float64 :  src << "double";    break;
                    default :                   RETURN_ERR( "unsupported value type for matrix" );
                }
                src << ToString( field.cols ) << 'x' << ToString( field.rows ) << "_storage";
                return true;
            }
            else
                return false;
        }};

        src << "#ifndef " << Typename() << "_DEFINED\n"
            << "#\tdefine " << Typename() << "_DEFINED\n"
            << "\t// size: " << ToString( usize(_size) );

        if ( _size != AlignUp( _size, _align ))
            src << " (" << ToString(usize( AlignUp( _size, _align ))) << ")";

        src << ", align: " << ToString( usize(_align) );

        if ( _align != _structAlign )
            src << " (" << ToString( usize(_structAlign) ) << ")";

        src << "\n\tstruct ";

        if ( _size != AlignUp( _size, _align ))
            src << "alignas(" << ToString( usize(_align) ) << ") ";

        src << Typename() << "\n"
            << "\t{\n"
            << "\t\tstatic constexpr auto  TypeName = ShaderStructName{HashVal32{0x"
            << ToString<16>( uint{ShaderStructName{Typename()}} ) << "u}};  // '" << Typename() << "'\n\n";

        for (auto& field : _fields)
        {
            if ( field.padding )
                continue;

            src << (field.IsDynamicArray() ? "\t//\t" : "\t\t");

            if ( field.IsStaticArray() )
                src << "StaticArray< ";

            if ( field.IsDeviceAddress() )
            {
                src << "TDeviceAddress< ";
                CHECK_ERR( TypeToStr( field, INOUT src ));
                src << (field.pointer ? " *" : " ")
                    << ">";
            }
            else
            if ( field.IsStruct() or field.IsVec() or field.IsScalar() or field.IsMat() )
            {
                CHECK_ERR( TypeToStr( field, INOUT src ));
            }
            else
            {
                RETURN_ERR( "unknown field type" );
            }

            if ( field.IsStaticArray() )
                src << ", " << ToString(field.arraySize) << " >  ";

            src << "  " << field.name;

            if ( field.IsDynamicArray() )
                src << " []";

            src << ";\n";

            if ( not field.IsDynamicArray() )
                test << "\tSTATIC_ASSERT( offsetof(" << Typename() << ", " << field.name << ") == " << ToString(usize( field.offset )) << " );\n";
        }
        src << "\t};\n"
            << "#endif\n"
            << test;

        if ( _size > 0 ) {
            src << "\tSTATIC_ASSERT( sizeof(" << Typename() << ") == " << ToString(usize( AlignUp( _size, _align ))) << " );\n";
        }
        src << "\n";

        outTypes << src;
        return true;
    }

    bool  ShaderStructType::ToCPP (INOUT String &types) const
    {
        UniqueTypes_t   uniqueTypes;
        return ToCPP( INOUT types, uniqueTypes );
    }

/*
=================================================
    FieldCount
----
    total number of fields will be used in
    constant buffer with argument buffer
=================================================
*/
    bool  ShaderStructType::FieldCount (INOUT usize &count) const
    {
        CHECK_ERR( not HasDynamicArray() );

        for (auto& field : _fields)
        {
            CHECK_ERR( not field.IsDynamicArray() );

            if ( field.IsStruct() )
            {
                usize   cnt = 0;
                CHECK_ERR( field.stType->FieldCount( INOUT cnt ));
                count += cnt * Max( 1u, field.arraySize );
            }else{
                count += Max( 1u, field.arraySize );
            }
        }
        return true;
    }

/*
=================================================
    VertexInputToGLSL
=================================================
*/
    String  ShaderStructType::VertexInputToGLSL (const String &prefix, INOUT uint &loc) C_Th___
    {
        _usage |= EUsage::VertexAttribs;

        String  str;
        for (auto& field : _fields)
        {
            CHECK_THROW_MSG( not field.IsArray() );
            CHECK_THROW_MSG( not field.IsDeviceAddress() );

            if ( field.IsStruct() )
            {
                String  pref = prefix;
                if ( not pref.empty() ) pref += '_';
                pref += field.name;

                str << field.stType->VertexInputToGLSL( pref, INOUT loc );
            }
            else
            {
                CHECK_THROW_MSG( not field.IsMat(),
                    "Matrix is not supported for VertexInput" );

                StringView  s_name, v_name, m_name;
                CHECK_ERR( ValueTypeToStrGLSL_WithPrecision( field.type, OUT s_name, OUT v_name, OUT m_name ));

                String  tname;
                if ( field.IsScalar() ) tname << s_name;
                if ( field.IsVec() )    tname << v_name << ToString( field.rows );

                str << "layout(location=" << ToString(loc++) << ") in " << tname << "  "
                    << prefix << (prefix.empty() ? ""s : "_"s) << field.name << ";\n";
            }
        }
        return str;
    }

/*
=================================================
    VertexInputToMSL
=================================================
*/
    String  ShaderStructType::VertexInputToMSL (const String &prefix, INOUT uint &index) C_Th___
    {
        _usage |= EUsage::VertexAttribs;

        String  str;
        for (auto& field : _fields)
        {
            CHECK_THROW_MSG( not field.IsArray() );
            CHECK_THROW_MSG( not field.IsDeviceAddress() );

            if ( field.IsStruct() )
            {
                String  pref = prefix;
                if ( not pref.empty() ) pref += '_';
                pref += field.name;

                str << field.stType->VertexInputToMSL( pref, INOUT index );
            }
            else
            {
                CHECK_THROW_MSG( not field.IsMat(),
                    "Matrix is not supported for VertexInput" );

                str << "  ";
                CHECK_ERR( ValueTypeToStrMSL_WithNorm( field.type, field.rows, INOUT str ));

                str << "  " << field.name << "  [[attribute(" << ToString( uint(index) ) << ")]];\n";
                ++index;
            }
        }
        return str;
    }

/*
=================================================
    GetVertexInput
=================================================
*/
    void  ShaderStructType::GetVertexInput (INOUT uint &loc, INOUT Array<VertexInput> &arr) C_Th___
    {
        for (auto& field : _fields)
        {
            if ( field.padding ) continue;

            CHECK_THROW_MSG( not field.IsDeviceAddress() );

            if ( field.IsStruct() )
            {
                field.stType->GetVertexInput( INOUT loc, INOUT arr );
            }
            else
            {
                CHECK_THROW_MSG( not field.IsMat(),
                    "Matrix is not supported for VertexInput" );

                EVertexType type = Default;
                BEGIN_ENUM_CHECKS();
                switch ( field.type )
                {
                    case EValueType::Int8 :         type = EVertexType::Byte;           break;
                    case EValueType::UInt8 :        type = EVertexType::UByte;          break;
                    case EValueType::Int16 :        type = EVertexType::Short;          break;
                    case EValueType::UInt16 :       type = EVertexType::UShort;         break;
                    case EValueType::Int32 :        type = EVertexType::Int;            break;
                    case EValueType::UInt32 :       type = EVertexType::UInt;           break;
                    case EValueType::Int64 :        type = EVertexType::Long;           break;
                    case EValueType::UInt64 :       type = EVertexType::ULong;          break;
                    case EValueType::Float16 :      type = EVertexType::Half;           break;
                    case EValueType::Float32 :      type = EVertexType::Float;          break;
                    case EValueType::Float64 :      type = EVertexType::Double;         break;
                    case EValueType::Int8_Norm :    type = EVertexType::Byte_Norm;      break;
                    case EValueType::UInt8_Norm :   type = EVertexType::UByte_Norm;     break;
                    case EValueType::Int16_Norm :   type = EVertexType::Short_Norm;     break;
                    case EValueType::UInt16_Norm :  type = EVertexType::UShort_Norm;    break;

                    case EValueType::Bool8 :        // TODO
                    case EValueType::Bool32 :
                    case EValueType::DeviceAddress:
                    case EValueType::Unknown :
                    case EValueType::_Count :
                    default :                       CHECK_THROW_MSG( false, "unknown value type" );
                }
                END_ENUM_CHECKS();

                if ( field.IsVec() )
                    type |= EVertexType((field.rows - 1) << uint(EVertexType::_VecOffset));

                auto&   dst = arr.emplace_back();
                dst.type    = type;
                dst.index   = CheckCast<ubyte>(loc++);
                dst.offset  = Bytes16u{field.offset};

                TestFeature_VertexType( _features, &FeatureSet::vertexFormats, type, "vertexFormats" );  // throw
            }
        }
    }

/*
=================================================
    ToShaderIO_GLSL
=================================================
*/
    String  ShaderStructType::ToShaderIO_GLSL (EShader shaderType, const bool input, INOUT UniqueTypes_t &uniqueTypes) C_Th___
    {
        if ( (shaderType == EShader::MeshTask)      or
             (input and shaderType == EShader::Mesh) )
        {
            String  str;
            CHECK_THROW( StructToGLSL( OUT str, INOUT uniqueTypes ));

            str << "taskPayloadSharedEXT " << Typename() << " ";
            str << (input ? "In" : "Out");
            str << ";\n\n";
            return str;
        }

        _usage |= EUsage::ShaderIO;

        uint    loc         = 0;
        bool    is_array    = false;
        String  str;

        BEGIN_ENUM_CHECKS();
        switch ( shaderType )
        {
            case EShader::Vertex :          str << "Vertex";         break;
            case EShader::TessControl :     str << "TessControl";    is_array = true;       break;
            case EShader::TessEvaluation :  str << "TessEvaluation"; is_array = input;      break;
            case EShader::Geometry :        str << "Geometry";       is_array = input;      break;
            case EShader::Fragment :        str << "Fragment";       break;
            case EShader::MeshTask :        str << "MeshTask";       is_array = false;      break;
            case EShader::Mesh :            str << "Mesh";           is_array = not input;  break;

            case EShader::Compute :
            case EShader::Tile :
            case EShader::RayGen :
            case EShader::RayAnyHit :
            case EShader::RayClosestHit :
            case EShader::RayMiss :
            case EShader::RayIntersection :
            case EShader::RayCallable :
            case EShader::_Count :
            case EShader::Unknown :
            default :
                CHECK_THROW_MSG( false, "unsupported shader type for shader IO" );
        }
        END_ENUM_CHECKS();

        (input ?
            "// stage input\n"s  << (is_array ? "layout(location="s << ToString(loc) << ") " : "") << "in " :
            "// stage output\n"s << (is_array ? "layout(location="s << ToString(loc) << ") " : "") << "out ")
            >> str;

        str << (input ? "Input" : "Output");
        str << " {\n"
            << _ToShaderIO_GLSL( "", INOUT loc, not is_array )
            << "} ";
        str << (input ? "In" : "Out");

        if ( is_array ) str << " []";

        str << ";\n\n";
        return str;
    }

/*
=================================================
    _ToShaderIO_GLSL
=================================================
*/
    String  ShaderStructType::_ToShaderIO_GLSL (const String &prefix, INOUT uint &loc, bool useLocations) C_Th___
    {
        const bool  is_internal_io  = (_layout == EStructLayout::InternalIO);

        String  str;
        for (auto& field : _fields)
        {
            if ( field.padding ) continue;

            CHECK_THROW_MSG( not field.IsArray() );
            CHECK_THROW_MSG( not field.IsDeviceAddress() );

            if ( field.IsStruct() )
            {
                String  pref = prefix;
                if ( not pref.empty() ) pref += '_';
                pref += field.name;

                str << field.stType->_ToShaderIO_GLSL( pref, INOUT loc, useLocations );
            }
            else
            {
                StringView  s_name, v_name, m_name;
                if ( is_internal_io ){
                    CHECK_ERR( ValueTypeToStrGLSL_WithPrecision( field.type, OUT s_name, OUT v_name, OUT m_name ));
                }else{
                    CHECK_ERR( ValueTypeToStrGLSL( field.type, OUT s_name, OUT v_name, OUT m_name ));
                }

                String  tname;
                if ( is_internal_io and field.type < EValueType::Float16 )  tname << "flat ";

                if ( field.IsScalar() ) tname << s_name;
                if ( field.IsVec() )    tname << v_name << ToString( field.rows );
                if ( field.IsMat() )    tname << m_name << ToString( field.cols ) << 'x' << ToString( field.rows );
                CHECK_ERR( not tname.empty() );

                str << "  ";
                if ( useLocations ) str << "layout(location=" << ToString(loc++) << ") ";
                str << tname << "  " << prefix << (prefix.empty() ? ""s : "_"s) << field.name << ";\n";
            }
        }
        return str;
    }

/*
=================================================
    ToShaderIO_MSL
=================================================
*/
    String  ShaderStructType::ToShaderIO_MSL (EShader, bool, INOUT UniqueTypes_t &) C_Th___
    {
        _usage |= EUsage::ShaderIO;

        return  "struct "s << Typename() << "\n{\n" <<
                "  float4  position [[position]];\n" <<
                    _ToShaderIO_MSL( "" ) <<
                "};\n\n";
    }

    String  ShaderStructType::_ToShaderIO_MSL (const String &prefix) C_Th___
    {
        String  str;
        for (auto& field : _fields)
        {
            if ( field.padding ) continue;

            CHECK_THROW_MSG( not field.IsArray() );
            CHECK_THROW_MSG( not field.IsDeviceAddress() );

            if ( field.IsStruct() )
            {
                String  pref = prefix;
                if ( not pref.empty() ) pref += '_';
                pref += field.name;

                str << field.stType->_ToShaderIO_MSL( pref );
            }
            else
            {
                CHECK_THROW_MSG( not field.IsMat(),
                    "Matrix is not supported for MSL ShaderIO" );

                str << "  ";
                CHECK_ERR( ValueTypeToStrMSL_WithNorm( field.type, field.rows, INOUT str ));

                str << "  " << field.name << ";\n";
            }
        }
        return str;
    }

/*
=================================================
    FieldsToString
=================================================
*/
    String  ShaderStructType::FieldsToString () const
    {
        // TODO
        return "";
    }

/*
=================================================
    Compare
=================================================
*/
    bool  ShaderStructType::Compare (const ShaderStructType &rhs) const
    {
        return  _fields     == rhs._fields      and
                _layout     == rhs._layout      and
                _features   == rhs._features;
    }

/*
=================================================
    TotalSize
=================================================
*/
    Bytes  ShaderStructType::TotalSize (uint arraySize) const
    {
        CHECK( (arraySize > 0) == HasDynamicArray() );

        return StaticSize() + ArrayStride() * arraySize;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ShaderStructType::Bind (const ScriptEnginePtr &se) __Th___
    {
        {
            EnumBinder<EUsage>  binder{ se };
            binder.Create();

            binder.Comment( "Used as interface between graphics pipeline stages. Reflection to C++ is not supported." );
            binder.AddValue( "ShaderIO",        EUsage::ShaderIO );

            binder.Comment( "Used as vertex buffer layout. Enables reflection to C++." );
            binder.AddValue( "VertexLayout",    EUsage::VertexLayout );

            binder.Comment( "Used as vertex attributes in shader.\n"
                            "Reflection to C++ is not enabled, use 'VertexLayout' to enable it." );
            binder.AddValue( "VertexAttribs",   EUsage::VertexAttribs );

            binder.Comment( "Used as uniform/storage buffer. Layout must be same in GLSL/MSL and C++. Enables reflection to C++." );
            binder.AddValue( "BufferLayout",    EUsage::BufferLayout );

            binder.Comment( "Used as buffer reference in shader. Layout must be same between shaders in single platform.\n"
                            "Reflection to C++ is not enabled, use 'BufferLayout' to enable it." );
            binder.AddValue( "BufferReference", EUsage::BufferReference );
            STATIC_ASSERT( uint(EUsage::All) == 31 );
        }
        {
            ClassBinder<ShaderStructType>   binder{ se };
            binder.CreateRef();

            binder.Comment( "Create structure type.\n"
                            "Name is used as typename for uniform/storage/vertex buffer or as shader in/out block." );
            binder.AddFactoryCtor( &ShaderStructType_Ctor, {"name"} );

            binder.Comment( "Add FeatureSet to the structure.\n"
                            "If used float64/int64 types FeatureSet must support this features." );
            binder.AddMethod( &ShaderStructType::AddFeatureSet,     "AddFeatureSet",    {"fsName"} );

            binder.Comment( "Set source with structure fields.\n"
                            "Layout - offset and align rules." );
            binder.AddMethod( &ShaderStructType::Set,               "Set",              {"layout", "fields"} );
            binder.AddMethod( &ShaderStructType::Set2,              "Set",              {"fields"} );

        //  binder.AddMethod( &ShaderStructType::FieldsToString,    "FieldsToString"    );

            binder.Comment( "Manually specify how structure will be used." );
            binder.AddMethod( &ShaderStructType::AddUsage,          "AddUsage",         {} );

            binder.Comment( "Returns size of the static data." );
            binder.AddMethod( &ShaderStructType::_StaticSize,       "StaticSize",       {} );

            binder.Comment( "Returns array element size for dynamic arrays." );
            binder.AddMethod( &ShaderStructType::_ArrayStride,      "ArrayStride",      {} );
        }
    }


} // AE::PipelineCompiler
