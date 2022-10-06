// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"
#include "scripting/Impl/ScriptTypes.h"
#include "base/Algorithms/StringUtils.h"

namespace AE::Scripting
{
	template <typename F>
	class ScriptFn;


	//
	// Script Function
	//

	template <typename R, typename ...Types>
	class ScriptFn< R (Types...) > final : public EnableRC< ScriptFn<R (Types...)> >
	{
		friend class ScriptEngine;

	// types
	private:
		using Result_t	= Conditional< IsSameTypes<R, void>, bool, Optional<R> >;
		using Self		= ScriptFn< R (Types...) >;


	// variables
	private:
		ScriptModulePtr					_module;
		AngelScript::asIScriptContext*	_ctx		= null;


	// methods
	private:
		ScriptFn (const ScriptModulePtr &mod, AngelScript::asIScriptContext* ctx) :
			_module{ mod }, _ctx{ ctx }
		{}

	public:
		~ScriptFn () override
		{
			if ( _ctx != null )
				_ctx->Release();
		}

		template <typename ...Args>
		ND_ Result_t  Run (Args&& ...args);

	private:
		bool  _CheckError (int exec_res) const;
	};


}	// AE::Scripting
