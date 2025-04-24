
#ifdef AE_LICENSE_MIT

	// paper https://jcgt.org/published/0002/02/05/paper.pdf
	// source from https://github.com/zeux/niagara/blob/master/src/shaders/math.h
	// MIT license
	//
	// P00 - projection matrix [0][0]
	// P11 - projection matrix [1][1]
	
	float4  ProjectSphere (float3 center, float radius, float P00, float P11)
	{
		float3	cr		= center * radius;
		float	czr2	= center.z * center.z - radius * radius;

		float	vx		= Sqrt( center.x * center.x + czr2 );
		float	minx	= (vx * center.x - cr.z) / (vx * center.z + cr.x);
		float	maxx	= (vx * center.x + cr.z) / (vx * center.z - cr.x);

		float	vy		= Sqrt( center.y * center.y + czr2 );
		float	miny	= (vy * center.y - cr.z) / (vy * center.z + cr.y);
		float	maxy	= (vy * center.y + cr.z) / (vy * center.z - cr.y);

		return float4( minx * P00, miny * P11, maxx * P00, maxy * P11 );
	}

#endif
