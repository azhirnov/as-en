// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"

namespace AE::Scripting
{

    enum class EUnaryOperator
    {
        Inverse,
        Not,
        PreInc,
        PreDec,
        PostInc,
        PostDec
    };

    enum class EBinaryOperator
    {
        Assign,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        And,
        Or,
        Xor,
        ShiftLeft,
        ShiftRight
    };


    namespace _hidden_
    {
        struct OperatorBinderHelper
        {
            static const char *  _UnaryToStr (EUnaryOperator op);
            static const char *  _BinToStr (EBinaryOperator op);
            static const char *  _BinAssignToStr (EBinaryOperator op);
            static const char *  _BinRightToStr (EBinaryOperator op);
        };

        template <typename Fn>
        ND_ constexpr bool IsGlobal ();

    } // _hidden_



    //
    // Class Binder
    //

    template <typename T>
    struct ClassBinder final
    {
    // types
    public:
        using Self          = ClassBinder<T>;
        using Class_t       = T;
        using ArgNames_t    = ArrayView<StringView>;


        //
        // Operator Binder (helper class)
        // see http://www.angelcode.com/angelscript/sdk/docs/manual/doc_script_class_ops.html
        //
        struct OperatorBinder final : Scripting::_hidden_::OperatorBinderHelper
        {
            friend struct ClassBinder<T>;

        // types
        public:
            using Self  = OperatorBinder;


        // variables
        private:
            Ptr< ClassBinder<T> >       _binder;


        // methods
        private:
            OperatorBinder () {}
            explicit OperatorBinder (ClassBinder<T> *ptr) : _binder(ptr) {}

        public:
            template <typename Fn>      Self &  Unary (EUnaryOperator op, Fn func)          __Th___;
            template <typename Fn>      Self &  BinaryAssign (EBinaryOperator op, Fn func)  __Th___;
            template <typename Fn>      Self &  Binary (EBinaryOperator op, Fn func)        __Th___;
            template <typename Fn>      Self &  BinaryRH (EBinaryOperator op, Fn func)      __Th___;


            // index
            template <typename OutType, typename ...InTypes>    Self &  Index ()            __Th___;    // x[...]
            template <typename Fn>                              Self &  Index (Fn func)     __Th___;


            // call
            template <typename OutType, typename ...InTypes>    Self &  Call (OutType (T::*) (InTypes...))          __Th___;    // x(...)
            template <typename OutType, typename ...InTypes>    Self &  Call (OutType (T::*) (InTypes...) const)    __Th___;
            template <typename OutType, typename ...InTypes>    Self &  Call (OutType (*) (T&, InTypes...))         __Th___;
            template <typename OutType, typename ...InTypes>    Self &  Call (OutType (*) (const T&, InTypes...))   __Th___;


            // convert
            template <typename OutType> Self &  Convert ()                                  __Th___;    // y(x)
            template <typename OutType> Self &  Convert (OutType (T::*) () const)           __Th___;
            template <typename OutType> Self &  Convert (OutType (*) (const T &))           __Th___;


            // explicit cast
            template <typename OutType> Self &  ExpCast ()                                  __Th___;    // y(x)
            template <typename OutType> Self &  ExpCast (OutType& (T::*) ())                __Th___;
            template <typename OutType> Self &  ExpCast (OutType const& (T::*) () const)    __Th___;
            template <typename OutType> Self &  ExpCast (OutType* (T::*) ())                __Th___;
            template <typename OutType> Self &  ExpCast (OutType const* (T::*) () const)    __Th___;
            template <typename OutType> Self &  ExpCast (OutType& (*) (T &))                __Th___;
            template <typename OutType> Self &  ExpCast (OutType const& (*) (const T &))    __Th___;
            template <typename OutType> Self &  ExpCast (OutType* (*) (T *))                __Th___;
            template <typename OutType> Self &  ExpCast (OutType const* (*) (const T *))    __Th___;

            // implicit cast
            template <typename OutType> Self &  ImplCast ()                                 __Th___;    // y(x)
            template <typename OutType> Self &  ImplCast (OutType& (T::*) ())               __Th___;
            template <typename OutType> Self &  ImplCast (OutType const& (T::*) () const)   __Th___;
            template <typename OutType> Self &  ImplCast (OutType* (T::*) ())               __Th___;
            template <typename OutType> Self &  ImplCast (OutType const* (T::*) () const)   __Th___;
            template <typename OutType> Self &  ImplCast (OutType& (*) (T &))               __Th___;
            template <typename OutType> Self &  ImplCast (OutType const& (*) (const T &))   __Th___;
            template <typename OutType> Self &  ImplCast (OutType* (*) (T *))               __Th___;
            template <typename OutType> Self &  ImplCast (OutType const* (*) (const T *))   __Th___;

            // compare
            template <typename Fn>      Self &  Equal (Fn func)                         __Th___;    // x == y
            template <typename Fn>      Self &  Compare (Fn func)                           __Th___;    // x <> y
        };


    // variables
    private:
        ScriptEnginePtr     _engine;
        String              _name;
        int                 _flags      = 0;    // asEObjTypeFlags

        bool                _genHeader  = false;
        String              _header;


    // methods
    public:
        explicit ClassBinder (const ScriptEnginePtr &eng)                                               __NE___;
        ClassBinder (const ScriptEnginePtr &eng, StringView name)                                       __Th___;
        ~ClassBinder ()                                                                                 __NE___;


        void  CreatePodValue (int flags = 0)                                                            __Th___;
        void  CreateClassValue (int flags = 0)                                                          __Th___;
        void  CreateRef (int flags = 0, Bool hasFactory = True{})                                       __Th___;
        void  CreateRef (T* (*create)(), void (T:: *addRef)(), void (T:: *releaseRef)(), int flags = 0) __Th___;

        // can be used to write docs in code
        void  Comment (StringView text)                                                                 __Th___;

        ND_ bool  IsRegistered ()                                                                       C_NE___;

        template <typename Fn>
        void  AddConstructor (Fn ctorPtr, ArgNames_t = {})                                              __Th___;

        template <typename ...Args>
        void  AddConstructor (ArgNames_t = {})                                                          __Th___;

        template <typename Fn>
        void  AddFactoryCtor (Fn ctorPtr, ArgNames_t = {})                                              __Th___;

        template <typename ...Args>
        void  AddFactoryCtor (ArgNames_t = {})                                                          __Th___;

        template <typename B>
        void  AddProperty (B T::* value, StringView name)                                               __Th___;

        template <typename A, typename B>
        void  AddProperty (A T::* base, B A::* value, StringView name)                                  __Th___;

        template <typename B>
        void  AddProperty (const T &self, B &value, StringView name)                                    __Th___;

        template <typename Fn>
        void  AddMethod (Fn methodPtr, StringView name, ArgNames_t = {})                                __Th___;

        template <typename Fn>
        void  AddGenericMethod (void (*fn)(ScriptArgList), StringView name, ArgNames_t = {})            __Th___;

        template <typename Fn>
        void  AddMethodFromGlobal (Fn funcPtr, StringView name, ArgNames_t = {})                        __Th___;

        template <typename Fn>
        void  AddMethodFromGlobalObjFirst (Fn funcPtr, StringView name, ArgNames_t = {})                __Th___;

        template <typename Fn>
        void  AddMethodFromGlobalObjLast (Fn funcPtr, StringView name, ArgNames_t = {})                 __Th___;

        ND_ OperatorBinder                      Operators ()                                            __NE___ { return OperatorBinder( this ); }

        ND_ StringView                          Name ()                                                 C_NE___ { return _name; }

        ND_ const ScriptEnginePtr &             GetEngine ()                                            C_NE___ { return _engine; }
        ND_ Ptr< AngelScript::asIScriptEngine > GetASEngine ()                                          __NE___ { return _engine->Get(); }


    private:
        void  _Create (int flags)                                                                       __Th___;

        template <typename T1>
        struct _IsSame;

        struct _Util;
    };


} // AE::Scripting

#include "scripting/Impl/ClassBinder.inl.h"
