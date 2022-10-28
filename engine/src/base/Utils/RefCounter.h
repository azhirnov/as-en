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
		using Self		= RC<T>;


	// variables
	private:
		T *		_ptr = null;


	// methods
	public:
		RC () {}
		RC (std::nullptr_t) {}

		RC (T* ptr) : _ptr{ptr}							{ _Inc(); }
		RC (Ptr<T> ptr) : _ptr{ptr}						{ _Inc(); }
		RC (Self &&other) : _ptr{other.release()}		{}
		RC (const Self &other) : _ptr{other._ptr}		{ _Inc(); }
		
		template <typename B>
		RC (RC<B> &&other) : _ptr{static_cast<T*>(other.release())} {}

		template <typename B>
		RC (const RC<B> &other) : _ptr{static_cast<T*>(other.get())} { _Inc(); }

		~RC ()											{ _Dec(); }

		Self&  operator = (std::nullptr_t)				{ _Dec();  _ptr = null;                return *this; }
		Self&  operator = (T* rhs)						{ _Dec();  _ptr = rhs;        _Inc();  return *this; }
		Self&  operator = (Ptr<T> rhs)					{ _Dec();  _ptr = rhs.get();  _Inc();  return *this; }
		Self&  operator = (const Self &rhs)				{ _Dec();  _ptr = rhs._ptr;   _Inc();  return *this; }
		Self&  operator = (Self &&rhs)					{ _Dec();  _ptr = rhs.release();       return *this; }

		template <typename B, typename = EnableIf<IsBaseOf<T,B>> >
		Self&  operator = (RC<B> &&rhs)					{ _Dec();  _ptr = static_cast<T*>(rhs.release());       return *this; }
		
		template <typename B, typename = EnableIf<IsBaseOf<T,B>> >
		Self&  operator = (const RC<B> &&rhs)			{ _Dec();  _ptr = static_cast<T*>(rhs.get());  _Inc();  return *this; }

		ND_ bool  operator == (const T* rhs)	const	{ return _ptr == rhs; }
		ND_ bool  operator == (Ptr<T> rhs)		const	{ return _ptr == rhs.get(); }
		ND_ bool  operator == (const Self &rhs)	const	{ return _ptr == rhs._ptr; }
		ND_ bool  operator == (std::nullptr_t)	const	{ return _ptr == null; }

		template <typename B>
		ND_ bool  operator != (const B& rhs)	const	{ return not (*this == rhs); }

		ND_ bool  operator <  (const Self &rhs)	const	{ return _ptr <  rhs._ptr; }
		ND_ bool  operator >  (const Self &rhs)	const	{ return _ptr >  rhs._ptr; }
		ND_ bool  operator <= (const Self &rhs)	const	{ return _ptr <= rhs._ptr; }
		ND_ bool  operator >= (const Self &rhs)	const	{ return _ptr >= rhs._ptr; }

		ND_ T *		operator -> ()				const	{ ASSERT( _ptr );  return _ptr; }
		ND_ T &		operator *  ()				const	{ ASSERT( _ptr );  return *_ptr; }

		ND_ T *		get ()						const	{ return _ptr; }
		ND_ T *		release ()							{ T* p = _ptr;  _ptr = null;  return p; }
		ND_ int		use_count ()				const	{ return _ptr ? _ptr->_counter.load( std::memory_order_relaxed ) : 0; }

		ND_ explicit operator bool ()			const	{ return _ptr != null; }

			void	attach (T* ptr)						{ _Dec();  _ptr = ptr; }
			void	reset (T* ptr)						{ _Dec();  _ptr = ptr;  _Inc(); }

			void	Swap (INOUT Self &rhs);

	private:
		void  _Inc ();
		void  _Dec ();
	};

	

	//
	// Static Reference Counter
	//

	template <typename T>
	struct StaticRC final : Noninstancable
	{
		template <typename ...Args>
		static void  New (INOUT T& obj, Args&& ...args)
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
		AtomicRC () {}
		AtomicRC (std::nullptr_t) {}

		AtomicRC (T* ptr)							{ _IncSet( ptr ); }
		AtomicRC (Ptr<T> ptr)						{ _IncSet( ptr.get() ); }
		AtomicRC (RC<T> &&ptr)						{ _ptr.store( ptr.release().release(), std::memory_order_relaxed ); }
		AtomicRC (const RC<T> &ptr)					{ _IncSet( ptr.get() ); }

		~AtomicRC ()								{ _ResetDec(); }
		
		ND_ T *		unsafe_get ()			const	{ return _RemoveLockBit( _ptr.load( std::memory_order_relaxed )); }
		ND_ RC_t	release ();

		ND_ RC_t	get ();

			void	reset (T* ptr);

		Self&  operator = (std::nullptr_t)			{ _ResetDec();				return *this; }
		Self&  operator = (T* rhs)					{ reset( rhs );				return *this; }
		Self&  operator = (Ptr<T> rhs)				{ reset( rhs );				return *this; }
		Self&  operator = (const RC<T> &rhs)		{ reset( rhs.get() );		return *this; }
		Self&  operator = (RC<T> &&rhs);

	private:
		void  _IncSet (T *ptr);
		void  _ResetDec ();
		
		ND_ T*		_Lock ();
			void	_Unlock ();
		ND_ T*		_Exchange (T* ptr);

		static void  _Inc (T *ptr);
		static void  _Dec (T *ptr);

		ND_ static bool  _HasLockBit (T* ptr)
		{
			return (usize(ptr) & usize{1});
		}

		ND_ static T*  _SetLockBit (T* ptr)
		{
			return reinterpret_cast< T *>((usize(ptr) | usize{1}));
		}

		ND_ static T*  _RemoveLockBit (T* ptr)
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
	ND_ forceinline RC<T>  MakeRC (Args&& ...args)
	{
		STATIC_ASSERT( not IsBaseOf< NonAllocatable, T >);

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
	forceinline void  RC<T>::Swap (INOUT RC<T> &rhs)
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
	ND_ AtomicRC<T>  MakeAtomicRC (Args&& ...args)
	{
		return AtomicRC<T>{ new T{ FwdArg<Args>(args)... }};
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	AtomicRC<T>&  AtomicRC<T>::operator = (RC<T> &&rhs)
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
	void  AtomicRC<T>::_Inc (T *ptr)
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
	void  AtomicRC<T>::_Dec (T *ptr)
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
	void  AtomicRC<T>::_IncSet (T *ptr)
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
	void  AtomicRC<T>::_ResetDec ()
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
	RC<T>  AtomicRC<T>::release ()
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
	RC<T>  AtomicRC<T>::get ()
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
	void  AtomicRC<T>::reset (T* ptr)
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
	T*  AtomicRC<T>::_Exchange (T* ptr)
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
	T*  AtomicRC<T>::_Lock ()
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
	void  AtomicRC<T>::_Unlock ()
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

} // AE::Base
