// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Float16.h"
#include "base/Math/Vec.h"
#include "base/Math/Rectangle.h"

namespace AE::Math
{

/*
=================================================
    unorm16 -> float
=================================================
*/
    ND_ inline constexpr float  UNormShortToFloat (ushort value) __NE___
    {
        return float(value) * 1.5259021896696421759365224689097e-5f;
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<float,I,Q>  UNormShortToFloat (const TVec<ushort,I,Q> &value) __NE___
    {
        TVec<float,I,Q>     res;
        for (int i = 0; i < I; ++i) {
            res[i] = UNormShortToFloat( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<float>  UNormShortToFloat (const Rectangle<ushort> &value) __NE___
    {
        return Rectangle<float>{ UNormShortToFloat( value.left ),   UNormShortToFloat( value.top ),
                                 UNormShortToFloat( value.right ),  UNormShortToFloat( value.bottom )};
    }

/*
=================================================
    float -> unorm16
=================================================
*/
    ND_ inline constexpr ushort  FloatToUNormShort (float value) __NE___
    {
        return ushort(value * 65535.0f);
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<ushort,I,Q>  FloatToUNormShort (const TVec<float,I,Q> &value) __NE___
    {
        TVec<ushort,I,Q>        res;
        for (int i = 0; i < I; ++i) {
            res[i] = FloatToUNormShort( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<ushort>  FloatToUNormShort (const Rectangle<float> &value) __NE___
    {
        return Rectangle<ushort>{ FloatToUNormShort( value.left ),  FloatToUNormShort( value.top ),
                                  FloatToUNormShort( value.right ), FloatToUNormShort( value.bottom )};
    }

/*
=================================================
    snorm16 -> float
=================================================
*/
    ND_ inline constexpr float  SNormShortToFloat (sshort value) __NE___
    {
        return float(value) * 3.0518509475997192297128208258309e-5f;
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<float,I,Q>  SNormShortToFloat (const TVec<sshort,I,Q> &value) __NE___
    {
        TVec<float,I,Q>     res;
        for (int i = 0; i < I; ++i) {
            res[i] = SNormShortToFloat( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<float>  SNormShortToFloat (const Rectangle<sshort> &value) __NE___
    {
        return Rectangle<float>{ SNormShortToFloat( value.left ),   SNormShortToFloat( value.top ),
                                 SNormShortToFloat( value.right ),  SNormShortToFloat( value.bottom )};
    }

/*
=================================================
    float -> snorm16
=================================================
*/
    ND_ inline constexpr sshort  FloatToSNormShort (float value) __NE___
    {
        return sshort(value * 32767.0f);
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<sshort,I,Q>  FloatToSNormShort (const TVec<float,I,Q> &value) __NE___
    {
        TVec<sshort,I,Q>        res;
        for (int i = 0; i < I; ++i) {
            res[i] = FloatToSNormShort( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<sshort>  FloatToSNormShort (const Rectangle<float> &value) __NE___
    {
        return Rectangle<sshort>{ FloatToSNormShort( value.left ),  FloatToSNormShort( value.top ),
                                  FloatToSNormShort( value.right ), FloatToSNormShort( value.bottom )};
    }

/*
=================================================
    unorm8 -> float
=================================================
*/
    ND_ inline constexpr float  UNormByteToFloat (ubyte value) __NE___
    {
        return float(value) * 0.0039215686274509803921568627451f;
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<float,I,Q>  UNormByteToFloat (const TVec<ubyte,I,Q> &value) __NE___
    {
        TVec<float,I,Q>     res;
        for (int i = 0; i < I; ++i) {
            res[i] = UNormByteToFloat( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<float>  UNormByteToFloat (const Rectangle<ubyte> &value) __NE___
    {
        return Rectangle<float>{ UNormByteToFloat( value.left ),    UNormByteToFloat( value.top ),
                                 UNormByteToFloat( value.right ),   UNormByteToFloat( value.bottom )};
    }

/*
=================================================
    float -> unorm8
=================================================
*/
    ND_ inline constexpr ubyte  FloatToUNormByte (float value) __NE___
    {
        return ubyte(value * 255.0f);
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<ubyte,I,Q>  FloatToUNormByte (const TVec<float,I,Q> &value) __NE___
    {
        TVec<ubyte,I,Q>     res;
        for (int i = 0; i < I; ++i) {
            res[i] = FloatToUNormByte( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<ubyte>  FloatToUNormByte (const Rectangle<float> &value) __NE___
    {
        return Rectangle<ubyte>{ FloatToUNormByte( value.left ),    FloatToUNormByte( value.top ),
                                 FloatToUNormByte( value.right ),   FloatToUNormByte( value.bottom )};
    }

/*
=================================================
    snorm8 -> float
=================================================
*/
    ND_ inline constexpr float  SNormByteToFloat (sbyte value) __NE___
    {
        return float(value) * 0.0078740157480315f;
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<float,I,Q>  SNormByteToFloat (const TVec<sbyte,I,Q> &value) __NE___
    {
        TVec<float,I,Q>     res;
        for (int i = 0; i < I; ++i) {
            res[i] = SNormByteToFloat( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<float>  SNormByteToFloat (const Rectangle<sbyte> &value) __NE___
    {
        return Rectangle<float>{ SNormByteToFloat( value.left ),    SNormByteToFloat( value.top ),
                                 SNormByteToFloat( value.right ),   SNormByteToFloat( value.bottom )};
    }

/*
=================================================
    float -> snorm8
=================================================
*/
    ND_ inline constexpr sbyte  FloatToSNormByte (float value) __NE___
    {
        return sbyte(value * 127.0f);
    }

    template <int I, glm::qualifier Q>
    ND_ TVec<sbyte,I,Q>  FloatToSNormByte (const TVec<float,I,Q> &value) __NE___
    {
        TVec<sbyte,I,Q>     res;
        for (int i = 0; i < I; ++i) {
            res[i] = FloatToSNormByte( value[i] );
        }
        return res;
    }

    ND_ inline constexpr Rectangle<sbyte>  FloatToSNormByte (const Rectangle<float> &value) __NE___
    {
        return Rectangle<sbyte>{ FloatToSNormByte( value.left ),    FloatToSNormByte( value.top ),
                                 FloatToSNormByte( value.right ),   FloatToSNormByte( value.bottom )};
    }
//-----------------------------------------------------------------------------



/*
=================================================
    unorm16x2 / snorm16x2 <-> uint
=================================================
*/
    template <glm::qualifier Q>
    ND_ uint  UNorm16x2ToUint (const TVec<float,2,Q> &value) __NE___
    {
        return glm::packUnorm2x16( value );
    }

    template <glm::qualifier Q>
    ND_ uint  SNorm16x2ToUint (const TVec<float,2,Q> &value) __NE___
    {
        return glm::packUnorm2x16( value );
    }

    ND_ inline float2  UIntToUNorm16x2 (uint value) __NE___
    {
        return glm::unpackUnorm2x16( value );
    }

    ND_ inline float2  UIntToSNorm16x2 (uint value) __NE___
    {
        return glm::unpackSnorm2x16( value );
    }

/*
=================================================
    unorm8x4 / snorm8x4 <-> uint
=================================================
*/
    template <glm::qualifier Q>
    ND_ uint  UNorm8x4ToUint (const TVec<float,4,Q> &value) __NE___
    {
        return glm::packUnorm4x8( value );
    }

    template <glm::qualifier Q>
    ND_ uint  SNorm8x4ToUint (const TVec<float,4,Q> &value) __NE___
    {
        return glm::packUnorm4x8( value );
    }

    ND_ inline float4  UIntToUNorm8x4 (uint value) __NE___
    {
        return glm::unpackUnorm4x8( value );
    }

    ND_ inline float4  UIntToSNorm8x4 (uint value) __NE___
    {
        return glm::unpackSnorm4x8( value );
    }

/*
=================================================
    float16x2 <-> uint
=================================================
*/
    template <glm::qualifier Q>
    ND_ uint  Float16x2ToUint (const TVec<float,2,Q> &value) __NE___
    {
        return glm::packHalf2x16( value );
    }

    ND_ inline float2  UIntToFloat16x2 (uint value) __NE___
    {
        return glm::unpackHalf2x16( value );
    }


} // AE::Math
