// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
//
// see http://www.angelcode.com/angelscript/
//
// online reference http://www.angelcode.com/angelscript/sdk/docs/manual/index.html
//

#pragma once

#ifndef AE_ENABLE_ANGELSCRIPT
#	error AngelScript is not available
#endif

// AngelScript + Addons //
#include "angelscript.h"
#include <atomic>
#include <mutex>

#include "base/Containers/Ptr.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/NtStringView.h"
#include "base/CompileTime/TypeList.h"
#include "base/CompileTime/FunctionInfo.h"
#include "base/Math/BitMath.h"
#include "base/Utils/RefCounter.h"
#include "base/Utils/SourceLoc.h"

#ifndef AE_DBG_SCRIPTS
# ifdef AE_DEBUG
#	define AE_DBG_SCRIPTS	1
# else
#	define AE_DBG_SCRIPTS	0
# endif
#endif

namespace AE::Scripting
{
	using namespace AE::Base;

	using ScriptModulePtr = RC< class ScriptModule >;
	using ScriptEnginePtr = RC< class ScriptEngine >;
	
	template <typename Fn>
	class ScriptFn;

	template <typename Fn>
	using ScriptFnPtr = RC< ScriptFn<Fn> >;
	


	//
	// Script Module
	//

	class ScriptModule final : public EnableRC<ScriptModule>
	{
		friend class ScriptEngine;
		
	// types
	public:
		struct ModuleSource
		{
			String		name;
			String		script;
			SourceLoc	dbgLocation;

			ModuleSource () {}

			ModuleSource (NtStringView name, NtStringView script, const SourceLoc &loc = Default) :
				name{name}, script{script}, dbgLocation{loc} {}

			ModuleSource (String name, String script, const SourceLoc &loc = Default) :
				name{RVRef(name)}, script{RVRef(script)}, dbgLocation{loc} {}
		};

	private:
		using DbgLocationMap_t = FlatHashMap< /*section*/String, SourceLoc2 >;


	// variables
	private:
		AngelScript::asIScriptModule*	_module;
		
		#if AE_DBG_SCRIPTS
			mutable std::recursive_mutex	_dbgLocationGuard;
			DbgLocationMap_t				_dbgLocation;
		#endif


	// methods
	private:
		ScriptModule (AngelScript::asIScriptModule* mod, ArrayView<ModuleSource> dbgSrc);

	public:
		~ScriptModule () override;
		
		#if AE_DBG_SCRIPTS
			void  LogError (StringView fnEntry, StringView section, int line, int column, StringView exceptionMsg) const;
		#endif
	};



	//
	// Script Engine
	//

	class ScriptEngine final : public EnableRC<ScriptEngine>
	{
	// types
	public:
		using ModuleSource		= ScriptModule::ModuleSource;
	private:
		using DbgLocationMap_t	= ScriptModule::DbgLocationMap_t;


	// variables
	private:
		Ptr< AngelScript::asIScriptEngine >		_engine;
		std::atomic<usize>						_moduleIndex	{0};
		
		#if AE_DBG_SCRIPTS
			std::recursive_mutex	_dbgLocationGuard;
			DbgLocationMap_t		_dbgLocation;
		#endif


	// methods
	public:
		ScriptEngine ();
		~ScriptEngine ();
		
		ScriptEngine (const ScriptEngine &) = delete;
		ScriptEngine (ScriptEngine &&) = delete;
		ScriptEngine& operator = (const ScriptEngine &) = delete;
		ScriptEngine& operator = (ScriptEngine &&) = delete;

		ND_ AngelScript::asIScriptEngine *			Get ()					{ return _engine.operator->(); }
		ND_ AngelScript::asIScriptEngine const *	Get ()	const			{ return _engine.operator->(); }

		ND_ AngelScript::asIScriptEngine *			operator -> ()			{ return _engine.operator->(); }
		ND_ AngelScript::asIScriptEngine const *	operator -> () const	{ return _engine.operator->(); }

		bool  Create ();
		bool  Create (AngelScript::asIScriptEngine *se);

		ND_ ScriptModulePtr  CreateModule (ArrayView<ModuleSource> src);
		
		template <typename Fn>
		ND_ ScriptFnPtr<Fn>  CreateScript (StringView entry, const ScriptModulePtr &module);

		template <typename T>
		bool  AddFunction (T func, StringView name);

		//template <typename T>
		//void  AddFunctionTemplate (T func, StringView name);

		template <typename T>
		bool  AddProperty (INOUT T &var, StringView name);
		
		template <typename T>
		bool  AddConstProperty (const T &var, StringView name);

		bool  SetNamespace (NtStringView name);
		bool  SetDefaultNamespace ();


	// utils //
		static bool  _CheckError (int err, StringView asFunc, StringView func, StringView file, int line);

	private:
		bool  _CreateContext (const String &signature, const ScriptModulePtr &module, OUT AngelScript::asIScriptContext* &ctx);

		static void  _MessageCallback (const AngelScript::asSMessageInfo *msg, void *param);
	};



	//
	// Script Engine Multithreading Scope
	//
	struct ScriptEngineMultithreadingScope
	{
		ScriptEngineMultithreadingScope ();
		~ScriptEngineMultithreadingScope ();
	};
	

	//
	// Script Thread Scope
	//
	struct ScriptThreadScope
	{
		ScriptThreadScope () {}
		~ScriptThreadScope ();
	};


#	define AS_CALL( /* expr */... ) \
	{ \
		int __as_result = ( __VA_ARGS__ ); \
		::AE::Scripting::ScriptEngine::_CheckError( __as_result, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, __FILE__, __LINE__ ); \
	}
	
#	define AS_CALL_R( /* expr */... ) \
	{ \
		int __as_result = ( __VA_ARGS__ ); \
		if ( not ::AE::Scripting::ScriptEngine::_CheckError( __as_result, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, __FILE__, __LINE__ )) \
			return Default; \
	}
	

} // AE::Scripting
