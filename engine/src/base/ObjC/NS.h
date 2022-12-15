// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/FileSystem.h"

namespace AE::NS
{
	using UInteger	= usize;
	

	//
	// NSObject smart pointer
	//
	class Object
	{
	// variables
	private:
		const void *	_ptr	= null;


	// methods
	public:
		Object ()									__NE___	= default;
		Object (const Object& other)				__NE___;
		Object (Object && other)					__NE___;

		Object& operator = (const Object& rhs)		__NE___;
		Object& operator = (Object && rhs)			__NE___;

		virtual ~Object ()							__NE___;

		ND_ explicit	operator bool ()			C_NE___	{ return _ptr != null; }

		ND_ const void* Ptr ()						C_NE___	{ ASSERT( _ptr != null ); return _ptr; }
		ND_ const void* PtrOrNull ()				C_NE___	{ return _ptr; }
			
		ND_ UInteger	RetainCount ()				__NE___;
			void		Retain (const void* ptr)	__NE___;
			void		Attach (const void* ptr)	__NE___;
	};
	


	//
	// NSObject raw pointer
	//
	class ObjectRef
	{
		template <typename T> friend class ObjStrongPtr;
		
	// variables
	private:
		const void *	_ptr	= null;
		

	// methods
	public:
		ObjectRef ()									__NE___	{}
		ObjectRef (const ObjectRef &)					__NE___	= default;
		ObjectRef (std::nullptr_t)						__NE___	{}

		ObjectRef&  operator = (const ObjectRef &)		__NE___	= default;

		ND_ bool  operator == (const ObjectRef &rhs)	C_NE___	{ return _ptr == rhs._ptr; }
		ND_ bool  operator != (const ObjectRef &rhs)	C_NE___	{ return _ptr != rhs._ptr; }
		ND_ bool  operator <  (const ObjectRef &rhs)	C_NE___	{ return _ptr <  rhs._ptr; }
		ND_ bool  operator >  (const ObjectRef &rhs)	C_NE___	{ return _ptr >  rhs._ptr; }
		ND_ bool  operator <= (const ObjectRef &rhs)	C_NE___	{ return _ptr <= rhs._ptr; }
		ND_ bool  operator >= (const ObjectRef &rhs)	C_NE___	{ return _ptr >= rhs._ptr; }

		ND_ explicit		operator bool ()			C_NE___	{ return _ptr != null; }

		ND_ const void *	Ptr ()						C_NE___	{ ASSERT( _ptr != null );  return _ptr; }
		ND_ const void *	PtrOrNull ()				C_NE___	{ return _ptr; }
		
			void			Attach (const void* ptr)	__NE___	{ ASSERT( _ptr == null );  _ptr = ptr; }

	protected:
		void  _AddRef ()								__NE___;
		void  _ReleaseRef ()							__NE___;
		void  _Retain (const void* ptr)					__NE___;

		ND_ const void*	_Release ()						__NE___	{ const void* p = _ptr;  _ptr = null;  return p; }
		ND_ UInteger	_RetainCount ()					__NE___;
	};



	//
	// NSObject strong pointer
	//
	template <typename T>
	class ObjStrongPtr
	{
	// types
	public:
		STATIC_ASSERT( Base::IsBaseOf< ObjectRef, T >);

		using Self = ObjStrongPtr<T>;

		
	// variables
	private:
		mutable T	_ref;
		

	// methods
	public:
		ObjStrongPtr ()									__NE___ : _ref{}			{}
		ObjStrongPtr (std::nullptr_t) 					__NE___						{}
		explicit ObjStrongPtr (T ref)					__NE___ : _ref{ref}			{ _ref._AddRef(); }
		explicit ObjStrongPtr (const void* ptr)			__NE___ : _ref{T{ptr}}		{ _ref._AddRef(); }
		ObjStrongPtr (const Self &other)				__NE___ : _ref{other._ref}	{ _ref._AddRef(); }
		ObjStrongPtr (Self &&other)						__NE___ : _ref{other._ref}	{ other._ref = {}; }
		~ObjStrongPtr ()								__NE___						{ _ref._ReleaseRef(); }

		Self&  operator = (const Self &rhs)				__NE___	{ _ref._ReleaseRef();  _ref = rhs._ref;  _ref._AddRef();  return *this; }
		Self&  operator = (Self &&rhs)					__NE___	{ _ref._ReleaseRef();  _ref = rhs._ref;  rhs._ref = {};   return *this; }
		Self&  operator = (const T &rhs)				__NE___	{ _ref._ReleaseRef();  _ref = rhs;		 _ref._AddRef();  return *this; }
		Self&  operator = (std::nullptr_t)				__NE___	{ _ref._ReleaseRef();  return *this; }
		
		ND_ bool  operator == (const Self &rhs)			C_NE___	{ return _ref == rhs._ref; }
		ND_ bool  operator != (const Self &rhs)			C_NE___	{ return _ref != rhs._ref; }

		ND_ T*	  operator -> ()						C_NE___	{ return &_ref; }
		ND_ T&	  Get ()								C_NE___	{ return _ref; }

		ND_ operator T ()								C_NE___	{ return _ref; }
		ND_ explicit operator bool ()					C_NE___	{ return bool(_ref); }
		
		ND_ const void*  Ptr ()							C_NE___	{ return _ref.Ptr(); }
		ND_ const void*  PtrOrNull ()					C_NE___	{ return _ref.PtrOrNull(); }
		ND_ const void*	 ReleaseAsPtr ()				__NE___	{ return _ref._Release(); }
		ND_ T			 Release ()						__NE___	{ T tmp = _ref;  _ref = null;  return tmp; }
		
		ND_ UInteger	 RetainCount ()					C_NE___	{ return _ref._RetainCount(); }

			void  Attach (const void* ptr)				C_NE___	{ _ref._ReleaseRef();  _ref.Attach( ptr ); }
			void  Retain (const void* ptr)				C_NE___	{ _ref._ReleaseRef();  _ref._Retain( ptr ); }
	};

	
	namespace _hidden_
	{
		//
		// Array Base
		//
		class ArrayBase : public Object
		{
		// methods
		public:
			ND_ UInteger size ()						C_NE___;

		protected:
			ND_ void*	 GetItem (UInteger index)		C_NE___;
		};
		
		
		//
		// Mutable Array Base
		//
		class MutableArrayBase : public Object
		{
		// methods
		public:
			ND_ UInteger size ()						C_NE___;

		protected:
			ND_ void*	 GetItem (UInteger index)		C_NE___;
			
			template <typename ObjType>
			void  SetItem (UInteger index, void* obj)	__NE___;
		};
	
	} // _hidden_


	//
	// NSArray
	//
	template<typename T>
	class Array final : public NS::_hidden_::ArrayBase
	{
	// methods
	public:
		ND_ const T  operator [] (UInteger index)		C_NE___
		{
			T	result;
			result.Retain( ArrayBase::GetItem( index ));
			return result;
		}

		ND_ T  operator [] (UInteger index)				__NE___
		{
			T	result;
			result.Retain( ArrayBase::GetItem( index ));
			return result;
		}
	};


	//
	// NSMutableArray
	//
	template<typename T>
	class MutableArray final : public NS::_hidden_::MutableArrayBase
	{
	// methods
	public:
		ND_ const T  operator [] (UInteger index)		C_NE___
		{
			T	result;
			result.Retain( MutableArrayBase::GetItem( index ));
			return result;
		}

		ND_ T  operator [] (UInteger index)				__NE___
		{
			T	result;
			result.Retain( MutableArrayBase::GetItem( index ));
			return result;
		}
		
		void  SetItem (UInteger index, const T &obj)	__NE___
		{
			MutableArrayBase::SetItem( index, obj.Ptr() );
		}
	};



	//
	// NSString
	//
	class String final : public Object
	{
	// methods
	public:
		String ()									__NE___	{}
		explicit String (const CharAnsi* cstr)		__NE___	: String{Base::StringView{ cstr }} {}
		explicit String (const CharUtf8* cstr)		__NE___	: String{Base::U8StringView{ cstr }} {}
		explicit String (Base::StringView view)		__NE___;
		explicit String (Base::U8StringView view)	__NE___;
		explicit String (Base::U16StringView view)	__NE___;
		explicit String (Base::U32StringView view)	__NE___;
		
		ND_ const char*			c_str ()			C_NE___	{ return ToAnsi(); }
		ND_ const CharAnsi*		ToAnsi ()			C_NE___;
		ND_ const CharUtf8*		ToUtf8 ()			C_NE___;
		ND_ const CharUtf16*	ToUtf16 ()			C_NE___;
		ND_ const CharUtf32*	ToUtf32 ()			C_NE___;
		ND_ UInteger			LengthOfUtf16 ()	C_NE___;

		ND_ Base::String		ToAnsiString ()		C_NE___;
		ND_ Base::U8String		ToUtf8String ()		C_NE___;
		ND_ Base::U16String		ToUtf16String ()	C_NE___;
		ND_ Base::U32String		ToUtf32String ()	C_NE___;
		
		ND_ operator Base::StringView ()			C_NE___	{ return Base::StringView{ c_str(), LengthOfUtf16() }; }
	};



	//
	// NSURL
	//
	class URL final : public Object
	{
	// methods
	public:
		URL ()										__NE___	{}
		explicit URL (const char* path)				__NE___;
		explicit URL (const Base::Path &path)		__NE___;
	};


} // AE::NS
