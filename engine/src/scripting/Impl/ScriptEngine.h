// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	http://www.angelcode.com/angelscript/
	http://www.angelcode.com/angelscript/sdk/docs/manual/index.html
*/

#pragma once

#ifndef AE_ENABLE_ANGELSCRIPT
#	error AngelScript is not available
#endif

// AngelScript + Addons //
#include "base/Defines/StdInclude.h"
#include "angelscript.h"

#include "base/Containers/Ptr.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/NtStringView.h"
#include "base/CompileTime/TypeList.h"
#include "base/CompileTime/FunctionInfo.h"
#include "base/Math/BitMath.h"
#include "base/Utils/RefCounter.h"
#include "base/Utils/SourceLoc.h"
#include "base/Utils/FileSystem.h"

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
			bool		usePreprocessor	= false;

			ModuleSource () {}

			ModuleSource (StringView name, StringView script, const SourceLoc &loc = Default, Bool preprocess = False{}) :
				name{name}, script{script}, dbgLocation{loc}, usePreprocessor{preprocess} {}

			ModuleSource (String name, String script, const SourceLoc &loc = Default, Bool preprocess = False{}) :
				name{RVRef(name)}, script{RVRef(script)}, dbgLocation{loc}, usePreprocessor{preprocess} {}
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
			ND_ bool  LogError (StringView fnEntry, StringView section, int line, int column, StringView exceptionMsg) const;
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
		using CppHeaderMap_t	= FlatHashMap< String, Pair<usize, int> >;		// index in '_cppHeaders'
		

	// variables
	private:
		Ptr< AngelScript::asIScriptEngine >		_engine;
		std::atomic<usize>						_moduleIndex	{0};
		
		// Generate C++ header to use autocomplete in IDE for scripts
		std::mutex					_cppHeaderGuard;
		CppHeaderMap_t				_cppHeaderMap;
		Array<String>				_cppHeaders;
		bool						_genCppHeader	= false;

		#if AE_DBG_SCRIPTS
			std::recursive_mutex	_dbgLocationGuard;
			DbgLocationMap_t		_dbgLocation;
		#endif


	// methods
	public:
		ScriptEngine ();
		~ScriptEngine ();

		ND_ AngelScript::asIScriptEngine *			Get ()			__NE___	{ return _engine.operator->(); }
		ND_ AngelScript::asIScriptEngine const *	Get ()			C_NE___	{ return _engine.operator->(); }

		ND_ AngelScript::asIScriptEngine *			operator -> ()	__NE___	{ return _engine.operator->(); }
		ND_ AngelScript::asIScriptEngine const *	operator -> ()	C_NE___	{ return _engine.operator->(); }

		ND_ bool  IsInitialized ()									C_NE___	{ return bool{_engine}; }
		ND_ bool  IsUsingCppHeader ()								C_NE___	{ return _genCppHeader; }

		ND_ bool  Create (Bool genCppHeader = False{});
		ND_ bool  Create (AngelScript::asIScriptEngine *se, Bool genCppHeader = False{});

		ND_ ScriptModulePtr  CreateModule (ArrayView<ModuleSource> src, ArrayView<StringView> defines = Default);
		
		template <typename Fn>
		ND_ ScriptFnPtr<Fn>  CreateScript (StringView entry, const ScriptModulePtr &module);

		template <typename T>
		void  AddFunction (T func, StringView name)					__Th___;

		//template <typename T>
		//void  AddFunctionTemplate (T func, StringView name);

		template <typename T>
		void  AddProperty (INOUT T &var, StringView name)			__Th___;
		
		template <typename T>
		void  AddConstProperty (const T &var, StringView name)		__Th___;

		bool  SetNamespace (NtStringView name);
		bool  SetDefaultNamespace ();

			void  AddCppHeader (StringView typeName, String str, int flags);
			void  GetCppHeader (OUT String &str, OUT HashVal32 &hash);
		ND_	bool  SaveCppHeader (const Path &fname);


	// utils //
		ND_ static bool  _CheckError (int err, StringView asFunc, StringView func, const SourceLoc &loc);

		ND_ static bool  _Preprocessor (StringView, OUT String &, ArrayView<StringView> defines);

	private:
		ND_ bool  _CreateContext (const String &signature, const ScriptModulePtr &module, OUT AngelScript::asIScriptContext* &ctx);

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


	enum class AngelScriptException {};


# ifdef AE_RELEASE
#	define AS_CHECK( /* expr */... )	{AE::Base::Unused( __VA_ARGS__ );}

# else
#	define AS_CHECK( /* expr */... )																															\
	{																																							\
		int __as_result = ( __VA_ARGS__ );																														\
		AE::Base::Unused( AE::Scripting::ScriptEngine::_CheckError( __as_result, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, SourceLoc_Current() ));			\
	}
# endif

	
#	define AS_CHECK_ERR( /* expr */... )																														\
	{																																							\
		int __as_result = ( __VA_ARGS__ );																														\
		if_unlikely( not AE::Scripting::ScriptEngine::_CheckError( __as_result, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, SourceLoc_Current() ))			\
			return Default;																																		\
	}

#	define AS_CHECK_THROW( /*expr*/... )																														\
	{																																							\
		int __as_result = ( __VA_ARGS__ );																														\
		AE_PRIVATE_CHECK_THROW( (AE::Scripting::ScriptEngine::_CheckError( __as_result, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, SourceLoc_Current() )),	\
								AE::Scripting::AngelScriptException(0) );																						\
	}
	

} // AE::Scripting
