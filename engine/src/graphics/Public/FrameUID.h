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
		explicit FrameUID (Value_t uid, Value_t idx, Value_t maxFrames);

	public:
		FrameUID () {}

		FrameUID (const FrameUID &) = default;
		FrameUID (FrameUID &&) = default;

		FrameUID&  operator = (const FrameUID &) = default;
		FrameUID&  operator = (FrameUID &&) = default;

		ND_ bool  operator == (const FrameUID &rhs) const;
		ND_ bool  operator != (const FrameUID &rhs) const;


		ND_ _EFrameUID	Unique ()		const;
		ND_ uint		Index ()		const;
		ND_ uint		MaxFrames ()	const;
		ND_ bool		IsValid ()		const;


		FrameUID&  Inc ();

		ND_ Optional<FrameUID>  PrevCycle () const;

		ND_ SValue_t  Diff (FrameUID rhs) const;

		ND_ static FrameUID  FromIndex (uint idx, uint maxFrames);

		ND_ static FrameUID  Init (uint maxFrames);
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

		ND_ FrameUID  load () const;

		void  store (FrameUID value);

		FrameUID  Inc ();
	};


}	// AE::Graphics
