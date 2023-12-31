// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Dynamic/DynamicDimension.h"
#include "res_editor/Dynamic/DynamicMatrix.h"
#include "res_editor/Dynamic/DynamicVec.h"
#include "res_editor/Dynamic/DynamicScalar.h"

namespace AE::ResEditor
{

    //
    // Script Dynamic Dimension
    //
    class ScriptDynamicDim final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicDim>      _dynSize;


    // methods
    public:
        explicit ScriptDynamicDim ()                                    __NE___ {}
        explicit ScriptDynamicDim (RC<DynamicDim> ds)                   __NE___ : _dynSize{RVRef(ds)} {}

        ND_ RC<DynamicDim>      Get ()                                  C_NE___ { return _dynSize; }

        ND_ ScriptDynamicDim*   Mul1 (int value)                        C_Th___ { return Mul3( packed_int3{value} ); }
        ND_ ScriptDynamicDim*   Div1 (int value)                        C_Th___ { return Div3( packed_int3{value} ); }
        ND_ ScriptDynamicDim*   DivRound1 (int value)                   C_Th___ { return DivRound3( packed_int3{value} ); }
        ND_ ScriptDynamicDim*   DivCeil1 (int value)                    C_Th___ { return DivCeil3( packed_int3{value} ); }

        ND_ ScriptDynamicDim*   Mul2 (const packed_int2 &value)         C_Th___ { return Mul3( packed_int3{value,1} ); }
        ND_ ScriptDynamicDim*   Div2 (const packed_int2 &value)         C_Th___ { return Div3( packed_int3{value,1} ); }
        ND_ ScriptDynamicDim*   DivRound2 (const packed_int2 &value)    C_Th___ { return DivRound3( packed_int3{value,1} ); }
        ND_ ScriptDynamicDim*   DivCeil2 (const packed_int2 &value)     C_Th___ { return DivCeil3( packed_int3{value,1} ); }

        ND_ ScriptDynamicDim*   Mul3 (const packed_int3 &value)         C_Th___;
        ND_ ScriptDynamicDim*   Div3 (const packed_int3 &value)         C_Th___;
        ND_ ScriptDynamicDim*   DivRound3 (const packed_int3 &value)    C_Th___;
        ND_ ScriptDynamicDim*   DivCeil3 (const packed_int3 &value)     C_Th___;

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Matrix4x4
    //
    /*
    class ScriptDynamicMatrix4x4 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicMatrix4x4>    _value;


    // methods
    public:
        explicit ScriptDynamicMatrix4x4 ()                              __Th___ {}
        explicit ScriptDynamicMatrix4x4 (RC<DynamicMatrix4x4> dm)       __NE___ : _value{RVRef(dm)} {}

        ND_ RC<DynamicMatrix4x4>  Get ()                                C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };
    */



    //
    // Script Dynamic UInt
    //
    class ScriptDynamicUInt final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicUInt>     _value;


    // methods
    public:
        explicit ScriptDynamicUInt ()                                   __Th___ : _value{ MakeRC<DynamicUInt>() } {}
        explicit ScriptDynamicUInt (RC<DynamicUInt> dv)                 __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicUInt>  Get ()                                     C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic UInt2
    //
    class ScriptDynamicUInt2 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicUInt2>    _value;


    // methods
    public:
        explicit ScriptDynamicUInt2 ()                                  __Th___ : _value{ MakeRC<DynamicUInt2>() } {}
        explicit ScriptDynamicUInt2 (RC<DynamicUInt2> dv)               __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicUInt2>  Get ()                                    C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic UInt3
    //
    class ScriptDynamicUInt3 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicUInt3>    _value;


    // methods
    public:
        explicit ScriptDynamicUInt3 ()                                  __Th___ : _value{ MakeRC<DynamicUInt3>() } {}
        explicit ScriptDynamicUInt3 (RC<DynamicUInt3> dv)               __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicUInt3>  Get ()                                    C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic UInt4
    //
    class ScriptDynamicUInt4 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicUInt4>    _value;


    // methods
    public:
        explicit ScriptDynamicUInt4 ()                                  __Th___ : _value{ MakeRC<DynamicUInt4>() } {}
        explicit ScriptDynamicUInt4 (RC<DynamicUInt4> dv)               __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicUInt4>  Get ()                                    C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Int
    //
    class ScriptDynamicInt final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicInt>      _value;


    // methods
    public:
        explicit ScriptDynamicInt ()                                    __Th___ : _value{ MakeRC<DynamicInt>() } {}
        explicit ScriptDynamicInt (RC<DynamicInt> dv)                   __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicInt>  Get ()                                      C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Int2
    //
    class ScriptDynamicInt2 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicInt2>     _value;


    // methods
    public:
        explicit ScriptDynamicInt2 ()                                   __Th___ : _value{ MakeRC<DynamicInt2>() } {}
        explicit ScriptDynamicInt2 (RC<DynamicInt2> dv)                 __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicInt2>  Get ()                                     C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Int3
    //
    class ScriptDynamicInt3 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicInt3>     _value;


    // methods
    public:
        explicit ScriptDynamicInt3 ()                                   __Th___ : _value{ MakeRC<DynamicInt3>() } {}
        explicit ScriptDynamicInt3 (RC<DynamicInt3> dv)                 __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicInt3>  Get ()                                     C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Int4
    //
    class ScriptDynamicInt4 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicInt4>     _value;


    // methods
    public:
        explicit ScriptDynamicInt4 ()                                   __Th___ : _value{ MakeRC<DynamicInt4>() } {}
        explicit ScriptDynamicInt4 (RC<DynamicInt4> dv)                 __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicInt4>  Get ()                                     C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Float
    //
    class ScriptDynamicFloat final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicFloat>    _value;


    // methods
    public:
        explicit ScriptDynamicFloat ()                                  __Th___ : _value{ MakeRC<DynamicFloat>() } {}
        explicit ScriptDynamicFloat (RC<DynamicFloat> dv)               __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicFloat>  Get ()                                    C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Float2
    //
    class ScriptDynamicFloat2 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicFloat2>   _value;


    // methods
    public:
        explicit ScriptDynamicFloat2 ()                                 __Th___ : _value{ MakeRC<DynamicFloat2>() } {}
        explicit ScriptDynamicFloat2 (RC<DynamicFloat2> dv)             __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicFloat2>  Get ()                                   C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Float3
    //
    class ScriptDynamicFloat3 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicFloat3>   _value;


    // methods
    public:
        explicit ScriptDynamicFloat3 ()                                 __Th___ : _value{ MakeRC<DynamicFloat3>() } {}
        explicit ScriptDynamicFloat3 (RC<DynamicFloat3> dv)             __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicFloat3>  Get ()                                   C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic Float4
    //
    class ScriptDynamicFloat4 final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicFloat4>   _value;


    // methods
    public:
        explicit ScriptDynamicFloat4 ()                                 __Th___ : _value{ MakeRC<DynamicFloat4>() } {}
        explicit ScriptDynamicFloat4 (RC<DynamicFloat4> dv)             __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicFloat4>  Get ()                                   C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };



    //
    // Script Dynamic ULong
    //
    class ScriptDynamicULong final : public EnableScriptRC
    {
    // variables
    private:
        RC<DynamicULong>    _value;


    // methods
    public:
        explicit ScriptDynamicULong ()                                  __Th___ : _value{ MakeRC<DynamicULong>() } {}
        explicit ScriptDynamicULong (RC<DynamicULong> dv)               __NE___ : _value{RVRef(dv)} {}

        ND_ RC<DynamicULong>  Get ()                                    C_NE___ { return _value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;
    };


} // AE::ResEditor
