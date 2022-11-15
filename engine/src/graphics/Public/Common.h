// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/Math.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Math/BitMath.h"
#include "base/Math/Math.h"
#include "base/Math/Vec.h"
#include "base/Math/Color.h"
#include "base/Math/Rectangle.h"
#include "base/Math/POTValue.h"
#include "base/Utils/RefCounter.h"
#include "base/Containers/Union.h"
#include "base/Containers/FixedMap.h"
#include "base/Containers/FixedSet.h"
#include "base/DataSource/Stream.h"

#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/TaskSystem/Promise.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Graphics
{
	using namespace AE::Base;
	using namespace AE::Math;
	
	using AE::Threading::Promise;
	using AE::Threading::Atomic;
	using AE::Threading::BytesAtomic;
	using AE::Threading::DeferExLock;
	using AE::Threading::DeferSharedLock;
	using AE::Threading::EMemoryOrder;
	using AE::Threading::ThreadFence;


	//
	// Graphics Config
	//
	struct GraphicsConfig final : Noninstancable
	{
		// buffer
		static constexpr uint	MaxVertexBuffers		= 8;
		static constexpr uint	MaxVertexAttribs		= 16;

		// render pass
		static constexpr uint	MaxViewports			= 8;
		static constexpr uint	MaxColorAttachments		= 8;
		static constexpr uint	MaxAttachments			= MaxColorAttachments + 1;	// color + depth_stencil
		static constexpr uint	MaxSamples				= 32;
		static constexpr uint	MaxMultiViews			= 2;
		static constexpr uint	MaxSubpasses			= 8;
		static constexpr uint	MaxSubpassDeps			= MaxSubpasses * 2;
			
		// pipeline
		static constexpr uint	MaxDescriptorSets		= 8;
		static constexpr uint	MaxPushConstants		= 8;
		static constexpr uint	MaxSpecConstants		= 8;

		static constexpr uint	MinFrames				= 2;
		static constexpr uint	MaxFrames				= 4;
		static constexpr uint	MaxCmdBuffersPerPool	= 16;
		static constexpr uint	MaxCmdPoolsPerQueue		= 8;	// == max render threads

		static constexpr uint	MaxCmdBufPerBatch		= 32;

		//static constexpr Bytes	MinBufferBlockSize		= 64_b;	// for stream uploading
	};


	//
	// Compile-time Device Limits
	//
	static constexpr struct
	{
		// shader resources
		uint	minUniformBufferOffsetAlignment			= 256;		// nvidia - 64/256, amd -  16,  intel -  64,  mali -  16,  adreno - 64,   apple - 16/32/256
		uint	minStorageBufferOffsetAlignment			= 256;		// nvidia - 16,     amd -   4,  intel -  64,  mali - 256,  adreno - 64,   apple - 16
		uint	minVertexBufferOffsetAlignment			= 256;		// vulkan - 1 (not specified),											  apple - 16
		uint	minTexelBufferOffsetAlignment			= 256;		// nvidia - 16,     amd -   4,  intel -  64,  mali - 256,  adreno - 64,   apple - ?
		uint	maxUniformBufferRange					= 65536;	// nvidia - 64k,    amd - inf,  intel - inf,  mali - 64k,  adreno - 64k,  apple - 4k/inf?
		uint	maxDescriptorSetUniformBuffersDynamic	= 8;		// nvidia - 15,     amd -   8,  intel -  16,  mali -   8,  adreno - 32,   apple - 31
		uint	maxDescriptorSetStorageBuffersDynamic	= 4;		// nvidia - 16,     amd -   8,  intel -  16,  mali -   4,  adreno - 16,   apple - 31
		uint	maxDescriptorSetInputAttachments		= 4;
		uint	maxBoundDescriptorSets					= 4;		// nvidia -  32,    amd -  32,  intel -   8,  mali -   4,  adreno -   4,  apple - 31
		uint	maxVertexInputAttributes				= 16;		// nvidia -  32,    amd -  64,  intel -  32,  mali -  16,  adreno -  32,  apple - 31
		uint	maxPushConstantsSize					= 128;		// nvidia - 256,    amd - 128,  intel - 256,  mali - 128,  adreno - 128,  apple - 4k
		uint	minThreadgroupMemoryLengthAlign			= 16;		//																		  apple - 16

		// render targets
		uint	maxColorAttachments						= 4;		// nvidia -   8,    amd -   8,  intel -   8,  mali -   4,  adreno -   8,  apple - 4
		uint	maxFramebufferLayers					= 256;		//																		  apple - 2048

		// copy
		uint	minMemoryMapAlignment					= 64;		// nvidia - 64,     amd -  64,  intel -  64,  mali -  64,  adreno - 64,   apple - ?
		uint	minNonCoherentAtomSize					= 128;		// nvidia - 64,     amd - 128,  intel -   1,  mali -  64,  adreno -  1,   apple - ?
		uint	minOptimalBufferCopyOffsetAlignment		= 64;		// nvidia -   1,    amd -   1,  intel -  64,  mali -  64,  adreno -  64,  apple - 1
		uint	minOptimalBufferCopyRowPitchAlignment	= 64;		// nvidia -   1,    amd -   1,  intel -  64,  mali -  64,  adreno -  64,  apple - 256

		uint	minComputeWorkgroupInvocations			= 64;		// nvidia - 1024,   amd - 1024, intel - 1024, mali -  64,  adreno - 128,  apple - 512
	} DeviceLimits;


} // AE::Graphics
