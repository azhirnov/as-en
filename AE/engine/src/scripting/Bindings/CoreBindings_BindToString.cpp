// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
    static String  ToString_SInt (sint x)                       { return ToString( x ); }
    static String  ToString_UInt (uint x)                       { return ToString( x ); }
    static String  ToString_SLong (slong x)                     { return ToString( x ); }
    static String  ToString_ULong (ulong x)                     { return ToString( x ); }
    static String  ToString_Float (float x)                     { return ToString( x ); }
    static String  ToString_Double (double x)                   { return ToString( x ); }
    static String  ToString_Float2 (float x, uint fractParts)   { return ToString( x, fractParts ); }
    static String  ToString_Double2 (double x, uint fractParts) { return ToString( x, fractParts ); }


    struct ToStringVisitor
    {
        ScriptEnginePtr     se;

        template <typename T, usize I>
        void  operator () () __Th___
        {
            se->AddFunction( static_cast< String (*)(const T &) >(&Base::ToString), "ToString", {"value"} );
        }
    };
}

/*
=================================================
    BindToString
=================================================
*/
    void  CoreBindings::BindToString (const ScriptEnginePtr &se,
                                      bool scalar,
                                      bool vec,
                                      bool rect,
                                      bool color) __Th___
    {
        CHECK_THROW( se and se->IsInitialized() );

        if ( scalar )
        {
            se->AddFunction( &ToString_SInt,    "ToString", {"value"} );
            se->AddFunction( &ToString_UInt,    "ToString", {"value"} );
            se->AddFunction( &ToString_SLong,   "ToString", {"value"} );
            se->AddFunction( &ToString_ULong,   "ToString", {"value"} );
            se->AddFunction( &ToString_Float,   "ToString", {"value"} );
            se->AddFunction( &ToString_Double,  "ToString", {"value"} );
            se->AddFunction( &ToString_Float2,  "ToString", {"value", "fractParts"} );
            se->AddFunction( &ToString_Double2, "ToString", {"value", "fractParts"} );
        }
        if ( vec )
        {
            VecTypes::Visit( ToStringVisitor{ se });
        }
        if ( rect )
        {
            RectTypes::Visit( ToStringVisitor{ se });
        }
        if ( color )
        {
            ColorTypes::Visit( ToStringVisitor{ se });
        }
    }

} // AE::Scripting
