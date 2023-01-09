// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	IDescriptorUpdater - helper class for descriptor updating.

	IDescriptorUpdater
		thread-safe:	no
*/
#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/ShaderEnums.h"
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
	// Metal Binding per stage
	//
	struct MetalBindingPerStage
	{
	// variables
		static constexpr uint		count			= 3;
		ubyte						data [count+1]	= { UMax, UMax, UMax, UMax };	// [3] - vulkan like index to use it in MDrawContext
		

	// methods
		constexpr MetalBindingPerStage ()					__NE___	{}
		explicit constexpr MetalBindingPerStage (ubyte idx) __NE___	: data{ idx, idx, idx, UMax } {}

		ND_ bool				operator == (MetalBindingPerStage rhs)	C_NE___	{ return std::memcmp( data, rhs.data, sizeof(data) ) == 0; }

		ND_ constexpr bool		IsDefined ()	C_NE___	{ return (data[0] != UMax) | (data[1] != UMax) | (data[2] != UMax); }
		ND_ constexpr bool		Has (usize idx)	C_NE___	{ ASSERT( idx < count );	return data[idx] != UMax; }
		ND_ constexpr ubyte		Get (usize idx)	C_NE___	{ ASSERT( Has( idx ));		return data[idx]; }
		
		ND_ constexpr ubyte		Vertex	 ()		C_NE___	{ return Get(0); }
		ND_ constexpr ubyte		Tile	 ()		C_NE___	{ return Get(0); }
		ND_ constexpr ubyte		Compute	 ()		C_NE___	{ return Get(0); }
		ND_ constexpr ubyte		Mesh	 ()		C_NE___	{ return Get(0); }
		ND_ constexpr ubyte		Fragment ()		C_NE___	{ return Get(1); }
		ND_ constexpr ubyte		MeshTask ()		C_NE___	{ return Get(2); }

		// binding index is same for all stages
		// used only for DS caching
		ND_ constexpr ubyte		BindingIndex ()	C_NE___	{ ASSERT( data[3] != UMax );  return data[3]; }
		ND_ ubyte&				BindingIndex ()	__NE___	{ return data[3]; }
		

		ND_ static constexpr int	ShaderToIndex (EShader type) __NE___
		{
			switch ( type ) {
				case EShader::Vertex :
				case EShader::Tile :
				case EShader::Compute :
				case EShader::Mesh :		return 0;
				case EShader::Fragment :	return 1;
				case EShader::MeshTask :	return 2;
			}
			return -1;
		}

		ND_ static constexpr int	StageToIndex (EShaderStages stage) __NE___
		{
			ASSERT( IsSingleBitSet( stage ));
			switch ( stage ) {
				case EShaderStages::Vertex :
				case EShaderStages::Tile :
				case EShaderStages::Compute :
				case EShaderStages::Mesh :		return 0;
				case EShaderStages::Fragment :	return 1;
				case EShaderStages::MeshTask :	return 2;
			}
			return -1;
		}

		ND_ ubyte const*	PtrForShader (EShaderStages stage)	C_NE___	{ int i = StageToIndex( stage );  return i >= 0 ? &data[i] : null; }
		ND_ ubyte*			PtrForShader (EShaderStages stage)	__NE___	{ int i = StageToIndex( stage );  return i >= 0 ? &data[i] : null; }
		
		ND_ ubyte const*	PtrForShader (EShader type)			C_NE___	{ int i = ShaderToIndex( type );  return i >= 0 ? &data[i] : null; }
		ND_ ubyte*			PtrForShader (EShader type)			__NE___	{ int i = ShaderToIndex( type );  return i >= 0 ? &data[i] : null; }
		
		ND_ ubyte			ForShader (EShaderStages stage)		C_NE___	{ auto* ptr = PtrForShader( stage );  return ptr != null ? *ptr : UMax; }
		ND_ ubyte			ForShader (EShader type)			C_NE___	{ auto* ptr = PtrForShader( type );   return ptr != null ? *ptr : UMax; }
	};



	//
	// Descriptor Set Binding
	//
	union DescSetBinding
	{
	// variables
		uint					vkIndex		= UMax;
		MetalBindingPerStage	mtlIndex;

	// methods
		explicit constexpr DescSetBinding ()									__NE___	{}
		explicit constexpr DescSetBinding (uint vulkanBinding)					__NE___	: vkIndex{vulkanBinding} {}
		explicit constexpr DescSetBinding (MetalBindingPerStage metalBinding)	__NE___	: mtlIndex{metalBinding} {}

		ND_ constexpr bool  operator == (const DescSetBinding &rhs)				C_NE___	{ return vkIndex == rhs.vkIndex; }
	};



	//
	// Descriptor Updater
	//

	class IDescriptorUpdater : public EnableRC<IDescriptorUpdater>
	{
	// interface
	public:
		ND_ virtual bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode)																	__NE___	= 0;
			virtual bool  Flush ()																													__NE___	= 0;
		
			virtual bool  BindImage (const UniformName &name, ImageViewID image, uint elementIndex = 0)												__NE___	= 0;
			virtual bool  BindImages (const UniformName &name, ArrayView<ImageViewID> images, uint firstIndex = 0)									__NE___	= 0;

			virtual bool  BindTexture (const UniformName &name, ImageViewID image, const SamplerName &sampler, uint elementIndex = 0)				__NE___	= 0;
			virtual bool  BindTextures (const UniformName &name, ArrayView<ImageViewID> images, const SamplerName &sampler, uint firstIndex = 0)	__NE___	= 0;

			virtual bool  BindSampler (const UniformName &name, const SamplerName &sampler, uint elementIndex = 0)									__NE___	= 0;
			virtual bool  BindSamplers (const UniformName &name, ArrayView<SamplerName> samplers, uint firstIndex = 0)								__NE___	= 0;

			virtual bool  BindBuffer (const UniformName &name, BufferID buffer, uint elementIndex = 0)												__NE___	= 0;
			virtual bool  BindBuffer (const UniformName &name, BufferID buffer, Bytes offset, Bytes size, uint elementIndex = 0)					__NE___	= 0;
			virtual bool  BindBuffers (const UniformName &name, ArrayView<BufferID> buffers, uint firstIndex = 0)									__NE___	= 0;

			//virtual bool  SetBufferBase (const UniformName &name, Bytes offset, uint elementIndex = 0) = 0;	// TODO

			virtual bool  BindTexelBuffer (const UniformName &name, BufferViewID view, uint elementIndex = 0)										__NE___	= 0;
			virtual bool  BindTexelBuffers (const UniformName &name, ArrayView<BufferViewID> views, uint firstIndex = 0)							__NE___	= 0;
			
			virtual bool  BindRayTracingScene (const UniformName &name, RTSceneID scene, uint elementIndex = 0)										__NE___	= 0;
			virtual bool  BindRayTracingScenes (const UniformName &name, ArrayView<RTSceneID> scenes, uint firstIndex = 0)							__NE___	= 0;
	};
	using DescriptorUpdaterPtr = RC< IDescriptorUpdater >;


} // AE::Graphics
