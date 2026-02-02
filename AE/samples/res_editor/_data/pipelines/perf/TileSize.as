// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,		"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::AllGraphics,	"un_OutBuf",	"OutBuffer",  EResourceState::ShaderStorage_RW );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		int2	ipos	= GenGridTriStrip( iGridSize );
		float3	vpos	= float3( ToSNorm(float2(ipos) / float2(iGridSize)), 0.f );

		if ( ipos.x > iGridSize )
			vpos.x = float_qnan;

		gl.Position		= float4( vpos, 1.0 );
		Out.uv			= float2(ipos);

		if ( gl.VertexIndex == 0 )
			gl.AtomicMax( INOUT un_OutBuf.vsSubgroupSize, gl.subgroup.Size );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	#define UNROLL		[[unroll]]
	#include "Noise.glsl"

	FBM_NOISE_Hash( VoronoiContour )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( IQNoise )


	int3  GetGlobalSize () {
		return int3( un_PerPass.resolution.xy, 1 );
	}

	float3  GetGlobalSizeRcp () {
		return float3( un_PerPass.invResolution, 1.0 );
	}


	void  SelectSubgroup (inout float4 color)
	{
		if ( un_PerPass.mouse.z > 0.0 )
		{
		  #ifdef AE_demote_to_helper_invocation
			bool	is_helper	= gl.IsHelperInvocation();
		  #else
			bool	is_helper	= gl.HelperInvocation;
		  #endif
			float2	pos1		= MapUNormCorrectedToDstPixCoord( un_PerPass.mouse.xy, float2(iDstDim), un_PerPass.resolution.xy );
			float2	pos2		= gl.FragCoord.xy;
			float	d			= Distance( pos1, pos2 );
			bool	selected	= d < 0.6;

			color.rgb *= 0.5;

			if ( gl.subgroup.Any( selected and !is_helper ))
				color = float4(1.0);
		}
	}


	float3  UniqueThread ()
	{
		return Rainbow( float(gl.subgroup.Index) / float(gl.subgroup.Size-1) ).rgb;
	}

	float3  UniqueSubgroup ()
	{
		float2	sum = gl.subgroup.Add( gl.FragCoord.xy );
		return	DHash32( sum * 111.1 );
	}

	float3  FullSubgroup ()
	{
		uint4	active_threads	= gl.subgroup.Ballot( true );
		uint	total_threads	= gl.subgroup.BallotBitCount( active_threads );

		return	Rainbow2( float(total_threads) / float(gl.subgroup.Size) ).rgb;
	}

	float3  RandomColor (float seed)
	{
		float3	col = DHash31( seed );
		if ( LengthSq( col ) < 0.1 )
			col = 1.0 - col;
		return col;
	}
	float3  PerPrimitiveColor ()	{ return RandomColor( float(gl.PrimitiveID) * 11.1 ); }


  #ifdef AE_NV_shader_sm_builtins
	float3  SMID ()
	{
		return Rainbow( DHash11( (float(gl.NV.SMID) / gl.NV.SMCount) * 11.1 )).rgb;
	}

	void  SelectSMID (inout float4 color)
	{
		if ( un_PerPass.mouse.z > 0.0 )
		{
			uint	id = uint(un_PerPass.mouse.x * float(gl.NV.SMCount) + 0.5);

			color.rgb *= 0.5;

			if ( id == gl.NV.SMID )
				color = float4(1.0);
		}
	}

  #elif defined(AE_ARM_shader_core_builtins)
	float3  SMID ()
	{
		return Rainbow( DHash11( (float(gl.ARM.CoreID) / gl.ARM.CoreMaxID) * 11.1 )).rgb;
	}

	void  SelectSMID (inout float4 color)
	{
		if ( un_PerPass.mouse.z > 0.0 )
		{
			uint	id = uint(un_PerPass.mouse.x * float(gl.NV.SMCount) + 0.5);

			color.rgb *= 0.5;

			if ( id == gl.NV.SMID )
				color = float4(1.0);
		}
	}

  #else
	float3  SMID () { return float3(0.0); }
	void  SelectSMID (inout float4 color) {}
  #endif


	void  Main ()
	{
		out_Color = float4(1.0);

		switch ( iDrawMode )
		{
			case 0 :	out_Color.rgb = UniqueThread();			break;
			case 1 :	out_Color.rgb = UniqueSubgroup();		break;
			case 2 :	out_Color.rgb = FullSubgroup();			break;	// blue - all threads in subgroup are active
			case 3 :	out_Color.rgb = PerPrimitiveColor();	break;
			case 4 :	out_Color.rgb = SMID();					break;
		}

		if ( AllLess( gl.FragCoord.xy, float2(8.0) ))
			gl.AtomicMax( INOUT un_OutBuf.fsSubgroupSize, gl.subgroup.Size );

		if ( iDrawMode == 4 )
			SelectSMID( INOUT out_Color );
		else
			SelectSubgroup( INOUT out_Color );

		#if defined(iOutput_0)
			// nothing

		#elif defined(iOutput_1)
			out_Color2 = float4(out_Color);

		#elif defined(iOutput_2)
			out_Color2 = float4(out_Color);
			out_Color3 = float4(out_Color);

		#elif defined(iOutput_3)
			out_Color2 = float4(out_Color);
			out_Color3 = float4(out_Color);
			out_Color4 = float4(out_Color);

		#elif defined(iOutput_4)
			out_Color2 = float4(out_Color);
			out_Color3 = float4(out_Color);
			out_Color4 = float4(out_Color);
			out_Color5 = float4(out_Color);

		#elif defined(iOutput_5)
			out_Color2 = float4(out_Color);
			out_Color3 = float4(out_Color);
			out_Color4 = float4(out_Color);
			out_Color5 = float4(out_Color);
			out_Color6 = float4(out_Color);

		#else
		#	error unsupported iOutput
		#endif


		#if defined(iRegisterCount_0)
			// nothing
			out_TriID.r = gl.PrimitiveID;

		#elif defined(iRegisterCount_1)
			float3			pos		= GetGlobalCoordSNorm() * 100.f;
			const float		lac		= 5.0;
			const float		pers	= 1.0;

			float	n = VoronoiContourFBM( pos, CreateFBMParams( lac, pers, 1 ));

			out_Color += n * 0.001;
			out_TriID.r = gl.PrimitiveID;

		#elif defined(iRegisterCount_2)
			float3			pos		= GetGlobalCoordSNorm() * 100.f;
			const float		lac		= 5.0;
			const float		pers	= 1.0;

			float	n = VoronoiContourFBM( pos, CreateFBMParams( lac, pers, 4 ));

			out_Color += n * 0.001;
			out_TriID.r = gl.PrimitiveID;

		#elif defined(iRegisterCount_3)
			float3			pos		= GetGlobalCoordSNorm() * 100.f;
			const float		lac		= 5.0;
			const float		pers	= 1.0;
			float			c		= DHash13( pos );
			float			n;

			if ( c < 0.1 )
				n = VoronoiContourFBM( pos, CreateFBMParams( lac, pers, 4 ));
			else
				n = IQNoiseFBM( pos, CreateFBMParams( lac, pers, 5 ));

			out_Color += n * 0.001;
			out_TriID.r = gl.PrimitiveID;

		#elif defined(iRegisterCount_4)
			float3			pos		= GetGlobalCoordSNorm() * 100.f;
			const float		lac		= 5.0;
			const float		pers	= 1.0;
			float			c		= DHash13( pos );
			float			n;

			if ( c < 0.3 )
				n = VoronoiContourFBM( pos, CreateFBMParams( lac, pers, 4 ));
			else
			if ( c < 0.6 )
				n = IQNoiseFBM( pos, CreateFBMParams( lac, pers, 5 ));
			else
				n = WarleyNoiseFBM( pos, CreateFBMParams( lac, pers, 6 ));

			out_Color += n * 0.001;
			out_TriID.r = gl.PrimitiveID;

		#elif defined(iRegisterCount_5)
			float3			pos		= GetGlobalCoordSNorm() * 100.f;
			const float		lac		= 5.0;
			const float		pers	= 1.0;
			float			c		= DHash13( pos );
			float			n;

			if ( c < 0.3 )
				n = VoronoiContourFBM( pos, CreateFBMParams( lac, pers, 4 ));
			else
			if ( c < 0.6 )
				n = IQNoiseFBM( pos, CreateFBMParams( lac, pers, 5 ));
			else
				n = WarleyNoiseFBM( pos, CreateFBMParams( lac, pers, 6 ));

			out_Color += n * 0.001;
			out_TriID.r = gl.PrimitiveID;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
