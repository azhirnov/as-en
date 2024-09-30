// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::ECS
{

/*
=================================================
	CreateEntity
=================================================
*/
	template <typename ...Components>
	EntityID  Registry::CreateEntity () __NE___
	{
		DRC_EXLOCK( _drCheck );

		EntityID		ent_id = CreateEntity();
		ArchetypeDesc	desc;
		( desc.Add<Components>(), ... );

		ArchetypeStorage*	storage = null;
		Index_t				index;

		_AddEntity( Archetype{desc}, ent_id, OUT storage, OUT index );
		NonNull( storage );

		#if AE_ECS_ENABLE_DEFAULT_MESSAGES
			for (auto& comp_id : storage->GetComponentIDs())
			{
				CHECK( _messages.Add<MsgTag_AddedComponent>( ent_id, comp_id ));
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
		void  CopyComponent (ArchetypeStorage* storage, ArchetypeStorage::Index_t index, T&& comp) __NE___
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
	EntityID  Registry::CreateEntity (Components&& ...comps) __NE___
	{
		DRC_EXLOCK( _drCheck );

		EntityID		ent_id = CreateEntity();
		ArchetypeDesc	desc;
		( desc.Add<RemoveCV<Components>>(), ... );

		ArchetypeStorage*	storage = null;
		Index_t				index;

		_AddEntity( Archetype{desc}, ent_id, OUT storage, OUT index );
		NonNull( storage );

		(_reg_detail_::CopyComponent( storage, index, FwdArg<Components>(comps) ), ...);

		#if AE_ECS_ENABLE_DEFAULT_MESSAGES
			for (auto& comp_id : storage->GetComponentIDs())
			{
				CHECK( _messages.Add<MsgTag_AddedComponent>( ent_id, comp_id ));
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
		class ComponentDbgView final :
			public Registry::IComponentDbgView,
			public NothrowAllocatable
		{
		private:
			ArrayView< T >			_comps;
			std::type_info const&	_type;

		public:
			ComponentDbgView (T const* ptr, usize count)			__NE___	: _comps{ptr, count}, _type{typeid(T)} {}
			~ComponentDbgView ()									__NE_OV {}

			Unique<IComponentDbgView>  ElementView (usize index)	C_Th_OV {
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
	void  Registry::RegisterComponent () __NE___
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
			  comp.dbgView	= [] (void* ptr, usize count) __NE___ -> Unique<IComponentDbgView> {
									return MakeUnique< _reg_detail_::ComponentDbgView<T> >( Cast<T>(ptr), count );
								};
			)
		}
	}

	template <typename ...Types>
	void  Registry::RegisterComponents () __NE___
	{
		(RegisterComponent<Types>(), ...);
	}

/*
=================================================
	GetComponentInfo
=================================================
*/
	inline Ptr<Registry::ComponentInfo const>  Registry::GetComponentInfo (ComponentID compId) C_NE___
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
	EnableIf<not IsEmpty<T>, T&>  Registry::AssignComponent (EntityID entId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	src_storage	= null;
		Index_t				src_index;
		ArchetypeDesc		desc;

		_entities.GetArchetype( entId, OUT src_storage, OUT src_index );

		if ( src_storage != null )
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
			CHECK( _messages.Add<MsgTag_AddedComponent>( entId, ComponentTypeInfo<T>::id ));
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
	EnableIf<IsEmpty<T>, void>  Registry::AssignComponent (EntityID entId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	src_storage	= null;
		Index_t				src_index;
		ArchetypeDesc		desc;

		_entities.GetArchetype( entId, OUT src_storage, OUT src_index );

		if ( src_storage != null )
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
	bool  Registry::RemoveComponent (EntityID entId) __NE___
	{
		return RemoveComponent( entId, ComponentTypeInfo<T>::id );
	}

/*
=================================================
	_DecreaseStorageSize
=================================================
*/
	inline void  Registry::_DecreaseStorageSize (ArchetypeStorage* storage) __NE___
	{
		if ( storage->Count()*4 < storage->Capacity() )
		{
			storage->Reserve( Max( ECS_Config::InitialStorageSize, storage->Count()*2 ));
		}
	}

/*
=================================================
	_IncreaseStorageSize
=================================================
*/
	inline void  Registry::_IncreaseStorageSize (ArchetypeStorage* storage, usize addCount) __NE___
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
	Ptr<T>  Registry::GetComponent (EntityID entId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	storage		= null;
		Index_t				index;

		_entities.GetArchetype( entId, OUT storage, OUT index );

		if ( storage != null )
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
	Ptr<AddConst<T>>  Registry::GetComponent (EntityID entId) C_NE___
	{
		return const_cast<Registry*>(this)->GetComponent< AddConst<T> >( entId );
	}

/*
=================================================
	GetComponents
=================================================
*/
	template <typename ...Types>
	Tuple<Ptr<Types>...>  Registry::GetComponents (EntityID entId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		ArchetypeStorage*	storage		= null;
		Index_t				index;

		_entities.GetArchetype( entId, OUT storage, OUT index );

		if_likely( storage != null )
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
	Tuple<Ptr<AddConst<Types>>...>  Registry::GetComponents (EntityID entId) C_NE___
	{
		return const_cast<Registry*>(this)->GetComponents< AddConst<Types>... >( entId );
	}

/*
=================================================
	RemoveComponents
=================================================
*/
	template <typename ...Types>
	void  Registry::RemoveComponents (QueryID query) __NE___
	{
		StaticAssert( CountOf<Types...>() > 0 );

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
	inline Registry::CompDbgView_t  Registry::EntityDbgView (EntityID entId) C_NE___
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
	T&  Registry::AssignSingleComponent () __NE___
	{
		DRC_EXLOCK( _drCheck );

		StaticAssert( not IsEmpty<T> );
		StaticAssert( std::is_standard_layout_v<T> );
		StaticAssert( std::is_trivially_copyable_v<T> );
		StaticAssert( IsTriviallyDestructible<T> );
		StaticAssert( std::is_nothrow_destructible_v<T> );	// TODO: add IsECSComponentCompatible<>

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
	bool  Registry::RemoveSingleComponent () __NE___
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
	Ptr<T>  Registry::GetSingleComponent () __NE___
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
		struct CompareSingleComponents< LT, WriteAccess<RT> >		: CT_Bool< IsSameTypes< LT, RT >> {};

		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, ReadAccess<RT> >		: CT_Bool< IsSameTypes< LT, RT >> {};

		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, OptionalWriteAccess<RT> > : CT_Bool< IsSameTypes< LT, RT >> {};

		template <typename LT, typename RT>
		struct CompareSingleComponents< LT, OptionalReadAccess<RT> > : CT_Bool< IsSameTypes< LT, RT >> {};

		template <typename LT, typename ...RTs>
		struct CompareSingleComponents< LT, Subtractive<RTs...> >	: CT_Bool< TypeList<RTs...>::template HasType<LT> >{};

		template <typename LT, typename ...RTs>
		struct CompareSingleComponents< LT, Require<RTs...> >		: CT_Bool< TypeList<RTs...>::template HasType<LT> >{};

		template <typename LT, typename ...RTs>
		struct CompareSingleComponents< LT, RequireAny<RTs...> >	: CT_Bool< TypeList<RTs...>::template HasType<LT> >{};	// TODO: allow cases with Optional<A> + RequireAny<A,B,C> ???


		template <typename RawTypeList, typename WrapedType>
		struct CompareMultiComponents								: CT_False {};

		template <bool isTL, typename LT, typename ...RTs>
		struct CompareMultiComponents_Helper						: CT_Bool< TypeList<RTs...>::template HasType<LT> >{};

		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents_Helper< true, LTs, RTs... >	: CT_Bool< TypeList<RTs...>::template ForEach_Or< LTs::template HasType >>{};

		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents< LTs, Subtractive<RTs...> >	: CT_Bool< CompareMultiComponents_Helper< IsTypeList<LTs>, LTs, RTs... >::value >{};

		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents< LTs, Require<RTs...> >		: CT_Bool< CompareMultiComponents_Helper< IsTypeList<LTs>, LTs, RTs... >::value >{};

		template <typename LTs, typename ...RTs>
		struct CompareMultiComponents< LTs, RequireAny<RTs...> >	: CT_Bool< CompareMultiComponents_Helper< IsTypeList<LTs>, LTs, RTs... >::value >{};


		template <template <typename, typename> class Comparator,
				  typename RefType, typename ArgsList, usize ExceptIdx>
		struct CompareComponents
		{
			template <usize I>
			static constexpr bool  Cmp () __NE___
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
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename T>
		struct CheckForDuplicateComponents< ReadAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename T>
		struct CheckForDuplicateComponents< OptionalWriteAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename T>
		struct CheckForDuplicateComponents< OptionalReadAccess<T> >
		{
			template <usize I, typename ArgsList>
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< CompareSingleComponents, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename ...Types>
		struct CheckForDuplicateComponents< Subtractive<Types...> >
		{
			StaticAssert( CountOf<Types...>() > 0 );

			template <usize I, typename ArgsList>
			static constexpr bool  Test () __NE___ {
				return not (CompareComponents< CompareMultiComponents, Types, ArgsList, I >::template Cmp<0>() or ...);
			}
		};

		template <typename ...Types>
		struct CheckForDuplicateComponents< Require<Types...> >
		{
			StaticAssert( CountOf<Types...>() > 0 );

			template <usize I, typename ArgsList>
			static constexpr bool  Test () __NE___ {
				return not (CompareComponents< CompareMultiComponents, Types, ArgsList, I >::template Cmp<0>() or ...);
			}
		};

		template <typename ...Types>
		struct CheckForDuplicateComponents< RequireAny<Types...> >
		{
			StaticAssert( CountOf<Types...>() > 0 );

			template <usize I, typename ArgsList>
			static constexpr bool  Test () __NE___ {
				return not (CompareComponents< CompareMultiComponents, Types, ArgsList, I >::template Cmp<0>() or ...);
			}
		};


		template <typename ArgsList, usize I = 0>
		static constexpr void  CheckForDuplicates () __NE___
		{
			if constexpr( I < ArgsList::Count )
			{
				constexpr bool is_valid = CheckForDuplicateComponents< typename ArgsList::template Get<I> >::template Test< I, ArgsList >();
				StaticAssert( is_valid );

				CheckForDuplicates< ArgsList, I+1 >();
			}
		}


		template <typename LT, typename RT>
		struct SC_Comparator;

		template <typename LT, typename RT>
		struct SC_Comparator< LT, RT* >			: CT_Bool< IsSameTypes< LT, RT >>{};

		template <typename LT, typename RT>
		struct SC_Comparator< LT, RT const* >	: CT_Bool< IsSameTypes< LT, RT >>{};

		template <typename LT, typename RT>
		struct SC_Comparator< LT, RT& >			: CT_Bool< IsSameTypes< LT, RT >>{};


		template <typename T>
		struct SC_CheckForDuplicateComponents;

		template <typename T>
		struct SC_CheckForDuplicateComponents< T* >
		{
			template <typename ArgsList, usize I>
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< SC_Comparator, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename T>
		struct SC_CheckForDuplicateComponents< T const* >
		{
			template <typename ArgsList, usize I>
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< SC_Comparator, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename T>
		struct SC_CheckForDuplicateComponents< T& >
		{
			template <typename ArgsList, usize I>
			static constexpr bool  Test () __NE___ {
				return not CompareComponents< SC_Comparator, T, ArgsList, I >::template Cmp<0>();
			}
		};

		template <typename ArgsList, usize I = 0>
		static constexpr void  SC_CheckForDuplicates () __NE___
		{
			if constexpr( I < ArgsList::Count )
			{
				constexpr bool is_valid = SC_CheckForDuplicateComponents< typename ArgsList::template Get<I> >::template Test< ArgsList, I >();
				StaticAssert( is_valid );

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
			StaticAssert( IsSpecializationOf< ChunkArray, ArrayView >);

			using Chunk = typename ChunkArray::value_type;
			StaticAssert( IsSpecializationOf< Chunk, Tuple >);

			using ChunkTL = TypeList< Chunk >;
			StaticAssert( ChunkTL::Count > 1 );
			StaticAssert( IsSameTypes< typename ChunkTL::template Get<0>, usize >);

			using CompOnly = typename ChunkTL::PopFront::type;
			using SCTuple  = Tuple<>;
		};

		template <typename Args>
		struct _SystemFnInfoImpl< Args, 2 > : _SystemFnInfoImpl< Args, 1 >
		{
			StaticAssert( Args::Count == 2 );

			using SCTuple = typename Args::template Get<1>;
			StaticAssert( IsSpecializationOf< SCTuple, Tuple >);
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
	Enqueue
=================================================
*/
	template <typename Obj, typename Class, typename ...Args>
	void  Registry::Enqueue (QueryID query, Obj obj, void (Class::*fn)(Args&&...)) __NE___
	{
		return Enqueue( query, [obj, fn](Args&& ...args) { return (obj->*fn)( FwdArg<Args>(args)... ); });
	}

	template <typename Fn>
	void  Registry::Enqueue (QueryID query, Fn &&fn) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if constexpr( FunctionInfo<Fn>::args::Count == 0 )
		{
			_pendingEvents.push_back( FwdArg<Fn>(fn) );
		}
		else
		{
			_pendingEvents.push_back(
				[this, query, fn = FwdArg<Fn>(fn)] () __NE___
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
	void  Registry::Execute (QueryID query, Fn &&fn) __NE___
	{
		using Args = typename FunctionInfo<Fn>::args;
		StaticAssert( Args::Count > 0 );

		DRC_EXLOCK( _drCheck );

		if constexpr( IsSpecializationOf< typename Args::template Get<0>, ArrayView >)
			return _Execute_v1( query, FwdArg<Fn>(fn) );
		else
			return _Execute_v2( query, FwdArg<Fn>(fn), static_cast<const Args*>(null) );
	}

/*
=================================================
	_Execute_v1
=================================================
*/
	template <typename Fn>
	void  Registry::_Execute_v1 (QueryID query, Fn &&fn) __NE___
	{
		using Info		= _reg_detail_::SystemFnInfo< Fn >;
		using Chunk		= typename Info::Chunk;
		using CompOnly	= typename Info::CompOnly;
		using SCTuple	= typename Info::SCTuple;

		#ifdef AE_ECS_VALIDATE_SYSTEM_FN
			_reg_detail_::CheckForDuplicates< CompOnly >();
			_reg_detail_::SC_CheckForDuplicates< TypeList<SCTuple> >();
		#endif

		Array<ArchetypeStorage*>	storages;		// TODO: optimize
		Array<Chunk>				chunks;
		const auto&					q_data = _queries[ query.Index() ];

		CHECK( not q_data.locked );
		q_data.locked = true;

		for (auto* ptr : q_data.archetypes)
		{
			ASSERT( _IsArchetypeSupported< CompOnly, 0 >( ptr->first ));

			auto&	storage	= ptr->second;
			storage->Lock();
			storages.emplace_back( storage.get() );													// throw
			chunks.emplace_back( _GetChunk( storage.get(), static_cast<const CompOnly *>(null) ));	// throw
		}

		_WithSingleComponents( FwdArg<Fn>(fn), ArrayView<Chunk>{chunks.data(), chunks.size()}, static_cast< SCTuple const *>(null) );

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
			static exact_t  Get (ChunkType &chunk, usize i) __NE___
			{
				return chunk.template Get< MapCompType<T> >()[i];
			}
		};

		template <typename T>
		struct GetStorageElement< T * >
		{
			template <typename ChunkType>
			static T*  Get (ChunkType &chunk, usize i) __NE___
			{
				auto&	arr = chunk.template Get< MapCompType<T*> >();
				return arr ? &arr[i] : null;
			}
		};

		template <typename ...Types>
		struct GetStorageElement< Require<Types...> >
		{
			template <typename ChunkType>
			static Require<Types...>  Get (ChunkType &, usize) __NE___ {
				return {};
			}
		};

		template <typename ...Types>
		struct GetStorageElement< RequireAny<Types...> >
		{
			template <typename ChunkType>
			static RequireAny<Types...>  Get (ChunkType &, usize) __NE___ {
				return {};
			}
		};

		template <typename ...Types>
		struct GetStorageElement< Subtractive<Types...> >
		{
			template <typename ChunkType>
			static Subtractive<Types...>  Get (ChunkType &, usize) __NE___ {
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
	void  Registry::_Execute_v2 (QueryID query, Fn &&inFn, const TypeList<Args...>*) __NE___
	{
		_Execute_v1( query,
			[fn = FwdArg<Fn>(inFn)] (ArrayView<Tuple< usize, _reg_detail_::MapCompType<Args>... >> chunks) __NE___
			{
				for (auto& chunk : chunks)
				{
					for (usize i = 0, cnt = chunk.template Get<0>(); i < cnt; ++i)
					{
						CheckNothrow( IsNoExcept( fn( _reg_detail_::GetStorageElement<Args>::template Get( chunk, i )... )));

						fn( _reg_detail_::GetStorageElement<Args>::template Get( chunk, i )... );
					}
				}
			});
	}
//-----------------------------------------------------------------------------


#ifdef AE_DEBUG
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
			static bool  Test (const Archetype &) __NE___ {
				return true;
			}
		};

		template <typename T>
		struct ArchetypeCompatibility< WriteAccess<T> >
		{
			static bool  Test (const Archetype &arch) __NE___ {
				return arch.Exists<T>();
			}
		};

		template <typename T>
		struct ArchetypeCompatibility< ReadAccess<T> >
		{
			static bool  Test (const Archetype &arch) __NE___ {
				return arch.Exists<T>();
			}
		};

		template <typename T>
		struct ArchetypeCompatibility< OptionalWriteAccess<T> >
		{
			static bool  Test (const Archetype &) __NE___ {
				return true;
			}
		};

		template <typename T>
		struct ArchetypeCompatibility< OptionalReadAccess<T> >
		{
			static bool  Test (const Archetype &) __NE___ {
				return true;
			}
		};

		template <typename ...Types>
		struct ArchetypeCompatibility< Subtractive<Types...> >
		{
			StaticAssert( CountOf<Types...>() > 0 );

			static bool  Test (const Archetype &arch) __NE___ {
				return _Test<Types...>( arch );
			}

			template <typename T, typename ...Next>
			static bool  _Test (const Archetype &arch) __NE___
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
			StaticAssert( CountOf<Types...>() > 0 );

			static bool  Test (const Archetype &arch) __NE___ {
				return _Test<Types...>( arch );
			}

			template <typename T, typename ...Next>
			static bool  _Test (const Archetype &arch) __NE___
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
			StaticAssert( CountOf<Types...>() > 0 );

			static bool  Test (const Archetype &arch) __NE___ {
				return _Test<Types...>( arch );
			}

			template <typename T, typename ...Next>
			static bool  _Test (const Archetype &arch) __NE___
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
	bool  Registry::_IsArchetypeSupported (const Archetype &arch) __NE___
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
#endif // AE_DEBUG

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
			static ReadAccess<EntityID>  Get (ArchetypeStorage* storage) __NE___ {
				return ReadAccess<EntityID>{ storage->GetEntities() };
			}
		};

		template <typename T>
		struct GetStorageComponent< WriteAccess<T> >
		{
			static WriteAccess<T>  Get (ArchetypeStorage* storage) __NE___ {
				return WriteAccess<T>{ storage->GetComponents<T>() };
			}
		};

		template <typename T>
		struct GetStorageComponent< ReadAccess<T> >
		{
			static ReadAccess<T>  Get (ArchetypeStorage* storage) __NE___ {
				return ReadAccess<T>{ storage->GetComponents<T>() };
			}
		};

		template <typename T>
		struct GetStorageComponent< OptionalWriteAccess<T> >
		{
			static OptionalWriteAccess<T>  Get (ArchetypeStorage* storage) __NE___ {
				return OptionalWriteAccess<T>{ storage->GetComponents<T>() };
			}
		};

		template <typename T>
		struct GetStorageComponent< OptionalReadAccess<T> >
		{
			static OptionalReadAccess<T>  Get (ArchetypeStorage* storage) __NE___ {
				return OptionalReadAccess<T>{ storage->GetComponents<T>() };
			}
		};

		template <typename ...Types>
		struct GetStorageComponent< Subtractive<Types...> >
		{
			static Subtractive<Types...>  Get (ArchetypeStorage*) __NE___ {
				return {};
			}
		};

		template <typename ...Types>
		struct GetStorageComponent< Require<Types...> >
		{
			static Require<Types...>  Get (ArchetypeStorage*) __NE___ {
				return {};
			}
		};

		template <typename ...Types>
		struct GetStorageComponent< RequireAny<Types...> >
		{
			static RequireAny<Types...>  Get (ArchetypeStorage*) __NE___ {
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
	Tuple<usize, Args...>  Registry::_GetChunk (ArchetypeStorage* storage, const TypeList<Args...> *) __NE___
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
	exact_t  Registry::_GetSingleComponent () __NE___
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
	void  Registry::_WithSingleComponents (Fn &&fn, ArrayView<Chunk> chunks, const Tuple<Types...> *) __NE___
	{
		if constexpr( CountOf<Types...>() == 0 )
		{
			CheckNothrow( IsNoExcept( fn( chunks )));

			return fn( chunks );
		}
		else
		{
			CheckNothrow( IsNoExcept( fn( chunks, Tuple<Types...>{ _GetSingleComponent<Types>() ... })));

			return fn( chunks, Tuple<Types...>{ _GetSingleComponent<Types>() ... });
		}
	}
//-----------------------------------------------------------------------------


/*
=================================================
	AddEventListener
=================================================
*/
	template <typename Ev, typename Fn>
	void  Registry::AddEventListener (Fn &&fn) __NE___
	{
		DRC_EXLOCK( _drCheck );
		StaticAssert( IsEmpty<Ev> );

		_eventListeners.insert({ TypeIdOf<Ev>(), EventListener_t{ FwdArg<Fn>(fn) }});
	}

/*
=================================================
	EnqueueEvent
=================================================
*/
	template <typename Ev>
	void  Registry::EnqueueEvent () __NE___
	{
		DRC_EXLOCK( _drCheck );
		StaticAssert( IsEmpty<Ev> );

		_pendingEvents.push_back( [this]() __NE___
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
	void  Registry::_RunEvent () __NE___
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
	bool  Registry::AddMessageListener (Fn &&fn) __NE___
	{
		return _messages.AddListener<Comp, Tag>( FwdArg<Fn>( fn ));
	}

/*
=================================================
	AddMessage
=================================================
*/
	template <typename Tag>
	bool  Registry::AddMessage (EntityID entId, ComponentID compId) __NE___
	{
		return _messages.Add<Tag>( entId, compId );
	}

	template <typename Tag, typename Comp>
	bool  Registry::AddMessage (EntityID entId, const Comp& comp) __NE___
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
			static void  Apply (ArchetypeQueryDesc &) __NE___ {}
		};

		template <typename T>
		struct BuildEntityQueryDesc< WriteAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) __NE___ {
				desc.required.Add<T>();
			}
		};

		template <typename T>
		struct BuildEntityQueryDesc< ReadAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) __NE___ {
				desc.required.Add<T>();
			}
		};

		template <typename T>
		struct BuildEntityQueryDesc< OptionalWriteAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &) __NE___ {}
		};

		template <typename T>
		struct BuildEntityQueryDesc< OptionalReadAccess<T> >
		{
			static void  Apply (ArchetypeQueryDesc &) __NE___ {}
		};

		template <typename ...Types>
		struct BuildEntityQueryDesc< Subtractive<Types...> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) __NE___ {
				(desc.subtractive.Add<Types>(), ...);
			}
		};

		template <typename ...Types>
		struct BuildEntityQueryDesc< Require<Types...> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) __NE___ {
				(desc.required.Add<Types>(), ...);
			}
		};

		template <typename ...Types>
		struct BuildEntityQueryDesc< RequireAny<Types...> >
		{
			static void  Apply (ArchetypeQueryDesc &desc) __NE___ {
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
	QueryID  Registry::CreateQuery () __NE___
	{
		ArchetypeQueryDesc	desc;
		(_reg_detail_::BuildEntityQueryDesc< Args >::Apply( INOUT desc ), ...);

		return CreateQuery( desc );
	}


} // AE::ECS
