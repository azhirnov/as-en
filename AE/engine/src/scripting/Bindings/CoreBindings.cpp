// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{

/*
=================================================
	BindStdTypes
=================================================
*/
	void  CoreBindings::BindStdTypes (const ScriptEnginePtr &se) __Th___
	{
		se->Typedef( "sbyte",	"int8" );
		se->Typedef( "ubyte",	"uint8" );
		se->Typedef( "sshort",	"int16" );
		se->Typedef( "ushort",	"uint16" );
		se->Typedef( "sint",	"int" );
		se->Typedef( "slong",	"int64" );
		se->Typedef( "ulong",	"uint64" );
	}

/*
=================================================
	BindArray
=================================================
*/
	void  CoreBindings::BindArray (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		AngelScript::RegisterScriptArray( se->Get(), false );
	}

/*
=================================================
	LogFunc
=================================================
*/
	struct LogFunc
	{
		static void  Err (const String &msg) {
			AE_LOGW( msg );
		}

		static void  Info (const String &msg) {
			AE_LOGI( msg );
		}

		static void  Dbg (const String &msg) {
			AE_LOG_DBG( msg );
			Unused( msg );
		}

		static void  Fatal (const String &msg) __Th___ {
			AE_LOGW( msg );
			throw std::runtime_error{ msg.c_str() };
		}

		static void  Assert1 (bool expr) __Th___ {
			if ( not expr )
				Fatal( "" );  // throw
		}

		static void  Assert2 (bool expr, const String &msg) __Th___ {
			if ( not expr )
				Fatal( msg );  // throw
		}
	};

/*
=================================================
	BindLog
=================================================
*/
	void CoreBindings::BindLog (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		AS_GLOBAL_FN( se, LogFunc::Err,		"LogError",		{"msg"} );
		AS_GLOBAL_FN( se, LogFunc::Info,	"LogInfo",		{"msg"} );
		AS_GLOBAL_FN( se, LogFunc::Dbg,		"LogDebug",		{"msg"} );
		AS_GLOBAL_FN( se, LogFunc::Fatal,	"LogFatal",		{"msg"} );
		AS_GLOBAL_FN( se, LogFunc::Assert1,	"Assert",		{"expr"} );
		AS_GLOBAL_FN( se, LogFunc::Assert2,	"Assert",		{"expr", "msg"} );
	}

/*
=================================================
	BindVectorSwizzle
=================================================
*/
namespace
{
	struct BindVecSwizzle_Func
	{
		static void  Ctor (OUT void* mem, const String &str)
		{
			VecSwizzle	temp = Base::operator ""_vecSwizzle( str.c_str(), str.length() );

			PlacementNew<VecSwizzle>( OUT mem, temp );
		}

		static void  Bind (const ScriptEnginePtr &se)
		{
			ClassBinder<VecSwizzle>	binder{ se };
			binder.CreateClassValue();

			binder.Comment( "Supported swizzle: 0XYZW-+, where '+' means +1 and '-' means -1." );
			binder.AddConstructor( &Ctor, {"swizzle"} );
		}
	};
}

	void  CoreBindings::BindVectorSwizzle (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		BindVecSwizzle_Func::Bind( se );
	}

/*
=================================================
	BindVectorMath
=================================================
*/
	void  CoreBindings::BindVectorMath (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		// separated on 3 parts because otherwise VS requires /bigobj
		_BindVectorMath1( se );
		_BindVectorMath2( se );
		_BindVectorMath3( se );
	}


} // AE::Scripting
