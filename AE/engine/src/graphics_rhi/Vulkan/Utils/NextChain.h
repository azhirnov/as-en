// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
	// Next Chain (setter for Vulkan 'pNext' field)
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


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
