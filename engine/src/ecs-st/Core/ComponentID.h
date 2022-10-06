// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Common.h"

namespace AE::ECS
{

	//
	// Component ID
	//

	namespace _hidden_
	{
		template <uint UID>
		struct _ComponentID
		{
			ushort	value;

			constexpr _ComponentID () : value{UMax} {}
			constexpr explicit _ComponentID (ushort id) : value{id} {}

			ND_ constexpr bool  operator <  (const _ComponentID &rhs) const	{ return value <  rhs.value; }
			ND_ constexpr bool  operator >  (const _ComponentID &rhs) const	{ return value >  rhs.value; }
			ND_ constexpr bool  operator == (const _ComponentID &rhs) const	{ return value == rhs.value; }
		};

	}	// _hidden_
	
	using ComponentID		= ECS::_hidden_::_ComponentID<0>;
	using TagComponentID	= ECS::_hidden_::_ComponentID<1>;
	using MsgTagID			= ECS::_hidden_::_ComponentID<2>;



	//
	// Component Type Info
	//
	
	template <typename Comp>
	struct ComponentTypeInfo
	{
		//STATIC_ASSERT( std::is_trivially_destructible_v<Comp> );
		//STATIC_ASSERT( std::is_trivially_copyable_v<Comp> );
		STATIC_ASSERT( std::is_nothrow_destructible_v<Comp> );

		using type	= Comp;
		static inline const ComponentID		id		{ CheckCast<ushort>( Base::_hidden_::StaticTypeIdOf< Comp, 0x1000 >::Get().Get() ) };
		static constexpr Bytes16u			align	{ IsEmpty<Comp> ? 0 : alignof(Comp) };
		static constexpr Bytes16u			size	{ IsEmpty<Comp> ? 0 : sizeof(Comp) };

		static void Ctor (OUT void *comp)
		{
			PlacementNew<Comp>( comp );
		}
	};
	
	template <typename Comp>	struct ComponentTypeInfo< Comp& >		: ComponentTypeInfo<Comp> {};
	template <typename Comp>	struct ComponentTypeInfo< const Comp& >	: ComponentTypeInfo<Comp> {};
	


	//
	// Message Tag Type Info
	//
	
	template <typename Comp>
	struct MsgTagTypeInfo
	{
		STATIC_ASSERT( IsEmpty<Comp> );

		using type	= Comp;
		static inline const MsgTagID	id {CheckCast<ushort>( Base::_hidden_::StaticTypeIdOf< Comp, 0x1002 >::Get().Get() )};
	};

}	// AE::ECS

namespace std
{
	template <uint32_t UID>
	struct hash< AE::ECS::_hidden_::_ComponentID<UID> >
	{
		ND_ size_t  operator () (const AE::ECS::_hidden_::_ComponentID<UID> &id) const
		{
			return AE::Math::BitRotateLeft( size_t(id.value), UID*8 );
		}
	};

}	// std
