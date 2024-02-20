// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"

namespace AE::Scripting
{

    //
    // Script Type Info
    //

    template <typename T>
    struct ScriptTypeInfo;
    //{
    //  using type = T;
    //
    //  static constexpr bool is_object      = false;
    //  static constexpr bool is_ref_counted = false;
    //
    //  static void  Name (INOUT String &);
    //  static void  ArgName (INOUT String &s);
    //  static void  CppArg (INOUT String &s);
    //  static uint  SizeOf ();
    //};

    template <>
    struct ScriptTypeInfo <void>
    {
        using type = void;

        static constexpr bool is_object      = false;
        static constexpr bool is_ref_counted = false;

        static void  Name (INOUT String &s)     { s << "void"; }
        static void  ArgName (INOUT String &s)  { s << "void"; }
        static void  CppArg (INOUT String &s)   { s << "void"; }
    };

#   define AE_DECL_SCRIPT_TYPE( _type_, _name_ )                                            \
        template <>                                                                         \
        struct AE::Scripting::ScriptTypeInfo < _type_ >                                     \
        {                                                                                   \
            using type = _type_;                                                            \
                                                                                            \
            static constexpr bool is_object      = false;                                   \
            static constexpr bool is_ref_counted = false;                                   \
                                                                                            \
            static void  Name (INOUT String &s)     { s << (_name_); }                      \
            static void  ArgName (INOUT String &s)  { s << (_name_); }                      \
            static void  CppArg (INOUT String &s)   { s << (_name_); }                      \
        }

#   define AE_DECL_SCRIPT_OBJ( _type_, _name_ )                                             \
        template <>                                                                         \
        struct AE::Scripting::ScriptTypeInfo < _type_ >                                     \
        {                                                                                   \
            using type = _type_;                                                            \
                                                                                            \
            static constexpr bool is_object      = true;                                    \
            static constexpr bool is_ref_counted = false;                                   \
                                                                                            \
            static void  Name (INOUT String &s)     { s << (_name_); }                      \
            static void  ArgName (INOUT String &s)  { s << (_name_); }                      \
            static void  CppArg (INOUT String &s)   { s << (_name_); }                      \
        }

#   define AE_DECL_SCRIPT_OBJ_RC( _type_, _name_ )                                          \
        template <>                                                                         \
        struct AE::Scripting::ScriptTypeInfo < _type_ >                                     \
        {                                                                                   \
            using type = _type_;                                                            \
                                                                                            \
            static constexpr bool is_object      = true;                                    \
            static constexpr bool is_ref_counted = true;                                    \
                                                                                            \
            static void  Name (INOUT String &s)     { s << (_name_); }                      \
            static void  ArgName (INOUT String &s)  { s << (_name_); }                      \
            static void  CppArg (INOUT String &s)   { s << (_name_); }                      \
        };                                                                                  \
                                                                                            \
        template <>                                                                         \
        struct AE::Scripting::ScriptTypeInfo < _type_* >                                    \
        {                                                                                   \
            using type   = _type_ *;                                                        \
            using Base_t = ScriptTypeInfo< _type_ >;                                        \
                                                                                            \
            static constexpr bool is_object      = true;                                    \
            static constexpr bool is_ref_counted = true;                                    \
                                                                                            \
            static void  Name (INOUT String &s)     { s << _name_ << '@'; }                 \
            static void  ArgName (INOUT String &s)  { s << _name_ << '@'; }                 \
            static void  CppArg (INOUT String &s)   { s << "RC<" << _name_ << ">"; }        \
        };                                                                                  \
                                                                                            \
        template <>                                                                         \
        struct AE::Scripting::ScriptTypeInfo < \
                                AE::Scripting::AngelScriptHelper::SharedPtr< _type_ > >     \
        {                                                                                   \
            using type   = AngelScriptHelper::SharedPtr< _type_ >;                          \
            using Base_t = ScriptTypeInfo< _type_ >;                                        \
                                                                                            \
            static constexpr bool is_object      = true;                                    \
            static constexpr bool is_ref_counted = true;                                    \
                                                                                            \
            static void  Name (INOUT String &s)     { s << _name_ << '@'; }                 \
            static void  ArgName (INOUT String &s)  { s << _name_ << '@'; }                 \
            static void  CppArg (INOUT String &s)   { s << "RC<" << _name_ << ">"; }        \
        };                                                                                  \
                                                                                            \
        template <> struct AE::Scripting::ScriptTypeInfo < const _type_* > {};              \
        template <> struct AE::Scripting::ScriptTypeInfo < _type_& > {};                    \
        template <> struct AE::Scripting::ScriptTypeInfo < const _type_& > {}

} // AE::Scripting


#define DECL_SCRIPT_TYPE( _type_ )  AE_DECL_SCRIPT_TYPE( _type_, AE_TOSTRING( _type_ ))
DECL_SCRIPT_TYPE( bool   );
DECL_SCRIPT_TYPE( float  );
DECL_SCRIPT_TYPE( double );
DECL_SCRIPT_TYPE( int    );
#undef DECL_SCRIPT_TYPE

AE_DECL_SCRIPT_TYPE( AE::uint,              "uint"      );
AE_DECL_SCRIPT_TYPE( AE::sbyte,             "int8"      );
AE_DECL_SCRIPT_TYPE( AE::ubyte,             "uint8"     );
AE_DECL_SCRIPT_TYPE( AE::sshort,            "int16"     );
AE_DECL_SCRIPT_TYPE( AE::ushort,            "uint16"    );
AE_DECL_SCRIPT_TYPE( AE::slong,             "int64"     );
AE_DECL_SCRIPT_TYPE( AE::ulong,             "uint64"    );
AE_DECL_SCRIPT_OBJ(  AE::Base::String,      "string"    );
AE_DECL_SCRIPT_OBJ(  AE::Base::U8String,    "string"    );


namespace AE::Scripting
{

    // only 'in' and 'inout' are supported
#   define AE_DECL_SCRIPT_WRAP( _templ_, _name_, _arg_, _cppArg_ )              \
        template <typename T>                                                   \
        struct ScriptTypeInfo < _templ_ >                                       \
        {                                                                       \
        public:                                                                 \
            using type   = _templ_;                                             \
            using Base_t = ScriptTypeInfo<T>;                                   \
                                                                                \
            static constexpr bool is_object         = false;                    \
            static constexpr bool is_ref_counted    = false;                    \
                                                                                \
        private:                                                                \
            static constexpr bool _is_object        = Base_t::is_object;        \
            static constexpr bool _is_ref_counted   = Base_t::is_ref_counted;   \
                                                                                \
        public:                                                                 \
            static void  Name (INOUT String &s)     { _name_; }                 \
            static void  ArgName (INOUT String &s)  { _arg_; }                  \
            static void  CppArg (INOUT String &s)   { _cppArg_; }               \
        }

#   define MULTILINE_ARG( ... )  __VA_ARGS__
    AE_DECL_SCRIPT_WRAP( const T,
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::Name( s );
                         ),
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::Name( s );
                         ),
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::CppArg( s );
                         ));

    AE_DECL_SCRIPT_WRAP( const T &,
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::Name( s );
                            s << " &";
                         ),
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::Name( s );
                            s << " &in";
                         ),
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::CppArg( s );
                            s << " &";
                         ));

    AE_DECL_SCRIPT_WRAP( T &,
                         MULTILINE_ARG(
                            Base_t::Name( s );
                            s << " &";
                         ),
                         MULTILINE_ARG(
                            Base_t::Name( s );
                            s << (_is_ref_counted ? " &inout" : " &out");
                         ),
                         MULTILINE_ARG(
                            Base_t::CppArg( s );
                            s << " &";
                         ));

    AE_DECL_SCRIPT_WRAP( const T *,
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::Name( s );
                            s << " &";
                         ),
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::Name( s );
                            s << " &in";
                         ),
                         MULTILINE_ARG(
                            s << "const ";
                            Base_t::CppArg( s );
                            s << " &";
                         ));

    AE_DECL_SCRIPT_WRAP( T *,
                         MULTILINE_ARG(
                            Base_t::Name( s );
                            s << " &";
                         ),
                         MULTILINE_ARG(
                            Base_t::Name( s );
                            s << (_is_ref_counted ? " &inout" : " &out");
                         ),
                         MULTILINE_ARG(
                            Base_t::CppArg( s );
                            s << " &";
                         ));
#   undef MULTILINE_ARG
#   undef AE_DECL_SCRIPT_WRAP




    //
    // Angel Script Helper
    //

    namespace AngelScriptHelper
    {
        struct SimpleRefCounter
        {
        // variables
        private:
            int     _counter    = 0;

        public:
            DEBUG_ONLY( static inline Atomic<slong>  _dbgTotalCount {0}; )


        // methods
        private:
            SimpleRefCounter (SimpleRefCounter &&) = delete;
            SimpleRefCounter (const SimpleRefCounter &) = delete;
            void operator = (SimpleRefCounter &&) = delete;
            void operator = (const SimpleRefCounter &) = delete;

        public:
            SimpleRefCounter ()         { DEBUG_ONLY( _dbgTotalCount.fetch_add( 1 ); )}
            virtual ~SimpleRefCounter (){ ASSERT( _counter == 0 );  DEBUG_ONLY( _dbgTotalCount.fetch_sub( 1 ); )}

            void  __AddRef ()           { ASSERT( _counter >= 0 );  ++_counter; }
            void  __Release ()          { ASSERT( _counter >= 0 );  if_unlikely( (--_counter) == 0 ) { delete this; }}
            int   __Counter () const    { return _counter; }
        };


        template <typename T>
        struct SharedPtr
        {
            //StaticAssert( IsBaseOf< SimpleRefCounter, T > );

        // types
        public:
            using Type_t = T;


        // variables
        private:
            T *     _ptr = null;


        // methods
        private:
            void  _IncRef ()                                    C_NE___ { if_likely( _ptr ) { _ptr->__AddRef(); }}
            void  _DecRef ()                                    __NE___ { if_likely( _ptr ) { _ptr->__Release();  _ptr = null; }}

        public:
            SharedPtr ()                                        __NE___ {}
            SharedPtr (std::nullptr_t)                          __NE___ {}
            explicit SharedPtr (T* ptr)                         __NE___ : _ptr{ptr}         { _IncRef(); }
            SharedPtr (const SharedPtr<T> &other)               __NE___ : _ptr{other._ptr}  { _IncRef(); }
            SharedPtr (SharedPtr<T> &&other)                    __NE___ : _ptr{other._ptr}  { other._ptr = null; }
            ~SharedPtr ()                                       __NE___ { _DecRef(); }

            template <typename B>
            SharedPtr (SharedPtr<B> &&other)                    __NE___ : _ptr{static_cast<T*>(other.Detach())} {}

            template <typename B>
            SharedPtr (const SharedPtr<B> &other)               __NE___ : _ptr{static_cast<T*>(other.Get())}    { _IncRef(); }

            SharedPtr<T>&  operator = (const SharedPtr<T> &rhs) __NE___ { _DecRef();  _ptr = rhs._ptr;  _IncRef();          return *this; }
            SharedPtr<T>&  operator = (SharedPtr<T> &&rhs)      __NE___ { _DecRef();  _ptr = rhs._ptr;  rhs._ptr = null;    return *this; }

            ND_ bool  operator == (std::nullptr_t)              C_NE___ { return _ptr == null; }
            ND_ bool  operator == (const SharedPtr<T> &rhs)     C_NE___ { return _ptr == rhs._ptr; }
            ND_ bool  operator >  (const SharedPtr<T> &rhs)     C_NE___ { return _ptr >  rhs._ptr; }
            ND_ bool  operator <  (const SharedPtr<T> &rhs)     C_NE___ { return _ptr <  rhs._ptr; }

            ND_ T*  operator -> ()                              C_NE___ { ASSERT( _ptr );  return _ptr; }
            ND_ T&  operator *  ()                              C_NE___ { ASSERT( _ptr );  return *_ptr; }
            ND_ T*  Get ()                                      C_NE___ { return _ptr; }

            ND_ explicit operator bool ()                       C_NE___ { return _ptr != null; }

            ND_ int     UseCount ()                             C_NE___ { return _ptr ? _ptr->__Counter() : 0; }
            ND_ T*      Detach ()                               __NE___ { T* tmp = _ptr;  _ptr = null;  return tmp; }
            ND_ T*      Retain ()                               C_NE___ { _IncRef();  return _ptr; }

                void    Set (T* ptr)                            __NE___ { _DecRef();  _ptr = ptr;  _IncRef(); }
                void    Attach (T* ptr)                         __NE___ { _DecRef();  _ptr = ptr; }
        };


        template <typename T>
        static constexpr bool   IsSharedPtr = IsSpecializationOf< T, SharedPtr >;

        template <typename T>
        static constexpr bool   IsSharedPtrNoQual = IsSpecializationOf< RemoveAllQualifiers<T>, SharedPtr >;

        template <typename T, bool IsPtr>
        struct _RemoveSharedPtr {
            using type = typename RemoveAllQualifiers<T>::Type_t;
        };
        template <typename T>
        struct _RemoveSharedPtr< T, false > {
            using type = T;
        };

        template <typename T>
        using RemoveSharedPtr = typename _RemoveSharedPtr< T, IsSharedPtrNoQual<T> >::type;


        template <typename T>
        static T *  FactoryCreate ()
        {
            StaticAssert( not IsBaseOf< SimpleRefCounter, T > );
            return new T{};
        }

        template <typename T, typename ...Args>
        static T *  FactoryCreate2 (const Args& ...args)
        {
            StaticAssert( not IsBaseOf< SimpleRefCounter, T > );
            return new T{ args... };
        }

        template <typename ...Args>
        struct GetFactoryCreate2
        {
            template <typename T>
            static auto*  Get () {
                return &FactoryCreate2< T, Args... >;
            }
        };


        template <typename T>
        static T *  FactoryCreateRC ()
        {
            return SharedPtr<T>{ new T{} }.Detach();
        }

        template <typename T, typename ...Args>
        static T *  FactoryCreateRC2 (const Args& ...args)
        {
            return SharedPtr<T>{ new T{ args... }}.Detach();
        }

        template <typename ...Args>
        struct GetFactoryCreateRC2
        {
            template <typename T>
            static auto*  Get () {
                return &FactoryCreateRC2< T, Args... >;
            }
        };


        template <typename T>
        static void  Constructor (AngelScript::asIScriptGeneric* gen)
        {
            // TODO: bug in AngelScript: address is not aligned
            PlacementNew<T>( OUT gen->GetObject() );    // throw
        }


        template <typename T>
        static void  CopyConstructor (AngelScript::asIScriptGeneric* gen) __Th___
        {
            T const*    src = static_cast< const T *>( gen->GetArgObject(0) );
            void *      dst = gen->GetObject();
            PlacementNew<T>( OUT dst, *src );   // throw
        }


        template <typename T>
        static void  Destructor (AngelScript::asIScriptGeneric* gen)
        {
            static_cast<T *>(gen->GetObject())->~T();
        }


        template <typename T>
        static void  CopyAssign (AngelScript::asIScriptGeneric* gen) __Th___
        {
            T const*    src = static_cast< const T *>( gen->GetArgObject(0) );
            T*          dst = static_cast< T *>( gen->GetObject() );

            dst->~T();
            PlacementNew<T>( OUT dst, *src );   // throw
        }

    }; // AngelScriptHelper



    //
    // Script Function Descriptor
    //

    namespace _hidden_
    {

        template <typename T>
        struct GlobalFunction;

        template <typename T>
        struct MemberFunction;


        struct ArgsToString_Func
        {
            String &    result;
            const uint  first;
            const uint  last;

            ArgsToString_Func (uint first, uint last, INOUT String &str) __NE___ :
                result(str), first(first), last(last)
            {
                ASSERT( first <= last );
            }

            template <typename T, usize Index>
            void  operator () () __NE___
            {
                if ( Index < first or Index > last )    return;
                if ( Index > first )                    result << ", ";
                ScriptTypeInfo<T>::ArgName( INOUT result );
            }
        };


        template <typename Typelist>
        struct ArgsToString
        {
            static void  Get (INOUT String &str, uint first, uint last)
            {
                ArgsToString_Func   func( first, last, INOUT str );
                Typelist::Visit( func );
            }

            static void  GetArgs (INOUT String &str, uint offsetFromStart, uint offsetFromEnd)
            {
                ASSERT( offsetFromEnd < Typelist::Count );

                str << '(';
                Get( INOUT str, offsetFromStart, Typelist::Count - offsetFromEnd );
                str << ')';
            }
        };


        struct CppArgsToString_Func
        {
            String &                result;
            const uint              first;
            const uint              last;
            ArrayView<StringView>   argNames;

            CppArgsToString_Func (uint first, uint last, ArrayView<StringView> argNames, INOUT String &str) __NE___ :
                result{str}, first{first}, last{last}, argNames{argNames}
            {
                ASSERT( first <= last );
                ASSERT( argNames.empty() or argNames.size() == (last - first) );
            }

            template <typename T, usize Index>
            void  operator () () __NE___
            {
                if ( Index < first or Index > last )    return;
                if ( Index > first )                    result << ", ";
                ScriptTypeInfo<T>::CppArg( INOUT result );

                ASSERT( argNames.empty() or (Index-first < argNames.size()) );
                if ( Index-first < argNames.size() )    result << ' ' << argNames[Index-first];
            }
        };


        template <typename Typelist>
        struct CppArgsToString
        {
            static void  Get (INOUT String &str, ArrayView<StringView> argNames, uint first, uint last)
            {
                CppArgsToString_Func    func( first, last, argNames, INOUT str );
                Typelist::Visit( func );
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView> argNames, uint offsetFromStart, uint offsetFromEnd)
            {
                ASSERT( offsetFromEnd < Typelist::Count );

                str << '(';
                Get( INOUT str, argNames, offsetFromStart, Typelist::Count - offsetFromEnd );
                str << ')';
            }
        };


        template <typename Ret, typename ...Types>
        struct GlobalFunction < Ret (AE_CDECL *) (Types...) >
        {
            using TypeList_t    = TypeList< Types... >;
            using Result_t      = Ret;

            static void  GetDescriptor (INOUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                // can not convert between R* to SharedPtr<R>
                StaticAssert( not AngelScriptHelper::IsSharedPtrNoQual< Result_t >);

                ScriptTypeInfo< Result_t >::Name( INOUT str );
                str << ' ' << name;
                GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
            }

            static void  GetArgs (INOUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                ArgsToString< TypeList_t >::GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppDescriptor (INOUT String &str, StringView name, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                // can not convert between R* to SharedPtr<R>
                StaticAssert( not AngelScriptHelper::IsSharedPtrNoQual< Result_t >);

                ScriptTypeInfo< Result_t >::CppArg( INOUT str );
                str << "  " << name << ' ';
                GetCppArgs( INOUT str, argNames, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                CppArgsToString< TypeList_t >::GetCppArgs( INOUT str, argNames, offsetFromStart, offsetFromEnd );
            }
        };

        template <typename Ret, typename ...Types>
        struct GlobalFunction < Ret (AE_CDECL *) (Types...) noexcept > :  GlobalFunction < Ret (AE_CDECL *) (Types...) > {};


        template <typename Ret>
        struct GlobalFunction < Ret (AE_CDECL *) () >
        {
            using TypeList_t    = TypeList<>;
            using Result_t      = Ret;

            static void  GetDescriptor (INOUT String &str, StringView name, uint = 0, uint = 0)
            {
                // can not convert between R* to SharedPtr<R>
                StaticAssert( not AngelScriptHelper::IsSharedPtrNoQual< Result_t >);

                ScriptTypeInfo< Result_t >::Name( INOUT str );
                str << ' ' << name << "()";
            }

            static void  GetArgs (INOUT String &str, uint = 0, uint = 0)
            {
                str << "()";
            }

            static void  GetCppDescriptor (INOUT String &str, StringView name, ArrayView<StringView>, uint = 0, uint = 0)
            {
                // can not convert between R* to SharedPtr<R>
                StaticAssert( not AngelScriptHelper::IsSharedPtrNoQual< Result_t >);

                ScriptTypeInfo< Result_t >::CppArg( INOUT str );
                str << "  " << name << " ()";
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView>, uint = 0, uint = 0)
            {
                str << " ()";
            }
        };

        template <typename Ret>
        struct GlobalFunction < Ret (AE_CDECL *) () noexcept > :  GlobalFunction < Ret (AE_CDECL *) () > {};


        template <typename Ret, typename ...Types>
        struct GlobalFunction < Ret (Types...) > : GlobalFunction< Ret (AE_CDECL *) (Types...) > {};

        template <typename Ret, typename ...Types>
        struct GlobalFunction < Ret (Types...) noexcept > :  GlobalFunction < Ret (Types...) > {};


        template <typename C, typename Ret, typename ...Types>
        struct MemberFunction < Ret (AE_THISCALL C:: *) (Types...) >
        {
            using TypeList_t    = TypeList< Types... >;
            using Result_t      = Ret;

            static void  GetDescriptor (INOUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetDescriptor( INOUT str, name, offsetFromStart, offsetFromEnd );
            }

            static void  GetArgs (INOUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppDescriptor (INOUT String &str, StringView name, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetCppDescriptor( INOUT str, name, argNames, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetCppArgs( INOUT str, argNames, offsetFromStart, offsetFromEnd );
            }
        };

        template <typename C, typename Ret, typename ...Types>
        struct MemberFunction < Ret (AE_THISCALL C:: *) (Types...) noexcept > :  MemberFunction < Ret (AE_THISCALL C:: *) (Types...) > {};


        template <typename C, typename Ret>
        struct MemberFunction < Ret (AE_THISCALL C:: *) () >
        {
            using TypeList_t    = TypeList<>;
            using Result_t      = Ret;

            static void  GetDescriptor (INOUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetDescriptor( INOUT str, name, offsetFromStart, offsetFromEnd );
            }

            static void  GetArgs (INOUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppDescriptor (INOUT String &str, StringView name, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetCppDescriptor( INOUT str, name, argNames, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetCppArgs( INOUT str, argNames, offsetFromStart, offsetFromEnd );
            }
        };

        template <typename C, typename Ret>
        struct MemberFunction < Ret (AE_THISCALL C:: *) () noexcept > :  MemberFunction < Ret (AE_THISCALL C:: *) () > {};


        template <typename C, typename Ret, typename ...Types>
        struct MemberFunction < Ret (AE_THISCALL C:: *) (Types...) const >
        {
            using TypeList_t    = TypeList< Types... >;
            using Result_t      = Ret;

            static void  GetDescriptor (INOUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetDescriptor( INOUT str, name, offsetFromStart, offsetFromEnd );
                str << " const";
            }

            static void  GetArgs (INOUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppDescriptor (INOUT String &str, StringView name, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetCppDescriptor( INOUT str, name, argNames, offsetFromStart, offsetFromEnd );
                str << " const";
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) (Types...) >::GetCppArgs( INOUT str, argNames, offsetFromStart, offsetFromEnd );
            }
        };

        template <typename C, typename Ret, typename ...Types>
        struct MemberFunction < Ret (AE_THISCALL C:: *) (Types...) const noexcept > :  MemberFunction < Ret (AE_THISCALL C:: *) (Types...) const > {};


        template <typename C, typename Ret>
        struct MemberFunction < Ret (AE_THISCALL C:: *) () const >
        {
            using TypeList_t    = TypeList<>;
            using Result_t      = Ret;

            static void  GetDescriptor (INOUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetDescriptor( INOUT str, name, offsetFromStart, offsetFromEnd );
                str << " const";
            }

            static void  GetArgs (INOUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
            }

            static void  GetCppDescriptor (INOUT String &str, StringView name, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetCppDescriptor( INOUT str, name, argNames, offsetFromStart, offsetFromEnd );
                str << " const";
            }

            static void  GetCppArgs (INOUT String &str, ArrayView<StringView> argNames, uint offsetFromStart = 0, uint offsetFromEnd = 0)
            {
                GlobalFunction< Result_t (*) () >::GetCppArgs( INOUT str, argNames, offsetFromStart, offsetFromEnd );
            }
        };

        template <typename C, typename Ret>
        struct MemberFunction < Ret (AE_THISCALL C:: *) () const noexcept > :  MemberFunction < Ret (AE_THISCALL C:: *) () const > {};

    } // _hidden_



    //
    // Global Function
    //

    template <typename T>
    struct GlobalFunction : Scripting::_hidden_::GlobalFunction<T>
    {};


    //
    // Member Function
    //

    template <typename T>
    struct MemberFunction : Scripting::_hidden_::MemberFunction<T>
    {};


    namespace _hidden_
    {

        //
        // Context Setter Getter
        //
        template <typename T, bool IsObject>
        struct ContextSetterGetter_Var
        {
            ND_ static T    Get (AngelScript::asIScriptContext* ctx)                    { return *static_cast<T *>(ctx->GetReturnObject()); }
                static int  Set (AngelScript::asIScriptContext* ctx, int arg, T& value) { return ctx->SetArgObject( arg, static_cast<void *>(&value) ); }
        };

        template <typename T>
        struct ContextSetterGetter_Var< AngelScriptHelper::SharedPtr<T>, true >
        {
            StaticAssert( ScriptTypeInfo<T*>::is_ref_counted );

            ND_ static AngelScriptHelper::SharedPtr<T>  Get (AngelScript::asIScriptContext* ctx) {
                return AngelScriptHelper::SharedPtr<T>{ static_cast<T *>(ctx->GetReturnObject()) };
            }

            static int  Set (AngelScript::asIScriptContext* ctx, int arg, const AngelScriptHelper::SharedPtr<T> &ptr) {
                return ctx->SetArgObject( arg, reinterpret_cast<void *>(ptr.Get()) );
            }
        };

#       define DECL_CONTEXT_RESULT( _type_, _get_, _set_ ) \
            template <> \
            struct ContextSetterGetter_Var< _type_, false > \
            { \
                ND_ static _type_ Get (AngelScript::asIScriptContext* ctx)                                  { return _type_(ctx->_get_()); } \
                    static int    Set (AngelScript::asIScriptContext* ctx, int arg, const _type_ &value)    { return ctx->_set_( arg, value ); } \
            }

        DECL_CONTEXT_RESULT( sbyte,     GetReturnByte,      SetArgByte );
        DECL_CONTEXT_RESULT( ubyte,     GetReturnByte,      SetArgByte );
        DECL_CONTEXT_RESULT( sshort,    GetReturnWord,      SetArgWord );
        DECL_CONTEXT_RESULT( ushort,    GetReturnWord,      SetArgWord );
        DECL_CONTEXT_RESULT( int32_t,   GetReturnDWord,     SetArgDWord );
        DECL_CONTEXT_RESULT( uint,      GetReturnDWord,     SetArgDWord );
        DECL_CONTEXT_RESULT( slong,     GetReturnQWord,     SetArgQWord );
        DECL_CONTEXT_RESULT( ulong,     GetReturnQWord,     SetArgQWord );
        DECL_CONTEXT_RESULT( float,     GetReturnFloat,     SetArgFloat );
        DECL_CONTEXT_RESULT( double,    GetReturnDouble,    SetArgDouble );
#       undef DECL_CONTEXT_RESULT

        template <typename T, bool IsObject>
        struct _ContextSetterGetter_Ptr
        {
            ND_ static T *  Get (AngelScript::asIScriptContext* ctx)                    { return static_cast<T *>(ctx->GetReturnAddress()); }
                static int  Set (AngelScript::asIScriptContext* ctx, int arg, T* ptr)   { return ctx->SetArgAddress( arg, (void *)(ptr) ); }
        };

        template <typename T>
        struct _ContextSetterGetter_Ptr < T, true >
        {
            ND_ static T *  Get (AngelScript::asIScriptContext* ctx)
            {
                T* result = static_cast<T *>(ctx->GetReturnObject());
                if ( result != null ) result->__AddRef();
                return result;
            }

            static int  Set (AngelScript::asIScriptContext* ctx, int arg, T* ptr)   { return ctx->SetArgObject( arg, static_cast<void *>(ptr) ); }
        };

        template <typename T>
        struct ContextSetterGetter :
            ContextSetterGetter_Var< RemoveCVRef<T>, ScriptTypeInfo<RemoveCVRef<T>>::is_object >
        {};

        template <>
        struct ContextSetterGetter <void>
        {
            static void  Get (AngelScript::asIScriptContext *)      {}
            static int   Set (AngelScript::asIScriptContext *, int) { return 0; }
        };

        template <typename T>
        struct ContextSetterGetter < T * > :
            _ContextSetterGetter_Ptr< T, ScriptTypeInfo<T*>::is_ref_counted >
        {};


        template <typename T>
        inline void  ValidateRC (const T &arg)
        {
            if constexpr( IsBaseOfNoQual< AngelScriptHelper::SimpleRefCounter, T >)
            {
                if constexpr( IsPointer<T> ) {
                    CHECK( arg != null and arg->__Counter() > 0 );
                }else {
                    CHECK(false);
                }
            }
        }


        //
        // Set Context Args
        //
        template <typename ...Args>
        struct SetContextArgs;

        template <typename Arg0, typename ...Args>
        struct SetContextArgs< Arg0, Args...>
        {
            static void  Set (AngelScript::asIScriptContext* ctx, int index, Arg0&& arg0, Args&& ...args)
            {
                ValidateRC( arg0 );
                AS_CHECK( ContextSetterGetter<Arg0>::Set( ctx, index, arg0 ));
                SetContextArgs<Args...>::Set( ctx, index+1, FwdArg<Args>(args)... );
            }
        };

        template <typename Arg0>
        struct SetContextArgs< Arg0 >
        {
            static void  Set (AngelScript::asIScriptContext* ctx, int index, Arg0&& arg0)
            {
                ValidateRC( arg0 );
                AS_CHECK( ContextSetterGetter<Arg0>::Set( ctx, index, FwdArg<Arg0>(arg0) ));
            }
        };

        template <>
        struct SetContextArgs<>
        {
            static void  Set (AngelScript::asIScriptContext *, int)
            {}
        };



        //
        // Check Input Arg Types
        //
        template <typename L, typename R>
        struct _IsSame {
            static constexpr bool   value = IsSameTypes< L, R >;
        };

        template <typename L, typename R>
        struct _IsSame< AngelScriptHelper::SharedPtr<L>, R* > {
            static constexpr bool   value = IsSameTypes< L, R >;
        };

        template <typename L, typename R>
        struct _IsSame< L*, AngelScriptHelper::SharedPtr<R> > {
            static constexpr bool   value = IsSameTypes< L, R >;
        };


        template <typename TL1, typename TL2, usize Idx>
        struct CheckInputArgTypes1
        {
            using T1 = RemoveCVRef< typename TL1::template Get< Idx-1 >>;
            using T2 = RemoveCVRef< typename TL2::template Get< Idx-1 >>;

            StaticAssert( _IsSame< T1, T2 >::value );

            static constexpr bool   value = _IsSame< T1, T2 >::value and CheckInputArgTypes1< TL1, TL2, Idx-1 >::value;
        };

        template <typename TL1, typename TL2>
        struct CheckInputArgTypes1< TL1, TL2, 0 >
        {
            static constexpr bool   value = true;
        };

        template <typename TL1, typename TL2>
        struct CheckInputArgTypes
        {
            StaticAssert( TL1::Count == TL2::Count );

            static constexpr bool   value = CheckInputArgTypes1< TL1, TL2, TL1::Count >::value;
        };



        //
        // Check Type
        //
        template <typename Type>
        ND_ bool  CheckArgType (const int typeId, const AngelScript::asDWORD flags, AngelScript::asIScriptEngine* se)
        {
            using namespace AngelScript;
            using T = RemoveAllQualifiers< Type >;

            const bool  is_inref    = AllBits( flags, asTM_INREF );
            const bool  is_outref   = AllBits( flags, asTM_OUTREF );
            const bool  is_inoutref = AllBits( flags, asTM_INOUTREF );
            const bool  is_const    = AllBits( flags, asTM_CONST );

            if constexpr( not IsAnyConst<Type> )
            {
                if constexpr( IsReference<Type> or IsPointer<Type> )
                {
                    if ( not (is_inoutref or is_outref) )
                        return false;
                }
                else
                if ( is_const )
                    return false;
            }

            if constexpr( IsAnyConst<Type> )
            {
                if constexpr( IsReference<Type> or IsPointer<Type> )
                {
                    if ( not is_inref )
                        return false;
                }
                else
                if ( not is_const )
                    return false;
            }

            if constexpr( IsSameTypes< T, bool >)   return typeId == asTYPEID_BOOL;     else
            if constexpr( IsSameTypes< T, sbyte >)  return typeId == asTYPEID_INT8;     else
            if constexpr( IsSameTypes< T, ubyte >)  return typeId == asTYPEID_UINT8;    else
            if constexpr( IsSameTypes< T, sshort >) return typeId == asTYPEID_INT16;    else
            if constexpr( IsSameTypes< T, ushort >) return typeId == asTYPEID_UINT16;   else
            if constexpr( IsSameTypes< T, sint >)   return typeId == asTYPEID_INT32;    else
            if constexpr( IsSameTypes< T, uint >)   return typeId == asTYPEID_UINT32;   else
            if constexpr( IsSameTypes< T, slong >)  return typeId == asTYPEID_INT64;    else
            if constexpr( IsSameTypes< T, ulong >)  return typeId == asTYPEID_UINT64;   else
            if constexpr( IsSameTypes< T, float >)  return typeId == asTYPEID_FLOAT;    else
            if constexpr( IsSameTypes< T, double >) return typeId == asTYPEID_DOUBLE;   else
            if constexpr( IsEnum< T >)
            {
                asITypeInfo*    info = se->GetTypeInfoById( typeId );

                if ( info == null )
                    return false;

                StringView  name1   = info->GetName();
                String      name2;

                ScriptTypeInfo<T>::Name( OUT name2 );
                return name1 == name2;
            }
            else
            {
                using T2    = AngelScriptHelper::RemoveSharedPtr<T>;
                using Info  = ScriptTypeInfo<T>;

                constexpr bool  is_obj      = Info::is_object;
                constexpr bool  is_rc       = Info::is_ref_counted                  or
                                              ScriptTypeInfo<T2*>::is_ref_counted   or
                                              AngelScriptHelper::IsSharedPtrNoQual<T>;
                constexpr bool  is_value    = not (is_obj or is_rc) and IsClass<T>;
                StaticAssert( is_obj or is_rc or is_value );

                const asDWORD   obj_flags   = is_rc ? asOBJ_REF : asOBJ_VALUE;
                asITypeInfo*    info        = se->GetTypeInfoById( typeId );

                if ( info == null )
                    return false;

                StringView  name1   = info->GetName();
                String      name2;

                if constexpr( AngelScriptHelper::IsSharedPtrNoQual<T> )
                    ScriptTypeInfo<T2>::Name( OUT name2 );
                else
                    Info::Name( OUT name2 );

                if ( name1 != name2 )
                    return false;

                return AllBits( info->GetFlags(), obj_flags );
            }
        }


    } // _hidden_

} // AE::Scripting


template <typename T>
struct std::hash< AE::Scripting::AngelScriptHelper::SharedPtr<T> > {
    ND_ size_t  operator () (const AE::Scripting::AngelScriptHelper::SharedPtr<T> &key) const {
        return size_t(key.Get());
    }
};
