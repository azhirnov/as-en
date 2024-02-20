// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ClassBinder.h"

namespace AE::Scripting
{

    //
    // Enum Binder
    //
    template <typename T>
    class EnumBinder final
    {
    // types
    public:
        using Self      = EnumBinder<T>;
        using Enum_t    = T;


    // variables
    private:
        ScriptEnginePtr     _engine;
        String              _name;

        const bool          _genHeader  = false;
        String              _header;


    // methods
    public:
        explicit EnumBinder (const ScriptEnginePtr &eng)        __NE___;
        ~EnumBinder ()                                          __NE___;

            void  Create ()                                     __Th___;
        ND_ bool  IsRegistered ()                               C_NE___;

            void  AddValue (StringView name, T value)           __Th___;

        // can be used to write docs in code
            void  Comment (StringView text)                     __Th___;

        ND_ StringView                          Name ()         C_NE___ { return _name; }
        ND_ const ScriptEnginePtr &             GetEngine ()    C_NE___ { return _engine; }
        ND_ Ptr< AngelScript::asIScriptEngine > GetASEngine ()  __NE___ { return _engine->Get(); }
    };



/*
=================================================
    constructor
=================================================
*/
    template <typename T>
    EnumBinder<T>::EnumBinder (const ScriptEnginePtr &eng) __NE___ :
        _engine{ eng }, _genHeader{ eng->IsUsingCppHeader() }
    {
        ScriptTypeInfo< T >::Name( OUT _name );
    }

/*
=================================================
    destructor
=================================================
*/
    template <typename T>
    EnumBinder<T>::~EnumBinder () __NE___
    {
        if_unlikely( _genHeader )
            _engine->AddCppHeader( RVRef(_name), RVRef(_header), AngelScript::asOBJ_ENUM );
    }

/*
=================================================
    Create
=================================================
*/
    template <typename T>
    void  EnumBinder<T>::Create () __Th___
    {
        int res = GetASEngine()->RegisterEnum( _name.c_str() );

        if ( res == AngelScript::asALREADY_REGISTERED )
            AE_LOGE( "enum '" + String{Name()} + "' already registered" );

        AS_CHECK_THROW( res );

        if_unlikely( _genHeader )
        {
            const String    int_type = "uint"s << ToString(sizeof(T)*8);
            _header << "struct " << _name << "\n{\n";
            _header << "\t" << _name << " () {}\n";
            _header << "\t" << _name << " (" << int_type << ") {}\n";
            _header << "\toperator " << int_type << " () const;\n";
        }
    }

/*
=================================================
    IsRegistered
=================================================
*/
    template <typename T>
    bool  EnumBinder<T>::IsRegistered () C_NE___
    {
        return _engine->IsRegistered( _name );
    }

/*
=================================================
    AddValue
=================================================
*/
    template <typename T>
    void  EnumBinder<T>::AddValue (StringView valueName, T value) __Th___
    {
        ASSERT( slong(value) >= MinValue<int>() and slong(value) <= MaxValue<int>() );

        AS_CHECK_THROW( GetASEngine()->RegisterEnumValue( _name.c_str(), (String{Name()} + '_' + String{valueName}).c_str(), int(value) ));

        if_unlikely( _genHeader )
        {
            _header << "\tstatic constexpr ";
            _header << "uint"s << ToString(sizeof(T)*8) << ' ';
            _header << valueName << " = " << ToString( ulong(value) ) << ";\n";
        }
    }

/*
=================================================
    Comment
=================================================
*/
    template <typename T>
    void  EnumBinder<T>::Comment (StringView text) __Th___
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


} // AE::Scripting
