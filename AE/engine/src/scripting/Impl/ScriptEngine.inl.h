// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"
#include "scripting/Impl/ScriptTypes.h"
#include "scripting/Impl/ScriptArgList.h"

namespace AE::Scripting
{

/*
=================================================
    IsRegistred
=================================================
*/
    template <typename T>
    bool  ScriptEngine::IsRegistred () __NE___
    {
        String  name;
        ScriptTypeInfo< T >::Name( INOUT name );
        return IsRegistred( name );
    }

/*
=================================================
    AddFunction
=================================================
*/
    template <typename T>
    void  ScriptEngine::AddFunction (T func, StringView name) __Th___
    {
        AddFunction( func, name, Default, Default );
    }

    template <typename T>
    void  ScriptEngine::AddFunction (T func, StringView name, ArgNames_t argNames, StringView comment) __Th___
    {
        using namespace AngelScript;

        String  signature;
        GlobalFunction<T>::GetDescriptor( INOUT signature, name );

        AS_CHECK_THROW( _engine->RegisterGlobalFunction( signature.c_str(), asFUNCTION( *func ), asCALL_CDECL ));

        if_unlikely( IsUsingCppHeader() )
        {
            String  decl, src;
            GlobalFunction<T>::GetCppDescriptor( INOUT decl, name, argNames );
            decl << ";\n";

            if ( not comment.empty() )
            {
                src << '\n';
                for (usize pos = 0; pos < comment.size();)
                {
                    StringView  line;
                    Parser::ReadCurrLine( comment, INOUT pos, OUT line );
                    src << "// " << line << '\n';
                }
            }
            src << decl;

            AddCppHeader( RVRef(decl), RVRef(src), int(asOBJ_MASK_VALID_FLAGS) );
        }
    }

/*
=================================================
    AddGenericFn
=================================================
*/
    template <typename Fn>
    void  ScriptEngine::AddGenericFn (void (*fn)(ScriptArgList), StringView name) __Th___
    {
        AddGenericFn<Fn>( fn, name, Default, Default );
    }

    template <typename Fn>
    void  ScriptEngine::AddGenericFn (void (*fn)(ScriptArgList), StringView name, ArgNames_t argNames, StringView comment) __Th___
    {
        using namespace AngelScript;

        String  signature;
        GlobalFunction<Fn>::GetDescriptor( INOUT signature, name );

        AS_CHECK_THROW( _engine->RegisterGlobalFunction( signature.c_str(), asFUNCTION(reinterpret_cast<asGENFUNC_t>(fn)), asCALL_GENERIC ));

        if_unlikely( IsUsingCppHeader() )
        {
            String  decl, src;
            GlobalFunction<Fn>::GetCppDescriptor( INOUT decl, name, argNames );
            decl << ";\n";

            if ( not comment.empty() )
            {
                src << '\n';
                for (usize pos = 0; pos < comment.size();)
                {
                    StringView  line;
                    Parser::ReadCurrLine( comment, INOUT pos, OUT line );
                    src << "// " << line << '\n';
                }
            }
            src << decl;

            AddCppHeader( RVRef(decl), RVRef(src), int(asOBJ_MASK_VALID_FLAGS) );
        }
    }

/*
=================================================
    AddProperty
=================================================
*/
    template <typename T>
    void  ScriptEngine::AddProperty (INOUT T &var, StringView name) __Th___
    {
        String  signature;
        ScriptTypeInfo<T>::Name( INOUT signature );

        auto*   info = _engine->GetTypeInfoByDecl( signature.c_str() );
        if ( info != null )
        {
            if constexpr( IsEnum<T> )
            {
                if ( info->GetSize() != sizeof(T) )
                {
                    AE_LOG_DBG( "Global property '"s << signature << ' ' << name << "' has size (" << ToString(sizeof(T)) <<
                                "), but in AS it has size (" << ToString(info->GetSize()) << "), will be used 'uint" << ToString(sizeof(T)*8) << "' type instead." );
                    signature.clear();
                    ScriptTypeInfo< ToUnsignedInteger<T> >::Name( INOUT signature );
                }
            }
            else
                CHECK_THROW_Eq( info->GetSize(), sizeof(T) );
        }
        signature << ' ' << name;

        AS_CHECK_THROW( _engine->RegisterGlobalProperty( signature.c_str(), Cast<void *>(&var) ));

        if_unlikely( IsUsingCppHeader() )
        {
            String  str;
            ScriptTypeInfo<T>::CppArg( INOUT str );
            str << ' ' << name << ";\n";

            AddCppHeader( str, str, int(AngelScript::asOBJ_MASK_VALID_FLAGS) );
        }
    }

/*
=================================================
    AddConstProperty
=================================================
*/
    template <typename T>
    void  ScriptEngine::AddConstProperty (const T &var, StringView name) __Th___
    {
        String  signature( "const " );
        ScriptTypeInfo<T>::Name( INOUT signature );
        signature << ' ' << name;

        AS_CHECK_THROW( _engine->RegisterGlobalProperty( signature.c_str(), Cast<void *>(const_cast<T*>(&var)) ));

        if_unlikely( IsUsingCppHeader() )
        {
            String  str = "const ";
            ScriptTypeInfo<T>::CppArg( INOUT str );
            str << ' ' << name << ";\n";

            AddCppHeader( str, str, int(AngelScript::asOBJ_MASK_VALID_FLAGS) );
        }
    }

/*
=================================================
    CreateScript
=================================================
*/
    template <typename Fn>
    ScriptFnPtr<Fn>  ScriptEngine::CreateScript (StringView entry, const ScriptModulePtr &module) __NE___
    {
        String  signature;
        GlobalFunction<Fn>::GetDescriptor( INOUT signature, entry );

        AngelScript::asIScriptContext*  ctx = null;

        if_unlikely( not _CreateContext( signature, module, OUT ctx ))
        {
            if ( ctx )
                ctx->Release();

            return null;
        }

        return ScriptFnPtr<Fn>{ new ScriptFn<Fn>{ module, ctx }};
    }
//-----------------------------------------------------------------------------



/*
=================================================
    HasFunction
=================================================
*/
    template <typename Fn>
    bool  ScriptModule::HasFunction (StringView entry) C_NE___
    {
        CHECK_ERR( _module != null );

        String  signature;
        GlobalFunction<Fn>::GetDescriptor( INOUT signature, entry );

        auto*   fn = _module->GetFunctionByDecl( signature.c_str() );
        return fn != null;
    }


} // AE::Scripting
