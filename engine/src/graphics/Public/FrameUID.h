// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	FrameUID
		thread-safe:	no

	AtomicFrameUID
		thread-safe:	yes
*/

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Frame Unique ID
	//
	struct FrameUID
	{
		friend struct AtomicFrameUID;

	// types
	private:
		static constexpr uint	_FrameBits = CT_IntLog2< GraphicsConfig::MaxFrames > + 1;
		STATIC_ASSERT( (1u << _FrameBits) - 1 >= GraphicsConfig::MaxFrames );

		// ulong - 58 bits enought for 360 Ghz * 9.3e+9 days
		//       - 28 bits enought for 360 Ghz * 8.6 hours
		// uint  - 26 bits enought for 360 Ghz * 2.1 hours

		using Value_t	= ulong;
		using SValue_t	= ToSignedInteger<Value_t>;

		enum class _EFrameUID : Value_t {};
		
		struct _Bits
		{
			Value_t		counter		: (CT_SizeOfInBits<Value_t> - _FrameBits*2);	// unique frame index, 											
			Value_t		index		: _FrameBits;	// non-unique frame index in range 0 .. GraphicsCreateInfo::maxFrames
			Value_t		maxFrames	: _FrameBits;
		};

		STATIC_ASSERT( sizeof(_Bits) == sizeof(Value_t) );
		STATIC_ASSERT( (CT_SizeOfInBits<Value_t> - _FrameBits*2) >= 28 );


	// variables
	private:
		union {
			_Bits		_bits;
			Value_t		_value	= 0;
		};


	// methods
	private:
		explicit FrameUID (Value_t uid, Value_t idx, Value_t maxFrames) : _bits{ uid, idx, maxFrames } {}

	public:
		FrameUID ()										__NE___ {}

		FrameUID (const FrameUID &)						__NE___	= default;
		FrameUID (FrameUID &&)							__NE___	= default;

		FrameUID&  operator = (const FrameUID &)		__NE___	= default;
		FrameUID&  operator = (FrameUID &&)				__NE___	= default;

		ND_ bool  operator == (const FrameUID &rhs)		C_NE___	{ return _value == rhs._value; }
		ND_ bool  operator != (const FrameUID &rhs)		C_NE___	{ return _value != rhs._value; }

		ND_ bool  operator >  (const FrameUID &rhs)		C_NE___	{ ASSERT( _bits.maxFrames == rhs._bits.maxFrames );  return _bits.counter > rhs._bits.counter; }
		ND_ bool  operator <  (const FrameUID &rhs)		C_NE___	{ ASSERT( _bits.maxFrames == rhs._bits.maxFrames );  return _bits.counter < rhs._bits.counter; }
		
		ND_ bool  operator <=  (const FrameUID &rhs)	C_NE___	{ return not (*this > rhs); }
		ND_ bool  operator >=  (const FrameUID &rhs)	C_NE___	{ return not (*this < rhs); }


		ND_ _EFrameUID	Unique ()						C_NE___	{ return _EFrameUID(_bits.counter); }
		ND_ uint		Index ()						C_NE___	{ return uint(_bits.index); }
		ND_ uint		PrevIndex ()					C_NE___	{ return uint((_bits.index - 1) % _bits.maxFrames); }
		ND_ uint		NextIndex ()					C_NE___	{ return uint((_bits.index + 1) % _bits.maxFrames); }
		ND_ uint		MaxFrames ()					C_NE___	{ return uint(_bits.maxFrames); }
		ND_ bool		IsValid ()						C_NE___	{ return _bits.maxFrames > 0; }


		FrameUID&  Inc () __NE___
		{
			ASSERT( _bits.maxFrames > 0 );
			_bits.index = (_bits.index + 1) % _bits.maxFrames;
			++_bits.counter;
			ASSERT( (_bits.counter % _bits.maxFrames) == _bits.index );
			return *this;
		}

		ND_ Optional<FrameUID>  PrevCycle () C_NE___
		{
			if_likely( _bits.maxFrames <= _bits.counter )
			{
				FrameUID	id{ _bits.counter - _bits.maxFrames, 0, MaxFrames() };
				ASSERT( id.Unique() < Unique() );
				return {id};
			}
			return NullOptional;
		}

		ND_ SValue_t  Diff (FrameUID rhs) C_NE___
		{
			return SValue_t(_bits.counter) - SValue_t(rhs._bits.counter);
		}

		ND_ static FrameUID  FromIndex (uint idx, uint maxFrames) __NE___
		{
			ASSERT( maxFrames <= GraphicsConfig::MaxFrames );
			ASSERT( idx < maxFrames );
			return FrameUID{ 0, idx, maxFrames };
		}

		ND_ static FrameUID  Init (uint maxFrames) __NE___
		{
			CHECK( maxFrames > 0 );
			CHECK( maxFrames <= GraphicsConfig::MaxFrames );
			return FrameUID{ 0, 0, maxFrames };
		}
	};


	
	//
	// Atomic Frame Unique ID
	//
	struct AtomicFrameUID
	{
	// types
	private:
		using Value_t = FrameUID::Value_t;


	// variables
	private:
		Atomic< Value_t >	_value {0};
		

	// methods
	public:
		AtomicFrameUID ()				__NE___ {}

		ND_ FrameUID  load ()			C_NE___
		{
			return BitCast<FrameUID>( _value.load() );
		}

		void  store (FrameUID value)	__NE___
		{
		#ifdef AE_DEBUG
			FrameUID	old = BitCast<FrameUID>( _value.exchange( BitCast<Value_t>( value )));
			ASSERT( old.Unique() <= value.Unique() );
		#else
			_value.store( BitCast<Value_t>( value ));
		#endif
		}

		FrameUID  Inc ()				__NE___
		{
			Value_t	new_val;
			for (Value_t exp = _value.load();;)
			{
				new_val = BitCast<Value_t>( BitCast<FrameUID>( exp ).Inc() );

				if_likely( _value.CAS( INOUT exp, new_val ))
					break;

				ThreadUtils::Pause();
			}
			return BitCast<FrameUID>( new_val );
		}
	};


} // AE::Graphics
