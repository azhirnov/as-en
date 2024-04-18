// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ecs-st/Core/Registry.h"
#include "UnitTest_Common.h"

namespace
{
	using Index_t = ArchetypeStorage::Index_t;

	struct Comp1
	{
		int		value;
	};

	struct Comp2
	{
		float	value;
	};

	struct Tag1 {};
	struct Tag2 {};


	static void  RegisterComponents_Test1 ()
	{
		Registry	reg;
		reg.RegisterComponents< Comp1, Comp2, Tag1, Tag2 >();

		auto	comp1 = reg.GetComponentInfo( ComponentTypeInfo<Comp1>::id );
		auto	comp2 = reg.GetComponentInfo( ComponentTypeInfo<Comp2>::id );
		auto	tag1  = reg.GetComponentInfo( ComponentTypeInfo<Tag1>::id );
		auto	tag2  = reg.GetComponentInfo( ComponentTypeInfo<Tag2>::id );

		TEST( comp1 );
		TEST( comp1->size  == sizeof(Comp1) );
		TEST( comp1->align == alignof(Comp1) );
		TEST( comp1->ctor  != null );

		TEST( comp2 );
		TEST( comp2->size  == sizeof(Comp2) );
		TEST( comp2->align == alignof(Comp2) );
		TEST( comp2->ctor  != null );

		TEST( tag1 );
		TEST( tag1->size  == 0 );
		TEST( tag1->align == 0 );
		TEST( tag1->ctor  != null );

		TEST( tag2 );
		TEST( tag2->size  == 0 );
		TEST( tag2->align == 0 );
		TEST( tag2->ctor  != null );
	}


	static void  ArchetypeStorage_Test1 ()
	{
		ArchetypeDesc		desc;
		desc.Add<Comp1>();
		desc.Add<Comp2>();

		Registry			reg;
		reg.RegisterComponents< Comp1, Comp2, Tag1, Tag2 >();

		Archetype			arch{ desc };
		ArchetypeStorage	storage{ reg, arch, 1024 };
		EntityPool			pool;

		EntityID	id_1, id_2;
		TEST( pool.Assign( OUT id_1 ));
		TEST( pool.Assign( OUT id_2 ));

		Index_t		index_1, index_2;
		TEST( storage.Add( id_1, OUT index_1 ));
		TEST( storage.Add( id_2, OUT index_2 ));

		TEST( storage.IsValid( id_1, index_1 ));
		TEST( storage.IsValid( id_2, index_2 ));
		TEST( not storage.IsValid( id_2, index_1 ));
		TEST( not storage.IsValid( id_1, index_2 ));

		EntityID	m1, m2;
		TEST( storage.Erase( index_2, OUT m2 ));	// TODO: this is incorrect test
		TEST( storage.Erase( index_1, OUT m1 ));

		TEST( pool.Unassign( id_1 ));
		TEST( pool.Unassign( id_2 ));
	}


	static void  ArchetypeDesc_Test1 ()
	{
		ArchetypeDesc	a1;
		a1.Add<Comp1>();
		a1.Add<Comp2>();
		a1.Add<Tag1>();

		ArchetypeDesc	a2;
		a2.Add<Comp2>();
		a2.Add<Tag2>();

		ArchetypeDesc	a3;
		a3.Add<Tag1>();
		a3.Add<Tag2>();

		ArchetypeDesc	a4;
		a4.Add<Tag1>();
		a4.Add<Comp1>();
		a4.Add<Comp2>();
		a4.Add<Tag2>();
		a4.Add<Comp1>();

		ArchetypeDesc	a5;
		a5.Add<Tag1>();

		ArchetypeDesc	a6;

		// a1
		{
			TEST( a1.Equal( a1 ));
			TEST( not a1.Equal( a2 ));
			TEST( not a1.Equal( a3 ));
			TEST( not a1.Equal( a4 ));
			TEST( not a1.Equal( a5 ));

			TEST( a1.Any( a2 ));
			TEST( a1.Any( a3 ));
			TEST( a1.Any( a4 ));
			TEST( a1.Any( a5 ));

			TEST( not a1.All( a2 ));
			TEST( not a1.All( a3 ));
			TEST( not a1.All( a4 ));
			TEST( a1.All( a5 ));
		}

		// a2
		{
			TEST( not a2.Equal( a1 ));
			TEST( a2.Equal( a2 ));
			TEST( not a2.Equal( a3 ));
			TEST( not a2.Equal( a4 ));
			TEST( not a2.Equal( a5 ));

			TEST( not a2.All( a3 ));
			TEST( not a2.All( a4 ));
			TEST( not a2.All( a5 ));

			TEST( a2.Any( a3 ));
			TEST( a2.Any( a4 ));
			TEST( not a2.Any( a5 ));
		}

		// a3
		{
			TEST( not a3.Equal( a1 ));
			TEST( not a3.Equal( a2 ));
			TEST( a3.Equal( a3 ));
			TEST( not a3.Equal( a4 ));
			TEST( not a3.Equal( a5 ));

			TEST( not a3.All( a4 ));
			TEST( a3.All( a5 ));

			TEST( a3.Any( a4 ));
			TEST( a3.Any( a5 ));
		}

		// a4
		{
			TEST( not a4.Equal( a1 ));
			TEST( not a4.Equal( a2 ));
			TEST( not a4.Equal( a3 ));
			TEST( a4.Equal( a4 ));
			TEST( not a4.Equal( a5 ));

			TEST( not a4.Equal( a1 ));
			TEST( a4.All( a1 ));
			TEST( a4.Any( a1 ));
			TEST( a4.All( a5 ));

			a4.Remove<Tag2>();
			TEST( a4.Equal( a1 ));
			TEST( a1.All( a4 ));
		}

		// a6
		{
			TEST( a6.AnyOrEmpty( a1 ));
			TEST( a6.Empty() );
		}
	}


	static void  ArchetypeQuery_Test1 ()
	{
		ArchetypeDesc	a1;
		a1.Add<Comp1>();
		a1.Add<Comp2>();
		a1.Add<Tag1>();

		ArchetypeDesc	a2;
		a2.Add<Comp2>();
		a2.Add<Tag2>();

		ArchetypeDesc	a3;
		a3.Add<Tag1>();
		a3.Add<Tag2>();

		ArchetypeDesc	a4;
		a4.Add<Tag1>();
		a4.Add<Comp1>();
		a4.Add<Comp2>();
		a4.Add<Tag2>();
		a4.Add<Comp1>();

		ArchetypeDesc	a5;
		a5.Add<Tag1>();

		{
			ArchetypeQueryDesc	q;
			q.required.Add<Comp1>();
			q.required.Add<Comp2>();

			TEST( q.Compatible( a1 ));
			TEST( not q.Compatible( a2 ));
			TEST( not q.Compatible( a3 ));
			TEST( q.Compatible( a4 ));
			TEST( not q.Compatible( a5 ));
		}{
			ArchetypeQueryDesc	q;
			q.required.Add<Comp1>();
			q.requireAny.Add<Comp1>();
			q.requireAny.Add<Tag2>();

			TEST( q.Compatible( a1 ));
			TEST( not q.Compatible( a2 ));
			TEST( not q.Compatible( a3 ));
			TEST( q.Compatible( a4 ));
			TEST( not q.Compatible( a5 ));
		}{
			ArchetypeQueryDesc	q;
			q.required.Add<Tag1>();
			q.subtractive.Add<Comp1>();

			TEST( not q.Compatible( a1 ));
			TEST( not q.Compatible( a2 ));
			TEST( q.Compatible( a3 ));
			TEST( not q.Compatible( a4 ));
			TEST( q.Compatible( a5 ));
		}
	}
}


extern void UnitTest_Archetype ()
{
	RegisterComponents_Test1();
	ArchetypeStorage_Test1();
	ArchetypeDesc_Test1();
	ArchetypeQuery_Test1();

	TEST_PASSED();
}
