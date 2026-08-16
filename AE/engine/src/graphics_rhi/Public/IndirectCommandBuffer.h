// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	IPreprocessingStateCommandPool
		Thread-safe:	no

	IStateCommandBuffer
		Thread-safe:	no,  all command buffers from pool must be used in same thread
*/

#pragma once

#include "graphics_rhi/Public/IDs.h"
#include "graphics_rhi/Public/ShaderEnums.h"
#include "graphics_rhi/Public/CommandBufferTypes.h"

namespace AE::Graphics
{

	//
	// Generated Commands Memory Requirements Description
	//
	struct GeneratedCommandsMemoryRequirementsDesc
	{
		IndirectExecutionSetID		indirectExecutionSet;
		IndirectCommandsLayoutID	indirectCommandsLayout;

		uint						maxSequenceCount			= 1;			// see 'ExecuteGeneratedCommandsCmd::maxSequenceCount' description
		uint						maxDrawCount				= 0;			// see 'ExecuteGeneratedCommandsCmd::maxDrawCount' description
	};



	enum class EIndirectCommandsLayoutUsage : ubyte
	{
		Unknown				= 0,		// default: optional preprocess, sequential order
		ExplicitPreprocess	= 1 << 0,	// layout is always used with the manual preprocessing step
		UnorderedSequences	= 1 << 1,	// processing of sequences will happen at an implementation-dependent order, which is not guaranteed to be deterministic using the same input data.
	};

	enum class EIndirectCommandsInputMode : ubyte
	{
		Unknown				= 0,
		VulkanIndexBuffer,			// VkBindIndexBufferIndirectCommandEXT
		DXGI_IndexBuffer,			// D3D12_INDEX_BUFFER_VIEW
	};



	//
	// Indirect Commands Layout Description
	//
	struct IndirectCommandsLayoutDesc
	{
	// types
		enum class ETokenType
		{							//|			in 'ExecuteGeneratedCommandsCmd::indirectAddress'			|		on init				|	comments
			Unknown = 0,			//|---------------------------------------------------------------------|---------------------------|---------------
			ExecutionSet,			// 'uint[]' array of indices into the indirect execution set			|							| used to change shaders
			PushConstant,			// 'uint[]' raw data													| 'PushConstantToken'		|
			SequenceIndex,			// without shader access												| 'SequenceIndexToken'		|
			IndexBuffer,			// 'BindIndexBufferIndirectCommand'										| 'IndexBufferToken'		|
			VertexBuffer,			// 'BindVertexBufferIndirectCommand'									| 'VertexBufferToken'		|
			Draw,					// 'DrawIndirectCommand'												|							|
			DrawCount,				// 'DrawIndirectCommand + DrawIndexedIndirectCommand'					|							| requires 'deviceGeneratedCommandsMultiDrawIndirectCount'
			DrawIndexed,			// 'DrawIndexedIndirectCommand'											|							|
			DrawIndexedCount,		// 'DrawIndirectCountIndirectCommand + DrawIndexedIndirectCommand'		|							| requires 'deviceGeneratedCommandsMultiDrawIndirectCount'
			DrawMeshTasks,			// 'DrawMeshTasksIndirectCommand'										|							| requires 'meshShader'
			DrawMeshTasksCount,		// 'DrawIndirectCountIndirectCommand + DrawMeshTasksIndirectCommand'	|							| requires 'meshShader' and 'deviceGeneratedCommandsMultiDrawIndirectCount'
			Dispatch,				// 'DispatchIndirectCommand'											|							|
			TraceRays,				// 'TraceRayIndirectCommand2'											|							| requires 'rayTracingPipeline'
			_Count
		};

		struct ExecutionSetToken
		{
			EShaderStages	stages	= Default;
		};

		struct PushConstantToken
		{
			EShaderStages	stages	= Default;
			Bytes32u		offset;
			Bytes32u		size;
		};

		struct IndexBufferToken
		{
			EIndirectCommandsInputMode	mode	= Default;
		};

		struct VertexBufferToken
		{
			uint			vertexBindingUnit;
		};

		struct SequenceIndexToken
		{
			EShaderStages	stages	= Default;
			Bytes32u		offset;
		};

		struct EmptyToken {};

		struct Token
		{
			ETokenType				type	= Default;
			Bytes32u				offset;				// offset to command data for token,
														// must be <= 'DeviceProperties.icb.maxIndirectCommandsTokenOffset'
			union {
				EmptyToken				empty			= {};
				PushConstantToken		pushConstant;
				IndexBufferToken		indexBuffer;
				VertexBufferToken		vertexBuffer;
				ExecutionSetToken		executionSet;
				SequenceIndexToken		sequenceIndex;
			}						data;
		};


	// variables
		PipelineLayoutID				pipelineLayout;				// required only for 'PushConstant' and 'SequenceIndex' tokens
		Bytes							sequenceStride;				// distance between sequences in the indirect buffer 'ExecuteGeneratedCommandsCmd::indirectAddress',
																	// must be <= 'DeviceProperties.icb.maxIndirectCommandsIndirect'
		EIndirectCommandsLayoutUsage	usage			= Default;
		EShaderStages					stages			= Default;	// must be in 'FeatureSet::supportedIndirectCommandsShaderStages'
		Array<Token>					tokens;						// must be <= 'DeviceProperties.icb.maxIndirectCommandsTokenCount'
	};



	//
	// Indirect Commands Layout Description Builder
	//
	struct IndirectCommandsLayoutDescBuilder
	{
	// types
	public:
		using Self			= IndirectCommandsLayoutDescBuilder;
	private:
		using Token			= IndirectCommandsLayoutDesc::Token;
		using ETokenType	= IndirectCommandsLayoutDesc::ETokenType;


	// variables
	private:
		IndirectCommandsLayoutDesc	_desc;
		Bytes32u					_offset;
		void const*					_pl			= null;
		EShaderStages				_plStages	= Default;


	// methods
	public:
		IndirectCommandsLayoutDescBuilder ()										__NE___ {}

		bool   PipelineLayoutFrom (GraphicsPipelineID)								__NE___;
		bool   PipelineLayoutFrom (MeshPipelineID)									__NE___;
		bool   PipelineLayoutFrom (ComputePipelineID)								__NE___;
		bool   PipelineLayoutFrom (RayTracingPipelineID)							__NE___;
		bool   PipelineLayoutFrom (IndirectExecutionSetID)							__NE___;

		Self&  Usage (EIndirectCommandsLayoutUsage)									__NE___;
		Self&  Stages (EShaderStages)												__NE___;
		Self&  SequenceStride (Bytes)												__NE___;

		Self&  BaseOffset (Bytes)													__NE___;

		template <typename T>
		Self&  Advance (usize count = 1)											__NE___	{ return Advance( SizeOf<T> * count ); }
		Self&  Advance (Bytes)														__NE___;

		template <typename T>
		Self&  PushConstant		  (PushConstantName::Ref pcName)													__NE___	{ return PushConstant( pcName, T::TypeName ); }
		Self&  PushConstant		  (PushConstantName::Ref pcName, ShaderStructName::Ref typeName)					__NE___;
		Self&  PushConstant		  (EShaderStages stages, Bytes32u pcOffset, Bytes32u pcSize)						__NE___;
		Self&  ExecutionSet		  (EShaderStages stages)															__NE___;
		Self&  ExecutionSet		  ()																				__NE___	{ return ExecutionSet( _desc.stages ); }
		Self&  SequenceIndex	  (EShaderStages stages, Bytes32u idxOffset)										__NE___;
		Self&  IndexBuffer		  (EIndirectCommandsInputMode mode = EIndirectCommandsInputMode::VulkanIndexBuffer)	__NE___;
		Self&  VertexBuffer		  (uint vertexBindingUnit)															__NE___;
		Self&  VertexBuffer		  (GraphicsPipelineID pplnId, VertexBufferName::Ref name)							__NE___;

		Self&  Draw				  ()												__NE___;
		Self&  DrawIndexed		  ()												__NE___;
		Self&  DrawIndexedCount	  ()												__NE___;
		Self&  DrawMeshTasks	  ()												__NE___;
		Self&  DrawMeshTasksCount ()												__NE___;
		Self&  Dispatch			  ()												__NE___;
		Self&  TraceRays		  ()												__NE___;

		ND_ Bytes	SequenceSize ()													C_NE___	{ return _offset; }
		ND_ Bytes	Size (uint sequenceCount)										C_NE___	{ return Max( SequenceSize(), _desc.sequenceStride ) * sequenceCount; }

		ND_ IndirectCommandsLayoutDesc  Build ()									__NE___;

	private:
		template <typename PipeID>
		bool  _PipelineLayoutFrom (PipeID)											__NE___;
	};
//-----------------------------------------------------------------------------


	class IRayTracingContext;
	class IComputeContext;
	class IDrawContext;

	class IComputeStateCommandBuffer;
	class IGraphicsStateCommandBuffer;
	class IRayTracingStateCommandBuffer;



	//
	// State Command Buffer interface
	//
	class IStateCommandBuffer : public EnableRC<IStateCommandBuffer>
	{
	// types
	public:

	  #if defined(AE_ENABLE_VULKAN)
		using NativeCmdBuffer_t			= VkCommandBuffer;
	  #elif defined(AE_ENABLE_METAL)
		using NativeCmdBuffer_t			= void*;
	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using NativeCmdBuffer_t			= void*;		// TODO: use ID
	  #else
	  # error not implemented
	  #endif

	// interface
	public:
		ND_ virtual NativeCmdBuffer_t  GetCommandBuffer ()																		C_NE___ = 0;

		// 'ExecuteGeneratedCommands()' must be used on the same queue.
		ND_ virtual EQueueType  GetQueueType ()																					C_NE___ = 0;

		// must be used instead of 'BindPipeline()' if layout contains 'ExecutionSet' token.
		virtual void  BindInitialPipeline (IndirectExecutionSetID)																__Th___ = 0;

		virtual void  PushConstant (const PushConstantIndex &, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)									__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }

		// reset states for new recording
		ND_ virtual bool  Recycle ()																							__NE___ = 0;

		// stop recording states, returns 'true' if all states are set
		ND_ virtual bool  End ()																								__NE___ = 0;

		ND_ virtual IComputeStateCommandBuffer const*		AsCompute ()														C_NE___ { return null; }
		ND_ virtual IGraphicsStateCommandBuffer const*		AsGraphics ()														C_NE___ { return null; }
		ND_ virtual IRayTracingStateCommandBuffer const*	AsRayTracing ()														C_NE___ { return null; }
	};



	//
	// Compute State Command Buffer interface
	//
	class IComputeStateCommandBuffer : public IStateCommandBuffer
	{
	// interface
	public:
		virtual void  BindPipeline (ComputePipelineID ppln)																		__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds)												__Th___	= 0;

		virtual bool  CopyStates (IComputeContext &)																			C_Th___ = 0;
	};



	//
	// Graphics State Command Buffer interface
	//
	class IGraphicsStateCommandBuffer : public IStateCommandBuffer
	{
	// interface
	public:
		virtual void  BindPipeline (GraphicsPipelineID ppln)																	__Th___	= 0;
		virtual void  BindPipeline (MeshPipelineID ppln)																		__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds)												__Th___ = 0;

				void  SetViewport (const Viewport &viewport)																	__Th___	{ SetViewports({ &viewport, 1u }); }
		virtual void  SetViewports (ArrayView<Viewport> viewports)																__Th___	= 0;
				void  SetScissor (const RectI &scissor)																			__Th___	{ SetScissors({ &scissor, 1u }); }
		virtual void  SetScissors (ArrayView<RectI> scissors)																	__Th___	= 0;
		/*
		//	requires: EPipelineDynamicState::DepthBias
		virtual void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)			__Th___ = 0;
		//	requires: EPipelineDynamicState::StencilReference
		virtual void  SetStencilReference (uint reference)																		__Th___	= 0;
		virtual void  SetStencilReference (uint frontReference, uint backReference)												__Th___	= 0;
		//	requires: EPipelineDynamicState::BlendConstants
		virtual void  SetBlendConstants (const RGBA32f &color)																	__Th___	= 0;
		*/
		//		buffer:  EResourceState::IndexBuffer
		virtual void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)											__Th___	= 0;
		//		buffer:  EResourceState::VertexBuffer
		virtual void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)												__Th___	= 0;
		virtual void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)				__Th___	= 0;
		virtual void  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset)	__Th___ = 0;

		virtual bool  CopyStates (IDrawContext &)																				C_Th___ = 0;
	};



	//
	// Ray Tracing State Command Buffer interface
	//
	class IRayTracingStateCommandBuffer : public IStateCommandBuffer
	{
	// interface
	public:
		virtual void  BindPipeline (RayTracingPipelineID ppln)																	__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds)												__Th___	= 0;

		//	requires: EPipelineDynamicState::RTStackSize
		virtual void  SetStackSize (Bytes size)																					__Th___	= 0;

		virtual bool  CopyStates (IRayTracingContext &)																			C_Th___ = 0;
	};



	//
	// Preprocessing State Command Pool interface
	//
	class IPreprocessingStateCommandPool : public EnableRC<IPreprocessingStateCommandPool>
	{
	// interface
	public:
		ND_ virtual RC<IComputeStateCommandBuffer>		BeginCompute ()			__NE___ = 0;
		ND_ virtual RC<IGraphicsStateCommandBuffer>		BeginGraphics ()		__NE___ = 0;
		ND_ virtual RC<IRayTracingStateCommandBuffer>	BeginRayTracing ()		__NE___ = 0;

		ND_ virtual EQueueType							QueueType ()			C_NE___ = 0;
	};


} // AE::Graphics
