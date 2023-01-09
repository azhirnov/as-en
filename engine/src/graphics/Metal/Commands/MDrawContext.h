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
		using CmdBuf_t	= MCommandBuffer;

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
			
			PplnStates () {}
		};

	private:
		using VertexArgSet_t	= MArgumentSetter< EShader::Vertex,		true >;
		using FragmentArgSet_t	= MArgumentSetter< EShader::Fragment,	true >;
		using TileArgSet_t		= MArgumentSetter< EShader::Tile,		true >;
		using MeshArgSet_t		= MArgumentSetter< EShader::Mesh,		true >;
		using MeshTaskArgSet_t	= MArgumentSetter< EShader::MeshTask,	true >;

		using Viewport_t		= RenderPassDesc::Viewport;

		
	// variables
	private:
		PplnStates				_states;
	protected:
		MDrawBarrierManager		_mngr;


	// methods
	public:
		ND_ VertexArgSet_t		VertexArguments ()															__NE___	{ ASSERT( _states.type == EPplnType::Graphics );return VertexArgSet_t{ _Encoder2() }; }
		ND_ FragmentArgSet_t	FragmentArguments ()														__NE___	{ ASSERT( _states.type != EPplnType::Tile );	return FragmentArgSet_t{ _Encoder2() }; }
		ND_ TileArgSet_t		TileArguments ()															__NE___	{ ASSERT( _states.type == EPplnType::Tile );	return TileArgSet_t{ _Encoder2() }; }

	  #if AE_METAL_3_0_BETA
		ND_ MeshArgSet_t		MeshArguments ()															__NE___	{ ASSERT( _states.type == EPplnType::Mesh );	return MeshArgSet_t{ _Encoder2() }; }
		ND_ MeshTaskArgSet_t	MeshTaskArguments ()														__NE___	{ ASSERT( _states.type == EPplnType::Mesh );	return MeshTaskArgSet_t{ _Encoder2() }; }
	  #endif

		void  SetDepthStencilState (MetalDepthStencilState ds)												__Th___;

		// extended dynamic states
		void  SetPolygonMode (EPolygonMode value)															__Th___;
		void  SetCullMode (ECullMode value)																	__Th___;
		void  SetFrontFacing (bool ccw)																		__Th___;
		void  SetDepthClamp (bool value)																	__Th___;

		// tessellation
		void  SetTessellationFactor (MetalBuffer buffer, Bytes offset, Bytes instanceStride)				__Th___;
		void  SetTessellationFactorScale (float value)														__Th___;

		void  SetVertexAmplification (uint count)															__Th___;
		void  SetVertexAmplification (ArrayView<VertexAmplificationViewMapping> mapping)					__Th___;

		//void  SetVisibilityResultMode ();
		
		void  BindIndexBuffer (MetalBuffer buffer, Bytes offset, EIndex indexType)							__Th___;
		void  BindVertexBuffer (uint index, MetalBuffer buffer, Bytes offset)								__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<MetalBuffer> buffers, ArrayView<Bytes> offsets)__Th___;
		
		ND_ bool  					EndEncoding ()															__Th___;
		ND_ MetalCommandBufferRC	EndCommandBuffer ()														__Th___;
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ()													__Th___;

	protected:
		_MDirectDrawCtx (const DrawTask &task)																__Th___;
		_MDirectDrawCtx (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg)					__Th___;

		void  _BindPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs);
		void  _BindGraphicsPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags);
		void  _BindMeshPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags,
								 const uint3 &threadsPerObjectThreadgroup, const uint3 &threadsPerMeshThreadgroup);
		void  _BindTilePipeline (MetalRenderPipeline ppln, const uint2 &threadsPerTile);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetStencilReference (uint reference);
		void  _SetStencilReference (uint frontReference, uint backReference);
		void  _SetViewport (const Viewport_t &viewport);
		void  _SetViewports (ArrayView<Viewport_t> viewports);
		void  _SetScissor (const RectI &scissor);
		void  _SetScissors (ArrayView<RectI> scissors);
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

		void  _DispatchTile ()									{ _DispatchThreadsPerTile( uint2{_states.tile.threadsPerTile} ); }
		void  _DispatchThreadsPerTile (const uint2 &threadsPerTile);
		
		ND_ bool						_IsValid ()		C_NE___	{ return _cmdbuf.HasEncoder(); }
		ND_ auto						_Encoder ()		__NE___;
		ND_ MetalRenderCommandEncoder	_Encoder2 ()	__NE___	{ return MetalRenderCommandEncoder{ _cmdbuf.GetEncoder().Ptr() }; }

		void  _DebugMarker (DebugLabel dbg)						{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)					{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()									{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup(); }

		void  _CommitBarriers ();
		void  _AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

		ND_ bool	_NoPendingBarriers ()				C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()						C_NE___	{ return _mngr.GetDevice().GetFeatures(); }
		ND_ auto&	_GetDeviceProperties ()				C_NE___	{ return _mngr.GetDevice().GetDeviceProperties(); }
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
		using EPplnType							= _MDirectDrawCtx::EPplnType;
	private:
		using VertexArgSet_t	= MArgumentSetter< EShader::Vertex,		false >;
		using FragmentArgSet_t	= MArgumentSetter< EShader::Fragment,	false >;
		using TileArgSet_t		= MArgumentSetter< EShader::Tile,		false >;
		using MeshArgSet_t		= MArgumentSetter< EShader::Mesh,		false >;
		using MeshTaskArgSet_t	= MArgumentSetter< EShader::MeshTask,	false >;
		
		using Viewport_t		= RenderPassDesc::Viewport;


	// variables
	private:
		PplnStates				_states;
	protected:
		MDrawBarrierManager		_mngr;


	// methods
	public:
		ND_ VertexArgSet_t		VertexArguments ()															__NE___	{ ASSERT( _states.type == EPplnType::Graphics );return VertexArgSet_t{ this->_cmdbuf.get() }; }
		ND_ FragmentArgSet_t	FragmentArguments ()														__NE___	{ ASSERT( _states.type != EPplnType::Tile );	return FragmentArgSet_t{ this->_cmdbuf.get() }; }
		ND_ TileArgSet_t		TileArguments ()															__NE___	{ ASSERT( _states.type == EPplnType::Tile );	return TileArgSet_t{ this->_cmdbuf.get() }; }

	  #if AE_METAL_3_0_BETA
		ND_ MeshArgSet_t		MeshArguments ()															__NE___	{ ASSERT( _states.type == EPplnType::Mesh );	return MeshArgSet_t{ this->_cmdbuf.get() }; }
		ND_ MeshTaskArgSet_t	MeshTaskArguments ()														__NE___	{ ASSERT( _states.type == EPplnType::Mesh );	return MeshTaskArgSet_t{ this->_cmdbuf.get() }; }
	  #endif

		void  SetDepthStencilState (MetalDepthStencilState ds)												__Th___;

		// extended dynamic states
		void  SetPolygonMode (EPolygonMode value)															__Th___;
		void  SetCullMode (ECullMode value)																	__Th___;
		void  SetFrontFacing (bool ccw)																		__Th___;
		void  SetDepthClamp (bool value)																	__Th___;

		// tessellation
		void  SetTessellationFactor (MetalBuffer buffer, Bytes offset, Bytes instanceStride)				__Th___;
		void  SetTessellationFactorScale (float value)														__Th___;

		void  SetVertexAmplification (uint count)															__Th___;
		void  SetVertexAmplification (ArrayView<VertexAmplificationViewMapping> mapping)					__Th___;

		//void  SetVisibilityResultMode ();
		
		void  BindIndexBuffer (MetalBuffer buffer, Bytes offset, EIndex indexType)							__Th___;
		void  BindVertexBuffer (uint index, MetalBuffer buffer, Bytes offset)								__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<MetalBuffer> buffers, ArrayView<Bytes> offsets)__Th___;
		
		ND_ MBakedCommands		EndCommandBuffer ()															__Th___;
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ()														__Th___;

	protected:
		explicit _MIndirectDrawCtx (const DrawTask &task)													__Th___;
		_MIndirectDrawCtx (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg)				__Th___;
		
		void  _BindPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs);
		void  _BindGraphicsPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags);
		void  _BindMeshPipeline (MetalRenderPipeline ppln, MetalDepthStencilState ds, const MDynamicRenderState &rs, EPipelineDynamicState flags,
								 const uint3 &threadsPerObjectThreadgroup, const uint3 &threadsPerMeshThreadgroup);
		void  _BindTilePipeline (MetalRenderPipeline ppln, const uint2 &threadsPerTile);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetStencilReference (uint reference)								{ _SetStencilReference( reference, reference ); }
		void  _SetStencilReference (uint frontReference, uint backReference);
		void  _SetViewport (const Viewport_t &viewport)							{ _SetViewports({ viewport }); }
		void  _SetViewports (ArrayView<Viewport_t> viewports);
		void  _SetScissor (const RectI &scissor)								{ _SetScissors({ scissor }); }
		void  _SetScissors (ArrayView<RectI> scissors);
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
		
		void  _DispatchTile ()									{ _DispatchThreadsPerTile( uint2{_states.tile.threadsPerTile} ); }
		void  _DispatchThreadsPerTile (const uint2 &threadsPerTile);

		void  _CommitBarriers ();
		void  _AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

		ND_ bool	_NoPendingBarriers ()				C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()						C_NE___	{ return _mngr.GetDevice().GetFeatures(); }
		ND_ auto&	_GetDeviceProperties ()				C_NE___	{ return _mngr.GetDevice().GetDeviceProperties(); }
	};



	//
	// Metal Draw Context implementation
	//

	template <typename CtxImpl>
	class _MDrawContextImpl : public CtxImpl, public IDrawContext
	{
	// types
	public:
		static constexpr bool	IsDrawContext		= true;
		static constexpr bool	IsMetalDrawContext	= true;
		
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= MAccumDrawBarriers< _MDrawContextImpl< CtxImpl >>;
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
		using Viewports_t	= ArrayView< RenderPassDesc::Viewport >;
		using BoundDS_t		= StaticArray< DescriptorSetID, GraphicsConfig::MaxDescriptorSets >;


	// variables
	private:
		BoundDS_t	_boundDS	{};

		
	// methods
	public:
		_MDrawContextImpl (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, Viewports_t, DebugLabel dbg)				__Th___;
		explicit _MDrawContextImpl (const DrawTask &task)																__Th___;
		
		_MDrawContextImpl ()																							= delete;
		_MDrawContextImpl (const _MDrawContextImpl &)																	= delete;

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln)																	__Th_OV;
		void  BindPipeline (MeshPipelineID ppln)																		__Th_OV;
		void  BindPipeline (TilePipelineID ppln)																		__Th_OV;

		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)	__Th_OV;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)							__Th_OV;
		
		// dynamic states
		void  SetViewport (const Viewport_t &viewport)																	__Th_OV	{ RawCtx::_SetViewport( viewport ); }
		void  SetViewports (ArrayView<Viewport_t> viewports)															__Th_OV	{ RawCtx::_SetViewports( viewports ); }
		void  SetScissor (const RectI &scissor)																			__Th_OV	{ RawCtx::_SetScissor( scissor ); }
		void  SetScissors (ArrayView<RectI> scissors)																	__Th_OV	{ RawCtx::_SetScissors( scissors ); }
		void  SetBlendConstants (const RGBA32f &color)																	__Th_OV	{ RawCtx::_SetBlendConstants( color ); }
		void  SetStencilReference (uint reference)																		__Th_OV	{ RawCtx::_SetStencilReference( reference ); }
		void  SetStencilReference (uint frontRef, uint backRef)															__Th_OV	{ RawCtx::_SetStencilReference( frontRef, backRef ); }
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)			__Th_OV;
		
		// draw commands
		using RawCtx::BindIndexBuffer;
		using RawCtx::BindVertexBuffers;

		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)											__Th_OV;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)												__Th_OV;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)				__Th_OV;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)	__Th_OV;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0)																			__Th_OV;

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0)																	__Th_OV;

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																			__Th_OV;

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																	__Th_OV;
		
		// tile shader
		void  DispatchTile ()																							__Th_OV;
		
		// mesh shader
		void  DrawMeshTasks (const uint3 &taskCount)																	__Th_OV;

		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																	__Th_OV;

		void  CommitBarriers ()																							__Th_OV	{ RawCtx::_CommitBarriers(); }

		void  DebugMarker (DebugLabel dbg)																				__Th_OV	{ RawCtx::_DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																			__Th_OV	{ RawCtx::_PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																							__Th_OV	{ RawCtx::_PopDebugGroup(); }
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)					__Th_OV	{ RawCtx::_AttachmentBarrier( name, srcState, dstState ); }
		
		ND_ AccumBar  AccumBarriers ()																							{ return AccumBar{ *this }; }
		
		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result)													__Th_OV;
		
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()															C_NE___	{ return this->_mngr.GetPrimaryCtxState(); }
		
		ND_ bool  _IsValid ()																							C_NE___	{ return RawCtx::_IsValid(); }

	private:
		template <typename ...IDs>
		ND_ decltype(auto)  _GetResourcesOrThrow (IDs ...ids)															__Th___	{ return this->_mngr.GetResourceManager().GetResourcesOrThrow( ids... ); }
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectDrawContext	= Graphics::_hidden_::_MDrawContextImpl< Graphics::_hidden_::_MDirectDrawCtx >;
	using MIndirectDrawContext	= Graphics::_hidden_::_MDrawContextImpl< Graphics::_hidden_::_MIndirectDrawCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
		
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_MDrawContextImpl<C>::_MDrawContextImpl (const MPrimaryCmdBufState &state, CmdBuf_t cmdbuf, Viewports_t viewports, DebugLabel dbg) :
		RawCtx{ state, RVRef(cmdbuf), dbg }	// throw
	{
		SetViewports( viewports );
	}
	
	template <typename C>
	_MDrawContextImpl<C>::_MDrawContextImpl (const DrawTask &task) :
		RawCtx{ task }	// throw
	{
		if_likely( auto* batch = task.GetDrawBatchPtr() )
		{
			if_likely( not batch->GetViewports().empty() )
				SetViewports( batch->GetViewports() );
				
			if_likely( not batch->GetScissors().empty() )
				SetScissors( batch->GetScissors() );
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
		auto&	gppln = _GetResourcesOrThrow( ppln );
		ASSERT( gppln.RasterOrderGroup() == GetPrimaryCtxState().rasterOrderGroup );

		RawCtx::_BindGraphicsPipeline( gppln.Handle(), gppln.DepthStencilState(), gppln.GetRenderState(), gppln.DynamicState() );
	}
	
	template <typename C>
	void  _MDrawContextImpl<C>::BindPipeline (MeshPipelineID ppln)
	{
		auto&	mppln = _GetResourcesOrThrow( ppln );
		ASSERT( mppln.RasterOrderGroup() == GetPrimaryCtxState().rasterOrderGroup );
		
		RawCtx::_BindMeshPipeline( mppln.Handle(), mppln.DepthStencilState(), mppln.GetRenderState(), mppln.DynamicState(),
								   mppln.TaskLocalSize(), mppln.MeshLocalSize() );
	}
	
	template <typename C>
	void  _MDrawContextImpl<C>::BindPipeline (TilePipelineID ppln)
	{
		auto&	tppln = _GetResourcesOrThrow( ppln );
		ASSERT( tppln.RasterOrderGroup() == GetPrimaryCtxState().rasterOrderGroup );
		
		RawCtx::_BindTilePipeline( tppln.Handle(), tppln.LocalSize() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::BindDescriptorSet (const DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		const bool	is_bound = (_boundDS[ index.mtlIndex.BindingIndex() ] == ds);
		_boundDS[ index.mtlIndex.BindingIndex() ] = ds;
		
		if_unlikely( is_bound and dynamicOffsets.empty() )
			return;

		auto&	desc_set	= _GetResourcesOrThrow( ds );
		auto	dyn_bufs	= desc_set.GetDynamicBuffers();

		CHECK_THROW( dyn_bufs.size() == dynamicOffsets.size() );

		const auto	BindDynBufs = [&] (auto && args, uint idx)
		{{
			if ( is_bound )
			{
				for (usize i = 0; i < dynamicOffsets.size(); ++i, ++idx)
				{
					args.SetBufferOffset( Bytes{dynamicOffsets[i]}, MBufferIndex(idx) );
				}
			}
			else
			{
				for (usize i = 0; i < dynamicOffsets.size(); ++i, ++idx)
				{
					args.SetBuffer( dyn_bufs.at<MetalBuffer>(i), dyn_bufs.at<Bytes>(i), MBufferIndex(idx) );
					args.SetBufferOffset( Bytes{dynamicOffsets[i]}, MBufferIndex(idx) );
				}
				args.SetBuffer( desc_set.Handle(), 0_b, MBufferIndex(idx) );
			}
		}};
		
		for (EShaderStages stages = desc_set.ShaderStages(); stages != Default;)
		{
			const EShader	type = ExtractBitLog2<EShader>( INOUT stages );
			switch ( type )
			{
				case EShader::Vertex :		BindDynBufs( this->VertexArguments(),	index.mtlIndex.Vertex() );		break;
				case EShader::Fragment :	BindDynBufs( this->FragmentArguments(),	index.mtlIndex.Fragment() );	break;
				case EShader::MeshTask :	BindDynBufs( this->MeshTaskArguments(),	index.mtlIndex.MeshTask() );	break;
				case EShader::Mesh :		BindDynBufs( this->MeshArguments(),		index.mtlIndex.Mesh() );		break;
				case EShader::Tile :		BindDynBufs( this->TileArguments(),		index.mtlIndex.Tile() );		break;
				default :					CHECK_THROW(false);
			}
		}
	}
	
/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( IsAligned( size, sizeof(uint) ));
		Unused( offset, size, values, stages );

		// TODO
		//RawCtx::_PushGraphicsConstant( offset, size, values, stages );
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
		auto&	buf = _GetResourcesOrThrow( buffer );

		return BindIndexBuffer( buf.Handle(), offset, indexType );
	}
	
/*
=================================================
	BindVertexBuffer
=================================================
*/
	template <typename C>
	void  _MDrawContextImpl<C>::BindVertexBuffer (uint index, BufferID buffer, Bytes offset)
	{
		auto&	buf = _GetResourcesOrThrow( buffer );

		RawCtx::BindVertexBuffer( index, buf.Handle(), offset );
	}
	
	template <typename C>
	bool  _MDrawContextImpl<C>::BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)
	{
		auto  [ppln, buf] = _GetResourcesOrThrow( pplnId, buffer );

		uint	idx = ppln.GetVertexBufferIndex( name );
		CHECK_ERR( idx != UMax );
		
		RawCtx::BindVertexBuffer( idx, buf.Handle(), offset );
		return true;
	}

	template <typename C>
	void  _MDrawContextImpl<C>::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)
	{
		StaticArray< MetalBuffer, GraphicsConfig::MaxVertexBuffers >	dst_buffers;

		for (uint i = 0; i < buffers.size(); ++i)
		{
			auto&	buffer = _GetResourcesOrThrow( buffers[i] );

			dst_buffers[i] = buffer.Handle();
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
		auto&	buf = _GetResourcesOrThrow( indirectBuffer ); 

		RawCtx::_DrawPrimitivesIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
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
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );

		RawCtx::_DrawIndexedPrimitivesIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
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
		auto&	buf = _GetResourcesOrThrow( indirectBuffer ); 

		RawCtx::_DrawMeshTasksIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
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
