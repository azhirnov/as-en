// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/FnWrapper.h"
#include "scripting/Impl/ScriptEngine.inl.h"
#include "CoreBindings_BindVectorMath.inl.h"

namespace AE::Scripting
{
/*
=================================================
	_BindVectorMath3
=================================================
*/
	void  CoreBindings::_BindVectorMath3 (const ScriptEnginePtr &se) __Th___
	{
		// bind
		BindIntVector_Func	func{ se };

		VecTypes::VisitTh( func );
	}


} // AE::Scripting
