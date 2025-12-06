// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Test sampler reduction mode.

	For non-power of 2 images it lost data.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define FILL
#	define GEN_MIPMAP
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
	//	RC<DynamicDim>	dim = SurfaceSize();
		RC<DynamicDim>	dim = DynamicDim( uint3(1324, 1024, 1) );

		RC<Image>		mipmaps		= Image( EPixelFormat::RGBA8_UNorm, dim, MipmapLevel(~0) );	mipmaps.Name( "Mipmapped" );
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, dim );					rt.Name( "RT" );
		RC<DynamicUInt>	raster		= DynamicUInt();
		RC<DynamicUInt>	reduction	= DynamicUInt();
		RC<DynamicUInt>	mode		= raster.Add( reduction.Mul(2) );

		if ( GetFeatureSet().hasSamplerFilterMinmax() )
			Slider( reduction,	"UseReduction",		0,	1 );

		Slider( raster,	"Gfx",	0,	1 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "FILL" );
			pass.Output( "out_Color",	mipmaps );
			pass.Slider( "iMinValPos",	float2(0.0),	float2(1.0),	float2(0.5) );
		}

		{
			RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP" );
			pass.Variable( "un_InImage",	"un_OutImage",	mipmaps,	Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<RasterMip>		pass = RasterMip( "", "GEN_MIPMAP" );
			pass.Variable( "un_InImage",	"out_Color",	mipmaps,	Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 1 );
		}

		if ( GetFeatureSet().hasSamplerFilterMinmax() )
		{
			{
				RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP; USE_REDUCTION" );
				pass.Variable( "un_InImage",	"un_OutImage",	mipmaps,	Sampler_MinLinearClamp );
				pass.EnableIfEqual( mode, 2 );
			}{
				RC<RasterMip>		pass = RasterMip( "", "GEN_MIPMAP; USE_REDUCTION" );
				pass.Variable( "un_InImage",	"out_Color",	mipmaps,	Sampler_MinLinearClamp );
				pass.EnableIfEqual( mode, 3 );
			}
		}

		{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Image",	mipmaps,	Sampler_NearestClamp );
			pass.Slider( "iMip",		0,		12,		4 );
			pass.Slider( "iIntUV",		0,		1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FILL
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		int2	pos = int2(iMinValPos * un_PerPass.resolution.xy);

		if ( AllEqual( pos, int2(gl.FragCoord.xy) ))
			out_Color = float4(0.0);
		else
			out_Color = float4(1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MIPMAP
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4	c;

		#ifdef USE_REDUCTION
		{
			float2	uv = (float2(GetGlobalCoord().xy) + 0.5) * iInvResolution;
			c = gl.texture.SampleLod( un_InImage, uv, 0.0 );
		}
		#else
		{
			int2	p = GetGlobalCoord().xy * 2;
			c = gl.texture.Fetch( un_InImage, p, 0 );
			c = Min( c, gl.texture.Fetch( un_InImage, p + int2(1,0), 0 ));
			c = Min( c, gl.texture.Fetch( un_InImage, p + int2(0,1), 0 ));
			c = Min( c, gl.texture.Fetch( un_InImage, p + int2(1,1), 0 ));
		}
		#endif

		#ifdef SH_FRAG
			out_Color = c;
		#else
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "InvocationID.glsl"

	void  Main ()
	{
		int		max_mip	= gl.texture.QueryLevels( un_Image );
		if ( iMip >= max_mip )
		{
			out_Color = float4(1.0, 0.0, 0.0, 1.0);
			return;
		}

		float2	uv		= GetGlobalCoordUNorm().xy;
		float2	uv2		= GetGlobalCoordUNorm( int3(1) ).xy;
		float2	dim		= gl.texture.GetSize( un_Image, iMip );
		int2	coord	= int2(uv * dim);
		int2	coord2	= int2(uv2 * dim);

		out_Color = gl.texture.Fetch( un_Image, coord, iMip );

		if ( iIntUV == 1 ){
			out_Color.b = float(coord.x);
			out_Color.a = float(coord.y);
		}else{
			out_Color.b = float(coord.x) / dim.x + 1.0;
			out_Color.a = float(coord.y) / dim.y + 1.0;
		}

		if ( NotAllEqual( coord, coord2 ))
			out_Color = float4(0.4);
	}

#endif
//-----------------------------------------------------------------------------
