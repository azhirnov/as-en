// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ClassBinder.h"

namespace AE::Scripting
{

	//
	// Interface Binder
	//
	template <typename T>
	class InterfaceBinder final
	{
	// types
	public:
		using Self		= InterfaceBinder<T>;
		using Class_t	= T;


	// variables
	private:
		ScriptEnginePtr		_engine;
		String				_name;
		
		const bool			_genHeader	= false;
		String				_header;


	// methods
	public:
		explicit InterfaceBinder (const ScriptEnginePtr &eng)			__NE___;
		InterfaceBinder (const ScriptEnginePtr &eng, StringView name)	__NE___;
		~InterfaceBinder ()												__NE___;

		void  Create ()													__Th___;
		
		template <typename Func>
		void  AddMethod (Func methodPtr, StringView name)				__Th___;

		ND_ StringView							Name ()					C_NE___	{ return _name; }
		ND_ Ptr< AngelScript::asIScriptEngine >	GetASEngine ()			__NE___	{ return _engine->Get(); }
	};

	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	InterfaceBinder<T>::InterfaceBinder (const ScriptEnginePtr &eng) __NE___ :
		_engine{ eng }, _genHeader{ eng->IsUsingCppHeader() }
	{
		ScriptTypeInfo< T >::Name( INOUT _name );
	}
	
	template <typename T>
	InterfaceBinder<T>::InterfaceBinder (const ScriptEnginePtr &eng, StringView name) __NE___
		_engine{ eng }, _name{ name }, _genHeader{ eng->IsUsingCppHeader() }
	{
		CHECK_THROW( not _name.empty() );
	}
	
	template <typename T>
	InterfaceBinder<T>::~InterfaceBinder () __NE___
	{
		if_unlikely( _genHeader )
			_engine->AddCppHeader( _name, RVRef(_header), _flags );
	}
	
/*
=================================================
	Create
=================================================
*/
	template <typename T>
	void  InterfaceBinder<T>::Create () __Th___
	{
		AS_CHECK_THROW( GetASEngine()->RegisterInterface( _name.c_str() ));
	}
		
/*
=================================================
	AddMethod
=================================================
*/
	template <typename T>
	template <typename Func>
	void  InterfaceBinder<T>::AddMethod (Func methodPtr, StringView name) __Th___
	{
		using C = typename FunctionInfo<Func>::clazz;
		STATIC_ASSERT( IsBaseOf< C, T >);

		String	signature;
		MemberFunction<Func>::GetDescriptor( INOUT signature, name );

		AS_CHECK_THROW( GetASEngine()->RegisterInterfaceMethod( _name.c_str(), signature.c_str() ));
	}


} // AE::Scripting
