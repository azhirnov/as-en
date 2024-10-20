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

			constexpr _ComponentID ()									__NE___	: value{UMax} {}
			constexpr explicit _ComponentID (ushort id)					__NE___	: value{id} {}

			ND_ constexpr bool  operator <  (const _ComponentID &rhs)	C_NE___	{ return value <  rhs.value; }
			ND_ constexpr bool  operator >  (const _ComponentID &rhs)	C_NE___	{ return value >  rhs.value; }
			ND_ constexpr bool  operator == (const _ComponentID &rhs)	C_NE___	{ return value == rhs.value; }
		};

	} // _hidden_

	using ComponentID		= ECS::_hidden_::_ComponentID<0>;
	using TagComponentID	= ECS::_hidden_::_ComponentID<1>;
	using MsgTagID			= ECS::_hidden_::_ComponentID<2>;



	//
	// Is Trivial Component
	//
	template <typename T>
	struct TTrivialComponent : CT_Bool< Base::IsTrivial<T> >{};

	template <typename T>
	static constexpr bool	IsTrivialComponent = TTrivialComponent< T >::value;



	//
	// Component Type Info
	//
	template <typename Comp>
	struct ComponentTypeInfo
	{
		StaticAssert( IsTrivialComponent<Comp> );

		using type	= Comp;
		static inline const ComponentID		id		{ CheckCast<ushort>( Base::_hidden_::StaticTypeIdOf< Comp, 0x1000 >::Get().Get() ) };
		static constexpr Bytes16u			align	{ushort( IsEmpty<Comp> ? 0 : alignof(Comp) )};
		static constexpr Bytes16u			size	{ushort( IsEmpty<Comp> ? 0 : sizeof(Comp) )};

		static void  Ctor (OUT void* comp) __NE___	{ PlacementNew<Comp>( OUT comp ); }
	};

	template <typename Comp>	struct ComponentTypeInfo< Comp& >		: ComponentTypeInfo<Comp> {};
	template <typename Comp>	struct ComponentTypeInfo< const Comp& >	: ComponentTypeInfo<Comp> {};



	//
	// Message Tag Type Info
	//

	template <typename Comp>
	struct MsgTagTypeInfo
	{
		StaticAssert( IsEmpty<Comp> );

		using type	= Comp;
		static inline const MsgTagID	id {CheckCast<ushort>( Base::_hidden_::StaticTypeIdOf< Comp, 0x1002 >::Get().Get() )};
	};

} // AE::ECS


template <uint32_t UID>
struct std::hash< AE::ECS::_hidden_::_ComponentID<UID> >
{
	ND_ size_t  operator () (const AE::ECS::_hidden_::_ComponentID<UID> &id) C_NE___
	{
		return AE::Math::BitRotateLeft( size_t(id.value), UID*8 );
	}
};
