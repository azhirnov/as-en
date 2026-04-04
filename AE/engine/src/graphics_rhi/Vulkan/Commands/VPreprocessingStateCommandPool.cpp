// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VPreprocessingStateCommandPool.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/Commands/VDrawContext.h"
# include "graphics_rhi/RenderGraphImpl.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	constructor
=================================================
*/
	VPreprocessingStateCommandPool::VPreprocessingStateCommandPool (VDevice const& dev, VQueuePtr q) __NE___ :
		_queue{ q }, _device{ dev }
	{
		VkCommandPoolCreateInfo	info = {};
		info.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.queueFamilyIndex	= uint(_queue->familyIndex);
		info.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_CHECK( _device.vkCreateCommandPool( _device.GetVkDevice(), &info, null, OUT &_handle ));
	}

/*
=================================================
	destructor
=================================================
*/
	VPreprocessingStateCommandPool::~VPreprocessingStateCommandPool () __NE___
	{
		// TODO: delayed delete?

		if ( _handle != Default )
			_device.vkDestroyCommandPool( _device.GetVkDevice(), _handle, null );
	}

/*
=================================================
	_Alloc
=================================================
*/
	VkCommandBuffer  VPreprocessingStateCommandPool::_Alloc () __NE___
	{
		ASSERT( IsCreated() );

		VkCommandBufferAllocateInfo	info = {};
		info.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.commandPool		= _handle;
		info.commandBufferCount	= 1;

		VkCommandBufferBeginInfo	begin = {};
		begin.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkCommandBuffer		cmd = Default;
		VK_CHECK_ERR( _device.vkAllocateCommandBuffers( _device.GetVkDevice(), &info, OUT &cmd ));

		VK_CHECK_ERR( _device.vkBeginCommandBuffer( cmd, &begin ));

		return cmd;
	}

/*
=================================================
	BeginCompute
=================================================
*/
	RC<IComputeStateCommandBuffer>  VPreprocessingStateCommandPool::BeginCompute () __NE___
	{
		CHECK_ERR( AnyEqual( QueueType(), EQueueType::Graphics, EQueueType::AsyncCompute ));

		VkCommandBuffer		cmdbuf = _Alloc();
		CHECK_ERR( cmdbuf != Default );

		return MakeRC<VComputeStateCommandBuffer>( cmdbuf, GetRC<VPreprocessingStateCommandPool>() );
	}

/*
=================================================
	BeginGraphics
=================================================
*/
	RC<IGraphicsStateCommandBuffer>  VPreprocessingStateCommandPool::BeginGraphics () __NE___
	{
		CHECK_ERR( QueueType() == EQueueType::Graphics );
		ASSERT( IsCreated() );

		VkCommandBuffer		cmdbuf = _Alloc();
		CHECK_ERR( cmdbuf != Default );

		return MakeRC<VGraphicsStateCommandBuffer>( cmdbuf, GetRC<VPreprocessingStateCommandPool>() );
	}

/*
=================================================
	BeginRayTracing
=================================================
*/
	RC<IRayTracingStateCommandBuffer>  VPreprocessingStateCommandPool::BeginRayTracing () __NE___
	{
		CHECK_ERR( AnyEqual( QueueType(), EQueueType::Graphics, EQueueType::AsyncCompute ));
		ASSERT( IsCreated() );

		VkCommandBuffer		cmdbuf = _Alloc();
		CHECK_ERR( cmdbuf != Default );

		return MakeRC<VRayTracingStateCommandBuffer>( cmdbuf, GetRC<VPreprocessingStateCommandPool>() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VPreprocessingState::VPreprocessingState (VkPipelineBindPoint bindPoint, VkCommandBuffer cmdbuf, RC<VPreprocessingStateCommandPool> pool) __NE___ :
		_cmdbuf{ cmdbuf }, _pool{ RVRef(pool) }, _bindPoint{ bindPoint }
	{}

/*
=================================================
	destructor
=================================================
*/
	VPreprocessingState::~VPreprocessingState () __NE___
	{
		EXLOCK( _pool->GetGuard() );

		auto&	dev = _pool->GetDevice();
		VK_CHECK( dev.vkEndCommandBuffer( _cmdbuf ));
		dev.vkFreeCommandBuffers( dev.GetVkDevice(), _pool->Handle(), 1, &_cmdbuf );
	}

/*
=================================================
	_IsResourcesAlive2
=================================================
*/
	inline bool  VPreprocessingState::_IsResourcesAlive2 (const ResourceManager &resMngr) C_NE___
	{
		bool	alive = true;
		for (auto& ds : _state.descSets) {
			alive &= (ds == Default or resMngr.IsAlive( ds ));
		}
		return alive;
	}

/*
=================================================
	_BindInitialPipeline
=================================================
*/
	inline void  VPreprocessingState::_BindInitialPipeline (IndirectExecutionSetID id) __Th___
	{
		CHECK_THROW( not _endRecording );
		ASSERT( _state.pplnLayout == Default );
		ASSERT( _state.execSet == Default );
		ASSERT( _state.pipeline == Default );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	dev		 = res_mngr.GetDevice();
		auto&	exec_set = res_mngr.GetResourcesOrThrow( id );
		auto	state	 = exec_set.GetInitialState();

		_state.execSet			= id;
		_state.pipeline			= state.pipeline;
		_state.pplnLayout		= state.layout;
		_state.reqDynStates		= state.dynamicState;
		_state.usedDynStates	= Default;

		dev.vkCmdBindPipeline( _cmdbuf, _bindPoint, _state.pipeline );
	}

/*
=================================================
	_PushConstant
=================================================
*/
	inline void  VPreprocessingState::_PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		CHECK_THROW( not _endRecording );
		CHECK_THROW( _state.pplnLayout != Default );  // pipeline must be bound
		CHECK_THROW( not _state.pushConst.IsFull() );
		CHECK_THROW( _state.pcStorageOffset + size <= Sizeof(_state.pcStorage) );

		auto&	dev = GraphicsScheduler().GetDevice();
		dev.vkCmdPushConstants( _cmdbuf, _state.pplnLayout, VEnumCast( EShaderStages(0) | idx.stage ), uint{idx.vulkanOffset}, uint{size}, values );

		auto&	dst		= _state.pushConst.emplace_back();
		dst.idx			= idx;
		dst.size		= size;
		dst.offset		= _state.pcStorageOffset;
		dst.typeName	= typeName;

		_state.pcStorageOffset += size;
		MemCopy( OUT _state.pcStorage.data() + dst.offset, values, size );
	}

/*
=================================================
	_BindPipeline
=================================================
*/
	template <typename PipeType>
	inline void  VPreprocessingState::_BindPipeline (PipeType id) __Th___
	{
		CHECK_THROW( not _endRecording );
		ASSERT( _state.pplnLayout != Default );
		ASSERT( _state.execSet == Default );
		ASSERT( _state.pipeline == Default );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	dev		 = res_mngr.GetDevice();
		auto&	ppln	 = res_mngr.GetResourcesOrThrow( id );

		_state.pipeline			= ppln.Handle();
		_state.pplnLayout		= ppln.Layout();
		_state.reqDynStates		= ppln.DynamicState();
		_state.usedDynStates	= Default;

		dev.vkCmdBindPipeline( _cmdbuf, _bindPoint, _state.pipeline );
	}

/*
=================================================
	_BindDescriptorSet
=================================================
*/
	inline void  VPreprocessingState::_BindDescriptorSet (DescSetBinding index, DescriptorSetID id) __Th___
	{
		CHECK_THROW( not _endRecording );
		CHECK_THROW( _state.pplnLayout != Default );  // pipeline must be bound
		CHECK_THROW( index.vkIndex < _state.descSets.size() );
		ASSERT( _state.descSets[ index.vkIndex ] == Default );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	dev		 = res_mngr.GetDevice();
		auto&	ds		 = res_mngr.GetResourcesOrThrow( id );
		auto	vk_ds	 = ds.Handle();

		_state.descSets[ index.vkIndex ] = id;
		dev.vkCmdBindDescriptorSets( _cmdbuf, _bindPoint, _state.pplnLayout, index.vkIndex, 1, &vk_ds, 0, null );
	}

/*
=================================================
	_Recycle
=================================================
*/
	inline bool  VPreprocessingState::_Recycle () __NE___
	{
		CHECK_ERR( _endRecording );

		EXLOCK( _pool->GetGuard() );

		FrameUID	complete_fid = GraphicsScheduler().LastCompletedFrameId();
		CHECK_ERR( _lastUsed.load() < complete_fid );

		auto&	dev = _pool->GetDevice();

		VK_CHECK_ERR( dev.vkEndCommandBuffer( _cmdbuf ));
		VK_CHECK_ERR( dev.vkResetCommandBuffer( _cmdbuf, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT ));

		VkCommandBufferBeginInfo	begin = {};
		begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK_ERR( dev.vkBeginCommandBuffer( _cmdbuf, &begin ));

		_endRecording	= false;
		_state			= {};

		_lastUsed.store( Default );
		return true;
	}

/*
=================================================
	_BindDescSets
=================================================
*/
	template <typename Ctx>
	inline void  VPreprocessingState::_BindDescSets (Ctx &ctx) C_Th___
	{
		for (auto [ds, idx] : WithIndex( _state.descSets ))
		{
			if ( ds != Default )
				ctx.BindDescriptorSet( DescSetBinding{uint(idx)}, ds );
		}
	}

/*
=================================================
	_PushConstants
=================================================
*/
	template <typename Ctx>
	inline void  VPreprocessingState::_PushConstants (Ctx &ctx) C_Th___
	{
		for (auto& pc : _state.pushConst)
		{
			ctx.PushConstant( pc.idx, pc.size, _state.pcStorage.data() + pc.offset, pc.typeName );
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_IsResourcesAlive
=================================================
*/
	bool  VComputeStateCommandBuffer::_IsResourcesAlive () C_NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		return	_IsResourcesAlive2( res_mngr )			and
				(res_mngr.IsAlive( _state2.ppln )		or
				 res_mngr.IsAlive( _state.execSet ));
	}

/*
=================================================
	Recycle
=================================================
*/
	bool  VComputeStateCommandBuffer::Recycle () __NE___
	{
		_state2 = {};
		return _Recycle();
	}

/*
=================================================
	End
=================================================
*/
	bool  VComputeStateCommandBuffer::End () __NE___
	{
		CHECK_ERR( not _endRecording );
		CHECK_ERR( _IsResourcesAlive() );
		CHECK_ERR( _state.execSet or _state2.ppln );
		CHECK_ERR( _state.reqDynStates == _state.usedDynStates );

		_endRecording = true;
		return true;
	}

/*
=================================================
	CopyStates
=================================================
*/
	bool  VComputeStateCommandBuffer::CopyStates (IComputeContext &ctx) C_Th___
	{
		CHECK_ERR( _endRecording );
		GCTX_CHECK( _IsResourcesAlive() );

		if ( _state.execSet )
			ctx.BindInitialPipeline( _state.execSet );  // throw
		else
			ctx.BindPipeline( _state2.ppln );  // throw

		_PushConstants( ctx );  // throw
		_BindDescSets( ctx );  // throw

		_lastUsed.store( ctx.GetFrameId() );
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BindInitialPipeline
=================================================
*/
	void  VGraphicsStateCommandBuffer::BindInitialPipeline (IndirectExecutionSetID id) __Th___
	{
		ASSERT( _state2.gppln == Default and _state2.mppln == Default );
		_BindInitialPipeline( id );
	}

/*
=================================================
	PushConstant
=================================================
*/
	void  VGraphicsStateCommandBuffer::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		ASSERT( (idx.stage >= EShader::Vertex and idx.stage <= EShader::Fragment) or
				AnyEqual( idx.stage, EShader::MeshTask, EShader::Mesh ));

		_PushConstant( idx, size, values, typeName );
	}

/*
=================================================
	Recycle
=================================================
*/
	bool  VGraphicsStateCommandBuffer::Recycle () __NE___
	{
		// TODO: end render pass

		_state2 = {};
		return _Recycle();
	}

/*
=================================================
	End
=================================================
*/
	bool  VGraphicsStateCommandBuffer::End () __NE___
	{
		CHECK_ERR( not _endRecording );
		CHECK_ERR( _IsResourcesAlive() );
		CHECK_ERR( _state.execSet or _state2.gppln or _state2.mppln );
		CHECK_ERR( _state.reqDynStates == _state.usedDynStates );

		_endRecording = true;
		return true;
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	void  VGraphicsStateCommandBuffer::BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) __Th___
	{
		ASSERT( _state2.indexBuffer == Default );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	dev		 = res_mngr.GetDevice();
		auto&	ib		= res_mngr.GetResourcesOrThrow( buffer );

		_state2.indexBuffer			= buffer;
		_state2.indexBufferOffset	= offset;
		_state2.indexType			= indexType;

		dev.vkCmdBindIndexBuffer( _cmdbuf, ib.Handle(), VkDeviceSize{offset}, VEnumCast(indexType) );
	}

/*
=================================================
	BindVertexBuffer
=================================================
*/
	void  VGraphicsStateCommandBuffer::BindVertexBuffer (const uint index, BufferID buffer, Bytes offset) __Th___
	{
		CHECK_THROW( index < _state2.vertexBuffers.size() );
		ASSERT( _state2.vertexBuffers[ index ] == Default );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	dev		 = res_mngr.GetDevice();
		auto&	vb		 = res_mngr.GetResourcesOrThrow( buffer );
		auto	vk_buf	 = vb.Handle();

		_state2.vertexBuffers[ index ]		 = buffer;
		_state2.vertexBufferOffsets[ index ] = offset;
		_state2.vbCount						= ubyte(Max( _state2.vbCount, index+1 ));

		dev.vkCmdBindVertexBuffers( _cmdbuf, index, 1, &vk_buf, Cast<VkDeviceSize>(&offset) );
	}

/*
=================================================
	BindVertexBuffers
=================================================
*/
	void  VGraphicsStateCommandBuffer::BindVertexBuffers (const uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) __Th___
	{
		const uint	count = uint(Min( buffers.size(), offsets.size() ));

		CHECK_THROW( firstBinding + count <= _state2.vertexBuffers.size() );
		ASSERT( buffers.size() == offsets.size() );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	dev		 = res_mngr.GetDevice();

		StaticArray< VkBuffer, GraphicsConfig::MaxVertexBuffers >	vk_bufs;

		for (uint i = 0; i < count; ++i)
		{
			ASSERT( _state2.vertexBuffers[ firstBinding + i ] == Default );

			auto&	vb = res_mngr.GetResourcesOrThrow( buffers[i] );

			vk_bufs[i] = vb.Handle();

			_state2.vertexBuffers[ firstBinding + i ]		= buffers[i];
			_state2.vertexBufferOffsets[ firstBinding + i ] = offsets[i];
		}

		_state2.vbCount = ubyte(Max( _state2.vbCount, firstBinding + count ));

		dev.vkCmdBindVertexBuffers( _cmdbuf, firstBinding, count, vk_bufs.data(), Cast<VkDeviceSize>(offsets.data()) );
	}

/*
=================================================
	BindVertexBuffer
=================================================
*/
	void  VGraphicsStateCommandBuffer::BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset) __Th___
	{
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	dev			= res_mngr.GetDevice();

		auto	[ppln, vb]	= res_mngr.GetResourcesOrThrow( pplnId, buffer );
		auto	vk_buf		= vb.Handle();
		uint	idx			= ppln.GetVertexBufferIndex( name );

		CHECK_THROW( idx != UMax );
		ASSERT( _state2.vertexBuffers[ idx ] == Default );

		_state2.vertexBuffers[ idx ]		= buffer;
		_state2.vertexBufferOffsets[ idx ]	= offset;
		_state2.vbCount						= ubyte(Max( _state2.vbCount, idx+1 ));

		dev.vkCmdBindVertexBuffers( _cmdbuf, idx, 1, &vk_buf, Cast<VkDeviceSize>(&offset) );
	}

/*
=================================================
	SetViewports
=================================================
*/
	void  VGraphicsStateCommandBuffer::SetViewports (ArrayView<Viewport> viewports) __NE___
	{
		ASSERT( _state2.vpCount == 0 );
		ASSERT( not viewports.empty() );

		DrawCommandBatch::Viewports_t	vk_viewports;
		DrawCommandBatch::Scissors_t	vk_scissors;
		ConvertViewports( viewports, Default, int2(1<<30), OUT vk_viewports, OUT vk_scissors );

		auto&	dev = _pool->GetDevice();

		dev.vkCmdSetViewport( _cmdbuf, 0, uint(vk_viewports.size()), vk_viewports.data() );
		dev.vkCmdSetScissor( _cmdbuf, 0, uint(vk_scissors.size()), vk_scissors.data() );

		MemCopy( OUT _state2.viewports.data(), viewports.data(), Min( ArraySizeOf(viewports), ArraySizeOf(_state2.viewports)) );
		_state2.vpCount = ubyte(viewports.size());
	}

/*
=================================================
	SetScissors
=================================================
*/
	void  VGraphicsStateCommandBuffer::SetScissors (ArrayView<RectI> scissors) __NE___
	{
		ASSERT( scissors.size() == _state2.vpCount );
		ASSERT( not scissors.empty() );

		StaticArray< VkRect2D, GraphicsConfig::MaxViewports >	vk_scissors;
		for (usize i : IndicesOnly( scissors ))
		{
			auto&	src = scissors[i];
			auto&	dst = vk_scissors[i];

			dst.offset.x		= src.left;
			dst.offset.y		= src.top;
			dst.extent.width	= src.Width();
			dst.extent.height	= src.Height();
		}

		auto&	dev = _pool->GetDevice();
		dev.vkCmdSetScissor( _cmdbuf, 0, uint(vk_scissors.size()), vk_scissors.data() );

		MemCopy( OUT _state2.scissors.data(), scissors.data(), Min( ArraySizeOf(scissors), ArraySizeOf(_state2.scissors)) );
	}

/*
=================================================
	CopyStates
=================================================
*/
	bool  VGraphicsStateCommandBuffer::CopyStates (IDrawContext &ctx) C_Th___
	{
		CHECK_ERR( _endRecording );

		GCTX_CHECK( _IsResourcesAlive() );

		if ( _state.execSet )
			ctx.BindInitialPipeline( _state.execSet );  // throw
		else
		if ( _state2.gppln )
			ctx.BindPipeline( _state2.gppln );  // throw
		else
			ctx.BindPipeline( _state2.mppln );  // throw

		_PushConstants( ctx );  // throw
		_BindDescSets( ctx );  // throw

		#ifdef AE_DEBUG
		{
			VPrimaryCmdBufState const*	state = null;

			if ( auto* impl = DynCast<VDirectDrawContext>( &ctx ))
				state = &impl->GetPrimaryCtxState();

			if ( auto* impl = DynCast<VIndirectDrawContext>( &ctx ))
				state = &impl->GetPrimaryCtxState();

			if ( auto* impl = DynCast<RG::DirectCtx::Draw>( &ctx ))
				state = &impl->GetBaseContext().GetPrimaryCtxState();

			if ( auto* impl = DynCast<RG::IndirectCtx::Draw>( &ctx ))
				state = &impl->GetBaseContext().GetPrimaryCtxState();

			CHECK_ERR( state != null );

			// TODO
		}
		#endif

		if ( _state2.indexBuffer )
			ctx.BindIndexBuffer( _state2.indexBuffer, _state2.indexBufferOffset, _state2.indexType );  // throw

		for (uint i = 0; i < _state2.vbCount; ++i)
		{
			if ( _state2.vertexBuffers[i] )
				ctx.BindVertexBuffer( i, _state2.vertexBuffers[i], _state2.vertexBufferOffsets[i] );  // throw
		}

		ASSERT( _state2.vpCount != 0 );

		_lastUsed.store( ctx.GetFrameId() );
		return true;
	}

/*
=================================================
	_IsResourcesAlive
=================================================
*/
	bool  VGraphicsStateCommandBuffer::_IsResourcesAlive () C_NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		bool	alive	 = _IsResourcesAlive2( res_mngr );

		alive &= (res_mngr.IsAlive( _state2.gppln )		or
				  res_mngr.IsAlive( _state2.mppln )		or
				  res_mngr.IsAlive( _state.execSet ));

		if ( _state2.indexBuffer )
			alive &= res_mngr.IsAlive( _state2.indexBuffer );

		for (uint i = 0; i < _state2.vbCount; ++i)
		{
			if ( _state2.vertexBuffers[i] )
				alive &= res_mngr.IsAlive( _state2.vertexBuffers[i] );
		}
		return alive;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	PushConstant
=================================================
*/
	void  VRayTracingStateCommandBuffer::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		ASSERT( idx.stage >= EShader::RayGen and idx.stage <= EShader::RayCallable );

		_PushConstant( idx, size, values, typeName );
	}

/*
=================================================
	Recycle
=================================================
*/
	bool  VRayTracingStateCommandBuffer::Recycle () __NE___
	{
		_state2 = {};
		return _Recycle();
	}

/*
=================================================
	End
=================================================
*/
	bool  VRayTracingStateCommandBuffer::End () __NE___
	{
		CHECK_ERR( not _endRecording );
		CHECK_ERR( _IsResourcesAlive() );
		CHECK_ERR( _state.execSet or _state2.ppln );
		CHECK_ERR( _state.reqDynStates == _state.usedDynStates );

		_endRecording = true;
		return true;
	}

/*
=================================================
	SetStackSize
=================================================
*/
	void  VRayTracingStateCommandBuffer::SetStackSize (Bytes size) __Th___
	{
		CHECK_THROW( _state.pipeline != Default );
		CHECK_THROW( AllBits( _state.reqDynStates, EPipelineDynamicState::RTStackSize ));
		ASSERT( _state2.stackSize == 0 );

		_state2.stackSize = size;
		_state.usedDynStates |= EPipelineDynamicState::RTStackSize;
	}

/*
=================================================
	CopyStates
=================================================
*/
	bool  VRayTracingStateCommandBuffer::CopyStates (IRayTracingContext &ctx) C_Th___
	{
		CHECK_ERR( _endRecording );
		GCTX_CHECK( _IsResourcesAlive() );

		if ( _state.execSet )
			ctx.BindInitialPipeline( _state.execSet );  // throw
		else
			ctx.BindPipeline( _state2.ppln );  // throw

		_PushConstants( ctx );  // throw
		_BindDescSets( ctx );  // throw

		_lastUsed.store( ctx.GetFrameId() );
		return true;
	}

/*
=================================================
	_IsResourcesAlive
=================================================
*/
	bool  VRayTracingStateCommandBuffer::_IsResourcesAlive () C_NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		return	_IsResourcesAlive2( res_mngr )			and
				(res_mngr.IsAlive( _state2.ppln )		or
				 res_mngr.IsAlive( _state.execSet ));
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
