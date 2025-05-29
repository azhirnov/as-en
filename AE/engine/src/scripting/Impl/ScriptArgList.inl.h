// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Scripting
{

/*
=================================================
	Arg
=================================================
*/
	template <typename B>
	exact_t  ScriptArgList::Arg (const uint idx) C_Th___
	{
		using namespace AngelScript;

		using T = RemoveAllQualifiers< B >;
		
		CHECK_THROW( idx < ArgCount() );
		CHECK_THROW( IsArg<B>( idx ));

		if constexpr( (IsInteger<T> or IsFloatPoint<T>) and IsPointer<B> )
			return static_cast<B>( _gen->GetAddressOfArg( idx ));
		else
		if constexpr( (IsInteger<T> or IsFloatPoint<T>) and IsLValueRef<B> )
			return *static_cast<T*>( _gen->GetAddressOfArg( idx ));
		else

		if constexpr( IsSame< T, ubyte > or IsSame< T, sbyte > or IsSame< T, bool >)
			return T(_gen->GetArgByte( idx ));
		else
		if constexpr( IsSame< T, ushort > or IsSame< T, sshort >)
			return T(_gen->GetArgWord( idx ));
		else
		if constexpr( IsSame< T, uint > or IsSame< T, sint > or IsEnum<T> )
			return T(_gen->GetArgDWord( idx ));
		else
		if constexpr( IsSame< T, ulong > or IsSame< T, slong >)
			return T(_gen->GetArgQWord( idx ));
		else
		if constexpr( IsSame< T, float >)
			return T(_gen->GetArgFloat( idx ));
		else
		if constexpr( IsSame< T, double >)
			return T(_gen->GetArgDouble( idx ));
		else
		{
			StaticAssert( IsCompleteType< ScriptTypeInfo<T> >);

			constexpr bool	is_obj	= ScriptTypeInfo<T>::is_object;
			constexpr bool	is_rc	= ScriptTypeInfo<T>::is_ref_counted		or
									  ScriptTypeInfo<T*>::is_ref_counted	or
									  AngelScriptHelper::IsSharedPtrNoQual<T>;

			using CT2 = Conditional< IsAnyConst<B>, const T, T >;

			if constexpr( IsSame< T, String >)
			{
				// AS doesn't correctly handle alignment, so all checks are disabled
				return *static_cast<CT2 *>( static_cast<void *>( _gen->GetArgAddress( idx )));
			}
			else
			if constexpr( is_obj and not is_rc )
			{
				CT2*  arg = static_cast<CT2*>(_gen->GetArgObject( idx ));
				CHECK_THROW( arg != null );
				if constexpr( IsPointer<B> )	return arg;
				else							return *arg;
			}
			else
			if constexpr( is_rc )
			{
				if constexpr( AngelScriptHelper::IsSharedPtrNoQual<T> )
				{
					using T3 = typename AngelScriptHelper::RemoveSharedPtr<T>;
					T3*  arg = *static_cast<T3 **>(_gen->GetArgAddress( idx ));
					CHECK_THROW( arg == null or arg->__Counter() > 0 );
					return T{arg};
				}
				else
				{
					T*  arg = Cast<T>(_gen->GetArgObject( idx ));
					CHECK_THROW( arg != null );
					if constexpr( IsPointer<B> )	return arg;
					else							return *arg;
				}
			}
			else
			{
				StaticAssert( IsClass<T> );
				CT2*  arg = Cast<CT2>(_gen->GetArgAddress( idx ));
				CHECK_THROW( arg != null );
				if constexpr( IsPointer<B> )	return arg;
				else							return *arg;
			}
		}
	}

/*
=================================================
	IsArg
=================================================
*/
	template <typename T>
	bool  ScriptArgList::IsArg (const uint idx) C_NE___
	{
		using namespace AngelScript;

		if ( idx >= ArgCount() )
			return false;

		asDWORD		flags	= 0;
		const int	tid		= _gen->GetArgTypeId( idx, OUT &flags );

		return Scripting::_hidden_::CheckArgType<T>( tid, flags, _gen->GetEngine() );
	}

/*
=================================================
	ArgCount
=================================================
*/
	inline uint  ScriptArgList::ArgCount () C_NE___
	{
		return _gen->GetArgCount();
	}

/*
=================================================
	Return
=================================================
*/
	template <typename T>
	void  ScriptArgList::Return (const T &value) C_Th___
	{
		CHECK_THROW( IsReturn<T>() );
		AS_CHECK_THROW( _Return( value ));
	}

	template <typename T>
	int  ScriptArgList::_Return (const T &value) C_NE___
	{
		using namespace AngelScript;

		if constexpr( IsSame< T, bool > or IsSame< T, ubyte > or IsSame< T, sbyte > or IsSame< T, char >)
			return _gen->SetReturnByte( asBYTE(value) );
		else
		if constexpr( IsSame< T, ushort > or IsSame< T, sshort >)
			return _gen->SetReturnWord( asWORD(value) );
		else
		if constexpr( IsSame< T, uint > or IsSame< T, sint > or IsEnum<T> )
			return _gen->SetReturnDWord( asDWORD(value) );
		else
		if constexpr( IsSame< T, ulong > or IsSame< T, slong >)
			return _gen->SetReturnQWord( asQWORD(value) );
		else
		if constexpr( IsSame< T, float >)
			return _gen->SetReturnFloat( value );
		else
		if constexpr( IsSame< T, double >)
			return _gen->SetReturnDouble( value );
		else
		if constexpr( IsPointer<T> )
			return _gen->SetReturnAddress( BitCast<void*>( const_cast<T>( value )));
		else
		if constexpr( IsCompleteType< ScriptTypeInfo<T> >)
		{
			if constexpr( ScriptTypeInfo<T>::is_object )
				return _gen->SetReturnObject( const_cast<T*>(&value) );
			else{
				*Cast<T>(_gen->GetAddressOfReturnLocation()) = value;
				return 0;
			}
		}else
		{
			*Cast<T>(_gen->GetAddressOfReturnLocation()) = value;
			return 0;
		}
	}

/*
=================================================
	IsReturn
=================================================
*/
	template <typename T>
	bool  ScriptArgList::IsReturn () C_NE___
	{
		using namespace AngelScript;

		asDWORD		flags	= asTM_NONE;
		const int	tid		= _gen->GetReturnTypeId( OUT &flags );

		return Scripting::_hidden_::CheckArgType<T>( tid, flags, _gen->GetEngine() );
	}

/*
=================================================
	IsObject
=================================================
*/
	template <typename T>
	bool  ScriptArgList::IsObject () C_NE___
	{
		using namespace AngelScript;
		using Info = ScriptTypeInfo<T>;

		constexpr bool	is_obj	= Info::is_object;
		constexpr bool	is_rc	= Info::is_ref_counted or AngelScriptHelper::IsSharedPtrNoQual<T>;
		StaticAssert( is_obj or is_rc );

		const asDWORD	obj_flags	= is_rc ? asOBJ_REF : asOBJ_VALUE;
		asITypeInfo*	info		= _gen->GetEngine()->GetTypeInfoById( _gen->GetObjectTypeId() );

		if ( info == null )
			return false;

		StringView	name1	= info->GetName();
		String		name2;	Info::Name( OUT name2 );

		if ( name1 != name2 )
			return false;

		return AllBits( info->GetFlags(), obj_flags );
	}

/*
=================================================
	GetObject
=================================================
*/
	template <typename T>
	T*  ScriptArgList::GetObject () C_NE___
	{
		//ASSERT( IsObject<T>() );
		return static_cast<T*>( _gen->GetObject() );
	}

/*
=================================================
	Is
=================================================
*/
	template <typename Fn>
	bool  ScriptArgList::Is () C_NE___
	{
		using FnInfo = FunctionInfo< Fn >;

		if constexpr( IsVoid< typename FnInfo::clazz >)
		{
			if ( not IsGlobal() )
				return false;
		}
		else
		{
			if ( not IsObject< typename FnInfo::clazz >() )
				return false;
		}

		Visitor		vis {*this};
		FnInfo::args::Visit( vis );

		return IsReturn< typename FnInfo::result >() and vis.result;
	}

/*
=================================================
	GetArgTypename
=================================================
*/
	inline StringView  ScriptArgList::GetArgTypename (uint argIndex) C_NE___
	{
		using namespace AngelScript;

		asDWORD				flags	= 0;
		const int			tid		= _gen->GetArgTypeId( argIndex, OUT &flags );
		asIScriptEngine*	se		= _gen->GetEngine();
		asITypeInfo*		info	= se->GetTypeInfoById( tid );

		if ( info == null )
			return Default;

		return info->GetName();
	}


} // AE::Scripting
