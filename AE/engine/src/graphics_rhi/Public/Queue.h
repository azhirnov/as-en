// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/Common.h"

namespace AE::Graphics
{

	enum class EQueueType : ubyte
	{
		Graphics,			// also supports compute and transfer commands
		AsyncCompute,		// separate compute queue
		AsyncTransfer,		// separate transfer queue
		VideoEncode,
		VideoDecode,

		// TODO:
		//	- async ray tracing
		//	- async graphics (Metal)

		_Count,
		Unknown			= _Count,
	};


	enum class EQueueMask : ubyte
	{
		Graphics		= 1 << uint(EQueueType::Graphics),
		AsyncCompute	= 1 << uint(EQueueType::AsyncCompute),
		AsyncTransfer	= 1 << uint(EQueueType::AsyncTransfer),
		VideoEncode		= 1 << uint(EQueueType::VideoEncode),
		VideoDecode		= 1 << uint(EQueueType::VideoDecode),
		All				= (1 << uint(EQueueType::_Count)) - 1,
		Unknown			= 0,
		_BITOPS_
	};


	enum class EPipelineScope : ubyte
	{
		Host,
		Graphics,
		FramebufferLocal,
		Compute,
		Transfer_Graphics,	// blit, clear, resolve requires graphics queue, include Transfer_Copy
		Transfer_Copy,		// for async transfer queue
		RayTracing,
		RTAS_Build,			// build acceleration structure
		RTAS_Copy,			// copy acceleration structure
		MM_Build,			// build micromap
		Video,
		ICB_Preprocess,		// indirect command buffer preprocessing
		None,
		All,
		_Count
	};


	//
	// Command Batch Description
	//
	struct CmdBatchDesc
	{
	// types
		enum class EFlags : ubyte
		{
			Unknown		= 0,
			ResetQuery	= 1 << 0,		// used instead of host query reset
			FrameEnd	= 1 << 1,		// when used frame boundary, batch with 'FrameEnd' will mark batch as end of frame
			_BITOPS_
		};

	// variables
		EQueueType		queue		= Default;
		EFlags			flags		= Default;
		uint			submitIdx	= UMax;			// in which order batches must be submitted to GPU, must be unique per queue type
		DebugLabel		dbg			= Default;
		void *			userData	= null;			// used by RG, can be overriden if RG is not enabled
		// TODO: ESubmitMode

	// methods
		CmdBatchDesc ()										__NE___	{}
		CmdBatchDesc (EQueueType	queue,
					  uint			submitIdx,
					  DebugLabel	dbg			= Default,
					  void *		userData	= null,
					  EFlags		flags		= Default)	__NE___ :
			queue{queue}, flags{flags},
			submitIdx{submitIdx},
			dbg{dbg}, userData{userData}
		{}

		CmdBatchDesc&	ResetQuery ()						__NE___;
		CmdBatchDesc&	FrameEnd ()							__NE___;
	};
//-----------------------------------------------------------------------------



	inline CmdBatchDesc&  CmdBatchDesc::ResetQuery () __NE___
	{
		flags |= EFlags::ResetQuery;
		return *this;
	}

	inline CmdBatchDesc&  CmdBatchDesc::FrameEnd () __NE___
	{
		flags |= EFlags::FrameEnd;
		return *this;
	}


	__Cx__ EQueueMask&  operator |= (EQueueMask &lhs, EQueueType rhs) __NE___
	{
		ASSERT_Cx( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return lhs = EQueueMask( uint(lhs) | (1u << uint(rhs)) );
	}

	NdCx__ EQueueMask   operator |  (EQueueMask lhs, EQueueType rhs) __NE___
	{
		ASSERT_Cx( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return EQueueMask( uint(lhs) | (1u << uint(rhs)) );
	}

	NdCx__ EQueueMask   operator &  (EQueueMask lhs, EQueueType rhs) __NE___
	{
		ASSERT_Cx( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return EQueueMask( uint(lhs) & (1u << uint(rhs)) );
	}


} // AE::Graphics
