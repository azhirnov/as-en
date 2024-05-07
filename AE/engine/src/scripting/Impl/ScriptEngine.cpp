// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Impl/ScriptEngine.h"
#include "scripting/Impl/ScriptTypes.h"

namespace AE::Scripting
{

/*
=================================================
	constructor
=================================================
*/
	ScriptModule::ScriptModule (AngelScript::asIScriptModule* mod, ArrayView<ModuleSource> dbgSrc) :
		_module{ mod }
	{
		ASSERT( _module );
		Unused( dbgSrc );

		#if AE_DBG_SCRIPTS
			EXLOCK( _dbgLocationGuard );
			for (auto& src : dbgSrc) {
				CHECK( _dbgLocation.emplace( src.name, src.dbgLocation ).second );
			}
		#endif
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptModule::~ScriptModule () __NE___
	{
		if ( _module ) {
			_module->Discard();
		}
	}

/*
=================================================
	LogError
=================================================
*/
#if AE_DBG_SCRIPTS
	bool  ScriptModule::LogError (StringView fnEntry, StringView section, int line, int column, StringView exceptionMsg) const
	{
		String	str;
		str << "Exception thrown in entry: " << fnEntry << ", "
			<< section << " (" << ToString( line ) << ", " << ToString( column ) << "): "
			<< exceptionMsg;

		EXLOCK( _dbgLocationGuard );

		auto	iter = _dbgLocation.find( String{section} );
		if ( iter != _dbgLocation.end() )
		{
			AE_LOGW( str, iter->second.file, iter->second.line + line );
			return true;
		}
		return false;
	}
#endif
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	ScriptEngine::ScriptEngine () __NE___
	{
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptEngine::~ScriptEngine () __NE___
	{
		if ( _engine ) {
			_engine->ShutDownAndRelease();
		}

		AngelScript::UnregisterStdString();

		DEBUG_ONLY(
			auto	cnt = AngelScriptHelper::SimpleRefCounter::_dbgTotalCount.load();
			if ( cnt > 0 )
				AE_LOGI( "AngelScript: not an all ref counted objects are released" );
		)
	}

/*
=================================================
	Create
=================================================
*/
	bool  ScriptEngine::Create (Bool genCppHeader) __NE___
	{
		CHECK_ERR( not _engine );

		using namespace AngelScript;

		_engine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
		CHECK_ERR( _engine );

		_engine->SetMessageCallback( asFUNCTION( _MessageCallback ), this, asCALL_CDECL );

		#if AE_SCRIPT_CPP_REFLECTION
			_genCppHeader = genCppHeader;
		#else
			Unused( genCppHeader );
		#endif

		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  ScriptEngine::Create (AngelScript::asIScriptEngine* se, Bool genCppHeader) __NE___
	{
		CHECK_ERR( not _engine );
		CHECK_ERR( se != null );

		_engine = se;
		_engine->AddRef();

		#if AE_SCRIPT_CPP_REFLECTION
			_genCppHeader = genCppHeader;
		#else
			Unused( genCppHeader );
		#endif

		return true;
	}

/*
=================================================
	CreateModule
=================================================
*/
	ScriptModulePtr  ScriptEngine::CreateModule (ArrayView<ModuleSource> sources, ArrayView<StringView> defines, ArrayView<Path> includeDirs) __NE___
	{
		using namespace AngelScript;
		using ModulePtr = Unique< asIScriptModule, void (*)(asIScriptModule*) >;

		const String	name = "module_" + ToString( _moduleIndex.Inc() );

		CHECK_ERR( not _engine->GetModule( name.c_str(), asGM_ONLY_IF_EXISTS ));

		ModulePtr	module{
			_engine->GetModule( name.c_str(), asGM_ALWAYS_CREATE ),
			[](asIScriptModule* m) { m->Discard(); }
		};

		for (auto& src : sources)
		{
			String		temp;
			StringView	script	= src.script;

			if ( src.usePreprocessor )
			{
				CHECK_ERR( _Preprocessor2( script, OUT temp, defines, includeDirs ));
				script = temp;
			}

			AS_CHECK_ERR( module->AddScriptSection( src.name.c_str(), script.data(), script.length() ));

			#if AE_DBG_SCRIPTS
				EXLOCK( _dbgLocationGuard );
				CHECK( _dbgLocation.emplace( src.name, src.dbgLocation ).second );
			#endif
		}

		auto	res = module->Build();

		#if AE_DBG_SCRIPTS
			EXLOCK( _dbgLocationGuard );
			for (auto& src : sources) {
				_dbgLocation.erase( src.name );
			}
		#endif

		AS_CHECK_ERR( res );

		return ScriptModulePtr{ new ScriptModule{ module.release(), sources }};
	}

/*
=================================================
	_Preprocessor
=================================================
*/
namespace
{
	inline void  SkipSpaces (StringView str, INOUT usize &pos) __NE___
	{
		for (; pos < str.size(); ++pos)
		{
			const char	c = str[pos];
			if ( not ((c == ' ') or (c == '\t')) )
				break;
		}
	}

	ND_ inline bool  IsNumber (char c) __NE___
	{
		return	(c >= '0') and (c <= '9');
	}

	ND_ inline bool  IsWordBegin (char c) __NE___
	{
		return	((c >= 'A') and (c <= 'Z')) or
				((c >= 'a') and (c <= 'z')) or
				(c == '_');
	}

	ND_ inline bool  IsWord (char c) __NE___
	{
		return IsWordBegin( c ) or IsNumber( c );
	}

	ND_ inline bool  IsSpaceOrSymb (char c) __NE___
	{
		return not IsWord( c );
	}
}

	bool  ScriptEngine::_Preprocessor (StringView							str,
									   OUT String							&dst,
									   OUT Array<Pair< StringView, usize >>	&includeFileAndPos,
									   ArrayView<StringView>				defines) __Th___
	{
		usize		begin_block		= 0;
		Array<bool>	include_scope;	include_scope.push_back(true);

		const auto	CheckMacro = [str, defines, &begin_block, &include_scope] (INOUT usize &pos)
		{{
			SkipSpaces( str, INOUT pos );

			bool	include = false;

			if ( IsWordBegin( str[pos] ))
			{
				const usize	begin = pos;

				// move to end of word
				for (; IsWord( str[pos] ); ++pos) {}

				StringView	macro_name	= str.substr( begin, pos - begin );

				for (auto def : defines) {
					if ( def == macro_name ) {
						include = true;
						break;
					}
				}
			}

			if ( IsNumber( str[pos] ))
			{
				const usize	begin = pos;

				// move to end of number
				for (; IsNumber( str[pos] ); ++pos) {}

				StringView	number_str	= str.substr( begin, pos - begin );
				int			number		= StringToInt( number_str, 10 );

				include = (number != 0);
			}

			include &= include_scope.back();
			include_scope.push_back( include );

			if ( include )
			{
				ASSERT( begin_block == UMax );
				begin_block = pos;
				Parser::ToEndOfLine( str, INOUT begin_block );
			}
		}};

		bool	multiline_strings_assert_once = true;
		usize	pos = 1;

		for (; pos < str.size();)
		{
			const char	c = str[pos-1];
			const char	n = str[pos];

			// new line
			if_unlikely( (c == '\r') and (n == '\n') )
			{
				++pos;
				continue;
			}
			if_unlikely( (c == '\n') or (c == '\r') )
			{
				if ( not include_scope.back() )
					dst << '\n';
			}

			// single line comment
			if_unlikely( (c == '/') and (n == '/') )
			{
				Parser::ToNextLine( str, INOUT pos );
				continue;
			}

			// multi line comment
			if_unlikely( (c == '/') and (n == '*') )
			{
				pos += 2;
				for (; pos < str.size();)
				{
					const char	a = str[pos-1];
					const char	b = str[pos];

					// new line
					if_unlikely( (a == '\r') and (b == '\n') )
					{
						++pos;
						continue;
					}
					if_unlikely( (a == '\n') or (a == '\r') )
					{
						if ( not include_scope.back() )
							dst << '\n';
					}

					if_unlikely( (a == '*') and (b == '/') )
					{
						pos += 2;
						break;
					}

					++pos;
				}
				continue;
			}

			// string
			if_unlikely( c == '"' )
			{
				++pos;
				for (; pos < str.size();)
				{
					const char	a = str[pos-1];
					const char	b = str[pos];
					++pos;

					#ifdef AE_DEBUG
					if ( a == '\n' and multiline_strings_assert_once ) {
						multiline_strings_assert_once = false;
						CHECK_MSG( false, "multiline strings are not supported" );
					}
					#endif
					Unused( multiline_strings_assert_once );

					if_unlikely( a == '"' )
						break;

					if_unlikely( (a != '\\') and (b == '"') )
					{
						++pos;
						break;
					}
				}
				continue;
			}

			// macros
			if_unlikely( ((c != '#') and (n == '#')) or
						 ((pos == 1) and (c == '#')) )
			{
				if ( n == '#' )	++pos;
				SkipSpaces( str, INOUT pos );

				if ( begin_block != UMax )
				{
					usize tmp = pos;
					Parser::ToBeginOfLine( str, INOUT tmp );

					dst << str.substr( begin_block, tmp - begin_block );
					begin_block = UMax;
				}

				if ( str.substr( pos, 5 ) == "ifdef" )
				{
					pos += 5;
					CheckMacro( INOUT pos );
				}
				else
				if ( str.substr( pos, 2 ) == "if" )
				{
					pos += 2;
					CheckMacro( INOUT pos );
				}
				else
				if ( str.substr( pos, 4 ) == "else" )
				{
					bool	include = not include_scope.back();
					include_scope.pop_back();
					if ( not include_scope.empty() )
						include &= include_scope.back();
					include_scope.push_back( include );

					if ( include )
					{
						ASSERT( begin_block == UMax );
						begin_block = pos;
						Parser::ToEndOfLine( str, INOUT begin_block );
					}
				}
				else
				if ( str.substr( pos, 5 ) == "endif" )
				{
					ASSERT( include_scope.size() > 1 );
					if ( include_scope.size() > 1 )
						include_scope.pop_back();

					if ( include_scope.back() )
					{
						ASSERT( begin_block == UMax );
						begin_block = pos;
						Parser::ToEndOfLine( str, INOUT begin_block );
					}
				}
				else
				if ( str.substr( pos, 7 ) == "include" )
				{
					if ( include_scope.back() )
					{
						begin_block = pos;
						Parser::ToNextLine( str, INOUT begin_block );

						pos += 7;
						SkipSpaces( str, INOUT pos );

						if ( str[pos] == '<' )
						{
							// skip system include
							pos = str.find( '>', pos );
							CHECK_ERR( pos < begin_block );
						}
						else
						if ( str[pos] == '"' )
						{
							StringView	fname;
							CHECK_ERR( Parser::ReadString( str, INOUT pos, OUT fname ));
							includeFileAndPos.emplace_back( fname, dst.size() );
						}
						else
							RETURN_ERR( "not supported" );

						dst << '\n';
						pos = begin_block;
					}
				}
				#ifdef AE_DEBUG
					else if ( str.substr( pos, 5 ) == "undef" )		{}
					else if ( str.substr( pos, 6 ) == "define" )	{}
					else if ( str.substr( pos, 6 ) == "pragma" )	{}
					else if ( str.substr( pos, 9 ) == "extension" )	{}	// GLSL
					//else DBG_WARNING( "unknown macros" );
				#endif

				continue;
			}

			if_unlikely( ((c == ':') and (n == ':')) or
						 ((c == 'R') and (n == 'C')) )
			{
				if ( include_scope.back() )
				{
					ASSERT( begin_block != UMax );

					bool	is_ns	= (c == ':') and (n == ':');
					is_ns	&= (pos >= 2 ? IsWord( str[pos-2] ) : true);
					is_ns	&= (pos+1 < str.size() ? IsWord( str[pos+1] ) : false);

					bool	is_rc	= (c == 'R') and (n == 'C');
					is_rc	&= (pos >= 2 ? IsSpaceOrSymb( str[pos-2] ) : true);
					is_rc	&= (pos+1 < str.size() ? str[pos+1] == '<' : false);

					if ( is_ns )
					{
						dst << str.substr( begin_block, pos-1 - begin_block ) << '_';
						begin_block = pos+1;
					}

					if ( is_rc )
					{
						dst << str.substr( begin_block, pos-1 - begin_block );

						SkipSpaces( str, INOUT pos += 2 );
						CHECK_ERR( IsWordBegin( str[pos] ));
						const usize	begin = pos;	// skip 'RC<'

						for (; IsWord( str[pos] ); ++pos) {}
						const usize	end = pos;

						SkipSpaces( str, INOUT pos );
						CHECK_ERR( str[pos] == '>' );

						dst << str.substr( begin, end - begin ) << '@';
						begin_block = ++pos;
					}
				}
			}

			//ASSERT( c != '@' );

			++pos;
		}

		ASSERT( include_scope.size() == 1 );

		if ( begin_block != UMax and
			 begin_block <  pos  and
			 include_scope.back() )
		{
			dst << str.substr( begin_block );
		}

		// TODO: check is it a word
		FindAndReplace( INOUT dst, "INOUT ",	"      " );
		FindAndReplace( INOUT dst, "OUT ",		"    " );
		FindAndReplace( INOUT dst, "ND_ ",		"    " );

		return true;
	}

/*
=================================================
	_Preprocessor2
=================================================
*/
	bool  ScriptEngine::_Preprocessor2 (StringView str, OUT String &dst, ArrayView<StringView> defines, ArrayView<Path> includeDirs) __NE___
	{
		const auto	ReadFile = [&includeDirs] (StringView fname, OUT String &s) -> bool
		{{
			Path	path;
			for (auto& dir : includeDirs)
			{
				path = dir / fname;
				if ( FileSystem::IsFile( path ))
				{
					FileRStream  file {path};
					return file.IsOpen() and file.Read( file.RemainingSize(), OUT s );
				}
			}
			RETURN_ERR( "failed to find included file: '"s << fname << "'" );
		}};

		TRY{
			Array<Pair< StringView, usize >>	file_and_pos;

			dst.clear();
			CHECK_ERR( _Preprocessor( str, OUT dst, file_and_pos, defines ));
			CHECK_ERR( file_and_pos.empty() or not includeDirs.empty() );

			usize	offset = 0;
			for (auto& [fname, pos] : file_and_pos)
			{
				String	in, out;
				CHECK_ERR( ReadFile( fname, OUT in ));
				CHECK_ERR( _Preprocessor2( in, OUT out, defines, includeDirs ));

				dst.insert( dst.begin() + pos + offset, out.begin(), out.end() );
				offset += out.size();
			}
			return true;
		}
		CATCH_ALL(
			return false;
		)
	}

/*
=================================================
	_CreateContext
=================================================
*/
	bool  ScriptEngine::_CreateContext (const String &signature, const ScriptModulePtr &module, OUT AngelScript::asIScriptContext* &ctx)
	{
		using namespace AngelScript;

		ctx = _engine->CreateContext();
		CHECK_ERR( ctx != null );

		asIScriptFunction* func = module->_module->GetFunctionByDecl( signature.c_str() );
		CHECK_ERR_MSG( func, "can't find function '"s << signature << "' in module '" << module->GetName() << "'" );

		AS_CHECK_ERR( ctx->Prepare( func ));
		return true;
	}

/*
=================================================
	IsRegistered
=================================================
*/
	bool  ScriptEngine::IsRegistered (NtStringView name) __NE___
	{
		auto*	info = _engine->GetTypeInfoByName( name.c_str() );
		return info != null;
	}

/*
=================================================
	Typedef
=================================================
*/
	void  ScriptEngine::Typedef (NtStringView newType, NtStringView existType) __Th___
	{
		using namespace AngelScript;

		CHECK_THROW( not existType.empty() );
		CHECK_THROW( not newType.empty() );

		AS_CHECK_THROW( _engine->RegisterTypedef( newType.c_str(), existType.c_str() ));

		if_unlikely( IsUsingCppHeader() )
		{
			AddCppHeader( String{newType}, ("using "s << newType << " = " << existType << ";\n"), int(asOBJ_MASK_VALID_FLAGS) );
		}
	}

/*
=================================================
	SetNamespace
=================================================
*/
	bool  ScriptEngine::SetNamespace (NtStringView name) __NE___
	{
		AS_CHECK_ERR( _engine->SetDefaultNamespace( name.c_str() ));
		return true;
	}

/*
=================================================
	SetDefaultNamespace
=================================================
*/
	bool  ScriptEngine::SetDefaultNamespace () __NE___
	{
		return SetNamespace( "" );
	}

/*
=================================================
	AddCppHeader
=================================================
*/
	void  ScriptEngine::AddCppHeader (String typeName, String str, int flags)
	{
	#if AE_SCRIPT_CPP_REFLECTION
		if ( str.empty() )
			return;

		EXLOCK( _cppHeaderGuard );
		if ( _genCppHeader )
		{
			auto [it, inserted] = _cppHeaderMap.emplace( RVRef(typeName), MakePair( _cppHeaders.size(), flags ));

			if ( inserted )
				_cppHeaders.emplace_back( RVRef(str) );
			else{
				_cppHeaders[ it->second.first ] << str;
				ASSERT( flags == 0 );
			}
		}
	#else
		Unused( typeName, str, flags );
	#endif
	}

/*
=================================================
	GetCppHeader
=================================================
*/
	void  ScriptEngine::GetCppHeader (OUT String &str, OUT HashVal32 &hash)
	{
		str.clear();
		hash = Default;

	#if AE_SCRIPT_CPP_REFLECTION
		EXLOCK( _cppHeaderGuard );

		str << "#include <vector>\n";
		str << "#include <string>\n\n";

		str << "using int8		= std::int8_t;\n";
		str << "using uint8		= std::uint8_t;\n";
		str << "using int16		= std::int16_t;\n";
		str << "using uint16	= std::uint16_t;\n";
		str << "using int		= std::int32_t;\n";
		str << "using uint		= std::uint32_t;\n";
		str << "using int32		= std::int32_t;\n";
		str << "using uint32	= std::uint32_t;\n";
		str << "using int64		= std::int64_t;\n";
		str << "using uint64	= std::uint64_t;\n";
		str << "using string	= std::string;\n\n";

		str << "template <typename T>\nstruct RC;\n\n";
		str << "template <typename T>\nusing array = std::vector<T>;\n\n";

		for (auto& [name, p] : _cppHeaderMap)
		{
			if_unlikely( name.empty() )
				continue;

			if_unlikely( p.second == int(AngelScript::asOBJ_MASK_VALID_FLAGS) )
				continue;

			str << "struct " << name << ";\n";
		}

		str << "\n";

		for (auto& hdr : _cppHeaders)
		{
			str << hdr;
			hash << CT_Hash( hdr.data(), hdr.length(), 0 );

			if ( StartsWith( hdr, "struct " ))
				str << "};\n\n";
		}

		for (auto& [name, p] : _cppHeaderMap)
		{
			if_unlikely( p.second == int(AngelScript::asOBJ_MASK_VALID_FLAGS) )
				continue;

			if ( p.second & AngelScript::asOBJ_REF )
			{
				str << "template <>\nstruct RC<" << name << "> : " << name;
				str << "\n{\n" << "\tRC (const " << name << " &);\n";
				str << "};\n\n";
			}
		}

		// free memory
		Reconstruct( _cppHeaders );
		Reconstruct( _cppHeaderMap );
	#endif
	}

/*
=================================================
	SaveCppHeader
=================================================
*/
	bool  ScriptEngine::SaveCppHeader (const Path &fname)
	{
	#if	AE_SCRIPT_CPP_REFLECTION
		HashVal32	prev_hash;

		if ( FileSystem::IsFile( fname ))
		{
			FileRStream	file {fname};

			if ( file.IsOpen() )
			{
				char	hash_str [2+8+1] = {};
				if ( file.Read( OUT hash_str, Sizeof(hash_str) ))
				{
					ASSERT( hash_str[0] == '/' );
					ASSERT( hash_str[1] == '/' );
					ASSERT( hash_str[10] == '\n' );
					prev_hash = HashVal32{StringToUInt( StringView{hash_str}.substr( 2 ), 16 )};
				}
			}
		}

		String		src;
		HashVal32	hash;

		GetCppHeader( OUT src, OUT hash );
		CHECK_ERR( not src.empty() );

		if ( hash != prev_hash )
		{
			FileWStream	file {fname};
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Write( "//"s << FormatAlignedI<16>( uint{hash}, 8, '0' ) << "\n" ));
			CHECK_ERR( file.Write( src ));
		}
		return true;

	#else
		Unused( fname );
		AE_LOGI( "can't save C++ headers for script because AE_SCRIPT_CPP_REFLECTION is disabled" );
		return true;
	#endif
	}

/*
=================================================
	_MessageCallback
=================================================
*/
	void  ScriptEngine::_MessageCallback (const AngelScript::asSMessageInfo* msg, void* self)
	{
		using namespace AngelScript;

		String	str("AngelScript message: ");

		str << msg->section << " (" << ToString( msg->row ) << ", " << ToString( msg->col ) << ") ";

		switch ( msg->type )
		{
			case asMSGTYPE_WARNING :		str << "WARN  ";	break;
			case asMSGTYPE_INFORMATION :	str << "INFO  ";	break;
			case asMSGTYPE_ERROR :			str << "ERROR ";	break;
		}

		str << msg->message;

		StringView	msg_str		= msg->message;
		StringView	code_mark	= "(Code: ";

		if ( usize pos = msg_str.find( code_mark ); pos != StringView::npos )
		{
			const usize		begin	= pos + code_mark.length();
			StringView		code	= msg_str.substr( begin );

			//if ( code.Find( ')', OUT pos ))
			//	code = code.SubString( 0, pos );

			const auto	int_code = std::strtol( NtStringView{code}.c_str(), null, 0 );

			str << ", code name: ";

			switch ( int_code )
			{
				case asERetCodes::asERROR :									str << "asERROR";						break;
				case asERetCodes::asCONTEXT_ACTIVE :						str << "asCONTEXT_ACTIVE";				break;
				case asERetCodes::asCONTEXT_NOT_FINISHED :					str << "asCONTEXT_NOT_FINISHED";		break;
				case asERetCodes::asCONTEXT_NOT_PREPARED :					str << "asCONTEXT_NOT_PREPARED";		break;
				case asERetCodes::asINVALID_ARG :							str << "asINVALID_ARG";					break;
				case asERetCodes::asNO_FUNCTION :							str << "asNO_FUNCTION";					break;
				case asERetCodes::asNOT_SUPPORTED :							str << "asNOT_SUPPORTED";				break;
				case asERetCodes::asINVALID_NAME :							str << "asINVALID_NAME";				break;
				case asERetCodes::asNAME_TAKEN :							str << "asNAME_TAKEN";					break;
				case asERetCodes::asINVALID_DECLARATION :					str << "asINVALID_DECLARATION";			break;
				case asERetCodes::asINVALID_OBJECT :						str << "asINVALID_OBJECT";				break;
				case asERetCodes::asINVALID_TYPE :							str << "asINVALID_TYPE";				break;
				case asERetCodes::asALREADY_REGISTERED :					str << "asALREADY_REGISTERED";			break;
				case asERetCodes::asMULTIPLE_FUNCTIONS :					str << "asMULTIPLE_FUNCTIONS";			break;
				case asERetCodes::asNO_MODULE :								str << "asNO_MODULE";					break;
				case asERetCodes::asNO_GLOBAL_VAR :							str << "asNO_GLOBAL_VAR";				break;
				case asERetCodes::asINVALID_CONFIGURATION :					str << "asINVALID_CONFIGURATION";		break;
				case asERetCodes::asINVALID_INTERFACE :						str << "asINVALID_INTERFACE";			break;
				case asERetCodes::asCANT_BIND_ALL_FUNCTIONS :				str << "asCANT_BIND_ALL_FUNCTIONS";		break;
				case asERetCodes::asLOWER_ARRAY_DIMENSION_NOT_REGISTERED :	str << "asLOWER_ARRAY_DIMENSION_NOT_REGISTERED"; break;
				case asERetCodes::asWRONG_CONFIG_GROUP :					str << "asWRONG_CONFIG_GROUP";			break;
				case asERetCodes::asCONFIG_GROUP_IS_IN_USE :				str << "asCONFIG_GROUP_IS_IN_USE";		break;
				case asERetCodes::asILLEGAL_BEHAVIOUR_FOR_TYPE :			str << "asILLEGAL_BEHAVIOUR_FOR_TYPE";	break;
				case asERetCodes::asWRONG_CALLING_CONV :					str << "asWRONG_CALLING_CONV";			break;
				case asERetCodes::asBUILD_IN_PROGRESS :						str << "asBUILD_IN_PROGRESS";			break;
				case asERetCodes::asINIT_GLOBAL_VARS_FAILED :				str << "asINIT_GLOBAL_VARS_FAILED";		break;
				case asERetCodes::asOUT_OF_MEMORY :							str << "asOUT_OF_MEMORY";				break;
				case asERetCodes::asMODULE_IS_IN_USE :						str << "asMODULE_IS_IN_USE";			break;
				default :													str << "unknown";
			}
		}

		#if AE_DBG_SCRIPTS
		{
			ScriptEngine*	eng = Cast<ScriptEngine>(self);
			EXLOCK( eng->_dbgLocationGuard );

			auto	iter = eng->_dbgLocation.find( String{msg->section} );
			if ( iter != eng->_dbgLocation.end() )
			{
				AE_LOGI( str, iter->second.file, iter->second.line + msg->row );
				return;
			}
		}
		#endif
		Unused( self );

		AE_LOGI( str );
	}

/*
=================================================
	_CheckError
=================================================
*/
	bool  ScriptEngine::_CheckError (int err, StringView asFunc, StringView func, const SourceLoc &loc) __NE___
	{
		using namespace AngelScript;

		if_likely( err >= 0 )
			return true;

		String	str("AngelScript error: ");

		#define __AS_CASE_ERR( _val_ ) \
			case _val_ : str << AE_TOSTRING( _val_ ); break;

		switch ( err )
		{
			__AS_CASE_ERR( asERROR );
			__AS_CASE_ERR( asCONTEXT_ACTIVE );
			__AS_CASE_ERR( asCONTEXT_NOT_FINISHED );
			__AS_CASE_ERR( asCONTEXT_NOT_PREPARED );
			__AS_CASE_ERR( asINVALID_ARG );
			__AS_CASE_ERR( asNO_FUNCTION  );
			__AS_CASE_ERR( asNOT_SUPPORTED );
			__AS_CASE_ERR( asINVALID_NAME );
			__AS_CASE_ERR( asNAME_TAKEN );
			__AS_CASE_ERR( asINVALID_DECLARATION );
			__AS_CASE_ERR( asINVALID_OBJECT );
			__AS_CASE_ERR( asINVALID_TYPE );
			__AS_CASE_ERR( asALREADY_REGISTERED );
			__AS_CASE_ERR( asMULTIPLE_FUNCTIONS );
			__AS_CASE_ERR( asNO_MODULE );
			__AS_CASE_ERR( asNO_GLOBAL_VAR );
			__AS_CASE_ERR( asINVALID_CONFIGURATION );
			__AS_CASE_ERR( asINVALID_INTERFACE );
			__AS_CASE_ERR( asCANT_BIND_ALL_FUNCTIONS );
			__AS_CASE_ERR( asLOWER_ARRAY_DIMENSION_NOT_REGISTERED );
			__AS_CASE_ERR( asWRONG_CONFIG_GROUP );
			__AS_CASE_ERR( asCONFIG_GROUP_IS_IN_USE );
			__AS_CASE_ERR( asILLEGAL_BEHAVIOUR_FOR_TYPE );
			__AS_CASE_ERR( asWRONG_CALLING_CONV );
			__AS_CASE_ERR( asBUILD_IN_PROGRESS );
			__AS_CASE_ERR( asINIT_GLOBAL_VARS_FAILED );
			__AS_CASE_ERR( asOUT_OF_MEMORY );

			default :
				str << "code: 0x" << ToString<16>( err );
				break;
		}

		#undef __AS_CASE_ERR

		str << ", in " << asFunc;
		str << ", function: " << func;

		AE_LOGE( str, loc.file, loc.line );
		return false;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	helpers to use multiple ScriptEngine instances in different threads
	see https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_multithread.html
=================================================
*/
	ScriptEngineMultithreadingScope::ScriptEngineMultithreadingScope ()
	{
		AngelScript::asPrepareMultithread();
	}

	ScriptEngineMultithreadingScope::~ScriptEngineMultithreadingScope ()
	{
		AngelScript::asUnprepareMultithread();
	}

	ScriptThreadScope::~ScriptThreadScope ()
	{
		AngelScript::asThreadCleanup();
	}


} // AE::Scripting
