/*
	based on code from GLM (MIT license) https://github.com/g-truc/glm
*/

#ifdef AE_LICENSE_MIT


float  QuadraticEaseIn (const float x)
{
	return x * x;
}

float  QuadraticEaseOut (const float x)
{
	return -x * (x - 2.0f);
}

float  QuadraticEaseInOut (const float x)
{
	return	x < 0.5f ?
				(2.f * x * x) :
				(-2.f * x * x) + (4.f * x) - 1.f;
}
//------------------------------------------------


float  CubicEaseIn (const float x)
{
	return x * x * x;
}

float  CubicEaseOut (float x)
{
	x = x - 1.f;
	return x * x * x + 1.f;
}

float  CubicEaseInOut (const float x)
{
	float	a = 2.f * x - 2.f;
	return	x < 0.5f ?
				(4.f * x * x * x) :
				0.5f * a * a * a + 1.f;
}
//------------------------------------------------


float  QuarticEaseIn (const float x)
{
	return x * x * x * x;
}

float  QuarticEaseOut (const float x)
{
	const float	a = x - 1.f;
	return a * a * a * (1.f - x) + 1.f;
}

float  QuarticEaseInOut (const float x)
{
	const float	a = x - 1.f;
	return	x < 0.5f ?
				(8.f * x * x * x * x) :
				(-8.f * a * a * a * a + 1.f);
}
//------------------------------------------------


float  QuinticEaseIn (const float x)
{
	return x * x * x * x * x;
}

float  QuinticEaseOut (const float x)
{
	const float	a = x - 1.f;
	return a * a * a * a * a + 1.f;
}

float  QuinticEaseInOut (const float x)
{
	const float	a = 2.f * x - 2.f;
	return	x < 0.5f ?
				(16.f * x * x * x * x * x) :
				(0.5f * a * a * a * a * a + 1.f);
}
//------------------------------------------------


float  SineEaseIn (const float x)
{
	return Sin( (x - 1.f) * float_HalfPi ) + 1.f;
}

float  SineEaseOut (const float x)
{
	return Sin( x * float_HalfPi );
}

float  SineEaseInOut (const float x)
{
	return 0.5f * (1.f - Cos( x * float_Pi ));
}
//------------------------------------------------


float  CircularEaseIn (const float x)
{
	return 1.f - Sqrt( 1.f - (x*x) );
}

float  CircularEaseOut (const float x)
{
	return Sqrt( (2.f - x) * x );
}

float  CircularEaseInOut (const float x)
{
	return	x < 0.5f ?
				0.5f * (1.f - Sqrt( 1.f - 4.f * x*x )) :
				0.5f * (Sqrt( -(2.f * x - 3.f) * ((2.f * x) - 1.f)) + 1.f);
}
//------------------------------------------------


float  ExponentialEaseIn (const float x)
{
	return Exp2( (x - 1.f) * 10.f );
}

float  ExponentialEaseOut (const float x)
{
	return 1.f - Exp2( -10.f * x );
}

float  ExponentialEaseInOut (const float x)
{
	return	x < 0.5f ?
				0.5f * Exp2( 20.f * x - 10.f ) :
				-0.5f * Exp2( (-20.f * x) + 10.f ) + 1.f;
}
//------------------------------------------------


float  ElasticEaseIn (const float x)
{
	return Sin( 13.f * float_HalfPi * x ) * Exp2( 10.f * (x - 1.f) );
}

float  ElasticEaseOut (const float x)
{
	return Sin( -13.f * float_HalfPi * (x + 1.f) ) * Exp2( -10.f * x ) + 1.f;
}

float  ElasticEaseInOut (const float x)
{
	return	x < 0.5f ?
				0.5f * Sin( 13.f * float_Pi * x ) * Exp2( 10.f * (2.f * x - 1.f) ) :
				0.5f * (Sin( -13.f * float_Pi * x ) * Exp2( -10.f * (2.f * x - 1.f)) + 2.f);
}
//------------------------------------------------

#endif // AE_LICENSE_MIT
