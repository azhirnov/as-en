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
	// types
	public:
		using Self		= EnumBinder<T>;
		using Enum_t	= T;


	// variables
	private:
		ScriptEnginePtr		_engine;
		String				_name;


	// methods
	public:
		explicit EnumBinder (const ScriptEnginePtr &eng);

		bool  Create ();

		bool  AddValue (StringView name, T value);

		ND_ StringView							Name ()		const	{ return _name; }
		ND_ Ptr< AngelScript::asIScriptEngine >	GetASEngine ()		{ return _engine->Get(); }
	};


	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	EnumBinder<T>::EnumBinder (const ScriptEnginePtr &eng) :
		_engine{ eng }
	{
		ScriptTypeInfo< T >::Name( OUT _name );
	}

/*
=================================================
	Create
=================================================
*/
	template <typename T>
	bool  EnumBinder<T>::Create ()
	{
		int	res = GetASEngine()->RegisterEnum( NtStringView{Name()}.c_str() );
		
		if ( res < 0 and res != AngelScript::asALREADY_REGISTERED )
		{
			AS_CALL( res );
			RETURN_ERR( "enum '" + String{Name()} + "' already registerd" );
		}
		return true;
	}
	
/*
=================================================
	AddValue
=================================================
*/
	template <typename T>
	bool  EnumBinder<T>::AddValue (StringView valueName, T value)
	{
		ASSERT( slong(value) >= MinValue<int>() and slong(value) <= MaxValue<int>() );

		AS_CALL_R( GetASEngine()->RegisterEnumValue( NtStringView{Name()}.c_str(), (String{Name()} + '_' + String{valueName}).c_str(), int(value) ));
		return true;
	}

} // AE::Scripting
