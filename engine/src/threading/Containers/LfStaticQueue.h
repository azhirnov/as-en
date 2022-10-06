// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Purpose:
	* lock-free multiple producers
	* blocked for extracting multiple elements (single consumer)
	* lock-free for extracting single element
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/Math/BitMath.h"
# include "base/Math/Bytes.h"
# include "base/Math/Math.h"
# include "base/Memory/UntypedAllocator.h"
# include "base/Memory/CopyPolicy.h"
# include "threading/Primitives/Atomic.h"
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{

	//
	// Lock-free Static Queue
	//

	template <typename Value,
			  typename AllocatorType = UntypedAlignedAllocator>
	class LfStaticQueue
	{
	// types
	public:
		using Self			= LfStaticQueue< AllocatorType >;
		using Allocator_t	= AllocatorType;
		using Value_t		= Value;

	private:
		static constexpr bool	_NonTrivialDtor	= not (IsZeroMemAvailable<Value> or std::is_trivially_destructible_v<Value>);
		static constexpr uint	_MaxSize		= 1 << 12;
		static constexpr uint	_MaxSpin		= 2'000;

		struct Bits
		{
			uint	first	: 15;
			uint	last	: 15;
			uint	locked	: 1;

			Bits () : first{0}, last{0}, locked{0} {}

			ND_ bool  Empty ()	const	{ return first >= last; }
			ND_ uint  Count ()	const	{ return first < last ? last - first : 0; }
		};


	// variables
	private:
		BitAtomic<Bits>		_packed		{ Bits{} };
		uint				_count		= 0;
		void *				_arr		= null;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;

		DRC_ONLY(
			RWDataRaceCheck	_drCheck;
		)


	// methods
	public:
		LfStaticQueue (const Allocator_t &alloc = Allocator_t{}) : _allocator{alloc} {}
		~LfStaticQueue () { Release(); }

			bool  Init (usize size);
			void  Release ();
			void  Clear ();

			bool  Push (const Value_t &value);
			bool  Push (Value_t &&value);

		ND_ bool  Empty ();
		ND_ uint  Count ();
		ND_ bool  IsLockedForRead ();

		ND_ bool  First (OUT Value_t &value);
		ND_ bool  Pop (OUT Value_t &value);

		template <typename ArrayType>
		ND_ bool  ExtractAll (OUT ArrayType &dstArr);

		template <typename FN>
			void  Visit (FN &&fn);
	};


} // AE::Threading
