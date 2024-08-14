// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

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
		Unknown			= 0xFF,
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
	};
	AE_BIT_OPERATORS( EQueueMask );


	enum class EPipelineScope : ubyte
	{
		Host,
		Graphics,
		FramebufferLocal,
		Compute,
		Transfer_Graphics,	// blit, clear, resolve requires graphics queue, include Transfer_Copy
		Transfer_Copy,		// for async transfer queue
		RayTracing,
		RTAS_Build,
		Video,
		//DeviceCommands,
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
			ResetQuery	= 1 << 0,
		};

	// variables
		EQueueType		queue		= Default;
		EFlags			flags		= Default;
		uint			submitIdx	= UMax;
		DebugLabel		dbg			= Default;
		void *			userData	= null;

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
	};
//-----------------------------------------------------------------------------



	AE_BIT_OPERATORS( CmdBatchDesc::EFlags );


	inline CmdBatchDesc&  CmdBatchDesc::ResetQuery () __NE___
	{
		flags |= EFlags::ResetQuery;
		return *this;
	}


	constexpr EQueueMask&  operator |= (EQueueMask &lhs, EQueueType rhs) __NE___
	{
		ASSERT( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return lhs = EQueueMask( uint(lhs) | (1u << uint(rhs)) );
	}

	ND_ constexpr EQueueMask   operator |  (EQueueMask lhs, EQueueType rhs) __NE___
	{
		ASSERT( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return EQueueMask( uint(lhs) | (1u << uint(rhs)) );
	}

	ND_ constexpr EQueueMask   operator &  (EQueueMask lhs, EQueueType rhs) __NE___
	{
		ASSERT( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return EQueueMask( uint(lhs) & (1u << uint(rhs)) );
	}


} // AE::Graphics
