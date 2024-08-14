// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Spline.glsl"

ND_ float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float p5);
ND_ float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float2 p56);
ND_ float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float3 p567);
ND_ float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float4 p5678);

ND_ float2  ApplySpline2 (float x, const int splineMode, const float4 p0123, const float4 p4567);
ND_ float2  ApplySpline2 (float x, const int splineMode, const float4 p0123, const float4 p4567, const float2 p89);

float2  ApplySpline3 (float x, const float3 xScale, const int splineMode, const float4 p0123, const float4 p4567);
float2  ApplySpline3 (float x, const float4 xScale, const int splineMode, const float4 p0123, const float4 p4567, const float2 p89);
//-----------------------------------------------------------------------------


float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float p5)
{
	const int	im	= 2;
	x *= float(im);

	int			i	= int(Floor( x ));
	const float	t	= i < 0 ? 0.f : (i < im ? Fract( x ) : 1.0f);
	float		s	= 0.f;
				i	= Clamp( i, 0, im-1 );

	float4	a;
	switch ( i )
	{
		case 0 :	a = p1234;										break;
		case 1 :	a = float4( p1234.y, p1234.z, p1234.w, p5  );	break;
	}
	switch ( splineMode )
	{
		case 0:		s = CatmullRom( a.x, a.y, a.z, a.w, t );		break;
		case 1:		s = BSpline( a.x, a.y, a.z, a.w, t );			break;
		case 2:		s = Lerp( a.y, a.z, t );						break;
	}
	s = Saturate( s );
	return float2( s, i );
}

float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float2 p56)
{
	const int	im	= 3;
	x *= float(im);

	int			i	= int(Floor( x ));
	const float	t	= i < 0 ? 0.f : (i < im ? Fract( x ) : 1.0f);
	float		s	= 0.f;
				i	= Clamp( i, 0, im-1 );

	float4	a;
	switch ( i )
	{
		case 0 :	a = p1234;											break;
		case 1 :	a = float4( p1234.y, p1234.z, p1234.w, p56.x  );	break;
		case 2 :	a = float4( p1234.z, p1234.w, p56.x,   p56.y  );	break;
	}
	switch ( splineMode )
	{
		case 0:		s = CatmullRom( a.x, a.y, a.z, a.w, t );			break;
		case 1:		s = BSpline( a.x, a.y, a.z, a.w, t );				break;
		case 2:		s = Lerp( a.y, a.z, t );							break;
	}
	s = Saturate( s );
	return float2( s, i );
}

float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float3 p567)
{
	const int	im	= 4;
	x *= float(im);

	int			i	= int(Floor( x ));
	const float	t	= i < 0 ? 0.f : (i < im ? Fract( x ) : 1.0f);
	float		s	= 0.f;
				i	= Clamp( i, 0, im-1 );

	float4	a;
	switch ( i )
	{
		case 0 :	a = p1234;											break;
		case 1 :	a = float4( p1234.y, p1234.z, p1234.w, p567.x  );	break;
		case 2 :	a = float4( p1234.z, p1234.w, p567.x,  p567.y  );	break;
		case 3 :	a = float4( p1234.w, p567.x,  p567.y,  p567.z  );	break;
	}
	switch ( splineMode )
	{
		case 0:		s = CatmullRom( a.x, a.y, a.z, a.w, t );			break;
		case 1:		s = BSpline( a.x, a.y, a.z, a.w, t );				break;
		case 2:		s = Lerp( a.y, a.z, t );							break;
	}
	s = Saturate( s );
	return float2( s, i );
}

float2  ApplySpline (float x, const int splineMode, const float4 p1234, const float4 p5678)
{
	const int	im	= 5;
	x *= float(im);

	int			i	= int(Floor( x ));
	const float	t	= i < 0 ? 0.f : (i < im ? Fract( x ) : 1.0f);
	float		s	= 0.f;
				i	= Clamp( i, 0, im-1 );

	float4	a;
	switch ( i )
	{
		case 0 :	a = p1234;											break;
		case 1 :	a = float4( p1234.y, p1234.z, p1234.w, p5678.x );	break;
		case 2 :	a = float4( p1234.z, p1234.w, p5678.x, p5678.y );	break;
		case 3 :	a = float4( p1234.w, p5678.x, p5678.y, p5678.z );	break;
		case 4 :	a = p5678;											break;
	}
	switch ( splineMode )
	{
		case 0:		s = CatmullRom( a.x, a.y, a.z, a.w, t );			break;
		case 1:		s = BSpline( a.x, a.y, a.z, a.w, t );				break;
		case 2:		s = Lerp( a.y, a.z, t );							break;
	}
	s = Saturate( s );
	return float2( s, i );
}



float2  ApplySpline2 (float x, const int splineMode, const float4 p0123, const float4 p4567)
{
	const float	p0	= p0123.x;
	const float	p1	= p0123.y;
	const float	p2	= p0123.z;
	const float	p3	= p0123.w;
	const float	p4	= p4567.x;
	const float	p5	= p4567.y;
	const float	p6	= p4567.z;
	const float	p7	= p4567.w;
	const int	im	= 3;

				x	*= float(im);
	int			i	= int(Floor( x ));
	const float	t	= i < 0 ? 0.f : (i < im ? Fract( x ) : 1.0f);
	float3		s	= float3(0.0);

	switch ( splineMode )
	{
		case 0:
		{
			s.x	= CatmullRom( p1, p2, p3, p4, t );
			s.y	= CatmullRom( p2, p3, p4, p5, t );
			s.z	= CatmullRom( p3, p4, p5, p6, t );
			break;
		}
		case 1:
		{
			s.x	= QLerp( p2, p3, t );
			s.y	= QLerp( p3, p4, t );
			s.z	= QLerp( p4, p5, t );
			break;
		}
		case 2:
		{
			s.x	= Lerp( p2, p3, t );
			s.y	= Lerp( p3, p4, t );
			s.z	= Lerp( p4, p5, t );
			break;
		}
		case 3:
		{
			s.x	= Bezier3( p1, p2, p3, t );
			s.y	= Bezier3( p3, p4, p5, t );
			s.z	= Bezier3( p5, p6, p7, t );
			break;
		}
		case 4:
		{
			s.x	= Bezier4( p0, p0, p1, p2, t );
			s.y	= Bezier4( p2, p3, p4, p5, t );
			s.z	= Bezier4( p5, p6, p7, p7, t );
			break;
		}
		case 5:
		{
			s.x	= BSpline( p1, p2, p3, p4, t );
			s.y	= BSpline( p2, p3, p4, p5, t );
			s.z	= BSpline( p3, p4, p5, p6, t );
			break;
		}
	}

	s = Saturate( s );
	i = Clamp( i, 0, im-1 );
	return float2( s[i], i );
}

float2  ApplySpline2 (float x, const int splineMode, const float4 p0123, const float4 p4567, const float2 p89)
{
	const float	p0	= p0123.x;
	const float	p1	= p0123.y;
	const float	p2	= p0123.z;
	const float	p3	= p0123.w;
	const float	p4	= p4567.x;
	const float	p5	= p4567.y;
	const float	p6	= p4567.z;
	const float	p7	= p4567.w;
	const float	p8	= p89.x;
	const float	p9	= p89.y;
	const int	im	= 4;

				x	*= float(im);
	float4		s	= float4(0.0);
	int			i	= int(Floor( x ));
	const float	t	= i < 0 ? 0.f : (i < im ? Fract( x ) : 1.0f);

	switch ( splineMode )
	{
		case 0:
		{
			s.x	= CatmullRom( p1, p2, p3, p4, t );
			s.y	= CatmullRom( p2, p3, p4, p5, t );
			s.z	= CatmullRom( p3, p4, p5, p6, t );
			s.w	= CatmullRom( p4, p5, p6, p7, t );
			break;
		}
		case 1:
		{
			s.x	= QLerp( p2, p3, t );
			s.y	= QLerp( p3, p4, t );
			s.z	= QLerp( p4, p5, t );
			s.w	= QLerp( p5, p6, t );
			break;
		}
		case 2:
		{
			s.x	= Lerp( p2, p3, t );
			s.y	= Lerp( p3, p4, t );
			s.z	= Lerp( p4, p5, t );
			s.w	= Lerp( p5, p6, t );
			break;
		}
		case 3:
		{
			s.x	= Bezier3( p1, p2, p3, t );
			s.y	= Bezier3( p3, p4, p5, t );
			s.z	= Bezier3( p5, p6, p7, t );
			s.w	= Bezier3( p7, p8, p9, t );
			break;
		}
		case 4:
		{
			s.x	= Bezier4( p0, p0, p1, p2, t );
			s.y	= Bezier4( p2, p3, p4, p5, t );
			s.z	= Bezier4( p5, p6, p7, p8, t );
			s.w	= Bezier4( p8, p9, p9, p9, t );
			break;
		}
		case 5:
		{
			s.x	= BSpline( p1, p2, p3, p4, t );
			s.y	= BSpline( p2, p3, p4, p5, t );
			s.z	= BSpline( p3, p4, p5, p6, t );
			s.w	= BSpline( p4, p5, p6, p7, t );
			break;
		}
	}

	s = Saturate( s );
	i = Clamp( i, 0, im-1 );
	return float2( s[i], i );
}


float2  ApplySpline3 (float x, const float3 xScale, const int splineMode, const float4 p0123, const float4 p4567)
{
	const float	p0	= p0123.x;
	const float	p1	= p0123.y;
	const float	p2	= p0123.z;
	const float	p3	= p0123.w;
	const float	p4	= p4567.x;
	const float	p5	= p4567.y;
	const float	p6	= p4567.z;
	const float	p7	= p4567.w;
	const int	im	= 3;

	x *= xScale.x + xScale.y + xScale.z;

	int	i = 0;
	for (; i < im; ++i)
	{
		if ( x < xScale[i] ) break;
		x -= xScale[i];
	}

	float	t = i < im ? Remap( float2(0.0, xScale[i]), float2(0.0, 1.0), x ) : 1.f;
	float3	s = float3(0.0);

	switch ( splineMode )
	{
		case 0:
		{
			s.x	= CatmullRom( p1, p2, p3, p4, t );
			s.y	= CatmullRom( p2, p3, p4, p5, t );
			s.z	= CatmullRom( p3, p4, p5, p6, t );
			break;
		}
		case 1:
		{
			s.x	= QLerp( p2, p3, t );
			s.y	= QLerp( p3, p4, t );
			s.z	= QLerp( p4, p5, t );
			break;
		}
		case 2:
		{
			s.x	= Lerp( p2, p3, t );
			s.y	= Lerp( p3, p4, t );
			s.z	= Lerp( p4, p5, t );
			break;
		}
		case 3:
		{
			s.x	= Bezier3( p1, p2, p3, t );
			s.y	= Bezier3( p3, p4, p5, t );
			s.z	= Bezier3( p5, p6, p7, t );
			break;
		}
		case 4:
		{
			s.x	= Bezier4( p0, p0, p1, p2, t );
			s.y	= Bezier4( p2, p3, p4, p5, t );
			s.z	= Bezier4( p5, p6, p7, p7, t );
			break;
		}
		case 5:
		{
			s.x	= BSpline( p1, p2, p3, p4, t );
			s.y	= BSpline( p2, p3, p4, p5, t );
			s.z	= BSpline( p3, p4, p5, p6, t );
			break;
		}
	}

	s = Saturate( s );
	i = Clamp( i, 0, im-1 );
	return float2( s[i], i );
}

float2  ApplySpline3 (float x, const float4 xScale, const int splineMode, const float4 p0123, const float4 p4567, const float2 p89)
{
	const float	p0	= p0123.x;
	const float	p1	= p0123.y;
	const float	p2	= p0123.z;
	const float	p3	= p0123.w;
	const float	p4	= p4567.x;
	const float	p5	= p4567.y;
	const float	p6	= p4567.z;
	const float	p7	= p4567.w;
	const float	p8	= p89.x;
	const float	p9	= p89.y;
	const int	im	= 4;

	x *= xScale.x + xScale.y + xScale.z + xScale.w;

	int	i = 0;
	for (; i < im; ++i)
	{
		if ( x < xScale[i] ) break;
		x -= xScale[i];
	}

	float	t = i < im ? Remap( float2(0.0, xScale[i]), float2(0.0, 1.0), x ) : 1.f;
	float4	s = float4(0.0);

	switch ( splineMode )
	{
		case 0:
		{
			s.x	= CatmullRom( p1, p2, p3, p4, t );
			s.y	= CatmullRom( p2, p3, p4, p5, t );
			s.z	= CatmullRom( p3, p4, p5, p6, t );
			s.w	= CatmullRom( p4, p5, p6, p7, t );
			break;
		}
		case 1:
		{
			s.x	= QLerp( p2, p3, t );
			s.y	= QLerp( p3, p4, t );
			s.z	= QLerp( p4, p5, t );
			s.w	= QLerp( p5, p6, t );
			break;
		}
		case 2:
		{
			s.x	= Lerp( p2, p3, t );
			s.y	= Lerp( p3, p4, t );
			s.z	= Lerp( p4, p5, t );
			s.w	= Lerp( p5, p6, t );
			break;
		}
		case 3:
		{
			s.x	= Bezier3( p1, p2, p3, t );
			s.y	= Bezier3( p3, p4, p5, t );
			s.z	= Bezier3( p5, p6, p7, t );
			s.w	= Bezier3( p7, p8, p9, t );
			break;
		}
		case 4:
		{
			s.x	= Bezier4( p0, p0, p1, p2, t );
			s.y	= Bezier4( p2, p3, p4, p5, t );
			s.z	= Bezier4( p5, p6, p7, p8, t );
			s.w	= Bezier4( p8, p9, p9, p9, t );
			break;
		}
		case 5:
		{
			s.x	= BSpline( p1, p2, p3, p4, t );
			s.y	= BSpline( p2, p3, p4, p5, t );
			s.z	= BSpline( p3, p4, p5, p6, t );
			s.w	= BSpline( p4, p5, p6, p7, t );
			break;
		}
	}

	s = Saturate( s );
	i = Clamp( i, 0, im-1 );
	return float2( s[i], i );
}
