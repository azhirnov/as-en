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
		FrameUID () {}

		FrameUID (const FrameUID &) = default;
		FrameUID (FrameUID &&) = default;

		FrameUID&  operator = (const FrameUID &) = default;
		FrameUID&  operator = (FrameUID &&) = default;

		ND_ bool  operator == (const FrameUID &rhs) const	{ return _value == rhs._value; }
		ND_ bool  operator != (const FrameUID &rhs) const	{ return _value != rhs._value; }

		ND_ bool  operator >  (const FrameUID &rhs) const	{ ASSERT( _bits.maxFrames == rhs._bits.maxFrames );  return _bits.counter > rhs._bits.counter; }
		ND_ bool  operator <  (const FrameUID &rhs) const	{ ASSERT( _bits.maxFrames == rhs._bits.maxFrames );  return _bits.counter < rhs._bits.counter; }
		
		ND_ bool  operator <=  (const FrameUID &rhs) const	{ return not (*this > rhs); }
		ND_ bool  operator >=  (const FrameUID &rhs) const	{ return not (*this < rhs); }


		ND_ _EFrameUID	Unique ()		const	{ return _EFrameUID(_bits.counter); }
		ND_ uint		Index ()		const	{ return uint(_bits.index); }
		ND_ uint		PrevIndex ()	const	{ return uint((_bits.index - 1) % _bits.maxFrames); }
		ND_ uint		NextIndex ()	const	{ return uint((_bits.index + 1) % _bits.maxFrames); }
		ND_ uint		MaxFrames ()	const	{ return uint(_bits.maxFrames); }
		ND_ bool		IsValid ()		const	{ return _bits.maxFrames > 0; }


		FrameUID&  Inc ()
		{
			ASSERT( _bits.maxFrames > 0 );
			_bits.index = (_bits.index + 1) % _bits.maxFrames;
			++_bits.counter;
			ASSERT( (_bits.counter % _bits.maxFrames) == _bits.index );
			return *this;
		}

		ND_ Optional<FrameUID>  PrevCycle () const
		{
			if_likely( _bits.maxFrames <= _bits.counter )
			{
				FrameUID	id{ _bits.counter - _bits.maxFrames, 0, MaxFrames() };
				ASSERT( id.Unique() < Unique() );
				return {id};
			}
			return NullOptional;
		}

		ND_ SValue_t  Diff (FrameUID rhs) const
		{
			return SValue_t(_bits.counter) - SValue_t(rhs._bits.counter);
		}

		ND_ static FrameUID  FromIndex (uint idx, uint maxFrames)
		{
			ASSERT( maxFrames <= GraphicsConfig::MaxFrames );
			ASSERT( idx < maxFrames );
			return FrameUID{ 0, idx, maxFrames };
		}

		ND_ static FrameUID  Init (uint maxFrames)
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
		AtomicFrameUID () {}

		ND_ FrameUID  load () const
		{
			return BitCast<FrameUID>( _value.load() );
		}

		void  store (FrameUID value)
		{
		#ifdef AE_DEBUG
			FrameUID	old = BitCast<FrameUID>( _value.exchange( BitCast<Value_t>( value )));
			ASSERT( old.Unique() <= value.Unique() );
		#else
			_value.store( BitCast<Value_t>( value ));
		#endif
		}

		FrameUID  Inc ()
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
