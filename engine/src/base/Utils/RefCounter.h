// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/Ptr.h"
#include "base/Algorithms/Cast.h"
#include "base/Memory/MemUtils.h"
#include "base/Platforms/ThreadUtils.h"

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
		
		template <typename B>
		friend struct AtomicRC;

	// types
	private:
		using _EnableRC_t = EnableRC< T >;


	// variables
	private:
		std::atomic<int>	_counter {0};

		STATIC_ASSERT( decltype(_counter)::is_always_lock_free );


	// methods
	public:
		EnableRC ()						__NE___ {}
		virtual ~EnableRC ()			__NE___ { ASSERT( _GetRC_UseCount() == 0 ); }

		EnableRC (EnableRC<T> &&)						= delete;
		EnableRC (const EnableRC<T> &)					= delete;

		EnableRC<T>&  operator = (EnableRC<T> &&)		= delete;
		EnableRC<T>&  operator = (const EnableRC<T> &)	= delete;

		ND_ RC<T>  GetRC ()				__NE___;
		
		template <typename B>
		ND_ RC<B>  GetRC ()				__NE___;

	protected:
		// this methods allows to catch object destruction and change bechavior,
		// for example - add back to object pool.
		virtual void  _ReleaseObject () __NE___;

		ND_ int  _GetRC_UseCount ()		C_NE___	{ return _counter.load( std::memory_order_relaxed ); }
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
		using Self		= RC<T>;


	// variables
	private:
		T *		_ptr = null;


	// methods
	public:
		RC ()											__NE___ {}
		RC (std::nullptr_t)								__NE___ {}

		RC (T* ptr)										__NE___ : _ptr{ptr}				{ _Inc(); }
		RC (Ptr<T> ptr)									__NE___ : _ptr{ptr}				{ _Inc(); }
		RC (Self &&other)								__NE___ : _ptr{other.release()}	{}
		RC (const Self &other)							__NE___ : _ptr{other._ptr}		{ _Inc(); }
		
		template <typename B>	RC (RC<B> &&other)		__NE___ : _ptr{static_cast<T*>(other.release())} {}

		template <typename B>	RC (const RC<B> &other)	__NE___ : _ptr{static_cast<T*>(other.get())} { _Inc(); }

		~RC ()											__NE___ { _Dec(); }

		Self&  operator = (std::nullptr_t)				__NE___ { _Dec();  _ptr = null;                return *this; }
		Self&  operator = (T* rhs)						__NE___ { _Dec();  _ptr = rhs;        _Inc();  return *this; }
		Self&  operator = (Ptr<T> rhs)					__NE___ { _Dec();  _ptr = rhs.get();  _Inc();  return *this; }
		Self&  operator = (const Self &rhs)				__NE___ { _Dec();  _ptr = rhs._ptr;   _Inc();  return *this; }
		Self&  operator = (Self &&rhs)					__NE___ { _Dec();  _ptr = rhs.release();       return *this; }

		template <typename B, typename = EnableIf<IsBaseOf<T,B>> >
		Self&  operator = (RC<B> &&rhs)					__NE___ { _Dec();  _ptr = static_cast<T*>(rhs.release());       return *this; }
		
		template <typename B, typename = EnableIf<IsBaseOf<T,B>> >
		Self&  operator = (const RC<B> &&rhs)			__NE___ { _Dec();  _ptr = static_cast<T*>(rhs.get());  _Inc();  return *this; }

		ND_ bool  operator == (const T* rhs)			C_NE___ { return _ptr == rhs; }
		ND_ bool  operator == (Ptr<T> rhs)				C_NE___ { return _ptr == rhs.get(); }
		ND_ bool  operator == (const Self &rhs)			C_NE___ { return _ptr == rhs._ptr; }
		ND_ bool  operator == (std::nullptr_t)			C_NE___ { return _ptr == null; }

		template <typename B>
		ND_ bool  operator != (const B& rhs)			C_NE___ { return not (*this == rhs); }

		ND_ bool  operator <  (const Self &rhs)			C_NE___ { return _ptr <  rhs._ptr; }
		ND_ bool  operator >  (const Self &rhs)			C_NE___ { return _ptr >  rhs._ptr; }
		ND_ bool  operator <= (const Self &rhs)			C_NE___ { return _ptr <= rhs._ptr; }
		ND_ bool  operator >= (const Self &rhs)			C_NE___ { return _ptr >= rhs._ptr; }

		ND_ T *		operator -> ()						C_NE___ { ASSERT( _ptr != null );  return _ptr; }
		ND_ T &		operator *  ()						C_NE___	{ ASSERT( _ptr != null );  return *_ptr; }

		ND_ T *		get ()								C_NE___ { return _ptr; }
		ND_ T *		release ()							__NE___ { T* p = _ptr;  _ptr = null;  return p; }
		ND_ int		use_count ()						C_NE___ { return _ptr != null ? _ptr->_counter.load( std::memory_order_relaxed ) : 0; }

		ND_ explicit operator bool ()					C_NE___ { return _ptr != null; }

			void	attach (T* ptr)						__NE___ { _Dec();  _ptr = ptr; }
			void	reset (T* ptr)						__NE___ { _Dec();  _ptr = ptr;  _Inc(); }

			void	Swap (INOUT Self &rhs)				__NE___;

	private:
			void	_Inc ()								__NE___;
			void	_Dec ()								__NE___;
	};

	

	//
	// Static Reference Counter
	//

	template <typename T>
	struct StaticRC final : Noninstancable
	{
		template <typename ...Args>
		static void  New (INOUT T& obj, Args&& ...args)	__TH___
		{
			PlacementNew<T>( &obj, FwdArg<Args>( args )... );	// throw
			obj._counter.store( 1, std::memory_order_relaxed );
		}

		static void  Delete (INOUT T& obj)				__NE___
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
	// types
	public:
		using Value_t	= T;
		using RC_t		= RC<T>;
		using Self		= AtomicRC<T>;


	// variables
	private:
		std::atomic< T *>	_ptr {null};


	// methods
	public:
		AtomicRC ()									__NE___ {}
		AtomicRC (std::nullptr_t)					__NE___ {}

		AtomicRC (T* ptr)							__NE___ { _IncSet( ptr ); }
		AtomicRC (Ptr<T> ptr)						__NE___ { _IncSet( ptr.get() ); }
		AtomicRC (RC<T> &&ptr)						__NE___ { _ptr.store( ptr.release().release(), std::memory_order_relaxed ); }
		AtomicRC (const RC<T> &ptr)					__NE___ { _IncSet( ptr.get() ); }

		~AtomicRC ()								__NE___ { _ResetDec(); }
		
		ND_ T *		unsafe_get ()					C_NE___ { return _RemoveLockBit( _ptr.load( std::memory_order_relaxed )); }
		ND_ RC_t	release ()						__NE___;

		ND_ RC_t	get ()							__NE___;

			void	reset (T* ptr)					__NE___;

		Self&  operator = (std::nullptr_t)			__NE___ { _ResetDec();			return *this; }
		Self&  operator = (T* rhs)					__NE___ { reset( rhs );			return *this; }
		Self&  operator = (Ptr<T> rhs)				__NE___ { reset( rhs );			return *this; }
		Self&  operator = (const RC<T> &rhs)		__NE___ { reset( rhs.get() );	return *this; }
		Self&  operator = (RC<T> &&rhs)				__NE___;

	private:
		void  _IncSet (T *ptr)						__NE___;
		void  _ResetDec ()							__NE___;
		
		ND_ T*		_Lock ()						__NE___;
			void	_Unlock ()						__NE___;
		ND_ T*		_Exchange (T* ptr)				__NE___;

		static void  _Inc (T *ptr)					__NE___;
		static void  _Dec (T *ptr)					__NE___;

		ND_ static bool  _HasLockBit (T* ptr)		__NE___
		{
			return (usize(ptr) & usize{1});
		}

		ND_ static T*  _SetLockBit (T* ptr)			__NE___
		{
			return reinterpret_cast< T *>((usize(ptr) | usize{1}));
		}

		ND_ static T*  _RemoveLockBit (T* ptr)		__NE___
		{
			return reinterpret_cast< T *>((usize(ptr) & ~usize{1}));
		}
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
		
		template <typename T>
		struct _RemoveRC< StaticRC<T> > {
			using type = T;
		};
		
		template <typename T>
		struct _RemoveRC< AtomicRC<T> > {
			using type = T;
		};

	} // _hidden_

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
	ND_ forceinline RC<T>  MakeRC (Args&& ...args) __TH___
	{
		STATIC_ASSERT( not IsBaseOf< NonAllocatable, T >);

		return RC<T>{ new T{ FwdArg<Args>(args)... }};
	}
	
	template <typename T, typename ...Args>
	ND_ forceinline RC<T>  MakeRC_NE (Args&& ...args) __NE___
	{
		STATIC_ASSERT( not IsBaseOf< NonAllocatable, T >);
		try {
			return RC<T>{ new T{ FwdArg<Args>(args)... }};
		}
		catch(...) {
			return null;
		}
	}

/*
=================================================
	_Inc
=================================================
*/
	template <typename T>
	forceinline void  RC<T>::_Inc () __NE___
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
	forceinline void  RC<T>::_Dec () __NE___
	{
		if_likely( _ptr != null )
		{
			const auto	res = _ptr->_counter.fetch_sub( 1, std::memory_order_relaxed );
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
	forceinline void  RC<T>::Swap (INOUT RC<T> &rhs) __NE___
	{
		std::swap( _ptr, rhs._ptr );
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	MakeAtomicRC
=================================================
*/
	template <typename T, typename ...Args>
	ND_ AtomicRC<T>  MakeAtomicRC (Args&& ...args) __TH___
	{
		return AtomicRC<T>{ new T{ FwdArg<Args>(args)... }};
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	AtomicRC<T>&  AtomicRC<T>::operator = (RC<T> &&rhs) __NE___
	{
		_ResetDec();
		
		T*	old = _Exchange( rhs.release() );

		// pointer may be changed in another thread
		_Dec( old );
		return *this;
	}

/*
=================================================
	_Inc
=================================================
*/
	template <typename T>
	void  AtomicRC<T>::_Inc (T *ptr) __NE___
	{
		ASSERT( not _HasLockBit( ptr ));

		if_likely( ptr != null )
			ptr->_counter.fetch_add( 1, std::memory_order_relaxed );
	}
	
/*
=================================================
	_Dec
=================================================
*/
	template <typename T>
	void  AtomicRC<T>::_Dec (T *ptr) __NE___
	{
		ASSERT( not _HasLockBit( ptr ));

		if_likely( ptr != null )
		{
			const auto	res = ptr->_counter.fetch_sub( 1, std::memory_order_relaxed );
			ASSERT( res > 0 );

			if_unlikely( res == 1 )
			{
				static_cast< typename T::_EnableRC_t *>( ptr )->_ReleaseObject();
			}
		}
	}

/*
=================================================
	_IncSet
=================================================
*/
	template <typename T>
	void  AtomicRC<T>::_IncSet (T *ptr) __NE___
	{
		_Inc( ptr );
		
		T*	old = _Exchange( ptr );

		// pointer may be changed in another thread
		_Dec( old );
	}
	
/*
=================================================
	_ResetDec
=================================================
*/
	template <typename T>
	void  AtomicRC<T>::_ResetDec () __NE___
	{
		T*	old = _Exchange( null );
		
		_Dec( old );
	}
	
/*
=================================================
	release
=================================================
*/
	template <typename T>
	RC<T>  AtomicRC<T>::release () __NE___
	{
		T*	old = _Exchange( null );
		ASSERT( not _HasLockBit( old ));

		RC<T>	ptr;
		ptr.attach( old );

		return ptr;
	}
	
/*
=================================================
	get
=================================================
*/
	template <typename T>
	RC<T>  AtomicRC<T>::get () __NE___
	{
		RC<T>	res{ _Lock() };
		_Unlock();
		return res;
	}
	
/*
=================================================
	reset
=================================================
*/
	template <typename T>
	void  AtomicRC<T>::reset (T* ptr) __NE___
	{
		_ResetDec();
		_IncSet( ptr );
	}
	
/*
=================================================
	_Exchange
=================================================
*/
	template <typename T>
	T*  AtomicRC<T>::_Exchange (T* ptr) __NE___
	{
		T*	exp = _RemoveLockBit( _ptr.load( std::memory_order_relaxed ));
		
		for (uint i = 0;
			 not _ptr.compare_exchange_weak( INOUT exp, ptr, std::memory_order_relaxed, std::memory_order_relaxed );
			 ++i)
		{
			if_unlikely( i > ThreadUtils::SpinBeforeLock() )
			{
				i = 0;
				ThreadUtils::Yield();
			}
				
			exp = _RemoveLockBit( exp );
			ThreadUtils::Pause();
		}
		
		ASSERT( not _HasLockBit( exp ));
		return exp;
	}

/*
=================================================
	_Lock
=================================================
*/
	template <typename T>
	T*  AtomicRC<T>::_Lock () __NE___
	{
		T*	exp = _RemoveLockBit( _ptr.load( std::memory_order_relaxed ));
		for (uint i = 0;
			 not _ptr.compare_exchange_weak( INOUT exp, _SetLockBit( exp ), std::memory_order_relaxed, std::memory_order_relaxed );
			 ++i)
		{
			if_unlikely( i > ThreadUtils::SpinBeforeLock() )
			{
				i = 0;
				ThreadUtils::Yield();
			}
				
			exp = _RemoveLockBit( exp );
			ThreadUtils::Pause();
		}

		ASSERT( not _HasLockBit( exp ));
		return exp;
	}
	
/*
=================================================
	_Unlock
=================================================
*/
	template <typename T>
	void  AtomicRC<T>::_Unlock () __NE___
	{
		T*	exp		= _RemoveLockBit( _ptr.load( std::memory_order_relaxed ));
		T*	prev	= _ptr.exchange( exp, std::memory_order_relaxed );
		Unused( prev );
		ASSERT( prev == _SetLockBit( exp ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetRC
=================================================
*/
	template <typename T>
	forceinline RC<T>  EnableRC<T>::GetRC () __NE___
	{
		return RC<T>{ static_cast<T*>(this) };
	}
	
	template <typename T>
	template <typename B>
	forceinline RC<B>  EnableRC<T>::GetRC () __NE___
	{
		return RC<B>{ static_cast<B*>(this) };
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	template <typename T>
	void  EnableRC<T>::_ReleaseObject () __NE___
	{
		// update cache before calling destructor
		std::atomic_thread_fence( std::memory_order_acquire );
		delete this;
		//std::atomic_thread_fence( std::memory_order_release );
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	Cast
=================================================
*/
	template <typename R, typename T>
	ND_ forceinline constexpr RC<R>  Cast (const RC<T> &value) __NE___
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
	ND_ forceinline constexpr RC<R>  DynCast (const RC<T> &value) __NE___
	{
		return RC<R>{ dynamic_cast<R*>( value.get() )};
	}
#endif

} // AE::Base
