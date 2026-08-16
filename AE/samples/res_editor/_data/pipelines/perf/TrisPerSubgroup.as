// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string name, string define, bool triStrip)
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl"+name );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			vs.Define( define );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			fs.Define( define );
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= triStrip ? EPrimitive::TriangleStrip : EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float2		uv;"
					"uint		inst;"
					"uint		instPerVsSg;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.CombinedImage( EShaderStages::Fragment,	"un_Noise",		EImageType::Float_2D, Sampler_LinearMipmapRepeat );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		CreatePipeline( "WithInstancing",		"INSTANCING=1",	false );
		CreatePipeline( "WithoutInstancing",	"INSTANCING=0",	true );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	uint  InstancesPerSubgroup ()
	{
		// same as [Histogram](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/compute/Histogram-2.as)

		uint	count = 0;
		for (; count < gl.subgroup.Size; ++count)
		{
			sg_uniform uint  id = gl.subgroup.BroadcastFirst( gl.InstanceIndex );

			if ( id == gl.InstanceIndex )
				break;
		}

		gl.subgroup.Barrier();

		uint	max_count = gl.subgroup.Max( count );
		return	max_count;
	}


	void Main ()
	{
	  #if INSTANCING
		int		tri_idx	= gl.VertexIndex / 3;
		int		v_idx	= gl.VertexIndex % 3;
		int2	ipos	= int2( tri_idx % iGridSize, tri_idx / iGridSize );
		int2	ioff	= int2( (v_idx & 2) >> 1, v_idx & 1 );	// 0..1

		if ( gl.InstanceIndex > 0 )
			ioff = 1 - ioff;

		ipos += ioff;

	  #else
		int2	ipos	= GenGridTriStrip( iGridSize );
	  #endif

		float3	vpos	= float3( ToSNorm(float2(ipos) / float2(iGridSize)), 0.f );
	  #if INSTANCING == 0
		if ( ipos.x > iGridSize )
			vpos.x = float_qnan;
	  #endif

		gl.Position		= float4( vpos, 1.0 );
		Out.uv			= float2(ipos);

	  #if INSTANCING
		Out.inst		= gl.InstanceIndex;
		Out.instPerVsSg	= InstancesPerSubgroup();
	  #else
		Out.inst		= 0;
		Out.instPerVsSg	= 0;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "CodeTemplates.glsl"
	#include "InvocationID.glsl"


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


	float3  RandomColor (float seed)
	{
		float3	col = DHash31( seed );
		if ( LengthSq( col ) < 0.1 )
			col = 1.0 - col;
		return col;
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


	float3  FullActiveInSubgroup ()
	{
		bool	is_helper = false;
		#ifdef AE_demote_to_helper_invocation
			is_helper = gl.IsHelperInvocation();
		#else
			is_helper = gl.HelperInvocation;
		#endif

		uint4	active_threads	= gl.subgroup.Ballot( ! is_helper );
		uint	total_threads	= gl.subgroup.BallotBitCount( active_threads );

		return	Rainbow2( float(total_threads) / float(gl.subgroup.Size) ).rgb;
	}


	float4  TrisPerSubgroup ()
	{
		// same as [Histogram](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/compute/Histogram-2.as)

		uint	count = 0;
		for (; count < gl.subgroup.Size; ++count)
		{
			sg_uniform int  id = gl.subgroup.BroadcastFirst( gl.PrimitiveID );

			if ( id == gl.PrimitiveID )
				break;
		}

		gl.subgroup.Barrier();

		uint	max_count = gl.subgroup.Max( count );

		const float	max_tris = 4.0;
		return	float4( Rainbow2( 1.0 - Saturate( float(max_count) / max_tris )).rgb, float(max_count) );
	}


	// blue  - instances are NOT merged
	// red   - instances are merged
	// green - instances merged in VS too
	float4  InstancesPerSubgroup ()
	{
		// same as [Histogram](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/compute/Histogram-2.as)

		uint	count = 0;
		for (; count < gl.subgroup.Size; ++count)
		{
			sg_uniform uint  id = gl.subgroup.BroadcastFirst( In.inst );

			if ( id == In.inst )
				break;
		}

		gl.subgroup.Barrier();

		uint	max_count = gl.subgroup.Max( count );
		#if 0
			return	float4( float(max_count != 0), 0.0, float(max_count == 0), float(max_count+1) );
		#elif 0
			return	float4( Rainbow2( 1.0 - Saturate( float(In.instPerVsSg) / 4.0 )).rgb, float(In.instPerVsSg+1) );
		#else
			return	float4( float(max_count != 0), float(In.instPerVsSg > 0) * 0.5, float(max_count == 0), float(max_count+1) );
		#endif
	}


	float3  PerPrimitiveColor ()	{ return RandomColor( float(gl.PrimitiveID) * 11.1 ); }
	float3  PerInstanceColor ()		{ return RandomColor( float(In.inst) * 22.2 + 100.0 ); }


	void Main ()
	{
		// enable helper invocations on some devices
		out_Color = gl.texture.Sample( un_Noise, gl.FragCoord.xy * un_PerPass.invResolution );

		switch ( iDrawMode )
		{
			case 0 :	// solid color
			{
			  #if INSTANCING
				out_Color.rgb = PerInstanceColor();
			  #else
				out_Color.rgb = PerPrimitiveColor();
			  #endif
				break;
			}

			case 1 :	// blue - all threads in subgroup are active
			{
				out_Color.rgb = FullSubgroup();
				break;
			}

			case 2 :	// blue - single triangle per subgroup, red - greater than 4 tris
			{
			  #if INSTANCING
				out_Color = InstancesPerSubgroup();
			  #else
				out_Color = TrisPerSubgroup();
			  #endif
				break;
			}

			case 3 :
			{
				out_Color.rgb = UniqueThread();
				break;
			}

			case 4 :
			{
				out_Color.rgb = UniqueSubgroup();
				break;
			}
		}

		SelectSubgroup( INOUT out_Color );

	  #if INSTANCING
		out_TriID.r = gl.PrimitiveID + In.inst * (iGridSize * iGridSize / 2);
	  #else
		out_TriID.r = gl.PrimitiveID;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
