// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include "tools/TerrainNoise.as"

void  SetupPasses ()
{
	{
		const array<float>	params = {
			1, 										// iNoise
			2, 										// iOctaves
			0, 										// iOp
			15.0000f, 								// iPScale
			0.0000f, 0.0000f, 0.0000f, 				// iPBias
			1.0000f, 1.0000f, 1.0000f, 1.0000f, 	// iParams
			0.7050f, 								// iDScale
			0.0f,									// iDOffset
		};
		AddTurbulence( params );
	}{
		const array<float>	params = {
			10, 									// iNoise
			1, 										// iOctaves
			5, 0, 									// iOp
			7.2360f, 								// iPScale
			0.0000f, 0.0000f, 0.0000f, 				// iPBias
			0.7860f, 0.2760f, 1.0000f, 1.0000f, 	// iParams
			1.0000f, 0.0000f, 						// iVScaleBias
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
	}
}

void  ASmain ()
{
	SetupTerrainNoise( SetupPasses );
	Present( rt );
	Export( noise_tex, "noise-terrain-.aeimg" );
}
