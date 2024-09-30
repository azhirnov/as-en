

/*
=================================================
	FresnelDielectric
----
	Dielectric-dielectric interface, non-metal material.
	'eta'		- index of refraction for I - incoming, T - transmitted ray.
	'cosThetaI'	- cos of angle between incident and normal, must be in range [0,1].
	Returns amount of light reflected from a surface.
----
	based on code from PBRTv3 (BSD2 license) https://pbr-book.org/
=================================================
*/
#ifdef AE_LICENSE_BSD2

float  FresnelDielectric (const float cosThetaI, const float eta)
{
	float	sin_theta_t_sq	= (eta * eta) * (1.0f - cosThetaI * cosThetaI);	// Snell law
	float	cos_theta_t		= Sqrt( Max( 0.f, 1.0f - sin_theta_t_sq ));

	float	Rp	=	(eta * cos_theta_t - cosThetaI) /
					(eta * cos_theta_t + cosThetaI);	// parallel polarization

	float	Rs	=	(eta * cosThetaI - cos_theta_t) /
					(eta * cosThetaI + cos_theta_t);	// perpendicular polarization

	return sin_theta_t_sq >= 1.0f ?
				1.0f :				// total internal reflection
				0.5f * (Rp * Rp + Rs * Rs);
}

#endif

/*
=================================================
	FresnelDielectric
----
	Dielectric-dielectric interface, non-metal material.
	'eta'		- index of refraction for I - incoming, T - transmitted ray.
	'cosThetaI'	- cos of angle between incident and normal,
				  [0,1] - ray is on the outside, [-1,0] - ray is on the inside.
	Returns amount of light reflected from a surface.
----
	from PBRTv3 (BSD2 license) https://pbr-book.org/
=================================================
*/
#ifdef AE_LICENSE_BSD2

float  FresnelDielectric (float cosThetaI, float etaI, float etaT)
{
	if ( cosThetaI <= 0.0f )
	{
		Swap( INOUT etaI, INOUT etaT );
		cosThetaI = -cosThetaI;
	}
	return FresnelDielectric( Saturate( cosThetaI ), etaI / etaT );
}

#endif

/*
=================================================
	FresnelDielectricConductor
----
	Dielectric-conductor interface, metal material.
	'eta'	- index of refraction for I - incoming, T - transmitted ray.
	'k'		- absorption coefficient.
	Returns amount of light reflected from a surface.
----
	from PBRTv3 (BSD2 license) https://pbr-book.org/
	from https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
=================================================
*/
#ifdef AE_LICENSE_BSD2

float  FresnelDielectricConductor (float cosThetaI, const float etaI, const float etaT, const float k)
{
			cosThetaI		= Clamp( cosThetaI, -1.f, 1.f );
	float	eta				= etaT / etaI;
	float	eta_k			= k / etaI;

	float	cos_theta_sq	= cosThetaI * cosThetaI;
	float	sin_theta_sq	= 1.f - cos_theta_sq;
	float	eta_sq			= eta * eta;
	float	eta_k_sq		= eta_k * eta_k;

	float	t0				= eta_sq - eta_k_sq - sin_theta_sq;
	float	a2plusb2		= Sqrt( t0 * t0 + 4.f * eta_sq * eta_k_sq );
	float	t1				= a2plusb2 + cos_theta_sq;
	float	a				= Sqrt( 0.5f * (a2plusb2 + t0) );
	float	t2				= 2.f * cosThetaI * a;
	float	Rs				= (t1 - t2) / (t1 + t2);

	float	t3				= cos_theta_sq * a2plusb2 + sin_theta_sq * sin_theta_sq;
	float	t4				= t2 * sin_theta_sq;
	float	Rp				= Rs * (t3 - t4) / (t3 + t4);

	return 0.5f * (Rp + Rs);
}

#endif

/*
=================================================
	FresnelAirConductorApprox
----
	Air-conductor interface, metal material.
	'eta'	- index of refraction for I - incoming, T - transmitted ray.
	'k'		- absorption coefficient.
	Returns amount of light reflected from a surface.
----
	from https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
=================================================
*/
#ifdef AE_ENABLE_UNKNOWN_LICENSE

float  FresnelDielectricConductorApprox (float cosThetaI, const float etaI, const float etaT, const float k)
{
			cosThetaI		= Clamp( cosThetaI, -1.f, 1.f );
	float	eta				= etaT / etaI;
	float	eta_k			= k / etaI;

	float	cos_theta_sq	= cosThetaI * cosThetaI;
	float	eta_cos_theta_2	= 2.f * eta * cosThetaI;

	float	t0				= eta * eta + eta_k * eta_k;
	float	t1				= t0 * cos_theta_sq;
	float	Rs				= (t0 - eta_cos_theta_2 + cos_theta_sq) / (t0 + eta_cos_theta_2 + cos_theta_sq);
	float	Rp				= (t1 - eta_cos_theta_2 + 1.f) / (t1 + eta_cos_theta_2 + 1.f);

	return 0.5f * (Rp + Rs);
}

#endif
