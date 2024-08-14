/*
	based on code from GLM (MIT license) https://github.com/g-truc/glm
*/

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
