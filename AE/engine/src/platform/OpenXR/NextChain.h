// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_OPENXR

namespace AE::App
{

	//
	// Next Chain (setter for OpenXR 'next' field)
	//

	template <typename InitialType>
	struct XRNextChain
	{
	private:
		using PNext_t = decltype(InitialType::next);

		PNext_t *	_next;

	public:
		template <typename T>
		explicit XRNextChain (T &st)				__NE___
		{
			_next  = &st.next;
			*_next = null;
		}

		template <typename T>
		void  Add (T &st)						__NE___
		{
			*_next = &st;
			 _next = &st.next;
			*_next = null;
		}

		// allow const_cast
		template <typename T>
		void  AddConst (T &st)					__NE___
		{
			*_next = &st;
			 _next = const_cast< const void** >( &st.next );
			*_next = null;
		}
	};


	template <typename T>
	XRNextChain (T &) -> XRNextChain<T>;


} // AE::App

#endif // AE_ENABLE_OPENXR
