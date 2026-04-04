// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../tests/shared/UnitTest_Shared.h"
#include "pch/LangModel.h"

#include "res_loaders/AllImages/AllImageSavers.h"
#include "res_loaders/Intermediate/IntermImage.h"

using namespace AE::LangModel;

namespace
{
	static void  SaveImages (IImageSequence &seq, const Path &dstFolder)
	{
		using namespace AE::Graphics;
		using namespace AE::ResLoader;

		FileSystem::CreateDirectories( dstFolder );

		AllImageSavers	saver;
		IntermImage		interm;

		const auto	desc = seq.GetDescription();

		TEST( interm.Allocate( EImage_2D, desc.format, uint3{desc.dim, 1u} ));

		auto	mipmaps = interm.GetMutableData();
		TEST( mipmaps );

		for (uint i = 0; i < desc.count; ++i)
		{
			ImageRef	img;
			TEST( seq.GetImage( i, OUT img ));
			TEST( (*mipmaps)[0][0].SetPixelDataRef( img.pixels ));

			const Path	path = dstFolder / ("image-"s << ToString(i) << ".png");
			TEST( saver.SaveImage( path, interm ));

			AE_LOGI( "Save image: "s << ToString(path) );
		}
	}


	static RC<IDiffusionModel>  OpenModel ()
	{
		SD::OpenParams	params;
		params.backend		= EBackend::Vulkan;
		params.enableLogger	= true;

		params.flags	|= SD::EFlags::OffloadParamsToCPU;
		params.flags	|= SD::EFlags::KeepClipOnCPU;

		// put path to downloaded model
		// qwen-image: https://huggingface.co/unsloth/Qwen-Image-2512-GGUF
		// flux.1-schnell: https://huggingface.co/Green-Sky/flux.1-schnell-GGUF

		params.modelFiles.emplace_back( SD::EModelFile::TextToImageLLM, "(qwen-image-2512-Q4_K_M.gguf)" );
		params.modelFiles.emplace_back( SD::EModelFile::VAE,			"(ae-f16.gguf)" );
		params.modelFiles.emplace_back( SD::EModelFile::ClipL,			"(clip_l-f16.gguf)" );
		params.modelFiles.emplace_back( SD::EModelFile::T5_XXL,			"(t5xxl_q8_0.gguf)" );
		params.modelFiles.emplace_back( SD::EModelFile::DiffusionModel,	"(flux1-schnell-q4_k.gguf)" );

		auto	model = DMFactory::CreateStableDiffusion( params );
		TEST( model );

		return model;
	}


	static void  SD_Test1 (const Path &dstFolder)
	{
		// TODO: library crashed on second 'Generate()', so model reloaded for each call, try new version...

		const uint2		dim {1024};

		{
			RC<IDiffusionModel>	model = OpenModel();

			SD::ImageGenParams	params;
			params.dim				= dim;
			params.prompt			= "landscape with rocks and river in realistic style";

			params.seed				= Random{}.Uniform<uint>();
			params.resultCount		= 5;

			params.sample.steps		= 4;
			params.sample.method	= SD::ESampleMethod::Euler;

			auto	images = model->Generate( params );
			TEST( images );

			SaveImages( *images, dstFolder / "test1" );
		}

		ImageStorage	init_img1;
		{
			RC<IDiffusionModel>	model = OpenModel();

			SD::ImageGenParams	params;
			params.dim				= dim;
			params.prompt			= "old city in painting style";

			params.seed				= Random{}.Uniform<uint>();
			params.resultCount		= 1;

			params.sample.steps		= 4;
			params.sample.method	= SD::ESampleMethod::Euler;

			auto	images = model->Generate( params );
			TEST( images );

			SaveImages( *images, dstFolder / "test2" );

			TEST( images->GetImage( 0, OUT init_img1 ));
		}

		{
			RC<IDiffusionModel>	model = OpenModel();

			SD::ImageGenParams	params;
			params.dim				= dim;
			params.prompt			= "convert to realistic style and add more details";

			params.seed				= Random{}.Uniform<uint>();
			params.resultCount		= 5;

			params.sample.steps		= 4;
			params.sample.method	= SD::ESampleMethod::Euler;

			params.initImage		= init_img1;
			params.initImageStrength = 0.5f;

			auto	images = model->Generate( params );
			TEST( images );

			SaveImages( *images, dstFolder / "test3" );
		}

		{
			RC<IDiffusionModel>	model = OpenModel();

			SD::ImageGenParams	params;
			params.dim				= dim;
			params.prompt			= "convert to futuristic city, add neon, fliying cars";

			params.seed				= Random{}.Uniform<uint>();
			params.resultCount		= 5;

			params.sample.steps		= 20;
			params.sample.method	= SD::ESampleMethod::EulerA;
			params.guidance.texCfg	= 4.f;

			params.initImage		= init_img1;
			params.initImageStrength = 0.5f;

			auto	images = model->Generate( params );
			TEST( images );

			SaveImages( *images, dstFolder / "test4" );
		}
	}
}


extern void  UnitTest_StableDiffusion (const Path &dstFolder)
{
	SD_Test1( dstFolder );

	TEST_PASSED();
}
