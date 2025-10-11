// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VComputeContext.h"
# include "graphics_rhi/Vulkan/Commands/VCommands.cpp.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{
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

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
