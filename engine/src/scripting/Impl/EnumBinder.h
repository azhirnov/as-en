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
        ND_ bool  IsRegistred ()                                C_NE___;

            void  AddValue (StringView name, T value)           __Th___;

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
            _engine->AddCppHeader( _name, RVRef(_header), AngelScript::asOBJ_ENUM );
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
            AE_LOGE( "enum '" + String{Name()} + "' already registerd" );

        AS_CHECK_THROW( res );

        if_unlikely( _genHeader )
        {
            _header += "struct ";
            _header += _name;
            _header += "\n{\n";

            _header += "\t";
            _header += _name;
            _header += " () {}\n";

            _header += "\t";
            _header += _name;
            _header += " (";
            if constexpr( sizeof(T) == sizeof(ulong) )  _header += "uint64";
            else                                        _header += "uint";
            _header += ") {}\n";

            _header += "\toperator ";
            if constexpr( sizeof(T) == sizeof(ulong) )  _header += "uint64";
            else                                        _header += "uint";
            _header += " () const;\n";
        }
    }

/*
=================================================
    IsRegistred
=================================================
*/
    template <typename T>
    bool  EnumBinder<T>::IsRegistred () C_NE___
    {
        return _engine->IsRegistred( _name );
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
            _header += "\tstatic constexpr ";
            if constexpr( sizeof(T) == sizeof(ulong) )  _header += "uint64 ";
            else                                        _header += "uint ";
            _header += valueName;
            _header += " = ";
            _header += ToString( ulong(value) );
            _header += ";\n";
        }
    }

} // AE::Scripting
