// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	using Obj_t = DebugInstanceCounter< int, 7 >;

	class RCObj final : public EnableRC<RCObj>, public Obj_t
	{
	public:
		RCObj ()				__NE___	: Obj_t{ 0 } {}
		explicit RCObj (int i)	__NE___	: Obj_t{ i } {}
	};

	using RC_t			= RC<RCObj>;
	using AtomicRC_t	= AtomicRC<RCObj>;


	static void  RC_Test1 ()
	{
		Obj_t::ClearStatistic();
		{
			static constinit RC_t b0;

			RC_t	a0 = MakeRC<RCObj>( 1 );

			TEST( a0 );
			TEST( a0 != null );
			TEST( not (not a0) );

			a0 = a0.get();
			TEST( a0 );

          #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL)
			a0 = a0;    // clang error: -Wself-assign-overloaded
			TEST( a0 );
          #endif

			//a0 = RVRef(a0);	// error: result will be null
			//TEST( a0 );
		}
		TEST( Obj_t::CheckStatistic() );
	}


	static void  RC_Test2 ()
	{
		struct C1 : EnableRC<C1> {};
		struct C2 : C1 {};

		StaticAssert( IsBaseOfNotSame< C1, C2 >);
		StaticAssert( not IsBaseOfNotSame< C2, C1 >);
		StaticAssert( not IsBaseOfNotSame< C1, C1 >);

		RC<C1>	a0;
		RC<C2>	a1;

		TEST( not a0 );
		TEST( a0 == null );

		a0 = a1;
	//	a1 = a0;			// error
		a1 = RC<C2>{ a0 };
		a0 = RC<C1>{ a1 };
	}


	static void  AtomicRC_Test1 ()
	{
		Obj_t::ClearStatistic();
		{
			const RC_t	a0 = MakeRC<RCObj>( 1 );
			const RC_t	a1 = MakeRC<RCObj>( 2 );

			AtomicRC_t	b0;
			RC_t		b1;

			b0.store( a0 );

			TEST( a0.use_count() == 2 );
			TEST( a1.use_count() == 1 );

			TEST( not b0.CAS_Loop( INOUT b1, a1 ));
			TEST( b1 == a0 );
			TEST( a0.use_count() == 3 );	// b0, b1
			TEST( a1.use_count() == 1 );

			TEST( b0.CAS_Loop( INOUT b1, a1 ));
			TEST( b1 == a0 );
			TEST( b0.unsafe_get() == a1.get() );

			TEST( a0.use_count() == 2 );	// b1
			TEST( a1.use_count() == 2 );	// b0

			b0.reset();
			b1.reset( null );
			TEST( a0.use_count() == 1 );
			TEST( a1.use_count() == 1 );
		}
		TEST( Obj_t::CheckStatistic() );
	}


	static void  PackedRC_Test1 ()
	{
		Obj_t::ClearStatistic();
		{
			PackedRC<RCObj, 4>	a0 {new RCObj{ 1 }};
			RC_t				b0 = MakeRC<RCObj>( 1 );

			TEST( a0 );
			TEST( a0 != null );
			TEST( not (not a0) );
			TEST( a0.Extra() == 0 );

			a0.SetExtra( 2 );

			a0 = a0.get();
			TEST( a0 );
			TEST( a0.Extra() == 0 );

			a0.SetExtra( 3 );

			a0 = RVRef(b0);
			TEST( a0.Extra() == 0 );
		}
		TEST( Obj_t::CheckStatistic() );
	}


	static void  PackedRC_Test2 ()
	{
		struct C1 : EnableRC<C1> {};
		struct C2 : C1 {};

		StaticAssert( IsBaseOfNotSame< C1, C2 >);
		StaticAssert( not IsBaseOfNotSame< C2, C1 >);
		StaticAssert( not IsBaseOfNotSame< C1, C1 >);

		PackedRC<C1,4>	a0;
		PackedRC<C2,4>	a1;

		TEST( not a0 );
		TEST( a0 == null );

		a0 = a1;
	//	a1 = a0;			// error
		a1 = RC<C2>{ a1.get() };
		a0 = RC<C1>{ a0.get() };

		RC<C1> b0 = RC<C1>{ a0 };
		RC<C1> b1 = RC<C1>{ a1 };
		RC<C2> b2 = RC<C2>{ a0 };
	}
}


extern void UnitTest_RC ()
{
	RC_Test1();
	RC_Test2();

	AtomicRC_Test1();

	PackedRC_Test1();
	PackedRC_Test2();

	TEST_PASSED();
}
