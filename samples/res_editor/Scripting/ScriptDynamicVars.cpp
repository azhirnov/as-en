// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{
    using namespace AE::Scripting;

//-----------------------------------------------------------------------------
// ScriptDynamicDim

/*
=================================================
    Mul
=================================================
*/
    ScriptDynamicDim*  ScriptDynamicDim::Mul3 (const packed_int3 &value) C_Th___
    {
        CHECK_THROW( All( value > 0 ));

        auto    ds = MakeRC<DynamicDim>( _dynSize );
        ds->SetScale( value );

        ScriptDynamicDimPtr result{new ScriptDynamicDim{ ds }};

        return result.Detach();
    }

/*
=================================================
    Div
=================================================
*/
    ScriptDynamicDim*  ScriptDynamicDim::Div3 (const packed_int3 &value) C_Th___
    {
        CHECK_THROW( All( value > 0 ));

        auto    ds = MakeRC<DynamicDim>( _dynSize );
        ds->SetScale( -value, DynamicDim::ERounding::Floor );

        ScriptDynamicDimPtr result{new ScriptDynamicDim{ ds }};

        return result.Detach();
    }

/*
=================================================
    DivRound
=================================================
*/
    ScriptDynamicDim*  ScriptDynamicDim::DivRound3 (const packed_int3 &value) C_Th___
    {
        CHECK_THROW( All( value > 0 ));

        auto    ds = MakeRC<DynamicDim>( _dynSize );
        ds->SetScale( -value, DynamicDim::ERounding::Round );

        ScriptDynamicDimPtr result{new ScriptDynamicDim{ ds }};

        return result.Detach();
    }

/*
=================================================
    DivCeil
=================================================
*/
    ScriptDynamicDim*  ScriptDynamicDim::DivCeil3 (const packed_int3 &value) C_Th___
    {
        CHECK_THROW( All( value > 0 ));

        auto    ds = MakeRC<DynamicDim>( _dynSize );
        ds->SetScale( -value, DynamicDim::ERounding::Ceil );

        ScriptDynamicDimPtr result{new ScriptDynamicDim{ ds }};

        return result.Detach();
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptDynamicDim::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicDim>   binder{ se };
        binder.CreateRef();

        binder.AddMethod( &ScriptDynamicDim::Mul1,      "opMul",        {} );
        binder.AddMethod( &ScriptDynamicDim::Div1,      "opDiv",        {} );
        binder.AddMethod( &ScriptDynamicDim::Mul1,      "Mul",          {} );
        binder.AddMethod( &ScriptDynamicDim::Div1,      "Div",          {} );
        binder.AddMethod( &ScriptDynamicDim::DivRound1, "DivRound",     {} );
        binder.AddMethod( &ScriptDynamicDim::DivCeil1,  "DivCeil",      {} );

        binder.AddMethod( &ScriptDynamicDim::Mul2,      "opMul",        {} );
        binder.AddMethod( &ScriptDynamicDim::Div2,      "opDiv",        {} );
        binder.AddMethod( &ScriptDynamicDim::Mul2,      "Mul",          {} );
        binder.AddMethod( &ScriptDynamicDim::Div2,      "Div",          {} );
        binder.AddMethod( &ScriptDynamicDim::DivRound2, "DivRound",     {} );
        binder.AddMethod( &ScriptDynamicDim::DivCeil2,  "DivCeil",      {} );

        binder.AddMethod( &ScriptDynamicDim::Mul3,      "opMul",        {} );
        binder.AddMethod( &ScriptDynamicDim::Div3,      "opDiv",        {} );
        binder.AddMethod( &ScriptDynamicDim::Mul3,      "Mul",          {} );
        binder.AddMethod( &ScriptDynamicDim::Div3,      "Div",          {} );
        binder.AddMethod( &ScriptDynamicDim::DivRound3, "DivRound",     {} );
        binder.AddMethod( &ScriptDynamicDim::DivCeil3,  "DivCeil",      {} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ScriptDynamicMatrix4x4::Bind
=================================================
*
    void  ScriptDynamicMatrix4x4::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicMatrix4x4> binder{ se };
        binder.CreateRef();
    }
*/
//-----------------------------------------------------------------------------



/*
=================================================
    ScriptDynamicUInt::Bind
=================================================
*/
    void  ScriptDynamicUInt::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicUInt>  binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicUInt2::Bind
=================================================
*/
    void  ScriptDynamicUInt2::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicUInt2> binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicUInt3::Bind
=================================================
*/
    void  ScriptDynamicUInt3::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicUInt3> binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicUInt4::Bind
=================================================
*/
    void  ScriptDynamicUInt4::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicUInt4> binder{ se };
        binder.CreateRef();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ScriptDynamicInt::Bind
=================================================
*/
    void  ScriptDynamicInt::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicInt>   binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicInt2::Bind
=================================================
*/
    void  ScriptDynamicInt2::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicInt2>  binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicInt3::Bind
=================================================
*/
    void  ScriptDynamicInt3::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicInt3>  binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicInt4::Bind
=================================================
*/
    void  ScriptDynamicInt4::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicInt4>  binder{ se };
        binder.CreateRef();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ScriptDynamicFloat::Bind
=================================================
*/
    void  ScriptDynamicFloat::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicFloat> binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicFloat2::Bind
=================================================
*/
    void  ScriptDynamicFloat2::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicFloat2>    binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicFloat3::Bind
=================================================
*/
    void  ScriptDynamicFloat3::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicFloat3>    binder{ se };
        binder.CreateRef();
    }

/*
=================================================
    ScriptDynamicFloat4::Bind
=================================================
*/
    void  ScriptDynamicFloat4::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicFloat4>    binder{ se };
        binder.CreateRef();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ScriptDynamicULong::Bind
=================================================
*/
    void  ScriptDynamicULong::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<ScriptDynamicULong> binder{ se };
        binder.CreateRef();
    }


} // AE::ResEditor
