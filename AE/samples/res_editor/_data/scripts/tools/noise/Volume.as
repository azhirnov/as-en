// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include "tools/VolumeNoise.as"

void  SetupPasses ()
{
	// default
	{
		const array<float>	params = {
			1, 							// iNoise
			2, 							// iOctaves
			0,							// iOp
			5.0000f, 					// iPScale
			0.0000f, 0.0000f, 0.0000f, 	// iPBias
			1.f, 1.f, 1.f, 1.f,			// iParams
			0.5f,						// iDScale
			0.0f,						// iDOffset
		};
		//AddTurbulence( params );
	}{
		const array<float>	params = {
			0, 							// iNoise
			1, 							// iOctaves
			0, 0, 						// iOp
			5.0000f, 					// iPScale
			0.0000f, 0.0000f, 0.0000f, 	// iPBias
			1.f, 1.f, 1.f, 1.f,			// iParams
			1.0f, 0.0f,		 			// iVScaleBias
		};
		AddNoise( params );
	}{
		const array<float>	params = {
			0, 							// iMode
			-0.25f, 0.0f, 0.25f, 0.5f, 	// iA
			0.75f, 1.0f, 1.25f, 		// iB
			1.f,						// iScale
		};
		AddSpline( params );
	}
}

void  ASmain ()
{
	SetupVolumeNoise( SetupPasses );
	Present( rt );
	Export( noise_tex, "noise-volume-.aeimg" );
}
