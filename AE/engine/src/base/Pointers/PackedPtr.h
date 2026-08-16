// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	ref: https://stackoverflow.com/questions/16198700/using-the-extra-16-bits-in-64-bit-pointers

	With 64 byte align:
	- x64 uses only 48 bits (47 for user space).
	- minus 6 bits for 64 byte alignment.
	- 41..42 bits for pointer, 22..23 bits for extra data which is 4M.

	Android add tagged pointers which uses bits 56-63.
	AArch64 supports multiple VA sizes depending on kernel/page table configuration, for example 39, 42, 48, 52 bits.
*/

#pragma once

#include "base/Common.h"
#include "base/CompileTime/Math.h"
#include "base/Math/BitMath.h"
#include "base/Math/Vec.h"
#include "base/Math/POTValue.h"

namespace AE::Base
{
namespace _hidden_
{
	template <typename T>
	struct PackedPtr_Align { static constexpr usize  value = alignof(T); };

	template <>
	struct PackedPtr_Align<void> { static constexpr usize  value = 1; };
}


	//
	// Packed Pointer
	//

	template <typename T,
			  usize Align_v = Base::_hidden_::PackedPtr_Align<T>::value >
	class PackedPtr
	{
		StaticAssert( IsPowerOfTwo( Align_v ));

	// types
	public:
		using Value_t	= T;
		using Self		= PackedPtr< T, Align_v >;

	private:
		static constexpr usize	c_Align			= Align_v;
		static constexpr uint	c_AlignPOT		= uint(CT_IntLog2< Align_v >);

	  #if defined(AE_PLATFORM_ANDROID) and defined(AE_CPU_ARCH_ARM64)
		StaticAssert( sizeof(void*) == 8 );
		static constexpr bool	c_TaggedPtr		= true;
		static constexpr uint	c_MaxPtrBits	= 48 + 8;							// default 48 bits but can be extended to 52?
		static constexpr uint	c_PtrBits		= c_MaxPtrBits - c_AlignPOT;
		static constexpr uint	c_ExtraOffset	= c_PtrBits - 8;
		static constexpr usize	c_PtrLowPart	= ToBitMask<usize>( c_ExtraOffset );
		static constexpr usize	c_PtrHighPart	= 0xFFull << 56;
		static constexpr usize	c_PtrMask		= c_PtrLowPart | c_PtrHighPart;
	  #else
		static constexpr bool	c_TaggedPtr		= false;
		static constexpr uint	c_MaxPtrBits	= sizeof(void*) >= 8 ? 48 : 32;		// can be 47 bits
		static constexpr uint	c_PtrBits		= c_MaxPtrBits - c_AlignPOT;
		static constexpr uint	c_ExtraOffset	= c_PtrBits;
		static constexpr usize	c_PtrMask		= ToBitMask<usize>( c_PtrBits );
		static constexpr usize	c_PtrLowPart	= c_PtrMask;	// unused
		static constexpr usize	c_PtrHighPart	= 0;			// unused
	  #endif

		static constexpr uint	c_ExtraBits		= CT_SizeOfInBits<usize> - c_PtrBits;
		static constexpr usize	c_ExtraMax		= ToBitMask<usize>( c_ExtraBits );	// mask without offset


	// variables
	private:
		usize	_value	= 0;


	// methods
	public:
		PackedPtr ()									__NE___	{}
		PackedPtr (const Self &)						__NE___ = default;
		PackedPtr (Self &&)								__NE___ = default;

			Self&		operator = (const Self &)		__NE___	= default;
			Self&		operator = (Self &&)			__NE___	= default;

		ND_ T*			Ptr ()							__NE___	{ return BitCast<T*>( _Ptr() ); }
		ND_ T const*	Ptr ()							C_NE___	{ return BitCast<T const*>( _Ptr() ); }

			void		SetPtr (T* value)				__NE___;

		ND_ usize		Extra ()						C_NE___;
			void		SetExtra (usize value)			__NE___;

		NdCx__ static usize		ExtraBits ()			__NE___	{ return c_ExtraBits; }
		NdCx__ static usize		ExtraMax ()				__NE___	{ return c_ExtraMax; }
		NdCx__ static POTBytes	Align ()				__NE___	{ return POTBytes{ PowerOfTwo{ c_AlignPOT }}; }

	private:
		ND_ usize		_Ptr ()							C_NE___;
	};


/*
=================================================
	Extra
=================================================
*/
	template <typename T, usize A>
	usize  PackedPtr<T,A>::Extra () C_NE___
	{
		return (_value >> c_ExtraOffset) & c_ExtraMax;
	}

/*
=================================================
	_Ptr
=================================================
*/
	template <typename T, usize A>
	usize  PackedPtr<T,A>::_Ptr () C_NE___
	{
		if constexpr( c_TaggedPtr )
		{
			return	(_value & c_PtrHighPart) |		// tag
					(_value & c_PtrLowPart) << c_AlignPOT;
		}
		else
		{
			return (_value & c_PtrMask) << c_AlignPOT;
		}
	}

/*
=================================================
	SetPtr
=================================================
*/
	template <typename T, usize A>
	void  PackedPtr<T,A>::SetPtr (T* value) __NE___
	{
		usize	v = BitCast<usize>( value );
		ASSERT( IsMultipleOf( v, Align() ));

		if constexpr( c_TaggedPtr )
		{
			// keep tag in last 8 bits
			v = (v & c_PtrHighPart) |
				((v >> c_AlignPOT) & c_PtrLowPart);
		}
		else
		{
			v >>= c_AlignPOT;
		}

		ASSERT_MSG( (v & c_PtrMask) == v,
			"Assertion '(v & c_PtrMask) == v' failed:\n"
			"pointer: 0x"s << ToString<16>( v ) << ", mask: 0x" << ToString<16>( c_PtrMask ) <<
			", result 0x" << ToString<16>( v & c_PtrMask ) << ", align " << ToString( c_Align ));

		_value &= ~c_PtrMask;  // remove ptr
		_value |= (v & c_PtrMask);
	}

/*
=================================================
	SetExtra
=================================================
*/
	template <typename T, usize A>
	void  PackedPtr<T,A>::SetExtra (usize value) __NE___
	{
		ASSERT( value <= c_ExtraMax );
		StaticAssert( c_ExtraMax > 0 );

		_value &= c_PtrMask;	// remove extra
		_value |= (value & c_ExtraMax) << c_ExtraOffset;
	}


} // AE::Base
