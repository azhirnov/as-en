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

		ND_ NtStringView						Name ()		const	{ return _name; }
		ND_ Ptr< AngelScript::asIScriptEngine >	GetASEngine ()		{ return _engine->Get(); }
	};


}	// AE::Scripting
