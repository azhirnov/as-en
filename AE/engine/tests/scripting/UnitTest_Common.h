// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/Scripting.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Scripting;

template <typename Fn, typename R, typename ...Args> requires( not IsVoid< typename FunctionInfo<Fn>::result >)
Nd__In bool  Run (const ScriptEnginePtr &se, StringView script, const String &entry, const SourceLoc &dbgLocation, OUT R &result, Args&& ...args)
{
	auto mod = se->CreateModule({ScriptEngine::ModuleSource{ "def", script, dbgLocation }});
	auto scr = se->CreateScript<Fn>( entry, mod );
	if ( not scr )
		return false;

	auto res = scr->Run( FwdArg<Args>(args)... );
	if ( not res.has_value() )
		return false;

	if constexpr( AngelScriptHelper::IsSharedPtrNoQual<R> )
		result.Attach( RVRef( res.value() ));
	else
		result = RVRef( res.value() );

	return true;
}

template <typename Fn, typename R, typename ...Args> requires( not IsVoid< typename FunctionInfo<Fn>::result >)
Nd__In bool  Run (const ScriptEnginePtr &se, StringView script, const String &entry, OUT R &result, Args&& ...args)
{
	return Run< Fn, R >( se, script, entry, SourceLoc{}, OUT result, FwdArg<Args>(args)... );
}

template <typename Fn, typename ...Args> requires( IsVoid< typename FunctionInfo<Fn>::result >)
Nd__In bool  Run (const ScriptEnginePtr &se, StringView script, const String &entry, Args&& ...args)
{
	auto mod = se->CreateModule({ ScriptEngine::ModuleSource{"def", script} });
	auto scr = se->CreateScript<Fn>( entry, mod );
	if ( not scr )
		return false;

	return scr->Run( FwdArg<Args>(args)... );
}
