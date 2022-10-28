// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Core/Archetype.h"
#include "base/Containers/FixedTupleArray.h"

namespace AE::ECS
{
	
	//
	// Archetype Storage
	//

	class ArchetypeStorage final
	{
	// types
	public:
		enum class Index_t : uint { Unknown = ~0u };

		DEBUG_ONLY(		
			class IComponentDbgView {
			public:
				virtual ~IComponentDbgView () {}
				ND_ virtual Unique<IComponentDbgView>  ElementView (usize index) const = 0;
			};

			using CompDbgView_t	= FixedArray< Unique<IComponentDbgView>, ECS_Config::MaxComponentsPerArchetype >;
		)

	private:
		using Allocator_t	= UntypedAlignedAllocator;
		using Components_t	= FixedTupleArray< ECS_Config::MaxComponentsPerArchetype,
									/*0 - id    */ ComponentID,
									/*1 - size  */ Bytes16u,
									/*2 - align */ Bytes16u,
									/*3 - ptr   */ void*,
									/*4 - ctor  */ void (*)(void*) >;


	// variables
	private:
		void *				_memory;
		usize				_count;
		Atomic<int>			_locks;

		const Archetype		_archetype;
		Components_t		_components;
		usize				_capacity;
		Bytes				_maxAlign;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;

		Registry const&		_owner;

		DEBUG_ONLY(
			CompDbgView_t	_dbgView;
			void *			_memoryEnd;
		)


	// methods
	public:
		explicit ArchetypeStorage (const Registry &reg, const Archetype &archetype, usize capacity);
		~ArchetypeStorage ();

			bool  AddEntities (ArrayView<EntityID> ids, OUT Index_t &startIndex);
			bool  Add (EntityID id, OUT Index_t &index);
			bool  Erase (Index_t index, OUT EntityID &movedEntity);
		ND_ bool  IsValid (EntityID id, Index_t index) const;
			void  Clear ();
			void  Reserve (usize size);
			//void  Reorder (Index_t offset, ArrayView<Index_t> newOrder);

			void  Lock ();
			void  Unlock ();
		ND_ bool  IsLocked () const;
		
		template <typename T>
		ND_ T*					GetComponent (Index_t idx)		const;
		ND_ Pair<void*, Bytes>	GetComponent (Index_t idx, ComponentID id) const;

		template <typename T>
		ND_ T*					GetComponents ()				const;
		ND_ void*				GetComponents (ComponentID id)	const;
		
		template <typename T>
		ND_ bool				HasComponent ()					const	{ return _archetype.Exists<T>(); }
		ND_ bool				HasComponent (ComponentID id)	const	{ return _archetype.Exists( id ); }

		ND_ EntityID const*		GetEntities ()					const;	// local index to EntityID
		ND_ usize				Capacity ()						const	{ return _capacity; }
		ND_ usize				Count ()						const	{ return _count; }
		ND_ bool				Empty ()						const	{ return _count == 0; }
		ND_ Archetype const&	GetArchetype ()					const	{ return _archetype; }

		ND_ ArrayView<ComponentID>	GetComponentIDs ()		const	{ return _components.get<0>(); }
		ND_ ArrayView<Bytes16u>		GetComponentSizes ()	const	{ return _components.get<1>(); }
		ND_ ArrayView<Bytes16u>		GetComponentAligns ()	const	{ return _components.get<2>(); }
		ND_ ArrayView<void*>		GetComponentData ()				{ return _components.get<3>(); }

		DEBUG_ONLY(
		 ND_ CompDbgView_t		EntityDbgView (Index_t idx)		const;

		 ND_ bool				IsInMemoryRange (const void* ptr, Bytes size) const;
		)


	private:
		ND_ EntityID *	_GetEntities ();

		ND_ usize		_IndexOf (ComponentID id) const;

		bool _InitComponents ();
	};


	
/*
=================================================
	GetComponents
=================================================
*/
	template <typename T>
	inline T*  ArchetypeStorage::GetComponents () const
	{
		STATIC_ASSERT( not IsEmpty<T> );
		return Cast<T>( GetComponents( ComponentTypeInfo<T>::id ));
	}
	
/*
=================================================
	GetComponents
=================================================
*/
	inline void*  ArchetypeStorage::GetComponents (ComponentID id) const
	{
		ASSERT( _memory );
		usize	pos = _IndexOf( id );
		return	pos < _components.size() ?
					_components.at<3>(pos) :
					null;
	}
	
/*
=================================================
	GetComponent
=================================================
*/
	template <typename T>
	inline T*  ArchetypeStorage::GetComponent (Index_t idx) const
	{
		STATIC_ASSERT( not IsEmpty<T> );
		ASSERT( usize(idx) < Count() );
		ASSERT( _memory );

		usize	pos = _IndexOf( ComponentTypeInfo<T>::id );
		return	pos < _components.size() ?
					Cast<T>( _components.at<3>(pos) ) + usize(idx) :
					null;

	}
	
/*
=================================================
	GetComponent
=================================================
*/
	inline Pair<void*, Bytes>  ArchetypeStorage::GetComponent (Index_t idx, ComponentID id) const
	{
		ASSERT( usize(idx) < Count() );
		ASSERT( _memory );

		usize	pos = _IndexOf( id );
		return	pos < _components.size() ?
					Pair<void*, Bytes>{ _components.at<3>(pos) + Bytes{_components.at<1>(pos)} * usize(idx), Bytes{_components.at<1>(pos)} } :
					Pair<void*, Bytes>{ null, 0_b };
	}

/*
=================================================
	_IndexOf
=================================================
*/
	inline usize  ArchetypeStorage::_IndexOf (ComponentID id) const
	{
		return BinarySearch( _components.get<0>(), id );
	}
	
/*
=================================================
	GetEntities
=================================================
*/
	inline EntityID const*  ArchetypeStorage::GetEntities () const
	{
		ASSERT( _memory );
		return Cast<EntityID>( _memory );
	}
	
/*
=================================================
	_GetEntities
=================================================
*/
	inline EntityID*  ArchetypeStorage::_GetEntities ()
	{
		ASSERT( _memory );
		return Cast<EntityID>( _memory );
	}
	
/*
=================================================
	Lock
=================================================
*/
	inline void  ArchetypeStorage::Lock ()
	{
		_locks.fetch_add( 1 );
	}
	
/*
=================================================
	Unlock
=================================================
*/
	inline void  ArchetypeStorage::Unlock ()
	{
		_locks.fetch_sub( 1 );
	}
	
/*
=================================================
	IsLocked
=================================================
*/
	inline bool  ArchetypeStorage::IsLocked () const
	{
		return _locks.load() > 0;
	}
	
/*
=================================================
	EntityDbgView
=================================================
*/
DEBUG_ONLY(
	inline ArchetypeStorage::CompDbgView_t  ArchetypeStorage::EntityDbgView (Index_t idx) const
	{
		CompDbgView_t	result;
		for (auto& comp : _dbgView) {
			result.emplace_back( comp ? comp->ElementView( usize(idx) ) : Default );
		}
		return result;
	}
)

/*
=================================================
	IsInMemoryRange
=================================================
*/
DEBUG_ONLY(
	inline bool  ArchetypeStorage::IsInMemoryRange (const void* ptr, Bytes size) const
	{
		return (ptr >= _memory) and ((ptr + size) <= _memoryEnd);
	}
)

} // AE::ECS
