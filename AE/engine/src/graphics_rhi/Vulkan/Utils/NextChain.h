// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN

namespace AE::Graphics
{
	template <typename T>
	concept IsMutableVkPNext = requires
	{
		{&T::sType} -> SameAs< VkStructureType T::* >;
		{&T::pNext} -> SameAs< void* T::* >;
	};

	template <typename T>
	concept IsConstVkPNext = requires
	{
		{&T::sType} -> SameAs< VkStructureType T::* >;
		{&T::pNext} -> SameAs< const void* T::* >;
	};



	//
	// pNext Chain (setter for Vulkan 'pNext' field)
	//

	template <typename InitialType>
	struct VNextChain
	{
	private:
		using PNext_t = decltype(InitialType::pNext);

		PNext_t *	_next;

	public:
		template <typename T>
		explicit VNextChain (T &st)				__NE___
		{
			StaticAssert( IsMutableVkPNext<T> or IsConstVkPNext<T> );

			_next  = &st.pNext;
			*_next = null;
		}

		template <typename T>
		void  Add (T &st)						__NE___
		{
			StaticAssert( IsMutableVkPNext<T> or IsConstVkPNext<T> );

			*_next = &st;
			 _next = &st.pNext;
			*_next = null;
		}

		// allow const_cast
		template <typename T>
		void  AddConst (T &st)					__NE___
		{
			StaticAssert( IsMutableVkPNext<T> or IsConstVkPNext<T> );
			StaticAssert( IsMutableVkPNext<T> != IsMutableVkPNext<InitialType> );

			*_next = &st;
			 _next = const_cast< const void** >( &st.pNext );
			*_next = null;
		}
	};


	template <typename T>
	VNextChain (T &) -> VNextChain<T>;



	//
	// pNext Iterator
	//

	template <typename InitialType>
	struct VNextIterator
	{
	private:
		InitialType *	_next;

	public:
		template <typename T>
		explicit VNextIterator (T &st)							__NE___	{ _next = static_cast< InitialType *>( st.pNext ); }

		template <typename T>
		ND_ auto&				As ()							C_NE___	{ NonNull( _next );  return *Cast<T>( _next ); }
		ND_ VkStructureType		Type ()							C_NE___	{ NonNull( _next );  return _next->sType; }

		ND_ bool				operator != (std::nullptr_t)	C_NE___	{ return _next != null; }
		ND_ auto const&			operator * ()					C_NE___	{ return *this; }

		ND_ InitialType*		operator -> ()					C_NE___	{ NonNull( _next );  return _next; }

		VNextIterator&			operator ++ ()					__NE___
		{
			NonNull( _next );
			_next = _next->pNext;
			return *this;
		}
	};


	template <typename InitialType>
	struct VNextRange
	{
	private:
		using Struct_t	= Conditional< IsMutableVkPNext<InitialType>, VkBaseOutStructure,
							Conditional< IsConstVkPNext<InitialType>, const VkBaseInStructure, void >>;
		StaticAssert( not IsSame< Struct_t, void >);

		using Iterator = VNextIterator< Struct_t >;

	private:
		Iterator	_begin;

	public:
		template <typename T>
		explicit VNextRange (T &st)			__NE___ : _begin{ st } {}

		ND_ Iterator		begin ()		C_NE___	{ return _begin; }
		ND_ std::nullptr_t	end ()			C_NE___	{ return {}; }
	};


	template <typename T>
	VNextRange (T &) -> VNextRange<T>;


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
