// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>


void CreateRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "UIPass" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::SwapchainColor;
		rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>	rp = compat.AddSpecialization( "UIPass" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Load;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
}


void  CreateRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "ImGui.RTech" );
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );
		pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
	}
}


void  CreatePipeline ()
{
	// samplers
	{
		RC<Sampler>		samp = Sampler( "imgui.LinearRepeat" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
	}

	{
		RC<ShaderStructType>	st = ShaderStructType( "imgui.io" );
		st.Set( EStructLayout::InternalIO,
				"float4		color;" +
				"float2		uv;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "imgui.ub" );
		st.Set( EStructLayout::Compatible_Std140,
				"float2		transform_c0;" +
				"float2		transform_c1;" +
				"float2		transform_c2;" );

		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "imgui.ds" );
		ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::Float_2D, "imgui.LinearRepeat" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "imgui.pl" );
		pl.PushConst( "imguiUB", "imgui.ub", EShader::Vertex );
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
		vs.file		= "imgui.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/ui_editor/_ui_data/shaders/imgui.glsl)
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs = Shader();
		fs.file		= "imgui.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/ui_editor/_ui_data/shaders/imgui.glsl)
		fs.options	= EShaderOpt::Optimize;
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


void ASmain ()
{
	CreateRenderPass();
	CreateRenderTech();
	CreatePipeline();
}
