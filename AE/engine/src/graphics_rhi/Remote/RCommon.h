// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	[supported features](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/RemoteRHI.md)
*/

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/Common.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Public/GraphicsCreateInfo.h"
# include "graphics_rhi/Private/Defines.h"
# include "graphics_rhi/Remote/RMessages.h"

namespace AE::Graphics
{
	using AE::Threading::SpinLock;
	using AE::Threading::RWSpinLock;
	using AE::Threading::SpinLockRelaxed;
	using AE::Threading::GlobalLinearAllocatorRef;
	using AE::Threading::GraphicsFrameAllocatorRef;
	using AE::Threading::StructAtomic;
	using AE::Threading::FAtomic;
	using AE::Threading::ETaskQueue;

	class RDevice;

	GFX_DBG_ONLY(
		using DebugName_t = FixedString<64>;
	)

	// Used for temporary allocations.
	// Thread-safe: no
	using RTempLinearAllocator	= LinearAllocator< UntypedAllocator, 8, false >;
	using RTempStackAllocator	= StackAllocator<  UntypedAllocator, 8, false >;


	//
	// Config
	//
	struct RConfig final : Noninstanceable
	{
	};


	//
	// Dependency Info
	//
	struct RDependencyInfo
	{
		const void*		data;
		Bytes32u		size;
		uint			count	= 0;

		RDependencyInfo ()				__NE___ {}

		ND_ explicit operator bool ()	C_NE___	{ return count != 0; }
		ND_ auto&  operator * ()		C_NE___	{ return *this; }
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
