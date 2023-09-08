// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Scripting.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Scripting;

template <typename Fn, typename R, typename ...Args>
ND_ inline EnableIf< not IsSameTypes<typename FunctionInfo<Fn>::result, void>, bool >
Run (const ScriptEnginePtr &se, StringView script, const String &entry, const SourceLoc &dbgLocation, OUT R &result, Args&& ...args)
{
    auto mod = se->CreateModule({ScriptEngine::ModuleSource{ "def", script, dbgLocation }});
    auto scr = se->CreateScript<Fn>( entry, mod );
    auto res = scr->Run( FwdArg<Args>(args)... );
    if ( not res.has_value() )
        return false;

    if constexpr( AngelScriptHelper::IsSharedPtrNoQual<R> )
        result.Attach( RVRef( res.value() ));
    else
        result = RVRef( res.value() );

    return true;
}

template <typename Fn, typename R, typename ...Args>
ND_ inline EnableIf< not IsSameTypes<typename FunctionInfo<Fn>::result, void>, bool >
Run (const ScriptEnginePtr &se, StringView script, const String &entry, OUT R &result, Args&& ...args)
{
    return Run< Fn, R >( se, script, entry, SourceLoc{}, OUT result, FwdArg<Args>(args)... );
}

template <typename Fn, typename ...Args>
ND_ inline EnableIf< IsSameTypes<typename FunctionInfo<Fn>::result, void>, bool >
Run (const ScriptEnginePtr &se, StringView script, const String &entry, Args&& ...args)
{
    auto mod = se->CreateModule({ ScriptEngine::ModuleSource{"def", script} });
    auto scr = se->CreateScript<Fn>( entry, mod );
    return scr->Run( FwdArg<Args>(args)... );
}
