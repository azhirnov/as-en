// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#define AE_TEST_PIPELINE_COMPILER

#include "res_pack/pipeline_compiler/ScriptObjects/DescriptorSetLayout.h"
#include "res_pack/pipeline_compiler/ScriptObjects/PipelineLayout.h"
#include "res_pack/pipeline_compiler/ScriptObjects/RenderTechnique.h"
#include "res_pack/pipeline_compiler/ScriptObjects/GraphicsPipeline.h"
#include "res_pack/pipeline_compiler/ScriptObjects/ComputePipeline.h"

#include "res_pack/pipeline_compiler/ScriptObjects/ScriptRenderPass.h"
#include "res_pack/pipeline_compiler/ScriptObjects/RenderTechnique.h"

#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

namespace AE::ResEditor
{
	using namespace AE::Base;
	using AE::PipelineCompiler::EImageType;

/*
=================================================
	GetDescriptorImageType
=================================================
*/
	ND_ inline EImageType  GetDescriptorImageType (const Graphics::EPixelFormat fmt, const Graphics::EImage type, bool multisampling)
	{
		return	PipelineCompiler::EImageType_FromPixelFormat( fmt ) |
				PipelineCompiler::EImageType_FromImage( type, multisampling );
	}

	ND_ inline EImageType  GetDescriptorImageType (const Graphics::ImageDesc &desc)
	{
		CHECK_ERR( desc.imageDim != Default );
		CHECK_ERR( desc.format != Default );

		Graphics::ImageViewDesc	view{desc};
		view.Validate( desc );

		return GetDescriptorImageType( desc.format, view.viewType, desc.samples.IsEnabled() );
	}

	ND_ inline EImageType  GetDescriptorImageType (const Graphics::ImageDesc &desc, const Graphics::ImageViewDesc &view)
	{
		CHECK_ERR( desc.imageDim != Default );
		CHECK_ERR( view.format != Default );

		return GetDescriptorImageType( view.format, view.viewType, desc.samples.IsEnabled() );
	}

/*
=================================================
	GetDescriptorImageTypeRelaxed
=================================================
*/
	ND_ inline EImageType  GetDescriptorImageTypeRelaxed (const Graphics::EPixelFormat fmt, const Graphics::EImage type, Bool multisampling, Bool cubemap)
	{
		return	PipelineCompiler::EImageType_FromPixelFormatRelaxed( fmt ) |
				PipelineCompiler::EImageType_FromImage( type, multisampling, cubemap );
	}

	ND_ inline Tuple< EImageType, EImageType >  GetDescriptorImageTypeRelaxed (const Graphics::ImageDesc &desc)
	{
		CHECK_ERR( desc.imageDim != Default );
		CHECK_ERR( desc.format != Default );

		Graphics::ImageViewDesc	view {desc};
		view.Validate( desc );

		auto	t0 = GetDescriptorImageTypeRelaxed( desc.format, view.viewType, Bool{desc.samples.IsEnabled()}, False{} );
		auto	t1 = t0;

		if ( AllBits( desc.options, Graphics::EImageOpt::CubeCompatible ))
		{
			view.viewType = desc.arrayLayers.Get() > 6 ? Graphics::EImage::CubeArray : Graphics::EImage::Cube;

			t1 = GetDescriptorImageTypeRelaxed( desc.format, view.viewType, Bool{desc.samples.IsEnabled()}, True{} );
		}
		return Tuple{ t0, t1 };
	}


} // AE::ResEditor
