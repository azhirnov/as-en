// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void CreatePipeline (string samplerName)
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "imgui.io" );
			st.Set( EStructLayout::InternalIO,
					"float4		color;"
					"float2		uv;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "imgui.vs.pc" );
			st.Set( EStructLayout::Compatible_Std140,
					"float2		transform_c0;"
					"float2		transform_c1;"
					"float2		transform_c2;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "imgui.fs.pc" );
			st.Set( EStructLayout::Compatible_Std140,
					"uint		texIndex;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "imgui.ds" );
			ds.CombinedImage( EShaderStages::Fragment, "un_Textures", ArraySize(16), EImageType::Float_2D, samplerName );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "imgui.pl" );
			pl.PushConst( "imguiVSpc", "imgui.vs.pc", EShader::Vertex );
			pl.PushConst( "imguiFSpc", "imgui.fs.pc", EShader::Fragment );
			pl.DSLayout( 0, "imgui.ds" );
		}
		{
			RC<ShaderStructType>	st = ShaderStructType( "imgui.vertex" );
			st.Set( EStructLayout::InternalIO,
					"packed_float2		Position;"	+
					"packed_float2		UV;"		+
					"packed_ubyte_norm4	Color;"		);

			RC<VertexBufferInput>	vb = VertexBufferInput( "VB.imgui.vertex" );
			vb.Add( "Vertex",	"imgui.vertex" );
		}


		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "imgui.draw1" );
		ppln.SetLayout( "imgui.pl" );
		ppln.SetVertexInput( "VB.imgui.vertex" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "imgui.io" );

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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "imgui" );
			spec.AddToRenderTech( "ImGui.RTech", "Main" );

			RenderState	rs;
			{
				RenderState_ColorBuffer		cb;
				cb.SrcBlendFactor( EBlendFactor::SrcAlpha );
				cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );
				cb.BlendOp( EBlendOp::Add );
				rs.color.SetColorBuffer( 0, cb );
			}
			rs.depth.test = false;

			rs.inputAssembly.topology = EPrimitive::TriangleList;

			rs.rasterization.cullMode = ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

	void CreatePipeline ()
	{
		CreatePipeline( "LinearRepeat" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		float3x2 m	= float3x2( imguiVSpc.transform_c0, imguiVSpc.transform_c1, imguiVSpc.transform_c2 );
		gl.Position	= float4(m * float3(in_Position, 1.0), 0.0, 1.0 );
		Out.uv		= in_UV;
		Out.color	= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	layout(location=0) out float4  out_Color;

	void Main ()
	{
		out_Color = In.color * gl.texture.Sample( un_Textures[imguiFSpc.texIndex], In.uv );
	}

#endif
//-----------------------------------------------------------------------------
