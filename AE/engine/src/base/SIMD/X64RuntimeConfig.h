// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Affects only current thread.
*/

#pragma once

#include "base/Algorithms/Iterators.h"

namespace AE::Base
{

	//
	// Runtime Config
	//
	
	#define AE_SimdRuntimeConfig
	struct SimdRuntimeConfig final : Base::Noninstanceable
	{
	// types
		enum class ERounding : uint
		{
			Nearest,
			Down,
			Up,
			TowardZero,
		};

		enum class ExceptionFlags : uint
		{
			Unknown		= 0,
			Invalid		= 1 << 0,
			Denorm		= 1 << 1,
			DivZero		= 1 << 2,
			Overflow	= 1 << 3,
			Underflow	= 1 << 4,
			Inexact		= 1 << 5,
		};

		struct State
		{
			uint	_value	= 0;
		};


	// methods
		ND_ static ExceptionFlags	GetExceptionState	()					__NE___;
		//	static void				SetExceptionState	(ExceptionFlags)	__NE___;
			static void				ResetExceptionState	()					__NE___;

			static void				ExceptionMask		(ExceptionFlags)	__NE___;

			static void				RoundingMode		(ERounding)			__NE___;
			static void				DenormalFlushToZero	(bool flushToZero)	__NE___;

		ND_ static State			GetState			()					__NE___;
			static void				SetState			(State)				__NE___;
	};

	AE_BIT_OPERATORS( SimdRuntimeConfig::ExceptionFlags );
//-----------------------------------------------------------------------------


	
/*
=================================================
	GetExceptionState
=================================================
*/
	inline SimdRuntimeConfig::ExceptionFlags  SimdRuntimeConfig::GetExceptionState () __NE___
	{
		auto			e	= _MM_GET_EXCEPTION_STATE();
		ExceptionFlags	res	= Default;

		for (auto bit : BitfieldIterate( e ))
		{
			switch ( bit )
			{
				case _MM_EXCEPT_INVALID :		res |= ExceptionFlags::Invalid;		break;
				case _MM_EXCEPT_DENORM :		res |= ExceptionFlags::Denorm;		break;
				case _MM_EXCEPT_DIV_ZERO :		res |= ExceptionFlags::DivZero;		break;
				case _MM_EXCEPT_OVERFLOW :		res |= ExceptionFlags::Overflow;	break;
				case _MM_EXCEPT_UNDERFLOW :		res |= ExceptionFlags::Underflow;	break;
				case _MM_EXCEPT_INEXACT :		res |= ExceptionFlags::Inexact;		break;
			}
		}
		return res;
	}
	
/*
=================================================
	SetExceptionState
=================================================
*
	inline void  SimdRuntimeConfig::SetExceptionState (ExceptionFlags flags) __NE___
	{
		uint e = 0;
		for (ExceptionFlags bit : BitfieldIterate( flags ))
		{
			switch_enum( bit )
			{
				case ExceptionFlags::Invalid :		e |= _MM_EXCEPT_INVALID;	break;
				case ExceptionFlags::Denorm :		e |= _MM_EXCEPT_DENORM;		break;
				case ExceptionFlags::DivZero :		e |= _MM_EXCEPT_DIV_ZERO;	break;
				case ExceptionFlags::Overflow :		e |= _MM_EXCEPT_OVERFLOW;	break;
				case ExceptionFlags::Underflow :	e |= _MM_EXCEPT_UNDERFLOW;	break;
				case ExceptionFlags::Inexact :		e |= _MM_EXCEPT_INEXACT;	break;
				case ExceptionFlags::Unknown :		break;
			}
			switch_end
		}
		_MM_SET_EXCEPTION_STATE( e );
	}
	
/*
=================================================
	ResetExceptionState
=================================================
*/
	inline void  SimdRuntimeConfig::ResetExceptionState () __NE___
	{
		_MM_SET_EXCEPTION_STATE( 0 );
	}
	
/*
=================================================
	ExceptionMask
=================================================
*/
	inline void  SimdRuntimeConfig::ExceptionMask (ExceptionFlags flags) __NE___
	{
		uint m = 0;
		for (ExceptionFlags bit : BitfieldIterate( flags ))
		{
			switch_enum( bit )
			{
				case ExceptionFlags::Invalid :		m |= _MM_EXCEPT_INVALID;	break;
				case ExceptionFlags::Denorm :		m |= _MM_EXCEPT_DENORM;		break;
				case ExceptionFlags::DivZero :		m |= _MM_EXCEPT_DIV_ZERO;	break;
				case ExceptionFlags::Overflow :		m |= _MM_EXCEPT_OVERFLOW;	break;
				case ExceptionFlags::Underflow :	m |= _MM_EXCEPT_UNDERFLOW;	break;
				case ExceptionFlags::Inexact :		m |= _MM_EXCEPT_INEXACT;	break;
				case ExceptionFlags::Unknown :		break;
			}
			switch_end
		}
		_MM_SET_EXCEPTION_MASK( m );
	}

/*
=================================================
	RoundingMode
=================================================
*/
	inline void  SimdRuntimeConfig::RoundingMode (ERounding mode) __NE___
	{
		uint	m = _MM_ROUND_NEAREST;
		switch_enum( mode )
		{
			case ERounding::Nearest :		m = _MM_ROUND_NEAREST;		break;
			case ERounding::Down :			m = _MM_ROUND_DOWN;			break;
			case ERounding::Up :			m = _MM_ROUND_UP;			break;
			case ERounding::TowardZero :	m = _MM_ROUND_TOWARD_ZERO;	break;
		}
		switch_end
		_MM_SET_ROUNDING_MODE( m );
	}
	
/*
=================================================
	DenormalFlushToZero
=================================================
*/
	inline void  SimdRuntimeConfig::DenormalFlushToZero (bool flushToZero) __NE___
	{
		_MM_SET_FLUSH_ZERO_MODE( flushToZero ? _MM_FLUSH_ZERO_ON : _MM_FLUSH_ZERO_OFF );
	}
	
/*
=================================================
	GetState
=================================================
*/
	inline SimdRuntimeConfig::State  SimdRuntimeConfig::GetState () __NE___
	{
		State	s;
		s._value = _mm_getcsr();
		return s;
	}

/*
=================================================
	SetState
=================================================
*/
	inline void  SimdRuntimeConfig::SetState (State state) __NE___
	{
		_mm_setcsr( state._value );
	}


} // AE::Base
