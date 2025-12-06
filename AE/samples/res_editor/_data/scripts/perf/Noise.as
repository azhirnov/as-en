// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MODE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		octaves	= DynamicUInt();
		RC<DynamicUInt>		count	= DynamicUInt();
		RC<DynamicUInt>		tex_pot	= DynamicUInt();
		RC<DynamicDim>		tex_dim	= tex_pot.Exp2().Dimension2();
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, tex_dim );
		RC<DynamicUInt>		mode	= DynamicUInt();
		array<string>		mode_arr = {
			"GradientNoise", "ValueNoise", "PerlinNoise", "SimplexNoise", "IQNoise", "WaveletNoise",
			"Voronoi2D", "Voronoi3D", "VoronoiContour2D", "VoronoiContour3D", "VoronoiContourSparse2D",
			"VoronoiContourSparse3D", "VoronoiCircles", "WarleyNoise", "GradientNoiseFBM", "ValueNoiseFBM",
			"PerlinNoiseFBM", "SimplexNoiseFBM", "VoronoiFBM", "WarleyNoiseFBM", "IQNoiseFBM",
			"VoronoiContourFBM", "VoronoiContourSparseFBM"
		};

		Slider( tex_pot,	"TexDim",	1,	12,		4 );
		Slider( count,		"Repeat",	1,	16 );
		Slider( octaves,	"Octaves",	0,	16,		4 );
		Slider( mode,		"Noise",	0,	mode_arr.size()-1 );

		Label( tex_dim.XY(), "Dimension" );

		// render loop
		for (uint i = 0; i < mode_arr.size(); ++i)
		{
			RC<Postprocess>		pass = Postprocess( "", "MODE=M"+mode_arr[i] );
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Constant( "iOctaves",	octaves );
			pass.Repeat( count );
			pass.EnableIfEqual( mode, i );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Noise.glsl"
	#include "InvocationID.glsl"

	#define MGradientNoise				1
	#define MValueNoise					2
	#define MPerlinNoise				3
	#define MSimplexNoise				4
	#define MIQNoise					5
	#define MWaveletNoise				6
	#define MVoronoi2D					7
	#define MVoronoi3D					8
	#define MVoronoiContour2D			9
	#define MVoronoiContour3D			10
	#define MVoronoiContourSparse2D		11
	#define MVoronoiContourSparse3D		12
	#define MVoronoiCircles				13
	#define MWarleyNoise				14
	#define MGradientNoiseFBM			15
	#define MValueNoiseFBM				16
	#define MPerlinNoiseFBM				17
	#define MSimplexNoiseFBM			18
	#define MVoronoiFBM					19
	#define MWarleyNoiseFBM				20
	#define MIQNoiseFBM					21
	#define MVoronoiContourFBM			22
	#define MVoronoiContourSparseFBM	23

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )
	FBM_NOISE_Hash( IQNoise )
	FBM_NOISE_Hash( Voronoi )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( VoronoiContour )
	FBM_NOISE_Hash( VoronoiContourSparse )

	void Main ()
	{
		const float			lac		= 5.0;
		const float			pers	= 1.0;
		float3				pos		= GetGlobalCoordSNorm() * 100.f;
		float				n		= 0.f;
		const FBMParams		fbm		= CreateFBMParams( lac, pers, iOctaves );

		#if MODE == MGradientNoise
			n = GradientNoise( pos );

		#elif MODE == MValueNoise
			n = ValueNoise( pos );

		#elif MODE == MPerlinNoise
			n = PerlinNoise( pos );

		#elif MODE == MSimplexNoise
			n = SimplexNoise( pos );

		#elif MODE == MIQNoise
			n = IQNoise( pos );

		#elif MODE == MWaveletNoise
			n = WaveletNoise( pos.xy );

		#elif MODE == MVoronoi2D
			n = Voronoi( pos.xy );

		#elif MODE == MVoronoi3D
			n = Voronoi( pos );

		#elif MODE == MVoronoiContour2D
			n = VoronoiContour( pos.xy );

		#elif MODE == MVoronoiContour3D
			n = VoronoiContour( pos );

		#elif MODE == MVoronoiContourSparse2D
			n = VoronoiContourSparse( pos.xy );

		#elif MODE == MVoronoiContourSparse3D
			n = VoronoiContourSparse( pos );

		#elif MODE == MVoronoiCircles
			n = VoronoiCircles( pos.xy, 0.5 );

		#elif MODE == MWarleyNoise
			n = WarleyNoise( pos );

		#elif MODE == MGradientNoiseFBM
			n = GradientNoiseFBM( pos, fbm );

		#elif MODE == MValueNoiseFBM
			n = ValueNoiseFBM( pos, fbm );

		#elif MODE == MPerlinNoiseFBM
			n = PerlinNoiseFBM( pos, fbm );

		#elif MODE == MSimplexNoiseFBM
			n = SimplexNoiseFBM( pos, fbm );

		#elif MODE == MVoronoiFBM
			n = VoronoiFBM( pos, fbm );

		#elif MODE == MWarleyNoiseFBM
			n = WarleyNoiseFBM( pos, fbm );

		#elif MODE == MIQNoiseFBM
			n = IQNoiseFBM( pos, fbm );

		#elif MODE == MVoronoiContourFBM
			n = VoronoiContourFBM( pos, fbm );

		#elif MODE == MVoronoiContourSparseFBM
			n = VoronoiContourSparseFBM( pos, fbm );

		#else
		#	error !!!
		#endif

		out_Color = float4(n) * 0.001;
	}

#endif
//-----------------------------------------------------------------------------
