// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
				Fatal( "" );
		}

		static void  Assert2 (bool expr, const String &msg) __Th___ {
			if ( not expr )
				Fatal( msg );
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

		se->AddFunction( &LogFunc::Err,		"LogError",		{"msg"} );
		se->AddFunction( &LogFunc::Info,	"LogInfo",		{"msg"} );
		se->AddFunction( &LogFunc::Dbg,		"LogDebug",		{"msg"} );
		se->AddFunction( &LogFunc::Fatal,	"LogFatal",		{"msg"} );
		se->AddFunction( &LogFunc::Assert1,	"Assert",		{"expr"} );
		se->AddFunction( &LogFunc::Assert2,	"Assert",		{"expr", "msg"} );
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
			VecSwizzle	temp = Math::operator ""_vecSwizzle( str.c_str(), str.length() );

			PlacementNew<VecSwizzle>( OUT mem, temp );
		}

		static void  Bind (const ScriptEnginePtr &se)
		{
			ClassBinder<VecSwizzle>	binder{ se };
			binder.CreateClassValue();

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

		// separated on 2 parts because otherwise VS requires /bigobj
		_BindVectorMath1( se );
		_BindVectorMath2( se );
	}


} // AE::Scripting
