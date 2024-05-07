// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"
#include "scripting/Impl/ScriptTypes.h"

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
		ScriptFn (const ScriptModulePtr &mod, AngelScript::asIScriptContext* ctx) __NE___ :
			_module{ mod }, _ctx{ ctx }
		{}

	public:
		~ScriptFn ()						__NE_OV
		{
			if ( _ctx != null )
				_ctx->Release();
		}

		template <typename ...Args>
		ND_ Result_t  Run (Args&& ...args)	__NE___;

	private:
		bool  _CheckError (int exec_res) const;
	};



/*
=================================================
	Run
=================================================
*/
	template <typename R, typename ...Types>
	template <typename ...Args>
	typename ScriptFn< R (Types...) >::Result_t
		ScriptFn< R (Types...) >::Run (Args&& ...args) __NE___
	{
		using namespace AngelScript;

		using ExpectedArgs_t	= TypeList< Types... >;
		using InputArgs_t		= TypeList< Args... >;

		StaticAssert( Scripting::_hidden_::CheckInputArgTypes< ExpectedArgs_t, InputArgs_t >::value );

		if_unlikely( not (_module and _ctx != null) )
		{
			if constexpr( IsSameTypes<R, void> ) {
				RETURN_ERR( "not initialized", false );
			}else{
				RETURN_ERR( "not initialized", Optional<R>{} );
			}
		}

		Scripting::_hidden_::SetContextArgs<Args...>::Set( _ctx, 0, FwdArg<Args>(args)... );

		const int	exec_res = _ctx->Execute();
		// result same as _ctx->GetState();

		if constexpr( IsSameTypes<R, void> )
		{
			if_likely( exec_res == asEXECUTION_FINISHED )
				return true;

			return _CheckError( exec_res );
		}
		else
		{
			if_likely( exec_res == asEXECUTION_FINISHED )
				return {Scripting::_hidden_::ContextSetterGetter<R>::Get( _ctx )};

			_CheckError( exec_res );
			return {};
		}
	}

/*
=================================================
	_CheckError
=================================================
*/
	template <typename R, typename ...Types>
	bool  ScriptFn< R (Types...) >::_CheckError (int exec_res) const
	{
		using namespace AngelScript;

		if ( exec_res == asEXECUTION_EXCEPTION )
		{
			String	err;
			err	<< "Exception in function: "
				<< _ctx->GetExceptionFunction()->GetName();

			const char*	section	= 0;
			int			column	= 0;
			const int	line	= _ctx->GetExceptionLineNumber( OUT &column, OUT &section );

			err << ", in script " << section << " (" << ToString( line ) << ", " << ToString( column ) << "):\n";
			err << _ctx->GetExceptionString();

			#if AE_DBG_SCRIPTS
			if ( not _module->LogError( _ctx->GetExceptionFunction()->GetName(), section, line, column, _ctx->GetExceptionString() ))
			#endif
				AE_LOGW( err );

			return false;
		}
		else
		{
			RETURN_ERR( "AngelScript execution failed" );
		}
	}


} // AE::Scripting
