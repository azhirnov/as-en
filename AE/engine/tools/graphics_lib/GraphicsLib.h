// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe:  ???
*/

#pragma once

#include "graphics/Public/CommandBuffer.h"
#include "graphics/Public/NativeWindow.h"
#include "graphics/Public/IDevice.h"
#include "platform/Public/OutputSurface.h"

#ifdef AE_BUILD_GRAPHICS_LIB
#	define AE_GLIB_API	AE_DLL_EXPORT
#else
#	define AE_GLIB_API	AE_DLL_IMPORT
#endif

#if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define VULKAN_ONLY( ... )	__VA_ARGS__
#else
#	define VULKAN_ONLY( ... )
#endif
#if defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define METAL_ONLY( ... )	__VA_ARGS__
#else
#	define METAL_ONLY( ... )
#endif

namespace AE::GraphicsLib
{
	using namespace AE::Graphics;


	//
	// Base interface
	//
	class GLibBase
	{
	public:
		virtual void  Destroy ()	__NE___	= 0;
	};


	//
	// Unique Pointer
	//
	template <typename T>
	class GLibUniquePtr
	{
	public:
		StaticAssert( IsBaseOf< GLibBase, T >);
		using Self = GLibUniquePtr<T>;

	private:
		T*		_ptr	= null;

	public:
		GLibUniquePtr ()							__NE___ {}
		GLibUniquePtr (std::nullptr_t)				__NE___ {}
		GLibUniquePtr (GLibUniquePtr &&other)		__NE___	: _ptr{other._ptr} { other._ptr = null; }
		GLibUniquePtr (const GLibUniquePtr &)		= delete;
		GLibUniquePtr (T* ptr)						__NE___ : _ptr{ptr} {}

		~GLibUniquePtr ()							__NE___	{ _Dec(); }

		Self&  operator = (std::nullptr_t)			__NE___ { _Dec();  _ptr = null;			return *this; }
		Self&  operator = (T* rhs)					__NE___ { _Dec();  _ptr = rhs;			return *this; }
		Self&  operator = (Self &&rhs)				__NE___ { _Dec();  _ptr = rhs._ptr();	rhs._ptr = null;	return *this; }

		ND_ T*	operator -> ()						C_NE___	{ NonNull( _ptr );  return _ptr; }
		ND_ T&	operator *  ()						C_NE___	{ NonNull( _ptr );  return *_ptr; }

		ND_ explicit operator bool ()				C_NE___	{ return _ptr != null; }
		ND_ operator T* ()							C_NE___	{ return _ptr; }

	private:
		void  _Dec ()								__NE___ { if ( _ptr != null ) _ptr->Destroy();  _ptr = null; }
	};



	//
	// Draw Context
	//
	class DrawContext2 final : public IDrawContext
	{
	// variables
	public:
		Ptr<IDrawContext>	_ctx;

	// methods
	public:
		explicit DrawContext2 (IDrawContext* ctx)																			__NE___ : _ctx{ctx} { NonNull( ctx ); }
		~DrawContext2 ()																									__NE___ { ASSERT( _ctx == null ); }

		void  BindPipeline (GraphicsPipelineID ppln)																		__Th_OV	{ _ctx->BindPipeline( ppln ); }
		void  BindPipeline (MeshPipelineID ppln)																			__Th_OV	{ _ctx->BindPipeline( ppln ); }
		void  BindPipeline (TilePipelineID ppln)																			__Th_OV	{ _ctx->BindPipeline( ppln ); }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV	{ _ctx->BindDescriptorSet( index, ds, dynamicOffsets ); }
		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV	{ _ctx->PushConstant( idx, size, values, typeName ); }
		using IDrawContext::PushConstant;

		void  SetViewport (const Viewport &viewport)																		__Th_OV	{ _ctx->SetViewport( viewport ); }
		void  SetViewports (ArrayView<Viewport> viewports)																	__Th_OV	{ _ctx->SetViewports( viewports ); }
		void  SetScissor (const RectI &scissor)																				__Th_OV	{ _ctx->SetScissor( scissor ); }
		void  SetScissors (ArrayView<RectI> scissors)																		__Th_OV	{ _ctx->SetScissors( scissors ); }
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th_OV	{ _ctx->SetDepthBias( depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor ); }
		void  SetStencilReference (uint reference)																			__Th_OV	{ _ctx->SetStencilReference( reference ); }
		void  SetStencilReference (uint frontReference, uint backReference)													__Th_OV	{ _ctx->SetStencilReference( frontReference, backReference ); }
		void  SetBlendConstants (const RGBA32f &color)																		__Th_OV	{ _ctx->SetBlendConstants( color ); }

		VULKAN_ONLY(
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th_OV	{ _ctx->SetDepthBounds( minDepthBounds, maxDepthBounds ); }
		void  SetStencilCompareMask (uint compareMask)																		__Th_OV	{ _ctx->SetStencilCompareMask( compareMask ); }
		void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th_OV	{ _ctx->SetStencilCompareMask( frontCompareMask, backCompareMask ); }
		void  SetStencilWriteMask (uint writeMask)																			__Th_OV	{ _ctx->SetStencilWriteMask( writeMask ); }
		void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th_OV	{ _ctx->SetStencilWriteMask( frontWriteMask, backWriteMask ); }
		void  SetFragmentShadingRate (EShadingRate rate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp texOp)	__Th_OV	{ _ctx->SetFragmentShadingRate( rate, primitiveOp, texOp ); }
		)

		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th_OV	{ _ctx->BindIndexBuffer( buffer, offset, indexType ); }
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th_OV	{ _ctx->BindVertexBuffer( index, buffer, offset ); }
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th_OV	{ _ctx->BindVertexBuffers( firstBinding, buffers, offsets ); }
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset)		__Th_OV	{ return _ctx->BindVertexBuffer( pplnId, name, buffer, offset ); }

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;
		using IDrawContext::DrawMeshTasksIndirect;
		using IDrawContext::DrawIndirectCount;
		using IDrawContext::DrawIndexedIndirectCount;
		using IDrawContext::DrawMeshTasksIndirectCount;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0)																				__Th_OV	{ _ctx->Draw( vertexCount, instanceCount, firstVertex, firstInstance ); }

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0)																		__Th_OV	{ _ctx->DrawIndexed( indexCount, instanceCount, firstIndex, vertexOffset, firstInstance ); }

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																				__Th_OV	{ _ctx->DrawIndirect( indirectBuffer, indirectBufferOffset, drawCount, stride ); }

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																		__Th_OV	{ _ctx->DrawIndexedIndirect( indirectBuffer, indirectBufferOffset, drawCount, stride ); }

		void  DispatchTile ()																								__Th_OV	{ _ctx->DispatchTile(); }

		void  DrawMeshTasks (const uint3 &taskCount)																		__Th_OV	{ _ctx->DrawMeshTasks( taskCount ); }

		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																		__Th_OV	{ _ctx->DrawMeshTasksIndirect( indirectBuffer, indirectBufferOffset, drawCount, stride ); }

		VULKAN_ONLY(
		void  DrawIndirectCount (BufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 BufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																			__Th_OV	{ _ctx->DrawIndirectCount( indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride ); }

		void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										BufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)																	__Th_OV	{ _ctx->DrawIndexedIndirectCount( indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride ); }

		void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  BufferID	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)																	__Th_OV	{ _ctx->DrawMeshTasksIndirectCount( indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride ); }
		)

		void  DebugMarker (DebugLabel dbg)																					__Th_OV	{ _ctx->DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																				__Th_OV	{ _ctx->PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																								__Th_OV	{ _ctx->PopDebugGroup(); }

		VULKAN_ONLY(
		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)							__Th_OV	{ _ctx->WriteTimestamp( q, index, srcScope ); }
		)

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)				__Th_OV	{ _ctx->AttachmentBarrier( name, srcState, dstState ); }
		void  CommitBarriers ()																								__Th_OV	{ _ctx->CommitBarriers(); }

		bool  ClearAttachment (AttachmentName::Ref name, const RGBA32f &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV	{ return _ctx->ClearAttachment( name, color, rect, baseLayer, layerCount ); }
		bool  ClearAttachment (AttachmentName::Ref name, const RGBA32u &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV	{ return _ctx->ClearAttachment( name, color, rect, baseLayer, layerCount ); }
		bool  ClearAttachment (AttachmentName::Ref name, const RGBA32i &color,   const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV	{ return _ctx->ClearAttachment( name, color, rect, baseLayer, layerCount ); }
		bool  ClearAttachment (AttachmentName::Ref name, const DepthStencil &ds, const RectI &rect, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV	{ return _ctx->ClearAttachment( name, ds, rect, baseLayer, layerCount ); }

		bool  AllocVStream (Bytes size, OUT VertexStream &result)															__Th_OV	{ return _ctx->AllocVStream( size, OUT result ); }

		FrameUID  GetFrameId ()																								C_NE_OV	{ return _ctx->GetFrameId(); }
	};


	//
	// Graphics Context interface
	//
	class IGraphicsContext2 : public GLibBase, public IGraphicsContext
	{
	// interface
	public:
		// Record draw commands into the same command buffer.
		ND_ virtual DrawContext2	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg)	__Th___	= 0;
		ND_ virtual DrawContext2	NextSubpass (DrawContext2 &prevPassCtx, DebugLabel dbg)			__Th___	= 0;
			virtual void			EndRenderPass (DrawContext2 &)									__Th___	= 0;
	};


	class ITransferContext2 : public GLibBase, public ITransferContext {};
	class IComputeContext2 : public GLibBase, public IComputeContext {};
	class IDescriptorUpdater2 : public GLibBase, public IDescriptorUpdater {};


	//
	// Resource Manager
	//
	class IResourceManager2 : public IResourceManager
	{
	// types
	protected:
		using Types_t = TypeList< ImageID, ImageViewID, DescriptorSetID >;


	// interface
	public:
		template <typename ID>
		bool  ReleaseResource (INOUT Strong<ID> &id)							__NE___
		{
			StaticAssert( sizeof(ID) <= sizeof(ulong) );
			return _DelayedRelease( BitCastRlx<ulong>( id.Release().Data() ), Types_t::Index<ID> );
		}

		template <typename ArrayType>
		void  ReleaseResourceArray (INOUT ArrayType &arr)						__NE___
		{
			for (auto& id : arr) {
				ReleaseResource( INOUT id );
			}
		}

		template <typename ID>
		bool  ImmediatelyRelease2 (INOUT Strong<ID> &id)						__NE___
		{
			StaticAssert( sizeof(ID) <= sizeof(ulong) );
			return _ImmediatelyRelease( BitCastRlx<ulong>( id.Release().Data() ), Types_t::Index<ID> );
		}

		template <typename Arg0, typename ...Args>
		void  ImmediatelyReleaseResources (Arg0 &arg0, Args& ...args)			__NE___
		{
			ImmediatelyRelease2( INOUT arg0 );
			if constexpr( CountOf<Args...>() > 0 )
				return ImmediatelyReleaseResources( FwdArg<Args&>( args )... );
		}

		template <typename Arg0, typename ...Args>
		void  ReleaseResources (Arg0 &arg0, Args& ...args)						__NE___
		{
			ReleaseResource( INOUT FwdArg<Arg0&>( arg0 ));
			if constexpr( CountOf<Args...>() > 0 )
				return ReleaseResources( FwdArg<Args&>( args )... );
		}

		ND_ virtual IDevice const&  GetDevice ()								__NE___ = 0;

	protected:
		virtual bool  _DelayedRelease (ulong id, uint type)						__NE___ = 0;
		virtual bool  _ImmediatelyRelease (ulong id, uint type)					__NE___ = 0;
	};


	using TransferContextPtr	= GLibUniquePtr< ITransferContext2 >;
	using ComputeContextPtr		= GLibUniquePtr< IComputeContext2 >;
	using GraphicsContextPtr	= GLibUniquePtr< IGraphicsContext2 >;
	using DescUpdaterPtr		= GLibUniquePtr< IDescriptorUpdater2 >;


	//
	// Graphics Library interface
	//
	class IGraphicsLib : public GLibBase
	{
	// interface
	public:
		ND_ virtual bool				Initialize (const GraphicsCreateInfo &,
													StringView buildConfig,
													bool optimizeIDs,
													Version3 engineVersion)										__NE___	= 0;
		ND_ virtual bool				CreateSwapchain (const uint2 &, const SwapchainDesc &, NativeWindow)	__NE___ = 0;

		ND_ virtual IDevice const*		GetDevice ()															__NE___	= 0;
		ND_ virtual IResourceManager2*	GetResourceManager ()													__NE___	= 0;
		ND_ virtual DescUpdaterPtr		CreateDescriptorUpdater ()												__NE___ = 0;

		ND_ virtual TransferContextPtr	BeginTransferContext (IBaseContext* prev = null)						__NE___	= 0;
		ND_ virtual ComputeContextPtr	BeginComputeContext (IBaseContext* prev = null)							__NE___	= 0;
		ND_ virtual GraphicsContextPtr	BeginGraphicsContext (IBaseContext* prev = null)						__NE___	= 0;

		ND_	virtual bool				BeginFrame ()															__NE___ = 0;
		ND_	virtual bool				GetTargets (OUT App::IOutputSurface::RenderTargets_t &)					__NE___	= 0;
		ND_	virtual bool				EndFrame (IBaseContext* ctx)											__NE___ = 0;
		ND_ virtual bool				WaitAll (nanoseconds timeout)											__NE___ = 0;
	};


	extern "C" AE_GLIB_API IGraphicsLib*  CreateGraphicsLib ();


} // AE::GraphicsLib

#undef VULKAN_ONLY
#undef METAL_ONLY
