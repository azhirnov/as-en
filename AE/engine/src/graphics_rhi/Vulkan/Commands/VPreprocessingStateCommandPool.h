// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/IndirectCommandBuffer.h"
# include "graphics_rhi/Vulkan/VDevice.h"
# include "graphics_rhi/Vulkan/Commands/VBaseIndirectContext.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Preprocessing State Command Pool
	//
	class VPreprocessingStateCommandPool final : public IPreprocessingStateCommandPool
	{
	// variables
	private:
		VkCommandPool		_handle		= Default;
		VQueuePtr			_queue;
		VDevice const&		_device;
		RecursiveMutex		_poolGuard;


	// methods
	public:
		VPreprocessingStateCommandPool (VDevice const&, VQueuePtr)	__NE___;
		~VPreprocessingStateCommandPool ()							__NE_OV;

		RC<IComputeStateCommandBuffer>		BeginCompute ()			__NE_OV;
		RC<IGraphicsStateCommandBuffer>		BeginGraphics ()		__NE_OV;
		RC<IRayTracingStateCommandBuffer>	BeginRayTracing ()		__NE_OV;

		EQueueType							QueueType ()			C_NE_OV	{ return _queue->type; }
		ND_ bool							IsCreated ()			C_NE___	{ return _handle != Default; }
		ND_ VDevice const&					GetDevice ()			C_NE___	{ return _device; }
		ND_ VkCommandPool					Handle ()				C_NE___	{ return _handle; }
		ND_ RecursiveMutex &				GetGuard ()				__NE___	{ return _poolGuard; }

	private:
		ND_ VkCommandBuffer  _Alloc ()								__NE___;
	};



	//
	// Vulkan Preprocessing State
	//
	class VPreprocessingState
	{
	// types
	protected:
		using DescSets_t	= StaticArray< DescriptorSetID, GraphicsConfig::MaxDescriptorSets >;
		using PCStorage_t	= StaticArray< uint, 256/sizeof(uint) >;

		struct PushConst
		{
			PushConstantIndex				idx;
			Bytes16u						size;
			Bytes16u						offset;
			ShaderStructName::Optimized_t	typeName;
		};
		using PushConstants_t	= FixedArray< PushConst, GraphicsConfig::MaxPushConstants >;


	// variables
	protected:
		VkCommandBuffer							_cmdbuf			= Default;
		RC<VPreprocessingStateCommandPool>		_pool;
		bool									_endRecording	= false;
		const VkPipelineBindPoint				_bindPoint;

		mutable AtomicFrameUID					_lastUsed;

		// saved states
		struct {
			VkPipeline								pipeline		= Default;
			VkPipelineLayout						pplnLayout		= Default;
			IndirectExecutionSetID					execSet;
			DescSets_t								descSets;
			PushConstants_t							pushConst;
			PCStorage_t								pcStorage;
			Bytes16u								pcStorageOffset;
			EPipelineDynamicState					reqDynStates	= Default;
			EPipelineDynamicState					usedDynStates	= Default;
		}										_state;


	// methods
	protected:
		VPreprocessingState (VkPipelineBindPoint, VkCommandBuffer, RC<VPreprocessingStateCommandPool>)					__NE___;
		~VPreprocessingState ()																							__NE___;

		void  _BindInitialPipeline (IndirectExecutionSetID)																__Th___;
		void  _PushConstant (const PushConstantIndex &, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___;

		template <typename PipeType>
		void  _BindPipeline (PipeType ppln)																				__Th___;
		void  _BindDescriptorSet (DescSetBinding index, DescriptorSetID ds)												__Th___;

		bool  _Recycle ()																								__NE___;

		template <typename Ctx>
		void  _BindDescSets (Ctx &ctx)																					C_Th___;

		template <typename Ctx>
		void  _PushConstants (Ctx &ctx)																					C_Th___;

		ND_ bool  _IsResourcesAlive2 (const ResourceManager &)															C_NE___;
	};



	//
	// Vulkan Compute State Command Buffer
	//
	class VComputeStateCommandBuffer final : public IComputeStateCommandBuffer, public VPreprocessingState
	{
	// variables
	private:
		struct {
			ComputePipelineID		ppln;
		}						_state2;


	// methods
	public:
		VComputeStateCommandBuffer (VkCommandBuffer cmdbuf, RC<VPreprocessingStateCommandPool> pool)						__NE___ :
			VPreprocessingState{ VK_PIPELINE_BIND_POINT_COMPUTE, cmdbuf, RVRef(pool) } {}

		// IStateCommandBuffer //
		VkCommandBuffer		GetCommandBuffer ()																				C_NE_OV	{ CHECK_ERR( _endRecording );  return _cmdbuf; }
		EQueueType			GetQueueType ()																					C_NE_OV	{ return _pool->QueueType(); }

		void  BindInitialPipeline (IndirectExecutionSetID id)																__Th_OV	{ ASSERT( _state2.ppln == Default );  _BindInitialPipeline( id ); }
		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV	{ ASSERT( idx.stage == EShader::Compute );  _PushConstant( idx, size, values, typeName ); }

		bool  Recycle ()																									__NE_OV;
		bool  End ()																										__NE_OV;

		// IComputeStateCommandBuffer //
		void  BindPipeline (ComputePipelineID id)																			__Th_OV	{ _BindPipeline( id );  _state2.ppln = id; }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID id)													__Th_OV	{ _BindDescriptorSet( index, id ); }

		bool  CopyStates (IComputeContext &)																				C_Th_OV;

		IComputeStateCommandBuffer const*  AsCompute ()																		C_NE_OV { return this; }

	private:
		ND_ bool  _IsResourcesAlive ()																						C_NE___;
	};



	//
	// Vulkan Graphics State Command Buffer
	//
	class VGraphicsStateCommandBuffer final : public IGraphicsStateCommandBuffer, public VPreprocessingState
	{
	// types
	private:
		using VertexBuffers_t		= StaticArray< BufferID,	GraphicsConfig::MaxVertexBuffers >;
		using VertexBufferOffsets_t	= StaticArray< Bytes,		GraphicsConfig::MaxVertexBuffers >;
		using Viewports_t			= StaticArray< Viewport,	GraphicsConfig::MaxViewports >;
		using Scissors_t			= StaticArray< RectI,		GraphicsConfig::MaxViewports >;


	// variables
	private:
		struct {
			GraphicsPipelineID			gppln;
			MeshPipelineID				mppln;

			BufferID					indexBuffer;
			Bytes						indexBufferOffset;
			EIndex						indexType			= Default;
			ubyte						vpCount				= 0;
			ubyte						vbCount				= 0;
			VertexBuffers_t				vertexBuffers		= {};
			VertexBufferOffsets_t		vertexBufferOffsets;
			Viewports_t					viewports;
			Scissors_t					scissors;
		}							_state2;


	// methods
	public:
		VGraphicsStateCommandBuffer (VkCommandBuffer cmdbuf, RC<VPreprocessingStateCommandPool> pool)						__NE___ :
			VPreprocessingState{ VK_PIPELINE_BIND_POINT_GRAPHICS, cmdbuf, RVRef(pool) } {}

		// IStateCommandBuffer //
		VkCommandBuffer		GetCommandBuffer ()																				C_NE_OV	{ CHECK_ERR( _endRecording );  return _cmdbuf; }
		EQueueType			GetQueueType ()																					C_NE_OV	{ return _pool->QueueType(); }

		void  BindInitialPipeline (IndirectExecutionSetID id)																__Th_OV;
		void  PushConstant (const PushConstantIndex &, Bytes size, const void* values, ShaderStructName::Ref typeName)		__Th_OV;

		bool  Recycle ()																									__NE_OV;
		bool  End ()																										__NE_OV;

		// IGraphicsStateCommandBuffer //
		void  BindPipeline (GraphicsPipelineID id)																			__Th_OV	{ _BindPipeline( id );  _state2.gppln = id; }
		void  BindPipeline (MeshPipelineID id)																				__Th_OV	{ _BindPipeline( id );  _state2.mppln = id; }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID id)													__Th_OV	{ _BindDescriptorSet( index, id ); }

		void  SetViewports (ArrayView<Viewport> viewports)																	__NE_OV;
		void  SetScissors (ArrayView<RectI> scissors)																		__NE_OV;
		/*
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__NE_OV;
		void  SetStencilReference (uint reference)																			__NE_OV;
		void  SetStencilReference (uint frontReference, uint backReference)													__NE_OV;
		void  SetBlendConstants (const RGBA32f &color)																		__NE_OV;
		*/
		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th_OV;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th_OV;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th_OV;
		void  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset)		__Th_OV;

		bool  CopyStates (IDrawContext &)																					C_Th_OV;

		IGraphicsStateCommandBuffer const*  AsGraphics ()																	C_NE_OV { return this; }

	private:
		ND_ bool  _IsResourcesAlive ()																						C_NE___;
	};



	//
	// Vulkan Ray Tracing State Command Buffer
	//
	class VRayTracingStateCommandBuffer final : public IRayTracingStateCommandBuffer, public VPreprocessingState
	{
	// variables
	private:
		struct {
			RayTracingPipelineID		ppln;
			Bytes						stackSize;
		}							_state2;


	// methods
	public:
		VRayTracingStateCommandBuffer (VkCommandBuffer cmdbuf, RC<VPreprocessingStateCommandPool> pool)						__NE___ :
			VPreprocessingState{ VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, cmdbuf, RVRef(pool) } {}

		// IStateCommandBuffer //
		VkCommandBuffer		GetCommandBuffer ()																				C_NE_OV	{ CHECK_ERR( _endRecording );  return _cmdbuf; }
		EQueueType			GetQueueType ()																					C_NE_OV	{ return _pool->QueueType(); }

		void  BindInitialPipeline (IndirectExecutionSetID id)																__Th_OV	{ ASSERT( _state2.ppln == Default );  _BindInitialPipeline( id ); }
		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV;

		bool  Recycle ()																									__NE_OV;
		bool  End ()																										__NE_OV;

		// IRayTracingStateCommandBuffer //
		void  BindPipeline (RayTracingPipelineID id)																		__Th_OV	{ _BindPipeline( id );  _state2.ppln = id; }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID id)													__Th_OV	{ _BindDescriptorSet( index, id ); }

		void  SetStackSize (Bytes size)																						__Th_OV;

		bool  CopyStates (IRayTracingContext &)																				C_Th_OV;

		IRayTracingStateCommandBuffer const*  AsRayTracing ()																C_NE_OV { return this; }

	private:
		ND_ bool  _IsResourcesAlive ()																						C_NE___;
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
