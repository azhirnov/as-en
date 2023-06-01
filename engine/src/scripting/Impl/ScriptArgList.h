// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptTypes.h"

namespace AE::Scripting
{

	//
	// Argument List
	//

	class ScriptArgList
	{
	// types
	private:
		struct Visitor
		{
			ScriptArgList const&	self;
			bool					result	= true;

			Visitor (ScriptArgList const& s) : self{s} {}

			template <typename T, usize I>
			void  operator () () {
				result &= self.IsArg<T>( uint(I) );
			}
		};


	// variables
	private:
		Ptr<AngelScript::asIScriptGeneric>	_gen;


	// methods
	public:
		ScriptArgList () {}


	// arguments //
		template <typename T>
		ND_ decltype(auto)  Arg (uint argIndex)				C_NE___;

		template <typename T>
		ND_ bool			IsArg (uint argIndex)			C_NE___;
								
		ND_ uint			ArgCount ()						C_NE___;


	// return //
		template <typename T>
			void			Return (const T &value)			C_Th___;

		template <typename T>
		ND_ bool			IsReturn ()						C_NE___;


	// info //
		template <typename Fn>
		ND_ bool			Is ()							C_NE___;

		ND_ bool			IsGlobal ()						C_NE___	{ return _gen->GetObject() == null; }
		ND_ bool			IsMethod ()						C_NE___	{ return _gen->GetObject() != null; }

		template <typename Ret, typename ...Args>
		ND_ bool			Is (Ret (*)(Args...))			C_NE___	{ return Is< Ret (*)(Args...) >(); }

		template <typename C, typename Ret, typename ...Args>
		ND_ bool			Is (Ret (C::*)(Args...))		C_NE___	{ return Is< Ret (C::*)(Args...) >(); }


	// self object (caller) //
		template <typename T>
		ND_ bool			IsObject ()						C_NE___;
		
		template <typename T>
		ND_ T*				GetObject ()					C_NE___;


	private:
		template <typename T>
		ND_	int				_Return (const T &value)		C_NE___;
	};

	STATIC_ASSERT( sizeof(ScriptArgList) == sizeof(AngelScript::asIScriptGeneric*) );

} // AE::Scripting

#include "scripting/Impl/ScriptArgList.inl.h"
