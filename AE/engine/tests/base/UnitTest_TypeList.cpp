// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	struct TL_Visitor
	{
		usize sizeof_sum = 0;

		template <typename T, usize I>
		void operator () () __NE___ {
			sizeof_sum += sizeof(T);
		}
	};

	template <typename T>
	struct GreaterThen4 : CT_Bool< (sizeof(T) > 4) >{};

	template <typename T>
	struct TypeSize {
		static constexpr usize	value = sizeof(T);
	};
}


extern void UnitTest_TypeList ()
{
	{
		StaticAssert(( TypeList<int, bool, float>::Count == 3 ));
		StaticAssert(( TypeList<int, int, int>::Count == 3 ));
	}
	{
		using TL = TypeList< int, float, bool, double >;
		StaticAssert( IsTypeList< TL > );
		StaticAssert( not IsTypeList< int > );
	}
	{
		using TL = TypeList< int, float, bool, double >;
		StaticAssert( IsSame< TL::Get<0>, int > );
		StaticAssert( IsSame< TL::Get<1>, float > );
		StaticAssert( IsSame< TL::Get<2>, bool > );
		StaticAssert( IsSame< TL::Get<3>, double > );
		StaticAssert( TL::Index<int> == 0 );
		StaticAssert( TL::Index<float> == 1 );
		StaticAssert( TL::Index<bool> == 2 );
		StaticAssert( TL::Index<double> == 3 );
		StaticAssert( TL::IndexOrMax<slong> == UMax );
	}
	{
		using TL = TypeList< int, bool, int, float >;
		StaticAssert( TL::FirstIndex<int> == 0 );
		StaticAssert( TL::FirstIndex<bool> == 1 );
		StaticAssert( TL::FirstIndex<float> == 3 );
		StaticAssert( TL::FirstIndex<double> == UMax );
		StaticAssert( TL::LastIndex<int> == 2 );
		StaticAssert( TL::LastIndex<bool> == 1 );
		StaticAssert( TL::LastIndex<float> == 3 );
		StaticAssert( TL::LastIndex<double> == UMax );
	}
	{
		using TL = TypeList< int, float, bool, double >;
		TL_Visitor v;
		TL::Visit( v );
		TEST( v.sizeof_sum == (sizeof(int) + sizeof(float) + sizeof(bool) + sizeof(double)) );
	}
	{
		using TL = TypeList< int, float, bool, double >;
		using TL2 = TL::Reverse::type;
		StaticAssert( TL2::Count == 4 );
		StaticAssert( IsSame< TL2::Get<0>, double >);
		StaticAssert( IsSame< TL2::Get<1>, bool >);
		StaticAssert( IsSame< TL2::Get<2>, float >);
		StaticAssert( IsSame< TL2::Get<3>, int >);
		StaticAssert(( IsSame< TL2, TypeList< double, bool, float, int > >));
	}
	{
		using TL = TypeList<>;
		StaticAssert( TL::Count == 0 );

		using TL2 = TL::PushBack<int>;
		StaticAssert( TL2::Count == 1 );
		StaticAssert( IsSame< TL2::Get<0>, int >);
		StaticAssert( IsSame< TL2, TypeList<int> >);

		using TL3 = TL2::PushBack<float>;
		StaticAssert( TL3::Count == 2 );
		StaticAssert( IsSame< TL3::Get<0>, int >);
		StaticAssert( IsSame< TL3::Get<1>, float >);
		StaticAssert(( IsSame< TL3, TypeList<int, float> >));

		using TL4 = TL3::PushFront<bool>;
		StaticAssert( TL4::Count == 3 );
		StaticAssert( IsSame< TL4::Get<0>, bool >);
		StaticAssert( IsSame< TL4::Get<1>, int >);
		StaticAssert( IsSame< TL4::Get<2>, float >);
		StaticAssert(( IsSame< TL4, TypeList<bool, int, float> >));

		using TL5 = TL4::PopBack::type;
		StaticAssert( TL5::Count == 2 );
		StaticAssert( IsSame< TL5::Get<0>, bool >);
		StaticAssert( IsSame< TL5::Get<1>, int >);
		StaticAssert(( IsSame< TL5, TypeList<bool, int> >));

		using TL6 = TL5::PopFront::type;
		StaticAssert( TL6::Count == 1 );
		StaticAssert( IsSame< TL6::Get<0>, int >);
		StaticAssert(( IsSame< TL6, TypeList<int> >));

		using TL7 = TL6::PopFront::type;
		StaticAssert( TL7::Count == 0 );
	}
	{
		using TL = TypeList< int, float, bool, double >;

		constexpr bool		val1	= TL::ForEach_Or< GreaterThen4 >();
		StaticAssert( val1 );

		constexpr bool		val2	= TL::ForEach_And< GreaterThen4 >();
		StaticAssert( not val2 );

		constexpr usize		val3	= TL::ForEach_Add< TypeSize >();
		StaticAssert( val3 == 4+4+1+8 );

		constexpr usize		val4	= TL::ForEach_Max< TypeSize >();
		StaticAssert( val4 == 8 );

		constexpr usize		val5	= TL::ForEach_Min< TypeSize >();
		StaticAssert( val5 == 1 );
	}
	{
		using TL = TypeList< int, float, Pair<int, float>, Tuple<int, float>, Pair<float, int> >;

		StaticAssert( TL::FirstSpecializationOf< std::pair > == 2 );
		StaticAssert( TL::FirstSpecializationOf< Tuple > == 3 );
	}

	// EraseBack
	{
		using TL = TypeList< int, float, ulong, char >;
		using TL2 = TL::EraseBack< 0 >;
		StaticAssert( TL2::Count == 4 );
	}{
		using TL = TypeList< int, float, ulong, char >;
		using TL2 = TL::EraseBack< 1 >;
		StaticAssert( TL2::Count == 3 );
		StaticAssert( IsSame< TL2::Get<0>, int >);
		StaticAssert( IsSame< TL2::Get<1>, float >);
		StaticAssert( IsSame< TL2::Get<2>, ulong >);
	}{
		using TL = TypeList< int, float, ulong, char >;
		using TL2 = TL::EraseBack< 2 >;
		StaticAssert( TL2::Count == 2 );
		StaticAssert( IsSame< TL2::Get<0>, int >);
		StaticAssert( IsSame< TL2::Get<1>, float >);
	}{
		using TL = TypeList< int, float, ulong, char >;
		using TL2 = TL::EraseBack< 3 >;
		StaticAssert( TL2::Count == 1 );
		StaticAssert( IsSame< TL2::Get<0>, int >);
	}
	#if 0
	{
		using TL = TypeList< int, float, ulong, char >;
		using TL2 = TL::EraseBack< 4 >;		// error
		StaticAssert( TL2::Count == 0 );
	}
	#endif

	// EraseFront
	{
		using TL = TypeList< ulong, char, int, float >;
		using TL2 = TL::EraseFront< 0 >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert( IsSame< TL2::Get<0>, ulong >);
		StaticAssert( IsSame< TL2::Get<1>, char >);
		StaticAssert( IsSame< TL2::Get<2>, int >);
		StaticAssert( IsSame< TL2::Get<3>, float >);
	}{
		using TL = TypeList< ulong, char, int, float >;
		using TL2 = TL::EraseFront< 1 >;
		StaticAssert( TL2::Count == 3 );
		StaticAssert( IsSame< TL2::Get<0>, char >);
		StaticAssert( IsSame< TL2::Get<1>, int >);
		StaticAssert( IsSame< TL2::Get<2>, float >);
	}{
		using TL = TypeList< ulong, char, int, float >;
		using TL2 = TL::EraseFront< 2 >;
		StaticAssert( TL2::Count == 2 );
		StaticAssert( IsSame< TL2::Get<0>, int >);
		StaticAssert( IsSame< TL2::Get<1>, float >);
	}{
		using TL = TypeList< ulong, char, int, float >;
		using TL2 = TL::EraseFront< 3 >;
		StaticAssert( TL2::Count == 1 );
		StaticAssert( IsSame< TL2::Get<0>, float >);
	}
	#if 0
	{
		using TL = TypeList< ulong, char, int, float >;
		using TL2 = TL::EraseFront< 4 >;	// error
		StaticAssert( TL2::Count == 0 );
	}
	#endif

	// Erase
	{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::Erase< 0 >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< char, bool, int, float > >));
	}{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::Erase< 1 >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< ulong, bool, int, float > >));
	}{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::Erase< 2 >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< ulong, char, int, float > >));
	}{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::Erase< 3 >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< ulong, char, bool, float > >));
	}{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::Erase< 4 >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< ulong, char, bool, int > >));
	}
	#if 0
	{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::Erase< 5 >;		// error
	}
	#endif

	// EraseType
	{
		using TL = TypeList< ulong, char, bool, int, float >;
		using TL2 = TL::EraseType< bool >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< ulong, char, int, float > >));
	}{
		using TL = TypeList< bool, ulong, char, bool, int, float, bool >;
		using TL2 = TL::EraseType< bool >;
		StaticAssert( TL2::Count == 4 );
		StaticAssert(( IsSame< TL2, TypeList< ulong, char, int, float > >));
	}

	// TypeListFill
	{
		using TL1 = TypeListFill< int, 4 >;
		using TL2 = TypeList< int, int, int, int >;
		StaticAssert( IsSame< TL1, TL2 >);
	}

	TEST_PASSED();
}
