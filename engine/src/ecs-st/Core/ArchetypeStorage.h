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
		using Allocator_t	= UntypedAllocator;
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
		Bytes32u			_maxAlign;
		Bytes32u			_memSize;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;

		Registry const&		_owner;

		DEBUG_ONLY(
			CompDbgView_t	_dbgView;
			void *			_memoryEnd;
		)


	// methods
	public:
		explicit ArchetypeStorage (const Registry &reg, const Archetype &archetype, usize capacity) __NE___;
		~ArchetypeStorage ()										__NE___;

			bool  AddEntities (ArrayView<EntityID> ids, OUT Index_t &startIndex) __NE___;
			bool  Add (EntityID id, OUT Index_t &index)				__NE___;
			bool  Erase (Index_t index, OUT EntityID &movedEntity)	__NE___;
		ND_ bool  IsValid (EntityID id, Index_t index)				C_NE___;
			void  Clear ()											__NE___;
			void  Reserve (usize size)								__NE___;
			//void  Reorder (Index_t offset, ArrayView<Index_t> newOrder);

			void  Lock ()											__NE___;
			void  Unlock ()											__NE___;
		ND_ bool  IsLocked ()										C_NE___;
		
		template <typename T>
		ND_ T*					GetComponent (Index_t idx)			C_NE___;
		ND_ Pair<void*, Bytes>	GetComponent (Index_t, ComponentID)	C_NE___;

		template <typename T>
		ND_ T*					GetComponents ()					C_NE___;
		ND_ void*				GetComponents (ComponentID id)		C_NE___;
		
		template <typename T>
		ND_ bool				HasComponent ()						C_NE___	{ return _archetype.Exists<T>(); }
		ND_ bool				HasComponent (ComponentID id)		C_NE___	{ return _archetype.Exists( id ); }

		ND_ EntityID const*		GetEntities ()						C_NE___;	// local index to EntityID
		ND_ usize				Capacity ()							C_NE___	{ return _capacity; }
		ND_ usize				Count ()							C_NE___	{ return _count; }
		ND_ bool				Empty ()							C_NE___	{ return _count == 0; }
		ND_ Archetype const&	GetArchetype ()						C_NE___	{ return _archetype; }
		ND_ Bytes				GetMemorySize ()					C_NE___	{ return _memSize; }

		ND_ auto				GetComponentIDs ()					C_NE___	-> ArrayView<ComponentID>	{ return _components.get<0>(); }
		ND_ auto				GetComponentSizes ()				C_NE___	-> ArrayView<Bytes16u>		{ return _components.get<1>(); }
		ND_ auto				GetComponentAligns ()				C_NE___	-> ArrayView<Bytes16u>		{ return _components.get<2>(); }
		ND_ auto				GetComponentData ()					__NE___	-> ArrayView<void*>			{ return _components.get<3>(); }

		DEBUG_ONLY(
		 ND_ CompDbgView_t		EntityDbgView (Index_t idx)			C_NE___;

		 ND_ bool				IsInMemoryRange (const void*, Bytes)C_NE___;
		)


	private:
		ND_ EntityID *	_GetEntities ()								__NE___;

		ND_ usize		_IndexOf (ComponentID id)					C_NE___;

			bool		_InitComponents ()							__NE___;
	};


	
/*
=================================================
	GetComponents
=================================================
*/
	template <typename T>
	inline T*  ArchetypeStorage::GetComponents () C_NE___
	{
		STATIC_ASSERT( not IsEmpty<T> );
		return Cast<T>( GetComponents( ComponentTypeInfo<T>::id ));
	}
	
/*
=================================================
	GetComponents
=================================================
*/
	inline void*  ArchetypeStorage::GetComponents (ComponentID id) C_NE___
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
	inline T*  ArchetypeStorage::GetComponent (Index_t idx) C_NE___
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
	inline Pair<void*, Bytes>  ArchetypeStorage::GetComponent (Index_t idx, ComponentID id) C_NE___
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
	inline usize  ArchetypeStorage::_IndexOf (ComponentID id) C_NE___
	{
		return BinarySearch( _components.get<0>(), id );
	}
	
/*
=================================================
	GetEntities
=================================================
*/
	inline EntityID const*  ArchetypeStorage::GetEntities () C_NE___
	{
		ASSERT( _memory );
		return Cast<EntityID>( _memory );
	}
	
/*
=================================================
	_GetEntities
=================================================
*/
	inline EntityID*  ArchetypeStorage::_GetEntities () __NE___
	{
		ASSERT( _memory );
		return Cast<EntityID>( _memory );
	}
	
/*
=================================================
	Lock
=================================================
*/
	inline void  ArchetypeStorage::Lock () __NE___
	{
		_locks.fetch_add( 1 );
	}
	
/*
=================================================
	Unlock
=================================================
*/
	inline void  ArchetypeStorage::Unlock () __NE___
	{
		_locks.fetch_sub( 1 );
	}
	
/*
=================================================
	IsLocked
=================================================
*/
	inline bool  ArchetypeStorage::IsLocked () C_NE___
	{
		return _locks.load() > 0;
	}
	
/*
=================================================
	EntityDbgView
=================================================
*/
DEBUG_ONLY(
	inline ArchetypeStorage::CompDbgView_t  ArchetypeStorage::EntityDbgView (Index_t idx) C_NE___
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
	inline bool  ArchetypeStorage::IsInMemoryRange (const void* ptr, Bytes size) C_NE___
	{
		return (ptr >= _memory) and ((ptr + size) <= _memoryEnd);
	}
)

} // AE::ECS
