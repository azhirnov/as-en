// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/CompileTime/TypeList.h"
#include "UnitTest_Common.h"

namespace
{
    struct TL_Visitor 
    {
        usize sizeof_sum = 0;

        template <typename T, usize I>
        void operator () () {
            sizeof_sum += sizeof(T);
        }
    };

    template <typename T>
    struct GreaterThen4 {
        static constexpr bool   value = sizeof(T) > 4;
    };

    template <typename T>
    struct TypeSize {
        static constexpr usize  value = sizeof(T);
    };
}


extern void UnitTest_TypeList ()
{
    {
        STATIC_ASSERT(( TypeList<int, bool, float>::Count == 3 ));
        STATIC_ASSERT(( TypeList<int, int, int>::Count == 3 ));
    }
    {
        using TL = TypeList< int, float, bool, double >;
        STATIC_ASSERT( IsTypeList< TL > );
        STATIC_ASSERT( not IsTypeList< int > );
    }
    {
        using TL = TypeList< int, float, bool, double >;
        STATIC_ASSERT( IsSameTypes< TL::Get<0>, int > );
        STATIC_ASSERT( IsSameTypes< TL::Get<1>, float > );
        STATIC_ASSERT( IsSameTypes< TL::Get<2>, bool > );
        STATIC_ASSERT( IsSameTypes< TL::Get<3>, double > );
        STATIC_ASSERT( TL::Index<int> == 0 );
        STATIC_ASSERT( TL::Index<float> == 1 );
        STATIC_ASSERT( TL::Index<bool> == 2 );
        STATIC_ASSERT( TL::Index<double> == 3 );
        STATIC_ASSERT( TL::Index<slong> == UMax );
    }
    {
        using TL = TypeList< int, bool, int, float >;
        STATIC_ASSERT( TL::FirstIndex<int> == 0 );
        STATIC_ASSERT( TL::FirstIndex<bool> == 1 );
        STATIC_ASSERT( TL::FirstIndex<float> == 3 );
        STATIC_ASSERT( TL::FirstIndex<double> == UMax );
        STATIC_ASSERT( TL::LastIndex<int> == 2 );
        STATIC_ASSERT( TL::LastIndex<bool> == 1 );
        STATIC_ASSERT( TL::LastIndex<float> == 3 );
        STATIC_ASSERT( TL::LastIndex<double> == UMax );
    }
    {
        using TL = TypeList< int, float, bool, double >;
        TL_Visitor v;
        TL::Visit( v );
        TEST( v.sizeof_sum == (sizeof(int) + sizeof(float) + sizeof(bool) + sizeof(double)) );
    }
    {
        using TL = TypeList<>;
        STATIC_ASSERT( TL::Count == 0 );

        using TL2 = TL::PushBack<int>;
        STATIC_ASSERT( TL2::Count == 1 );
        STATIC_ASSERT( IsSameTypes< TL2::Get<0>, int >);
        STATIC_ASSERT( IsSameTypes< TL2, TypeList<int> >);

        using TL3 = TL2::PushBack<float>;
        STATIC_ASSERT( TL3::Count == 2 );
        STATIC_ASSERT( IsSameTypes< TL3::Get<0>, int >);
        STATIC_ASSERT( IsSameTypes< TL3::Get<1>, float >);
        STATIC_ASSERT(( IsSameTypes< TL3, TypeList<int, float> >));

        using TL4 = TL3::PushFront<bool>;
        STATIC_ASSERT( TL4::Count == 3 );
        STATIC_ASSERT( IsSameTypes< TL4::Get<0>, bool >);
        STATIC_ASSERT( IsSameTypes< TL4::Get<1>, int >);
        STATIC_ASSERT( IsSameTypes< TL4::Get<2>, float >);
        STATIC_ASSERT(( IsSameTypes< TL4, TypeList<bool, int, float> >));

        using TL5 = TL4::PopBack::type;
        STATIC_ASSERT( TL5::Count == 2 );
        STATIC_ASSERT( IsSameTypes< TL5::Get<0>, bool >);
        STATIC_ASSERT( IsSameTypes< TL5::Get<1>, int >);
        STATIC_ASSERT(( IsSameTypes< TL5, TypeList<bool, int> >));

        using TL6 = TL5::PopFront::type;
        STATIC_ASSERT( TL6::Count == 1 );
        STATIC_ASSERT( IsSameTypes< TL6::Get<0>, int >);
        STATIC_ASSERT(( IsSameTypes< TL6, TypeList<int> >));
    }
    {
        using TL = TypeList< int, float, bool, double >;

        constexpr bool      val1    = TL::ForEach_Or< GreaterThen4 >();
        STATIC_ASSERT( val1 );

        constexpr bool      val2    = TL::ForEach_And< GreaterThen4 >();
        STATIC_ASSERT( not val2 );

        constexpr usize val3    = TL::ForEach_Add< TypeSize >();
        STATIC_ASSERT( val3 == 4+4+1+8 );

        constexpr usize val4    = TL::ForEach_Max< TypeSize >();
        STATIC_ASSERT( val4 == 8 );

        constexpr usize val5    = TL::ForEach_Min< TypeSize >();
        STATIC_ASSERT( val5 == 1 );
    }
    {
        using TL = TypeList< int, float, Pair<int, float>, Tuple<int, float>, Pair<float, int> >;

        STATIC_ASSERT( TL::FirstSpecializationOf< std::pair > == 2 );
        STATIC_ASSERT( TL::FirstSpecializationOf< Tuple > == 3 );
    }

    TEST_PASSED();
}
