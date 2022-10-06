// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	DrawCtx --> DirectDrawCtx   --> BarrierMngr --> Metal device 
			\-> IndirectDrawCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"
# include "graphics/Metal/Commands/MArgumentSetter.h"

namespace AE::Graphics::_hidden_
{
	template <typename CtxImpl>
	class _MGraphicsContextImpl;


	//
	// Metal Direct Draw Context implementation
	//
	
	class _MDirectDrawCtx : public _MBaseDirectContext
	{
	// types
	public:
		using CmdBuf_t			= MCommandBuffer;

		struct VertexAmplificationViewMapping
		{
			uint	renderTargetArrayIndexOffset;
			uint	viewportArrayIndexOffset;
		};

	private:
		using VertexArgSet_t	= MArgumentSetter< EShader::Vertex,		true >;
		using FragmentArgSet_t	= MArgumentSetter< EShader::Fragment,	true >;
		using TileArgSet_t		= MArgumentSetter< EShader::Tile,		true >;
		using MeshArgSet_t		= MArgumentSetter< EShader::Mesh,		true >;
		using MeshTaskArgSet_t	= MArgumentSetter< EShader::MeshTask,	true >;

		using Viewport_t = RenderPassDesc::Viewport;
		
		
	// variables
	private:
		MetalRenderCommandEncoderRC	_encoder;
		struct {
			EPipelineDynamicState		flags			= Default;
			ushort2						threadsPerTile;
			ushort						primitiveType	= UMax;
			ushort						indexType		= UMax;
			
			MetalBuffer					indexBuffer;
			Bytes						indexBufferOffset;
		}							_states;
	protected:
		MDrawBarrierManager			_mngr;


	// methods
	public:
		ND_ VertexArgSet_t		VertexArguments ()				{ return VertexArgSet_t{ _encoder }; }
		ND_ FragmentArgSet_t	FragmentArguments ()			{ return FragmentArgSet_t{ _encoder }; }
		ND_ TileArgSet_t		TileArguments ()				{ return TileArgSet_t{ _encoder }; }
		ND_ MeshArgSet_t		MeshArguments ()				{ return MeshArgSet_t{ _encoder }; }
		ND_ MeshTaskArgSet_t	MeshTaskArguments ()			{ return MeshTaskArgSet_t{ _encoder }; }

		void  SetDepthStencilState (MetalDepthStencilState ds);

		// extended dynamic states
		void  SetPolygonMode (EPolygonMode value);
		void  SetCullMode (ECullMode value);
		void  SetFrontFacing (bool ccw);
		void  SetDepthClamp (bool value);

		// tessellation
		void  SetTessellationFactor (MetalBuffer buffer, Bytes offset, Bytes instanceStride);
		void  SetTessellationFactorScale (float value);

		void  SetVertexAmplification (uint count);
		void  SetVertexAmplification (ArrayView<VertexAmplificationViewMapping> mapping);

		//void  SetVisibilityResultMode ();
		
		void  BindIndexBuffer (MetalBuffer buffer, Bytes offset, EIndex indexType);
		void  BindVertexBuffer (uint index, MetalBuffer buffer, Bytes offset);
		void  BindVertexBuffers (uint firstBinding, ArrayView<MetalBuffer> buffers, ArrayView<Bytes> offsets);

	protected:
		void  _BindGraphicsPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags);
		void  _BindTilePipeline (MetalRenderPipeline ppln, const uint2 &threadsPerTile);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetStencilReference (uint reference);
		void  _SetStencilReference (uint frontReference, uint backReference);
		void  _SetViewport (const Viewport_t &viewport);
		void  _SetViewport (ArrayView<Viewport_t> viewports);
		void  _SetScissor (const RectI &scissor);
		void  _SetScissor (ArrayView<RectI> scissors);
		void  _SetBlendConstants (const RGBA32f &color);
		
		void  _DrawPrimitives (uint vertexCount,
							   uint instanceCount,
							   uint firstVertex,
							   uint firstInstance);

		void  _DrawIndexedPrimitives (uint indexCount,
									  uint instanceCount,
									  uint firstIndex,
									  int  vertexOffset,
									  uint firstInstance);
		
		void  _DrawPrimitivesIndirect (MetalBuffer	indirectBuffer,
									   Bytes		indirectBufferOffset,
									   uint			drawCount,
									   Bytes		stride);

		void  _DrawIndexedPrimitivesIndirect (MetalBuffer	indirectBuffer,
											  Bytes			indirectBufferOffset,
											  uint			drawCount,
											  Bytes			stride);

		void  _DrawPatches ();
		void  _DrawIndexedPatches ();
		void  _DrawPatchesIndirect ();
		void  _DrawIndexedPatchesIndirect ();

		void  _DispatchTile ();
		
		ND_ MetalCommandEncoder  _BaseEncoder ();

		void  _DebugMarker (NtStringView text, RGBA8u);
		void  _PushDebugGroup (NtStringView text, RGBA8u);
		void  _PopDebugGroup ();

		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()			const	{ return _mngr.GetDevice().GetFeatures(); }
	};

	

	//
	// Metal Indirect Draw Context implementation
	//
	
	class _MIndirectDrawCtx : public _MBaseIndirectContext
	{
	// types
	public:
		using CmdBuf_t			= MSoftwareCmdBufPtr;
	private:
		using VertexArgSet_t	= MArgumentSetter< EShader::Vertex,		false >;
		using FragmentArgSet_t	= MArgumentSetter< EShader::Fragment,	false >;
		using TileArgSet_t		= MArgumentSetter< EShader::Tile,		false >;
		using MeshArgSet_t		= MArgumentSetter< EShader::Mesh,		false >;
		using MeshTaskArgSet_t	= MArgumentSetter< EShader::MeshTask,	false >;


	// variables
	protected:
		MDrawBarrierManager		_mngr;


	// methods
	public:
		ND_ VertexArgSet_t		VertexArguments ()				{ return VertexArgSet_t{ this->_cmdbuf.get() }; }
		ND_ FragmentArgSet_t	FragmentArguments ()			{ return FragmentArgSet_t{ this->_cmdbuf.get() }; }
		ND_ TileArgSet_t		TileArguments ()				{ return TileArgSet_t{ this->_cmdbuf.get() }; }
		ND_ MeshArgSet_t		MeshArguments ()				{ return MeshArgSet_t{ this->_cmdbuf.get() }; }
		ND_ MeshTaskArgSet_t	MeshTaskArguments ()			{ return MeshTaskArgSet_t{ this->_cmdbuf.get() }; }
		
		void  SetDepthStencilState (MetalDepthStencilState ds);

		// extended dynamic states
		void  SetPolygonMode (EPolygonMode value);
		void  SetCullMode (ECullMode value);
		void  SetFrontFacing (bool ccw);
		void  SetDepthClamp (bool value);

		// tessellation
		void  SetTessellationFactor (MetalBuffer buffer, Bytes offset, Bytes instanceStride);
		void  SetTessellationFactorScale (float value);

		void  SetVertexAmplification (uint count);
		void  SetVertexAmplification (ArrayView<VertexAmplificationViewMapping> mapping);

		//void  SetVisibilityResultMode ();
		
		void  BindIndexBuffer (MetalBuffer buffer, Bytes offset, EIndex indexType);
		void  BindVertexBuffer (uint index, MetalBuffer buffer, Bytes offset);
		void  BindVertexBuffers (uint firstBinding, ArrayView<MetalBuffer> buffers, ArrayView<Bytes> offsets);

	protected:
		void  _BindGraphicsPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags);
		void  _BindTilePipeline (MetalRenderPipeline ppln, const uint2 &threadsPerTile);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetStencilReference (uint reference);
		void  _SetStencilReference (uint frontReference, uint backReference);
		void  _SetViewport (const Viewport_t &viewport);
		void  _SetViewport (ArrayView<Viewport_t> viewports);
		void  _SetScissor (const RectI &scissor);
		void  _SetScissor (ArrayView<RectI> scissors);
		void  _SetBlendConstants (const RGBA32f &color);
		
		void  _DrawPrimitives (uint vertexCount,
							   uint instanceCount,
							   uint firstVertex,
							   uint firstInstance);

		void  _DrawIndexedPrimitives (uint indexCount,
									  uint instanceCount,
									  uint firstIndex,
									  int  vertexOffset,
									  uint firstInstance);
		
		void  _DrawPrimitivesIndirect (MetalBuffer	indirectBuffer,
									   Bytes		indirectBufferOffset,
									   uint			drawCount,
									   Bytes		stride);

		void  _DrawIndexedPrimitivesIndirect (MetalBuffer	indirectBuffer,
											  Bytes			indirectBufferOffset,
											  uint			drawCount,
											  Bytes			stride);

		void  _DrawPatches ();
		void  _DrawIndexedPatches ();
		void  _DrawPatchesIndirect ();
		void  _DrawIndexedPatchesIndirect ();

		void  _DispatchTile ();
		
		ND_ MetalCommandEncoder  _BaseEncoder ();

		void  _DebugMarker (NtStringView text, RGBA8u);
		void  _PushDebugGroup (NtStringView text, RGBA8u);
		void  _PopDebugGroup ();

		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	const;
		ND_ auto&	_GetFeatures ()			const;
	};



	//
	// Metal Draw Context implementation
	//

	template <typename CtxImpl>
	class _MDrawContextImpl : public CtxImpl, public IDrawContext
	{
		friend class _MIndirectGraphicsCtx;

	// types
	public:
		static constexpr bool	IsDrawContext		= true;
		static constexpr bool	IsMetalDrawContext	= true;
		
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumDrawBarriers< _MDrawContextImpl< CtxImpl >>;
		using CmdBuf_t	= typename CtxImpl::CmdBuf_t;

		
	// methods
	private:
		explicit _MDrawContextImpl (CmdBuf_t cmdbuf);

	public:
		explicit _MDrawContextImpl (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName = Default);
		explicit _MDrawContextImpl (Ptr<MDrawCommandBatch> batch);
		
		_MDrawContextImpl () = delete;
		_MDrawContextImpl (const _MDrawContextImpl &) = delete;
		~_MDrawContextImpl () override {}

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln) override final;
		void  BindPipeline (MeshPipelineID ppln) override final;
		void  BindPipeline (TilePipelineID ppln) override final;

		//using RawCtx::BindDescriptorSet;

		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		
		// dynamic states
		void  SetViewport (const Viewport_t &viewport) override final			{ RawCtx::_SetViewport( viewport ); }
		void  SetViewports (ArrayView<Viewport_t> viewports) override final		{ RawCtx::_SetViewport( viewports ); }
		void  SetScissor (const RectI &scissor) override final					{ RawCtx::_SetScissor( scissor ); }
		void  SetScissors (ArrayView<RectI> scissors) override final			{ RawCtx::_SetScissor( scissors ); }
		void  SetBlendConstants (const RGBA32f &color) override final			{ RawCtx::_SetBlendConstants( color ); }
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override final;
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds) override final;
		
		// draw commands
		using RawCtx::BindIndexBuffer;

		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) override final;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset) override final;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) override final;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset) override final;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0) override final;

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0) override final;

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride) override final;

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride) override final;

		// mesh shader
		// TODO
		
		// tile shader
		void  DispatchTile () override final;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }

		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	override final	{ RawCtx::AttachmentBarrier( name, srcState, dstState ); }
		
		ND_ AccumBar  AccumBarriers ()		{ return AccumBar{ *this }; }
		
		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result) override final;
		
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return this->_mngr.GetPrimaryCtxState(); }


	private:
		using RawCtx::_GetFeatures;
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectDrawContext	= _hidden_::_MDrawContextImpl< _hidden_::_MDirectDrawCtx >;
	using MIndirectDrawContext	= _hidden_::_MDrawContextImpl< _hidden_::_MIndirectDrawCtx >;

} // AE::Graphics

#endif // AE_ENABLE_METAL
