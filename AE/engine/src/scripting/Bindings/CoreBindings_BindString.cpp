// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
	static String  String_FindAndReplace (const String &str, const String &oldStr, const String &newStr)
	{
		String	temp = str;
		if ( oldStr.size() == 1 and newStr.size() == 1 )
			Base::FindAndReplace( INOUT temp, oldStr[0], newStr[0] );
		else
			Base::FindAndReplace( INOUT temp, oldStr, newStr );
		return temp;
	}

	static bool  String_StartsWith (const String &str, const String &substr)
	{
		return Base::StartsWith( str, substr );
	}

	static bool  String_StartsWithIC (const String &str, const String &substr)
	{
		return Base::StartsWithIC( str, substr );
	}

	static bool  String_EndsWith (const String &str, const String &substr)
	{
		return Base::EndsWith( str, substr );
	}

	static bool  String_EndsWithIC (const String &str, const String &substr)
	{
		return Base::EndsWithIC( str, substr );
	}
}

/*
=================================================
	BindString
=================================================
*/
	void  CoreBindings::BindString (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		AngelScript::RegisterStdString( se->Get() );

		se->AddFunction( &String_FindAndReplace,	"FindAndReplace"	);
		se->AddFunction( &String_StartsWith,		"StartsWith"		);
		se->AddFunction( &String_StartsWithIC,		"StartsWithIC"		);
		se->AddFunction( &String_EndsWith,			"EndsWith"			);
		se->AddFunction( &String_EndsWithIC,		"EndsWithIC"		);
	}

} // AE::Scripting
