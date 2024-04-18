// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"
#include "CoreBindings_BindVectorMath.inl.h"

namespace AE::Scripting
{
/*
=================================================
	_BindVectorMath2
=================================================
*/
	void  CoreBindings::_BindVectorMath2 (const ScriptEnginePtr &se) __Th___
	{
		// bind
		BindVector_Func	func{ se };

		VecTypes::VisitTh( func );
	}


} // AE::Scripting
