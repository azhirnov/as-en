// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Com Pointer
	//

	template <typename T>
	struct ComPtr
	{
	// variables
	private:
		T *		_ptr	= null;


	// methods
	public:
		ComPtr ()											__NE___ {}
		ComPtr (ComPtr<T> &&rhs)							__NE___ : _ptr{rhs._ptr} { rhs._ptr = null; }
		ComPtr (const ComPtr<T> &rhs)						__NE___	: _ptr{rhs._ptr} { _IncRef(); }
		explicit ComPtr (T* ptr)							__NE___ : _ptr{ptr} { _IncRef(); }
		~ComPtr ()											__NE___	{ Release(); }

		ND_ operator T* ()									C_NE___	{ NonNull(_ptr);  return _ptr; }
		ND_ explicit operator bool ()						C_NE___	{ return _ptr != null; }

			void		Release ()							__NE___	{ if ( _ptr != null ) { _ptr->Release();  _ptr = null; } }
			void		Attach (T *ptr)						__NE___	{ ASSERT( ptr != _ptr );  Release();  _ptr = ptr;  _IncRef(); }
		ND_ T*			Detach ()							__NE___	{ T* res = _ptr;  return res; }

		ND_ void**		VRef ()								__NE___	{ return reinterpret_cast< void** >( &_ptr ); }
		ND_ T*			get ()								__NE___	{ NonNull(_ptr);  return _ptr; }
		ND_ T*			GetOrNull ()						__NE___	{ return _ptr; }

			ComPtr&		operator = (const ComPtr<T> &rhs)	__NE___	{ ASSERT( _ptr != rhs._ptr );  Release();  _ptr = rhs._ptr;  _IncRef();	return *this; }
			ComPtr&		operator = (ComPtr<T> &&rhs)		__NE___	{ Release();  _ptr = rhs._ptr;  rhs._ptr = null;	return *this; }
			ComPtr&		operator = (std::nullptr_t)			__NE___	{ Release();										return *this; }
			ComPtr&		operator = (T* ptr)					__NE___	{ Release();  _ptr = ptr;		_IncRef();			return *this; }

		ND_ T&			operator * ()						__NE___	{ NonNull(_ptr);  return *_ptr; }
		ND_ T*			operator -> ()						__NE___	{ NonNull(_ptr);  return _ptr; }

		ND_ T**			operator & ()						__NE___	{ ASSERT( _ptr == null );  return &_ptr; }

	private:
			void  _IncRef ()								__NE___	{ if ( _ptr != null ) _ptr->AddRef(); }
	};

} // AE::Base
