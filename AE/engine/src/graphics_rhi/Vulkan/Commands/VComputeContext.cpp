// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VComputeContext.h"
# include "graphics_rhi/Vulkan/Commands/VCommands.cpp.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  _VDirectComputeCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		vkCmdBindDescriptorSets( _cmdbuf.Get(), _bindPoint, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}

/*
=================================================
	_BindComputePipeline
=================================================
*/
	void  _VDirectComputeCtx::_BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout) __NE___
	{
		_states.pplnLayout = layout;
		vkCmdBindPipeline( _cmdbuf.Get(), _bindPoint, ppln );
	}

/*
=================================================
	_Dispatch
=================================================
*/
	void  _VDirectComputeCtx::_Dispatch (const uint3 &groupCount) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( Dispatch( _states.pplnLayout, groupCount ));

		vkCmdDispatch( _cmdbuf.Get(), groupCount.x, groupCount.y, groupCount.z );
	}

/*
=================================================
	DispatchIndirect
=================================================
*/
	void  _VDirectComputeCtx::DispatchIndirect (VkBuffer buffer, Bytes offset) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( DispatchIndirect( _states.pplnLayout, buffer ));

		vkCmdDispatchIndirect( _cmdbuf.Get(), buffer, VkDeviceSize(offset) );
	}

/*
=================================================
	_DispatchBase
=================================================
*/
	void  _VDirectComputeCtx::_DispatchBase (const uint3 &baseGroup, const uint3 &groupCount) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( DispatchBase( _states.pplnLayout, baseGroup, groupCount ));

		vkCmdDispatchBaseKHR( _cmdbuf.Get(), baseGroup.x, baseGroup.y, baseGroup.z, groupCount.x, groupCount.y, groupCount.z );
	}

/*
=================================================
	PreprocessGeneratedCommands / ExecuteGeneratedCommands
=================================================
*/
	void  _VDirectComputeCtx::PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, VkCommandBuffer stateCmdbuf) __Th___
	{
		vkCmdPreprocessGeneratedCommandsEXT( _cmdbuf.Get(), &info, stateCmdbuf );
	}

	void  _VDirectComputeCtx::ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, bool isPreprocessed) __Th___
	{
		vkCmdExecuteGeneratedCommandsEXT( _cmdbuf.Get(), isPreprocessed, &info );
	}

/*
=================================================
	_PushComputeConstant
=================================================
*/
	void  _VDirectComputeCtx::_PushComputeConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		this->vkCmdPushConstants( this->_cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VDirectComputeCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd> inCommands) __Th___
	{
		ConvertCooperativeVectorMatrixImpl( *this, this->_cmdbuf.Get(), inCommands );
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VDirectComputeCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd2> inCommands) __Th___
	{
		FixedArray< ConvertCoopMatrixCmd, 16 >	out_cmds;

		for (auto& src : inCommands)
		{
			auto&	dst = out_cmds.emplace_back();

			auto [src_buf, dst_buf] = this->_GetResourcesOrThrow( src.srcBuffer, src.dstBuffer );

			GCTX_CHECK( src.srcOffset < src_buf.Size() );
			GCTX_CHECK( src.srcOffset + src.srcSize <= src_buf.Size() );

			GCTX_CHECK( src.dstOffset < dst_buf.Size() );
			GCTX_CHECK( src.dstOffset + src.dstSize <= dst_buf.Size() );

			dst.srcSize		= src.srcSize;
			dst.srcAddress	= src_buf.GetDeviceAddress() + src.srcOffset;
			dst.dstSize		= src.dstSize;
			dst.dstAddress	= dst_buf.GetDeviceAddress() + src.dstOffset;
			dst.numRows		= src.numRows;
			dst.numColumns	= src.numColumns;
			dst.srcStride	= src.srcStride;
			dst.dstStride	= src.dstStride;
			dst.srcType		= src.srcType;
			dst.dstType		= src.dstType;
			dst.srcLayout	= src.srcLayout;
			dst.dstLayout	= src.dstLayout;

			StaticAssert64( sizeof(ConvertCoopMatrixCmd2) == 80 );

			if_unlikely( out_cmds.IsFull() )
			{
				ConvertCooperativeVectorMatrixImpl( *this, this->_cmdbuf.Get(), out_cmds );
				out_cmds.clear();
			}
		}

		if ( not out_cmds.empty() )
			ConvertCooperativeVectorMatrixImpl( *this, this->_cmdbuf.Get(), out_cmds );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VDirectComputeCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::Compute ));

		return VBaseDirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VDirectComputeCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::Compute ));

		return VBaseDirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	EndCommandBuffer
=================================================
*/
	VBakedCommands  _VIndirectComputeCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::Compute ));

		return VBaseIndirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VIndirectComputeCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::Compute ));

		return VBaseIndirectContext::_ReleaseCommandBuffer();
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  _VIndirectComputeCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		_cmdbuf->BindDescriptorSet( _bindPoint, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );
	}

/*
=================================================
	_BindComputePipeline
=================================================
*/
	void  _VIndirectComputeCtx::_BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout)
	{
		_states.pplnLayout = layout;
		_cmdbuf->BindPipeline( _bindPoint, ppln, layout );
	}

/*
=================================================
	_PushComputeConstant
=================================================
*/
	void  _VIndirectComputeCtx::_PushComputeConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		_cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );
	}

/*
=================================================
	_Dispatch
=================================================
*/
	void  _VIndirectComputeCtx::_Dispatch (const uint3 &groupCount) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( Dispatch( _states.pplnLayout, groupCount ));

		auto&	cmd = _cmdbuf->CreateCmd< DispatchCmd >();	// throw
		MemCopy( OUT cmd.groupCount, &groupCount, Sizeof( cmd.groupCount ));
	}

/*
=================================================
	DispatchIndirect
=================================================
*/
	void  _VIndirectComputeCtx::DispatchIndirect (VkBuffer buffer, Bytes offset) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( DispatchIndirect( _states.pplnLayout, buffer ));

		auto&	cmd = _cmdbuf->CreateCmd< DispatchIndirectCmd >();	// throw
		cmd.buffer	= buffer;
		cmd.offset	= offset;
	}

/*
=================================================
	_DispatchBase
=================================================
*/
	void  _VIndirectComputeCtx::_DispatchBase (const uint3 &baseGroup, const uint3 &groupCount) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( DispatchBase( _states.pplnLayout, baseGroup, groupCount ));

		auto&	cmd = _cmdbuf->CreateCmd< DispatchBaseCmd >();	// throw
		MemCopy( OUT cmd.baseGroup,  &baseGroup,  Sizeof( cmd.baseGroup ));
		MemCopy( OUT cmd.groupCount, &groupCount, Sizeof( cmd.groupCount ));
	}

/*
=================================================
	PreprocessGeneratedCommands / ExecuteGeneratedCommands
=================================================
*/
	void  _VIndirectComputeCtx::PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, VkCommandBuffer stateCmdbuf) __Th___
	{
		auto&	cmd	= _cmdbuf->CreateCmd< PreprocessIndirectCommandsCmd >();	// throw
		cmd.info		= info;
		cmd.stateCmdbuf	= stateCmdbuf;
	}

	void  _VIndirectComputeCtx::ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, bool isPreprocessed) __Th___
	{
		auto&	cmd	= _cmdbuf->CreateCmd< ExecuteIndirectCommandsCmd >();	// throw
		cmd.info			= info;
		cmd.isPreprocessed	= isPreprocessed;
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VIndirectComputeCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd> inCommands) __Th___
	{
		auto&	cmd		 = _cmdbuf->CreateCmd< ConvertCooperativeVectorMatrixCmd, ConvertCoopMatrixCmd >( inCommands.size() );	// throw
		auto*	dst_cmds = Cast<ConvertCoopMatrixCmd>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<ConvertCoopMatrixCmd> ));

		cmd.count = uint(inCommands.size());
		MemCopy( OUT dst_cmds, inCommands.data(), ArraySizeOf(inCommands) );
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VIndirectComputeCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd2> inCommands) __Th___
	{
		FixedArray< ConvertCoopMatrixCmd, 16 >	out_cmds;

		for (auto& src : inCommands)
		{
			auto&	dst = out_cmds.emplace_back();

			auto [src_buf, dst_buf] = this->_GetResourcesOrThrow( src.srcBuffer, src.dstBuffer );

			GCTX_CHECK( src.srcOffset < src_buf.Size() );
			GCTX_CHECK( src.srcOffset + src.srcSize <= src_buf.Size() );

			GCTX_CHECK( src.dstOffset < dst_buf.Size() );
			GCTX_CHECK( src.dstOffset + src.dstSize <= dst_buf.Size() );

			dst.srcSize		= src.srcSize;
			dst.srcAddress	= src_buf.GetDeviceAddress() + src.srcOffset;
			dst.dstSize		= src.dstSize;
			dst.dstAddress	= dst_buf.GetDeviceAddress() + src.dstOffset;
			dst.numRows		= src.numRows;
			dst.numColumns	= src.numColumns;
			dst.srcStride	= src.srcStride;
			dst.dstStride	= src.dstStride;
			dst.srcType		= src.srcType;
			dst.dstType		= src.dstType;
			dst.srcLayout	= src.srcLayout;
			dst.dstLayout	= src.dstLayout;

			StaticAssert64( sizeof(ConvertCoopMatrixCmd2) == 80 );

			if_unlikely( out_cmds.IsFull() )
			{
				_ConvertCooperativeVectorMatrix( out_cmds );
				out_cmds.clear();
			}
		}

		if ( not out_cmds.empty() )
			_ConvertCooperativeVectorMatrix( out_cmds );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VComputeContextImpl<C>::_VComputeContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RawCtx{ task, RVRef(cmdbuf), dbg }
	{
		Validator_t::CtxInit( task.QueueMask() );
	}

/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindPipeline (ComputePipelineID ppln) __Th___
	{
		auto&	cppln = _GetResourcesOrThrow( ppln );

		RawCtx::_BindComputePipeline( cppln.Handle(), cppln.Layout() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );

		RawCtx::BindDescriptorSet( index, desc_set.Handle(), dynamicOffsets );
	}

/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		VALIDATE_GCTX( PushConstant( idx, size, typeName ));
		Unused( typeName );

		RawCtx::_PushComputeConstant( idx.vulkanOffset, size, values, EShaderStages(0) | idx.stage );
	}

/*
=================================================
	DispatchIndirect
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::DispatchIndirect (BufferID bufferId, Bytes offset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		VALIDATE_GCTX( DispatchIndirect( buf.Description(), offset ));

		RawCtx::DispatchIndirect( buf.Handle(), offset );
	}

/*
=================================================
	BindInitialPipeline
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindInitialPipeline (IndirectExecutionSetID id) __Th___
	{
		auto&	exec_set = _GetResourcesOrThrow( id );
		auto	state	 = exec_set.GetInitialState();

		RawCtx::_BindComputePipeline( state.pipeline, state.layout );
	}

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------

# define VCTX_TYPE	_VComputeContextImpl
# include "graphics_rhi/Vulkan/Commands/VIndirectCommandExecutorImpl.cpp.h"
# undef VCTX_TYPE
//-----------------------------------------------------------------------------

namespace AE::Graphics::_hidden_
{
	template class _VComputeContextImpl< _VDirectComputeCtx >;
	template class _VComputeContextImpl< _VIndirectComputeCtx >;

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
