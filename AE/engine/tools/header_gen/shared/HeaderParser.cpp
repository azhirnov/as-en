// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "HeaderParser.h"

namespace AE::Parsers
{
	using namespace std::string_view_literals;

/*
=================================================
	_IsWord
=================================================
*/
	bool HeaderParser::_IsWord (StringView value)
	{
		for (usize i = 0; i < value.length(); ++i)
		{
			const char	c = value[i];
			bool		valid = false;

			valid |= (c >= 'a' and c <= 'z');
			valid |= (c >= 'A' and c <= 'Z');
			valid |= (c == '_');

			if ( i > 0 )
				valid |= (c >= '0' and c <= '9');

			if ( not valid )
				return false;
		}
		return true;
	}

/*
=================================================
	_IsNumber
=================================================
*/
	bool HeaderParser::_IsNumber (StringView value)
	{
		if ( EndsWithIC( value, "ull" ))
			value = value.substr( 0, value.length()-3 );
		else
		if ( EndsWithIC( value, "ul" ))
			value = value.substr( 0, value.length()-2 );
		else
		if ( EndsWithIC( value, "u" ))
			value = value.substr( 0, value.length()-1 );


		if ( StartsWithIC( value, "0x" ))
		{
			value = value.substr( 2 );
			for (auto& c : value)
			{
				if ( not (c >= '0' and c <= '9') and
					 not (c >= 'A' and c <= 'F') and
					 not (c >= 'a' and c <= 'f') )
					return false;
			}
		}
		else
		{
			if ( value[0] == '-' or value[0] == '+' )
				value = value.substr( 1 );

			for (auto& c : value)
			{
				if ( not (c >= '0' and c <= '9') )
					return false;
			}
		}
		return true;
	}

/*
=================================================
	_IsTypeOrQual
=================================================
*/
	bool HeaderParser::_IsTypeOrQual (StringView value)
	{
		return	value == "*"		or
				value == "["		or
				value == "]"		or
				_IsWord( value )	or
				_IsNumber( value );
	}

/*
=================================================
	_ParseHeader
=================================================
*/
	bool  HeaderParser::_ParseHeader (StringView fileData,
									  const uint fileIndex,
									  ArrayView< StringView > enableIfdef,
									  ArrayView< StringView > disableIfdef,
									  ArrayView< StringView > skiFunctions,
									  const bool defaultSkip)
	{
		enum class EMode
		{
			None,
			Struct,
			Enum,
			Func,
			Define,
		};

		Array< StringView >	lines;
		Parser::DivideLines( fileData, lines );

		EnumInfo			curr_enum;
		StructInfo			curr_struct;
		FunctionInfo		curr_func;

		Array< StringView >	tokens;
		EMode				mode			= EMode::None;
		bool				is_voidfunc		= false;

		StringView			curr_ext;
		Array<bool>			skip_stack;		skip_stack.push_back( defaultSkip );

		CHECK( not _macroPrefix.empty() );
		CHECK( not _structPrefix.empty() );

		const String	no_proto	= _macroPrefix + "NO_PROTOTYPES"s;
		const String	flags_type	= _structPrefix + "Flags";

		const auto	ParseArgs	= [&mode, &is_voidfunc, &curr_func, &curr_ext, this] (ArrayView<StringView> in_tokens, usize j)
		{{
			FuncArg		curr_arg;

			for (; j < in_tokens.size(); ++j)
			{
				if ( not is_voidfunc )
				{
					if ( in_tokens[j] == "," or in_tokens[j] == ")" )
					{
						// skip array type
						if ( curr_arg.type.back() == "]" )
						{
							CHECK( curr_arg.type.size() > 3 );
							curr_arg.name = *(curr_arg.type.end()-4);
							curr_arg.type.erase( curr_arg.type.end()-4 );
						}
						else
						{
							curr_arg.name = curr_arg.type.back();
							curr_arg.type.pop_back();
						}

						CHECK( _IsWord( curr_arg.name ) and not curr_arg.name.empty() );
						CHECK( not curr_arg.type.empty() );

						curr_func.args.push_back( RVRef(curr_arg) );
						curr_arg = Default;
					}
					else
					{
						CHECK( _IsTypeOrQual( in_tokens[j] ));
						curr_arg.type.push_back( in_tokens[j] );
					}
				}

				if ( in_tokens[j] == ")" )
				{
					mode = EMode::None;
					break;
				}
			}

			if ( mode == EMode::None and not is_voidfunc )
			{
				CHECK( _funcs.find( SearchableFunc{curr_func.name} ) == _funcs.end() );

				curr_func.extension = curr_ext;
				CHECK( _funcs.insert( RVRef(curr_func) ).second );
				curr_func = Default;
			}
		}};

		const auto	ParseEnumField	= [this] (ArrayView<StringView> in_tokens, usize i, OUT EnumField &outValue)
		{{
			// find name
			if ( i < in_tokens.size() )
			{
				CHECK( StartsWith( in_tokens[i], _macroPrefix ));
				CHECK( _IsWord( in_tokens[i] ));

				outValue.name = in_tokens[i];
				++i;
			}

			// find value
			for (; i < in_tokens.size(); ++i)
			{
				if ( in_tokens[i] == "=" )
					continue;

				if ( in_tokens[i] == "," )
					break;

				//CHECK( _IsNumber( tokens[i] ));

				outValue.value << in_tokens[i];
			}

			CHECK( 	not outValue.name.empty()  and
					not outValue.value.empty() );
		}};

		const auto	ParseStructField = [] (ArrayView<StringView> in_tokens, usize i, OUT FuncArg &outField)
		{{
			for (; i < in_tokens.size(); ++i)
			{
				if ( in_tokens[i] == "[" )
				{
					// skip array
					CHECK( in_tokens.size() >= i+2 );
					CHECK( in_tokens[i+2] == "]" );
					i += 2;
				}
				else
				if ( in_tokens[i] == ";" or in_tokens[i] == ":" )
				{
					outField.name = outField.type.back();
					outField.type.pop_back();

					CHECK( _IsWord( outField.name ) and not outField.name.empty() );
					CHECK( not outField.type.empty() );
					return;
				}
				else
				{
					CHECK( _IsTypeOrQual( in_tokens[i] ));
					outField.type.push_back( in_tokens[i] );
				}
			}
		}};


		for (auto& curr_line : lines)
		{
			Parser::DivideString_CPP( curr_line, OUT tokens );

			switch ( mode )
			{
				case EMode::Struct :
				{
					if ( tokens.size() > 1 and
						 tokens[0] == "}" )
					{
						CHECK( _structs.find( SearchableStruct{curr_struct.name} ) == _structs.end() );

						curr_struct.extension = curr_ext;
						CHECK( _structs.insert( RVRef(curr_struct) ).second );

						curr_struct	= Default;
						mode		= EMode::None;
					}
					else
					if ( tokens.size() > 1 and
						 tokens[0] == "//" )
					{
						// skip comment
					}
					else
					{
						FuncArg		val;
						ParseStructField( tokens, 0, OUT val );
						curr_struct.fields.push_back( RVRef(val) );
					}
					break;
				}

				case EMode::Enum :
				{
					if ( tokens.size() > 1 and
						 tokens[0] == "}" )
					{
						CHECK( _enums.find( SearchableEnum{curr_enum.name} ) == _enums.end() );

						curr_enum.extension = curr_ext;
						CHECK( _enums.insert( RVRef(curr_enum) ).second );

						curr_enum	= Default;
						mode		= EMode::None;
					}
					else
					if ( tokens.size() >= 2 and
						 (tokens[0] == "#" and (tokens[1] == "ifdef" or tokens[1] == "endif")) )
					{
						// skip #ifdef beta_extension #endif
					}
					else
					if ( tokens.size() > 1 and
						 tokens[0] == "//" )
					{
						// skip comment
					}
					else
					{
						ASSERT( not curr_enum.name.empty() );
						EnumField	val;
						ParseEnumField( tokens, 0, OUT val );
						ASSERT( val.value != "-" );
						curr_enum.fields.push_back( RVRef(val) );
					}
					break;
				}

				case EMode::Func :
				{
					ParseArgs( tokens, 0 );
					break;
				}

				case EMode::Define :
				{
					if ( tokens.empty() or tokens.back() != "\\" )
					{
						mode = EMode::None;
					}
					break;
				}
			}


			if ( tokens.size() > 2		and
				 tokens[0] == "#"		and
				 tokens[1] == "define" )
			{
				if ( _extensions.count( tokens[2] ))
				{
					curr_ext = tokens[2];
					AE_LOGI( "Ext: "s << curr_ext );
				}
			}

			if ( tokens.size() == 3		and
				 tokens[0] == "#"		and
				 tokens[1] == "ifndef"	and
				 tokens[2] == no_proto )
			{
				curr_ext = {};
			}

			if ( tokens.size() > 1 and
				 tokens[0] == "#" and
				 tokens[1] == "endif" )
			{
				skip_stack.pop_back();
				continue;
			}

			// is need to skip
			if ( tokens.size() > 2 and
				 tokens[0] == "#" and
				(tokens[1] == "ifdef" or tokens[1] == "ifndef") )
			{
				bool	is_ifdef	= ( tokens[1] == "ifdef" );
				bool	enabled		= false;
				bool	disabled	= false;

				for (auto& item : enableIfdef)
				{
					if ( item == tokens[2] )
					{
						enabled = true;
						break;
					}
				}

				for (auto& item : disableIfdef)
				{
					if ( item == tokens[2] )
					{
						disabled = true;
						break;
					}
				}

				// enabled == false and disable == false is OK
				CHECK( (not enabled and not disabled) or enabled != disabled );

				skip_stack.push_back( is_ifdef ? not enabled and disabled : not( not enabled and disabled ));
				continue;
			}

			if ( tokens.size() > 2		and
				 tokens[0] == "#"		and
				 tokens[1] == "if" )
			{
				auto	last = skip_stack.back();
				skip_stack.push_back( last );
				continue;
			}

			if ( tokens.size() > 2		and
				 tokens[0] == "#"		and
				 (tokens[1] == "ifdef" or tokens[1] == "ifndef" or tokens[1] == "if") )
			{
				auto	last = skip_stack.back();
				skip_stack.push_back( last );
				continue;
			}

			if ( skip_stack.back() )
				continue;

			// struct / enum / union
			if ( tokens.size() > 3				and
				 tokens[0] == "typedef"			and
				 (tokens[1] == "struct" or tokens[1] == "enum" or tokens[1] == "union") and
				 StartsWith( tokens[2], _structPrefix ))
			{
				if ( tokens[1] == "enum" )
				{
					curr_enum.name			= tokens[2];
					curr_enum.fileIndex		= fileIndex;
					mode					= EMode::Enum;
				}
				else
				{
					curr_struct.name		= tokens[2];
					curr_struct.fileIndex	= fileIndex;
					mode					= EMode::Struct;
				}
				continue;
			}


			// function or type
			if ( tokens.size() > 1		and
				 tokens[0] == "typedef" )
			{
				StringView	prefix		= "PFN_";
				usize		ret_pos		= 0;
				usize		name_pos	= 0;

				for (usize j = 1; j < tokens.size(); ++j) {
					if ( tokens[j-1] == "(" and EndsWith( tokens[j], "API_PTR" )) {
						ret_pos = j-1;
						break;
					}
				}

				for (usize j = ret_pos; j < tokens.size(); ++j) {
					if ( StartsWithIC( tokens[j], prefix )) {
						name_pos = j;
						break;
					}
				}

				// function
				if ( name_pos > 0 )
				{
					mode = EMode::Func;

					StringView	func_name = tokens[name_pos].substr( prefix.length() );

					is_voidfunc = false;

					for (auto& item : skiFunctions)
					{
						if ( func_name == item )
						{
							is_voidfunc = true;
							break;
						}
					}

					if ( not is_voidfunc )
					{
						curr_func.fileIndex	= fileIndex;
						curr_func.name		= func_name;

						CHECK( _IsWord( curr_func.name ));

						for (usize k = 1; k < ret_pos; ++k) {
							curr_func.result.type.push_back( tokens[k] );
						}
					}

					usize	j = name_pos;

					// move to args
					for (; j < tokens.size(); ++j) {
						if ( tokens[j] == "(" )
							break;
					}

					ParseArgs( tokens, ++j );
					continue;
				}

				// typedef
				if ( tokens.size() == 4 and tokens[3] == ";" )
				{
					TypedefInfo		info;
					info.dstType	= tokens[1];
					info.extension	= curr_ext;
					info.fileIndex	= fileIndex;

					CHECK( _typedefs.emplace( tokens[2], info ).second );
					continue;
				}
			}


			// bitfield
			if ( tokens.size() == 4		 and
				 tokens[0] == "typedef"  and
				 tokens[1] == flags_type and
				 tokens[3] == ";" )
			{
				String	enum_name { tokens[2] };

				if ( enum_name.back() == 's' )
				{
					enum_name.pop_back();
					enum_name << "Bits";
				}

				auto	it = _enums.find( SearchableEnum{enum_name} );

				BitfieldInfo		bitfield;
				bitfield.name		= tokens[2];
				bitfield.enumName	= it != _enums.end() ? it->data.name : "";
				bitfield.fileIndex	= fileIndex;
				bitfield.extension	= curr_ext;

				CHECK( _bitfields.insert( bitfield ).second );
				continue;
			}


			// alias
			if ( tokens.size() == 4						and
				 tokens[0] == "typedef"					and
				 tokens[3] == ";"						and
				 not _basicTypes.contains( tokens[1] )	and
				 not _resourceTypes.contains( tokens[1] ))
			{
				if ( StartsWith( tokens[2], tokens[1] ))
				{
					auto	st_iter = _structs.find( SearchableStruct{tokens[1]} );
					if ( st_iter != _structs.end() )
					{
						auto	alias = st_iter->data;

						CHECK( alias.extension.empty() );
						CHECK( not curr_ext.empty() );

						alias.name		= tokens[2];
						alias.extension = curr_ext;

						CHECK( _structs.insert( RVRef(alias) ).second );
						continue;
					}

					auto	en_iter = _enums.find( SearchableEnum{tokens[1]} );
					if ( en_iter != _enums.end() )
					{
						continue;
					}

					auto	bf_iter = _bitfields.find( SearchableBitfield{tokens[1]} );
					if ( bf_iter != _bitfields.end() )
					{
						continue;
					}

					AE_LOGI( "skip alias: "s << tokens[2] << " = " << tokens[1] );
				}
				else
					AE_LOGI( "skip alias: "s << tokens[2] << " = " << tokens[1] );
			}


			// define
			if ( tokens.size() > 1		and
				 tokens[0] == "#"		and
				 tokens[1] != "include" )
			{
				mode = ( tokens.back() == "\\" ? EMode::Define : EMode::None );
				continue;
			}

			// const
			if ( tokens.size() > 5		and
				 tokens[0] == "static"	and
				 tokens[1] == "const"	and
				 tokens[4] == "=" )
			{
				ConstInfo	ci;
				ci.type		= tokens[2];
				ci.name		= tokens[3];
				ci.value	= tokens[5];

				const char*	end = ci.value.data();
				for (; *end != ';' and *end != '\0'; ++end) {}
				ci.value = SubString( ci.value.data(), end );

				auto	it = _constants.emplace( ci.type, ConstSet_t{} ).first;

				it->second.push_back( RVRef(ci) );
				continue;
			}

			if ( tokens.size() > 1 and tokens[0] == "#" )
				CHECK( tokens[1] == "include" );
		}

		CHECK( skip_stack.size() == 1 );
		return true;
	}

/*
=================================================
	_BuildExtensionList
=================================================
*/
	bool  HeaderParser::_BuildExtensionList (StringView fileData, usize fileIdx)
	{
		Array< StringView >	lines;
		Parser::DivideLines( fileData, lines );

		Array< StringView >	tokens;

		for (auto& curr_line : lines)
		{
			Parser::DivideString_CPP( curr_line, OUT tokens );

			if ( tokens.size() > 5							and
				 tokens[0] == "#"							and
				 tokens[1] == "define"						and
				 EndsWith( tokens[2], "_EXTENSION_NAME" )	and
				 tokens[3] == "\""							and
				 tokens[5] == "\""							)
			{
				CHECK( _extensions.emplace( tokens[4], ubyte(fileIdx) ).second );
			}
		}
		return true;
	}

/*
=================================================
	RemoveEnumValDuplicates
=================================================
*/
	void  HeaderParser::RemoveEnumValDuplicates ()
	{
		for (auto& en : _enums)
		{
			auto&	fields = const_cast< Array<EnumField>& >(en.data.fields);

			HashSet<StringView>		names;

			for (auto& fl : fields) {
				CHECK( names.insert( fl.name ).second );
			}

			for (auto it = fields.begin(); it != fields.end();)
			{
				if ( names.contains( it->value ))
				{
					AE_LOGI( "Removed duplicated enum: "s << it->name << " = " << it->value );
					it = fields.erase( it );
				}
				else
					++it;
			}

			HashSet<StringView>		values;

			for (auto it = fields.begin(); it != fields.end();)
			{
				if ( not values.insert( it->value ).second )
				{
					AE_LOGI( "Removed duplicated enum: "s << it->name << " = " << it->value );
					it = fields.erase( it );
				}
				else
					++it;
			}
		}


		for (auto& const_set : _constants)
		{
			HashSet<StringView>		names;

			for (auto& c : const_set.second) {
				CHECK( names.insert( c.name ).second );
			}

			for (auto it = const_set.second.begin(); it != const_set.second.end();)
			{
				if ( names.contains( it->value ))
				{
					AE_LOGI( "Removed duplicated const: "s << it->name << " = " << it->value );
					it = const_set.second.erase( it );
				}
				else
					++it;
			}

			HashSet<StringView>		values;

			for (auto it = const_set.second.begin(); it != const_set.second.end();)
			{
				if ( not values.insert( it->value ).second )
				{
					AE_LOGI( "Removed duplicated const: "s << it->name << " = " << it->value );
					it = const_set.second.erase( it );
				}
				else
					++it;
			}
		}
	}

} // AE::Parsers
