// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Implement UInt64 type using 'uint2'.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

#ifdef __cplusplus
	using U64pack = uint2;
#else
#	define U64pack	uint2
#endif

ND_ U64pack  Add64 (U64pack a, U64pack b);
ND_ U64pack  Sub64 (U64pack a, U64pack b);
ND_ U64pack  Min64 (U64pack a, U64pack b);
ND_ U64pack  Max64 (U64pack a, U64pack b);
ND_ float	 U64PackToFloat (U64pack a);

#if AE_ENABLE_LONG_TYPE
ND_ ulong	 U64PackToULong (U64pack a);
ND_ U64pack	 ULongToPacked (ulong a);
#endif
#if AE_ENABLE_DOUBLE_TYPE
ND_ double	 U64PackToDouble (U64pack a);
#endif
//-----------------------------------------------------------------------------

#define U64PACK_OPT	1

/*
=================================================
	Add64
=================================================
*/
U64pack  Add64 (U64pack a, U64pack b)
{
	U64pack	result = a + b;
	result.y += uint(result.x < a.x);
	return result;
}

/*
=================================================
	Sub64
=================================================
*/
U64pack  Sub64 (U64pack a, U64pack b)
{
	U64pack	result = a - b;
	result.y -= uint(a.x < b.x);
	return result;
}

/*
=================================================
	Min64
=================================================
*/
U64pack  Min64 (U64pack a, U64pack b)
{
#if U64PACK_OPT

	if ( a.y < b.y ) return a;
	if ( a.y > b.y ) return b;
	if ( a.x < b.x ) return a;
	return b;
#else

	bool2	cmp		 = Less( a, b );
	bool2	eq		 = Equal( a, b );
	bool	select_a = cmp.y or (eq.y and cmp.x);
	return	select_a ? a : b;
#endif
}

/*
=================================================
	Max64
=================================================
*/
U64pack  Max64 (U64pack a, U64pack b)
{
#if U64PACK_OPT

	if ( a.y > b.y ) return a;
	if ( a.y < b.y ) return b;
	if ( a.x > b.x ) return a;
	return b;
#else

	bool2	cmp		 = Greater( a, b );
	bool2	eq		 = Equal( a, b );
	bool	select_a = cmp.y or (eq.y and cmp.x);
	return	select_a ? a : b;
#endif
}

/*
=================================================
	U64PackToFloat
=================================================
*/
float  U64PackToFloat (U64pack a)
{
	const float HIGH_SCALE = 4294967296.0;	// Exp2(32.0)
	return float(a.y) * HIGH_SCALE + float(a.x);
}

/*
=================================================
	U64PackToULong / ULongToPacked
=================================================
*/
#if AE_ENABLE_LONG_TYPE
	ulong  U64PackToULong (U64pack a)
	{
		return ulong(a.x) | (ulong(a.y) << 32);
	}

	U64pack	 ULongToPacked (ulong a)
	{
		return U64pack( uint(a & 0xFFFFFFFF), uint(a >> 32) );
	}
#endif


/*
=================================================
	U64PackToDouble
=================================================
*/
#if AE_ENABLE_DOUBLE_TYPE
	double  U64PackToDouble (U64pack a)
	{
	#if AE_ENABLE_LONG_TYPE
		return double(U64PackToULong( a ));
	#else
		const double HIGH_SCALE = 4294967296.0;	// Exp2(32.0)
		return double(a.y) * HIGH_SCALE + double(a.x);
	#endif
	}
#endif

#undef U64PACK_OPT
