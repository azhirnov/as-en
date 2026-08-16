// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "lang_model/Public/Types.h"

namespace AE::LangModel::LLama
{

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

		bool			keepModelInMemory	= true;		// force system to keep model in RAM
		bool			checkTensors		= true;		// validate model tensor data
		bool			useMMap				= true;		// use mmap if possible, improves load time, improve stability on low RAM

		OpenParams ()					__NE___ {}
		OpenParams (OpenParams &&)		__NE___ = default;
		OpenParams (const OpenParams &)	__NE___ = default;
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
