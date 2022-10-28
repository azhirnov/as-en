// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Core/ArchetypeStorage.h"
#include "ecs-st/Core/EntityPool.h"
#include "ecs-st/Core/MessageBuilder.h"
#include "ecs-st/Core/ComponentAccessTypes.h"

namespace AE::ECS
{

	//
	// System Event helpers
	//

	template <typename T>
	struct AfterEvent {};

	template <typename T>
	struct BeforeEvent {};



	//
	// Registry
	//

	class Registry final : public EnableRC< Registry >
	{
	// types
	public:
		DEBUG_ONLY(
			using IComponentDbgView	= ArchetypeStorage::IComponentDbgView;
			using CompDbgView_t		= ArchetypeStorage::CompDbgView_t;
		)

		struct ComponentInfo
		{
			using Ctor_t = void (*) (void *);

			Ctor_t		ctor		= null;
			Bytes16u	align;
			Bytes16u	size;
			bool		created		= false;
			
			DEBUG_ONLY(
				using DbgView_t	= Unique<IComponentDbgView> (*) (void *, usize);
				DbgView_t		dbgView		= null;
			)

			ND_ bool	IsTag ()	const	{ return size == 0; }
			ND_ bool	HasData ()	const	{ return size > 0; }
		};
		

	private:
		using ComponentMap_t		= Unique< StaticArray< ComponentInfo, ECS_Config::MaxComponents >>;

		using ArchetypeStoragePtr	= Unique< ArchetypeStorage >;
		using ArchetypeMap_t		= HashMap< Archetype, ArchetypeStoragePtr >;	// require pointer stability
		using Index_t				= ArchetypeStorage::Index_t;
		using ArchetypePair_t		= Pair< const Archetype, ArchetypeStoragePtr >;

		class SingleCompWrap final : public Noncopyable
		{
		public:
			void*	data			= null;
			void(*	deleter)(void*)	= null;

			SingleCompWrap () {}

			~SingleCompWrap ()
			{
				if ( data and deleter )
					deleter( data );
			}
		};

		using SingleCompMap_t	= HashMap< TypeId, SingleCompWrap >;
		using SCAllocator_t		= UntypedAllocator;

		using EventListener_t	= Function< void (Registry &) >;
		using EventListeners_t	= HashMultiMap< TypeId, EventListener_t >;
		using EventQueue_t		= Array< Function< void () >>;

		struct Query
		{
			ArchetypeQueryDesc			desc;
			Array<ArchetypePair_t *>	archetypes;
			mutable bool				locked	= true;
		};
		using Queries_t			= Array< Query >;


	// variables
	private:
		// entity + components
		EntityPool			_entities;
		ArchetypeMap_t		_archetypes;		// don't erase elements!
		MessageBuilder		_messages;
		ComponentMap_t		_componentInfo;

		// single components
		SingleCompMap_t		_singleComponents;
		//SCAllocator_t		_scAllocator;		// TODO

		EventListeners_t	_eventListeners;

		EventQueue_t		_eventQueue;
		EventQueue_t		_pendingEvents;

		Queries_t			_queries;

		DRC_ONLY(
			DataRaceCheck	_drCheck;
		)


	// methods
	public:
		Registry ();
		~Registry ();


		// entity
			template <typename ...Components>
			EntityID	CreateEntity ();
			template <typename ...Components>
			EntityID	CreateEntity (Components&& ...comps);
		ND_ EntityID	CreateEntity ();
			bool		DestroyEntity (EntityID entId);
			void		DestroyAllEntities ();

		ND_ Ptr<Archetype const>  GetArchetype (EntityID entId);

		// component
			template <typename T>
			void  RegisterComponent ();

			template <typename ...Types>
			void  RegisterComponents ();

		ND_ Ptr<ComponentInfo const>  GetComponentInfo (ComponentID compId) const;

			template <typename T>
			EnableIf< not IsEmpty<T>, T& >  AssignComponent (EntityID entId);
		
			template <typename T>
			EnableIf< IsEmpty<T>, void >  AssignComponent (EntityID entId);

			template <typename T>
			bool  RemoveComponent (EntityID entId);
			bool  RemoveComponent (EntityID entId, ComponentID compId);

			template <typename T>
		ND_ Ptr<T>  GetComponent (EntityID entId);
		
			template <typename T>
		ND_ Ptr<std::add_const_t<T>>  GetComponent (EntityID entId) const;

			template <typename ...Types>
		ND_ Tuple<Ptr<Types>...>  GetComponenets (EntityID entId);
		
			template <typename ...Types>
		ND_ Tuple<Ptr<std::add_const_t<Types>>...>  GetComponenets (EntityID entId) const;

			template <typename ...Types>
			void  RemoveComponents (QueryID query);
			void  RemoveComponents (QueryID query, const ArchetypeDesc &removeComps);

		DEBUG_ONLY(
		 ND_ CompDbgView_t  EntityDbgView (EntityID entId) const;
		)


		// single component
			template <typename T>
			T&  AssignSingleComponent ();
		
			template <typename T>
			bool  RemoveSingleComponent ();

			template <typename T>
		ND_ Ptr<T>  GetSingleComponent ();

			void DestroyAllSingleComponents ();


		// system
			template <typename ...Args>
			ND_ QueryID  CreateQuery ();
			ND_ QueryID  CreateQuery (const ArchetypeQueryDesc &desc);

			template <typename Fn>
			void  Execute (QueryID query, Fn &&fn);

			template <typename Fn>
			void  Enque (QueryID query, Fn &&fn);
			
			template <typename Obj, typename Class, typename ...Args>
			void  Enque (QueryID query, Obj obj, void (Class::*)(Args&&...));

			template <typename Ev>
			void  EnqueEvent ();

			void  Process ();

			template <typename Ev, typename Fn>
			void  AddEventListener (Fn &&fn);

			template <typename Comp, typename Tag, typename Fn>
			void  AddMessageListener (Fn &&fn);

			template <typename Tag>
			void  AddMessage (EntityID entId, ComponentID compId);
		
			template <typename Tag, typename Comp>
			void  AddMessage (EntityID entId, const Comp& comp);


	private:
			template <typename Ev>
			void  _RunEvent ();

			bool  _RemoveEntity (EntityID entId);
			void  _AddEntity (const Archetype &arch, EntityID entId, OUT ArchetypeStorage* &storage, OUT Index_t &index);
			void  _AddEntity (const Archetype &arch, EntityID entId);
			void  _MoveEntity (const Archetype &arch, EntityID entId, ArchetypeStorage* srcStorage, Index_t srcIndex,
							   OUT ArchetypeStorage* &dstStorage, OUT Index_t &dstIndex);

			void  _OnNewArchetype (ArchetypePair_t *);
			
			static void  _IncreaseStorageSize (ArchetypeStorage *, usize addCount);
			static void  _DecreaseStorageSize (ArchetypeStorage *);

			template <typename ArgsList, usize I>
		ND_ static bool  _IsArchetypeSupported (const Archetype &arch);

			template <typename ...Args>
		ND_ static Tuple<usize, Args...>  _GetChunk (ArchetypeStorage* storage, const TypeList<Args...> *);

			template <typename Fn, typename Chunk, typename ...Types>
			void _WithSingleComponents (Fn &&fn, ArrayView<Chunk> chunks, const Tuple<Types...> *);

			template <typename T>
		ND_ decltype(auto)  _GetSingleComponent ();

		
			template <typename Fn>
			void  _Execute_v1 (QueryID query, Fn &&fn);

			template <typename Fn, typename ...Args>
			void  _Execute_v2 (QueryID query, Fn &&fn, const TypeList<Args...>*);
	};
	

	
/*
=================================================
	CreateEntity
=================================================
*/
	template <typename ...Components>
	inline EntityID  Registry::CreateEntity ()
	{
		DRC_EXLOCK( _drCheck );

		EntityID		ent_id = CreateEntity();
		ArchetypeDesc	desc;
		( desc.Add<Components>(), ... );
		
		ArchetypeStorage*	storage = null;
		Index_t				index;
		
		_AddEntity( Archetype{desc}, ent_id, OUT storage, OUT index );
		ASSERT( storage );

		#if AE_ECS_ENABLE_DEFAULT_MESSAGES
			for (auto& comp_id : storage->GetComponentIDs())
			{
				_messages.Add<MsgTag_AddedComponent>( ent_id, comp_id );
			}
		#endif

		return ent_id;
	}
	
/*
=================================================
	CreateEntity
=================================================
*/
	namespace _reg_detail_
	{
		template <typename T>
		void CopyComponent (ArchetypeStorage* storage, ArchetypeStorage::Index_t index, T&& comp)
		{
			if constexpr( not IsEmpty<T> )
			{
				auto*	ptr = storage->GetComponent<T>( index );
				ASSERT( ptr );
				*ptr = FwdArg<T>( comp );
			}
		}
	} // _reg_detail_

	template <typename ...Components>
	inline EntityID  Registry::CreateEntity (Components&& ...comps)
	{
		DRC_EXLOCK( _drCheck );
		
		EntityID		ent_id = CreateEntity();
		ArchetypeDesc	desc;
		( desc.Add<RemoveCV<Components>>(), ... );
		
		ArchetypeStorage*	storage = null;
		Index_t				index;
		
		_AddEntity( Archetype{desc}, ent_id, OUT storage, OUT index );
		ASSERT( storage );
		
		(_reg_detail_::CopyComponent( storage, index, FwdArg<Components>(comps) ), ...);

		#if AE_ECS_ENABLE_DEFAULT_MESSAGES
			for (auto& comp_id : storage->GetComponentIDs())
			{
				_messages.Add<MsgTag_AddedComponent>( ent_id, comp_id );
			}
		#endif

		return ent_id;
	}

/*
=================================================
	ComponentDbgView
=================================================
*/
DEBUG_ONLY(
	namespace _reg_detail_
	{
		template <typename T>
		class ComponentDbgView final : public Registry::IComponentDbgView
		{
		private:
			ArrayView< T >			_comps;
			std::type_info const&	_type;

		public:
			ComponentDbgView (T const* ptr, usize count) : _comps{ptr, count}, _type{typeid(T)} {}
			~ComponentDbgView () override {}

			Unique<IComponentDbgView>  ElementView (usize index) const override {
				return MakeUnique<ComponentDbgView<T>>( _comps.section( index, 1 ).data(), 1u );
			}
		};

	} // _reg_detail_
)
/*
=================================================
	RegisterComponent
=================================================
*/
	template <typename T>
	inline void  Registry::RegisterComponent ()
	{
		DRC_EXLOCK( _drCheck );

		using	Info = ComponentTypeInfo<T>;
		CHECK_ERRV( Info::id.value < _componentInfo->size() );

		auto&	comp = _componentInfo->operator[]( Info::id.value );
		if ( not comp.created )
		{
			comp.size		= Info::size;
			comp.align		= Info::align;
			comp.ctor		= &Info::Ctor;
			comp.created	= true;

			DEBUG_ONLY(
			  comp.dbgView	= [] (void *ptr, usize count) -> Unique<IComponentDbgView> {
									return MakeUnique< _reg_detail_::ComponentDbgView<T> >( Cast<T>(ptr), count );
								};
			)
		}
	}
	
	template <typename ...Types>
	inline void  Registry::RegisterComponents ()
	{
		(RegisterComponent<Types>(), ...);
	}

/*
=================================================
	GetComponentInfo
=================================================
*/
	inline Ptr<Registry::ComponentInfo const>  Registry::GetComponentInfo (ComponentID compId) const
	{
		DRC_EXLOCK( _drCheck );

		if ( compId.value < _componentInfo->size() )
		{
			auto&	comp = _componentInfo->operator[]( compId.value );
			return comp.created ? &comp : null;
		}
		return null;
	}

/*
=================================================
	AssignComponent
=================================================
*/
	template <typename T>
	inline EnableIf< not IsEmpty<T>, T& >  Registry::AssignComponent (EntityID entId)
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	src_storage	= null;
		Index_t				src_index;
		ArchetypeDesc		desc;
		
		_entities.GetArchetype( entId, OUT src_storage, OUT src_index );

		if ( src_storage )
		{
			ASSERT( not src_storage->IsLocked() );

			if ( auto* comps = src_storage->GetComponents<T>(); comps )
			{
				// already exists
				return comps[ usize(src_index) ];
			}
			else
			{
				desc = src_storage->GetArchetype().Desc();
			}
		}

		desc.Add<T>();

		#if AE_ECS_ENABLE_DEFAULT_MESSAGES
			_messages.Add<MsgTag_AddedComponent>( entId, ComponentTypeInfo<T>::id );
		#endif

		ArchetypeStorage*	dst_storage	= null;
		Index_t				dst_index;
		_MoveEntity( Archetype{desc}, entId, src_storage, src_index, OUT dst_storage, OUT dst_index );
		
		T* result = dst_storage->GetComponent<T>( dst_index );

		ASSERT( dst_storage->IsInMemoryRange( result, Bytes{sizeof(T)} ));
		return *result;
	}
	
/*
=================================================
	AssignComponent
=================================================
*/
	template <typename T>
	inline EnableIf< IsEmpty<T>, void >  Registry::AssignComponent (EntityID entId)
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	src_storage	= null;
		Index_t				src_index;
		ArchetypeDesc		desc;
		
		_entities.GetArchetype( entId, OUT src_storage, OUT src_index );

		if ( src_storage )
		{
			ASSERT( not src_storage->IsLocked() );

			if ( src_storage->HasComponent<T>() )
			{
				// already exists
				return;
			}
			else
			{
				desc = src_storage->GetArchetype().Desc();
			}
		}

		desc.Add<T>();

		#if AE_ECS_ENABLE_DEFAULT_MESSAGES
			_messages.Add<MsgTag_AddedComponent>( entId, ComponentTypeInfo<T>::id );
		#endif

		ArchetypeStorage*	dst_storage	= null;
		Index_t				dst_index;
		_MoveEntity( Archetype{desc}, entId, src_storage, src_index, OUT dst_storage, OUT dst_index );
	}

/*
=================================================
	RemoveComponent
=================================================
*/
	template <typename T>
	inline bool  Registry::RemoveComponent (EntityID entId)
	{
		return RemoveComponent( entId, ComponentTypeInfo<T>::id );
	}
	
/*
=================================================
	_DecreaseStorageSize
=================================================
*/
	inline void  Registry::_DecreaseStorageSize (ArchetypeStorage *storage)
	{
		if ( storage->Count()*4 < storage->Capacity() )
		{
			storage->Reserve( Max( ECS_Config::InitialtStorageSize, storage->Count()*2 ));
		}
	}
	
/*
=================================================
	_IncreaseStorageSize
=================================================
*/
	inline void  Registry::_IncreaseStorageSize (ArchetypeStorage *storage, usize addCount)
	{
		const usize	new_size = storage->Count() + addCount;

		if ( new_size > storage->Capacity() )
		{
			storage->Reserve( Max( (new_size*3 + new_size-1) / 2, storage->Capacity()*2 ));
		}
	}

/*
=================================================
	GetComponent
=================================================
*/
	template <typename T>
	inline Ptr<T>  Registry::GetComponent (EntityID entId)
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	storage		= null;
		Index_t				index;
		
		_entities.GetArchetype( entId, OUT storage, OUT index );

		if ( storage )
		{
			if constexpr( not IsConst<T> )
			{
				ASSERT( not storage->IsLocked() );
			}
			return storage->GetComponent< RemoveConst<T> >( index );
		}
		return null;
	}
	
	template <typename T>
	inline Ptr<std::add_const_t<T>>  Registry::GetComponent (EntityID entId) const
	{
		return const_cast<Registry*>(this)->GetComponent< std::add_const_t<T> >( entId );
	}

/*
=================================================
	GetComponenets
=================================================
*/
	template <typename ...Types>
	inline Tuple<Ptr<Types>...>  Registry::GetComponenets (EntityID entId)
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	storage		= null;
		Index_t				index;
		
		_entities.GetArchetype( entId, OUT storage, OUT index );
		
		if ( storage )
		{
			if constexpr( not TypeList<Types...>::template ForEach_And< std::is_class >() )
			{
				ASSERT( not storage->IsLocked() );
			}
			return Tuple<Ptr<Types>...>{ storage->GetComponent< RemoveConst<Types> >( index )... };
		}
		return Default;
	}
	
	template <typename ...Types>
	inline Tuple<Ptr<AddConst<Types>>...>  Registry::GetComponenets (EntityID entId) const
	{
		return const_cast<Registry*>(this)->GetComponenets< AddConst<Types>... >( entId );
	}

/*
=================================================
	RemoveComponents
=================================================
*/
	template <typename ...Types>
	inline void  Registry::RemoveComponents (QueryID query)
	{
		STATIC_ASSERT( CountOf<Types...>() > 0 );

		ArchetypeDesc	desc;
		(desc.Add<Types>(), ...);

		return RemoveComponents( query, desc );
	}
	
/*
=================================================
	EntityDbgView
=================================================
*/
DEBUG_ONLY(
	inline Registry::CompDbgView_t  Registry::EntityDbgView (EntityID entId) const
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	storage	= null;
		Index_t				index;
		_entities.GetArchetype( entId, OUT storage, OUT index );

		return storage ? storage->EntityDbgView( index ) : Default;
	}
)
//-----------------------------------------------------------------------------
	
/*
=================================================
	AssignSingleComponent
=================================================
*/
	template <typename T>
	inline T&  Registry::AssignSingleComponent ()
	{
		DRC_EXLOCK( _drCheck );

		STATIC_ASSERT( not IsEmpty<T> );
		STATIC_ASSERT( std::is_standard_layout_v<T> );
		STATIC_ASSERT( std::is_trivially_copyable_v<T> );
		STATIC_ASSERT( std::is_trivially_destructible_v<T> );
		STATIC_ASSERT( std::is_nothrow_destructible_v<T> );

		auto&	comp = _singleComponents[ TypeIdOf<T>() ];
		if ( not comp.data )
		{
			comp.data	 = new T{};
			comp.deleter = [](void* ptr) { delete Cast<T>(ptr); };

			// TODO: message that single component was created ?
		}

		return *Cast<T>( comp.data );
	}
		
/*
=================================================
	RemoveSingleComponent
=================================================
*/
	template <typename T>
	inline bool  Registry::RemoveSingleComponent ()
	{
		DRC_EXLOCK( _drCheck );

		// TODO: message that single component was destroyed ?

		return _singleComponents.erase( TypeIdOf<T>() ) > 0;
	}
	
/*
=================================================
	GetSingleComponent
=================================================
*/
	template <typename T>
	inline Ptr<T>  Registry::GetSingleComponent ()
	{
		DRC_EXLOCK( _drCheck );

		auto	iter = _singleComponents.find( TypeIdOf<T>() );
		return	iter != _singleComponents.end() ?
					Cast<T>( iter->second.data ) :
					null;
	}
//-----------------------------------------------------------------------------
	
/*
=================================================
	CheckForDuplicates
=================================================
*/
# ifdef AE_ECS_VALIDATE_SYSTEM_FN
	namespace _reg_detail_
	{
		template <typename RawType, typename WrapedType>
		struct CompareSingleComponents;
		
		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, WriteAccess<RT> >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};
		
		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, ReadAccess<RT> >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};
		
		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, OptionalWriteAccess<RT> >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};
		
		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, OptionalReadAccess<RT> >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};
		
		template <typename LT, typename ...RTs>
		struct CompareSingleComponents< LT, Subtractive<RTs...> >
		{
			static constexpr bool	value = TypeList<RTs...>::template HasType<LT>;
		};
		
		template <typename LT, typename ...RTs>
		struct CompareSingleComponents< LT, Require<RTs...> >
		{
			static constexpr bool	value = TypeList<RTs...>::template HasType<LT>;
		};
		
		template <typename LT, typename ...RTs>
		struct CompareSingleComponents< LT, RequireAny<RTs...> >
		{
			static constexpr bool	value = TypeList<RTs...>::template HasType<LT>;	// TODO: allow cases with Optional<A> + RequireAny<A,B,C> ???
		};


		template <typename RawTypeList, typename WrapedType>
		struct CompareMultiComponents
		{
			static constexpr bool	value = false;
		};
		
		template <bool isTL, typename LT, typename ...RTs>
		struct CompareMultiComponents_Helper
		{
			static constexpr bool	value = TypeList<RTs...>::template HasType<LT>;
		};

		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents_Helper< true, LTs, RTs... >
		{
			static constexpr bool	value = TypeList<RTs...>::template ForEach_Or< LTs::template HasType >;
		};

		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents< LTs, Subtractive<RTs...> >
		{
			static constexpr bool	value = CompareMultiComponents_Helper< IsTypeList<LTs>, LTs, RTs... >::value;
		};
		
		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents< LTs, Require<RTs...> >
		{
			static constexpr bool	value = CompareMultiComponents_Helper< IsTypeList<LTs>, LTs, RTs... >::value;
		};
		
		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents< LTs, RequireAny<RTs...> >
		{
			static constexpr bool	value = CompareMultiComponents_Helper< IsTypeList<LTs>, LTs, RTs... >::value;
		};


		template <template <typename, typename> class Comparator,
				  typename RefType, typename ArgsList, usize ExceptIdx>
		struct CompareComponents
		{
			template <usize I>
			static constexpr bool  Cmp ()
			{
				if constexpr( I == ExceptIdx )
				{
					// skip 'I'
					return Cmp< I+1 >();
				}
				else
				if constexpr( I < ArgsList::Count )
				{
					return	Comparator< RefType, typename ArgsList::template Get<I> >::value or
							Cmp< I+1 >();
				}
				else
					return false;
			}
		};


		template <typename T>
		struct CheckForDuplicateComponents;

		template <typename T>
		struct CheckForDuplicateComponents< WriteAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};
		
		template <typename T>
		struct CheckForDuplicateComponents< ReadAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};
		
		template <typename T>
		struct CheckForDuplicateComponents< OptionalWriteAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};
		
		template <typename T>
		struct CheckForDuplicateComponents< OptionalReadAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename ...Types>
		struct CheckForDuplicateComponents< Subtractive<Types...> >
		{
			STATIC_ASSERT( CountOf<Types...>() > 0 );
			
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not (CompareComponents< CompareMultiComponents, Types, ArgsList, I >::template Cmp<0>() or ...);
			}
		};

		template <typename ...Types>
		struct CheckForDuplicateComponents< Require<Types...> >
		{
			STATIC_ASSERT( CountOf<Types...>() > 0 );
			
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not (CompareComponents< CompareMultiComponents, Types, ArgsList, I >::template Cmp<0>() or ...);
			}
		};

		template <typename ...Types>
		struct CheckForDuplicateComponents< RequireAny<Types...> >
		{
			STATIC_ASSERT( CountOf<Types...>() > 0 );
			
			template <usize I, typename ArgsList>
			static constexpr bool  Test () {
				return not (CompareComponents< CompareMultiComponents, Types, ArgsList, I >::template Cmp<0>() or ...);
			}
		};

		
		template <typename ArgsList, usize I = 0>
		static constexpr void  CheckForDuplicates ()
		{
			if constexpr( I < ArgsList::Count )
			{
				constexpr bool is_valid = CheckForDuplicateComponents< typename ArgsList::template Get<I> >::template Test< I, ArgsList >();
				STATIC_ASSERT( is_valid );

				CheckForDuplicates< ArgsList, I+1 >();
			}
		}

		
		template <typename LT, typename RT>
		struct SC_Comparator;

		template <typename LT, typename RT>
		struct SC_Comparator< LT, RT* >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};
		
		template <typename LT, typename RT>
		struct SC_Comparator< LT, RT const* >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};

		template <typename LT, typename RT>
		struct SC_Comparator< LT, RT& >
		{
			static constexpr bool	value = IsSameTypes< LT, RT >;
		};


		template <typename T>
		struct SC_CheckForDuplicateComponents;
			
		template <typename T>
		struct SC_CheckForDuplicateComponents< T* >
		{
			template <typename ArgsList, usize I>
			static constexpr bool  Test () {
				return not CompareComponents< SC_Comparator, T, ArgsList, I >::template Cmp<0>();
			}
		};
		
		template <typename T>
		struct SC_CheckForDuplicateComponents< T const* >
		{
			template <typename ArgsList, usize I>
			static constexpr bool  Test () {
				return not CompareComponents< SC_Comparator, T, ArgsList, I >::template Cmp<0>();
			}
		};
		
		template <typename T>
		struct SC_CheckForDuplicateComponents< T& >
		{
			template <typename ArgsList, usize I>
			static constexpr bool  Test () {
				return not CompareComponents< SC_Comparator, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename ArgsList, usize I = 0>
		static constexpr void  SC_CheckForDuplicates ()
		{
			if constexpr( I < ArgsList::Count )
			{
				constexpr bool is_valid = SC_CheckForDuplicateComponents< typename ArgsList::template Get<I> >::template Test< ArgsList, I >();
				STATIC_ASSERT( is_valid );

				SC_CheckForDuplicates< ArgsList, I+1 >();
			}
		}

	} // _reg_detail_
# endif // AE_ECS_VALIDATE_SYSTEM_FN
	
/*
=================================================
	SystemFnInfo
=================================================
*/
	namespace _reg_detail_
	{
		template <typename Args, usize ArgsCount>
		struct _SystemFnInfoImpl;
		
		template <typename Args>
		struct _SystemFnInfoImpl< Args, 1 >
		{
			// check components
			using ChunkArray = typename Args::template Get<0>;
			STATIC_ASSERT( IsSpecializationOf< ChunkArray, ArrayView >);

			using Chunk = typename ChunkArray::value_type;
			STATIC_ASSERT( IsSpecializationOf< Chunk, Tuple >);

			using ChunkTL = TypeList< Chunk >;
			STATIC_ASSERT( ChunkTL::Count > 1 );
			STATIC_ASSERT( IsSameTypes< typename ChunkTL::template Get<0>, usize >);

			using CompOnly = typename ChunkTL::PopFront::type;
			using SCTuple  = Tuple<>;
		};
		
		template <typename Args>
		struct _SystemFnInfoImpl< Args, 2 > : _SystemFnInfoImpl< Args, 1 >
		{
			STATIC_ASSERT( Args::Count == 2 );

			using SCTuple = typename Args::template Get<1>;
			STATIC_ASSERT( IsSpecializationOf< SCTuple, Tuple >);
		};

		template <typename Fn>
		struct SystemFnInfo
		{
			using _Args		= typename FunctionInfo<Fn>::args;
			using _Info		= _SystemFnInfoImpl< _Args, _Args::Count >;

			using Chunk		= typename _Info::Chunk;
			using CompOnly	= typename _Info::CompOnly;
			using SCTuple	= typename _Info::SCTuple;
		};

	} // _reg_detail_

/*
=================================================
	Enque
=================================================
*/
	template <typename Obj, typename Class, typename ...Args>
	inline void  Registry::Enque (QueryID query, Obj obj, void (Class::*fn)(Args&&...))
	{
		return Enque( query, [obj, fn](Args&& ...args) { return (obj->*fn)( FwdArg<Args>(args)... ); });
	}

	template <typename Fn>
	inline void  Registry::Enque (QueryID query, Fn &&fn)
	{
		DRC_EXLOCK( _drCheck );

		if constexpr( FunctionInfo<Fn>::args::Count == 0 )
		{
			_pendingEvents.push_back( FwdArg<Fn>(fn) );
		}
		else
		{
			_pendingEvents.push_back(
				[this, query, fn = FwdArg<Fn>(fn)] ()
				{
					Execute( query, RVRef(fn) );
				});
		}
	}
	
/*
=================================================
	Execute
=================================================
*/
	template <typename Fn>
	inline void  Registry::Execute (QueryID query, Fn &&fn)
	{
		using Args = typename FunctionInfo<Fn>::args;
		STATIC_ASSERT( Args::Count > 0 );

		DRC_EXLOCK( _drCheck );

		if constexpr( IsSpecializationOf< typename Args::template Get<0>, ArrayView >)
			return _Execute_v1( query, FwdArg<Fn>(fn) );
		else
			return _Execute_v2( query, FwdArg<Fn>(fn), (const Args*)null );
	}
	
/*
=================================================
	_Execute_v1
=================================================
*/
	template <typename Fn>
	inline void  Registry::_Execute_v1 (QueryID query, Fn &&fn)
	{
		using Info		= _reg_detail_::SystemFnInfo< Fn >;
		using Chunk		= typename Info::Chunk;
		using CompOnly	= typename Info::CompOnly;
		using SCTuple	= typename Info::SCTuple;
				
		#ifdef AE_ECS_VALIDATE_SYSTEM_FN
			_reg_detail_::CheckForDuplicates< CompOnly >();
			_reg_detail_::SC_CheckForDuplicates< TypeList<SCTuple> >();
		#endif

		Array<ArchetypeStorage*>	storages;
		Array<Chunk>				chunks;
		const auto&					q_data = _queries[ query.Index() ];

		CHECK( not q_data.locked );
		q_data.locked = true;
				
		for (auto* ptr : q_data.archetypes)
		{
			ASSERT( _IsArchetypeSupported< CompOnly, 0 >( ptr->first ));
			
			auto&	storage	= ptr->second;
			storage->Lock();
			storages.emplace_back( storage.get() );
			chunks.emplace_back( _GetChunk( storage.get(), (const CompOnly *)null ));
		}

		_WithSingleComponents( RVRef(fn), ArrayView<Chunk>{chunks.data(), chunks.size()}, (const SCTuple*)null );
				
		for (auto* st : storages)
		{
			st->Unlock();
		}
		
		q_data.locked = false;
	}

/*
=================================================
	_Execute_v2
=================================================
*/
	namespace _reg_detail_
	{
		template <typename T>
		struct MapCompType2;
		
		template <typename T>
		struct MapCompType2< T& > {
			using type = WriteAccess<T>;
		};
		
		template <typename T>
		struct MapCompType2< T const& > {
			using type = ReadAccess<T>;
		};
		
		template <typename T>
		struct MapCompType2< T* > {
			using type = OptionalWriteAccess<T>;
		};
		
		template <typename T>
		struct MapCompType2< T const* > {
			using type = OptionalReadAccess<T>;
		};

		template <>
		struct MapCompType2< EntityID > {
			using type = ReadAccess<EntityID>;
		};

		template <typename ...Types>
		struct MapCompType2< Require<Types...> > {
			using type = Require<Types...>;
		};
		
		template <typename ...Types>
		struct MapCompType2< Subtractive<Types...> > {
			using type = Subtractive<Types...>;
		};
		
		template <typename ...Types>
		struct MapCompType2< RequireAny<Types...> > {
			using type = RequireAny<Types...>;
		};

		template <typename T>
		using MapCompType = typename MapCompType2<T>::type;
		

		template <typename T>
		struct GetStorageElement
		{
			template <typename ChunkType>
			static decltype(auto)  Get (ChunkType &chunk, usize i) {
				return chunk.template Get< MapCompType<T> >()[i];
			}
		};
		
		template <typename T>
		struct GetStorageElement< T * >
		{
			template <typename ChunkType>
			static T*  Get (ChunkType &chunk, usize i) {
				auto&	arr = chunk.template Get< MapCompType<T*> >();
				return arr ? &arr[i] : null;
			}
		};
		
		template <typename ...Types>
		struct GetStorageElement< Require<Types...> >
		{
			template <typename ChunkType>
			static Require<Types...>  Get (ChunkType &, usize) {
				return {};
			}
		};
		
		template <typename ...Types>
		struct GetStorageElement< RequireAny<Types...> >
		{
			template <typename ChunkType>
			static RequireAny<Types...>  Get (ChunkType &, usize) {
				return {};
			}
		};
		
		template <typename ...Types>
		struct GetStorageElement< Subtractive<Types...> >
		{
			template <typename ChunkType>
			static Subtractive<Types...>  Get (ChunkType &, usize) {
				return {};
			}
		};

	} // _reg_detail_
	
/*
=================================================
	_Execute_v2
=================================================
*/
	template <typename Fn, typename ...Args>
	inline void  Registry::_Execute_v2 (QueryID query, Fn &&fn, const TypeList<Args...>*)
	{
		_Execute_v1( query,
			[&fn] (ArrayView<Tuple< usize, _reg_detail_::MapCompType<Args>... >> chunks)
			{
				for (auto& chunk : chunks)
				{
					for (usize i = 0, cnt = chunk.template Get<0>(); i < cnt; ++i)
					{
						fn( _reg_detail_::GetStorageElement<Args>::template Get( chunk, i )... );
					}
				}
			});
	}
//-----------------------------------------------------------------------------
	

#ifdef AE_DBG_OR_DEV
/*
=================================================
	ArchetypeCompatibility
=================================================
*/
	namespace _reg_detail_
	{
		template <typename T>
		struct ArchetypeCompatibility;
		
		template <>
		struct ArchetypeCompatibility< ReadAccess<EntityID> >
		{
			static bool Test (const Archetype &) {
				return true;
			}
		};

		template <typename T>
		struct ArchetypeCompatibility< WriteAccess<T> >
		{
			static bool Test (const Archetype &arch) {
				return arch.Exists<T>();
			}
		};
		
		template <typename T>
		struct ArchetypeCompatibility< ReadAccess<T> >
		{
			static bool Test (const Archetype &arch) {
				return arch.Exists<T>();
			}
		};

		template <typename T>
		struct ArchetypeCompatibility< OptionalWriteAccess<T> >
		{
			static bool Test (const Archetype &) {
				return true;
			}
		};
		
		template <typename T>
		struct ArchetypeCompatibility< OptionalReadAccess<T> >
		{
			static bool Test (const Archetype &) {
				return true;
			}
		};

		template <typename ...Types>
		struct ArchetypeCompatibility< Subtractive<Types...> >
		{
			STATIC_ASSERT( CountOf<Types...>() > 0 );

			static bool Test (const Archetype &arch) {
				return _Test<Types...>( arch );
			}

			template <typename T, typename ...Next>
			static bool _Test (const Archetype &arch)
			{
				if constexpr( CountOf<Next...>() )
					return not arch.Exists<T>() and _Test<Next...>( arch );
				else
					return not arch.Exists<T>();
			}
		};

		template <typename ...Types>
		struct ArchetypeCompatibility< Require<Types...> >
		{
			STATIC_ASSERT( CountOf<Types...>() > 0 );

			static bool Test (const Archetype &arch) {
				return _Test<Types...>( arch );
			}

			template <typename T, typename ...Next>
			static bool _Test (const Archetype &arch)
			{
				if constexpr( CountOf<Next...>() )
					return arch.Exists<T>() and _Test<Next...>( arch );
				else
					return arch.Exists<T>();
			}
		};

		template <typename ...Types>
		struct ArchetypeCompatibility< RequireAny<Types...> >
		{
			STATIC_ASSERT( CountOf<Types...>() > 0 );

			static bool Test (const Archetype &arch) {
				return _Test<Types...>( arch );
			}

			template <typename T, typename ...Next>
			static bool _Test (const Archetype &arch)
			{
				if constexpr( CountOf<Next...>() )
					return arch.Exists<T>() or _Test<Next...>( arch );
				else
					return arch.Exists<T>();
			}
		};

	} // _reg_detail_
	
/*
=================================================
	_IsArchetypeSupported
=================================================
*/
	template <typename ArgsList, usize I>
	inline bool  Registry::_IsArchetypeSupported (const Archetype &arch)
	{
		if constexpr( I < ArgsList::Count )
		{
			using T = typename ArgsList::template Get<I>;

			return	_reg_detail_::ArchetypeCompatibility<T>::Test( arch ) and
					_IsArchetypeSupported<ArgsList, I+1>( arch );
		}
		else
		{
			Unused( arch );
			return true;
		}
	}
#endif // AE_DEBUG or AE_DEVELOP

/*
=================================================
	GetStorageComponent
=================================================
*/
	namespace _reg_detail_
	{
		template <typename T>
		struct GetStorageComponent;
		
		template <>
		struct GetStorageComponent< ReadAccess<EntityID> >
		{
			static ReadAccess<EntityID>  Get (ArchetypeStorage* storage) {
				return ReadAccess<EntityID>{ storage->GetEntities() };
			}
		};

		template <typename T>
		struct GetStorageComponent< WriteAccess<T> >
		{
			static WriteAccess<T>  Get (ArchetypeStorage* storage) {
				return WriteAccess<T>{ storage->GetComponents<T>() };
			}
		};
		
		template <typename T>
		struct GetStorageComponent< ReadAccess<T> >
		{
			static ReadAccess<T>  Get (ArchetypeStorage* storage) {
				return ReadAccess<T>{ storage->GetComponents<T>() };
			}
		};

		template <typename T>
		struct GetStorageComponent< OptionalWriteAccess<T> >
		{
			static OptionalWriteAccess<T>  Get (ArchetypeStorage* storage) {
				return OptionalWriteAccess<T>{ storage->GetComponents<T>() };
			}
		};
		
		template <typename T>
		struct GetStorageComponent< OptionalReadAccess<T> >
		{
			static OptionalReadAccess<T>  Get (ArchetypeStorage* storage) {
				return OptionalReadAccess<T>{ storage->GetComponents<T>() };
			}
		};
		
		template <typename ...Types>
		struct GetStorageComponent< Subtractive<Types...> >
		{
			static Subtractive<Types...>  Get (ArchetypeStorage*) {
				return {};
			}
		};
		
		template <typename ...Types>
		struct GetStorageComponent< Require<Types...> >
		{
			static Require<Types...>  Get (ArchetypeStorage*) {
				return {};
			}
		};
		
		template <typename ...Types>
		struct GetStorageComponent< RequireAny<Types...> >
		{
			static RequireAny<Types...>  Get (ArchetypeStorage*) {
				return {};
			}
		};

	} // _reg_detail_

/*
=================================================
	_GetChunk
=================================================
*/
	template <typename ...Args>
	inline Tuple<usize, Args...>  Registry::_GetChunk (ArchetypeStorage* storage, const TypeList<Args...> *)
	{
		return Tuple{ storage->Count(),
					  _reg_detail_::GetStorageComponent<Args>::Get( storage ) ... };
	}
	
/*
=================================================
	_GetSingleComponent
=================================================
*/
	template <typename T>
	inline decltype(auto)  Registry::_GetSingleComponent ()
	{
		if constexpr( IsPointer<T> )
		{
			using A = RemovePointer<T>;
			return GetSingleComponent<A>();		// can be null
		}
		else
		if constexpr( IsReference<T> )
		{
			using A = RemoveReference<T>;
			ASSERT( GetSingleComponent<A>() );	// TODO: component must be created
			return AssignSingleComponent<A>();
		}
		else
		{
			// error!
			return;
		}
	}
	
/*
=================================================
	_WithSingleComponents
=================================================
*/
	template <typename Fn, typename Chunk, typename ...Types>
	void  Registry::_WithSingleComponents (Fn &&fn, ArrayView<Chunk> chunks, const Tuple<Types...> *)
	{
		if constexpr( CountOf<Types...>() == 0 )
			return fn( chunks );
		else
			return fn( chunks, Tuple<Types...>{ _GetSingleComponent<Types>() ... });
	}
//-----------------------------------------------------------------------------
	
	
/*
=================================================
	AddEventListener
=================================================
*/
	template <typename Ev, typename Fn>
	inline void  Registry::AddEventListener (Fn &&fn)
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsEmpty<Ev> );

		_eventListeners.insert({ TypeIdOf<Ev>(), EventListener_t{ FwdArg<Fn>(fn) }});
	}
	
/*
=================================================
	EnqueEvent
=================================================
*/
	template <typename Ev>
	inline void  Registry::EnqueEvent ()
	{
		DRC_EXLOCK( _drCheck );
		STATIC_ASSERT( IsEmpty<Ev> );

		_pendingEvents.push_back( [this]()
		{
			_RunEvent<BeforeEvent<Ev>>();
			_RunEvent<Ev>();
			_RunEvent<AfterEvent<Ev>>();
		});
	}
	
/*
=================================================
	_RunEvent
=================================================
*/
	template <typename Ev>
	inline void  Registry::_RunEvent ()
	{
		TypeId	id	 = TypeIdOf<Ev>();
		auto	iter = _eventListeners.find( id );

		for (; iter != _eventListeners.end() and iter->first == id; ++iter)
		{
			iter->second( *this );
		}
	}
//-----------------------------------------------------------------------------
	

/*
=================================================
	AddMessageListener
=================================================
*/
	template <typename Comp, typename Tag, typename Fn>
	inline void  Registry::AddMessageListener (Fn &&fn)
	{
		_messages.AddListener<Comp, Tag>( FwdArg<Fn>( fn ));
	}
	
/*
=================================================
	AddMessage
=================================================
*/
	template <typename Tag>
	inline void  Registry::AddMessage (EntityID entId, ComponentID compId)
	{
		return _messages.Add<Tag>( entId, compId );
	}
		
	template <typename Tag, typename Comp>
	inline void  Registry::AddMessage (EntityID entId, const Comp& comp)
	{
		return _messages.Add<Tag>( entId, comp );
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	BuildEntityQueryDesc
=================================================
*/
	namespace _reg_detail_
	{
		template <typename T>
		struct BuildEntityQueryDesc;
		
		template <>
		struct BuildEntityQueryDesc< ReadAccess<EntityID> >
		{
			static void  Apply (ArchetypeQueryDesc &) {}
		};

		template <typename T>
		struct BuildEntityQueryDesc< WriteAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) {
				desc.required.Add<T>();
			}
		};
		
		template <typename T>
		struct BuildEntityQueryDesc< ReadAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) {
				desc.required.Add<T>();
			}
		};

		template <typename T>
		struct BuildEntityQueryDesc< OptionalWriteAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &) {}
		};
		
		template <typename T>
		struct BuildEntityQueryDesc< OptionalReadAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &) {}
		};
		
		template <typename ...Types>
		struct BuildEntityQueryDesc< Subtractive<Types...> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) {
				(desc.subtractive.Add<Types>(), ...);
			}
		};
		
		template <typename ...Types>
		struct BuildEntityQueryDesc< Require<Types...> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) {
				(desc.required.Add<Types>(), ...);
			}
		};
		
		template <typename ...Types>
		struct BuildEntityQueryDesc< RequireAny<Types...> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) {
				(desc.requireAny.Add<Types>(), ...);
			}
		};

	} // _reg_detail_

/*
=================================================
	CreateQuery
=================================================
*/
	template <typename ...Args>
	inline QueryID  Registry::CreateQuery ()
	{
		ArchetypeQueryDesc	desc;
		(_reg_detail_::BuildEntityQueryDesc< Args >::Apply( INOUT desc ), ...);

		return CreateQuery( desc );
	}


} // AE::ECS
