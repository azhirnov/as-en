// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "lang_model/Public/Types.h"

namespace AE::LangModel::LLama
{
	enum class EBackend : ubyte
	{
		CPU,		// will use CPU only backend which is slower, other backend will combine CPU with GPU or NPU
		Auto,
		Vulkan,		// may return out-of-memory error for small VRAM
		CUDA,		// can use RAM to increase memory size, but performance limited to PCI bandwidth
		Metal,
	//	SYCL,
	//	HIP,
		_Count
	};



	//
	// LLama Open Params
	//
	struct OpenParams : LangModel::OpenParams
	{
		Path			modelFile;

		Path			llamaLib;
		Path			ggmlLib;
		Path			cpuBackendLib;
		Path			gpuBackendLib;

		uint			gpuLayers			= UMax;		// max - ModelInfo::layerCount

		bool			enableLogger		= true;
		EBackend		backend				= EBackend::Auto;

		bool			keepModelInMemory	= true;		// force system to keep model in RAM
		bool			checkTensors		= true;		// validate model tensor data
		bool			useMMap				= true;		// use mmap if possible, improves load time, improve stability on low RAM
	};



	// Top-K sampling described in academic paper "The Curious Case of Neural Text Degeneration" https://arxiv.org/abs/1904.09751
	struct Sampler_TopK
	{
		int			k			= 0;
	};

	// Nucleus sampling described in academic paper "The Curious Case of Neural Text Degeneration" https://arxiv.org/abs/1904.09751
	struct Sampler_TopP
	{
		float		p			= 0.05f;
		usize		minKeep		= 1;
	};

	// Minimum P sampling as described in https://github.com/ggml-org/llama.cpp/pull/3841
	struct Sampler_MinP
	{
		float		p			= 0.05f;
		usize		minKeep		= 1;
	};

	// Locally Typical Sampling implementation described in the paper https://arxiv.org/abs/2202.00666
	struct Sampler_Typical
	{
		float		p			= 0.0f;
		usize		minKeep		= 1;
	};

	// XTC sampler as described in https://github.com/oobabooga/text-generation-webui/pull/6335
	struct Sampler_XTC
	{
		float		p			= 0.0f;
		float		t			= 0.0f;
		uint		seed		= 0;
		usize		minKeep		= 1;
	};

	// Top n sigma sampling as described in academic paper "Top-nσ: Not All Logits Are You Need" https://arxiv.org/pdf/2411.07641
	struct Sampler_TopNSigma
	{
		float		n			= 0.f;
	};

	struct Sampler_Penalties
	{
		uint		lastTokenCount	= UMax;		// max - context size
		float		repeat			= 1.f;		// 1 - disabled
		float		freq			= 0.f;		// 0 - disabled
		float		present			= 0.f;		// 0 - disabled
	};


	//
	// LLama Context Params
	//
	struct ContextParams : LangModel::ContextParams
	{
		uint				contextSize			= UMax;		// 0 - default, UMax - maximum supported (ModelInfo::maxContextSize)
		uint				threadCount			= UMax;		// 0 - default (4), UMax - maximum supported
		uint				maxSequences		= 0;		// 0 - default
		uint				evaluationBatchSize	= 0;		// 0 - default
		EFlashAttention		flashAttention		= EFlashAttention::Disabled;
		bool				extractEmbeddings	= true;
		bool				offloadKQV			= true;		// offload the KQV ops (including the KV cache) to GPU, improve performance but requires more GPU memory
		bool				opOffload			= true;		// offload host tensor operations to device

		struct {
			Optional<Sampler_TopP>		topP;
			Optional<Sampler_MinP>		minP;
			Optional<Sampler_TopK>		topK;
			Optional<Sampler_Typical>	typical;
			Optional<Sampler_XTC>		xtc;
			Optional<Sampler_TopNSigma>	topNSigma;
			Optional<Sampler_Penalties>	penalties;

			// Updates the logits l_i` = l_i/t. When t <= 0.0f, the maximum logit is kept at it's original value, the rest are set to -inf
			float						temperature	= 0.8f;
			uint						seed		= UMax;
		}							sampler;

		ContextParams () __NE___ : LangModel::ContextParams{ EImplementation::LLama } {}
	};

} // AE::LangModel::LLama


#ifdef AE_ENABLE_LOGS
namespace AE::Base
{

/*
=================================================
	ToString (EBackend)
=================================================
*/
	Nd__In StringView  ToString (LangModel::LLama::EBackend type) __NE___
	{
		switch_enum( type )
		{
			using enum LangModel::LLama::EBackend;
			case CPU :		return "CPU";
			case Auto :		return "Auto";
			case Vulkan :	return "Vulkan";
			case CUDA :		return "CUDA";
			case Metal :	return "Metal";
			case _Count :	break;
		}
		switch_end
		return Default;
	}

} // AE::Base
#endif // AE_ENABLE_LOGS
