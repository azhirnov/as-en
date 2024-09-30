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

	struct Event1 {};
	struct Event2 {};
	struct Event3 {};

	StaticAssert( not IsEmpty<Comp1> );
	StaticAssert( not IsEmpty<Comp2> );
	StaticAssert( IsEmpty<Tag1> );


	template <typename ...Args>
	static QueryID  CreateQuery1 (Registry &reg, TypeList<Args...> const*)
	{
		return reg.CreateQuery< Args... >();
	}

	template <typename ...Args>
	static QueryID  CreateQuery2 (Registry &reg, ArrayView<Tuple<Args...>> const*)
	{
		using A = TypeList< Args... >;
		StaticAssert(( IsSameTypes< typename A::template Get<0>, usize > ));
		using B = typename A::PopFront::type;

		return CreateQuery1( reg, Cast< B const >(null) );
	}

	template <typename Fn>
	static void  EnqueueWithoutQuery (Registry &reg, Fn &&fn)
	{
		QueryID	q = CreateQuery2( reg, Cast< typename FunctionInfo<Fn>::args::template Get<0> >(null) );

		reg.Enqueue( q, FwdArg<Fn>(fn) );
	}


	static void  InitRegistry (Registry &reg)
	{
		reg.RegisterComponents< Comp1, Comp2, Tag1 >();
	}


	static void  ComponentValidator_Test1 ()
	{
	#ifdef AE_ECS_VALIDATE_SYSTEM_FN
		{
			using Types = TypeList< WriteAccess<Comp1>, ReadAccess<Comp2> >;
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<0> >::Test< 0, Types >() );
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<1> >::Test< 1, Types >() );
		}{
			using Types = TypeList< WriteAccess<Comp1>, WriteAccess<Comp2>, ReadAccess<Comp2> >;
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<0> >::Test< 0, Types >() );
			StaticAssert( not _reg_detail_::CheckForDuplicateComponents< Types::Get<1> >::Test< 1, Types >() );
			StaticAssert( not _reg_detail_::CheckForDuplicateComponents< Types::Get<2> >::Test< 2, Types >() );
		}{
			using Types = TypeList< WriteAccess<Comp1>, OptionalReadAccess<Comp2>, ReadAccess<Comp2> >;
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<0> >::Test< 0, Types >() );
			StaticAssert( not _reg_detail_::CheckForDuplicateComponents< Types::Get<1> >::Test< 1, Types >() );
			StaticAssert( not _reg_detail_::CheckForDuplicateComponents< Types::Get<2> >::Test< 2, Types >() );
		}{
			using Types = TypeList< WriteAccess<Comp1>, Require<Comp2, Tag1> >;
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<0> >::Test< 0, Types >() );
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<1> >::Test< 1, Types >() );
		}{
			using Types = TypeList< WriteAccess<Comp1>, Require<Comp1, Comp2, Tag1> >;
			StaticAssert( not _reg_detail_::CheckForDuplicateComponents< Types::Get<0> >::Test< 0, Types >() );
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<1> >::Test< 1, Types >() );
		}{
			using Types = TypeList< Comp1*, Comp2* >;
			StaticAssert( _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<0> >::Test< Types, 0 >() );
			StaticAssert( _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<1> >::Test< Types, 1 >() );
		}{
			using Types = TypeList< Comp1*, Comp2& >;
			StaticAssert( _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<0> >::Test< Types, 0 >() );
			StaticAssert( _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<1> >::Test< Types, 1 >() );
		}{
			using Types = TypeList< Comp1 const*, Comp2* >;
			StaticAssert( _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<0> >::Test< Types, 0 >() );
			StaticAssert( _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<1> >::Test< Types, 1 >() );
		}{
			using Types = TypeList< Comp1*, Comp1& >;
			StaticAssert( not _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<0> >::Test< Types, 0 >() );
			StaticAssert( not _reg_detail_::SC_CheckForDuplicateComponents< Types::Get<1> >::Test< Types, 1 >() );
		}{
			using Types = TypeList< Require<Comp1>, Subtractive<Comp2> >;
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<0> >::Test< 0, Types >() );
			StaticAssert( _reg_detail_::CheckForDuplicateComponents< Types::Get<1> >::Test< 1, Types >() );
		}
	#endif // AE_ECS_VALIDATE_SYSTEM_FN
	}


	static void  Entity_Test1 ()
	{
		Registry	reg;
		InitRegistry( reg );

		// create
		EntityID	e1 = reg.CreateEntity<Comp1, Comp2>();
		TEST( e1 );
		{
			auto	c11 = reg.GetComponent<Comp1>( e1 );
			auto	c12 = reg.GetComponent<Comp2>( e1 );
			TEST( c11 );
			TEST( c12 );

			*c11 = Comp1{ 0x1234 };
			*c12 = Comp2{ 1.23f };
		}

		EntityID	t0 = reg.CreateEntity<Comp1, Comp2>();
		TEST( t0 );

		EntityID	t1 = reg.CreateEntity<Comp1, Comp2>();
		TEST( t1 );

		TEST( reg.DestroyEntity( t0 ));

		EntityID	e2 = reg.CreateEntity<Comp1, Comp2>();
		TEST( e2 );
		{
			auto	c21 = reg.GetComponent<Comp1>( e2 );
			auto	c22 = reg.GetComponent<Comp2>( e2 );
			TEST( c21 );
			TEST( c22 );

			*c21 = Comp1{ 0x111 };
			*c22 = Comp2{ 9.4f };

			reg.AssignComponent<Comp1>( e2 );
		}

		EntityID	t2 = reg.CreateEntity<Comp1>();
		TEST( t2 );

		EntityID	e3 = reg.CreateEntity();
		TEST( e3 );
		{
			auto&	c31 = reg.AssignComponent<Comp1>( e3 );
			c31.value = 0x777;
		}

		EntityID	e4 = reg.CreateEntity();
		TEST( e4 );
		{
			auto&	c42 = reg.AssignComponent<Comp2>( e4 );
			c42.value = 0.88f;

			TEST( not reg.RemoveComponent<Comp1>( e4 ));
		}

		EntityID	e5 = reg.CreateEntity<Comp1, Comp2>();
		TEST( e5 );
		{
			auto	c51 = reg.GetComponent<Comp1>( e5 );
			auto	c52 = reg.GetComponent<Comp2>( e5 );
			TEST( c51 );
			TEST( c52 );

			*c51 = Comp1{ 0x222 };
			*c52 = Comp2{ 5.3f };

			TEST( reg.RemoveComponent<Comp2>( e5 ));
		}

		TEST( reg.DestroyEntity( t1 ));
		TEST( reg.DestroyEntity( t2 ));


		// check
		{
			TEST( reg.GetArchetype( e1 ) == reg.GetArchetype( e2 ));
			TEST( reg.GetArchetype( e1 ) != reg.GetArchetype( e3 ));
			TEST( reg.GetArchetype( e1 ) != reg.GetArchetype( e4 ));
			TEST( reg.GetArchetype( e3 ) != reg.GetArchetype( e4 ));
			TEST( reg.GetArchetype( e3 ) == reg.GetArchetype( e5 ));
		}
		{
			auto	c11 = reg.GetComponent<Comp1>( e1 );
			auto	c12 = reg.GetComponent<Comp2>( e1 );
			TEST( c11 );
			TEST( c12 );
			TEST( c11->value == 0x1234 );
			TEST( c12->value == 1.23f );
		}
		{
			auto	c21 = reg.GetComponent<Comp1>( e2 );
			auto	c22 = reg.GetComponent<Comp2>( e2 );
			TEST( c21 );
			TEST( c22 );
			TEST( c21->value == 0x111 );
			TEST( c22->value == 9.4f );
		}
		{
			auto	c31 = reg.GetComponent<Comp1>( e3 );
			TEST( c31 );
			TEST( c31->value == 0x777 );
			TEST( reg.GetComponent<Comp2>( e3 ) == null );
		}
		{
			auto	c42 = reg.GetComponent<Comp2>( e4 );
			TEST( c42 );
			TEST( c42->value == 0.88f );
			TEST( reg.GetComponent<Comp1>( e4 ) == null );
		}
		{
			auto	c51 = reg.GetComponent<Comp1>( e5 );
			TEST( c51 );
			TEST( c51->value == 0x222 );
			TEST( reg.GetComponent<Comp2>( e5 ) == null );
		}


		// destroy
		TEST( reg.DestroyEntity( e1 ));
		TEST( reg.DestroyEntity( e2 ));
		TEST( reg.DestroyEntity( e3 ));
		TEST( reg.DestroyEntity( e4 ));
		TEST( reg.DestroyEntity( e5 ));
	}


	static void  Entity_Test2 ()
	{
		Registry	reg;
		InitRegistry( reg );

		// create
		EntityID	e1 = reg.CreateEntity<Comp1, Comp2>();
		TEST( e1 );
		{
			auto	c11 = reg.GetComponent<Comp1>( e1 );
			auto	c12 = reg.GetComponent<Comp2>( e1 );
			TEST( c11 );
			TEST( c12 );
			*c11 = Comp1{ 0x1234 };
			*c12 = Comp2{ 1.23f };
		}

		EntityID	e2 = reg.CreateEntity<Comp1, Comp2>();
		TEST( e2 );
		{
			auto	c11 = reg.GetComponent<Comp1>( e2 );
			auto	c12 = reg.GetComponent<Comp2>( e2 );
			TEST( c11 );
			TEST( c12 );
			*c11 = Comp1{ 0x865 };
			*c12 = Comp2{ 3.82f };
		}

		EntityID	e3 = reg.CreateEntity<Comp1>();
		TEST( e3 );
		{
			auto	c11 = reg.GetComponent<Comp1>( e3 );
			TEST( c11 );
			*c11 = Comp1{ 0x44 };
		}

		EntityID	e4 = reg.CreateEntity<Comp1>();
		TEST( e4 );
		{
			auto	c11 = reg.GetComponent<Comp1>( e4 );
			TEST( c11 );
			*c11 = Comp1{ 0x76 };
		}

		QueryID	query = reg.CreateQuery< Require<Comp1, Comp2> >();

		reg.RemoveComponents<Comp2>( query );


		// check
		{
			TEST( reg.GetArchetype( e1 ) == reg.GetArchetype( e2 ));
			TEST( reg.GetArchetype( e1 ) == reg.GetArchetype( e3 ));
			TEST( reg.GetArchetype( e1 ) == reg.GetArchetype( e4 ));
		}
		{
			auto	c11 = reg.GetComponent<Comp1>( e1 );
			auto	c12 = reg.GetComponent<Comp2>( e1 );
			TEST( c11 );
			TEST( not c12 );
			TEST( c11->value == 0x1234 );
		}
		{
			auto	c21 = reg.GetComponent<Comp1>( e2 );
			auto	c22 = reg.GetComponent<Comp2>( e2 );
			TEST( c21 );
			TEST( not c22 );
			TEST( c21->value == 0x865 );
		}
		{
			auto	c31 = reg.GetComponent<Comp1>( e3 );
			TEST( c31 );
			TEST( c31->value == 0x44 );
		}
		{
			auto	c41 = reg.GetComponent<Comp1>( e4 );
			TEST( c41 );
			TEST( c41->value == 0x76 );
		}

		reg.DestroyAllEntities();
	}


	static void  SingleComponent_Test1 ()
	{
		Registry	reg;
		InitRegistry( reg );

		TEST( not reg.RemoveSingleComponent<Comp1>() );
		TEST( reg.GetSingleComponent<Comp2>() == null );

		auto&	s1 = reg.AssignSingleComponent<Comp1>();
		s1.value = 0x8899;

		auto&	s2 = reg.AssignSingleComponent<Comp2>();
		s2.value = 6.32f;

		auto&	s3 = reg.AssignSingleComponent<Comp1>();
		TEST( s3.value == 0x8899 );

		auto	s4 = reg.GetSingleComponent<Comp2>();
		TEST( s4 );
		TEST( s4->value == 6.32f );

		TEST( reg.RemoveSingleComponent<Comp2>() );
		TEST( reg.RemoveSingleComponent<Comp1>() );
	}


	static void  System_Test1 ()
	{
		Registry	reg;
		const usize	count = 100;

		InitRegistry( reg );

		for (usize i = 0; i < count; ++i)
		{
			EntityID	e1 = reg.CreateEntity<Comp1, Comp2>();
			TEST( e1 );
		}

		QueryID	q = reg.CreateQuery< Require<Comp1, Comp2> >();

		usize	cnt1 = 0;
		reg.Enqueue( q,
			[&cnt1] (ArrayView<Tuple< usize, WriteAccess<Comp1>, ReadAccess<Comp2> >> chunks) __NE___
			{
				for (auto& chunk : chunks)
				{
					chunk.Apply(
						[&cnt1] (const usize cnt, WriteAccess<Comp1> comp1, ReadAccess<Comp2> comp2) __NE___
						{
							for (usize i = 0; i < cnt; ++i) {
								comp1[i].value = int(comp2[i].value);
								++cnt1;
							}
						});
				}
			});

		usize	cnt2 = 0;
		reg.Enqueue( q,
			[&cnt2] (Comp1 &comp1, const Comp2 &comp2) __NE___
			{
				comp1.value = int(comp2.value);
				++cnt2;
			});

		reg.Process();

		TEST( cnt1 == count );
		TEST( cnt2 == count );

		reg.DestroyAllEntities();
	}


	static void  System_Test2 ()
	{
		struct SingleComp1
		{
			usize	sum;
		};

		Registry	reg;
		const usize	count = 100;

		InitRegistry( reg );

		// create archetypes
		for (usize i = 0; i < count; ++i)
		{
			EntityID	e1 = reg.CreateEntity<Comp1, Comp2, Tag1>();
			EntityID	e2 = reg.CreateEntity<Comp1>();
			EntityID	e3 = reg.CreateEntity<Comp2, Tag1>();
			EntityID	e4 = reg.CreateEntity<Comp2, Comp1>();
			EntityID	e5 = reg.CreateEntity<Comp1, Tag1>();

			TEST( e1 and e2 and e3 and e4 and e5 );

			TEST( reg.GetArchetype( e1 ) != reg.GetArchetype( e2 ));
			TEST( reg.GetArchetype( e1 ) != reg.GetArchetype( e3 ));
			TEST( reg.GetArchetype( e1 ) != reg.GetArchetype( e4 ));
			TEST( reg.GetArchetype( e1 ) != reg.GetArchetype( e5 ));

			TEST( reg.GetArchetype( e2 ) != reg.GetArchetype( e3 ));
			TEST( reg.GetArchetype( e2 ) != reg.GetArchetype( e4 ));
			TEST( reg.GetArchetype( e2 ) != reg.GetArchetype( e5 ));

			TEST( reg.GetArchetype( e3 ) != reg.GetArchetype( e4 ));
			TEST( reg.GetArchetype( e3 ) != reg.GetArchetype( e5 ));

			TEST( reg.GetArchetype( e4 ) != reg.GetArchetype( e5 ));
		}

		// init single component
		{
			auto&	sc1 = reg.AssignSingleComponent<SingleComp1>();
			sc1.sum = 0;
		}

		usize	cnt1 = 0;
		EnqueueWithoutQuery( reg,
			[&cnt1] (ArrayView<Tuple< usize, ReadAccess<Comp1>, Subtractive<Tag1> >> chunks, Tuple< SingleComp1& > single) __NE___
			{
				usize&	sum = single.Get<0>().sum;

				for (auto& chunk : chunks)
				{
					for (usize i = 0; i < chunk.Get<0>(); ++i) {
						sum += chunk.Get<1>()[i].value;
						++cnt1;
					}
				}
			});

		usize	cnt2 = 0;
		EnqueueWithoutQuery( reg,
			[&cnt2] (ArrayView<Tuple< usize, ReadAccess<Comp2>, Require<Tag1, Comp1> >> chunks) __NE___
			{
				for (auto& chunk : chunks)
				{
					for (usize i = 0; i < chunk.Get<0>(); ++i) {
						void( chunk.Get<1>()[i].value );
						++cnt2;
					}
				}
			});

		reg.Process();

		TEST( cnt1 == count*2 );
		TEST( cnt2 == count );

		reg.DestroyAllEntities();
		reg.DestroyAllSingleComponents();
	}


	static void  System_Test3 ()
	{
		Registry	reg;

		InitRegistry( reg );

		// create archetypes
		{
			EntityID	e1 = reg.CreateEntity<Comp1, Comp2, Tag1>();
			EntityID	e2 = reg.CreateEntity<Comp1>();
			EntityID	e3 = reg.CreateEntity<Comp2, Tag1>();
			EntityID	e4 = reg.CreateEntity<Comp2, Comp1>();
			EntityID	e5 = reg.CreateEntity<Comp1, Tag1>();
			TEST( e1 and e2 and e3 and e4 and e5 );
		}

		Array<uint>	arr;
		EnqueueWithoutQuery( reg,
			[&reg, &arr] (ArrayView<Tuple< usize, ReadAccess<Comp1> >>) __NE___
			{
				arr.push_back( 1 );

				EnqueueWithoutQuery( reg,
					[&arr] (ArrayView<Tuple< usize, WriteAccess<Comp1>, ReadAccess<Comp2> >>) __NE___
					{
						arr.push_back( 2 );
					});
			});

		EnqueueWithoutQuery( reg,
			[&arr] (ArrayView<Tuple< usize, ReadAccess<Comp2> >>) __NE___
			{
				arr.push_back( 3 );
			});

		reg.Process();

		TEST( arr == Array<uint>{1, 2, 3} );

		reg.DestroyAllEntities();
	}


	static void  Events_Test1 ()
	{
		Registry	reg;
		Array<uint>	arr;

		reg.AddEventListener<BeforeEvent<Event1>>(
			[&arr] (Registry &)
			{
				arr.push_back( 1 );
			});
		reg.AddEventListener<Event1>(
			[&arr] (Registry &registry)
			{
				arr.push_back( 2 );
				registry.EnqueueEvent<Event2>();
			});
		reg.AddEventListener<AfterEvent<Event1>>(
			[&arr] (Registry &)
			{
				arr.push_back( 3 );
			});
		reg.AddEventListener<BeforeEvent<Event2>>(
			[&arr] (Registry &registry)
			{
				arr.push_back( 4 );
				registry.EnqueueEvent<Event3>();
			});
		reg.AddEventListener<Event2>(
			[&arr] (Registry &)
			{
				arr.push_back( 5 );
			});
		reg.AddEventListener<Event3>(
			[&arr] (Registry &)
			{
				arr.push_back( 6 );
			});

		reg.EnqueueEvent<Event1>();
		reg.Process();

		TEST( arr == Array<uint>{1, 2, 3, 4, 5, 6} );
	}


	static void  Messages_Test1 ()
	{
		Registry	reg;
		const usize	count = 100;

		InitRegistry( reg );

		usize	cnt1 = 0;
		reg.AddMessageListener<Comp1, MsgTag_RemovedComponent>(
			[&cnt1] (ArrayView<EntityID> entities, ArrayView<Comp1> components)
			{
				TEST( entities.size() == components.size() );
				cnt1 += entities.size();
			});

		Array<EntityID>	entities;

		for (usize i = 0; i < count; ++i)
		{
			EntityID	e1 = reg.CreateEntity<Comp1>();
			TEST( e1 );
			entities.push_back( e1 );
		}

		reg.Process();
		TEST( cnt1 == 0 );

		for (usize i = 0; i < entities.size(); ++i)
		{
			reg.DestroyEntity( entities[i] );
		}
		entities.clear();

		reg.Process();
		TEST( cnt1 == count );
	}


	static void  Messages_Test2 ()
	{
		Registry	reg;
		const usize	count = 100;

		InitRegistry( reg );

		usize	cnt1 = 0;
		reg.AddMessageListener<Comp1, MsgTag_ComponentChanged>(
			[&cnt1] (ArrayView<EntityID> entities)
			{
				cnt1 += entities.size();
			});

		for (usize i = 0; i < count; ++i)
		{
			EntityID	e1 = reg.CreateEntity<Comp1>();
			TEST( e1 );
		}

		EnqueueWithoutQuery( reg,
			[&reg] (ArrayView<Tuple< usize, ReadAccess<Comp1>, ReadAccess<EntityID> >> chunks) __NE___
			{
				for (auto& chunk : chunks)
				{
					chunk.Apply(
						[&reg] (const usize cnt, ReadAccess<Comp1>, ReadAccess<EntityID> entities) __NE___
						{
							for (usize i = 0; i < cnt; ++i)
							{
								reg.AddMessage<MsgTag_ComponentChanged>( entities[i], ComponentTypeInfo<Comp1>::id );
							}
						});
				}
			});
		reg.Process();

		TEST( cnt1 == count );

		reg.DestroyAllEntities();
	}
}


extern void UnitTest_Registry ()
{
	ComponentValidator_Test1();

	Entity_Test1();
	Entity_Test2();

	SingleComponent_Test1();

	System_Test1();
	System_Test2();
	System_Test3();

	Events_Test1();

	Messages_Test1();
	Messages_Test2();

	TEST_PASSED();
}
