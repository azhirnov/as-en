// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/NtStringView.h"

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
		Object () = default;
		Object (const Object& other);
		Object (Object && other);

		Object& operator = (const Object& rhs);
		Object& operator = (Object && rhs);

		virtual ~Object ();

		ND_ explicit operator bool () const	{ return _ptr != null; }

		ND_ const void* Ptr ()			const	{ ASSERT(_ptr); return _ptr; }
		ND_ const void* PtrOrNull ()	const	{ return _ptr; }
			
		ND_ UInteger	RetainCount ();
			void		Retain (const void* ptr);
			void		Attach (const void* ptr);
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
		ObjectRef () {}
		ObjectRef (const ObjectRef &) = default;
		ObjectRef (std::nullptr_t) {}

		ObjectRef&  operator = (const ObjectRef &) = default;

		ND_ bool  operator == (const ObjectRef &rhs) const	{ return _ptr == rhs._ptr; }
		ND_ bool  operator != (const ObjectRef &rhs) const	{ return _ptr != rhs._ptr; }

		ND_ explicit		operator bool ()		 const	{ return _ptr != null; }

		ND_ const void *	Ptr ()					 const	{ ASSERT(_ptr); return _ptr; }
		ND_ const void *	PtrOrNull ()			 const	{ return _ptr; }
		
			void			Attach (const void* ptr)		{ ASSERT( _ptr == null );  _ptr = ptr; }

	protected:
		void  _AddRef ();
		void  _ReleaseRef ();
		void  _Retain (const void* ptr);
		
		ND_ UInteger  _RetainCount ();
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
		ObjStrongPtr () : _ref{}							{}
		ObjStrongPtr (std::nullptr_t) 						{}
		explicit ObjStrongPtr (T ref) : _ref{ref}			{ _ref._AddRef(); }
		ObjStrongPtr (const Self &other) : _ref{other._ref}	{ _ref._AddRef(); }
		ObjStrongPtr (Self &&other) : _ref{other._ref}		{ other._ref = {}; }
		~ObjStrongPtr ()									{ _ref._ReleaseRef(); }

		Self&  operator = (const Self &rhs)					{ _ref._ReleaseRef();  _ref = rhs._ref;  _ref._AddRef();  return *this; }
		Self&  operator = (Self &&rhs)						{ _ref._ReleaseRef();  _ref = rhs._ref;  rhs._ref = {};   return *this; }
		Self&  operator = (const T &rhs)					{ _ref._ReleaseRef();  _ref = rhs;		 _ref._AddRef();  return *this; }
		Self&  operator = (std::nullptr_t)					{ _ref._ReleaseRef();  return *this; }
		
		ND_ bool  operator == (const Self &rhs) const		{ return _ref == rhs._ref; }
		ND_ bool  operator != (const Self &rhs) const		{ return _ref != rhs._ref; }

		ND_ T*	  operator -> ()				const		{ return &_ref; }
		ND_ T&	  Get ()						const		{ return _ref; }

		ND_ operator T ()						const		{ return _ref; }
		ND_ explicit operator bool ()			const		{ return bool(_ref); }
		
		ND_ const void*  Ptr ()					const		{ return _ref.Ptr(); }
		ND_ const void*  PtrOrNull ()			const		{ return _ref.PtrOrNull(); }
		
		ND_ UInteger	 RetainCount ()			const		{ return _ref._RetainCount(); }

			void  Attach (const void* ptr)		const		{ _ref._ReleaseRef();  _ref.Attach( ptr ); }
			void  Retain (const void* ptr)		const		{ _ref._ReleaseRef();  _ref._Retain( ptr ); }
	};

	
	namespace _hidden_
	{
		//
		// ArrayBase
		//
		class ArrayBase : public Object
		{
		// methods
		public:
			ND_ UInteger  size () const;

		protected:
			ND_ void*  GetItem (UInteger index) const;
			
				template <typename T>
				void  SetItem (UInteger index, void* obj);
		};
	}


	//
	// NSArray
	//
	template<typename T>
	class Array final : public _hidden_::ArrayBase
	{
	// methods
	public:
		ND_ const T  operator [] (UInteger index) const
		{
			T	result;
			result.Retain( GetItem( index ));
			return result;
		}

		ND_ T  operator [] (UInteger index)
		{
			T	result;
			result.Retain( GetItem( index ));
			return result;
		}
	};



	//
	// NSString
	//
	class String final : public Object
	{
	// methods
	public:
		String () {}
		explicit String (const char* cstr);
		explicit String (Base::NtStringView view) : String{view.c_str()} {}

		ND_ const char*	 c_str ()	const;
		ND_ UInteger	 size ()	const;
	};



	//
	// NSURL
	//
	class URL final : public Object
	{
	// methods
	public:
		URL () {}
		explicit URL (const char* path);
		explicit URL (const Base::Path &path);
	};


} // AE::NS
