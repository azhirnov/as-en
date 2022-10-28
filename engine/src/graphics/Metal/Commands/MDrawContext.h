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
		using CmdBuf_t = MCommandBuffer;

		struct VertexAmplificationViewMapping
		{
			uint	renderTargetArrayIndexOffset;
			uint	viewportArrayIndexOffset;
		};

		enum class EPplnType : ubyte
		{
			Unknown,
			Graphics,
			Mesh,
			Tile,
		};

		struct GraphicsPplnStates
		{
			MetalBuffer		indexBuffer;
			Bytes			indexBufferOffset;
			ushort			primitiveType		= UMax;
			ushort			indexType			= UMax;
		};

		struct MeshPplnStates
		{
			packed_ushort3	threadsPerObjectThreadgroup;
			packed_ushort3	threadsPerMeshThreadgroup;
		};

		struct TilePplnStates
		{
			packed_ushort2	threadsPerTile;
		};
		
		struct PplnStates
		{
			EPipelineDynamicState	flags	= Default;
			EPplnType				type	= Default;
			union {
				GraphicsPplnStates	graphics;
				MeshPplnStates		mesh;
				TilePplnStates		tile;
			};
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
		PplnStates					_states;
	protected:
		MDrawBarrierManager			_mngr;


	// methods
	public:
		ND_ VertexArgSet_t		VertexArguments ()				{ return VertexArgSet_t{ _encoder }; }
		ND_ FragmentArgSet_t	FragmentArguments ()			{ return FragmentArgSet_t{ _encoder }; }
		ND_ TileArgSet_t		TileArguments ()				{ return TileArgSet_t{ _encoder }; }

	  #if AE_METAL_MESH_SHADER
		ND_ MeshArgSet_t		MeshArguments ()				{ return MeshArgSet_t{ _encoder }; }
		ND_ MeshTaskArgSet_t	MeshTaskArguments ()			{ return MeshTaskArgSet_t{ _encoder }; }
	  #endif

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
		explicit _MDirectDrawCtx (Ptr<MDrawCommandBatch> batch);
		_MDirectDrawCtx (Ptr<MDrawCommandBatch> batch, CmdBuf_t cmdbuf);
		_MDirectDrawCtx (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName);
		
		void  _BindPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs);
		void  _BindGraphicsPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags);
		void  _BindMeshPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags,
								 const uint3 &threadsPerObjectThreadgroup, const uint3 &threadsPerMeshThreadgroup);
		void  _BindTilePipeline (MetalRenderPipeline ppln, const uint2 &threadsPerTile);
		void  _BindDescriptorSet (uint index, const MDescriptorSet &ds, ArrayView<uint> dynamicOffsets);

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

		void  _DrawMeshThreadgroups (const uint3 &threadgroupsPerGrid,
									 const uint3 &threadsPerObjectThreadgroup,
									 const uint3 &threadsPerMeshThreadgroup);

		void  _DrawMeshThreads (const uint3 &threadsPerGrid,
								const uint3 &threadsPerObjectThreadgroup,
								const uint3 &threadsPerMeshThreadgroup);

		void  _DrawMeshThreadgroupsIndirect (MetalBuffer	indirectBuffer,
											 Bytes			indirectBufferOffset,
											 const uint3	&threadsPerObjectThreadgroup,
											 const uint3	&threadsPerMeshThreadgroup);

		void  _DrawMeshTasks (const uint3 &taskCount);

		void  _DrawMeshTasksIndirect (MetalBuffer	indirectBuffer,
									  Bytes			indirectBufferOffset,
									  uint			drawCount,
									  Bytes			stride);

		void  _DrawPatches ();
		void  _DrawIndexedPatches ();
		void  _DrawPatchesIndirect ();
		void  _DrawIndexedPatchesIndirect ();

		void  _DispatchTile ()								{ _DispatchThreadsPerTile( _states.tile.threadsPerTile ); }
		void  _DispatchThreadsPerTile (const uint2 &threadsPerTile);
		
		ND_ MetalCommandEncoder  _BaseEncoder ()			{ return MetalCommandEncoder{ _encoder.Ptr() }; }

		void  _DebugMarker (NtStringView text, RGBA8u)		{ _MBaseDirectContext::_DebugMarker( _BaseEncoder(), text ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u)	{ _MBaseDirectContext::_PushDebugGroup( _BaseEncoder(), text ); }
		void  _PopDebugGroup ()								{ _MBaseDirectContext::_PopDebugGroup( _BaseEncoder() ); }

		void  _CommitBarriers ();
		void  _AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

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
		using CmdBuf_t							= MSoftwareCmdBufPtr;
		using VertexAmplificationViewMapping	= _MDirectDrawCtx::VertexAmplificationViewMapping;
		using PplnStates						= _MDirectDrawCtx::PplnStates;
	private:
		using VertexArgSet_t	= MArgumentSetter< EShader::Vertex,		false >;
		using FragmentArgSet_t	= MArgumentSetter< EShader::Fragment,	false >;
		using TileArgSet_t		= MArgumentSetter< EShader::Tile,		false >;
		using MeshArgSet_t		= MArgumentSetter< EShader::Mesh,		false >;
		using MeshTaskArgSet_t	= MArgumentSetter< EShader::MeshTask,	false >;
		
		using Viewport_t = RenderPassDesc::Viewport;


	// variables
	private:
		PplnStates				_states;
	protected:
		MDrawBarrierManager		_mngr;


	// methods
	public:
		ND_ VertexArgSet_t		VertexArguments ()				{ return VertexArgSet_t{ this->_cmdbuf.get() }; }
		ND_ FragmentArgSet_t	FragmentArguments ()			{ return FragmentArgSet_t{ this->_cmdbuf.get() }; }
		ND_ TileArgSet_t		TileArguments ()				{ return TileArgSet_t{ this->_cmdbuf.get() }; }

	  #if AE_METAL_MESH_SHADER
		ND_ MeshArgSet_t		MeshArguments ()				{ return MeshArgSet_t{ this->_cmdbuf.get() }; }
		ND_ MeshTaskArgSet_t	MeshTaskArguments ()			{ return MeshTaskArgSet_t{ this->_cmdbuf.get() }; }
	  #endif

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
		explicit _MIndirectDrawCtx (Ptr<MDrawCommandBatch> batch);
		_MIndirectDrawCtx (Ptr<MDrawCommandBatch> batch, CmdBuf_t cmdbuf);
		_MIndirectDrawCtx (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName);
		
		void  _BindPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs);
		void  _BindGraphicsPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags);
		void  _BindMeshPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags,
								 const uint3 &threadsPerObjectThreadgroup, const uint3 &threadsPerMeshThreadgroup);
		void  _BindTilePipeline (MetalRenderPipeline ppln, const uint2 &threadsPerTile);
		void  _BindDescriptorSet (uint index, const MDescriptorSet &ds, ArrayView<uint> dynamicOffsets);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetStencilReference (uint reference)								{ _SetStencilReference( reference, reference ); }
		void  _SetStencilReference (uint frontReference, uint backReference);
		void  _SetViewport (const Viewport_t &viewport)							{ _SetViewport({ viewport }); }
		void  _SetViewport (ArrayView<Viewport_t> viewports);
		void  _SetScissor (const RectI &scissor)								{ _SetScissor({ scissor }); }
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
		
		void  _DrawMeshThreadgroups (const uint3 &threadgroupsPerGrid,
									 const uint3 &threadsPerObjectThreadgroup,
									 const uint3 &threadsPerMeshThreadgroup);

		void  _DrawMeshThreads (const uint3 &threadsPerGrid,
								const uint3 &threadsPerObjectThreadgroup,
								const uint3 &threadsPerMeshThreadgroup);

		void  _DrawMeshThreadgroupsIndirect (MetalBuffer	indirectBuffer,
											 Bytes			indirectBufferOffset,
											 const uint3	&threadsPerObjectThreadgroup,
											 const uint3	&threadsPerMeshThreadgroup);

		void  _DrawMeshTasks (const uint3 &taskCount);

		void  _DrawMeshTasksIndirect (MetalBuffer	indirectBuffer,
									  Bytes			indirectBufferOffset,
									  uint			drawCount,
									  Bytes			stride);

		void  _DrawPatches ();
		void  _DrawIndexedPatches ();
		void  _DrawPatchesIndirect ();
		void  _DrawIndexedPatchesIndirect ();
		
		void  _DispatchTile ()								{ _DispatchThreadsPerTile( _states.tile.threadsPerTile ); }
		void  _DispatchThreadsPerTile (const uint2 &threadsPerTile);

		void  _CommitBarriers ();
		void  _AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()			const	{ return _mngr.GetDevice().GetFeatures(); }
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
		explicit _MDrawContextImpl (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName);
		explicit _MDrawContextImpl (Ptr<MDrawCommandBatch> batch);
		
		_MDrawContextImpl () = delete;
		_MDrawContextImpl (const _MDrawContextImpl &) = delete;
		~_MDrawContextImpl () override {}

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln) override final;
		void  BindPipeline (MeshPipelineID ppln) override final;
		void  BindPipeline (TilePipelineID ppln) override final;

		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		
		// dynamic states
		void  SetViewport (const Viewport_t &viewport) override final				{ RawCtx::_SetViewport( viewport ); }
		void  SetViewports (ArrayView<Viewport_t> viewports) override final			{ RawCtx::_SetViewport( viewports ); }
		void  SetScissor (const RectI &scissor) override final						{ RawCtx::_SetScissor( scissor ); }
		void  SetScissors (ArrayView<RectI> scissors) override final				{ RawCtx::_SetScissor( scissors ); }
		void  SetBlendConstants (const RGBA32f &color) override final				{ RawCtx::_SetBlendConstants( color ); }
		void  SetStencilReference (uint reference) override final					{  RawCtx::_SetStencilReference( reference ); }
		void  SetStencilReference (uint frontRef, uint backRef) override final		{  RawCtx::_SetStencilReference( frontRef, backRef ); }
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override final;
		
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
		
		// tile shader
		void  DispatchTile () override final;
		
		// mesh shader
		void  DrawMeshTasks (const uint3 &taskCount) override final;

		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride) override final;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }

		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	override final	{ RawCtx::_AttachmentBarrier( name, srcState, dstState ); }
		
		ND_ AccumBar  AccumBarriers ()		{ return AccumBar{ *this }; }
		
		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result) override final;
		
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return this->_mngr.GetPrimaryCtxState(); }
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectDrawContext	= _hidden_::_MDrawContextImpl< _hidden_::_MDirectDrawCtx >;
	using MIndirectDrawContext	= _hidden_::_MDrawContextImpl< _hidden_::_MIndirectDrawCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
		
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_MDrawContextImpl<C>::_MDrawContextImpl (CmdBuf_t cmdbuf) :
		RawCtx{ null, RVRef(cmdbuf) }
	{}

	template <typename C>
	_MDrawContextImpl<C>::_MDrawContextImpl (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName) :
		RawCtx{ state, RVRef(cmdbuf), dbgName }
	{}
	
	template <typename C>
	_MDrawContextImpl<C>::_MDrawContextImpl (Ptr<MDrawCommandBatch> batch) :
		RawCtx{ batch }
	{
		if_likely( batch )
		{
			if_likely( not batch->GetViewports().empty() )
				RawCtx::_SetViewport( batch->GetViewports() );
				
			if_likely( not batch->GetScissors().empty() )
				RawCtx::_SetScissor( batch->GetScissors() );
		}
	}
	
/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::BindPipeline (GraphicsPipelineID ppln)
	{
		auto*	gppln = this->_mngr.Get( ppln );
		CHECK_ERRV( gppln );

		RawCtx::_BindGraphicsPipeline( gppln->Handle(), gppln->DepthStencilState(), gppln->GetRenderState(), gppln->DynamicState() );
	}
	
	template <typename C>
	void  _MDrawContextImpl<C>::BindPipeline (MeshPipelineID ppln)
	{
		auto*	mppln = this->_mngr.Get( ppln );
		CHECK_ERRV( mppln );
		
		RawCtx::_BindMeshPipeline( mppln->Handle(), mppln->DepthStencilState(), mppln->GetRenderState(), mppln->DynamicState(),
								   mppln->TaskLocalSize(), mppln->MeshLocalSize() );
	}
	
	template <typename C>
	void  _MDrawContextImpl<C>::BindPipeline (TilePipelineID ppln)
	{
		auto*	tppln = this->_mngr.Get( ppln );
		CHECK_ERRV( tppln );
		
		RawCtx::_BindTilePipeline( tppln->Handle(), tppln->LocalSize() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		auto*	desc_set = this->_mngr.Get( ds );
		CHECK_ERRV( desc_set );

		RawCtx::_BindDescriptorSet( index, *desc_set, dynamicOffsets );
	}
	
/*
=================================================
	PushConstant
=================================================
*
	template <typename C>
	void  _MDrawContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( IsAligned( size, sizeof(uint) ));

		RawCtx::_PushGraphicsConstant( offset, size, values, stages );
	}
	
/*
=================================================
	SetDepthBias
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
	{
		RawCtx::_SetDepthBias( depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}
	
/*
=================================================
	BindIndexBuffer
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)
	{
		auto*	buf = this->_mngr.Get( buffer );
		CHECK_ERRV( buf );

		return BindIndexBuffer( buf->Handle(), offset, indexType );
	}
	
/*
=================================================
	BindVertexBuffer
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::BindVertexBuffer (uint index, BufferID buffer, Bytes offset)
	{
		auto*	buf = this->_mngr.Get( buffer );
		CHECK_ERRV( buf );

		RawCtx::BindVertexBuffer( index, buf->Handle(), offset );
	}
	
	template <typename C>
	bool  _MDrawContextImpl<C>::BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)
	{
		auto*	ppln = this->_mngr.Get( pplnId );
		CHECK_ERR( ppln );

		uint	idx = ppln->GetVertexBufferIndex( name );
		CHECK_ERR( idx != UMax );

		auto*	buf = this->_mngr.Get( buffer );
		CHECK_ERR( buf );
		
		RawCtx::BindVertexBuffer( idx, buf->Handle(), offset );
		return true;
	}

	template <typename C>
	void  _MDrawContextImpl<C>::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)
	{
		StaticArray< MetalBuffer, GraphicsConfig::MaxVertexBuffers >	dst_buffers;

		for (uint i = 0; i < buffers.size(); ++i)
		{
			auto*	buffer	= this->_mngr.Get( buffers[i] );
			CHECK_ERRV( buffer );

			dst_buffers[i] = buffer->Handle();
		}

		RawCtx::BindVertexBuffers( firstBinding, dst_buffers, offsets );
	}

/*
=================================================
	Draw
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::Draw (uint vertexCount,
									  uint instanceCount,
									  uint firstVertex,
									  uint firstInstance)
	{
		RawCtx::_DrawPrimitives( vertexCount, instanceCount, firstVertex, firstInstance );
	}
	
/*
=================================================
	DrawIndexed
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::DrawIndexed (uint indexCount,
											 uint instanceCount,
											 uint firstIndex,
											 int  vertexOffset,
											 uint firstInstance)
	{
		RawCtx::_DrawIndexedPrimitives( indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	}
	
/*
=================================================
	DrawIndirect
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::DrawIndirect (BufferID	indirectBuffer,
											  Bytes		indirectBufferOffset,
											  uint		drawCount,
											  Bytes		stride)
	{
		auto*	buf = this->_mngr.Get( indirectBuffer );
		CHECK_ERRV( buf );

		RawCtx::_DrawPrimitivesIndirect( buf->Handle(), indirectBufferOffset, drawCount, stride );
	}
	
/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::DrawIndexedIndirect (BufferID	indirectBuffer,
													 Bytes		indirectBufferOffset,
													 uint		drawCount,
													 Bytes		stride)
	{
		auto*	buf = this->_mngr.Get( indirectBuffer );
		CHECK_ERRV( buf );

		RawCtx::_DrawIndexedPrimitivesIndirect( buf->Handle(), indirectBufferOffset, drawCount, stride );
	}
	
/*
=================================================
	DispatchTile
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::DispatchTile ()
	{
		RawCtx::_DispatchTile();
	}
	
/*
=================================================
	DrawMeshTasks
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::DrawMeshTasks (const uint3 &taskCount)
	{
		RawCtx::_DrawMeshTasks( taskCount );
	}
	
/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::DrawMeshTasksIndirect (BufferID	indirectBuffer,
													   Bytes	indirectBufferOffset,
													   uint		drawCount,
													   Bytes	stride)
	{
		auto*	buf = this->_mngr.Get( indirectBuffer );
		CHECK_ERRV( buf );

		RawCtx::_DrawMeshTasksIndirect( buf->Handle(), indirectBufferOffset, drawCount, stride );
	}

/*
=================================================
	AllocVStream
=================================================
*/
	template <typename C>
	bool  _MDrawContextImpl<C>::AllocVStream (Bytes size, OUT VertexStream &result)
	{
		return this->_mngr.GetResourceManager().GetStagingManager().AllocVStream( this->_mngr.GetFrameId(), size, OUT result );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
