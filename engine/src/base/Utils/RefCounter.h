// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/Ptr.h"
#include "base/Algorithms/Cast.h"
#include "base/Memory/MemUtils.h"
#include "base/Platforms/ThreadUtils.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/Threading.h"

namespace AE::Base
{
	template <typename T>
	struct RC;

	struct RefCounterUtils;


	//
	// Enable Reference Counting
	//
	class EnableRCBase : public Noncopyable
	{
		friend struct RefCounterUtils;

	// variables
	private:
		std::atomic<int>	_counter {0};

		STATIC_ASSERT( decltype(_counter)::is_always_lock_free );


	// methods
	public:
		virtual ~EnableRCBase ()		__NE___ { ASSERT( _counter.load( EMemoryOrder::Relaxed ) == 0 ); }

	protected:
		// this methods allows to catch object destruction and change bechavior,
		// for example - add back to object pool.
		virtual void  _ReleaseObject () __NE___
		{
			// update cache before calling destructor
			std::atomic_thread_fence( EMemoryOrder::Acquire );
			delete this;

			// TODO: flush cache depends on allocator - default allocator flush cache because of internal sync, lock-free allocator may not flush cache
			//std::atomic_thread_fence( EMemoryOrder::Release );
		}
	};

	

	//
	// Ref Counter Utils
	//
	struct RefCounterUtils final : Noninstancable
	{
		// returns previous value of ref counter
			forceinline static int   IncRef (EnableRCBase &obj)			__NE___	{ return obj._counter.fetch_add( 1, EMemoryOrder::Relaxed ); }
		
		// returns previous value of ref counter
			forceinline static int   AddRef (EnableRCBase &obj, int cnt)__NE___	{ return obj._counter.fetch_add( cnt, EMemoryOrder::Relaxed ); }

		// returns '1' if object must be destroyed
			forceinline static int   DecRef (EnableRCBase &obj)			__NE___	{ return obj._counter.fetch_sub( 1, EMemoryOrder::Relaxed ); }
		
		// returns '1' if object have been destroyed.
		// 'ptr' can be null
		template <typename T>
			forceinline static int   DecRefAndRelease (INOUT T* &ptr)	__NE___;

		ND_ forceinline static int   UseCount (EnableRCBase &obj)		__NE___	{ return obj._counter.load( EMemoryOrder::Relaxed ); }
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
		
		enum class DontIncRef {};
		explicit RC (T* ptr, DontIncRef)				__NE___ : _ptr{ptr}				{}

		RC (T* ptr)										__NE___ : _ptr{ptr}				{ _Inc(); }
		RC (Ptr<T> ptr)									__NE___ : _ptr{ptr}				{ _Inc(); }
		RC (Self &&other)								__NE___ : _ptr{other.release()}	{}
		RC (const Self &other)							__NE___ : _ptr{other._ptr}		{ _Inc(); }
		
		template <typename B>	RC (RC<B> &&other)		__NE___ : _ptr{static_cast<T*>(other.release())}	{}
		template <typename B>	RC (const RC<B> &other)	__NE___ : _ptr{static_cast<T*>(other.get())}		{ _Inc(); }

		~RC ()											__NE___ { _Dec(); }

		Self&  operator = (std::nullptr_t)				__NE___ {						_Dec();  _ptr = null;			return *this; }
		Self&  operator = (T* rhs)						__NE___ { _Inc( rhs );			_Dec();  _ptr = rhs;			return *this; }
		Self&  operator = (Ptr<T> rhs)					__NE___ { _Inc( rhs.get() );	_Dec();  _ptr = rhs.get();		return *this; }
		Self&  operator = (const Self &rhs)				__NE___ { _Inc( rhs._ptr );		_Dec();  _ptr = rhs._ptr;		return *this; }
		Self&  operator = (Self &&rhs)					__NE___ {						_Dec();  _ptr = rhs.release();	return *this; }

		template <typename B>
		Self&  operator = (RC<B> &&rhs)					__NE___ { _Dec();  _ptr = static_cast<T*>(rhs.release());        return *this; }
		
		template <typename B>
		Self&  operator = (const RC<B> &rhs)			__NE___ { _Inc( static_cast<T*>(rhs.get()) );  _Dec();  _ptr = static_cast<T*>(rhs.get());  return *this; }

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
		ND_ T *		release ()							__NE___ { T* p = _ptr;  _ptr = null;  return p; }	// TODO: detach?
		ND_ int		use_count ()						C_NE___ { return _ptr != null ? RefCounterUtils::UseCount( *_ptr ) : 0; }

		ND_ explicit operator bool ()					C_NE___ { return _ptr != null; }

			void	attach (T* ptr)						__NE___ {				_Dec();  _ptr = ptr; }
			void	reset (T* ptr)						__NE___ { _Inc( ptr );	_Dec();  _ptr = ptr; }

			void	Swap (INOUT Self &rhs)				__NE___;

	private:
		static	void	_Inc (T* ptr)					__NE___;
				void	_Inc ()							__NE___;
				void	_Dec ()							__NE___;
	};
	

	
	//
	// Enable Reference Counting
	//

	template <typename T>
	class EnableRC : public EnableRCBase
	{
	// methods
	public:
		EnableRC ()				__NE___ {}

		ND_ RC<T>  GetRC ()		__NE___	{ return RC<T>{ static_cast<T*>(this) }; }
		
		template <typename B>
		ND_ RC<B>  GetRC ()		__NE___	{ return RC<B>{ static_cast<B*>(this) }; }
	};



	//
	// Pointer which depends on RC
	//

	template <typename T>
	class WithRC
	{
	// types
	public:
		using RC_t	= RC< EnableRCBase >;
		using Self	= WithRC< T >;

		STATIC_ASSERT( not IsBaseOf< EnableRCBase, T >);


	// variables
	private:
		T *		_ptr	= null;
		RC_t	_rc;


	// methods
	public:
		WithRC ()											__NE___	{}
		WithRC (Self &&)									__NE___	= default;
		WithRC (const Self &)								__NE___	= default;
		template <typename B>	WithRC (T* ptr, RC<B> rc)	__NE___	: _ptr{ptr}, _rc{RVRef(rc)} { ASSERT( (_ptr != null) == bool{_rc} ); }

		Self&  operator = (const Self &)					__NE___	= default;
		Self&  operator = (Self &&)							__NE___	= default;

		ND_ bool  operator == (const T* rhs)				C_NE___	{ return _ptr == rhs; }
		ND_ bool  operator == (Ptr<T> rhs)					C_NE___	{ return _ptr == rhs; }
		ND_ bool  operator == (const Self &rhs)				C_NE___	{ return _ptr == rhs._ptr; }
		ND_ bool  operator == (std::nullptr_t)				C_NE___ { return _ptr == null; }
		
		template <typename B>
		ND_ bool  operator != (const B& rhs)				C_NE___ { return not (*this == rhs); }

		ND_ bool  operator <  (const Self &rhs)				C_NE___ { return _ptr <  rhs._ptr; }
		ND_ bool  operator >  (const Self &rhs)				C_NE___ { return _ptr >  rhs._ptr; }
		ND_ bool  operator <= (const Self &rhs)				C_NE___ { return _ptr <= rhs._ptr; }
		ND_ bool  operator >= (const Self &rhs)				C_NE___ { return _ptr >= rhs._ptr; }
		
		ND_ T *		operator -> ()							C_NE___ { ASSERT( _ptr != null );  return _ptr; }
		ND_ T &		operator *  ()							C_NE___	{ ASSERT( _ptr != null );  return *_ptr; }
		ND_ T *		get ()									C_NE___ { return _ptr; }
		
		ND_ explicit operator bool ()						C_NE___ { return _ptr != null; }
		
		ND_ int		use_count ()							C_NE___ { return _rc.use_count(); }
	};

	

	//
	// Static Reference Counter
	//

	template <typename T>
	struct StaticRC final : Noninstancable
	{
		template <typename ...Args>
		static void  New (INOUT T& obj, Args&& ...args)	__Th___
		{
			// warning: don't use 'GetRC()' inside ctor!
			PlacementNew<T>( &obj, FwdArg<Args>( args )... );	// throw

			const int	cnt = RefCounterUtils::IncRef( obj );
			Unused( cnt );
			ASSERT( cnt == 0 );
		}

		static void  Delete (INOUT T& obj)				__NE___
		{
			const int	cnt = RefCounterUtils::DecRef( obj );
			Unused( cnt );
			ASSERT( cnt == 1 );
			
			// update cache before calling destructor
			std::atomic_thread_fence( EMemoryOrder::Acquire );

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

		STATIC_ASSERT( alignof(T) > 1 );	// because first bit is used for lock bit


	// variables
	private:
		std::atomic< T *>	_ptr {null};


	// methods
	public:
		AtomicRC ()													__NE___ {}
		AtomicRC (std::nullptr_t)									__NE___ {}

		AtomicRC (T* ptr)											__NE___ { _IncSet( ptr ); }
		AtomicRC (Ptr<T> ptr)										__NE___ { _IncSet( ptr.get() ); }
		AtomicRC (RC_t && rc)										__NE___ { _ptr.store( rc.release().release(), EMemoryOrder::Relaxed ); }
		AtomicRC (const RC_t &rc)									__NE___ { _IncSet( rc.get() ); }

		~AtomicRC ()												__NE___ { _ResetDec(); }
		
		ND_ T *		unsafe_get ()									C_NE___ { return _RemoveLockBit( _ptr.load( EMemoryOrder::Relaxed )); }
		ND_ RC_t	release ()										__NE___;

		ND_ RC_t	get ()											__NE___;

			void	reset (T* ptr)									__NE___;

		ND_ RC_t	exchange (T* desired)							__NE___;
		ND_ RC_t	exchange (RC_t desired)							__NE___;

		ND_ bool	CAS (INOUT RC_t& expected, RC_t desired)		__NE___	{ return _CAS<false>( INOUT expected, RVRef(desired) ); }
		ND_ bool	CAS_Loop (INOUT RC_t& expected, RC_t desired)	__NE___	{ return _CAS<true>( INOUT expected, RVRef(desired) ); }


		Self&  operator = (std::nullptr_t)							__NE___ { _ResetDec();			return *this; }
		Self&  operator = (T* rhs)									__NE___ { reset( rhs );			return *this; }
		Self&  operator = (Ptr<T> rhs)								__NE___ { reset( rhs );			return *this; }
		Self&  operator = (const RC_t &rhs)							__NE___ { reset( rhs.get() );	return *this; }
		Self&  operator = (RC_t && rhs)								__NE___;

	private:
		void  _IncSet (T *ptr)										__NE___;
		void  _ResetDec ()											__NE___;
		
		ND_ T*		_Lock ()										__NE___;
			void	_Unlock ()										__NE___;
		ND_ T*		_Exchange (T* ptr)								__NE___;
		
		template <bool IsStrong>
		ND_ bool	_CAS (INOUT RC_t& expected, RC_t desired)		__NE___;

		static void  _Inc (T *ptr)									__NE___;
		static void  _Dec (T *ptr)									__NE___;

		ND_ static bool	_HasLockBit (T* ptr)						__NE___	{ return (usize(ptr) & usize{1}); }

		ND_ static T*	_SetLockBit (T* ptr)						__NE___ { return reinterpret_cast< T *>((usize(ptr) | usize{1})); }

		ND_ static T*	_RemoveLockBit (T* ptr)						__NE___ { return reinterpret_cast< T *>((usize(ptr) & ~usize{1})); }
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
	ND_ forceinline RC<T>  MakeRC (Args&& ...args) __Th___
	{
		STATIC_ASSERT( not IsBaseOf< NonAllocatable, T >);

		return RC<T>{ new T{ FwdArg<Args>(args)... }};	// throw
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
	DecRefAndRelease
=================================================
*/
	template <typename T>
	forceinline int  RefCounterUtils::DecRefAndRelease (INOUT T* &ptr) __NE___
	{
		if_likely( ptr != null )
		{
			const auto	res = DecRef( *ptr );
			ASSERT( res > 0 );

			if_unlikely( res == 1 )
			{
				static_cast< EnableRCBase *>( ptr )->_ReleaseObject();
				ptr = null;
			}
			return res;
		}
		return 0;
	}

/*
=================================================
	_Inc
=================================================
*/
	template <typename T>
	forceinline void  RC<T>::_Inc (T* ptr) __NE___
	{
		if_likely( ptr != null )
			RefCounterUtils::IncRef( *ptr );
	}

	template <typename T>
	forceinline void  RC<T>::_Inc () __NE___
	{
		if_likely( _ptr != null )
			RefCounterUtils::IncRef( *_ptr );
	}
	
/*
=================================================
	_Dec
=================================================
*/
	template <typename T>
	forceinline void  RC<T>::_Dec () __NE___
	{
		RefCounterUtils::DecRefAndRelease( INOUT _ptr );
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
	ND_ AtomicRC<T>  MakeAtomicRC (Args&& ...args) __Th___
	{
		return AtomicRC<T>{ new T{ FwdArg<Args>(args)... }};
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	AtomicRC<T>&  AtomicRC<T>::operator = (RC_t &&rhs) __NE___
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
			RefCounterUtils::IncRef( *ptr );
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

		RefCounterUtils::DecRefAndRelease( INOUT ptr );
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

		RC_t	ptr;
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
		RC_t	res{ _Lock() };
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
	reset
=================================================
*/
	template <typename T>
	RC<T>  AtomicRC<T>::exchange (T* desired) __NE___
	{
		_Inc( desired );

		T*	old = _Exchange( desired );

		return RC_t{ old, RC_t::DontIncRef(0) };
	}
	
	template <typename T>
	RC<T>  AtomicRC<T>::exchange (RC_t desired) __NE___
	{
		T*	old = _Exchange( desired.release() );

		return RC_t{ old, RC_t::DontIncRef(0) };
	}
	
/*
=================================================
	CAS
=================================================
*/
	template <typename T>
	template <bool IsStrong>
	ND_ bool  AtomicRC<T>::_CAS (INOUT RC_t& expected, RC_t desired) __NE___
	{
		T*	exp = expected.get();
		T*	des	= desired.get();
		
		ASSERT( not _HasLockBit( exp ));
		ASSERT( not _HasLockBit( des ));
		
		bool	res;
		
		if constexpr( IsStrong )
			res = _ptr.compare_exchange_strong( INOUT exp, des, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed );
		else
			res = _ptr.compare_exchange_weak( INOUT exp, des, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed );
		
		if ( res ) {
			RefCounterUtils::DecRefAndRelease( exp );
			Unused( desired.release() );	// 'desired' copied to '_ptr' so don't decrease ref counter
		}else
			expected = exp;

		return res;
	}

/*
=================================================
	_Exchange
=================================================
*/
	template <typename T>
	T*  AtomicRC<T>::_Exchange (T* desired) __NE___
	{
		T*	exp = _RemoveLockBit( _ptr.load( EMemoryOrder::Relaxed ));
		
		for (uint i = 0;
			 not _ptr.compare_exchange_weak( INOUT exp, desired, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed );
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
		T*	exp = _RemoveLockBit( _ptr.load( EMemoryOrder::Relaxed ));
		for (uint i = 0;
			 not _ptr.compare_exchange_weak( INOUT exp, _SetLockBit( exp ), EMemoryOrder::Relaxed, EMemoryOrder::Relaxed );
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
		T*	exp		= _RemoveLockBit( _ptr.load( EMemoryOrder::Relaxed ));
		T*	prev	= _ptr.exchange( exp, EMemoryOrder::Relaxed );
		Unused( prev );
		ASSERT( prev == _SetLockBit( exp ));
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
