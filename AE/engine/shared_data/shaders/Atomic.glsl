// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Additional atomic functions.

    Warning: only for storage buffer and image!
*/

#include "Math.glsl"

ND_ uint   AtomicF_FloatToUint (float fvalue);
ND_ float  AtomicF_UintToFloat (uint uvalue);

// float (macros)
#if 0
    void   AtomicF_Add  (inout uint mem, const float value);
    void   AtomicF_Add2 (inout uint mem, const float value, out float oldValue);

    void   AtomicF_ImageAdd  (layout(r32ui) uimage2D image, const int2 coord, const float value);
    void   AtomicF_ImageAdd2 (layout(r32ui) uimage2D image, const int2 coord, const float value, out float oldValue);

ND_ float  AtomicF_Exchange (inout uint mem, const float value);

ND_ float  AtomicF_Max  (inout uint mem, const float value);
#endif

//-----------------------------------------------------------------------------


/*
=================================================
    AtomicF_FloatToUint
----
    converts float bits to uint and transform it to correct compare operations.
=================================================
*/
uint  AtomicF_FloatToUint (float fvalue)
{
    const uint  sign_bit    = 1u << 31;
    uint        uvalue      = floatBitsToUint( fvalue );

    return (uvalue & sign_bit) == 0 ?
                (uvalue | sign_bit) :
                ~uvalue;
}

/*
=================================================
    AtomicF_UintToFloat
----
    converts uint with transformed float bits back to float.
=================================================
*/
float  AtomicF_UintToFloat (uint uvalue)
{
    const uint  sign_bit = 1u << 31;
    uvalue = (uvalue & sign_bit) == 0 ?
                ~uvalue :
                (uvalue & ~sign_bit);
    return uintBitsToFloat( uvalue );
}

/*
=================================================
    AtomicF_Add
=================================================
*/
#define AtomicF_Add2( _mem_, _value_, _oldValue_ )                                                      \
{                                                                                                       \
    uint    aaa_expected    = 0;                                                                        \
    uint    aaa_old_value   = 0;                                                                        \
    float   aaa_val         = (_value_);                                                                \
                                                                                                        \
    do {                                                                                                \
        uint    aaa_new_value = AtomicF_FloatToUint( AtomicF_UintToFloat( aaa_old_value ) + aaa_val );  \
        aaa_expected  = aaa_old_value;                                                                  \
        aaa_old_value = atomicCompSwap( INOUT (_mem_), aaa_expected, aaa_new_value );                   \
    }                                                                                                   \
    while( aaa_old_value != aaa_expected );                                                             \
                                                                                                        \
    (_oldValue_) = AtomicF_UintToFloat( aaa_old_value );                                                \
}

#define AtomicF_Add( _mem_, _value_ )                                                                   \
{                                                                                                       \
    uint    aaa_expected    = 0;                                                                        \
    uint    aaa_old_value   = 0;                                                                        \
    float   aaa_val         = (_value_);                                                                \
                                                                                                        \
    do {                                                                                                \
        uint    aaa_new_value = AtomicF_FloatToUint( AtomicF_UintToFloat( aaa_old_value ) + aaa_val );  \
        aaa_expected  = aaa_old_value;                                                                  \
        aaa_old_value = atomicCompSwap( INOUT (_mem_), aaa_expected, aaa_new_value );                   \
    }                                                                                                   \
    while( aaa_old_value != aaa_expected );                                                             \
}

/*
=================================================
    AtomicF_Exchange
=================================================
*/
#define AtomicF_Exchange( _mem_, _value_ )                                                              \
    AtomicF_UintToFloat( atomicExchange( (_mem_), AtomicF_FloatToUint(_value_) ))

/*
=================================================
    AtomicF_ImageAdd
=================================================
*/
#define AtomicF_ImageAdd2( _image_, _coord_, _value_, _oldValue_ )                                      \
{                                                                                                       \
    uint    aaa_expected    = 0;                                                                        \
    uint    aaa_old_value   = 0;                                                                        \
    float   aaa_val         = (_value_);                                                                \
    int2    aaa_coord       = (_coord_);                                                                \
                                                                                                        \
    do {                                                                                                \
        uint    aaa_new_value = AtomicF_FloatToUint( AtomicF_UintToFloat( aaa_old_value ) + aaa_val );  \
        aaa_expected  = aaa_old_value;                                                                  \
        aaa_old_value = imageAtomicCompSwap( (_image_), aaa_coord, aaa_expected, aaa_new_value );       \
    }                                                                                                   \
    while( aaa_old_value != aaa_expected );                                                             \
                                                                                                        \
    (_oldValue_) = AtomicF_UintToFloat( aaa_old_value );                                                \
}

#define AtomicF_ImageAdd( _image_, _coord_, _value_ )                                                   \
{                                                                                                       \
    uint    aaa_expected    = 0;                                                                        \
    uint    aaa_old_value   = 0;                                                                        \
    float   aaa_val         = (_value_);                                                                \
    int2    aaa_coord       = (_coord_);                                                                \
                                                                                                        \
    do {                                                                                                \
        uint    aaa_new_value = AtomicF_FloatToUint( AtomicF_UintToFloat( aaa_old_value ) + aaa_val );  \
        aaa_expected  = aaa_old_value;                                                                  \
        aaa_old_value = imageAtomicCompSwap( (_image_), aaa_coord, aaa_expected, aaa_new_value );       \
    }                                                                                                   \
    while( aaa_old_value != aaa_expected );                                                             \
}

/*
=================================================
    AtomicF_Max
=================================================
*/
#define AtomicF_Max( _mem_, _value_ )                                                                   \
    AtomicF_UintToFloat( atomicMax( INOUT (_mem_), AtomicF_FloatToUint( _value_ )))
