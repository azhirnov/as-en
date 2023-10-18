// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Impl/ScriptTypes.h"
#include "scripting/Impl/ScriptArgList.h"

namespace AE::Scripting
{

/*
=================================================
    IsGlobal
=================================================
*/
namespace _hidden_ {
    template <typename Fn>
    constexpr bool  IsGlobal ()
    {
        return IsSameTypes< typename FunctionInfo<Fn>::clazz, void >;
    }
} // _hidden_


/*
=================================================
    constructor
=================================================
*/
    template <typename T>
    ClassBinder<T>::ClassBinder (const ScriptEnginePtr &eng) __NE___ :
        _engine{ eng }, _genHeader{ eng->IsUsingCppHeader() }
    {
        ScriptTypeInfo< T >::Name( INOUT _name );
    }

    template <typename T>
    ClassBinder<T>::ClassBinder (const ScriptEnginePtr &eng, StringView name) __Th___ :
        _engine{ eng }, _name{ name }, _genHeader{ eng->IsUsingCppHeader() }
    {
        CHECK_THROW( not _name.empty() );
    }

/*
=================================================
    destructor
=================================================
*/
    template <typename T>
    ClassBinder<T>::~ClassBinder () __NE___
    {
        if_unlikely( _genHeader )
            _engine->AddCppHeader( RVRef(_name), RVRef(_header), _flags );
    }

/*
=================================================
    Comment
=================================================
*/
    template <typename T>
    void  ClassBinder<T>::Comment (StringView text) __Th___
    {
        if_unlikely( _genHeader and not text.empty() )
        {
            _header << '\n';
            for (usize pos = 0; pos < text.size();)
            {
                StringView  line;
                Parser::ReadCurrLine( text, INOUT pos, OUT line );
                _header << "\t// " << line << '\n';
            }
        }
    }

/*
=================================================
    _IsSame
=================================================
*/
    template <typename T> template <typename T1>
    struct ClassBinder<T>::_IsSame {
        static constexpr bool   value = IsSameTypes< T *, T1 >          or
                                        IsSameTypes< T &, T1 >          or
                                        IsSameTypes< T const *, T1 >    or
                                        IsSameTypes< T const &, T1 >    or
                                        IsSameTypes< const AngelScriptHelper::SharedPtr<T> &, T1 >;
    };

/*
=================================================
    _Util
=================================================
*/
    struct _Util
    {
        template <typename TIn, typename TOut>
        static String  Desc (StringView name)
        {
            String signature;
            ScriptTypeInfo< TOut >::Name( INOUT signature );    signature << " " << name << "(";
            ScriptTypeInfo< TIn >::ArgName( INOUT signature );  signature << ") const";
            return signature;
        }
    };

/*
=================================================
    CreatePodValue
=================================================
*/
    template <typename T>
    void  ClassBinder<T>::CreatePodValue (int flags) __Th___
    {
        using namespace AngelScript;

        STATIC_ASSERT( alignof(T) <= 16 );

        _flags = asOBJ_VALUE | asOBJ_POD | flags;

        if constexpr( alignof(T) == 16 )
            _flags |= asOBJ_APP_ALIGN16;
        else
        if constexpr( alignof(T) == 8 )
            _flags |= asOBJ_APP_CLASS_ALIGN8;

        AS_CHECK_THROW( GetASEngine()->RegisterObjectType( _name.c_str(), sizeof(T), _flags ));

        _Create( _flags );  // throw
    }

/*
=================================================
    CreateClassValue
=================================================
*/
    template <typename T>
    void  ClassBinder<T>::CreateClassValue (int flags) __Th___
    {
        using namespace AngelScript;

        if ( AnyBits( flags, asOBJ_APP_CLASS_CDAK ))
            _flags = asOBJ_VALUE | flags;
        else
        {
            _flags = asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_APP_CLASS_DESTRUCTOR | flags;

            if constexpr( IsDefaultConstructible<T> )
                _flags |= asOBJ_APP_CLASS_CONSTRUCTOR;

            if constexpr( IsCopyConstructible<T> )
                _flags |= asOBJ_APP_CLASS_COPY_CONSTRUCTOR;

            if constexpr( IsCopyAssignable<T> or IsMoveAssignable<T> )
                _flags |= asOBJ_APP_CLASS_ASSIGNMENT;
        }

        if constexpr( alignof(T) == 8 )
            _flags |= asOBJ_APP_CLASS_ALIGN8;

        AS_CHECK_THROW( GetASEngine()->RegisterObjectType( _name.c_str(), sizeof(T), _flags ));

        _Create( _flags );  // throw
    }

/*
=================================================
    CreateRef
=================================================
*/
    template <typename T>
    void  ClassBinder<T>::CreateRef (int flags, const Bool hasFactory) __Th___
    {
        using constructor_t = T * (*) ();

        constructor_t create = null;

        if constexpr( IsAbstract<T> or not IsDefaultConstructible<T> ) {
            CHECK_THROW( not hasFactory );
        }else{
            if ( hasFactory )
                create = &AngelScriptHelper::FactoryCreateRC<T>;
        }

        return CreateRef( create, &T::__AddRef, &T::__Release, flags );
    }

/*
=================================================
    CreateRef
=================================================
*/
    template <typename T>
    void  ClassBinder<T>::CreateRef (T* (*create)(), void (T:: *addRef)(), void (T:: *releaseRef)(), int flags) __Th___
    {
        using namespace AngelScript;

        _flags = asOBJ_REF | flags | (addRef != null and releaseRef != null ? 0 : asOBJ_NOCOUNT);

        AS_CHECK_THROW( GetASEngine()->RegisterObjectType( _name.c_str(), sizeof(T), _flags ));

        if_unlikely( _genHeader )
            _header << "struct " << _name << "\n{\n";

        if ( addRef != null )
        {
            AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_ADDREF, "void AddRef()",
                                asSMethodPtr<sizeof(void (T::*)())>::Convert(static_cast<void (T::*)()>(addRef)), asCALL_THISCALL ));
        }

        if ( releaseRef != null )
        {
            AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_RELEASE, "void Release()",
                                asSMethodPtr<sizeof(void (T::*)())>::Convert(static_cast<void (T::*)()>(releaseRef)), asCALL_THISCALL ));
        }

        if ( create != null )
        {
            AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_FACTORY,
                                                        (_name + "@ new_" + _name + "()").c_str(),
                                                        asFUNCTION( create ), asCALL_CDECL ));

            if_unlikely( _genHeader )
                _header << '\t' << _name << " ();\n";
        }
    }

/*
=================================================
    _Create
=================================================
*/
    template <typename T>
    void  ClassBinder<T>::_Create (const int flags) __Th___
    {
        using namespace AngelScript;

        if_unlikely( _genHeader )
            _header << "struct " << _name << "\n{\n";

        // constructor
        if constexpr( IsDefaultConstructible<T> )
        {
            if ( AllBits( flags, asOBJ_APP_CLASS_CONSTRUCTOR ))
            {
                AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_CONSTRUCT, "void f()",
                                                asFUNCTION( &AngelScriptHelper::Constructor<T> ), asCALL_GENERIC ));

                if_unlikely( _genHeader )
                    _header << '\t' << _name << " ();\n";
            }
        }else{
            CHECK_THROW( not AllBits( flags, asOBJ_APP_CLASS_CONSTRUCTOR ));
        }

        // destructor
        if ( AllBits( flags, asOBJ_APP_CLASS_DESTRUCTOR ))
        {
            AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_DESTRUCT,  "void f()",
                                            asFUNCTION( &AngelScriptHelper::Destructor<T> ), asCALL_GENERIC ));
        }

        // copy constructor
        if constexpr( IsCopyConstructible<T> )
        {
            if ( AllBits( flags, asOBJ_APP_CLASS_COPY_CONSTRUCTOR ))
            {
                AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_CONSTRUCT,
                                                ("void f(const " + _name + " &in)").c_str(),
                                                asFUNCTION( &AngelScriptHelper::CopyConstructor<T> ), asCALL_GENERIC ));

                if_unlikely( _genHeader )
                    _header << '\t' << _name << " (const " << _name << "&);\n";
            }
        }else{
            CHECK_THROW( not AllBits( flags, asOBJ_APP_CLASS_COPY_CONSTRUCTOR ));
        }

        // assignment
        if constexpr( IsCopyAssignable<T> or IsMoveAssignable<T> )
        {
            if ( AllBits( flags, asOBJ_APP_CLASS_ASSIGNMENT ))
            {
                AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(),
                                                (_name + " & opAssign(const " + _name + " &in)").c_str(),
                                                asFUNCTION( &AngelScriptHelper::CopyAssign<T> ), asCALL_GENERIC ));

                if_unlikely( _genHeader )
                    _header << '\t' << _name << "&  operator = (const " << _name << "&);\n";
            }
        }else{
            CHECK_THROW( not AllBits( flags, asOBJ_APP_CLASS_ASSIGNMENT ));
        }
    }

/*
=================================================
    IsRegistred
=================================================
*/
    template <typename T>
    bool  ClassBinder<T>::IsRegistred () C_NE___
    {
        return _engine->IsRegistred( _name );
    }

/*
=================================================
    AddConstructor
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddConstructor (Fn ctorPtr, ArgNames_t argNames) __Th___
    {
        using namespace AngelScript;

        STATIC_ASSERT(( not IsBaseOf< AngelScriptHelper::SimpleRefCounter, T > ));
        STATIC_ASSERT(( IsSameTypes< void *, typename GlobalFunction<Fn>::TypeList_t::Front::type > ));

        String  signature("void f ");
        GlobalFunction<Fn>::GetArgs( INOUT signature, 1 );  // skip (void *)

        AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_CONSTRUCT,
                                        signature.c_str(), asFUNCTION( *ctorPtr ), asCALL_CDECL_OBJFIRST ));

        if_unlikely( _genHeader )
        {
            _header << '\t' << _name << ' ';
            GlobalFunction<Fn>::GetCppArgs( INOUT _header, argNames, 1 );   // skip (void *)
            _header << ";\n";
        }
    }

/*
=================================================
    AddFactoryCtor
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddFactoryCtor (Fn ctorPtr, ArgNames_t argNames) __Th___
    {
        using namespace AngelScript;

        STATIC_ASSERT(( IsBaseOf< AngelScriptHelper::SimpleRefCounter, T > ));
        STATIC_ASSERT(( IsSameTypes< T*, typename GlobalFunction<Fn>::Result_t > ));

        String  signature(_name + "@ new_" + _name);
        GlobalFunction<Fn>::GetArgs( INOUT signature );

        AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_FACTORY,
                                        signature.c_str(), asFUNCTION( *ctorPtr ), asCALL_CDECL ));

        if_unlikely( _genHeader )
        {
            _header << '\t' << _name << ' ';
            GlobalFunction<Fn>::GetCppArgs( INOUT _header, argNames );
            _header << ";\n";
        }
    }

/*
=================================================
    AddProperty
=================================================
*/
    template <typename T> template <typename B>
    void  ClassBinder<T>::AddProperty (B T::* value, StringView name) __Th___
    {
        String  signature;
        ScriptTypeInfo<B>::Name( INOUT signature );

        auto*   info = GetASEngine()->GetTypeInfoByDecl( signature.c_str() );
        if ( info != null )
        {
            if constexpr( IsEnum<B> )
            {
                if ( info->GetSize() != sizeof(B) )
                {
                    AE_LOG_DBG( "Property '"s << signature << ' ' << _name << '.' << name << "' has size (" << ToString(sizeof(B)) <<
                                "), but in AS it has size (" << ToString(info->GetSize()) << "), will be used 'uint" << ToString(sizeof(B)*8) << "' type instead." );
                    signature.clear();
                    ScriptTypeInfo< ToUnsignedInteger<B> >::Name( INOUT signature );
                }
            }
            else
                CHECK_THROW_Eq( info->GetSize(), sizeof(B) );
        }
        signature << ' ' << name;

        AS_CHECK_THROW( GetASEngine()->RegisterObjectProperty( _name.c_str(), signature.c_str(), int(OffsetOf( value )) ));

        if_unlikely( _genHeader )
        {
            signature.clear();
            ScriptTypeInfo<B>::Name( INOUT signature );
            _header << '\t' << signature << ' ' << name << ";\n";
        }
    }

/*
=================================================
    AddProperty
=================================================
*/
    template <typename T> template <typename A, typename B>
    void  ClassBinder<T>::AddProperty (A T::* base, B A::* value, StringView name) __Th___
    {
        String  signature;
        ScriptTypeInfo<B>::Name( INOUT signature );

        auto*   info = GetASEngine()->GetTypeInfoByDecl( signature.c_str() );
        if ( info != null )
        {
            if constexpr( IsEnum<B> )
            {
                if ( info->GetSize() != sizeof(B) )
                {
                    AE_LOG_DBG( "Property '"s << signature << ' ' << _name << '.' << name << "' has size (" << ToString(sizeof(B)) <<
                                "), but in AS it has size (" << ToString(info->GetSize()) << "), will be used 'uint" << ToString(sizeof(B)*8) << "' type instead." );
                    signature.clear();
                    ScriptTypeInfo< ToUnsignedInteger<B> >::Name( INOUT signature );
                }
            }
            else
                CHECK_THROW_Eq( info->GetSize(), sizeof(B) );
        }
        signature << ' ' << name;

        Bytes   base_off    = OffsetOf( base );
        Bytes   value_off   = OffsetOf( value );
        CHECK_THROW( base_off + value_off < SizeOf<T> );

        AS_CHECK_THROW( GetASEngine()->RegisterObjectProperty( _name.c_str(), signature.c_str(), int(base_off + value_off) ));

        if_unlikely( _genHeader )
        {
            signature.clear();
            ScriptTypeInfo<B>::Name( INOUT signature );
            _header << '\t' << signature << ' ' << name << ";\n";
        }
    }

/*
=================================================
    AddProperty
=================================================
*/
    template <typename T> template <typename B>
    void  ClassBinder<T>::AddProperty (const T &self, B &value, StringView name) __Th___
    {
        String  signature;
        ScriptTypeInfo<B>::Name( INOUT signature );

        auto*   info = GetASEngine()->GetTypeInfoByDecl( signature.c_str() );
        if ( info != null )
        {
            if constexpr( IsEnum<B> )
            {
                if ( info->GetSize() != sizeof(B) )
                {
                    AE_LOG_DBG( "Property '"s << signature << ' ' << _name << '.' << name << "' has size (" << ToString(sizeof(B)) <<
                                "), but in AS it has size (" << ToString(info->GetSize()) << "), will be used 'uint" << ToString(sizeof(B)*8) << "' type instead." );
                    signature.clear();
                    ScriptTypeInfo< ToUnsignedInteger<B> >::Name( INOUT signature );
                }
            }
            else
                CHECK_THROW_Eq( info->GetSize(), sizeof(B) );
        }
        signature << ' ' << name;

        const ssize     offset = BitCast<ssize>(&value) - BitCast<ssize>(&self);
        CHECK_THROW( offset >= 0 and offset <= ssize(sizeof(T) - sizeof(B)) );

        AS_CHECK_THROW( GetASEngine()->RegisterObjectProperty( _name.c_str(), signature.c_str(), int(offset) ));

        if_unlikely( _genHeader )
        {
            signature.clear();
            ScriptTypeInfo<B>::Name( INOUT signature );
            _header << '\t' << signature << ' ' << name << ";\n";
        }
    }

/*
=================================================
    AddMethod
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddMethod (Fn methodPtr, StringView name, ArgNames_t argNames) __Th___
    {
        using namespace AngelScript;

        using C = typename FunctionInfo<Fn>::clazz;
        STATIC_ASSERT( not IsVoid<C>, "'Fn' must be class method" );
        STATIC_ASSERT( IsBaseOf< C, T >, "'Fn' must be from this class or from base class");

        String  signature;
        MemberFunction<Fn>::GetDescriptor( INOUT signature, name );

        AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(),
                                asSMethodPtr< sizeof( void (C::*)() ) >::Convert( reinterpret_cast<void (C::*)()>(methodPtr) ),
                                asCALL_THISCALL ));

        if_unlikely( _genHeader )
        {
            _header << '\t';
            MemberFunction<Fn>::GetCppDescriptor( INOUT _header, name, argNames );
            _header << ";\n";
        }
    }

/*
=================================================
    AddGenericMethod
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddGenericMethod (void (*fn)(ScriptArgList), StringView name, ArgNames_t argNames) __Th___
    {
        using namespace AngelScript;

        String  signature;
        GlobalFunction<Fn>::GetDescriptor( INOUT signature, name );

        AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(),
                                asFUNCTION(reinterpret_cast<asGENFUNC_t>(fn)),
                                asCALL_GENERIC ));

        if_unlikely( _genHeader )
        {
            _header << '\t';
            GlobalFunction<Fn>::GetCppDescriptor( INOUT _header, name, argNames );
            _header << ";\n";
        }
    }

/*
=================================================
    AddMethodFromGlobal
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddMethodFromGlobal (Fn funcPtr, StringView name, ArgNames_t argNames) __Th___
    {
        using Args = typename GlobalFunction<Fn>::TypeList_t;
        STATIC_ASSERT( Args::Count > 0 );

        constexpr bool  obj_first   = _IsSame< typename Args::Front::type >::value;
        constexpr bool  obj_last    = _IsSame< typename Args::Back::type >::value;

        if constexpr( Args::Count == 1 ){
            STATIC_ASSERT( obj_first and obj_last );
        }else{
            STATIC_ASSERT( obj_first or obj_last );
        }
        if constexpr( obj_first )
            return AddMethodFromGlobalObjFirst( funcPtr, name, argNames );
        else
            return AddMethodFromGlobalObjLast( funcPtr, name, argNames );
    }

/*
=================================================
    AddMethodFromGlobalObjFirst
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddMethodFromGlobalObjFirst (Fn funcPtr, StringView name, ArgNames_t argNames) __Th___
    {
        using namespace AngelScript;

        using FuncInfo  = FunctionInfo<Fn>;
        using FrontArg  = typename FuncInfo::args::Front::type;

        STATIC_ASSERT( IsVoid< FuncInfo::clazz >, "'Fn' must be a function" );
        STATIC_ASSERT( _IsSame< FrontArg >::value );

        String  signature;
        GlobalFunction<Fn>::GetDescriptor( INOUT signature, name, 1, 0 );

        if constexpr( IsAnyConst< FrontArg >)
            signature << " const";

        AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(), asFUNCTION( *funcPtr ), asCALL_CDECL_OBJFIRST ));

        if_unlikely( _genHeader )
        {
            _header << '\t';
            GlobalFunction<Fn>::GetCppDescriptor( INOUT _header, name, argNames, 1, 0 );
            if constexpr( IsAnyConst< FrontArg >)  _header << " const";
            _header << ";\n";
        }
    }

/*
=================================================
    AddMethodFromGlobalObjLast
=================================================
*/
    template <typename T> template <typename Fn>
    void  ClassBinder<T>::AddMethodFromGlobalObjLast (Fn funcPtr, StringView name, ArgNames_t argNames) __Th___
    {
        using namespace AngelScript;

        using FuncInfo  = FunctionInfo<Fn>;
        using BackArg   = typename FuncInfo::args::template Get< FuncInfo::args::Count-1 >;

        STATIC_ASSERT( IsVoid< FuncInfo::clazz >, "'Fn' must be a function" );
        STATIC_ASSERT( _IsSame< BackArg >::value );

        String  signature;
        GlobalFunction<Fn>::GetDescriptor( INOUT signature, name, 0, 1 );

        if constexpr( IsAnyConst< BackArg >)
            signature << " const";

        AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(), asFUNCTION( *funcPtr ), asCALL_CDECL_OBJLAST ));

        if_unlikely( _genHeader )
        {
            _header << '\t';
            GlobalFunction<Fn>::GetCppDescriptor( INOUT signature, name, argNames, 0, 1 );
            if constexpr( IsAnyConst< BackArg >)  _header << " const";
            _header << ";\n";
        }
    }

/*
=================================================
    Unary
=================================================
*/
    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Unary (EUnaryOperator op, Fn func) __Th___
    {
        using FuncInfo  = FunctionInfo<Fn>;

        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( Scripting::_hidden_::IsGlobal<Fn>() )
        {
            STATIC_ASSERT( FuncInfo::args::Count == 1 );
            _binder->AddMethodFromGlobalObjFirst( func, _UnaryToStr( op ), {} );
        }
        else
        {
            STATIC_ASSERT( FuncInfo::args::Count == 0 );
            _binder->AddMethod( func, _UnaryToStr( op ), {} );
        }
        return *this;
    }

/*
=================================================
    BinaryAssign
=================================================
*/
    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::BinaryAssign (EBinaryOperator op, Fn func) __Th___
    {
        using FuncInfo  = FunctionInfo<Fn>;

        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( Scripting::_hidden_::IsGlobal<Fn>() )
        {
            STATIC_ASSERT( FuncInfo::args::Count == 2 );
            _binder->AddMethodFromGlobalObjFirst( func, _BinAssignToStr( op ), {} );
        }
        else
        {
            STATIC_ASSERT( FuncInfo::args::Count == 1 );
            _binder->AddMethod( func, _BinAssignToStr( op ), {} );
        }
        return *this;
    }

/*
=================================================
    Index
=================================================
*/
    template <typename T> template <typename OutType, typename ...InTypes>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Index () __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( IsAnyConst<OutType>() )
            return Index( static_cast< OutType (T::*) (InTypes...) const >( &T::operator [] ));
        else
            return Index( static_cast< OutType (T::*) (InTypes...) >( &T::operator [] ));
    }

    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Index (Fn func) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( Scripting::_hidden_::IsGlobal<Fn>() )
            _binder->AddMethodFromGlobalObjFirst( func, "opIndex", {} );
        else
            _binder->AddMethod( func, "opIndex", {} );

        return *this;
    }

/*
=================================================
    Call
=================================================
*/
    template <typename T> template <typename OutType, typename ...InTypes>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call (OutType (T::*func) (InTypes...)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opCall", {} );
        return *this;
    }

    template <typename T> template <typename OutType, typename ...InTypes>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call (OutType (T::*func) (InTypes...) const) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opCall", {} );
        return *this;
    }

    template <typename T> template <typename OutType, typename ...InTypes>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call (OutType (*func) (T&, InTypes...)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opCall", {} );
        return *this;
    }

    template <typename T> template <typename OutType, typename ...InTypes>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call (OutType (*func) (const T&, InTypes...)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opCall", {} );
        return *this;
    }

/*
=================================================
    Convert
=================================================
*/
    /*template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Convert () __Th___
    {
        return Convert( static_cast< OutType (T::*) () const >() );
    }*/

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Convert (OutType (T::*func) () const) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opConv", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Convert (OutType (*func) (const T &)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opConv", {} );
        return *this;
    }

/*
=================================================
    ExpCast
=================================================
*/
    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast () __Th___
    {
        return ExpCast( static_cast< OutType const& (T::*) () const >( &T::operator OutType ));
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType& (T::*func) ()) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType const& (T::*func) () const) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType* (T::*func) ()) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType const* (T::*func) () const) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType& (*func) (T &)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType const& (*func) (const T &)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType* (*func) (T *)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ExpCast (OutType const* (*func) (const T *)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opCast", {} );
        return *this;
    }

/*
=================================================
    ImplCast
=================================================
*/
    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast () __Th___
    {
        return ImplCast( static_cast< OutType const& (T::*) () const >( &T::operator OutType ));
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType& (T::*func) ()) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType const& (T::*func) () const) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType* (T::*func) ()) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType const* (T::*func) () const) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethod( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType& (*func) (T &)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType const& (*func) (const T &)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType* (*func) (T *)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opImplCast", {} );
        return *this;
    }

    template <typename T> template <typename OutType>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::ImplCast (OutType const* (*func) (const T *)) __Th___
    {
        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjFirst( func, "opImplCast", {} );
        return *this;
    }

/*
=================================================
    Binary
=================================================
*/
    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Binary (EBinaryOperator op, Fn func) __Th___
    {
        using FuncInfo  = FunctionInfo<Fn>;

        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( Scripting::_hidden_::IsGlobal<Fn>() )
        {
            STATIC_ASSERT( FuncInfo::args::Count == 2 );
            _binder->AddMethodFromGlobalObjFirst( func, _BinToStr( op ), {} );
        }
        else
        {
            STATIC_ASSERT( FuncInfo::args::Count == 1 );
            _binder->AddMethod( func, _BinToStr( op ), {} );
        }
        return *this;
    }

/*
=================================================
    BinaryRH
=================================================
*/
    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::BinaryRH (EBinaryOperator op, Fn func) __Th___
    {
        STATIC_ASSERT( Scripting::_hidden_::IsGlobal<Fn>() );
        STATIC_ASSERT( FunctionInfo<Fn>::args::Count == 2 );

        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        _binder->AddMethodFromGlobalObjLast( func, _BinRightToStr( op ), {} );
        return *this;
    }

/*
=================================================
    Equals
=================================================
*/
    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Equals (Fn func) __Th___
    {
        using FuncInfo = FunctionInfo<Fn>;

        STATIC_ASSERT( IsSameTypes< typename FuncInfo::result, bool > );

        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( Scripting::_hidden_::IsGlobal<Fn>() )
        {
            STATIC_ASSERT( FuncInfo::args::Count == 2 );
            STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<0>, T > or IsSameTypes< typename FuncInfo::args::template Get<0>, const T& > ));
            STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<1>, T > or IsSameTypes< typename FuncInfo::args::template Get<1>, const T& > ));

            _binder->AddMethodFromGlobalObjFirst( func, "opEquals", {} );
        }
        else
        {
            STATIC_ASSERT( FuncInfo::args::Count == 1 );
            _binder->AddMethod( func, "opEquals", {} );
        }
        return *this;
    }

/*
=================================================
    Compare
=================================================
*/
    template <typename T> template <typename Fn>
    typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Compare (Fn func) __Th___
    {
        using FuncInfo = FunctionInfo<Fn>;

        STATIC_ASSERT( IsSameTypes< typename FuncInfo::result, int > );
        STATIC_ASSERT( FuncInfo::args::Count == 2 );
        STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<0>, T > or IsSameTypes< typename FuncInfo::args::template Get<0>, const T& > ));
        STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<1>, T > or IsSameTypes< typename FuncInfo::args::template Get<1>, const T& > ));

        SCOPED_SET( _binder->_genHeader, false, _binder->_genHeader );

        if constexpr( Scripting::_hidden_::IsGlobal<Fn>() )
            _binder->AddMethodFromGlobalObjFirst( func, "opCmp", {} );
        else
            _binder->AddMethod( func, "opCmp", {} );

        return *this;
    }


} // AE::Scripting
