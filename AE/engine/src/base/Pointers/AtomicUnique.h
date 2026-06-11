// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/Atomic.h"

namespace AE::Base
{

	//
	// Atomic Unique
	//

	template <typename T>
	struct AtomicUnique
	{
	// types
	public:
		using Value_t		= T;
		using Unique_t		= Unique<T>;
		using Self			= AtomicUnique<T>;


	// variables
	private:
		Atomic< T *>	_ptr {null};


	// methods
	public:
		AtomicUnique ()															__NE___	{}
		~AtomicUnique ()														__NE___	{ Unused( release() );  StaticAssertMsg( alignof(T) > 1, "first bit is used for lock bit" ); }

		ND_ T *			unsafe_get ()											C_NE___ { return _RemoveLockBit( _ptr.load() ); }
		ND_ Unique_t	release ()												__NE___;

		ND_ Unique_t	load ()													__NE___;

			void		store (T* ptr)											__NE___;
			void		store (Unique_t ptr)									__NE___;

			void		reset ()												__NE___;

		ND_ Unique_t	exchange (T* desired)									__NE___;
		ND_ Unique_t	exchange (Unique_t desired)								__NE___;

		ND_ bool		CAS (INOUT Unique_t& expected, Unique_t desired)		__NE___	{ return _CAS<false>( INOUT expected, RVRef(desired) ); }
		ND_ bool		CAS_Loop (INOUT Unique_t& expected, Unique_t desired)	__NE___	{ return _CAS<true>( INOUT expected, RVRef(desired) ); }


	private:
		ND_ T*			_Lock ()												__NE___;
			void		_Unlock ()												__NE___;
		ND_ T*			_Exchange (T* ptr)										__NE___;

		template <bool IsStrong>
		ND_ bool		_CAS (INOUT Unique_t& expected, Unique_t desired)		__NE___;

		ND_ static bool	_HasLockBit (T* ptr)									__NE___	{ return (usize(ptr) & usize{1}); }
		ND_ static T*	_SetLockBit (T* ptr)									__NE___ { return reinterpret_cast< T *>((usize(ptr) | usize{1})); }
		ND_ static T*	_RemoveLockBit (T* ptr)									__NE___ { return reinterpret_cast< T *>((usize(ptr) & ~usize{1})); }
	};


} // AE::Base
