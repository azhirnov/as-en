// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"
#include "CoreBindings_BindVectorMath.inl.h"

namespace AE::Scripting
{
/*
=================================================
    _BindVectorMath1
=================================================
*/
    void  CoreBindings::_BindVectorMath1 (const ScriptEnginePtr &se) __Th___
    {
        // declare
        DefineVector_Func   func{ se };

        VecTypes::Visit( func );
    }


} // AE::Scripting
