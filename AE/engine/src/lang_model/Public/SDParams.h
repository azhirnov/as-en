// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "lang_model/Public/Types.h"

namespace AE::LangModel::SD
{

	enum class EModelFile : uint
	{
		TextToImageLLM,		// with LLM will better understand prompt
		VAE,
		ClipL,
		T5_XXL,
		DiffusionModel,
	};


	enum class EFlags
	{
		OffloadParamsToCPU,
		KeepClipOnCPU,
		VAE_DecodeOnly,

	/*	KeepControlNetOnCPU,
		keepVAEonCPU,
		DiffusionFlashAttn,
		TAE_PreviewOnly,
		DiffusionConvDirect,
		VAE_ConvDirect,
		CircularX,
		CircularY,
		ForceSdxlVaeConvScale,
		ChromaUseDitMask,
		ChromaUseT5Mask,
		QwenImageZeroCondT,

		EnableMMap,
		*/
		_Count
	};


	enum class ESampleMethod : ubyte
	{
		Euler,
		EulerA,
		Heun,
		DPM2,
		DPMPP2S_A,
		DPMPP2M,
		DPMPP2Mv2,
		IPNDM,
		IPNDM_V,
		LCM,
		DDIM_Trailing,
		TCD,
		_Count
	};



	//
	// Stable Diffusion Open Params
	//
	struct OpenParams : LangModel::OpenParams
	{
	// types
		using Models_t	= Array<Pair< EModelFile, Path >>;
		using FlagBits	= EnumSet< EFlags >;

	// variables
		Path			sdLib;
		Models_t		modelFiles;

		uint			threadCount			= 0;
		FlagBits		flags;
		EBackend		backend				= EBackend::Auto;

	// methods
		OpenParams ()					__NE___ {}
		OpenParams (OpenParams &&)		__NE___ = default;
		OpenParams (const OpenParams &)	__NE___ = default;
	};



	//
	// Image Sample Params
	//
	struct SampleParams
	{
		ESampleMethod		method		= ESampleMethod::Euler;
		uint				steps		= 1;		// denoising steps
	};


	struct SampleGuidance
	{
		float				texCfg		= 1.f;		// how strongly the model follows the text prompt
	};


	//
	// Image Generation Params
	//
	struct ImageGenParams : LangModel::ImageGenParams
	{
		SampleParams		sample;
		SampleGuidance		guidance;

	// methods
		ImageGenParams ()						__NE___ : LangModel::ImageGenParams{ EImplementation::StableDiffusion } {}
		ImageGenParams (ImageGenParams &&)		__NE___ = default;
		ImageGenParams (const ImageGenParams &)	__NE___ = default;
	};



	//
	// Video Generation Params
	//
	struct VideoGenParams : LangModel::VideoGenParams
	{
	// methods
		VideoGenParams ()						__NE___ : LangModel::VideoGenParams{ EImplementation::StableDiffusion } {}
		VideoGenParams (VideoGenParams &&)		__NE___ = default;
		VideoGenParams (const VideoGenParams &)	__NE___ = default;
	};


} // AE::LangModel::SD
