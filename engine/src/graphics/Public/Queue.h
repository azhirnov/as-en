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
		All				= Graphics | AsyncCompute | AsyncTransfer,
		Unknown			= 0,
	};
	AE_BIT_OPERATORS( EQueueMask );
	

	enum class EPipelineScope : ubyte
	{
		Host,
		Graphics,
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

	forceinline constexpr EQueueMask&  operator |= (EQueueMask &lhs, EQueueType rhs)
	{
		ASSERT( uint(rhs) < CT_SizeOfInBits<EQueueMask> );
		return lhs = EQueueMask( uint(lhs) | (1u << uint(rhs)) );
	}

	ND_ forceinline constexpr EQueueMask   operator |  (EQueueMask lhs, EQueueType rhs)
	{
		ASSERT( uint(rhs) < CT_SizeOfInBits<EQueueMask>  );
		return EQueueMask( uint(lhs) | (1u << uint(rhs)) );
	}

	ND_ forceinline constexpr EQueueMask   operator &  (EQueueMask lhs, EQueueType rhs)
	{
		ASSERT( uint(rhs) < CT_SizeOfInBits<EQueueMask>  );
		return EQueueMask( uint(lhs) & (1u << uint(rhs)) );
	}


} // AE::Graphics
