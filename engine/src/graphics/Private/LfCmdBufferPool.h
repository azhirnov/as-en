// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Graphics
{

	//
	// Lock-free Command Buffer Pool
	//

	template <typename NativeCmdBufferType,
			  typename BakedCommandsType
			 >
	struct LfCmdBufferPool
	{
	// types
	public:
		using NativeCmdBuffer_t		= NativeCmdBufferType;
		using BakedCommands_t		= BakedCommandsType;

		STATIC_ASSERT( sizeof(NativeCmdBuffer_t) >= sizeof(BakedCommands_t) );

		union Cmdbuf
		{
			NativeCmdBuffer_t		native;
			BakedCommands_t			baked;

			Cmdbuf () {}
		};
		using Pool_t = StaticArray< Cmdbuf, GraphicsConfig::MaxCmdBufPerBatch >;


	// variables
	protected:


	// methods
	public:
		LfCmdBufferPool ();

		// user api (thread safe)
		ND_ uint  Current ()	const;
		ND_ uint  Acquire ();
			void  Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf);
			void  Add (INOUT uint& idx, BakedCommands_t ctx);
			void  Complete (INOUT uint& idx);

		// owner api
			void  Lock ();
			void  Reset ();
		ND_ bool  IsReady ();
		ND_ bool  IsLocked ();
			
	protected:
			void  _GetCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, uint maxCount);
	};


} // AE::Graphics
