// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ClassBinder.h"

namespace AE::Scripting
{

	//
	// Enum Binder
	//
	template <typename T>
	class EnumBinder final
	{
		StaticAssert( sizeof(T) > 0, "type is not implemented" );

	// types
	public:
		using Self		= EnumBinder<T>;
		using Enum_t	= T;


	// variables
	private:
		ScriptEnginePtr		_engine;
		String				_name;

		const bool			_genHeader	= false;
	  #if AE_SCRIPT_CPP_REFLECTION
		String				_header;
		String				_header2;
	  #endif


	// methods
	public:
		explicit EnumBinder (const ScriptEnginePtr &eng)		__NE___;
		~EnumBinder ()											__NE___;

			void  Create ()										__Th___;
		ND_ bool  IsRegistered ()								C_NE___	{ return _engine->IsRegistered( _name ); }

			void  AddValue (StringView name, T value)			__Th___;

		// can be used to write docs in code
			void  Comment (StringView text)						__Th___;

		ND_ StringView							Name ()			C_NE___	{ return _name; }
		ND_ const ScriptEnginePtr &				GetEngine ()	C_NE___	{ return _engine; }
		ND_ Ptr< AngelScript::asIScriptEngine >	GetASEngine ()	__NE___	{ return _engine->Get(); }
	};



/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	EnumBinder<T>::EnumBinder (const ScriptEnginePtr &eng) __NE___ :
		_engine{ eng }, _genHeader{ eng->IsUsingCppHeader() }
	{
		ScriptTypeInfo< T >::Name( OUT _name );
	}

/*
=================================================
	destructor
=================================================
*/
	template <typename T>
	EnumBinder<T>::~EnumBinder () __NE___
	{
	  #if AE_SCRIPT_CPP_REFLECTION
		if_unlikely( _genHeader )
			_engine->AddCppHeader( RVRef(_name), RVRef(_header << _header2), AngelScript::asOBJ_ENUM );
	  #endif
	}

/*
=================================================
	Create
=================================================
*/
	template <typename T>
	void  EnumBinder<T>::Create () __Th___
	{
		int	res = GetASEngine()->RegisterEnum( _name.c_str() );

		if ( res == AngelScript::asALREADY_REGISTERED )
			AE_LOGE( "enum '" + String{Name()} + "' already registered" );

		AS_CHECK_THROW( res );

	  #if AE_SCRIPT_CPP_REFLECTION
		if_unlikely( _genHeader )
		{
			const String	int_type = "uint"s << ToString(sizeof(T)*8);
			_header << "enum class " << _name << " : " << int_type << "\n{\n";

			_header2 << "};\n";
			_header2 << int_type << "  operator | (" << _name << " lhs, " << _name << " rhs);\n";
			_header2 << int_type << "  operator | (" << int_type << " lhs, " << _name << " rhs);\n";
			_header2 << int_type << "  operator | (" << _name << " lhs, " << int_type << " rhs);\n";
		}
	  #endif
	}

/*
=================================================
	AddValue
=================================================
*/
	template <typename T>
	void  EnumBinder<T>::AddValue (StringView valueName, T value) __Th___
	{
		ASSERT( slong(value) >= MinValue<int>() and slong(value) <= MaxValue<int>() );

		CHECK_THROW( not valueName.empty() );
		AS_CHECK_THROW( GetASEngine()->RegisterEnumValue( _name.c_str(), (String{Name()} << '_' << valueName).c_str(), int(value) ));

	  #if AE_SCRIPT_CPP_REFLECTION
		if_unlikely( _genHeader )
		{
			if ( Parser::CPP.IsWordBegin( valueName[0] ))
			{
				_header << "\t" << valueName << ",\n";
			}
			else
			{
				_header2 << "static constexpr " << _name << ' ';
				_header2 << _name << "_" << valueName << " = ";
				_header2 << _name << "(" << ToString( ulong(value) ) << ");\n";
			}
		}
	  #endif
	}

/*
=================================================
	Comment
=================================================
*/
	template <typename T>
	void  EnumBinder<T>::Comment (StringView text) __Th___
	{
	  #if AE_SCRIPT_CPP_REFLECTION
		if_unlikely( _genHeader and not text.empty() )
		{
			_header << '\n';
			for (usize pos = 0; pos < text.size();)
			{
				StringView	line;
				Parser::ReadCurrLine( text, INOUT pos, OUT line );
				_header << "\t// " << line << '\n';
			}
		}
	  #endif
		Unused( text );
	}


} // AE::Scripting
