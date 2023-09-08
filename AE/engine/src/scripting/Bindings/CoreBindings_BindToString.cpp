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
            using VecTypes = TypeList<
                                packed_bool2,   packed_bool3,   packed_bool4,
                                packed_sbyte2,  packed_sbyte3,  packed_sbyte4,
                                packed_ubyte2,  packed_ubyte3,  packed_ubyte4,
                                packed_short2,  packed_short3,  packed_short4,
                                packed_int2,    packed_int3,    packed_int4,
                                packed_uint2,   packed_uint3,   packed_uint4,
                                packed_float2,  packed_float3,  packed_float4 >;

            VecTypes::Visit( ToStringVisitor{ se });
        }
        if ( rect )
        {
            using RectTypes = TypeList< RectU, RectI, RectF >;

            RectTypes::Visit( ToStringVisitor{ se });
        }
        if ( color )
        {
            using ColorTypes = TypeList< RGBA32f, RGBA32i, RGBA32u, RGBA8u /*,
                                         DepthStencil, HSVColor*/ >;

            ColorTypes::Visit( ToStringVisitor{ se });
        }
    }

} // AE::Scripting
