// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Memory/FrameAllocator.h"
#include "graphics/Public/CommandBuffer.h"
#include "graphics/Public/GraphicsProfiler.h"

namespace AE::Graphics::_hidden_
{

	//
	// Base software Command Buffer
	//

	class SoftwareCmdBufBase
	{
	// types
	public:
		static constexpr uint	BaseAlign	= 8;
		static constexpr Bytes	BlockSize	{ DefaultAllocationSize };

		struct Header
		{
			Header *	next	= null;
			Bytes		size;
		};
		STATIC_ASSERT( alignof(Header) <= BaseAlign );
		

		struct BaseCmd
		{
			DEBUG_ONLY(
				static constexpr uint	MAGIC = 0x762a3cf0;
				uint	_magicNumber	= MAGIC;
			)
			ushort		_commandID		= 0;
			ushort		_size			= 0;
		};

	private:
		using Allocator_t = AlignedAllocatorRefBaseAlign< Threading::GraphicsFrameAllocatorRef, BaseAlign >;


	// variables
	protected:
		void *		_dataPtr	= null;
		Bytes		_remainSize;

		Header *	_curBlock	= null;
		void *		_root		= null;		// first block in chain
		

	// methods
	public:
		SoftwareCmdBufBase ()			{}
		~SoftwareCmdBufBase ()			{ ASSERT( _root == null ); }

		ND_ bool	IsValid ()	const	{ return true; }

		static void  Deallocate (void* root);
		

	protected:
		ND_ void*  _Allocate (Bytes size);

		template <typename CommandsList, typename CmdType, typename ...DynamicTypes>
		ND_ CmdType&  _CreateCmd (usize dynamicArraySize = 0);

		template <usize I, typename TL>
		ND_ static constexpr Bytes  _CalcCmdSize (Bytes size, usize dynamicArraySize);
		
		ND_ bool  _Validate (const void* root, usize cmdCount) const;
	};



/*
=================================================
	_CreateCmd
=================================================
*/
	template <typename CommandsList, typename CmdType, typename ...DynamicTypes>
	CmdType&  SoftwareCmdBufBase::_CreateCmd (usize dynamicArraySize)
	{
		Bytes	size	= AlignUp( _CalcCmdSize< 0, TypeList<DynamicTypes...> >( SizeOf<CmdType>, dynamicArraySize ), BaseAlign );
		auto*	cmd		= Cast<CmdType>( _Allocate( size ));

		DEBUG_ONLY( cmd->_magicNumber = BaseCmd::MAGIC; )
		STATIC_ASSERT( CommandsList::template HasType< CmdType >);

		cmd->_commandID = CheckCast<ushort>( CommandsList::template Index< CmdType >);
		cmd->_size		= CheckCast<ushort>( size );
		return *cmd;
	}
	
/*
=================================================
	_CalcCmdSize
=================================================
*/
	template <usize I, typename TL>
	constexpr Bytes  SoftwareCmdBufBase::_CalcCmdSize (Bytes size, usize dynamicArraySize)
	{
		if constexpr( I < TL::Count )
		{
			using T = typename TL::template Get<I>;
			return _CalcCmdSize< I+1, TL >( AlignUp( size, alignof(T) ) + sizeof(T) * dynamicArraySize, dynamicArraySize ); 
		}
		else
			return size;
	}


} // AE::Graphics::_hidden_
