// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
		arm64 FPCR https://arm.jonpalmisc.com/2023_09_sysreg/AArch64-fpcr
		arm64 FPSR https://arm.jonpalmisc.com/2023_09_sysreg/AArch64-fpsr
		arm32 FPSCR https://arm.jonpalmisc.com/2023_09_sysreg/AArch32-fpscr
*/

#pragma once

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
			TowardPlus,
			TowardMinus,
			TowardZero,

			Down	= TowardMinus,
			Up		= TowardPlus,
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
			IntSat		= 1 << 6,
		};

	  #ifdef AE_CPU_ARCH_ARM64
		using FPCR_t	= ulong;
		using FPSR_t	= ulong;

	  #elif defined(AE_CPU_ARCH_ARM32)
		using FPCR_t	= uint;
		using FPSR_t	= uint;
	  #endif

		struct State
		{
			FPCR_t		_fpcr	= 0;
			
			ND_ bool  		DenormalFlushToZero ()						C_NE___;
			ND_ ERounding	RoundingMode ()								C_NE___;

			ND_ bool		Fp3264DenormalFlushToZero ()				C_NE___;	// non-portable			
			ND_ bool		Fp16DenormalFlushToZero	()					C_NE___;	// non-portable			
			ND_ bool		Fp3264DenormalFlushToZero_InputAndOutput ()	C_NE___;	// non-portable, input and output or only output
			
			ND_ bool		Fp16AlternativeFormat ()					C_NE___;	// non-portable
		};


	// methods
		ND_ static ExceptionFlags	GetExceptionState			()					__NE___;
			static void				ResetExceptionState			()					__NE___;

			static void				ExceptionMask				(ExceptionFlags)	__NE___;

			static void				RoundingMode				(ERounding)			__NE___;
			static void				DenormalFlushToZero			(bool flushToZero)	__NE___;

			static void				Fp3264DenormalFlushToZero	(bool flushToZero)	__NE___;	// non-portable
			static void				Fp16DenormalFlushToZero		(bool flushToZero)	__NE___;	// non-portable

			static void				Fp16AlternativeFormat		(bool enable)		__NE___;	// non-portable

		ND_ static State			GetState					()					__NE___;
			static void				SetState					(State)				__NE___;
	};

	AE_BIT_OPERATORS( SimdRuntimeConfig::ExceptionFlags );


/*
=================================================
	RoundingMode
=================================================
*/
	inline void  SimdRuntimeConfig::RoundingMode (const ERounding mode) __NE___
	{
		uint	bits = 0;
		switch_enum( mode )
		{
			case ERounding::Nearest :		bits = 0;		break;
			case ERounding::TowardPlus :	bits = 0b01;	break;
			case ERounding::TowardMinus :	bits = 0b10;	break;
			case ERounding::TowardZero :	bits = 0b11;	break;
		}
		switch_end

		auto	s = GetState();

		s._fpcr &= ~(3u << 22);		// clear bits 22, 23
		s._fpcr |= bits;

		SetState( s );
	}

	inline SimdRuntimeConfig::ERounding  SimdRuntimeConfig::State::RoundingMode () C_NE___
	{
		uint	bits = (_fpcr >> 22) & 0x3;
		switch ( bits )
		{
			case 0 :		return ERounding::Nearest;
			case 0b01 :		return ERounding::TowardPlus;
			case 0b10 :		return ERounding::TowardMinus;
			case 0b11 :		return ERounding::TowardZero;
		}
		// never happens
		return ERounding::Nearest;
	}
			
/*
=================================================
	DenormalFlushToZero
=================================================
*/
	inline void  SimdRuntimeConfig::DenormalFlushToZero (bool flushToZero) __NE___
	{
		auto	s 	 = GetState();
		uint	fz 	 = /*fp32-64*/(1 << 24) | /*fp16*/(1 << 19);
		uint	ah 	 = 1 << 1;
		
		s._fpcr &= ~(fz | ah);
		s._fpcr |= (flushToZero ? fz : 0) | ah;

		SetState( s );
	}

	inline bool  SimdRuntimeConfig::State::DenormalFlushToZero () C_NE___
	{
		return HasBit<24>( _fpcr ) and HasBit<19>( _fpcr );
	}

/*
=================================================
	Fp3264DenormalFlushToZero
----
	0 bit -	If FPCR.AH is 1, the flushing to zero of single-precision and double-precision denormalized outputs
			of floating-point instructions not enabled by this control, but other factors might cause the input
			denormalized numbers to be flushed to zero.
	1 bit - If FPCR.AH is 1, denormalized single-precision and double-precision outputs
			from floating-point instructions are flushed to zero.
=================================================
*/
	inline void  SimdRuntimeConfig::Fp3264DenormalFlushToZero (bool flushToZero) __NE___
	{
		auto	s = GetState();
		uint	fz = (1u << 24);
		uint	ah = (1 << 1);		// FPCR.AH

		s._fpcr &= ~(fz | ah);
		s._fpcr |= (flushToZero ? fz : 0) | ah;

		SetState( s );
	}

	inline bool  SimdRuntimeConfig::State::Fp3264DenormalFlushToZero () C_NE___
	{
		return HasBit<24>( _fpcr );
	}

/*
=================================================
	Fp16DenormalFlushToZero
=================================================
*/
	inline void  SimdRuntimeConfig::Fp16DenormalFlushToZero (bool flushToZero) __NE___
	{
		auto	s = GetState();
		uint	m = 1u << 19;

		s._fpcr &= ~m;
		s._fpcr |= flushToZero ? m : 0;

		SetState( s );
	}

	inline bool  SimdRuntimeConfig::State::Fp16DenormalFlushToZero () C_NE___
	{
		return HasBit<19>( _fpcr );
	}
	
/*
=================================================
	Fp3264DenormalFlushToZero_InputAndOutput
=================================================
*/
	inline bool  SimdRuntimeConfig::State::Fp3264DenormalFlushToZero_InputAndOutput () C_NE___
	{
		return not HasBit<1>( _fpcr );
	}
			
/*
=================================================
	Fp16AlternativeFormat
----
	ARM alternative format.
	This representation is similar to the IEEE format, but does not support infinities or NaNs.
	Instead, the range of exponents is extended, so that this format can represent normalized values in the range of 2^-14^ to 131008.
=================================================
*/
	inline void  SimdRuntimeConfig::Fp16AlternativeFormat (bool enable) __NE___
	{
		auto	s = GetState();
		uint	m = 1u << 26;

		s._fpcr &= ~m;
		s._fpcr |= enable ? m : 0;

		SetState( s );
	}

	inline bool  SimdRuntimeConfig::State::Fp16AlternativeFormat () C_NE___
	{
		return HasBit<26>( _fpcr );
	}

/*
=================================================
	GetState
=================================================
*/
	inline SimdRuntimeConfig::State  SimdRuntimeConfig::GetState () __NE___
	{
	#ifdef AE_CPU_ARCH_ARM64

		State	state;
		asm( "mrs %0,   fpcr" : "=r"( state._fpcr ));	// __builtin_aarch64_get_fpcr()
		return state;

	#endif
	#ifdef AE_CPU_ARCH_ARM32

		// exception state
		uint	mask = 0;
		mask |= ToBitMask<ulong>( 5 );	// bits [0, 4]
		mask |= 1u << 7;
		mask |= 1u << 27;

		State	state;
		asm( "vmrs %0,   fpscr" : "=r"( state._fpcr ));

		// remove exceptions
		state._fpcr &= ~mask;

		return state;

	#endif
	}

/*
=================================================
	SetState
=================================================
*/
	inline void  SimdRuntimeConfig::SetState (const State state) __NE___
	{
	#ifdef AE_CPU_ARCH_ARM64

		asm( "msr fpcr, %0"   :: "r"( state._fpcr ));	// __builtin_aarch64_set_fpsr()

	#endif
	#ifdef AE_CPU_ARCH_ARM32

		asm( "vmsr fpscr, %0"   :: "r"( state._fpcr ));

	#endif
	}

/*
=================================================
	ExceptionMask
=================================================
*/
	inline void  SimdRuntimeConfig::ExceptionMask (const ExceptionFlags flags) __NE___
	{
		uint	mask = 0;
		mask |= ToBitMask<ulong>( 5 ) << 8;	// bits [8, 12]
		mask |= 1u << 15;

		uint	add = 0;
		for (ExceptionFlags bit : BitfieldIterate( flags ))
		{
			switch_enum( bit )
			{
				case ExceptionFlags::Invalid :		add |= 1u << 8;		break;
				case ExceptionFlags::Denorm :		add |= 1u << 15;	break;
				case ExceptionFlags::DivZero :		add |= 1u << 9;		break;
				case ExceptionFlags::Overflow :		add |= 1u << 10;	break;
				case ExceptionFlags::Underflow :	add |= 1u << 11;	break;
				case ExceptionFlags::Inexact :		add |= 1u << 12;	break;
				case ExceptionFlags::IntSat :		break;
				case ExceptionFlags::Unknown :		break;
			}
			switch_end
		}

		#ifdef AE_CPU_ARCH_ARM64

			ulong	fpcr;
			asm( "mrs %0,   fpcr" : "=r"( fpcr ));

			fpcr &= ~mask;
			fpcr |= add;

			asm( "msr fpcr, %0"   :: "r"( fpcr ));

		#endif
		#ifdef AE_CPU_ARCH_ARM32

			// Warning: only for scalar FP !!!

			uint	fpscr;
			asm( "vmrs %0,   fpscr" : "=r"( fpscr ));

			fpscr &= ~mask;
			fpscr |= add;

			asm( "vmsr fpscr, %0"   :: "r"( fpscr ));

		#endif
	}

/*
=================================================
	GetExceptionState
=================================================
*/
	inline SimdRuntimeConfig::ExceptionFlags  SimdRuntimeConfig::GetExceptionState () __NE___
	{
		#ifdef AE_CPU_ARCH_ARM64

			ulong	reg;
			asm( "mrs %0,   fpsr" : "=r"( reg ));

		#endif
		#ifdef AE_CPU_ARCH_ARM32

			uint	reg;
			asm( "vmrs %0,   fpscr" : "=r"( reg ));

		#endif

		uint	mask = 0;
		mask |= ToBitMask<ulong>( 5 );	// bits [0, 4]
		mask |= 1u << 7;
		mask |= 1u << 27;

		ExceptionFlags	res	= Default;

		for (auto idx : BitIndexIterate( reg & mask ))
		{
			switch ( idx )
			{
				case 0 :	res |= ExceptionFlags::Invalid;		break;
				case 1 :	res |= ExceptionFlags::DivZero;		break;
				case 2 :	res |= ExceptionFlags::Overflow;	break;
				case 3 :	res |= ExceptionFlags::Underflow;	break;
				case 4 :	res |= ExceptionFlags::Inexact;		break;

				case 7 :	res |= ExceptionFlags::Denorm;		break;
				case 27 :	res |= ExceptionFlags::IntSat;		break;
			}
		}
		return res;
	}

/*
=================================================
	ResetExceptionState
=================================================
*/
	inline void  SimdRuntimeConfig::ResetExceptionState () __NE___
	{
		uint	mask = 0;
		mask |= ToBitMask<ulong>( 5 );	// bits [0, 4]
		mask |= 1u << 7;
		mask |= 1u << 27;

		#ifdef AE_CPU_ARCH_ARM64

			ulong	fpsr;
			asm( "mrs %0,   fpsr" : "=r"( fpsr ));

			fpsr &= mask;
			asm( "msr fpsr, %0"   :: "r"( fpsr ));

		#endif
		#ifdef AE_CPU_ARCH_ARM32

			uint	fpscr;
			asm( "vmrs %0,   fpscr" : "=r"( fpscr ));

			fpscr &= mask;
			asm( "vmsr fpscr, %0"   :: "r"( fpscr ));

		#endif
	}


} // AE::Base
