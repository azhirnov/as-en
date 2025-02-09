// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>


void InstructionBenchmark ()
{
	// render pass
	{
		RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "InstBench.CompatRP" );

		const string	pass = "Main";
		compat.AddSubpass( pass );

		{
			RC<Attachment>	rt	= compat.AddAttachment( "Color" );
			rt.format	= EPixelFormat::RGBA8_UNorm;
			rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
		}

		// specialization
		{
			RC<RenderPass>		rp = compat.AddSpecialization( "InstBench.RP" );

			RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
			rt.loadOp	= EAttachmentLoadOp::Clear;
			rt.storeOp	= EAttachmentStoreOp::Store;
			rt.Layout( pass, EResourceState::ColorAttachment );
		}
	}

	{
		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "InstBenchRT.ds" );
		ds.StorageImage( EShaderStages::Compute, "un_Image", EImageType_2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		RC<PipelineLayout>		pl = PipelineLayout( "InstBenchRT.pl" );
		pl.DSLayout( 0, "InstBenchRT.ds" );
	}

	// render technique
	const array<string>	perf = { "High", "Low" };
	const array<string>	fs	 = { "", "ShaderInt8", "ShaderInt16", "ShaderInt64", "ShaderFloat16",
								 "ShaderFloat64", "ShaderFloatInt16", "ShaderFloatInt64" };

	for (uint j = 0; j < perf.size(); ++j)
	for (uint i = 0; i < fs.size(); ++i)
	{
		string	name = "InstBenchRT." + perf[j];
		if ( fs[i].length() > 0 )
			name += "."+fs[i];

		RC<RenderTechnique>	rtech = RenderTechnique( name );

		if ( fs[i].length() > 0 )
			rtech.AddFeatureSet( fs[i] );

		{
			RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics" );
			pass.SetRenderPass( "InstBench.RP", /*subpass*/"Main" );
		}{
			RC<ComputePass>		pass = rtech.AddComputePass( "Compute" );
			pass.SetDSLayout( "InstBenchRT.ds" );
		}
	}
}


void  NaNTest ()
{
	// render pass
	{
		RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "NaN.CompatRP" );

		const string	pass = "Main";
		compat.AddSubpass( pass );

		{
			RC<Attachment>	rt	= compat.AddAttachment( "Color" );
			rt.format	= EPixelFormat::RG32U;
			rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
		}

		// specialization
		{
			RC<RenderPass>		rp = compat.AddSpecialization( "NaN.RP" );

			RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
			rt.loadOp	= EAttachmentLoadOp::Clear;
			rt.storeOp	= EAttachmentStoreOp::Store;
			rt.Layout( pass, EResourceState::ColorAttachment );
		}
	}

	// render technique
	const array<string>	fs	 = { "", "ShaderFloat16", "ShaderFloat64", "ShaderFloatInt16", "ShaderFloatInt64" };

	for (uint i = 0; i < fs.size(); ++i)
	{
		string	name = "NaN.RT";
		if ( fs[i].length() > 0 )
			name += "."+fs[i];

		RC<RenderTechnique>	rtech = RenderTechnique( name );
		{
			RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics" );
			pass.SetRenderPass( "NaN.RP", /*subpass*/"Main" );
		}
	}
}


void ASmain ()
{
	InstructionBenchmark();
	NaNTest();
}
