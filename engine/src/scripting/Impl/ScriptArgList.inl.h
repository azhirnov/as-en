// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Scripting
{

/*
=================================================
    Arg
=================================================
*/
    template <typename T>
    decltype(auto)  ScriptArgList::Arg (uint idx) C_NE___
    {
        using namespace AngelScript;

        ASSERT( IsArg<T>( idx ));

        if constexpr( IsSameTypes< T, ubyte > or IsSameTypes< T, sbyte >)
            return T(_gen->GetArgByte( idx ));
        else
        if constexpr( IsSameTypes< T, ushort > or IsSameTypes< T, sshort >)
            return T(_gen->GetArgWord( idx ));
        else
        if constexpr( IsSameTypes< T, uint > or IsSameTypes< T, sint > or IsEnum<T> )
            return T(_gen->GetArgDWord( idx ));
        else
        if constexpr( IsSameTypes< T, ulong > or IsSameTypes< T, slong >)
            return T(_gen->GetArgQWord( idx ));
        else
        if constexpr( IsSameTypes< T, float >)
            return T(_gen->GetArgFloat( idx ));
        else
        if constexpr( IsSameTypes< T, double >)
            return T(_gen->GetArgDouble( idx ));
        else
        {
            using T2 = RemoveAllQualifiers<T>;

            constexpr bool  is_obj  = ScriptTypeInfo<T2>::is_object;
            constexpr bool  is_rc   = ScriptTypeInfo<T2>::is_ref_counted    or
                                      ScriptTypeInfo<T2*>::is_ref_counted   or
                                      AngelScriptHelper::IsSharedPtrNoQual<T2>;

            using CT2 = Conditional< IsAnyConst<T>, const T2, T2 >;

            if constexpr( IsSameTypes< T2, String >)
                return *Cast<CT2>(_gen->GetArgAddress( idx ));
            else
            if constexpr( is_obj and not is_rc )
            {
                CT2*  arg = Cast<CT2>(_gen->GetArgObject( idx ));
                return *arg;
            }
            else
            if constexpr( is_rc )
            {
                if constexpr( AngelScriptHelper::IsSharedPtrNoQual<T2> )
                {
                    using T3 = typename AngelScriptHelper::RemoveSharedPtr<T2>;
                    T3*  arg = *static_cast<T3 **>(_gen->GetArgAddress( idx ));
                    ASSERT( arg == null or arg->__Counter() > 0 );
                    return T2{arg};
                }
                else
                {
                    STATIC_ASSERT( IsClass<T2> );
                    T2* arg = *static_cast<T2 **>(_gen->GetArgAddress( idx ));
                    ASSERT( arg == null or arg->__Counter() > 0 );
                    return arg;
                }
            }
            else
            {
                STATIC_ASSERT( IsClass<T2> );
                return *Cast<CT2>(_gen->GetArgAddress( idx ));
            }
        }
    }

/*
=================================================
    IsArg
=================================================
*/
    template <typename T>
    bool  ScriptArgList::IsArg (uint idx) C_NE___
    {
        using namespace AngelScript;

        asDWORD     flags   = 0;
        const int   tid     = _gen->GetArgTypeId( idx, OUT &flags );

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
        AS_CHECK_THROW( _Return( value ));
    }

    template <typename T>
    int  ScriptArgList::_Return (const T &value) C_NE___
    {
        using namespace AngelScript;

        ASSERT( IsReturn<T>() );

        if constexpr( IsSameTypes< T, ubyte > or IsSameTypes< T, sbyte > or IsSameTypes< T, char >)
            return _gen->SetReturnByte( asBYTE(value) );
        else
        if constexpr( IsSameTypes< T, ushort > or IsSameTypes< T, sshort >)
            return _gen->SetReturnWord( asWORD(value) );
        else
        if constexpr( IsSameTypes< T, uint > or IsSameTypes< T, sint > or IsEnum<T> )
            return _gen->SetReturnDWord( asDWORD(value) );
        else
        if constexpr( IsSameTypes< T, ulong > or IsSameTypes< T, slong >)
            return _gen->SetReturnQWord( asQWORD(value) );
        else
        if constexpr( IsSameTypes< T, float >)
            return _gen->SetReturnFloat( value );
        else
        if constexpr( IsSameTypes< T, double >)
            return _gen->SetReturnDouble( value );
        else
        if constexpr( IsPointer<T> )
            return _gen->SetReturnAddress( const_cast<T*>(&value) );
        else
        {
            STATIC_ASSERT( ScriptTypeInfo<T>::is_object );
            return _gen->SetReturnObject( const_cast<T*>(&value) );
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

        asDWORD     flags   = asTM_NONE;
        const int   tid     = _gen->GetReturnTypeId( OUT &flags );

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

        constexpr bool  is_obj  = Info::is_object;
        constexpr bool  is_rc   = Info::is_ref_counted or AngelScriptHelper::IsSharedPtrNoQual<T>;
        STATIC_ASSERT( is_obj or is_rc );

        const asDWORD   obj_flags   = is_rc ? asOBJ_REF : asOBJ_VALUE;
        asITypeInfo*    info        = _gen->GetEngine()->GetTypeInfoById( _gen->GetObjectTypeId() );

        if ( info == null )
            return false;

        StringView  name1   = info->GetName();
        String      name2;  Info::Name( OUT name2 );

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
        return Cast<T>( _gen->GetObject() );
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

        Visitor     vis {*this};
        FnInfo::args::Visit( vis );

        return  IsReturn< typename FnInfo::result >() and vis.result;
    }

/*
=================================================
    GetArgTypename
=================================================
*/
    inline StringView  ScriptArgList::GetArgTypename (uint argIndex) C_NE___
    {
        using namespace AngelScript;

        asDWORD             flags   = 0;
        const int           tid     = _gen->GetArgTypeId( argIndex, OUT &flags );
        asIScriptEngine*    se      = _gen->GetEngine();
        asITypeInfo*        info    = se->GetTypeInfoById( tid );

        if ( info == null )
            return Default;

        return info->GetName();
    }


} // AE::Scripting
