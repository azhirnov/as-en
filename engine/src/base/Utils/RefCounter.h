// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/Ptr.h"
#include "base/Algorithms/Cast.h"
#include "base/Memory/MemUtils.h"

namespace AE::Base
{
	template <typename T>
	struct RC;

	
	//
	// Enable Reference Counting
	//

	template <typename T>
	struct EnableRC
	{
		template <typename B>
		friend struct RC;
		
		template <typename B>
		friend struct StaticRC;

	// types
	private:
		using _EnableRC_t = EnableRC< T >;


	// variables
	private:
		std::atomic<int>	_counter {0};

		STATIC_ASSERT( decltype(_counter)::is_always_lock_free );


	// methods
	public:
		EnableRC () {}
		virtual ~EnableRC () { ASSERT( _counter.load( std::memory_order_relaxed ) == 0 ); }

		EnableRC (EnableRC<T> &&) = delete;
		EnableRC (const EnableRC<T> &) = delete;

		EnableRC<T>&  operator = (EnableRC<T> &&) = delete;
		EnableRC<T>&  operator = (const EnableRC<T> &) = delete;

		ND_ RC<T>  GetRC ();
		
		template <typename B>
		ND_ RC<B>  GetRC ();

	protected:
		virtual void  _ReleaseObject ();
	};



	//
	// Reference Counter Pointer
	//

	template <typename T>
	struct RC
	{
	// types
	public:
		using Value_t	= T;


	// variables
	private:
		T *		_ptr = null;


	// methods
	public:
		RC () {}
		RC (std::nullptr_t) {}

		RC (T* ptr) : _ptr{ptr}							{ _Inc(); }
		RC (Ptr<T> ptr) : _ptr{ptr}						{ _Inc(); }
		RC (RC<T> &&other) : _ptr{other.release()}		{}
		RC (const RC<T> &other) : _ptr{other._ptr}		{ _Inc(); }
		
		template <typename B>
		RC (RC<B> &&other) : _ptr{static_cast<T*>(other.release())} {}

		template <typename B>
		RC (const RC<B> &other) : _ptr{static_cast<T*>(other.get())} { _Inc(); }

		~RC ()											{ _Dec(); }

		RC&  operator = (std::nullptr_t)				{ _Dec();  _ptr = null;                return *this; }
		RC&  operator = (T* rhs)						{ _Dec();  _ptr = rhs;        _Inc();  return *this; }
		RC&  operator = (Ptr<T> rhs)					{ _Dec();  _ptr = rhs.get();  _Inc();  return *this; }
		RC&  operator = (const RC<T> &rhs)				{ _Dec();  _ptr = rhs._ptr;   _Inc();  return *this; }
		RC&  operator = (RC<T> &&rhs)					{ _Dec();  _ptr = rhs.release();       return *this; }

		template <typename B, typename = EnableIf<IsBaseOf<T,B>> >
		RC&  operator = (RC<B> &&rhs)					{ _Dec();  _ptr = static_cast<T*>(rhs.release());       return *this; }
		
		template <typename B, typename = EnableIf<IsBaseOf<T,B>> >
		RC&  operator = (const RC<B> &&rhs)				{ _Dec();  _ptr = static_cast<T*>(rhs.get());  _Inc();  return *this; }

		ND_ bool  operator == (const T* rhs)	 const	{ return _ptr == rhs; }
		ND_ bool  operator == (Ptr<T> rhs)		 const	{ return _ptr == rhs.get(); }
		ND_ bool  operator == (const RC<T> &rhs) const	{ return _ptr == rhs._ptr; }
		ND_ bool  operator == (std::nullptr_t)	 const	{ return _ptr == null; }

		template <typename B>
		ND_ bool  operator != (const B& rhs)	const	{ return not (*this == rhs); }

		ND_ bool  operator <  (const RC<T> &rhs) const	{ return _ptr <  rhs._ptr; }
		ND_ bool  operator >  (const RC<T> &rhs) const	{ return _ptr >  rhs._ptr; }
		ND_ bool  operator <= (const RC<T> &rhs) const	{ return _ptr <= rhs._ptr; }
		ND_ bool  operator >= (const RC<T> &rhs) const	{ return _ptr >= rhs._ptr; }

		ND_ T *		operator -> ()				const	{ ASSERT( _ptr );  return _ptr; }
		ND_ T &		operator *  ()				const	{ ASSERT( _ptr );  return *_ptr; }

		ND_ T *		get ()						const	{ return _ptr; }
		ND_ T *		release ()							{ T* p = _ptr;  _ptr = null;  return p; }
		ND_ int		use_count ()				const	{ return _ptr ? _ptr->_counter.load( std::memory_order_relaxed ) : 0; }

		ND_ explicit operator bool ()			const	{ return _ptr != null; }

			void	attach (T* ptr)						{ _Dec();  _ptr = ptr; }
			void	reset (T* ptr)						{ _Dec();  _ptr = ptr;  _Inc(); }

			void	Swap (INOUT RC<T> &rhs);

	private:
		void  _Inc ();
		void  _Dec ();
	};

	

	//
	// Static Reference Counter
	//

	template <typename T>
	struct StaticRC
	{
		template <typename ...Args>
		static void  New (T& obj, Args&& ...args)
		{
			PlacementNew<T>( &obj, FwdArg<Args>( args )... );
			obj._counter.store( 1, std::memory_order_relaxed );
		}

		static void  Delete (INOUT T& obj)
		{
			ASSERT( obj._counter.fetch_sub( 1, std::memory_order_relaxed ) == 1 );
			PlacementDelete( INOUT obj );
		}
	};



	//
	// Reference Counter Atomic Pointer
	//
	
	template <typename T>
	struct AtomicRC
	{
	// variables
	private:
		std::atomic< T *>	_ptr {null};


	// methods
	public:
		AtomicRC () {}
		AtomicRC (std::nullptr_t) {}

		AtomicRC (T* ptr)							{ _IncSet( ptr ); }
		AtomicRC (Ptr<T> ptr)						{ _IncSet( ptr.get() ); }
		AtomicRC (RC<T> &&ptr)						{ _ptr.store( ptr.release(), std::memory_order_relaxed ); }
		AtomicRC (const RC<T> &ptr)					{ _IncSet( ptr.get() ); }
		AtomicRC (AtomicRC<T> &&other)				{ _ptr.store( other._ptr.exchange( null, std::memory_order_relaxed ), std::memory_order_relaxed ); }
		AtomicRC (const AtomicRC<T> &other)			{ _IncSet( other._ptr.load( std::memory_order_relaxed )); }
		
		~AtomicRC ()								{ _ResetDec(); }
		
		ND_ T *		unsafe_get ()			const	{ return _ptr.load( std::memory_order_relaxed ); }
		ND_ T *		release ()						{ return _ptr.exchange( null, std::memory_order_relaxed ); }

	private:
		void  _IncSet (T *ptr);
		void  _IncSetDec (T *ptr);
		void  _ResetDec ();
	};


	
	namespace _hidden_
	{
		template <typename T>
		struct _RemoveRC {
			using type = T;
		};

		template <typename T>
		struct _RemoveRC< RC<T> > {
			using type = T;
		};

	}	// _hidden_

	template <typename T>
	using RemoveRC	= typename Base::_hidden_::_RemoveRC<T>::type;

	template <typename T>
	static constexpr bool	IsRC = IsSpecializationOf< T, RC >;

//-----------------------------------------------------------------------------


/*
=================================================
	MakeRC
=================================================
*/
	template <typename T, typename ...Args>
	ND_ forceinline RC<T>  MakeRC (Args&& ...args)
	{
		return RC<T>{ new T{ FwdArg<Args>(args)... }};
	}
	
/*
=================================================
	_Inc
=================================================
*/
	template <typename T>
	forceinline void  RC<T>::_Inc ()
	{
		if_likely( _ptr != null )
			_ptr->_counter.fetch_add( 1, std::memory_order_relaxed );
	}
	
/*
=================================================
	_Dec
=================================================
*/
	template <typename T>
	forceinline void  RC<T>::_Dec ()
	{
		if_likely( _ptr != null )
		{
			const auto	res = _ptr->_counter.fetch_sub( 1, std::memory_order_release );
			ASSERT( res > 0 );

			if_unlikely( res == 1 )
			{
				static_cast< typename T::_EnableRC_t *>( _ptr )->_ReleaseObject();
				_ptr = null;
			}
		}
	}
	
/*
=================================================
	Swap
=================================================
*/
	template <typename T>
	forceinline void  RC<T>::Swap (INOUT RC<T> &rhs)
	{
		std::swap( _ptr, rhs._ptr );
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	_IncSet
=================================================
*
	template <typename T>
	forceinline void  AtomicRC<T>::_IncSet (T *ptr)
	{
		if ( ptr )
			ptr->AddRef();

		_ptr.store( ptr );
	}
	
/*
=================================================
	_IncSetDec
=================================================
*
	template <typename T>
	forceinline void  AtomicRC<T>::_IncSetDec (T *ptr)
	{
		if ( ptr )
			ptr->AddRef();

		T*	old = _ptr.exchange( ptr, std::memory_order_relaxed );

		if ( old )
			old->ReleaseRef();
	}
	
/*
=================================================
	_ResetDec
=================================================
*
	template <typename T>
	forceinline void  AtomicRC<T>::_ResetDec ()
	{
		T*	old = _ptr.exchange( null, std::memory_order_relaxed );
		if ( old )
			old->ReleaseRef();
	}
//-----------------------------------------------------------------------------


/*
=================================================
	GetRC
=================================================
*/
	template <typename T>
	forceinline RC<T>  EnableRC<T>::GetRC ()
	{
		return RC<T>{ static_cast<T*>(this) };
	}
	
	template <typename T>
	template <typename B>
	forceinline RC<B>  EnableRC<T>::GetRC ()
	{
		return RC<B>{ static_cast<B*>(this) };
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	template <typename T>
	void  EnableRC<T>::_ReleaseObject ()
	{
		std::atomic_thread_fence( std::memory_order_acquire );
		delete this;
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	Cast
=================================================
*/
	template <typename R, typename T>
	ND_ forceinline constexpr RC<R>  Cast (const RC<T> &value)
	{
		return RC<R>{ static_cast<R*>( value.get() )};
	}
	
/*
=================================================
	DynCast
=================================================
*/
#ifdef AE_ENABLE_RTTI
	template <typename R, typename T>
	ND_ forceinline constexpr RC<R>  DynCast (const RC<T> &value)
	{
		return RC<R>{ dynamic_cast<R*>( value.get() )};
	}
#endif

}	// AE::Base
