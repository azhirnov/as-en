// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Alignment rules:
		std140	- https://registry.khronos.org/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
		std430	- https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shader_storage_buffer_object.txt
*/

#include "res_pack/pipeline_compiler/ScriptObjects/ShaderStructType.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"
#include "res_pack/pipeline_compiler/Compiler/MetalCompiler.h"
#include "res_pack/pipeline_compiler/Compiler/SlangCompiler.h"

namespace AE::PipelineCompiler
{
namespace
{
	using EFlags = ShaderStructType::EFlags;

	static constexpr EFlags		EFlags_Precision		= EFlags::HighPrecision | EFlags::MediumPrecision | EFlags::LowPrecision;
	static constexpr EFlags		EFlags_Interpolation	= EFlags::FlatInterpolation | EFlags::SmoothInterpolation | EFlags::NoPerspectiveInterpolation;
	static constexpr EFlags		EFlags_MSInterpolation	= EFlags::CentroidInterpolation | EFlags::PerSampleInterpolation;
	static constexpr EFlags		EFlags_ShaderIO			= EFlags_Precision | EFlags_Interpolation | EFlags_MSInterpolation | EFlags::Invariant;
	static constexpr EFlags		EFlags_VSCompatible		= EFlags_Precision | EFlags::Invariant;


	static void  ArraySize_Ctor (OUT void* mem, uint value) {
		PlacementNew<ArraySize>( OUT mem, value );
	}

	static void  Align_Ctor (OUT void* mem, uint value) {
		PlacementNew<Align>( OUT mem, value );
	}

	static ShaderStructType*  ShaderStructType_Ctor (const String &name) {
		return ShaderStructType::Create( name ).Detach();
	}

	ND_ static String  ValidateTypeName (const String &name)
	{
		String	str = name;
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
		return AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140, EStructLayout::HLSL_Const );
	}

	ND_ static bool  IsStd430 (EStructLayout layout)
	{
		return AnyEqual( layout, EStructLayout::Compatible_Std430, EStructLayout::Std430, EStructLayout::HLSL_Struct );
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
	IsHLSLCompatible
=================================================
*/
	ND_ static bool  IsHLSLCompatible (EStructLayout layout)
	{
		return AnyEqual( layout, EStructLayout::HLSL_Const, EStructLayout::HLSL_Struct, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430 );
	}

/*
=================================================
	IsCompatibleLayouts
----
	mainType {
		subType  t;
	}
=================================================
*/
	ND_ static bool  IsCompatibleLayouts (EStructLayout mainType, EStructLayout subType)
	{
		if ( mainType == subType )
			return true;

		if ( subType == EStructLayout::Compatible_Std140 and
			 (mainType == EStructLayout::Std140 or mainType == EStructLayout::HLSL_Const) )
			return true;

		if ( subType == EStructLayout::Compatible_Std430 and
			 (mainType == EStructLayout::Std430 or mainType == EStructLayout::HLSL_Struct) )
			return true;

		return false;
	}

/*
=================================================
	EValueType_ToString
=================================================
*/
	ND_ static StringView  EValueType_ToString (EValueType type)
	{
		switch_enum( type )
		{
			case EValueType::Bool8 :		return "Bool8";
			case EValueType::Bool32 :		return "Bool32";
			case EValueType::Int8 :			return "Int8";
			case EValueType::Int16 :		return "Int16";
			case EValueType::Int32 :		return "Int32";
			case EValueType::Int64 :		return "Int64";
			case EValueType::UInt8 :		return "UInt8";
			case EValueType::UInt16 :		return "UInt16";
			case EValueType::UInt32 :		return "UInt32";
			case EValueType::UInt64 :		return "UInt64";
			case EValueType::Float16 :		return "Float16";
			case EValueType::Float32 :		return "Float32";
			case EValueType::Float64 :		return "Float64";
			case EValueType::Int8_Norm :	return "Int8_Norm";
			case EValueType::Int16_Norm :	return "Int16_Norm";
			case EValueType::UInt8_Norm :	return "UInt8_Norm";
			case EValueType::UInt16_Norm :	return "UInt16_Norm";
			case EValueType::DeviceAddress:	return "DeviceAddress";
			case EValueType::Unknown :
			case EValueType::_Count :		break;
		}
		switch_end
		RETURN_ERR( "unknown EValueType" );
	}

/*
=================================================
	EStructLayout_ToString
=================================================
*/
	ND_ static StringView  EStructLayout_ToString (EStructLayout type)
	{
		switch_enum( type )
		{
			case EStructLayout::Compatible_Std140 :	return "Compatible_Std140";
			case EStructLayout::Compatible_Std430 :	return "Compatible_Std430";
			case EStructLayout::Metal :				return "Metal";
			case EStructLayout::Std140 :			return "Std140";
			case EStructLayout::Std430 :			return "Std430";
			case EStructLayout::InternalIO :		return "InternalIO";
			case EStructLayout::HLSL_Const :		return "HLSL_Const";
			case EStructLayout::HLSL_Struct :		return "HLSL_Struct";
			case EStructLayout::_Count :
			case EStructLayout::Unknown :			break;
		}
		switch_end
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
		ClassBinder<ArraySize>	binder{ se };
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
		ClassBinder<Align>	binder{ se };
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
		return	name		== rhs.name			and
				type		== rhs.type			and
				stType		== rhs.stType		and
				arraySize	== rhs.arraySize	and
				rows		== rhs.rows			and
				cols		== rhs.cols			and
				flags		== rhs.flags		and
				size		== rhs.size			and
				align		== rhs.align		and
				offset		== rhs.offset;
	}

	bool  ShaderStructType::Field::IsAnyPadding () const	{ return AnyBits( flags, EFlags::Padding_GLSL | EFlags::Padding_MSL | EFlags::Padding_HLSL ); }
//-----------------------------------------------------------------------------



/*
=================================================
	ShaderStructType::Constants
=================================================
*/
	ShaderStructType::Constants::Constants () :
		typeNames{
			{ "bool",			{ EValueType::Bool8,			1,	1 }},
			{ "lbool",			{ EValueType::Bool32,			4,	4 }},
			{ "sbyte",			{ EValueType::Int8,				1,	1 }},
			{ "ubyte",			{ EValueType::UInt8,			1,	1 }},
			{ "sshort",			{ EValueType::Int16,			2,	2 }},
			{ "ushort",			{ EValueType::UInt16,			2,	2 }},
			{ "sint",			{ EValueType::Int32,			4,	4 }},
			{ "uint",			{ EValueType::UInt32,			4,	4 }},
			{ "slong",			{ EValueType::Int64,			8,	8 }},
			{ "ulong",			{ EValueType::UInt64,			8,	8 }},
			{ "DeviceAddress",	{ EValueType::DeviceAddress,	8,	8 }},	// uvec2
			{ "half",			{ EValueType::Float16,			2,	2 }},
			{ "float",			{ EValueType::Float32,			4,	4 }},
			{ "double",			{ EValueType::Float64,			8,	8 }},
			{ "sbyte_norm",		{ EValueType::Int8_Norm,		1,	1 }},
			{ "ubyte_norm",		{ EValueType::UInt8_Norm,		1,	1 }},
			{ "sshort_norm",	{ EValueType::Int16_Norm,		2,	2 }},
			{ "ushort_norm",	{ EValueType::UInt16_Norm,		2,	2 }},
		},
		renameMap{
			{"int8_t",		"sbyte"},	{"byte",	"sbyte"},	{"char",	"sbyte"},
			{"uint8_t",		"ubyte"},	{"uchar",	"ubyte"},
			{"int16_t",		"sshort"},	{"short",	"sshort"},
			{"uint16_t",	"ushort"},
			{"int32_t",		"sint"},	{"int",		"sint"},
			{"uint32_t",	"uint"},
			{"int64_t",		"slong"},	{"long",	"slong"},	{"ssize",	"slong"},	{"ptrdiff_t",	"slong"},
			{"uint64_t",	"ulong"},	{"usize",	"ulong"},	{"size_t",	"ulong"},
			{"float16_t",	"half"},
			{"float32_t",	"float"},
			{"float64_t",	"double"},
			{"ivec",		"sint"},	{"uvec",	"uint"},
			{"i8vec",		"sbyte"},	{"u8vec",	"ubyte"},
			{"i16vec",		"sshort"},	{"u16vec",	"ushort"},
			{"i32vec",		"sint"},	{"u32vec",	"uint"},
			{"i64vec",		"slong"},	{"u64vec",	"ulong"},
			{"f16vec",		"half"},	{"f16mat",	"half"},
			{"f32vec",		"float"},	{"f32mat",	"float"},	{"vec",		"float"},	{"mat",		"float"},
			{"f64vec",		"double"},	{"f64mat",	"double"},	{"dvec",	"double"},	{"dmat",	"double"},

			// _norm
			{"char_norm",	"sbyte_norm"},
			{"uchar_norm",	"ubyte_norm"},
			{"short_norm",	"sshort_norm"},
			// norm_
			{"norm_sbyte",	"sbyte_norm"},	{"norm_byte",	"sbyte_norm"},	{"norm_char",	"sbyte_norm"},
			{"norm_ubyte",	"ubyte_norm"},	{"norm_uchar",	"ubyte_norm"},
			{"norm_sshort",	"sshort_norm"},	{"norm_short",	"sshort_norm"},
			{"norm_ushort",	"ushort_norm"},
		}
	{}

/*
=================================================
	_ParseFields
=================================================
*/
	void  ShaderStructType::_ParseFields (const String &fields, OUT Array<Field> &outFields) __Th___
	{
		outFields.clear();

		const auto&			const_ptr	= ObjectStorage::Instance()->_structTypeConstPtr;
		const auto&			c_typeNames	= const_ptr->typeNames;
		const auto&			c_renameMap	= const_ptr->renameMap;
		const StringView	c_packed	= "packed_";
		const StringView	c_atomic	= "Atomic";
		const Bytes			c_ptrSize	= 8_b;
		const Bytes			c_ptrAlign	= 8_b;

		const auto	IsTypeNameStart = [] (char c) -> bool
		{{
			return ((c >= 'a') and (c <= 'z')) or ((c >= 'A') and (c <= 'Z')) or (c == '_');
		}};
		const auto	IsNumber = [] (char c) -> bool
		{{
			return (c >= '0') and (c <= '9');
		}};

		Array<StringView>	tokens;

		const auto	ReadNameAndArray = [&tokens] (Array<StringView>::iterator &it, INOUT Field &field) __Th___
		{{
			field.name = String{*it};
			field.arraySizeChain.clear();

			for (auto c : field.name) {
				if_unlikely( not ( ((c >= 'a') and (c <= 'z')) or ((c >= 'A') and (c <= 'Z')) or ((c >= '0') and (c <= '9')) or (c == '_') ))
					CHECK_THROW_MSG( false, "invalid name: '"s << field.name << "'" );
			}
			++it;

			// non-array
			if ( it == tokens.end() )
			{
				field.arraySize = 0;
				return;
			}

			field.arraySize = 1;
			for (;;)
			{
				if ( *it != "[" )
					break;

				++it;
				CHECK_THROW_MSG( it != tokens.end(), "failed to parse ShaderStructType" );

				// dynamic array: []
				if ( *it == "]" )
				{
					field.arraySize = UMax;
					field.arraySizeChain.push_back( UMax );
					++it;
					CHECK_THROW_MSG( it == tokens.end(), "failed to parse ShaderStructType: dynamic array must be the last field" );
					CHECK_THROW_MSG( field.arraySizeChain.size() == 1, "failed to parse ShaderStructType: dynamic array can not be combined with static arrays" );
					return;
				}

				for (auto c : *it) {
					if ( not ( (c >= '0') and (c <= '9') ))
						CHECK_THROW_MSG( false, "invalid array size: '"s << *it << "'" );
				}

				uint	count = StringToUInt( *it );
				CHECK_THROW_MSG( count != 0 and count != UMax );

				field.arraySize *= count;
				field.arraySizeChain.push_back( count );

				++it;
				CHECK_THROW_MSG( it != tokens.end(), "failed to parse ShaderStructType" );
				CHECK_THROW_MSG( *it == "]", "failed to parse ShaderStructType" );
				++it;

				if ( it == tokens.end() )
					return;
			}

			CHECK_THROW_MSG( false, "can't parse token: '"s << *it << "'" );
		}};

		const auto	ParseFlags = [&tokens] (INOUT Array<StringView>::iterator &it, INOUT EFlags &flags)
		{{
			constexpr StringView	c_MediumP			= "mediump";
			constexpr StringView	c_Flat				= "flat";
			constexpr StringView	c_NoPerspective		= "noperspective";
			constexpr StringView	c_Centroid			= "centroid";
			constexpr StringView	c_PerSample			= "sample";
			constexpr StringView	c_Invariant			= "invariant";

			for (; it != tokens.end();)
			{
				if ( *it == c_MediumP )			{ flags |= EFlags::MediumPrecision;				++it;	continue; }
				if ( *it == c_Flat )			{ flags |= EFlags::FlatInterpolation;			++it;	continue; }
				if ( *it == c_NoPerspective )	{ flags |= EFlags::NoPerspectiveInterpolation;	++it;	continue; }
				if ( *it == c_Centroid )		{ flags |= EFlags::CentroidInterpolation;		++it;	continue; }
				if ( *it == c_PerSample )		{ flags |= EFlags::PerSampleInterpolation;		++it;	continue; }
				if ( *it == c_Invariant )		{ flags |= EFlags::Invariant;					++it;	continue; }
				return;
			}
		}};


		for (usize pos = 0, line_id = 0;; ++line_id)
		{
			StringView	line;
			{
				const usize		begin		= pos;
				const usize		semicolon	= fields.find( ';',  begin );
				const usize 	comment		= fields.find( "//", begin );

				if ( semicolon == String::npos and comment == String::npos )
				{
					bool	has_symbols = false;
					for (; pos < fields.size(); ++pos)
					{
						char c = fields[pos];
						has_symbols |= not (c == ' ' or c == '\t' or c == '\n');
					}

					CHECK_THROW_MSG( not has_symbols,
						"line("s << ToString(line_id) << ": missing semicolon in '" << StringView{fields}.substr( begin ) << "'" );
					break;
				}

				pos		= Min( semicolon, comment );
				line	= StringView{fields}.substr( begin, pos - begin );

				if ( pos == semicolon )		pos++;		else
				if ( pos == comment )		Parser::ToNextLine( fields, INOUT pos );
			}

			Parser::DivideString_CPP( line, OUT tokens );
			if ( tokens.empty() )
				continue;

			auto	it			= tokens.begin();
			EFlags	spec_flags	= Default;

			if ( *it == c_atomic )
			{
				spec_flags |= EFlags::Atomic;

				++it;
				CHECK_THROW_MSG( it != tokens.end(),
					"line("s << ToString(line_id) << ": unexpected end of field declaration, expected '<'." );
				CHECK_THROW_MSG( *it == "<",
					"line("s << ToString(line_id) << ": Atomic must be template type." );

				++it;
				CHECK_THROW_MSG( it != tokens.end(),
					"line("s << ToString(line_id) << ": unexpected end of field declaration after '<'." );
			}

			if ( IsTypeNameStart( it->front() ))
			{
				EFlags	flags = Default;

				ParseFlags( INOUT it, OUT flags );
				CHECK_THROW_MSG( it != tokens.end(),
					"line("s << ToString(line_id) << ": unexpected end of field declaration." );

				StringView	type_name = *it;

				if ( StartsWith( type_name, c_packed ))
				{
					flags		|= EFlags::Packed;
					type_name	= type_name.substr( c_packed.length() );
				}

				uint	rows		= 1;
				uint	cols		= 1;
				bool	has_rows	= false;
				bool	has_cols	= false;

				// vector or matrix
				if ( IsNumber( type_name.back() ))
				{
					rows		= uint(type_name.back() - '0');
					type_name	= type_name.substr( 0, type_name.size()-1 );
					has_rows	= true;
				}

				// matrix
				if ( has_rows and type_name.back() == 'x' and IsNumber( type_name[type_name.size()-2] ))
				{
					cols		= uint(type_name[type_name.size()-2] - '0');
					type_name	= type_name.substr( 0, type_name.size()-2 );
					has_cols	= true;
				}

				// remap type
				{
					auto	rm_it = c_renameMap.find( type_name );
					if ( rm_it != c_renameMap.end() )
						type_name = rm_it->second;
				}

				// find in standard types
				{
					auto	tn_it = c_typeNames.find( type_name );
					if ( tn_it != c_typeNames.end() )
					{
						if ( AllBits( flags, EFlags::Packed )) {
							CHECK_THROW_MSG( rows > 1,
								"line("s << ToString(line_id) << ": packed type is not supported for scalar" );
						}
						if ( has_rows ) {
							CHECK_THROW_MSG( rows >= 2 and rows <= 4,
								"line("s << ToString(line_id) << ": number of rows (" << ToString(rows) << ") must be in range [2,4]." );
						}
						if ( has_cols ) {
							CHECK_THROW_MSG( rows >= 2 and rows <= 4,
								"line("s << ToString(line_id) << ": for matrix number of rows (" << ToString(rows) << ") must be in range [2,4]." );

							CHECK_THROW_MSG( cols >= 2 and cols <= 4,
								"line("s << ToString(line_id) << ": for matrix number of columns (" << ToString(rows) << ") must be in range [2,4]." );
						}
						++it;
						CHECK_THROW_MSG( it != tokens.end(),
							"line("s << ToString(line_id) << ": unexpected end of field declaration after, expected field name." );

						if ( AnyBits( spec_flags, EFlags::Atomic ))
						{
							CHECK_THROW_MSG( *it == ">",
								"line("s << ToString(line_id) << ": expected '>'." );

							++it;
							CHECK_THROW_MSG( it != tokens.end(),
								"line("s << ToString(line_id) << ": unexpected end of field declaration after '>', expected field name." );
						}

						Field&	field	= outFields.emplace_back();
						field.type		= tn_it->second.type;
						field.align		= Bytes{tn_it->second.align};
						field.size		= Bytes{tn_it->second.size};
						field.rows		= ubyte(rows);
						field.cols		= ubyte(cols);
						field.flags		= flags | spec_flags;
						field.flags		|= (*it == "*") ? EFlags::Pointer | EFlags::Address : Default;

						if ( field.IsPointer() )
						{
							field.align	= c_ptrSize;
							field.size	= c_ptrAlign;

							++it;
							CHECK_THROW_MSG( it != tokens.end(),
								"line("s << ToString(line_id) << ": unexpected end of field declaration after, expected field name." );
						}

						ReadNameAndArray( it, INOUT field );
						CHECK_THROW_MSG( it == tokens.end(),
							"line("s << ToString(line_id) << ": semicolon expected." );
						continue;
					}
				}

				// TODO: simdgroup_float8x8
				// TODO: r8unorm<>, ...

				// custom type
				CHECK_THROW_MSG( spec_flags == Default,
					"Atomic<> is not compatible with user-defined type." );

				type_name = *it;
				++it;

				const bool	is_ref		 = (*it == "&");
				const auto&	struct_types = ObjectStorage::Instance()->structTypes;
				auto		st_it		 = struct_types.find( String{type_name} );
				CHECK_THROW_MSG( st_it != struct_types.end(),
					"line("s << ToString(line_id) << ": unknown typename: '"s << type_name << "'" );

				Field&	field	= outFields.emplace_back();

				if ( is_ref )
				{
					++it;
					st_it->second->AddUsage( EUsage::BufferReference );

					CHECK_THROW_MSG( not st_it->second->HasDynamicArray(),
						"line("s << ToString(line_id) << ": buffer reference with dynamic array is not supported, use pointer to <dynamic_array_element_type> instead" );

					field.stType	= st_it->second;
					field.align		= c_ptrSize;
					field.size		= c_ptrAlign;
					field.type		= EValueType::DeviceAddress;
					field.flags		|= EFlags::Address;
				}
				else
				{
					CHECK_THROW_MSG( not st_it->second->HasDynamicArray(),
						"line("s << ToString(line_id) << ": struct field with dynamic array is not supported" );

					field.stType	= st_it->second;
					field.align		= field.stType->_maxAlign;
					field.size		= field.stType->_size;
					field.flags		|= (*it == "*") ? EFlags::Pointer | EFlags::Address : Default;

					if ( field.IsPointer() )
					{
						++it;
						field.align	= c_ptrSize;
						field.size	= c_ptrAlign;
					}
				}

				ReadNameAndArray( it, INOUT field );
				CHECK_THROW_MSG( it == tokens.end(),
					"line("s << ToString(line_id) << ": failed to parse ShaderStructType, line: '"s << line << "'" );
				continue;
			}

			if ( *it == "//" )
			{
				Parser::ToNextLine( fields, INOUT pos );
				continue;
			}

			CHECK_THROW_MSG( false,
				"line("s << ToString(line_id) << ": failed to parse token: '"s << *it << "'" );
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
		maxAlign	= 0_b;
		structAlign	= 0_b;
		totalSize	= 0_b;

		for (usize i = 0; i < fields.size(); ++i)
		{
			auto&	field = fields[i];

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
				switch_enum( layout )
				{
					case EStructLayout::Compatible_Std140 :
					{
						// Metal does not support custom align and can not add padding for array element
						if ( field.IsArray() ) {
							CHECK_THROW_MSG( IsMultipleOf( field.align, 16 ) or IsMultipleOf( field.size, 16 ),
								"In Struct '"s << stName << "', field '" << field.name << "': align (" << ToString(uint(field.align)) <<
								") and size (" << ToString(uint(field.size)) << ") must be aligned to 16 bytes" );
						}
						field.align = Max( field.align, 16_b );
						break;
					}
					case EStructLayout::Std140 :
					case EStructLayout::HLSL_Const :
					{
						field.align = Max( field.align, 16_b );
						break;
					}
					case EStructLayout::Compatible_Std430 :	break;
					case EStructLayout::HLSL_Struct :		break;
					case EStructLayout::Metal :				break;
					case EStructLayout::Std430 :			break;
					case EStructLayout::InternalIO :		break;
					case EStructLayout::_Count :
					case EStructLayout::Unknown :
					default :								CHECK_THROW_MSG( false, "unknown layout type" );
				}
				switch_end
			}
			else
			{
				const bool	is_compat	= AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430 );
				const bool	is_msl		= is_compat and (layout == EStructLayout::Metal);
				const bool	is_glsl		= is_compat and AnyEqual( layout, EStructLayout::Std140, EStructLayout::Std430 );
				const bool	is_hlsl		= is_compat and AnyEqual( layout, EStructLayout::HLSL_Const, EStructLayout::HLSL_Struct );

				field.size *= field.rows;

				if ( AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430 ))
				{
					CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool8, EValueType::Bool32, EValueType::Float64 ),
						"In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
						"must not be Bool8/Bool32/Float64 for compatible layout" );
				}
				if ( AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430, EStructLayout::HLSL_Const, EStructLayout::HLSL_Struct ))
				{
					CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Int8, EValueType::UInt8, EValueType::Int8_Norm, EValueType::UInt8_Norm ),
						"In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
						"int8 types are not supported in HLSL" );
				}
				if ( AnyEqual( layout, EStructLayout::Std140, EStructLayout::Std430, EStructLayout::HLSL_Const, EStructLayout::HLSL_Struct ))
				{
					CHECK_THROW_MSG( not AnyEqual( field.type, EValueType::Bool8 ),
						"In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
						"must not be Bool8, use Bool32 or UInt instead" );
				}
				if ( IsStd140( layout ))
				{
					CHECK_THROW_MSG( not (field.IsPacked() and field.IsArray()),
						"In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
						"can not use packed type in array when used std140 layout" );
				}

				if ( is_msl and field.IsAtomic() )
				{
					bool	is_compat_type = AnyEqual( field.type, EValueType::Bool8, EValueType::Int32, EValueType::UInt32,
														EValueType::UInt64, EValueType::Float32 );

					CHECK_THROW_MSG( not is_compat_type,
						"In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
						"in MSL atomic type must be bool/int/uint/ulong/float" );
				}
				if ( (is_glsl or is_hlsl) and field.IsAtomic() )
				{
					bool	is_compat_type = AnyEqual( field.type, EValueType::Int32, EValueType::UInt32 );

					// TODO: check extensions

					CHECK_THROW_MSG( not is_compat_type,
						"In Struct '"s << stName << "', field '" << field.name << "', type '" << EValueType_ToString(field.type) << "': "
						"in GLSL atomic type must be int/uint" );
				}

				switch_enum( layout )
				{
					case EStructLayout::Compatible_Std140 :
					{
						if ( field.IsMat() and not field.IsPacked() )
						{
							field.align *= (field.rows == 3 ? 4 : field.rows);
							field.size  = AlignUp( field.size, field.align );

							CHECK_THROW_MSG( IsMultipleOf( field.size, 16 ),
								"In Struct '"s << stName << "', field '" << field.name << "': "
								"matrix columns are not aligned to 16 bytes, so it is incompatible with MSL and GLSL/HLSL in std140 layout, "
								"use std430 layout of replace matrix by vectors." );
						}
						else
						{
							if ( not field.IsPacked() and field.rows > 1 )
								field.align *= (field.rows == 3 or field.IsMat() ? 4 : field.rows);

							field.size = AlignUp( field.size, field.align );
						}

						if ( field.IsMat() )
							field.size *= field.cols;

						// Metal does not support custom align for arrays and can not add padding for array element
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
						if ( not field.IsPacked() and field.rows > 1 )
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

						if ( not field.IsPacked() and field.rows > 1 )
							field.align *= (field.rows == 3 ? 4 : field.rows);

						field.size = AlignUp( field.size, field.align );

						if ( field.IsMat() )
							field.size *= field.cols;
						break;
					}

					case EStructLayout::Std140 :
					case EStructLayout::HLSL_Const :
					{
						if ( not field.IsPacked() and field.rows > 1 )
							field.align *= (field.rows == 3 ? 4 : field.rows);

						if ( (field.IsMat() and not field.IsPacked()) or field.IsArray() )
							field.align = Max( field.align, 16_b );

						field.size = AlignUp( field.size, field.align );

						if ( field.IsMat() )
							field.size *= field.cols;
						break;
					}

					case EStructLayout::Std430 :
					case EStructLayout::HLSL_Struct :
					{
						if_unlikely( field.IsVec()		and field.IsPacked()						and
									 field.rows == 3	and IsMultipleOf( totalSize, field.align*4 ))
						{
							// special case for 'packed_vec3 + scalar'
							if ( i+1 < fields.size()	and
								 fields[i+1].IsScalar()	and fields[i+1].size == field.align )
							{
								field.flags = (field.flags & ~EFlags::Packed) | EFlags::PackedAlias;
								field.align *= 4;
								break;
							}

							// special case for 'packed_vec3 + aligned_vec'
							if ( (i+1 < fields.size() and not fields[i+1].IsPacked() and fields[i+1].rows > 1) or
								 (i+1 == fields.size() and maxAlign >= field.align*4) )
							{
								field.flags &= ~EFlags::Packed;
							}
						}

						if ( not field.IsPacked() and field.rows > 1 )
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
				switch_end
			}

			if ( field.IsStruct() or field.IsArray() )
				field.size = AlignUp( field.size, field.align );

			if ( field.IsStaticArray() )
				field.size *= field.arraySize;

			maxAlign		= Max( maxAlign, field.align );
			field.offset	= AlignUp( totalSize, field.align );

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
	ValueTypeSizeOf
=================================================
*/
namespace {
	ND_ static Bytes  ValueTypeSizeOf (EValueType type) __Th___
	{
		switch_enum( type )
		{
			case EValueType::Bool8 :		return 1_b;

			case EValueType::Int8 :
			case EValueType::Int8_Norm :	return 1_b;

			case EValueType::UInt8 :
			case EValueType::UInt8_Norm :	return 1_b;

			case EValueType::Int16 :
			case EValueType::Int16_Norm :	return 2_b;

			case EValueType::UInt16 :
			case EValueType::UInt16_Norm :	return 2_b;

			case EValueType::Bool32 :
			case EValueType::Int32 :
			case EValueType::UInt32 :		return 4_b;

			case EValueType::Int64 :
			case EValueType::UInt64 :		return 8_b;
			case EValueType::DeviceAddress:	return 8_b;

			case EValueType::Float16 :		return 2_b;
			case EValueType::Float32 :		return 4_b;
			case EValueType::Float64 :		return 8_b;

			case EValueType::Unknown :
			case EValueType::_Count :
			default :						CHECK_THROW_MSG( false, "unknown value type" );
		}
		switch_end
	}
}
/*
=================================================
	_GetCPPSizeAndAlign
=================================================
*/
	SizeAndAlign  ShaderStructType::_GetCPPSizeAndAlign2 (const Field &field, bool std140) __Th___
	{
	#define SWITCH_TYPE( _prefix_, _suffix_, ... )														\
		{																								\
			switch_enum( field.type )																	\
			{																							\
				case EValueType::Bool32 :		return SizeAndAlignOf<_prefix_ ## lbool  ## _suffix_>;	\
				case EValueType::Int32 :		return SizeAndAlignOf<_prefix_ ## int    ## _suffix_>;	\
				case EValueType::UInt32 :		return SizeAndAlignOf<_prefix_ ## uint   ## _suffix_>;	\
				case EValueType::Float32 :		return SizeAndAlignOf<_prefix_ ## float  ## _suffix_>;	\
				case EValueType::Int64 :		return SizeAndAlignOf<_prefix_ ## slong  ## _suffix_>;	\
				case EValueType::UInt64 :		return SizeAndAlignOf<_prefix_ ## ulong  ## _suffix_>;	\
				case EValueType::Float64 :		return SizeAndAlignOf<_prefix_ ## double ## _suffix_>;	\
				case EValueType::Bool8 :		return SizeAndAlignOf<_prefix_ ## bool   ## _suffix_>;	\
				case EValueType::Int8 :																	\
				case EValueType::Int8_Norm :	return SizeAndAlignOf<_prefix_ ## sbyte  ## _suffix_>;	\
				case EValueType::UInt8 :																\
				case EValueType::UInt8_Norm :	return SizeAndAlignOf<_prefix_ ## ubyte  ## _suffix_>;	\
				case EValueType::Int16 :																\
				case EValueType::Int16_Norm :	return SizeAndAlignOf<_prefix_ ## short  ## _suffix_>;	\
				case EValueType::UInt16 :																\
				case EValueType::UInt16_Norm :	return SizeAndAlignOf<_prefix_ ## ushort ## _suffix_>;	\
				case EValueType::Float16 :		return SizeAndAlignOf<_prefix_ ## half   ## _suffix_>;	\
				case EValueType::Unknown :																\
				case EValueType::_Count :																\
				__VA_ARGS__																				\
				default :						CHECK_THROW_MSG( false, "unknown value type" );			\
			}																							\
			switch_end																					\
		}
	#define SWITCH_MAT_TYPE( _prefix_, _suffix_ )														\
		{																								\
			switch_enum( field.type )																	\
			{																							\
				case EValueType::Float32 :		return SizeAndAlignOf<_prefix_ ## float  ## _suffix_>;	\
				case EValueType::Float64 :		return SizeAndAlignOf<_prefix_ ## double ## _suffix_>;	\
				case EValueType::Float16 :		return SizeAndAlignOf<_prefix_ ## half   ## _suffix_>;	\
				case EValueType::Bool32 :																\
				case EValueType::Int32 :																\
				case EValueType::UInt32 :																\
				case EValueType::Int64 :																\
				case EValueType::DeviceAddress:															\
				case EValueType::UInt64 :																\
				case EValueType::Bool8 :																\
				case EValueType::Int8 :																	\
				case EValueType::UInt8 :																\
				case EValueType::Int16 :																\
				case EValueType::UInt16 :																\
				case EValueType::Int8_Norm :															\
				case EValueType::UInt8_Norm :															\
				case EValueType::Int16_Norm :															\
				case EValueType::UInt16_Norm :	CHECK_THROW_MSG( false, "unsupported value type" );		\
				case EValueType::Unknown :																\
				case EValueType::_Count :																\
				default :						CHECK_THROW_MSG( false, "unknown value type" );			\
			}																							\
			switch_end																					\
		}

		CHECK_THROW_MSG( not (field.IsStruct() or field.IsDeviceAddress()) );

		const bool	packed = AnyBits( field.flags, EFlags::Packed | EFlags::PackedAlias );

		if ( field.IsScalar() )
		{
			SWITCH_TYPE( , ,
				case EValueType::DeviceAddress : return SizeAndAlignOf< Graphics::DeviceAddress >;
			);
		}
		else
		if ( field.IsVec() and packed )
		{
			switch ( field.rows )
			{
				case 2 :	SWITCH_TYPE( packed_, 2,  case EValueType::DeviceAddress: );	break;
				case 3 :	SWITCH_TYPE( packed_, 3,  case EValueType::DeviceAddress: );	break;
				case 4 :	SWITCH_TYPE( packed_, 4,  case EValueType::DeviceAddress: );	break;
				default :	CHECK_THROW_MSG( false, "unsupported vector size" );
			}
		}
		else
		if ( field.IsVec() and (not packed) )
		{
			switch ( field.rows )
			{
				case 2 :	SWITCH_TYPE( , 2,  case EValueType::DeviceAddress: );	break;
				case 3 :	SWITCH_TYPE( , 3,  case EValueType::DeviceAddress: );	break;
				case 4 :	SWITCH_TYPE( , 4,  case EValueType::DeviceAddress: );	break;
				default :	CHECK_THROW_MSG( false, "unsupported vector size" );
			}
		}
		else
		if ( field.IsMat() and packed )
		{
			switch ( uint(field.cols)*10 + uint(field.rows) )
			{
				case 22 :	SWITCH_MAT_TYPE( packed_, 2x2_storage );	break;
				case 23 :	SWITCH_MAT_TYPE( packed_, 2x3_storage );	break;
				case 24 :	SWITCH_MAT_TYPE( packed_, 2x4_storage );	break;
				case 32 :	SWITCH_MAT_TYPE( packed_, 3x2_storage );	break;
				case 33 :	SWITCH_MAT_TYPE( packed_, 3x3_storage );	break;
				case 34 :	SWITCH_MAT_TYPE( packed_, 3x4_storage );	break;
				case 42 :	SWITCH_MAT_TYPE( packed_, 4x2_storage );	break;
				case 43 :	SWITCH_MAT_TYPE( packed_, 4x3_storage );	break;
				case 44 :	SWITCH_MAT_TYPE( packed_, 4x4_storage );	break;
				default :	CHECK_THROW_MSG( false, "unsupported matrix size" );
			}
		}
		else
		if ( field.IsMat() and (not packed) and (not std140) )
		{
			switch ( uint(field.cols)*10 + uint(field.rows) )
			{
				case 22 :	SWITCH_MAT_TYPE( , 2x2_storage );	break;
				case 23 :	SWITCH_MAT_TYPE( , 2x3_storage );	break;
				case 24 :	SWITCH_MAT_TYPE( , 2x4_storage );	break;
				case 32 :	SWITCH_MAT_TYPE( , 3x2_storage );	break;
				case 33 :	SWITCH_MAT_TYPE( , 3x3_storage );	break;
				case 34 :	SWITCH_MAT_TYPE( , 3x4_storage );	break;
				case 42 :	SWITCH_MAT_TYPE( , 4x2_storage );	break;
				case 43 :	SWITCH_MAT_TYPE( , 4x3_storage );	break;
				case 44 :	SWITCH_MAT_TYPE( , 4x4_storage );	break;
				default :	CHECK_THROW_MSG( false, "unsupported matrix size" );
			}
		}
		else
		if ( field.IsMat() and (not packed) and std140 )
		{
			switch ( uint(field.cols)*10 + uint(field.rows) )
			{
				case 22 :	SWITCH_MAT_TYPE( , 2x2_storage_std140 );	break;
				case 23 :	SWITCH_MAT_TYPE( , 2x3_storage_std140 );	break;
				case 24 :	SWITCH_MAT_TYPE( , 2x4_storage_std140 );	break;
				case 32 :	SWITCH_MAT_TYPE( , 3x2_storage_std140 );	break;
				case 33 :	SWITCH_MAT_TYPE( , 3x3_storage_std140 );	break;
				case 34 :	SWITCH_MAT_TYPE( , 3x4_storage_std140 );	break;
				case 42 :	SWITCH_MAT_TYPE( , 4x2_storage_std140 );	break;
				case 43 :	SWITCH_MAT_TYPE( , 4x3_storage_std140 );	break;
				case 44 :	SWITCH_MAT_TYPE( , 4x4_storage_std140 );	break;
				default :	CHECK_THROW_MSG( false, "unsupported matrix size" );
			}
		}

		CHECK_THROW_MSG( false, "unknown field type" );

	#undef SWITCH_MAT_TYPE
	#undef SWITCH_TYPE
	}

	SizeAndAlign  ShaderStructType::_GetCPPSizeAndAlign (const Field &field, EStructLayout layout) __Th___
	{
		auto [size, align] = _GetCPPSizeAndAlign2( field, IsStd140( layout ));

		if ( field.IsDynamicArray() )
			size = 0_b;

		size = AlignUp( size, align );

		if ( IsStd140( layout ))
		{
			if ( field.IsArray() )
				align = Max( align, 16_b );
		}

		if ( field.IsStaticArray() )
		{
			size = AlignUp( size, align );
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
		CHECK_THROW_MSG( field.IsScalar() or field.IsVec() or field.IsMat(), "unknown field type" );
		CHECK_THROW_MSG( field.rows >= 1 and field.rows <= 4 );
		CHECK_THROW_MSG( field.cols >= 1 and field.cols <= 4 );

		const Bytes	scalar_size = ValueTypeSizeOf( field.type );

		if ( field.IsPacked() )
		{
			CHECK_THROW( field.align == scalar_size );
			return SizeAndAlign{ scalar_size * field.cols * field.rows, scalar_size };
		}

		const uint	rows	= field.rows == 3 ? 4 : field.rows;
		const uint	count	= rows * field.cols;

		CHECK_THROW_MSG( field.align == scalar_size * rows );

		return SizeAndAlign{ scalar_size * count, scalar_size * rows };
	}

	SizeAndAlign  ShaderStructType::_GetMSLSizeAndAlign (const Field &field, EStructLayout layout) __Th___
	{
		CHECK_THROW_MSG( NoBits( field.flags, EFlags::PackedAlias ),
			"'vec3 + scalar' is not supported in Metal, use packed vector type" );

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
	SizeAndAlign  ShaderStructType::_GetGLSLSizeAndAlign (const Field &field, EStructLayout layout) __Th___
	{
		const Bytes	scalar_size = ValueTypeSizeOf( field.type );
		Bytes		size		= scalar_size;
		Bytes		align		= scalar_size;

		if ( field.IsDynamicArray() )
			size = 0_b;

		if ( not field.IsPacked() )
			align *= (field.rows == 3 ? 4 : field.rows);

		size *= field.rows;

		if ( not AllBits( field.flags, EFlags::PackedAlias ))
			size = AlignUp( size, align );

		if ( IsStd140( layout ))
		{
			if ( not field.IsPacked() and field.IsMat() )
				align = Max( align, 16_b );

			if ( field.cols > 1 )
			{
				size = AlignUp( size, align );
				size = AlignUp( size * field.cols, align );
			}

			if ( field.IsArray() )
				align = Max( align, 16_b );
		}
		else
		{
			if ( field.cols > 1 )
			{
				size = AlignUp( size, align );
				size = AlignUp( size * field.cols, align );
			}
		}

		if ( field.IsStaticArray() )
		{
			size  = AlignUp( size, align );
			size  = AlignUp( size * field.arraySize, align );
		}

		return SizeAndAlign{ size, align };
	}

/*
=================================================
	_GetHLSLSizeAndAlign
=================================================
*/
	SizeAndAlign  ShaderStructType::_GetHLSLSizeAndAlign (const Field &field, EStructLayout layout) __Th___
	{
		const Bytes	scalar_size = ValueTypeSizeOf( field.type );
		Bytes		size		= scalar_size;
		Bytes		align		= scalar_size;

		if ( field.IsDynamicArray() )
			size = 0_b;

		// 5. Vectors are aligned by the size of a single vector element type
		// unless that alignment results in crossing the 16-byte row boundary, in which case it is aligned to the next row.

		if ( not field.IsPacked() )
			align *= (field.rows == 3 ? 4 : field.rows);

		size *= field.rows;

		if ( not AllBits( field.flags, EFlags::PackedAlias ))
			size = AlignUp( size, align );

		if ( IsStd140( layout ))
		{
			// 9.5.3 Constant Buffer Layout
			if ( not field.IsPacked() and field.IsMat() )
				align = Max( align, 16_b );

			if ( field.cols > 1 )
			{
				size = AlignUp( size, align );
				size = AlignUp( size * field.cols, align );
			}

			// 7. Individual array elements are always 16-byte row aligned.
			if ( field.IsArray() )
				align = Max( align, 16_b );
		}
		else
		{
			if ( field.cols > 1 )
			{
				size = AlignUp( size, align );
				size = AlignUp( size * field.cols, align );
			}
		}

		if ( field.IsStaticArray() )
		{
			size  = AlignUp( size, align );
			size  = AlignUp( size * field.arraySize, align );
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

		switch_enum( data.layout )
		{
			case EStructLayout::Compatible_Std140 :
			case EStructLayout::Compatible_Std430 :
				CHECK_THROW_MSG( data.glslOffset == offset );
				CHECK_THROW_MSG( data.mslOffset  == offset );
				CHECK_THROW_MSG( data.hlslOffset == offset );
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
			case EStructLayout::HLSL_Const :
			case EStructLayout::HLSL_Struct :
				CHECK_THROW_MSG( data.hlslOffset == offset );
				CHECK_THROW_MSG( data.cppOffset  == offset );
				break;
			case EStructLayout::InternalIO :
				// ignore offsets
				break;
			case EStructLayout::_Count :
			case EStructLayout::Unknown :
			default :				CHECK_THROW_MSG( false );
		}
		switch_end
	}

/*
=================================================
	_Validate
=================================================
*/
	void  ShaderStructType::_Validate (StringView prefix, StringView stName, ArrayView<Field> fields, INOUT ValidationData &data) __Th___
	{
		const bool	is_internal_io	= data.layout == EStructLayout::InternalIO;
		const bool	glsl_compat		= IsGLSLCompatible( data.layout );
		const bool	msl_compat		= IsMSLCompatible( data.layout );
		const bool	hlsl_compat		= IsHLSLCompatible( data.layout );

		for (auto& field : fields)
		{
			if ( field.IsAnyPadding() )
				continue;

			CHECK( field.IsDeviceAddress() == (field.IsBufferRef() or field.IsPointer() or field.IsUntypedDeviceAddress()) );

			if ( AnyBits( field.flags, EFlags_ShaderIO ))
			{
				CHECK_THROW_MSG( is_internal_io,
					"Struct '"s << stName << "' field '" << field.name << "' has shader Input/Output qualifiers which requires 'InternalIO' layout." );

				CHECK_THROW_MSG( (field.IsScalar() or field.IsVec()) and not (field.IsDeviceAddress() or field.IsPointer()),
					"Struct '"s << stName << "' field '" << field.name << "' has shader Input/Output qualifiers which is only compatible with scalar or vector type" );
			}

			if ( field.IsDeviceAddress() )
			{
				TEST_FEATURE_MSG( data.features, bufferDeviceAddress, ", required for field '"s << field.name << "'" );

				const Bytes		ptr_size	= 8_b;
				const Bytes		ptr_align	= 8_b;

				data.mslOffset	= AlignUp( data.mslOffset,  ptr_align );
				data.hlslOffset	= AlignUp( data.hlslOffset, ptr_align );
				data.glslOffset	= AlignUp( data.glslOffset, ptr_align );
				data.cppOffset	= AlignUp( data.cppOffset,  ptr_align );

				_ValidateOffsets( data, field.offset );

				const ulong	arr_size = field.IsStaticArray() ? field.arraySize : 1;

				data.mslOffset	+= ptr_size * arr_size;
				data.hlslOffset	+= ptr_size * arr_size;
				data.glslOffset	+= ptr_size * arr_size;
				data.cppOffset	+= ptr_size * arr_size;
			}
			else
			if ( field.IsStruct() )
			{
				CHECK_THROW_MSG( IsCompatibleLayouts( data.layout, field.stType->Layout() ),
					"Struct '"s << stName << "' field '" << field.name << "' uses Struct '" << field.stType->Name() << "' with layout '" <<
					EStructLayout_ToString(field.stType->Layout()) << "' which is not compatible with layout '" << EStructLayout_ToString(data.layout) << "'" );

				const Bytes		st_align = field.stType->_structAlign;
				data.baseOffset	= AlignUp( data.baseOffset, st_align );
				data.mslOffset	= AlignUp( data.mslOffset,  st_align );
				data.hlslOffset	= AlignUp( data.hlslOffset, st_align );
				data.glslOffset	= AlignUp( data.glslOffset, st_align );
				data.cppOffset	= AlignUp( data.cppOffset,  st_align );

				_ValidateOffsets( data, field.offset );

				String	str{prefix};
				if ( not str.empty() )	str << '.';

				ValidationData	data2 = data;
				data2.baseOffset += field.offset;

				_Validate( (str << field.name), field.stType->Name(), field.stType->_fields, INOUT data2 );

				if ( not field.IsDynamicArray() )
				{
					const ulong	arr_size = field.IsStaticArray() ? field.arraySize : 1;

					data.mslOffset	+= AlignUp( data2.mslOffset  - data.mslOffset,  st_align ) * arr_size;
					data.hlslOffset	+= AlignUp( data2.hlslOffset - data.hlslOffset, st_align ) * arr_size;
					data.glslOffset	+= AlignUp( data2.glslOffset - data.glslOffset, st_align ) * arr_size;
					data.cppOffset	+= AlignUp( data2.cppOffset  - data.cppOffset,  st_align ) * arr_size;
				}
			}
			else
			{
				switch_enum( field.type )
				{
					case EValueType::Bool32 :
					case EValueType::Int32 :
					case EValueType::UInt32 :
					case EValueType::Float32 :
						break;	// default types

					case EValueType::Int8_Norm :
					case EValueType::UInt8_Norm :
					case EValueType::Int16_Norm :
					case EValueType::UInt16_Norm :
						break;	// same as float

					case EValueType::Int64 :
					case EValueType::UInt64 :
						TEST_FEATURE_MSG( data.features, shaderInt64, ", required for field '"s << field.name << "'" );
						break;

					case EValueType::Float64 :
						TEST_FEATURE_MSG( data.features, shaderFloat64, ", required for field '"s << field.name << "'" );
						break;

					case EValueType::Float16 :
						TEST_FEATURE_MSG( data.features, shaderFloat16, ", required for field '"s << field.name << "'" );
						break;

					case EValueType::Bool8 :
					case EValueType::Int8 :
					case EValueType::UInt8 :
						TEST_FEATURE_MSG( data.features, shaderInt8, ", required for field '"s << field.name << "'" );
						break;

					case EValueType::Int16 :
					case EValueType::UInt16 :
						TEST_FEATURE_MSG( data.features, shaderInt16, ", required for field '"s << field.name << "'" );
						break;

					case EValueType::DeviceAddress :
						TEST_FEATURE_MSG( data.features, bufferDeviceAddress, ", required for field '"s << field.name << "'" );
						break;

					case EValueType::Unknown :
					case EValueType::_Count :
					default :					CHECK_THROW_MSG( false, "unknown value type" );
				}
				switch_end

				Bytes	msl_offset	= data.mslOffset;
				Bytes	hlsl_offset	= data.hlslOffset;
				Bytes	glsl_offset	= data.glslOffset;
				Bytes	cpp_offset	= data.cppOffset;

				// Metal
				if ( msl_compat )
				{
					auto [size, align] = _GetMSLSizeAndAlign( field, data.layout );

					CHECK_THROW( field.IsDynamicArray() == (size == 0_b) );  // internal error
					CHECK_THROW_MSG( size == 0_b or size == field.size,
						"Struct '"s << stName << "' field '" << field.name << "' size mismatch for Metal backend: (" <<
						ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

					data.mslOffset	= AlignUp( data.mslOffset, align );
					msl_offset		= data.mslOffset + size;
				}

				// HLSL
				if ( hlsl_compat )
				{
					auto [size, align] = _GetHLSLSizeAndAlign( field, data.layout );

					CHECK_THROW( field.IsDynamicArray() == (size == 0_b) );  // internal error
					CHECK_THROW_MSG( size == 0_b or size == field.size,
						"Struct '"s << stName << "' field '" << field.name << "' size mismatch for HLSL backend: (" <<
						ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

					data.hlslOffset	= AlignUp( data.hlslOffset, align );
					hlsl_offset		= data.hlslOffset + size;
				}

				// GLSL
				if ( glsl_compat )
				{
					auto [size, align] = _GetGLSLSizeAndAlign( field, data.layout );

					CHECK_THROW( field.IsDynamicArray() == (size == 0_b) );  // internal error
					CHECK_THROW_MSG( size == 0_b or size == field.size,
						"Struct '"s << stName << "' field '" << field.name << "' size mismatch for GLSL backend: (" <<
						ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

					data.glslOffset	= AlignUp( data.glslOffset, align );
					glsl_offset		= data.glslOffset + size;
				}

				// CPP
				{
					auto [size, align] = _GetCPPSizeAndAlign( field, data.layout );

					CHECK_THROW( field.IsDynamicArray() == (size == 0_b) );  // internal error
					CHECK_THROW_MSG( size == 0_b or size == field.size,
						"Struct '"s << stName << "' field '" << field.name << "' size mismatch for C++ backend: (" <<
						ToString(uint(size)) << " == " << ToString(uint(field.size)) << ")" );

					data.cppOffset	= AlignUp( data.cppOffset, align );
					cpp_offset		= data.cppOffset + size;
				}

				_ValidateOffsets( data, field.offset );

				data.mslOffset	= msl_offset;
				data.hlslOffset	= hlsl_offset;
				data.glslOffset	= glsl_offset;
				data.cppOffset	= cpp_offset;
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
		const auto	Vec3Padding = [layout, &fields] (INOUT usize &i) -> bool
		{{
			auto&	field = fields[i];
			if_unlikely( field.IsVec() and field.rows == 3 and not field.IsPacked() )
			{
				if ( layout == EStructLayout::Std430 and AllBits( field.flags, EFlags::PackedAlias ))
					return false;

				Field	pad = field;

				pad.name	= "_"s + field.name + "_padding_w";
				pad.rows	= 1;
				pad.size	= field.size / 4;
				pad.offset	= field.offset + (field.size - pad.size);
				pad.align	= pad.size;
				pad.flags	|= EFlags::Padding_GLSL | EFlags::Padding_HLSL;
				ASSERT( pad.IsScalar() );

				++i;
				fields.insert( fields.begin()+i, RVRef(pad) );
				return true;
			}
			return false;
		}};

		switch_enum( layout )
		{
			case EStructLayout::Compatible_Std140 :
				for (usize i = 0; i < fields.size(); ++i)
				{
					auto&	field = fields[i];	// !!! don't use after 'insert()'

					if ( field.IsArray() )
						continue;

					if ( Vec3Padding( INOUT i ))
						continue;

					if_unlikely( field.IsStruct() and not field.IsArray() )
					{
						Bytes		begin	= field.offset + field.stType->StaticSize();
						const Bytes	end		= field.offset + field.size;

						if ( begin == end )
							continue;

						Field	pad;
						pad.name	= "_"s + field.name + "_padding";

						if ( end - begin >= 4 ){
							pad.type	= EValueType::Float32;
							pad.align	= 4_b;
						}else
						if ( end - begin >= 2 ){
							pad.type	= EValueType::Int16;
							pad.align	= 2_b;
						}else{
							pad.type	= EValueType::Int8;
							pad.align	= 1_b;
						}

						pad.offset	= AlignUp( begin, pad.align );
						pad.size	= end - pad.offset;
						pad.rows	= ubyte(pad.size / pad.align);
						pad.cols	= 1;
						pad.flags	= EFlags::Packed | EFlags::Padding_MSL;
						ASSERT( pad.IsPacked() and (pad.IsScalar() or pad.IsVec()) );

						++i;
						fields.insert( fields.begin()+i, RVRef(pad) );
						continue;
					}
				}
				break;

			case EStructLayout::Compatible_Std430 :
			case EStructLayout::Std430 :
			case EStructLayout::Std140 :
				for (usize i = 0; i < fields.size(); ++i)
				{
					auto&	field = fields[i];

					if ( field.IsArray() )	continue;

					Vec3Padding( INOUT i );
				}
				break;

			case EStructLayout::Metal :
			case EStructLayout::HLSL_Const :
			case EStructLayout::HLSL_Struct :
			case EStructLayout::InternalIO :
			case EStructLayout::_Count :
			case EStructLayout::Unknown :	break;
		}
		switch_end

	}

/*
=================================================
	constructor
=================================================
*/
	ShaderStructType::ShaderStructType (const String &name) __NE___ :
		_originName{ name },
		_typeName{ ValidateTypeName( name )},
		_features{ ObjectStorage::Instance()->GetDefaultFeatureSets() }
	{}

	ShaderStructTypePtr  ShaderStructType::Create (const String &name) __Th___
	{
		ShaderStructTypePtr	result{ new ShaderStructType{ name }};

		ObjectStorage::Instance()->AddName<ShaderStructName>( name );
		CHECK_THROW_MSG( ObjectStorage::Instance()->structTypes.emplace( name, result ).second,
			"StructureType with name '"s << name << "' is already defined" );

		return result;
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

		auto&	storage = *ObjectStorage::Instance();
		auto	fs_it	= storage.featureSets.find( FeatureSetName{name} );
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

		_CalcOffsets( Name(), layout, INOUT _fields, OUT _maxAlign, OUT _structAlign, OUT _size );
		CHECK_THROW_MSG( _maxAlign > 0 );
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

		ValidationData	validation_data{ _features, _layout };
		{
			_Validate( "", Name(), _fields, INOUT validation_data );
			_ValidateOffsets( validation_data, HasDynamicArray() ? AlignUp( _size, _fields.back().align ) : _size );
		}

		_AddPadding( _layout, INOUT _fields );

		auto&	storage = *ObjectStorage::Instance();

		// GLSL
		if ( storage.spirvCompiler != null and IsGLSLCompatible( _layout ))
		{
			String	dsl_hdr, dsl_src;
			CHECK_THROW_MSG( ToGLSL( true, OUT dsl_hdr, OUT dsl_src ));
			ASSERT( IsStd140( _layout ) or IsStd430( _layout ));

			Version2	spv_ver {1,0};
			String		header, source;

			header	<< storage.GetShaderExtensionsGLSL( INOUT spv_ver, EShaderStages::Fragment, false, _features )
					<< dsl_hdr
					<< "layout(set=0, binding=0, "
					<< (IsStd430( _layout ) ? "std430" : "std140")
					<< ") buffer BufType {\n"
					<< dsl_src
					<< "} buf;\n\n";
			source << "void Main () {}";

			SpirvCompiler::Input	in;
			in.shaderType	= EShader::Fragment;
			in.spirvVersion	= spv_ver;
			in.entry		= "Main";
			in.header		= header;
			in.source		= source;

			SpirvCompiler::ShaderReflection	refl;
			String							log;

			CHECK_THROW_MSG( storage.spirvCompiler->BuildReflection( in, OUT refl, OUT log ),
				"Failed to compile temp shader:\n"s << log );

			CHECK_THROW_MSG( refl.layout.descrSets.size() == 1, "internal error" );
			CHECK_THROW_MSG( refl.layout.descrSets.front().layout.uniforms.size() == 1, "internal error" );

			auto&	un = refl.layout.descrSets.front().layout.uniforms[0];
			ASSERT( un.first == UniformName{"buf"} );
			CHECK_THROW_MSG( un.second.type == EDescriptorType::UniformBuffer or
							 un.second.type == EDescriptorType::StorageBuffer,
							 "internal error" );

			validation_data.glslOffset = AlignUp( validation_data.glslOffset, _maxAlign );

			CHECK_THROW_MSG( validation_data.glslOffset == Bytes{un.second.buffer.staticSize} );
			CHECK_THROW_MSG( ArrayStride() == Bytes{un.second.buffer.arrayStride} );
		}

		// MSL
		if ( storage.metalCompiler != null and IsMSLCompatible( _layout ))
		{
			String	dsl_src;
			CHECK_THROW_MSG( ToMSL( OUT dsl_src ));

			Version2	msl_ver {2,0};
			String		str;
			str << storage.GetShaderExtensionsMSL( INOUT msl_ver, EShaderStages::Fragment, _features )
				<< dsl_src
				<< "void Main (constant " << Typename() << "& buf [[buffer(0)]]) {}";

			MetalCompiler::Input	in;
			in.target				= ECompilationTarget::Metal_Mac;
			in.options				= Default;
			in.version				= EShaderVersion::Metal_Mac_2_3;
			in.source				= str;
			in.enablePreprocessing	= false;

			MetalBytecode_t	bytecode;
			String			log;
			CHECK_THROW_MSG( storage.metalCompiler->Compile( in, OUT bytecode, OUT log ),
				"MSL shader struct type validation failed:\n"s << log );

			CHECK_THROW( not bytecode.empty() ); // internal error
		}

		// Slang / HLSL
		if ( storage.slangCompiler != null						and
			 storage.slangCompiler->IsInitialized()				and
			 IsHLSLCompatible( _layout ) and not HasDynamicArray() )
		{
			String	dsl_src;
			String	test;
			CHECK_THROW_MSG( ToHLSL( OUT dsl_src, OUT &test ));
			ASSERT( IsStd140( _layout ) or IsStd430( _layout ));

			String	header, source;
			header	<< "typedef uint64_t DeviceAddress;\n"
					<< dsl_src
					<< "[[vk::binding(1,2)]] "
					<< (IsStd430( _layout ) ? "RWStructuredBuffer" : "ConstantBuffer")
					<< "<" << Typename() << "> buf : register(" << (IsStd430( _layout ) ? "u0" : "b0") << ");\n\n";
			source << "[shader(\"compute\")]\n[numthreads(1, 1, 1)]\nvoid Main () {\n" << test << "\n}";

			SLangCompiler::Input	in;
			in.entry		= "Main";
			in.header		= header;
			in.source		= source;
			in.dstVersion	= EShaderVersion::SPIRV_1_5;	// TODO

			SLangCompiler::Output	out;

			CHECK_THROW_MSG( storage.slangCompiler->Compile( in, out ),
				"Failed to compile temp shader:\n"s << out.log );

			CHECK_THROW( not out.spirv.empty() ); // internal error

			auto&	refl = out.reflection;
			CHECK_THROW_MSG( refl.layout.descrSets.size() == 1, "internal error" );
			CHECK_THROW_MSG( refl.layout.descrSets.front().layout.uniforms.size() == 1, "internal error" );

			auto&	un = refl.layout.descrSets.front().layout.uniforms[0];
			ASSERT( un.first == UniformName{"buf"} );
			CHECK_THROW_MSG( un.second.type == EDescriptorType::UniformBuffer or
							 un.second.type == EDescriptorType::StorageBuffer,
							 "internal error" );

			Bytes	static_size = Bytes{un.second.buffer.staticSize};
			if ( not _fields.empty()	and
				 (_fields.back().IsVec() or _fields.back().IsMat())	and
				 _fields.back().rows == 3 )
			{
				static_size = AlignUp( static_size, _fields.back().align );
			}
			CHECK_THROW_MSG( validation_data.hlslOffset == static_size );
			CHECK_THROW_MSG( ArrayStride() == Bytes{un.second.buffer.arrayStride} );
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

		CHECK_ERR_MSG( not field.IsArray(),
			"Field '"s << field.name << "': can not use packed type in array when used std140 layout" );

		const char	vec_field_names[] = "xyzw";

		str << "// size: " << ToString(usize( field.size )) << ", align: " << ToString(usize( field.align )) << "\n"
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
		String	short_name {packedTypeName.substr( packedTypeName.find( '_' )+1 )};  // remove 'inplace_'
		CHECK_ERR( not short_name.empty() );
		short_name[0] = ToUpperCase( short_name[0] );

		String	mat_field = short_name.substr( 0, short_name.size()-3 ) + short_name.substr( short_name.size()-1 );

		str << "#define GetInplace" << short_name << "( _fieldName_ )  " << dstType << "( ";

		if ( field.IsVec() )
		{
			for (ubyte r = 0; r < field.rows; ++r) {
				str << (r ? ", " : "") << "(_fieldName_ ## _" << vec_field_names[r] << ")";
			}
		}else
		if ( field.IsMat() )
		{
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

		const char	vec_field_names[] = "xyzw";

		str << "// size: " << ToString(usize( field.size )) << ", align: " << ToString(usize( field.align )) << "\n"
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

		// example: 'float3  Unpack (const packed_float3 src) ...'
		str << dstType << "  Unpack" << " (const " << packedTypeName << " src) { return " << dstType << "( ";
		if ( field.IsVec() )
		{
			for (ubyte r = 0; r < field.rows; ++r) {
				str << (r ? ", " : "") << "src." << vec_field_names[r];
			}
		}else
		if ( field.IsMat() )
		{
			for (ubyte c = 0; c < field.cols; ++c) {
				str << (c ? ", " : "") << "Unpack(src.c" << ToString( uint(c) ) << ")";
			}
		}
		str << " ); }\n";

		// example: 'packed_float3  Pack (const float3 src) ...'
		str << packedTypeName << "  Pack" << " (const " << dstType << " src) { return " << packedTypeName << "( ";
		if ( field.IsVec() )
		{
			for (ubyte r = 0; r < field.rows; ++r) {
				str << (r ? ", " : "") << "src." << vec_field_names[r];
			}
		}else
		if ( field.IsMat() )
		{
			for (ubyte c = 0; c < field.cols; ++c) {
				str << (c ? ", " : "") << "Pack(src[" << ToString( uint(c) ) << "])";
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
		switch_enum( type )
		{
			case EValueType::Int8 :
			case EValueType::Int8_Norm :	s_name = "int8_t";		v_name = "i8vec";	break;
			case EValueType::UInt8 :
			case EValueType::UInt8_Norm :	s_name = "uint8_t";		v_name = "u8vec";	break;
			case EValueType::Int16 :
			case EValueType::Int16_Norm :	s_name = "int16_t";		v_name = "i16vec";	break;
			case EValueType::UInt16 :
			case EValueType::UInt16_Norm :	s_name = "uint16_t";	v_name = "u16vec";	break;
			case EValueType::Bool32 :		s_name = "bool";		v_name = "bvec";	break;
			case EValueType::Int32 :		s_name = "int";			v_name = "ivec";	break;
			case EValueType::UInt32 :		s_name = "uint";		v_name = "uvec";	break;
			case EValueType::Int64 :		s_name = "int64_t";		v_name = "i64vec";	break;
			case EValueType::DeviceAddress:	s_name = "uvec2";							break;
			case EValueType::UInt64 :		s_name = "uint64_t";	v_name = "u64vec";	break;
			case EValueType::Float16 :		s_name = "float16_t";	v_name = "f16vec";	m_name = "f16mat";	break;
			case EValueType::Float32 :		s_name = "float";		v_name = "vec";		m_name = "mat";		break;
			case EValueType::Float64 :		s_name = "double";		v_name = "dvec";	m_name = "dmat";	break;

			case EValueType::Bool8 :		// TODO
			case EValueType::Unknown :
			case EValueType::_Count :
			default :						RETURN_ERR( "unknown value type" );
		}
		switch_end
		return true;
	}
}
/*
=================================================
	ValueTypeToStrGLSL_WithPrecision
----
			|   fp range   |  fp magnitude  | fp precision |   int range   |
	--------|--------------|----------------|--------------|---------------|
	highp	| -2^62; 2^62  |  2^-62;  2^62  |     2^-16    | -2^16;  2^16  |
	mediump	| -2^14; 2^14  |  2^-14;  2^14  |     2^-10    | -2^10;  2^10  |
	lowp	| -2;    2     |  2^-8;   2     |     2^-8     | -2^8;   2^8   |
=================================================
*/
namespace {
	ND_ static bool  ValueTypeToStrGLSL_WithPrecision (const EValueType type, EFlags flags, const EFlags flagMask,
													   OUT String &qual, OUT StringView &s_name, OUT StringView &v_name, OUT StringView &m_name)
	{
		if ( type >= EValueType::Int8_Norm and type <= EValueType::UInt16_Norm )
		{
			// add precision
			flags = (flags & ~EFlags_Precision) | EFlags::MediumPrecision;
		}

		if ( type <= EValueType::UInt64 )
			flags |= EFlags::FlatInterpolation;

		for (EFlags f : BitfieldIterate( (flags & flagMask) & EFlags_ShaderIO ))
		{
			switch_enum( f )
			{
				case EFlags::MediumPrecision :				qual << "mediump ";			break;
				case EFlags::FlatInterpolation :			qual << "flat ";			break;
				case EFlags::NoPerspectiveInterpolation :	qual << "noperspective ";	break;
				case EFlags::CentroidInterpolation :		qual << "centroid ";		break;
				case EFlags::PerSampleInterpolation :		qual << "sample ";			break;
				case EFlags::Invariant :					qual << "invariant ";		break;

				case EFlags::LowPrecision :
				case EFlags::Unknown :
				case EFlags::Packed :
				case EFlags::PackedAlias :
				case EFlags::Padding_GLSL :
				case EFlags::Padding_MSL :
				case EFlags::Padding_HLSL :
				case EFlags::Address :
				case EFlags::Pointer :
				case EFlags::Atomic :
				case EFlags::_BITOPS_ :
				default :				RETURN_ERR( "unsupported qualifier" );
			}
			switch_end
		}

		switch_enum( type )
		{
			// integer
			case EValueType::Int8 :			s_name = "int8_t";		v_name = "i8vec";	break;
			case EValueType::UInt8 :		s_name = "uint8_t";		v_name = "u8vec";	break;
			case EValueType::Int16 :		s_name = "int16_t";		v_name = "i16vec";	break;
			case EValueType::UInt16 :		s_name = "uint16_t";	v_name = "u16vec";	break;
			case EValueType::Bool32 :		s_name = "bool";		v_name = "bvec";	break;
			case EValueType::Int32 :		s_name = "int";			v_name = "ivec";	break;
			case EValueType::UInt32 :		s_name = "uint";		v_name = "uvec";	break;
			case EValueType::Int64 :		s_name = "int64_t";		v_name = "i64vec";	break;
			case EValueType::UInt64 :		s_name = "uint64_t";	v_name = "u64vec";	break;

			// normalized integer
			case EValueType::Int8_Norm :
			case EValueType::UInt8_Norm :
			case EValueType::Int16_Norm :
			case EValueType::UInt16_Norm :	s_name = "float";		v_name = "vec";		break;

			// float point
			case EValueType::Float16 :		s_name = "float16_t";	v_name = "f16vec";	m_name = "f16mat";		break;
			case EValueType::Float32 :		s_name = "float";		v_name = "vec";		m_name = "mat";			break;
			case EValueType::Float64 :		s_name = "double";		v_name = "dvec";	m_name = "dmat";		break;

			case EValueType::Bool8 :		// TODO
			case EValueType::DeviceAddress:
			case EValueType::Unknown :
			case EValueType::_Count :
			default :						RETURN_ERR( "unknown value type" );
		}
		switch_end
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
		switch_enum( type )
		{
			case EValueType::Bool8 :		src << "bool";			break;
			case EValueType::Bool32 :		src << "lbool";			break;
			case EValueType::Int8 :
			case EValueType::Int8_Norm :	src << "sbyte";			break;
			case EValueType::UInt8 :
			case EValueType::UInt8_Norm :	src << "ubyte";			break;
			case EValueType::Int16 :
			case EValueType::Int16_Norm :	src << "short";			break;
			case EValueType::UInt16 :
			case EValueType::UInt16_Norm :	src << "ushort";		break;
			case EValueType::Int32 :		src << "int";			break;
			case EValueType::UInt32 :		src << "uint";			break;
			case EValueType::Int64 :		src << "slong";			break;
			case EValueType::UInt64 :		src << "ulong";			break;
			case EValueType::Float16 :		src << "half";			break;
			case EValueType::Float32 :		src << "float";			break;
			case EValueType::Float64 :		src << "double";		break;
			case EValueType::DeviceAddress:	src << "DeviceAddress";	break;

			case EValueType::Unknown :
			case EValueType::_Count :
			default :						RETURN_ERR( "unknown value type" );
		}
		switch_end
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
		const bool	is_std140 = IsStd140( _layout );

		const auto	GetScalarName = [&uniqueTypes, &outTypes] (bool isStd140, const Field &field, INOUT String &str) -> bool
		{{
			StringView	s_name, v_name, m_name;
			CHECK_ERR( ValueTypeToStrGLSL( field.type, OUT s_name, OUT v_name, OUT m_name ));

			String	tname;
			if ( field.IsScalar() )	tname << s_name;
			if ( field.IsVec() )	tname << v_name << ToString( field.rows );
			if ( field.IsMat() )	tname << m_name << ToString( field.cols ) << 'x' << ToString( field.rows );
			CHECK_ERR( not tname.empty() );

			if ( not field.IsPacked() )
			{
				str << tname;
				return true;
			}

			String	public_name;
			CHECK_ERR( ValueTypeToStrCPP( field.type, OUT public_name ));

			// Prefix 'inplace_' - packed in the same place, because in 'std140' structure aligned to 16 bytes

			String	packed	= (isStd140 ? "inplace_"s : "packed_"s) << public_name;
			String	memt	{s_name};

			if ( field.IsVec() )
				packed << ToString( field.rows );

			if ( field.IsMat() )
			{
				memt = (isStd140 ? "inplace_"s : "packed_"s) << public_name << ToString( field.rows );
				packed << ToString( field.cols ) << 'x' << ToString( field.rows );

				// add packed vec type
				if ( uniqueTypes.insert( memt ).second )
				{
					Field	f;
					f.type		= field.type;
					f.rows		= field.rows;
					f.cols		= 1;
					f.arraySize	= 0;	// non-array
					f.align		= ValueTypeSizeOf( field.type );
					f.size		= f.align * field.rows;

					if ( not field.IsPointer() )
						ASSERT( field.align == f.align );

					if ( isStd140 ) {
						CHECK_ERR( _CreatePackedTypeGLSL1( INOUT outTypes, memt, s_name, (String{v_name} << ToString( field.rows )), f ));
					}else{
						CHECK_ERR( _CreatePackedTypeGLSL2( INOUT outTypes, memt, s_name, (String{v_name} << ToString( field.rows )), f ));
					}
				}
			}

			if ( uniqueTypes.insert( packed ).second )
			{
				Field	f;
				f.type		= field.type;
				f.rows		= field.rows;
				f.cols		= field.cols;
				f.arraySize	= 0;	// non-array
				f.align		= ValueTypeSizeOf( field.type );
				f.size		= f.align * f.rows * f.cols;

				if ( not field.IsPointer() )
					ASSERT( field.align == f.align );

				ASSERT( field.IsMat() == f.IsMat() );
				ASSERT( field.IsVec() == f.IsVec() );

				if ( isStd140 ) {
					CHECK_ERR( _CreatePackedTypeGLSL1( INOUT outTypes, packed, memt, tname, f ));
				}else{
					CHECK_ERR( _CreatePackedTypeGLSL2( INOUT outTypes, packed, memt, tname, f ));
				}
			}
			str << packed;
			return true;
		}};

		const auto	GetBufferRefName = [withOffsets, &uniqueTypes, &outTypes] (const Field &field, OUT String &type_name) -> bool
		{{
			type_name = String{field.stType->Typename()} + "_AERef";

			if ( uniqueTypes.insert( type_name ).second )
			{
				const bool	is_std140_2 = IsStd140( field.stType->Layout() );

				String	fields;
				CHECK_ERR( field.stType->ToGLSL( withOffsets, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));
				outTypes
					<< "layout(" << (is_std140_2 ? "std140" : "std430") << ", buffer_reference, buffer_reference_align="
					<< ToString(ulong( field.stType->Align() )) << ") buffer " << type_name << "\n{\n" << fields << "};\n\n";
			}
			return true;
		}};


		CHECK_ERR( Layout() == EStructLayout::InternalIO or IsGLSLCompatible( Layout() ));
		CHECK_ERR( IsMultipleOf( baseOffset, _maxAlign ));

		Array<Tuple< String, String, String, String >>		field_parts;	// offset, typename, name, comment

		for (auto& field : _fields)
		{
			// padding needed for structure declaration, buffer declaration has explicit offsets
			if ( withOffsets and field.IsAnyPadding() )
				continue;

			if ( AnyBits( field.flags, EFlags::Padding_MSL | EFlags::Padding_HLSL ) and
				 NoBits( field.flags, EFlags::Padding_GLSL ))
				continue;

			auto& [part0, part1, part2, part3] = field_parts.emplace_back();

			part0 << "\t";
			if ( withOffsets )
				part0 << "layout(offset=" << ToString(usize( field.offset + baseOffset )) << ", align=" << ToString(usize( field.align )) << ")  ";

			if ( field.IsBufferRef() and field.IsPointer() )
			{
				String	type_name;
				CHECK_ERR( GetBufferRefName( field, OUT type_name ));

				const String	ptr_name = String{field.stType->Typename()} + "_AEPtr";

				if ( uniqueTypes.insert( ptr_name ).second )
				{
					String	fields;
					CHECK_ERR( field.stType->ToGLSL( withOffsets, INOUT outTypes, INOUT fields, INOUT uniqueTypes ));
					outTypes
						<< "layout(std430, buffer_reference, buffer_reference_align="
						<< ToString(ulong( field.stType->Align() )) << ") buffer " << ptr_name << "\n{\n\t"
						<< type_name << "  data [];\n"
						<< "};\n\n";
				}
				part1 << ptr_name;
				part2 << field.name;
			}
			else
			if ( field.IsBufferRef() )
			{
				String	type_name;
				CHECK_ERR( GetBufferRefName( field, OUT type_name ));
				part1 << type_name;
				part2 << field.name;
			}
			else
			if ( field.IsPointer() )
			{
				StringView	s_name, v_name, m_name;
				CHECK_ERR( ValueTypeToStrGLSL( field.type, OUT s_name, OUT v_name, OUT m_name ));

				String		elem_name;
				CHECK_ERR( GetScalarName( false, field, INOUT elem_name ));

				String		type_name;
				CHECK_ERR( ValueTypeToStrCPP( field.type, OUT type_name ));
				if ( field.IsPacked() )	"packed_" >> type_name;
				if ( field.IsVec() )	type_name << ToString( field.rows );
				if ( field.IsMat() )	type_name << ToString( field.cols ) << 'x' << ToString( field.rows );
				CHECK_ERR( not type_name.empty() );
				type_name << "_AEPtr";

				const SizeAndAlign	size_align	= _GetGLSLSizeAndAlign( field, EStructLayout::Std430 );

				if ( uniqueTypes.insert( type_name ).second )
				{
					outTypes
						<< "layout(std430, buffer_reference, buffer_reference_align="
						<< ToString(ulong( size_align.align )) << ") buffer " << type_name << "\n{\n\t"
						<< elem_name << "  data [];\n"
						<< "};\n\n";
				}

				part1 << type_name;
				part2 << field.name;
			}
			else
			if ( field.IsStruct() )
			{
				const StringView	type_name = field.stType->Typename();

				CHECK_ERR( field.stType->StructToGLSL( INOUT outTypes, INOUT uniqueTypes ));

				part1 << type_name;
				part2 << field.name;
			}
			else
			// scalar, vec, mat and atomic types
			{
				CHECK_ERR( GetScalarName( is_std140, field, INOUT part1 ));

				if ( field.IsPacked() and is_std140 )
					part2 << "( " << field.name << " )";
				else
					part2 << field.name;
			}

			if ( field.IsDynamicArray() )
				part2 << " []";
			else
			if ( field.IsStaticArray() )
			{
				part2 << ' ';
				for (auto size : field.arraySizeChain)
					part2 << '[' << ToString(size) << ']';
			}
			part2 << ';';

			if ( withOffsets ){
				if ( not field.IsDynamicArray() )
					part3 << "// size: " << ToString(usize( field.size ));
			}else{
				part3 << "// offset: " << ToString(usize( field.offset ))
					<< ", align: " << ToString(usize( field.align ))
					<< ", size: " << ToString(usize( field.size ));
			}
		}

		// format fields
		usize	part0_len	= 0;
		usize	part1_len	= 0;
		usize	part2_len	= 0;
		usize	total_len	= 0;

		for (auto& [part0, part1, part2, part3] : field_parts)
		{
			part0_len  = Max( part0_len, part0.length() );
			part1_len  = Max( part1_len, part1.length() );
			part2_len  = Max( part2_len, part2.length() );
			total_len += part0.length() + part1.length() + part2.length() + part3.length();
		}
		part1_len = (part1_len == 0 ? 0 : part1_len + 2);
		part2_len = (part2_len == 0 ? 0 : part2_len + 2);

		outFields.reserve( outFields.size() + total_len );

		for (auto& [part0, part1, part2, part3] : field_parts)
		{
			outFields << part0;
			AppendToString( INOUT outFields, part0_len - part0.length(), ' ' );

			outFields << part1;
			AppendToString( INOUT outFields, part1_len - part1.length(), ' ' );

			outFields << part2;

			if ( not part3.empty() )
			{
				AppendToString( INOUT outFields, part2_len - part2.length(), ' ' );
				outFields << part3;
			}
			outFields << "\n";
		}
		return true;
	}

	bool  ShaderStructType::ToGLSL (bool withOffsets, INOUT String &types, INOUT String &fields, Bytes baseOffset) const
	{
		UniqueTypes_t	uniqueTypes;
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
			String	fields;
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

		str << "// size: " << ToString(usize( field.size )) << ", align: " << ToString(usize( field.align )) << "\n"
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
		switch_enum( type )
		{
			case EValueType::Bool8 :		src << "bool";		break;
			case EValueType::Int8 :
			case EValueType::Int8_Norm :	src << "char";		break;
			case EValueType::UInt8 :
			case EValueType::UInt8_Norm :	src << "uchar";		break;
			case EValueType::Int16 :
			case EValueType::Int16_Norm :	src << "short";		break;
			case EValueType::UInt16 :
			case EValueType::UInt16_Norm :	src << "ushort";	break;
			case EValueType::Int32 :		src << "int";		break;
			case EValueType::UInt32 :		src << "uint";		break;
			case EValueType::Int64 :		src << "long";		break;
			case EValueType::UInt64 :		src << "ulong";		break;
			case EValueType::Float16 :		src << "half";		break;
			case EValueType::Float32 :		src << "float";		break;
			case EValueType::DeviceAddress:	src << "void";		break;

			case EValueType::Bool32 :		// TODO
			case EValueType::Float64 :
			case EValueType::Unknown :
			case EValueType::_Count :
			default :						RETURN_ERR( "unknown value type" );
		}
		switch_end
		return true;
	}
}
/*
=================================================
	ValueTypeToStrMSL_WithNorm
=================================================
*/
namespace {
	ND_ static bool  ValueTypeToStrMSL_WithNorm (const EValueType type, EFlags flags, const EFlags flagMask, const uint rows, INOUT String &src, OUT String &qual)
	{
		const bool	no_persp = AllBits( flags, EFlags::NoPerspectiveInterpolation );

		if ( type <= EValueType::UInt64 )
			flags |= EFlags::FlatInterpolation;

		flags &= flagMask;

		if ( AllBits( flags, EFlags::FlatInterpolation ))
			qual << (qual.empty() ? "" : ", ") << "flat";

		if ( AllBits( flags, EFlags::CentroidInterpolation ))
			qual << (qual.empty() ? "" : ", ") << (no_persp ? "centroid_no_perspective" : "centroid_perspective");
		else
		if ( no_persp )
			qual << (qual.empty() ? "" : ", ") << "center_no_perspective";	// 'center_perspective' - default

		if ( AllBits( flags, EFlags::PerSampleInterpolation ))
			qual << (qual.empty() ? "" : ", ") << (no_persp ? "sample_no_perspective" : "sample_perspective");

		if ( not qual.empty() )
			"[[" >> (qual << "]]");

		// normalized types
		switch ( type )
		{
			case EValueType::Int8_Norm :
				switch ( rows ) {
					case 1 :	src << "r8snorm<half>";			return true;
					case 2 :	src << "rg8snorm<half2>";		return true;
					case 3 :	src << "rgb8snorm<half3>";		return true;
					case 4 :	src << "rgba8snorm<half4>";		return true;
				}
				break;
			case EValueType::UInt8_Norm :
				switch ( rows ) {
					case 1 :	src << "r8unorm<half>";			return true;
					case 2 :	src << "rg8unorm<half2>";		return true;
					case 3 :	src << "rgb8unorm<half3>";		return true;
					case 4 :	src << "rgba8unorm<half4>";		return true;
				}
				break;
			case EValueType::Int16_Norm :
				switch ( rows ) {
					case 1 :	src << "r16snorm<float>";		return true;
					case 2 :	src << "rg16snorm<float2>";		return true;
					case 3 :	src << "rgb16snorm<float3>";	return true;
					case 4 :	src << "rgba16snorm<float4>";	return true;
				}
				break;
			case EValueType::UInt16_Norm :
				switch ( rows ) {
					case 1 :	src << "r16unorm<float>";		return true;
					case 2 :	src << "rg16unorm<float2>";		return true;
					case 3 :	src << "rgb16unorm<float3>";	return true;
					case 4 :	src << "rgba16unorm<float4>";	return true;
				}
				break;
		}

		switch_enum( type )
		{
			case EValueType::Bool8 :		src << "bool";		break;
			case EValueType::Int8 :			src << "char";		break;
			case EValueType::UInt8 :		src << "uchar";		break;
			case EValueType::Int16 :		src << "short";		break;
			case EValueType::UInt16 :		src << "ushort";	break;
			case EValueType::Int32 :		src << "int";		break;
			case EValueType::UInt32 :		src << "uint";		break;
			case EValueType::Int64 :		src << "long";		break;
			case EValueType::UInt64 :		src << "ulong";		break;
			case EValueType::Float16 :		src << "half";		break;
			case EValueType::Float32 :		src << "float";		break;

			case EValueType::Int8_Norm :
			case EValueType::UInt8_Norm :
			case EValueType::Int16_Norm :
			case EValueType::UInt16_Norm :
			case EValueType::Bool32 :		// TODO
			case EValueType::Float64 :
			case EValueType::DeviceAddress:
			case EValueType::Unknown :
			case EValueType::_Count :
			default :						RETURN_ERR( "unknown value type" );
		}
		switch_end

		if ( rows > 1 )
			src << ToString( rows );

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
		const auto	TypeToStr = [&uniqueTypes, &outTypes] (const Field &field, INOUT String &src) -> bool
		{{
			if ( field.stType )
			{
				if ( uniqueTypes.insert( String{field.stType->Typename()} ).second )
					CHECK_ERR( field.stType->ToMSL( INOUT outTypes, INOUT uniqueTypes ));

				src << field.stType->Typename();
				return true;
			}
			else
			if ( field.IsScalar() and field.IsAtomic() )
			{
				//src << "atomic< ";	// TODO
				CHECK_ERR( ValueTypeToStrMSL( field.type, INOUT src ));
				//src << " >";
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
				if ( field.IsPacked() )
					src << "packed_";

				CHECK_ERR( ValueTypeToStrMSL( field.type, INOUT src ));

				src << ToString( field.rows );
				return true;
			}
			else
			if ( field.IsMat() )
			{
				StringView	s_name;
				switch ( field.type )
				{
					case EValueType::Float16 :	s_name = "half";	break;
					case EValueType::Float32 :	s_name = "float";	break;
					default :					RETURN_ERR( "unsupported value type for matrix" );
				}

				String	tname {s_name};
				tname << ToString( field.cols ) << 'x' << ToString( field.rows );

				if ( field.IsPacked() )
				{
					const String	memt	= String{s_name} << ToString( field.rows );
					const String	packed	= "packed_"s << tname;

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

		CHECK_ERR( Layout() == EStructLayout::InternalIO or IsMSLCompatible( Layout() ));

		Array<Tuple< String, String, String >>		field_parts;	// typename, name, comment

		for (auto& field : _fields)
		{
			if ( AnyBits( field.flags, EFlags::Padding_GLSL | EFlags::Padding_HLSL ) and
				 NoBits( field.flags, EFlags::Padding_MSL ))
				continue;

			auto& [part0, part1, part2] = field_parts.emplace_back();

			part0 << "\t";

			if ( field.IsDynamicArray() )
				part0 << "device ";

			if ( field.IsDeviceAddress() )
			{
				Field	f2 = field;
				f2.flags &= ~EFlags::Pointer;

				String	type_name;
				CHECK_ERR( TypeToStr( f2, INOUT type_name ));

				part0 << "device " << type_name << "*";
			}
			else
			if ( field.IsStruct() or field.IsScalar() or field.IsVec() or field.IsMat() )
			{
				CHECK_ERR( TypeToStr( field, INOUT part0 ));
			}
			else
			{
				RETURN_ERR( "unknown field type" );
			}

			if ( field.IsDynamicArray() )
				part0 << "*";

			part1 << field.name;

			if ( field.IsStaticArray() )
			{
				part1 << ' ';
				for (auto size : field.arraySizeChain)
					part1 << '[' << ToString(size) << ']';
			}
			part1 << ';';

			part2 << "// offset: " << ToString(usize( field.offset ))
				<< ", align: " << ToString(usize( field.align ))
				<< ", size: " << ToString(usize( field.size ))
				<< "\n";
		}

		String	src;
		src << "struct " << Typename() << "\n{\n";

		// format fields
		{
			usize	part0_len	= 0;
			usize	part1_len	= 0;
			usize	total_len	= 0;

			for (auto& [part0, part1, part2] : field_parts)
			{
				part0_len  = Max( part0_len, part0.length() );
				part1_len  = Max( part1_len, part1.length() );
				total_len += part0.length() + part1.length() + part2.length();
			}
			part0_len = (part0_len == 0 ? 0 : part0_len + 2);
			part1_len = (part1_len == 0 ? 0 : part1_len + 2);

			src.reserve( src.size() + total_len );

			for (auto& [part0, part1, part2] : field_parts)
			{
				src << part0;
				AppendToString( INOUT src, part0_len - part0.length(), ' ' );

				src << part1;
				AppendToString( INOUT src, part1_len - part1.length(), ' ' );

				src << part2;
			}
		}
		src << "};\n";

		if ( not HasDynamicArray() )
		{
			src	<< "static_assert( sizeof(" << Typename() << ") == " << ToString(usize( AlignUp( _size, _maxAlign )))
				<< ", \"size mismatch\" );\n";
		}
		src << "\n";

		outTypes << src;
		return true;
	}

	bool  ShaderStructType::ToMSL (INOUT String &types) const
	{
		UniqueTypes_t	uniqueTypes;
		return ToMSL( INOUT types, uniqueTypes );
	}

/*
=================================================
	ToCPP
=================================================
*/
	bool  ShaderStructType::ToCPP (INOUT String &outTypes, INOUT UniqueTypes_t &uniqueTypes) const
	{
		const auto	TypeToStr = [&] (const Field &field, INOUT String &str) -> bool
		{{
			if ( field.stType )
			{
				if ( uniqueTypes.insert( String{field.stType->Typename()} ).second )
					CHECK_ERR( field.stType->ToCPP( INOUT outTypes, INOUT uniqueTypes ));

				str << field.stType->Typename();
				return true;
			}
			else
			if ( field.IsVec() or field.IsScalar() )
			{
				// include Atomic type

				if ( field.IsVec() and AnyBits( field.flags, EFlags::Packed | EFlags::PackedAlias ) )
					str << "packed_";

				CHECK_ERR( ValueTypeToStrCPP( field.type, INOUT str ));

				if ( field.IsVec() )
					str << ToString( field.rows );
				return true;
			}
			else
			if ( field.IsMat() )
			{
				if ( field.IsPacked() )
					str << "packed_";

				switch ( field.type )
				{
					case EValueType::Float16 :	str << "half";		break;
					case EValueType::Float32 :	str << "float";		break;
					case EValueType::Float64 :	str << "double";	break;
					default :					RETURN_ERR( "unsupported value type for matrix" );
				}
				str << ToString( field.cols ) << 'x' << ToString( field.rows ) << "_storage";

				if ( not field.IsPacked() and IsStd140( _layout ))
					str << "_std140";

				return true;
			}
			else
				return false;
		}};

		String	src;
		String	test;

		src << "#ifndef " << Typename() << "_DEFINED\n"
			<< "#\tdefine " << Typename() << "_DEFINED\n"
			<< "\t// size: " << ToString(usize( _size ));

		if ( _size != AlignUp( _size, _maxAlign ))
			src << " (" << ToString(usize( AlignUp( _size, _maxAlign ))) << ")";

		src << ", align: " << ToString(usize( _maxAlign ));

		if ( _maxAlign != _structAlign )
			src << " (" << ToString(usize( _structAlign )) << ")";

		src << "\n\tstruct ";

		if ( _size != AlignUp( _size, _maxAlign ))
			src << "alignas(" << ToString(usize( _maxAlign )) << ") ";

		src << Typename() << "\n"
			<< "\t{\n"
			<< "\t\tstatic constexpr auto   TypeName = ShaderStructName{HashVal32{0x"
			<< ToString<16>( uint{ShaderStructName{Typename()}} ) << "u}};\n";

		if ( HasDynamicArray() )
		{
			auto&	field = _fields.back();
			src << "\t\tstatic constexpr size_t SizeOf (size_t count)  { return ";

			if ( _size > 0 )
				src << ToString(usize( AlignUp( _size, field.align ))) << " + ";

			src	<< "(" << ToString(usize( field.size ))
				<< " * count); }\n";
		}
		src << "\n";

		Array<Tuple< String, String, String, bool >>	field_parts;	// typename, name, comment, is_dynamic_array

		for (auto& field : _fields)
		{
			if ( field.IsAnyPadding() )
				continue;

			auto& [part0, part1, part2, is_dyn_arr] = field_parts.emplace_back();

			is_dyn_arr = field.IsDynamicArray();
			part0 << (is_dyn_arr ? "\t//\t" : "\t\t");

			if ( field.IsStaticArray() and field.arraySizeChain.size() == 1 )
				part0 << "StaticArray< ";

			if ( field.IsDeviceAddress() )
			{
				part0 << "TDeviceAddress< ";
				CHECK_ERR( TypeToStr( field, INOUT part0 ));
				part0 << (field.IsPointer() ? " *" : " ")
					<< ">";
			}
			else
			if ( field.IsStruct() or field.IsVec() or field.IsScalar() or field.IsMat() )
			{
				CHECK_ERR( TypeToStr( field, INOUT part0 ));
			}
			else
			{
				RETURN_ERR( "unknown field type" );
			}

			if ( field.IsStaticArray() and field.arraySizeChain.size() == 1 )
				part0 << ", " << ToString( field.arraySize ) << " >  ";

			part1 << field.name;

			if ( is_dyn_arr )
				part1 << " []";
			else
			if ( field.IsStaticArray() and field.arraySizeChain.size() > 1 )
			{
				part1 << ' ';
				for (auto size : field.arraySizeChain)
					part1 << '[' << ToString(size) << ']';
			}
			part1 << ';';

			if ( (field.IsScalar() or field.IsVec()) and (field.type >= EValueType::Int8_Norm and field.type <= EValueType::UInt16_Norm) )
				part2 << "// normalized";

			if ( not is_dyn_arr )
				test << "\tStaticAssert( offsetof(" << Typename() << ", " << field.name << ") == " << ToString(usize( field.offset )) << " );\n";
		}

		// format fields
		{
			usize	part0_len	= 0;
			usize	part1_len	= 0;
			usize	total_len	= 0;

			for (auto& [part0, part1, part2, is_dyn_arr] : field_parts)
			{
				part0_len  = Max( part0_len, part0.length() - (is_dyn_arr ? 2 : 0) );
				part1_len  = Max( part1_len, part1.length() );
				total_len += part0.length() + part1.length() + part2.length();
			}
			part0_len = (part0_len == 0 ? 0 : part0_len + 2);
			part1_len = (part1_len == 0 ? 0 : part1_len + 2);

			src.reserve( src.size() + total_len );

			for (auto& [part0, part1, part2, is_dyn_arr] : field_parts)
			{
				src << part0;
				AppendToString( INOUT src, part0_len - part0.length() + (is_dyn_arr ? 2 : 0), ' ' );

				src << part1;

				if ( not part2.empty() )
				{
					AppendToString( INOUT src, part1_len - part1.length(), ' ' );
					src << part2;
				}
				src << '\n';
			}
		}
		src << "\t};\n"
			<< "#endif\n"
			<< test;

		if ( _size > 0 ) {
			src << "\tStaticAssert( sizeof(" << Typename() << ") == " << ToString(usize( AlignUp( _size, _maxAlign ))) << " );\n";
		}
		src << "\n";

		outTypes << src;
		return true;
	}

	bool  ShaderStructType::ToCPP (INOUT String &types) const
	{
		UniqueTypes_t	uniqueTypes;
		return ToCPP( INOUT types, uniqueTypes );
	}

/*
=================================================
	ValueTypeToStrHLSL
=================================================
*/
namespace {
	ND_ static bool  ValueTypeToStrHLSL (EValueType type, INOUT String &src)
	{
		switch_enum( type )
		{
		//	case EValueType::Bool8 :		src << "bool";			break;
		//	case EValueType::Int8 :
		//	case EValueType::Int8_Norm :	src << "int8_t";		break;
		//	case EValueType::UInt8 :
		//	case EValueType::UInt8_Norm :	src << "uint8_t";		break;

			case EValueType::Bool32 :		src << "bool";			break;
			case EValueType::Int16 :
			case EValueType::Int16_Norm :	src << "int16_t";		break;
			case EValueType::UInt16 :
			case EValueType::UInt16_Norm :	src << "uint16_t";		break;	// -enable-16bit-types, SM 6.2+
			case EValueType::Int32 :		src << "int32_t";		break;
			case EValueType::UInt32 :		src << "uint32_t";		break;
			case EValueType::Int64 :		src << "int64_t";		break;
			case EValueType::UInt64 :		src << "uint64_t";		break;
			case EValueType::Float16 :		src << "half";			break;
			case EValueType::Float32 :		src << "float";			break;
			case EValueType::Float64 :		src << "double";		break;
			case EValueType::DeviceAddress:	src << "DeviceAddress";	break;

			case EValueType::Bool8 :
			case EValueType::Int8 :
			case EValueType::Int8_Norm :
			case EValueType::UInt8 :
			case EValueType::UInt8_Norm :

			case EValueType::Unknown :
			case EValueType::_Count :
			default :						RETURN_ERR( "unknown value type" );
		}
		switch_end
		return true;
	}
}
/*
=================================================
	ToHLSL
----
	https://microsoft.github.io/hlsl-specs/specs/hlsl.pdf
	https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst#rawbufferload-and-rawbufferstore
	https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/DXIL.rst#loadstoreatomics-via-pointer-in-future-sm
	https://web.archive.org/web/20241207194627/https://shader-slang.com/docs/understanding-generics/
=================================================
*/
	bool  ShaderStructType::ToHLSL (INOUT String &outTypes, INOUT UniqueTypes_t &uniqueTypes, INOUT String* sizeCheck) const
	{
		Unused( sizeCheck );
		const bool	is_std140 = IsStd140( _layout );

		const auto	TypeToStr = [&] (const Field &field, INOUT String &str) -> bool
		{{
			if ( field.IsPacked() and (field.IsVec() or field.IsMat()) )
			{
				String	scalar;
				CHECK_ERR( ValueTypeToStrHLSL( field.type, INOUT scalar ));

				String	packed	= (is_std140 ? "inplace_"s : "packed_"s);
				String	memt;
				String	dst_type;

				packed << scalar;
				if ( field.IsVec() )
				{
					packed	 << ToString( field.rows );
					memt	 << scalar;
					dst_type << "vector<"s << scalar << ',' << ToString( field.rows ) << '>';
				}else
				if ( field.IsMat() )
				{
					String	dst_vtype	= "vector<"s << scalar << ',' << ToString( field.rows ) << '>';
					String	vec_type	= packed + ToString( field.rows );

					packed	 << ToString( field.cols ) << 'x' << ToString( field.rows );
					memt	 << (is_std140 ? "inplace_"s : "packed_"s) << scalar << ToString( field.rows );
					dst_type << "matrix<"s << scalar << ',' << ToString( field.cols ) << ',' << ToString( field.rows ) << '>';

					if ( uniqueTypes.insert( vec_type ).second )
					{
						Field	f;
						f.type		= field.type;
						f.rows		= field.rows;
						f.cols		= 1;
						f.arraySize	= 0;	// non-array
						f.align		= ValueTypeSizeOf( field.type );
						f.size		= f.align * field.rows;

						if ( not field.IsPointer() )
							ASSERT( field.align == f.align );

						if ( is_std140 ) {
							CHECK_ERR( _CreatePackedTypeGLSL1( INOUT outTypes, vec_type, scalar, dst_vtype, f ));
						}else{
							CHECK_ERR( _CreatePackedTypeGLSL2( INOUT outTypes, vec_type, scalar, dst_vtype, f ));
						}
					}
				}

				if ( uniqueTypes.insert( packed ).second )
				{
					Field	f;
					f.type		= field.type;
					f.rows		= field.rows;
					f.cols		= field.cols;
					f.arraySize	= 0;	// non-array
					f.align		= ValueTypeSizeOf( field.type );
					f.size		= f.align * f.rows * f.cols;

					if ( not field.IsPointer() )
						ASSERT( field.align == f.align );

					ASSERT( field.IsMat() == f.IsMat() );
					ASSERT( field.IsVec() == f.IsVec() );

					if ( is_std140 ) {
						CHECK_ERR( _CreatePackedTypeGLSL1( INOUT outTypes, packed, memt, dst_type, f ));
					}else{
						CHECK_ERR( _CreatePackedTypeGLSL2( INOUT outTypes, packed, memt, dst_type, f ));
					}
				}
				str << packed;
				return true;
			}
			else
			if ( field.IsAtomic() and field.IsScalar() )
			{
				str << "Atomic< ";
				CHECK_ERR( ValueTypeToStrHLSL( field.type, INOUT str ));
				str << " >";
				return true;
			}
			else
			if ( field.IsScalar() )
			{
				CHECK_ERR( ValueTypeToStrHLSL( field.type, INOUT str ));
				return true;
			}
			else
			if ( field.IsVec() )
			{
				str << "vector<";
				CHECK_ERR( ValueTypeToStrHLSL( field.type, INOUT str ));
				str << ',' << ToString( field.rows ) << '>';
				return true;
			}
			else
			if ( field.IsMat() )
			{
				str << "matrix<";
				CHECK_ERR( ValueTypeToStrHLSL( field.type, INOUT str ));
				str << ',' << ToString( field.rows ) << ',' << ToString( field.cols ) << '>';
				return true;
			}
			else
			if ( field.stType )
			{
				if ( uniqueTypes.insert( String{field.stType->Typename()} ).second )
					CHECK_ERR( field.stType->ToHLSL( INOUT outTypes, INOUT uniqueTypes ));

				str << field.stType->Typename();
				return true;
			}
			else
				return false;
		}};

		if ( HasDynamicArray() )
		{
			AE_LOGW( "In HLSL dynamic array must be defined as StructuredBuffer with element type." );
			return true;
		}

		Array<Tuple< String, String, String >>	field_parts;	// typename, name, comment

		for (auto& field : _fields)
		{
			if ( AnyBits( field.flags, EFlags::Padding_MSL | EFlags::Padding_GLSL ) and
				 NoBits( field.flags, EFlags::Padding_HLSL ))
				continue;

			auto& [part0, part1, part2] = field_parts.emplace_back();

			part0 << "\t";

			if ( field.IsDeviceAddress() )
			{
				CHECK_ERR( TypeToStr( field, INOUT part0 ));
				part0 << " *";
				part1 << field.name;
			}
			else
			if ( field.IsStruct() or field.IsVec() or field.IsScalar() or field.IsMat() )
			{
				CHECK_ERR( TypeToStr( field, INOUT part0 ));

				if ( field.IsPacked() and is_std140 )
					part1 << "( " << field.name << " )";
				else
					part1 << field.name;
			}
			else
			{
				RETURN_ERR( "unknown field type" );
			}

			if ( field.IsStaticArray() )
			{
				part1 << ' ';
				for (auto size : field.arraySizeChain)
					part1 << '[' << ToString(size) << ']';
			}
			part1 << ';';

			part2 << "// offset: " << ToString(usize( field.offset ))
				<< ", align: " << ToString(usize( field.align ))
				<< ", size: " << ToString(usize( field.size ));
		}

		String	src;
		src << "// size: " << ToString(usize( _size )) << ", align: " << ToString(usize( _maxAlign )) << '\n';
		src << "struct " << Typename() << "\n{\n";

		// format fields
		{
			usize	part0_len	= 0;
			usize	part1_len	= 0;
			usize	total_len	= 0;

			for (auto& [part0, part1, part2] : field_parts)
			{
				part0_len  = Max( part0_len, part0.length() );
				part1_len  = Max( part1_len, part1.length() );
				total_len += part0.length() + part1.length() + part2.length();
			}
			part0_len = (part0_len == 0 ? 0 : part0_len + 2);
			part1_len = (part1_len == 0 ? 0 : part1_len + 2);

			src.reserve( src.size() + total_len );

			for (auto& [part0, part1, part2] : field_parts)
			{
				src << part0;
				AppendToString( INOUT src, part0_len - part0.length(), ' ' );

				src << part1;
				AppendToString( INOUT src, part1_len - part1.length(), ' ' );

				src << part2 << '\n';
			}
		}
		src << "};\n";

		// TODO: https://github.com/shader-slang/slang/pull/7945
		/*if ( sizeCheck != null and
			 not is_std140 )  // doesn't apply const buffer alignment
		{
			// last field is not aligned
			Bytes	st_size;
			if ( not _fields.empty() )
			{
				auto&	last = _fields.back();

				st_size = last.offset;
				if ( last.IsVec() and not last.IsPacked() and last.rows == 3 )
					st_size += (last.size / 4) * 3;
				else
				if ( last.IsMat() and not last.IsPacked() and last.rows == 3 )
					st_size += (last.size / (last.cols * 4)) * 3 * last.cols;
				else
					st_size += last.size;
			}

			*sizeCheck << "\t\tstatic_assert( sizeof(" << Typename() << ") == " << ToString(usize( st_size ))
			 << ", \"size mismatch\" );\n";
		}*/
		src << "\n";

		outTypes << src;
		return true;
	}

	bool  ShaderStructType::ToHLSL (INOUT String &outTypes, INOUT String* sizeCheck) const
	{
		UniqueTypes_t	uniqueTypes;
		return ToHLSL( INOUT outTypes, INOUT uniqueTypes, INOUT sizeCheck );
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
				usize	cnt = 0;
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

		String	str;
		for (auto& field : _fields)
		{
			CHECK_THROW_MSG( not field.IsArray() );
			CHECK_THROW_MSG( not field.IsDeviceAddress() );

			if ( field.IsStruct() )
			{
				String	pref = prefix;
				if ( not pref.empty() ) pref += '_';
				pref += field.name;

				str << field.stType->VertexInputToGLSL( pref, INOUT loc );
			}
			else
			{
				CHECK_THROW_MSG( not field.IsMat(),
					"Matrix is not supported for VertexInput" );

				String		tname;
				StringView	s_name, v_name, m_name;
				CHECK_ERR( ValueTypeToStrGLSL_WithPrecision( field.type, field.flags, EFlags_VSCompatible,
															 OUT tname, OUT s_name, OUT v_name, OUT m_name ));

				if ( field.IsScalar() )	tname << s_name;
				if ( field.IsVec() )	tname << v_name << ToString( field.rows );

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

		String	str;
		for (auto& field : _fields)
		{
			CHECK_THROW_MSG( not field.IsArray() );
			CHECK_THROW_MSG( not field.IsDeviceAddress() );

			if ( field.IsStruct() )
			{
				String	pref = prefix;
				if ( not pref.empty() ) pref += '_';
				pref += field.name;

				str << field.stType->VertexInputToMSL( pref, INOUT index );
			}
			else
			{
				CHECK_THROW_MSG( not field.IsMat(),
					"Matrix is not supported for VertexInput" );

				str << "  ";

				String	qual;
				CHECK_ERR( ValueTypeToStrMSL_WithNorm( field.type, field.flags, EFlags_VSCompatible, field.rows, INOUT str, OUT qual ));
				ASSERT( qual.empty() );

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
			if ( field.IsAnyPadding() )
				continue;

			CHECK_THROW_MSG( not field.IsDeviceAddress() );

			if ( field.IsStruct() )
			{
				field.stType->GetVertexInput( INOUT loc, INOUT arr );
			}
			else
			{
				CHECK_THROW_MSG( not field.IsMat(),
					"Matrix is not supported for VertexInput" );

				EVertexType	type = Default;
				switch_enum( field.type )
				{
					case EValueType::Int8 :			type = EVertexType::Byte;			break;
					case EValueType::UInt8 :		type = EVertexType::UByte;			break;
					case EValueType::Int16 :		type = EVertexType::Short;			break;
					case EValueType::UInt16 :		type = EVertexType::UShort;			break;
					case EValueType::Int32 :		type = EVertexType::Int;			break;
					case EValueType::UInt32 :		type = EVertexType::UInt;			break;
					case EValueType::Int64 :		type = EVertexType::Long;			break;
					case EValueType::UInt64 :		type = EVertexType::ULong;			break;
					case EValueType::Float16 :		type = EVertexType::Half;			break;
					case EValueType::Float32 :		type = EVertexType::Float;			break;
					case EValueType::Float64 :		type = EVertexType::Double;			break;
					case EValueType::Int8_Norm :	type = EVertexType::Byte_Norm;		break;
					case EValueType::UInt8_Norm :	type = EVertexType::UByte_Norm;		break;
					case EValueType::Int16_Norm :	type = EVertexType::Short_Norm;		break;
					case EValueType::UInt16_Norm :	type = EVertexType::UShort_Norm;	break;

					case EValueType::Bool8 :		// TODO
					case EValueType::Bool32 :
					case EValueType::DeviceAddress:
					case EValueType::Unknown :
					case EValueType::_Count :
					default :						CHECK_THROW_MSG( false, "unknown value type" );
				}
				switch_end

				if ( field.IsVec() )
					type |= EVertexType((field.rows - 1) << uint(EVertexType::_VecOffset));

				auto&	dst = arr.emplace_back();
				dst.type	= type;
				dst.index	= CheckCast{ loc++ };
				dst.offset	= Bytes16u{ field.offset };

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
		if ( (shaderType == EShader::MeshTask)		or
			 (input and shaderType == EShader::Mesh) )
		{
			String	str;
			CHECK_THROW( StructToGLSL( OUT str, INOUT uniqueTypes ));

			str << "taskPayloadSharedEXT " << Typename() << " ";
			str << (input ? "In" : "Out");
			str << ";\n\n";
			return str;
		}

		_usage |= EUsage::ShaderIO;

		uint	loc			= 0;
		bool	is_array	= false;
		String	str;

		switch_enum( shaderType )
		{
			case EShader::Vertex :			str << "Vertex";		 break;
			case EShader::TessControl :		str << "TessControl";	 is_array = true;		break;
			case EShader::TessEvaluation :	str << "TessEvaluation"; is_array = input;		break;
			case EShader::Geometry :		str << "Geometry";		 is_array = input;		break;
			case EShader::Fragment :		str << "Fragment";		 break;
			case EShader::MeshTask :		str << "MeshTask";		 is_array = false;		break;
			case EShader::Mesh :			str << "Mesh";			 is_array = not input;	break;

			case EShader::Compute :
			case EShader::Tile :
			case EShader::RayGen :
			case EShader::RayAnyHit :
			case EShader::RayClosestHit :
			case EShader::RayMiss :
			case EShader::RayIntersection :
			case EShader::RayCallable :
			case EShader::Unknown :
			default :
				CHECK_THROW_MSG( false, "unsupported shader type for shader IO" );
		}
		switch_end

		(input ?
			"// stage input\n"s  << (is_array ? "layout(location="s << ToString(loc) << ") " : "") << "in " :
			"// stage output\n"s << (is_array ? "layout(location="s << ToString(loc) << ") " : "") << "out ")
			>> str;

		str << (input ? "Input" : "Output");
		str << " {\n";

		Array<Tuple< String, String, String >>	field_parts;
		_ToShaderIO_GLSL( shaderType, "", not is_array, INOUT loc, INOUT field_parts );

		// format fields
		{
			usize	part0_len	= 0;
			usize	part1_len	= 0;
			usize	total_len	= 0;

			for (auto& [part0, part1, part2] : field_parts)
			{
				part0_len  = Max( part0_len, part0.length() );
				part1_len  = Max( part1_len, part1.length() );
				total_len += part0.length() + part1.length() + part2.length();
			}
			part1_len = (part1_len == 0 ? 0 : part1_len + 2);

			str.reserve( str.size() + total_len );

			for (auto& [part0, part1, part2] : field_parts)
			{
				str << part0;
				AppendToString( INOUT str, part0_len - part0.length(), ' ' );

				str << part1;
				AppendToString( INOUT str, part1_len - part1.length(), ' ' );

				str << part2;
			}
		}

		str << "} ";
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
	void  ShaderStructType::_ToShaderIO_GLSL (EShader shaderType, const String &prefix, bool useLocations, INOUT uint &loc,
											  INOUT Array<Tuple< String, String, String >> &fieldParts) C_Th___
	{
		for (auto& field : _fields)
		{
			if ( field.IsAnyPadding() )
				continue;

			CHECK_THROW_MSG( not field.IsArray() );
			CHECK_THROW_MSG( not field.IsDeviceAddress() );

			if ( field.IsStruct() )
			{
				String	pref = prefix;
				if ( not pref.empty() ) pref += '_';
				pref += field.name;

				field.stType->_ToShaderIO_GLSL( shaderType, pref, useLocations, INOUT loc, INOUT fieldParts );
			}
			else
			{
				const auto	mask = (shaderType == EShader::Fragment ? EFlags_ShaderIO : EFlags_VSCompatible);

				auto& [part0, part1, part2] = fieldParts.emplace_back();

				part0 << "  ";
				if ( useLocations ) part0 << "layout(location=" << ToString(loc++) << ") ";

				StringView	s_name, v_name, m_name;
				CHECK_THROW( ValueTypeToStrGLSL_WithPrecision( field.type, field.flags, mask,
																OUT part0, OUT s_name, OUT v_name, OUT m_name ));

				if ( field.IsScalar() )	part1 << s_name;
				if ( field.IsVec() )	part1 << v_name << ToString( field.rows );
				if ( field.IsMat() )	part1 << m_name << ToString( field.cols ) << 'x' << ToString( field.rows );
				CHECK_THROW( not part1.empty() );

				part2 << prefix << (prefix.empty() ? ""s : "_"s) << field.name << ";\n";
			}
		}
	}

/*
=================================================
	ToShaderIO_MSL
=================================================
*/
	String  ShaderStructType::ToShaderIO_MSL (EShader shaderType, bool, INOUT UniqueTypes_t &) C_Th___
	{
		_usage |= EUsage::ShaderIO;

		String	str;
		str << "struct "s << Typename() << "\n{\n";

		Array<Tuple< String, String, String >>	field_parts;
		field_parts.emplace_back( "  float4", "position", "[[position]];\n" );

		_ToShaderIO_MSL( shaderType, "", INOUT field_parts );

		// format fields
		{
			usize	part0_len	= 0;
			usize	part1_len	= 0;
			usize	total_len	= 0;

			for (auto& [part0, part1, part2] : field_parts)
			{
				part0_len  = Max( part0_len, part0.length() );
				part1_len  = Max( part1_len, part1.length() );
				total_len += part0.length() + part1.length() + part2.length();
			}
			part0_len = (part0_len == 0 ? 0 : part0_len + 2);
			part1_len = (part1_len == 0 ? 0 : part1_len + 2);

			str.reserve( str.size() + total_len );

			for (auto& [part0, part1, part2] : field_parts)
			{
				str << part0;
				AppendToString( INOUT str, part0_len - part0.length(), ' ' );

				str << part1;

				if ( not part2.empty() )
				{
					AppendToString( INOUT str, part1_len - part1.length(), ' ' );
					str << part2;
				}
			}
		}

		str << "};\n\n";
		return str;
	}

/*
=================================================
	_ToShaderIO_MSL
=================================================
*/
	void  ShaderStructType::_ToShaderIO_MSL (EShader shaderType, const String &prefix, INOUT Array<Tuple< String, String, String >> &fieldParts) C_Th___
	{
		for (auto& field : _fields)
		{
			if ( field.IsAnyPadding() )
				continue;

			CHECK_THROW_MSG( not field.IsArray() );
			CHECK_THROW_MSG( not field.IsDeviceAddress() );

			if ( field.IsStruct() )
			{
				String	pref = prefix;
				if ( not pref.empty() ) pref += '_';
				pref += field.name;

				field.stType->_ToShaderIO_MSL( shaderType, pref, INOUT fieldParts );
			}
			else
			{
				CHECK_THROW_MSG( not field.IsMat(),
					"Matrix is not supported for MSL ShaderIO" );

				auto& [part0, part1, part2] = fieldParts.emplace_back();

				const auto	mask = (shaderType == EShader::Fragment ? EFlags_ShaderIO : EFlags_VSCompatible);
				String		qual;

				part0 << "  ";
				part1 << field.name;

				CHECK_THROW( ValueTypeToStrMSL_WithNorm( field.type, field.flags, mask, field.rows, INOUT part0, OUT part2 ));

				if ( part2.empty() )
					part1 << ";\n";
				else
					part2 << ";\n";
			}
		}
	}

/*
=================================================
	ToShaderIO_HLSL
=================================================
*/
	String  ShaderStructType::ToShaderIO_HLSL (EShader shaderType, bool input, INOUT UniqueTypes_t &uniqueTypes) C_Th___
	{
		// TODO
		Unused( shaderType, input, uniqueTypes );
		return "";
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
		return	_fields		== rhs._fields		and
				_layout		== rhs._layout		and
				_features	== rhs._features;
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
			EnumBinder<EUsage>	binder{ se };
			binder.Create();

			binder.Comment( "Used as interface between graphics pipeline stages. Reflection to C++ is not supported." );
			binder.AddValue( "ShaderIO",		EUsage::ShaderIO );

			binder.Comment( "Used as vertex buffer layout. Enables reflection to C++." );
			binder.AddValue( "VertexLayout",	EUsage::VertexLayout );

			binder.Comment( "Used as vertex attributes in shader.\n"
							"Reflection to C++ is not enabled, use 'VertexLayout' to enable it." );
			binder.AddValue( "VertexAttribs",	EUsage::VertexAttribs );

			binder.Comment( "Used as uniform/storage buffer. Layout must be same in GLSL/MSL and C++. Enables reflection to C++." );
			binder.AddValue( "BufferLayout",	EUsage::BufferLayout );

			binder.Comment( "Used as buffer reference in shader. Layout must be same between shaders in single platform.\n"
							"Reflection to C++ is not enabled, use 'BufferLayout' to enable it." );
			binder.AddValue( "BufferReference",	EUsage::BufferReference );
			StaticAssert( uint(EUsage::All) == 31 );
		}
		{
			ClassBinder<ShaderStructType>	binder{ se };
			binder.CreateRef( 0, False{} );

			binder.Comment( "Create structure type.\n"
							"Name is used as typename for uniform/storage/vertex buffer or as shader in/out block." );
			binder.AddFactoryCtor( &ShaderStructType_Ctor, {"name"} );

			binder.Comment( "Add FeatureSet to the structure.\n"
							"If used float64/int64 types FeatureSet must support this features." );
			AS_METHOD( binder, ShaderStructType::AddFeatureSet,		"AddFeatureSet",	{"fsName"} );

			binder.Comment( "Set source with structure fields.\n"
							"Layout - offset and align rules." );
			AS_METHOD( binder, ShaderStructType::Set,				"Set",				{"layout", "fields"} );
			AS_METHOD( binder, ShaderStructType::Set2,				"Set",				{"fields"} );

		//	AS_METHOD( binder, ShaderStructType::FieldsToString,	"FieldsToString"	);

			binder.Comment( "Manually specify how structure will be used." );
			AS_METHOD( binder, ShaderStructType::AddUsage,			"AddUsage",			{} );

			binder.Comment( "Returns size of the static data." );
			AS_METHOD( binder, ShaderStructType::_StaticSize,		"StaticSize",		{} );

			binder.Comment( "Returns array element size for dynamic arrays." );
			AS_METHOD( binder, ShaderStructType::_ArrayStride,		"ArrayStride",		{} );
		}
	}


} // AE::PipelineCompiler
