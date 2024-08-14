// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include "tools/SphereNoise.as"

void  SetupPasses ()
{
	// default
	{
		const array<float>	params = {
			1, 							// iNoise
			2, 							// iOctaves
			0,							// iOp
			15.0000f, 					// iPScale
			0.0000f, 0.0000f, 0.0000f, 	// iPBias
			1.f, 1.f, 1.f, 1.f,			// iParams
			0.5f,						// iDScale
		};
		//AddTurbulence( params );
	}{
		const array<float>	params = {
			14, 										// iNoise
			6, 											// iOctaves
			0, 0, 										// iOp
			4.0000f, 									// iPScale
			0.0000f, 0.0000f, 0.0000f, 					// iPBias
			1.0000f, -0.2730f, 1.1210f, 0.9060f, 		// iParams
			0.9820f, 0.4020f, 							// iVScaleBias
		};
		AddNoise( params );
	}{
		const array<float>	params = {
			0, 											// iMode
			-0.2500f, 0.0000f, 0.2500f, 0.5000f, 		// iA
			0.7500f, 1.0000f, 1.2500f, 					// iB
			1.0000f, 									// iScale
			0.0000f, 									// iBias
		};
		AddSpline( params );
	}{
		displacement = 0.1f;
	}
}

void  ASmain ()
{
	SetupSphereNoise( SetupPasses );
	Present( rt );
	Export( noise_tex, "noise-sphere-.aeimg" );
}
