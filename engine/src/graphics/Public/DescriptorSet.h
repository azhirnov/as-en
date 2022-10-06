// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	IDescriptorUpdater - helper class for descriptor updating.

	IDescriptorUpdater
		thread-safe:	no
*/
#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/ImageDesc.h"

namespace AE::Graphics
{

	enum class EDescSetUsage : ubyte
	{
		Unknown				= 0,
		AllowPartialyUpdate	= 1 << 0,

		// vulkan only
		UpdateTemplate		= 1 << 1,

		// metal only
		ArgumentBuffer		= 1 << 2,
		MutableArgBuffer	= 1 << 3,	// use 'device' type instead of 'constant'

		_Last,
		All					= ((_Last - 1) << 1) - 1,
	};
	AE_BIT_OPERATORS( EDescSetUsage );


	enum class EDescriptorType : ubyte
	{
		UniformBuffer,
		StorageBuffer,
		
		UniformTexelBuffer,
		StorageTexelBuffer,
		
		StorageImage,
		SampledImage,					// texture without sampler
		CombinedImage,					// sampled image + sampler
		CombinedImage_ImmutableSampler,	// sampled image + immutable sampler
		SubpassInput,
		
		Sampler,
		ImmutableSampler,
		
		RayTracingScene,
		//RayTracingGeometry,	// metal only
		//InlineUniformBlock,	// TODO
		//ThreadgroupMemory,	// metal only

		_Count,
		Unknown		= 0xFF,
	};


	enum class EDescUpdateMode : ubyte
	{
		Partialy,
		UpdateTemplate,

		Unknown			= 0xFF
	};



	//
	// Descriptor Updater
	//

	class IDescriptorUpdater : public EnableRC<IDescriptorUpdater>
	{
	// interface
	public:
		ND_ virtual bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode) = 0;
			virtual bool  Flush () = 0;
		
			virtual bool  BindImage (const UniformName &name, ImageViewID image, uint elementIndex = 0) = 0;
			virtual bool  BindImages (const UniformName &name, ArrayView<ImageViewID> images, uint firstIndex = 0) = 0;

			virtual bool  BindTexture (const UniformName &name, ImageViewID image, const SamplerName &sampler, uint elementIndex = 0) = 0;
			virtual bool  BindTextures (const UniformName &name, ArrayView<ImageViewID> images, const SamplerName &sampler, uint firstIndex = 0) = 0;

			virtual bool  BindSampler (const UniformName &name, const SamplerName &sampler, uint elementIndex = 0) = 0;
			virtual bool  BindSamplers (const UniformName &name, ArrayView<SamplerName> samplers, uint firstIndex = 0) = 0;

			virtual bool  BindBuffer (const UniformName &name, BufferID buffer, uint elementIndex = 0) = 0;
			virtual bool  BindBuffer (const UniformName &name, BufferID buffer, Bytes offset, Bytes size, uint elementIndex = 0) = 0;
			virtual bool  BindBuffers (const UniformName &name, ArrayView<BufferID> buffers, uint firstIndex = 0) = 0;

			//virtual bool  SetBufferBase (const UniformName &name, Bytes offset, uint elementIndex = 0) = 0;	// TODO

			virtual bool  BindTexelBuffer (const UniformName &name, BufferViewID view, uint elementIndex = 0) = 0;
			virtual bool  BindTexelBuffers (const UniformName &name, ArrayView<BufferViewID> views, uint firstIndex = 0) = 0;
			
			virtual bool  BindRayTracingScene (const UniformName &name, RTSceneID scene, uint elementIndex = 0) = 0;
			virtual bool  BindRayTracingScenes (const UniformName &name, ArrayView<RTSceneID> scenes, uint firstIndex = 0) = 0;
	};
	using DescriptorUpdaterPtr = RC< IDescriptorUpdater >;


}	// AE::Graphics
