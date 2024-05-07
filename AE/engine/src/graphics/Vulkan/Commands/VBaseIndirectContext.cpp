// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VCommands.cpp.h"

namespace AE::Graphics::_hidden_
{
/*
=================================================
	_ReuseOrCreateCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VBaseIndirectContext::_ReuseOrCreateCommandBuffer (VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg) __Th___
	{
		if ( not (cmdbuf and cmdbuf->IsValid()) )
		{
			cmdbuf.reset( new VSoftwareCmdBuf{} );	// throw

			GFX_DBG_ONLY( cmdbuf->PushDebugGroup( dbg );)
		}
		Unused( dbg );

		return cmdbuf;
	}

/*
=================================================
	_EndCommandBuffer
=================================================
*/
	VBakedCommands  _VBaseIndirectContext::_EndCommandBuffer () __Th___
	{
		GCTX_CHECK( _IsValid() );

		GFX_DBG_ONLY( _PopDebugGroup();)

		VSoftwareCmdBufPtr	tmp = RVRef(_cmdbuf);

		return tmp->Bake();
	}

/*
=================================================
	_ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VBaseIndirectContext::_ReleaseCommandBuffer () __Th___
	{
		GCTX_CHECK( _IsValid() );

		return RVRef(_cmdbuf);
	}

/*
=================================================
	_WriteTimestamp
=================================================
*/
	void  _VBaseIndirectContext::_WriteTimestamp (const VQueryManager::Query &q, uint index, EPipelineScope srcScope, VkPipelineStageFlagBits2 mask) __Th___
	{
		VkPipelineStageFlagBits2	stage = VPipelineScope::GetStages( srcScope ) & mask;
		GCTX_CHECK( stage != 0 );
		GCTX_CHECK( index < q.count );

		_cmdbuf->WriteTimestamp( stage, q.pool, q.first );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bake
=================================================
*/
	VBakedCommands  VSoftwareCmdBuf::Bake () __NE___
	{
		if_likely( _curBlock != null )
		{
			_curBlock->size -= _remainSize;
		}

		ASSERT( _Validate( _root ));

		void*	root = _root;

		_dataPtr	= null;
		_remainSize	= 0_b;
		_curBlock	= null;
		_root		= null;

		return VBakedCommands{ root };
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  VSoftwareCmdBuf::BindDescriptorSet (VkPipelineBindPoint bindPoint, VkPipelineLayout layout, uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		auto&	cmd		= CreateCmd< BindDescriptorSetCmd, uint >( dynamicOffsets.size() );	// throw
		auto*	offsets	= Cast<uint>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<uint> ));

		cmd.layout				= layout;
		cmd.destSet				= ds;
		cmd.bindPoint			= bindPoint;
		cmd.index				= CheckCast<ushort>( index );
		cmd.dynamicOffsetCount	= CheckCast<ushort>( dynamicOffsets.size() );
		MemCopy( OUT offsets, dynamicOffsets.data(), ArraySizeOf(dynamicOffsets) );
	}

/*
=================================================
	BindPipeline
=================================================
*/
	void  VSoftwareCmdBuf::BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout) __Th___
	{
		auto&	cmd = CreateCmd< BindPipelineCmd >();	// throw
		cmd.pipeline	= ppln;
		cmd.layout		= layout;
		cmd.bindPoint	= bindPoint;
	}

/*
=================================================
	PushConstant
=================================================
*/
	void  VSoftwareCmdBuf::PushConstant (VkPipelineLayout layout, Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		auto&	cmd = CreateCmd< PushConstantCmd, ubyte >( usize(size) );	// throw
		auto*	dst	= Cast<ubyte>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<ubyte> ));

		cmd.layout	= layout;
		cmd.offset	= uint(offset);
		cmd.size	= uint(size);
		cmd.stages	= VEnumCast( stages );
		MemCopy( OUT dst, values, size );
	}

/*
=================================================
	ProfilerBeginContext
=================================================
*/
	void  VSoftwareCmdBuf::ProfilerBeginContext (IGraphicsProfiler* prof, const void* batch, StringView taskName, RGBA8u color, IGraphicsProfiler::EContextType type) __Th___
	{
		GCTX_CHECK( prof != null );

		auto&	cmd = CreateCmd< ProfilerBeginContextCmd, char >( taskName.size() + 1 );	// throw
		auto*	str	= Cast<char>( &cmd + 1 );

		cmd.batch	= batch;
		cmd.prof	= prof;		// warning: required delayed destruction
		cmd.type	= type;
		cmd.color	= color;

		MemCopy( OUT str, taskName.data(), Bytes{taskName.size()} );
		str[taskName.size()] = '\0';
	}

/*
=================================================
	ProfilerEndContext
=================================================
*/
	void  VSoftwareCmdBuf::ProfilerEndContext (IGraphicsProfiler* prof, const void* batch, IGraphicsProfiler::EContextType type) __Th___
	{
		GCTX_CHECK( prof != null );

		auto&	cmd = CreateCmd< ProfilerEndContextCmd >();		// throw
		cmd.batch	= batch;
		cmd.prof	= prof;
		cmd.type	= type;
	}

/*
=================================================
	DbgFillBuffer
=================================================
*/
	void  VSoftwareCmdBuf::DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		GCTX_CHECK( buffer != Default );

		auto&	cmd	= CreateCmd< FillBufferCmd >();		// throw
		cmd.buffer	= buffer;
		cmd.offset	= offset;
		cmd.size	= size;
		cmd.data	= data;
	}

/*
=================================================
	DebugMarker
=================================================
*/
	void  VSoftwareCmdBuf::DebugMarker (DebugLabel dbg) __Th___
	{
		ASSERT( not dbg.label.empty() );
		if ( dbg.label.empty() )
			return;

		auto&	cmd	= CreateCmd< DebugMarkerCmd, char >( dbg.label.size() + 1 );	// throw
		auto*	str	= Cast<char>( &cmd + 1 );

		cmd.color = dbg.color;
		MemCopy( OUT str, dbg.label.data(), Bytes{dbg.label.size()} );
		str[dbg.label.size()] = '\0';
	}

/*
=================================================
	PushDebugGroup
=================================================
*/
	void  VSoftwareCmdBuf::PushDebugGroup (DebugLabel dbg) __Th___
	{
		ASSERT( not dbg.label.empty() );

		auto&	cmd	= CreateCmd< PushDebugGroupCmd, char >( dbg.label.size() + 1 );	// throw
		auto*	str	= Cast<char>( &cmd + 1 );

		cmd.color = dbg.color;
		MemCopy( OUT str, dbg.label.data(), Bytes{dbg.label.size()} );
		str[dbg.label.size()] = '\0';
	}

/*
=================================================
	PopDebugGroup
=================================================
*/
	void  VSoftwareCmdBuf::PopDebugGroup () __Th___
	{
		Unused( CreateCmd< PopDebugGroupCmd >( 0 ));	// throw
	}

/*
=================================================
	PipelineBarrier
=================================================
*/
	void  VSoftwareCmdBuf::PipelineBarrier (const VkDependencyInfo &barrier)
	{
		ASSERT( barrier.pNext == null );

		constexpr usize		align = Max( alignof(VkMemoryBarrier2), alignof(VkBufferMemoryBarrier2), alignof(VkImageMemoryBarrier2) );

		Bytes	size = SizeOf<PipelineBarrierCmd>;
		size = barrier.memoryBarrierCount		? AlignUp( size, align ) + sizeof(VkMemoryBarrier2)       * barrier.memoryBarrierCount		 : size;
		size = barrier.bufferMemoryBarrierCount ? AlignUp( size, align ) + sizeof(VkBufferMemoryBarrier2) * barrier.bufferMemoryBarrierCount : size;
		size = barrier.imageMemoryBarrierCount  ? AlignUp( size, align ) + sizeof(VkImageMemoryBarrier2)  * barrier.imageMemoryBarrierCount  : size;
		size = AlignUp( size, BaseAlign );

		auto*	cmd = Cast<PipelineBarrierCmd>( _Allocate( size ));
		void*	ptr = AlignUp( static_cast< void *>(cmd + 1), align );

		GFX_DBG_ONLY(
			cmd->_magicNumber = BaseCmd::MAGIC;
		)
		cmd->_commandID = CheckCast<ushort>( Commands_t::template Index< PipelineBarrierCmd >);
		cmd->_size		= CheckCast<ushort>( size );

		cmd->memoryBarrierCount	= CheckCast<ushort>( barrier.memoryBarrierCount );
		cmd->bufferBarrierCount	= CheckCast<ushort>( barrier.bufferMemoryBarrierCount );
		cmd->imageBarrierCount	= CheckCast<ushort>( barrier.imageMemoryBarrierCount );
		cmd->dependencyFlags	= CheckCast<ushort>( barrier.dependencyFlags );

		MemCopy( OUT ptr, barrier.pMemoryBarriers, SizeOf<VkMemoryBarrier2> * barrier.memoryBarrierCount );
		ptr = AlignUp( ptr + SizeOf<VkMemoryBarrier2> * barrier.memoryBarrierCount, align );

		MemCopy( OUT ptr, barrier.pBufferMemoryBarriers, SizeOf<VkBufferMemoryBarrier2> * barrier.bufferMemoryBarrierCount );
		ptr = AlignUp( ptr + SizeOf<VkBufferMemoryBarrier2> * barrier.bufferMemoryBarrierCount, align );

		MemCopy( OUT ptr, barrier.pImageMemoryBarriers, SizeOf<VkImageMemoryBarrier2> * barrier.imageMemoryBarrierCount );
		ptr = AlignUp( ptr + SizeOf<VkImageMemoryBarrier2> * barrier.imageMemoryBarrierCount, align );
	}

/*
=================================================
	WriteTimestamp
=================================================
*/
	void  VSoftwareCmdBuf::WriteTimestamp (VkPipelineStageFlags2 stage, VkQueryPool pool, uint query) __Th___
	{
		auto&	cmd = CreateCmd< WriteTimestampCmd >();
		cmd.pool	= pool;
		cmd.query	= query;
		cmd.stage	= stage;
	}

/*
=================================================
	CmdProcessor
=================================================
*/
	struct VSoftwareCmdBuf::CmdProcessor
	{
	// shared commands
		static void  Fn_DebugMarkerCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DebugMarkerCmd &cmd) __NE___
		{
			auto*	text = Cast<char>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<char> ));

			VkDebugUtilsLabelEXT	info = {};
			info.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			info.pLabelName	= text;
			MemCopy( OUT info.color, RGBA32f{cmd.color} );
			fn.vkCmdInsertDebugUtilsLabelEXT( cmdbuf, &info );
		}

		static void  Fn_PushDebugGroupCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const PushDebugGroupCmd &cmd) __NE___
		{
			auto*	text = Cast<char>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<char> ));

			VkDebugUtilsLabelEXT	info = {};
			info.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			info.pLabelName	= text;
			MemCopy( OUT info.color, RGBA32f{cmd.color} );
			fn.vkCmdBeginDebugUtilsLabelEXT( cmdbuf, &info );
		}

		static void  Fn_PopDebugGroupCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const PopDebugGroupCmd &) __NE___
		{
			fn.vkCmdEndDebugUtilsLabelEXT( cmdbuf );
		}

		static void  Fn_PipelineBarrierCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const PipelineBarrierCmd &cmd) __NE___
		{
			constexpr usize		align = Max( alignof(VkMemoryBarrier2), alignof(VkBufferMemoryBarrier2), alignof(VkImageMemoryBarrier2) );

			auto*	memory	= Cast<VkMemoryBarrier2>(       AlignUp( static_cast< const void *>(&cmd + 1),                         align ));
			auto*	buffers	= Cast<VkBufferMemoryBarrier2>( AlignUp( static_cast< const void *>(memory + cmd.memoryBarrierCount),  align ));
			auto*	images	= Cast<VkImageMemoryBarrier2>(  AlignUp( static_cast< const void *>(buffers + cmd.bufferBarrierCount), align ));

			VkDependencyInfo	barrier;
			barrier.sType	= VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			barrier.pNext	= null;

			barrier.dependencyFlags				= cmd.dependencyFlags;
			barrier.memoryBarrierCount			= cmd.memoryBarrierCount;
			barrier.pMemoryBarriers				= cmd.memoryBarrierCount ? memory  : null;
			barrier.bufferMemoryBarrierCount	= cmd.bufferBarrierCount;
			barrier.pBufferMemoryBarriers		= cmd.bufferBarrierCount ? buffers : null;
			barrier.imageMemoryBarrierCount		= cmd.imageBarrierCount;
			barrier.pImageMemoryBarriers		= cmd.imageBarrierCount ? images  : null;

			fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );
		}

		static void  Fn_BindDescriptorSetCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BindDescriptorSetCmd &cmd) __NE___
		{
			auto*	offsets = Cast<uint>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<uint> ));
			fn.vkCmdBindDescriptorSets( cmdbuf, cmd.bindPoint, cmd.layout, cmd.index, 1, &cmd.destSet, cmd.dynamicOffsetCount, offsets );
		}

		static void  Fn_BindPipelineCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BindPipelineCmd &cmd) __NE___
		{
			fn.vkCmdBindPipeline( cmdbuf, cmd.bindPoint, cmd.pipeline );
		}

		static void  Fn_PushConstantCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const PushConstantCmd &cmd) __NE___
		{
			auto*	values	= Cast<ubyte>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<ubyte> ));
			fn.vkCmdPushConstants( cmdbuf, cmd.layout, cmd.stages, cmd.offset, cmd.size, values );
		}

		static void  Fn_ProfilerBeginContextCmd (VulkanDeviceFn, VkCommandBuffer cmdbuf, const ProfilerBeginContextCmd &cmd) __NE___
		{
			auto*	task_name = Cast<char>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<char> ));

			cmd.prof->BeginContext( cmd.batch, cmdbuf, task_name, cmd.color, cmd.type );
		}

		static void  Fn_ProfilerEndContextCmd (VulkanDeviceFn, VkCommandBuffer cmdbuf, const ProfilerEndContextCmd &cmd) __NE___
		{
			cmd.prof->EndContext( cmd.batch, cmdbuf, cmd.type );
		}

		static void  Fn_WriteTimestampCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const WriteTimestampCmd &cmd) __NE___
		{
			fn.vkCmdWriteTimestamp2KHR( cmdbuf, cmd.stage, cmd.pool, cmd.query );
		}


	// transfer commands

		static void  Fn_ClearColorImageCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const ClearColorImageCmd &cmd) __NE___
		{
			auto*	ranges = Cast<VkImageSubresourceRange>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkImageSubresourceRange> ));
			fn.vkCmdClearColorImage( cmdbuf, cmd.image, cmd.layout, &cmd.color, cmd.rangeCount, ranges );
		}

		static void  Fn_ClearDepthStencilImageCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const ClearDepthStencilImageCmd &cmd) __NE___
		{
			auto*	ranges = Cast<VkImageSubresourceRange>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkImageSubresourceRange> ));
			fn.vkCmdClearDepthStencilImage( cmdbuf, cmd.image, cmd.layout, &cmd.depthStencil, cmd.rangeCount, ranges );
		}

		static void  Fn_FillBufferCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const FillBufferCmd &cmd) __NE___
		{
			fn.vkCmdFillBuffer( cmdbuf, cmd.buffer, VkDeviceSize(cmd.offset), VkDeviceSize(cmd.size), cmd.data );
		}

		static void  Fn_UpdateBufferCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const UpdateBufferCmd &cmd) __NE___
		{
			auto*	data = Cast<ubyte>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<ubyte> ));
			fn.vkCmdUpdateBuffer( cmdbuf, cmd.buffer, VkDeviceSize(cmd.offset), VkDeviceSize(cmd.size), data );
		}

		static void  Fn_CopyBufferCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyBufferCmd &cmd) __NE___
		{
			auto*	regions	= Cast<VkBufferCopy>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkBufferCopy> ));
			fn.vkCmdCopyBuffer( cmdbuf, cmd.srcBuffer, cmd.dstBuffer, cmd.regionCount, regions );
		}

		static void  Fn_CopyImageCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyImageCmd &cmd) __NE___
		{
			auto*	regions	= Cast<VkImageCopy>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<VkImageCopy> ));
			fn.vkCmdCopyImage( cmdbuf, cmd.srcImage, cmd.srcLayout, cmd.dstImage, cmd.dstLayout, cmd.regionCount, regions );
		}

		static void  Fn_CopyBufferToImageCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyBufferToImageCmd &cmd) __NE___
		{
			auto*	regions	= Cast<VkBufferImageCopy>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkBufferImageCopy> ));
			fn.vkCmdCopyBufferToImage( cmdbuf, cmd.srcBuffer, cmd.dstImage, cmd.dstLayout, cmd.regionCount, regions );
		}

		static void  Fn_CopyImageToBufferCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyImageToBufferCmd &cmd) __NE___
		{
			auto*	regions	= Cast<VkBufferImageCopy>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkBufferImageCopy> ));
			fn.vkCmdCopyImageToBuffer( cmdbuf, cmd.srcImage, cmd.srcLayout, cmd.dstBuffer, cmd.regionCount, regions );
		}

		static void  Fn_BlitImageCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BlitImageCmd &cmd) __NE___
		{
			auto*	regions	= Cast<VkImageBlit>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkImageBlit> ));
			fn.vkCmdBlitImage( cmdbuf, cmd.srcImage, cmd.srcLayout, cmd.dstImage, cmd.dstLayout, cmd.regionCount, regions, cmd.filter );
		}

		static void  Fn_ResolveImageCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const ResolveImageCmd &cmd) __NE___
		{
			auto*	regions	= Cast<VkImageResolve>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<VkImageResolve> ));
			fn.vkCmdResolveImage( cmdbuf, cmd.srcImage, cmd.srcLayout, cmd.dstImage, cmd.dstLayout, cmd.regionCount, regions );
		}

		static void  Fn_GenerateMipmapsCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const GenerateMipmapsCmd &cmd) __NE___
		{
			auto*	ranges	= Cast<ImageSubresourceRange>( AlignUp( static_cast< const void *>(&cmd + 1), AlignOf<ImageSubresourceRange> ));
			GenerateMipmapsImpl( fn, cmdbuf, cmd.image, cmd.dimension, ArrayView<ImageSubresourceRange>{ ranges, cmd.rangeCount } );
		}

		static void  Fn_CopyQueryPoolResultsCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyQueryPoolResultsCmd &cmd) __NE___
		{
			// If VK_QUERY_RESULT_WAIT_BIT is set, this command defines an execution dependency with any earlier commands that writes one of the identified queries.
			fn.vkCmdCopyQueryPoolResults( cmdbuf, cmd.srcPool, cmd.srcIndex, cmd.srcCount,
										  cmd.dstBuffer, VkDeviceSize(cmd.dstOffset), VkDeviceSize(cmd.stride),
										  cmd.flags );
		}


	// compute commands

		static void  Fn_DispatchCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DispatchCmd &cmd) __NE___
		{
			fn.vkCmdDispatch( cmdbuf, cmd.groupCount[0], cmd.groupCount[1], cmd.groupCount[2] );
		}

		static void  Fn_DispatchBaseCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DispatchBaseCmd &cmd) __NE___
		{
			fn.vkCmdDispatchBaseKHR( cmdbuf, cmd.baseGroup[0], cmd.baseGroup[1], cmd.baseGroup[2], cmd.groupCount[0], cmd.groupCount[1], cmd.groupCount[2] );
		}

		static void  Fn_DispatchIndirectCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DispatchIndirectCmd &cmd) __NE___
		{
			fn.vkCmdDispatchIndirect( cmdbuf, cmd.buffer, VkDeviceSize(cmd.offset) );
		}


	// graphics commands

		static void  Fn_BeginRenderPassCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BeginRenderPassCmd &cmd) __NE___
		{
			VkRenderPassBeginInfo	pass_begin	= {};
			pass_begin.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			pass_begin.renderPass		= cmd.renderPass;
			pass_begin.framebuffer		= cmd.framebuffer;
			pass_begin.renderArea		= cmd.renderArea;
			pass_begin.clearValueCount	= cmd.clearValueCount;
			pass_begin.pClearValues		= Cast<VkClearValue>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<VkClearValue> ));

			VkSubpassBeginInfo		subpass_begin = {};
			subpass_begin.sType		= VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
			subpass_begin.contents	= cmd.contents;

			fn.vkCmdBeginRenderPass2KHR( cmdbuf, &pass_begin, &subpass_begin );
		}

		static void  Fn_NextSubpassCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const NextSubpassCmd &cmd) __NE___
		{
			VkSubpassBeginInfo		subpass_begin = {};
			subpass_begin.sType		= VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
			subpass_begin.contents	= cmd.contents;

			VkSubpassEndInfo		subpass_end = {};
			subpass_end.sType		= VK_STRUCTURE_TYPE_SUBPASS_END_INFO;

			fn.vkCmdNextSubpass2KHR( cmdbuf, &subpass_begin, &subpass_end );
		}

		static void  Fn_EndRenderPassCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const EndRenderPassCmd &) __NE___
		{
			VkSubpassEndInfo	subpass_end = {};
			subpass_end.sType	= VK_STRUCTURE_TYPE_SUBPASS_END_INFO;

			fn.vkCmdEndRenderPass2KHR( cmdbuf, &subpass_end );
		}

		static void  Fn_ExecuteCommandsCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const ExecuteCommandsCmd &cmd) __NE___
		{
			auto*	cmdbufs = Cast<VkCommandBuffer>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<VkCommandBuffer> ));

			fn.vkCmdExecuteCommands( cmdbuf, cmd.count, cmdbufs );
		}


	// draw commands

		static void  Fn_SetViewportCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetViewportCmd &cmd) __NE___
		{
			auto*	viewport = Cast<VkViewport>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<VkViewport> ));
			fn.vkCmdSetViewport( cmdbuf, cmd.first, cmd.count, viewport );
		}

		static void  Fn_SetScissorCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetScissorCmd &cmd) __NE___
		{
			auto*	scissor = Cast<VkRect2D>( AlignUp( static_cast<const void *>(&cmd + 1), AlignOf<VkRect2D> ));
			fn.vkCmdSetScissor( cmdbuf, cmd.first, cmd.count, scissor );
		}

		static void  Fn_SetDepthBiasCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetDepthBiasCmd &cmd) __NE___
		{
			fn.vkCmdSetDepthBias( cmdbuf, cmd.depthBiasConstantFactor, cmd.depthBiasClamp, cmd.depthBiasSlopeFactor );
		}

		static void  Fn_SetDepthBoundsCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetDepthBoundsCmd &cmd) __NE___
		{
			fn.vkCmdSetDepthBounds( cmdbuf, cmd.minDepthBounds, cmd.maxDepthBounds );
		}

		static void  Fn_SetStencilCompareMaskCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetStencilCompareMaskCmd &cmd) __NE___
		{
			fn.vkCmdSetStencilCompareMask( cmdbuf, cmd.faceMask, cmd.compareMask );
		}

		static void  Fn_SetStencilWriteMaskCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetStencilWriteMaskCmd &cmd) __NE___
		{
			fn.vkCmdSetStencilWriteMask( cmdbuf, cmd.faceMask, cmd.writeMask );
		}

		static void  Fn_SetStencilReferenceCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetStencilReferenceCmd &cmd) __NE___
		{
			fn.vkCmdSetStencilReference( cmdbuf, cmd.faceMask, cmd.reference );
		}

		static void  Fn_SetBlendConstantsCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetBlendConstantsCmd &cmd) __NE___
		{
			fn.vkCmdSetBlendConstants( cmdbuf, cmd.color.data() );
		}

		static void  Fn_SetFragmentShadingRateCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const SetFragmentShadingRateCmd &cmd) __NE___
		{
			VkExtent2D							frag_size		{ cmd.fragSize.x, cmd.fragSize.y };
			VkFragmentShadingRateCombinerOpKHR	combiner_ops[2] = { cmd.primitiveOp, cmd.textureOp };

			fn.vkCmdSetFragmentShadingRateKHR( cmdbuf, &frag_size, combiner_ops );
		}

		static void  Fn_BindIndexBufferCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BindIndexBufferCmd &cmd) __NE___
		{
			fn.vkCmdBindIndexBuffer( cmdbuf, cmd.buffer, cmd.offset, cmd.indexType );
		}

		static void  Fn_BindVertexBuffersCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BindVertexBuffersCmd &cmd) __NE___
		{
			auto*	buffers	= Cast<VkBuffer>(     AlignUp( static_cast<const void *>(&cmd + 1),            AlignOf<VkBuffer>     ));
			auto*	offsets	= Cast<VkDeviceSize>( AlignUp( static_cast<const void *>(buffers + cmd.count), AlignOf<VkDeviceSize> ));
			fn.vkCmdBindVertexBuffers( cmdbuf, cmd.firstBinding, cmd.count, buffers, offsets );
		}

		static void  Fn_DrawCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawCmd &cmd) __NE___
		{
			fn.vkCmdDraw( cmdbuf, cmd.vertexCount, cmd.instanceCount, cmd.firstVertex, cmd.firstInstance );
		}

		static void  Fn_DrawIndexedCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawIndexedCmd &cmd) __NE___
		{
			fn.vkCmdDrawIndexed( cmdbuf, cmd.indexCount, cmd.instanceCount, cmd.firstIndex, cmd.vertexOffset, cmd.firstInstance );
		}

		static void  Fn_DrawIndirectCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawIndirectCmd &cmd) __NE___
		{
			fn.vkCmdDrawIndirect( cmdbuf, cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride );
		}

		static void  Fn_DrawIndexedIndirectCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawIndexedIndirectCmd &cmd) __NE___
		{
			fn.vkCmdDrawIndexedIndirect( cmdbuf, cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride );
		}

		static void  Fn_DrawIndirectCountCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawIndirectCountCmd &cmd) __NE___
		{
			fn.vkCmdDrawIndirectCountKHR( cmdbuf, cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride );
		}

		static void  Fn_DrawIndexedIndirectCountCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawIndexedIndirectCountCmd &cmd) __NE___
		{
			fn.vkCmdDrawIndexedIndirectCountKHR( cmdbuf, cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride );
		}

		static void  Fn_DrawMeshTasksCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawMeshTasksCmd &cmd) __NE___
		{
			fn.vkCmdDrawMeshTasksEXT( cmdbuf, cmd.taskCount.x, cmd.taskCount.y, cmd.taskCount.z );
		}

		static void  Fn_DrawMeshTasksIndirectCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawMeshTasksIndirectCmd &cmd) __NE___
		{
			fn.vkCmdDrawMeshTasksIndirectEXT( cmdbuf, cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride );
		}

		static void  Fn_DrawMeshTasksIndirectCountCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DrawMeshTasksIndirectCountCmd &cmd) __NE___
		{
			fn.vkCmdDrawMeshTasksIndirectCountEXT( cmdbuf, cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride );
		}

		static void  Fn_DispatchTileCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const DispatchTileCmd &cmd) __NE___
		{
			Unused( cmd );
			fn.vkCmdSubpassShadingHUAWEI( cmdbuf );
		}

		static void  Fn_ClearAttachmentsCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const ClearAttachmentsCmd &cmd) __NE___
		{
			fn.vkCmdClearAttachments( cmdbuf, 1, &cmd.clear, 1, &cmd.rect );
		}


	// acceleration structure build commands

		static void  Fn_BuildASCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BuildASCmd &cmd) __NE___
		{
			fn.vkCmdBuildAccelerationStructuresKHR( cmdbuf, 1, &cmd.info, &cmd.pRangeInfos );
		}

		static void  Fn_BuildASIndirectCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const BuildASIndirectCmd &cmd) __NE___
		{
			const uint	stride = sizeof(VkAccelerationStructureBuildRangeInfoKHR);
			fn.vkCmdBuildAccelerationStructuresIndirectKHR( cmdbuf, 1, &cmd.info, &cmd.indirectMem, &stride, &cmd.maxPrimCount );
		}

		static void  Fn_CopyASCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyASCmd &cmd) __NE___
		{
			VkCopyAccelerationStructureInfoKHR	info;
			info.sType	= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
			info.pNext	= null;
			info.src	= cmd.src;
			info.dst	= cmd.dst;
			info.mode	= cmd.mode;

			fn.vkCmdCopyAccelerationStructureKHR( cmdbuf, &info );
		}

		static void  Fn_CopyASToMemoryCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyASToMemoryCmd &cmd) __NE___
		{
			VkCopyAccelerationStructureToMemoryInfoKHR	info;
			info.sType				= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
			info.pNext				= null;
			info.src				= cmd.src;
			info.dst.deviceAddress	= cmd.dst;
			info.mode				= cmd.mode;

			fn.vkCmdCopyAccelerationStructureToMemoryKHR( cmdbuf, &info );
		}

		static void  Fn_CopyMemoryToASCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const CopyMemoryToASCmd &cmd) __NE___
		{
			VkCopyMemoryToAccelerationStructureInfoKHR	info;
			info.sType				= VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
			info.pNext				= null;
			info.src.deviceAddress	= cmd.src;
			info.dst				= cmd.dst;
			info.mode				= cmd.mode;

			fn.vkCmdCopyMemoryToAccelerationStructureKHR( cmdbuf, &info );
		}

		static void  Fn_WriteASPropertiesCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const WriteASPropertiesCmd &cmd) __NE___
		{
			fn.vkCmdWriteAccelerationStructuresPropertiesKHR( cmdbuf, 1, &cmd.as, cmd.type, cmd.pool, cmd.index );
		}


	// ray tracing commands

		static void  Fn_TraceRaysCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const TraceRaysCmd &cmd) __NE___
		{
			fn.vkCmdTraceRaysKHR( cmdbuf, &cmd.raygen, &cmd.miss, &cmd.hit, &cmd.callable, cmd.dim.x, cmd.dim.y, cmd.dim.z );
		}

		static void  Fn_TraceRaysIndirectCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const TraceRaysIndirectCmd &cmd) __NE___
		{
			fn.vkCmdTraceRaysIndirectKHR( cmdbuf, &cmd.raygen, &cmd.miss, &cmd.hit, &cmd.callable, cmd.indirectDeviceAddress );
		}

		static void  Fn_TraceRaysIndirect2Cmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const TraceRaysIndirect2Cmd &cmd) __NE___
		{
			fn.vkCmdTraceRaysIndirect2KHR( cmdbuf, cmd.indirectDeviceAddress );
		}

		static void  Fn_RayTracingSetStackSizeCmd (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, const RayTracingSetStackSizeCmd &cmd)
		{
			fn.vkCmdSetRayTracingPipelineStackSizeKHR( cmdbuf, cmd.size );
		}
	};

/*
=================================================
	Execute
=================================================
*/
	bool  VSoftwareCmdBuf::Execute (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, void* root) __NE___
	{
		if ( root == null )
			return false;

		Header const*	hdr		= Cast<Header>( root );
		BaseCmd const*	base	= Cast<BaseCmd>( hdr + 1 );
		Bytes			size	= hdr->size;

		for (;;)
		{
			Bytes	offset = Bytes{base} - Bytes{hdr};

			// move to next memory block
			if_unlikely( offset >= size )
			{
				if ( hdr->next == null )
					return (offset == size);

				hdr		= hdr->next;
				base	= Cast<BaseCmd>( hdr + 1 );
				size	= hdr->size;
			}

			ASSERT( base->_magicNumber == BaseCmd::MAGIC );
			ASSERT( base->_commandID < Commands_t::Count );

			switch ( base->_commandID )
			{
				#define AE_BASE_IND_CTX_VISIT( _name_ )		case Commands_t::Index<_name_> :  CmdProcessor::Fn_ ## _name_( fn, cmdbuf, *Cast<_name_>( base ));  break;
				AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
				#undef AE_BASE_IND_CTX_VISIT

				default_unlikely :	return false;	// unknown command
			}

			base = base + Bytes{base->_size};
		}
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
