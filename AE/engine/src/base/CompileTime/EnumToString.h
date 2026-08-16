// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_LOGS
# include "base/Common.h"
# include "base/Algorithms/Iterators.h"

namespace AE::Base
{

	//
	// Enum to String Helper
	//

	template <typename T>
	struct _EnumToStringConverter
	{
	protected:
		StaticAssert( IsEnum<T> );

		template <T Value>
		NdCx__ static StringView  Convert ()
		{
		#if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL)
			constexpr StringView	signature	= __FUNCSIG__;
			constexpr StringView	prefix		= "Convert<";
			constexpr StringView	suffix		= ">(void)";

			const usize		begin = signature.find( prefix );
			if constexpr( begin == StringView::npos )
				return {};

			const usize	value_begin	= begin + prefix.size();
			const usize	value_end	= signature.find( suffix, value_begin );

			if constexpr( value_end == StringView::npos )
				return {};

			StringView	name = signature.substr( value_begin, value_end - value_begin );

			if ( name.front() == '(' )
				return {};

		#else
			constexpr StringView	signature	= __PRETTY_FUNCTION__;
			constexpr StringView	prefix		= "Value = ";

			const usize		begin = signature.find( prefix );
			if constexpr( begin == StringView::npos )
				return {};

			const usize	value_begin	= begin + prefix.size();
			const usize	value_end	= std::min( signature.find_first_of( ";]", value_begin ), signature.size() );

			StringView	name = signature.substr( value_begin, value_end - value_begin );
		#endif

			if ( name.empty()									or
				 name.find( "static_cast<" ) != StringView::npos )
			{
				return {};
			}

			// remove namespace and enum-class qualification
			const usize	scope = name.rfind( "::" );
			if ( scope != StringView::npos )
				name.remove_prefix( scope + 2 );

			// skip hidden values
			if ( name.front() == '_' )
				return {};

			if ( name == "Unknown" )
				return {};

			return name;
		}
	};



	//
	// Enum to String
	//

	template <typename T>
	struct EnumToString : _EnumToStringConverter<T>
	{
	private:
		StaticAssert( IsEnum<T> );
		StaticAssert( uint(T::_Count) < 256 );

		template <usize ...I>
		NdCx__ static auto  _MakeTable (IndexSequence<I...>)
		{
			return StaticArray< StringView, sizeof...(I) >{ _EnumToStringConverter<T>::template Convert< T(I) >()... };
		}

		inline static constexpr auto	c_Table = _MakeTable( MakeIndexSequence< usize(T::_Count) >{});

	public:
		NdCx__ static StringView  ToString (T value) __NE___
		{
			if ( uint(value) < uint(T::_Count) )
				return c_Table[ uint(value) ];
			else
				return {};
		}
	};



	//
	// Bit Enum to String
	//

	template <typename T>
	struct BitEnumToString : _EnumToStringConverter<T>
	{
	private:
		using U = ToUnsignedInteger<T>;

		StaticAssert( IsEnum<T> );
		StaticAssert( requires{ T::_Last; } and (requires{ T::All; } or requires{ T::_BITOPS_; }));

		static constexpr auto	c_Count = CT_IntLog2< U(T::_Last) > + 1;
		StaticAssert( c_Count <= 64 );

		template <uint I>
		NdCx__ static StringView  Convert2 ()
		{
			constexpr U	bit = U{1} << I;
			return _EnumToStringConverter<T>::template Convert< T(bit) >();
		}

		template <usize ...I>
		NdCx__ static auto  _MakeTable (IndexSequence<I...>)
		{
			return StaticArray< StringView, sizeof...(I) >{ Convert2<I>()... };
		}

		inline static constexpr auto	c_Table = _MakeTable( MakeIndexSequence< c_Count >{});

	public:
		NdCx__ static StringView  BitToString (T value) __NE___
		{
			CHECK_ERR( IsSingleBitSet( U(value) ));
			int	idx = IntLog2( U(value) );
			CHECK_ERR( idx < c_Count );
			return c_Table[ idx ];
		}

		ND_ static String  ToString (T values) __Th___
		{
			String	str;
			bool	separate = false;

			for (auto t : BitfieldIterate( values ))
			{
				if ( separate )
					str += " | ";

				StringView	s = BitToString( t );
				separate = not s.empty();

				str += s;
			}
			return str;
		}
	};

} // AE::Base
#endif // AE_ENABLE_LOGS
