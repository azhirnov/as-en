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

#include "scripting/Scripting.pch.h"

#ifdef AE_DEBUG
# define AE_DBG_SCRIPTS		1
#else
# define AE_DBG_SCRIPTS		0
#endif

#ifdef AE_RELEASE
# define AE_SCRIPT_CPP_REFLECTION	0
#else
# define AE_SCRIPT_CPP_REFLECTION	1
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

	class ScriptArgList;



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
		Ptr<AngelScript::asIScriptModule>	_module;

		#if AE_DBG_SCRIPTS
			mutable RecursiveMutex			_dbgLocationGuard;
			DbgLocationMap_t				_dbgLocation;
		#endif


	// methods
	private:
		ScriptModule (AngelScript::asIScriptModule* mod, ArrayView<ModuleSource> dbgSrc);

	public:
		~ScriptModule ()							__NE_OV;

		ND_ StringView	GetName ()					C_NE___	{ return _module != null ? _module->GetName() : Default; }

		template <typename Fn>
		ND_ bool	HasFunction (StringView entry)	C_NE___;

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
		using ArgNames_t		= List<StringView>;
	private:
		using DbgLocationMap_t	= ScriptModule::DbgLocationMap_t;
		using CppHeaderMap_t	= FlatHashMap< String, Pair<usize, int> >;		// index in '_cppHeaders'


	// variables
	private:
		Ptr< AngelScript::asIScriptEngine >		_engine;
		Atomic<usize>							_moduleIndex	{0};

		// Generate C++ header to use autocomplete in IDE for scripts
		#if AE_SCRIPT_CPP_REFLECTION
			Mutex						_cppHeaderGuard;
			CppHeaderMap_t				_cppHeaderMap;
			Array<String>				_cppHeaders;
			bool						_genCppHeader	= false;
		#endif

		#if AE_DBG_SCRIPTS
			RecursiveMutex				_dbgLocationGuard;
			DbgLocationMap_t			_dbgLocation;
		#endif


	// methods
	public:
		ScriptEngine ()																				__NE___;
		~ScriptEngine ()																			__NE___;

		ND_ AngelScript::asIScriptEngine *			Get ()											__NE___	{ return _engine.operator->(); }
		ND_ AngelScript::asIScriptEngine const *	Get ()											C_NE___	{ return _engine.operator->(); }

		ND_ AngelScript::asIScriptEngine *			operator -> ()									__NE___	{ return _engine.operator->(); }
		ND_ AngelScript::asIScriptEngine const *	operator -> ()									C_NE___	{ return _engine.operator->(); }

		ND_ bool  IsInitialized ()																	C_NE___	{ return bool{_engine}; }

	  #if AE_SCRIPT_CPP_REFLECTION
		ND_ bool  IsUsingCppHeader ()																C_NE___	{ return _genCppHeader; }
	  #else
		ND_ bool  IsUsingCppHeader ()																C_NE___	{ return false; }
	  #endif

		ND_ bool  Create (Bool genCppHeader = False{})												__NE___;
		ND_ bool  Create (AngelScript::asIScriptEngine* se, Bool genCppHeader = False{})			__NE___;

		ND_ ScriptModulePtr  CreateModule (ArrayView<ModuleSource>	src,
										   ArrayView<StringView>	defines		= Default,
										   ArrayView<Path>			includeDirs	= Default)			__NE___;

		template <typename Fn>
		ND_ ScriptFnPtr<Fn>  CreateScript (StringView entry, const ScriptModulePtr &module)			__NE___;

		template <typename T>
		ND_ bool  IsRegistered ()																	__NE___;
		ND_ bool  IsRegistered (NtStringView name)													__NE___;


		template <typename T>
		void  AddFunction (T func, StringView name)													__Th___;

		template <typename T>
		void  AddFunction (T func, StringView name, ArgNames_t argNames, StringView comment = {})	__Th___;


		template <typename Fn>
		void  AddGenericFn (void (*fn)(ScriptArgList), StringView name)								__Th___;

		template <typename Fn>
		void  AddGenericFn (void (*fn)(ScriptArgList), StringView name,
							ArgNames_t argNames, StringView comment = {})							__Th___;


		template <typename T>
		void  AddProperty (INOUT T &var, StringView name)											__Th___;

		template <typename T>
		void  AddConstProperty (const T &var, StringView name)										__Th___;

		void  Typedef (NtStringView newType, NtStringView existType)								__Th___;


		bool  SetNamespace (NtStringView name)														__NE___;
		bool  SetDefaultNamespace ()																__NE___;


			void  AddCppHeader (String typeName, String str, int flags)								__Th___;
			void  GetCppHeader (OUT String &str, OUT HashVal32 &hash)								__Th___;
		ND_	bool  SaveCppHeader (const Path &fname)													__Th___;


	// utils //
		ND_ static bool  _CheckError (int err, StringView asFunc, StringView func, const SourceLoc &loc)__NE___;

		ND_ static bool  _Preprocessor2 (StringView str,
										 OUT String &,
										 ArrayView<StringView> defines,
										 ArrayView<Path> includeDirs)								__NE___;

	private:
		ND_ bool  _CreateContext (const String &signature, const ScriptModulePtr &module, OUT AngelScript::asIScriptContext* &ctx);

			static void  _MessageCallback (const AngelScript::asSMessageInfo* msg, void* param);

		ND_ static bool  _Preprocessor (StringView str,
										OUT String &,
										OUT Array<Pair< StringView, usize >> &,
										ArrayView<StringView> defines)								__Th___;
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



# ifdef AE_CFG_RELEASE
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
