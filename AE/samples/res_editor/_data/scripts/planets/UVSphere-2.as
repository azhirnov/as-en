// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Quad decals on sphere.

	- Generate random dots on sphere.
	- Render to cubemap, in GS dot projected to the face and constructed quad.
	- Bug: geometry doesn't match between faces, it causes a incorrect UV interpolation and other. Only identity projection is correct.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_DOTS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>		scene			= Scene();
		RC<Image>		cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(1024), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>		cubemap_view	= cubemap.CreateView( EImage::Cube );
		RC<Buffer>		buf				= Buffer();
		RC<Buffer>		cbuf			= Buffer();
		const uint2		buf_dim			= uint2( 1 );
		const uint2		local_size		= Min( buf_dim, uint2(8) );

		buf.ArrayLayout(
			"Dots",
			"float4		pos_r;",
			buf_dim.x * buf_dim.y * 6
		);

		// setup render to cubemap
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = buf.ConstArraySize();
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		{
			const float4x4	proj = float4x4().InfinitePerspective( ToRad(90.0), 1.0, 0.1 );

			cbuf.Float( "proj",		proj );
		}

		RC<DynamicInt>	proj_type = DynamicInt();
		Slider( proj_type, "iProj",  0, 5,  1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_DOTS" );
			pass.ArgOut( "un_OutBuf",	buf );
			pass.LocalSize( local_size );
			pass.DispatchThreads( buf_dim.x, buf_dim.y, 6 );
			pass.Slider( "iHash",	0,				3 );
			pass.Slider( "iHScale",	float3(0.f),	float3(100.f),	float3(30.f) );
			pass.Slider( "iAScale",	0.f,			1.f,			0.47f );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw to cubemap" );
			draw.AddPipeline( "sphere/UVSphere-2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/UVSphere-2.as)
			draw.Output(	"out_Color",	cubemap,	RGBA32f(0.1, 0.1, 0.4, 0.0) );
			draw.ArgIn(		"un_Dots",		buf );
			draw.ArgIn(		"un_CBuf",		cbuf );
			draw.Constant(	"iProj",		proj_type );
			draw.Slider(	"iRadius",		0.02f,	0.9f,	0.4f );
			draw.Slider(	"iMinZ",		0.0f,	0.7f,	0.6f );
			draw.Slider(	"iDbgFace",		0,		5 );
			draw.Slider(	"iView",		0,		3 );

			GenMipmaps( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( OrbitalCamera() );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",	cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iScale",		0.25f,			1.1f,		1.1f );
			pass.Slider( "iFov",		0.f,			90.f,		0.f );
			pass.Slider( "iRadius",		0.0f,			0.9f,		0.5f );	// used to check circle distortion
			pass.Constant( "iProj",		proj_type );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"

	float3  Project (float3 n)
	{
		switch ( iProj )
		{
			case 0 :	n = CM_IdentitySC_Inverse( n );		break;
			case 1 :	n = CM_TangentialSC_Inverse( n );	break;
			case 2 :	n = CM_EverittSC_Inverse( n );		break;
			case 3 :	n = CM_5thPolySC_Inverse( n );		break;
			case 4 :	n = CM_COBE_SC_Inverse( n );		break;
			case 5 :	n = CM_ArvoSC_Inverse( n );			break;
		}
		return CM_IdentitySC_Forward( n.xy, ECubeFace(n.z) );
	}

	void  Main ()
	{
		float2	uv		= GetGlobalCoordSNormCorrected2() * iScale;
		float4	norm	= UVtoSphereNormal( uv, ToRad(iFov) );
		float3	uvw		= norm.xyz;

		uvw = float3x3(un_PerPass.camera.view) * uvw;
		uvw = Project( uvw );

		out_Color = gl.texture.Sample( un_CubeMap, uvw );

		if ( out_Color.a < 0.1 )
			out_Color.rgb = Rainbow( CM_InverseRotation( uvw ).w / 5.0 ).rgb * 0.5;

		out_Color.rgb *= SmoothStep( norm.w, 0.0, 0.01 );

		// screen space circle
		{
			float	d  = SDF2_Circle( uv, 0.23 * iRadius );
					d  = SDF_OpSub( d, SDF2_Circle( uv, 0.2 * iRadius ));

			float	a  = SmoothStep( -d*4.0/iRadius, 0.02, 0.025 );
			float	b  = SmoothStep( -d*2.5/iRadius, 0.02, 0.025 );

			out_Color.rgb *= (1.0 - a);
			out_Color.rgb = Lerp( out_Color.rgb, float3(0.8, 0.0, 1.0), b );
		}

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_DOTS
	#include "Hash.glsl"
	#include "CubeMap.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"

	#define PROJECTION  CM_TangentialSC_Forward

	void Main ()
	{
		float3		pos		= PROJECTION( GetGlobalCoordSNorm().xy, GetGlobalCoord().z );
		float3		angle	= float3(0.0);

		switch ( iHash )
		{
			case 0 :
				angle = DHash33( pos * iHScale );	// bad distribution
				break;

			case 1 :
				angle = HEHash33( uint3(Abs( pos * iHScale )) );
				break;

			case 2 :
				angle.x = Hash_Uniform( pos.xy * iHScale.xy, iHScale.x + 0.11f );
				angle.y = Hash_Uniform( pos.yz * iHScale.zy, iHScale.y + 0.22f );
				angle.z = Hash_Uniform( pos.zx * iHScale.xz, iHScale.z + 0.33f );
				break;

			case 3 :
				angle.x = Hash_Triangular( pos.xy * iHScale.xy, iHScale.x + 0.11f );
				angle.y = Hash_Triangular( pos.yz * iHScale.zy, iHScale.y + 0.22f );
				angle.z = Hash_Triangular( pos.zx * iHScale.xz, iHScale.z + 0.33f );
				break;
		}

		angle *= float_Pi * iAScale;

		pos = QMul( QRotationX( angle.x ), pos );
		pos = QMul( QRotationY( angle.y ), pos );
		pos = QMul( QRotationZ( angle.z ), pos );

		un_OutBuf.elements[ GetGlobalIndex() ].pos_r = float4( Normalize( pos ), -1.f );
	}

#endif
//-----------------------------------------------------------------------------
