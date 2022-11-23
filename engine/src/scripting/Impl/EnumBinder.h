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

		void  Create ()								__Th___;

		void  AddValue (StringView name, T value)	__Th___;

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
	void  EnumBinder<T>::Create () __Th___
	{
		int	res = GetASEngine()->RegisterEnum( NtStringView{Name()}.c_str() );
		
		if ( res == AngelScript::asALREADY_REGISTERED )
			AE_LOGE( "enum '" + String{Name()} + "' already registerd" );

		AS_CHECK_THROW( res );
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

		AS_CHECK_THROW( GetASEngine()->RegisterEnumValue( NtStringView{Name()}.c_str(), (String{Name()} + '_' + String{valueName}).c_str(), int(value) ));
	}

} // AE::Scripting
