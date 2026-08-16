// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Pointers/RefCounter.h"

namespace AE::Base
{

	//
	// Reference Counter Pointer
	//
	template <typename T, usize ExtraBits>
	struct PackedRC
	{
	// types
	public:
		using Value_t	= T;
		using Self		= PackedRC< T, ExtraBits >;
	private:
		static constexpr usize	_mask = (1u << ExtraBits) - 1;
		StaticAssert( _mask > 0 );


	// variables
	private:
		usize		_value	= 0;

	  RC_TRACK_ALL_REFS(
		public: uint	_uid = 0;
	  )


	// methods
	public:
		__Cx__ PackedRC ()									__NE___ { StaticAssert( alignof(T) >= ExtraBits ); }
		__Cx__ PackedRC (std::nullptr_t)					__NE___ {}
		__Cx__ PackedRC (Default_t)							__NE___ {}

		enum class DontIncRef {};
		__Cx__ explicit PackedRC (T* ptr, DontIncRef)		__NE___ { _SetPtr( ptr ); }

		explicit PackedRC (T* ptr)							__NE___ { _Inc( ptr );			_SetPtr( ptr ); }
		explicit PackedRC (Ptr<T> ptr)						__NE___ { _Inc( ptr.get() );	_SetPtr( ptr.get() ); }
		explicit PackedRC (Ref<T> ref)						__NE___ { _Inc( &ref );			_SetPtr( &ref ); }
		explicit PackedRC (const RC<T> &other)				__NE___	{ _Inc( other.get() );	_SetPtr( other.get() ); }

		PackedRC (RC<T> &&other)							__NE___	{ _SetPtr( other.release() );	RC_TRACK_ALL_REFS( _uid = other._uid;  other._uid = 0; )}
		PackedRC (Self &&other)								__NE___ : _value{other.GetRawData()}	{ Unused(other.release());  RC_TRACK_ALL_REFS( _uid = other._uid;  other._uid = 0; )}
		PackedRC (const Self &other)						__NE___ : _value{other.GetRawData()}	{ _Inc( other.get() ); }

		template <typename B, usize C>
				  requires( IsBaseOfNotSame< T, B >)
		__Cx__ PackedRC (PackedRC<B,C> &&other)				__NE___ : _value{other.GetRawData()}	{ Unused(other.release());  RC_TRACK_ALL_REFS( _uid = other._uid;  other._uid = 0; )}

		template <typename B, usize C>
				  requires( IsBaseOfNotSame< T, B >)
		PackedRC (const PackedRC<B,C> &other)				__NE___ : _value{other.GetRawData()}	{ _Inc( get() ); }


		template <typename B, usize C>
				  requires( IsBaseOfNotSame< B, T >)
		explicit PackedRC (PackedRC<B,C> &&other)			__NE___ : _value{other.GetRawData()}	{ Unused(other.release());  RC_TRACK_ALL_REFS( _uid = other._uid;  other._uid = 0; )}

		template <typename B, usize C>
				  requires( IsBaseOfNotSame< B, T >)
		explicit PackedRC (const PackedRC<B,C> &other)		__NE___ : _value{other.GetRawData()}	{ _Inc( get() ); }


		~PackedRC ()										__NE___ { _Dec(); }

		Self&  operator = (std::nullptr_t)					__NE___ {						_Dec();  _value = 0;			return *this; }
		Self&  operator = (Default_t)						__NE___ {						_Dec();  _value = 0;			return *this; }
		Self&  operator = (T* rhs)							__NE___ { _Set( rhs );			return *this; }
		Self&  operator = (Ptr<T> rhs)						__NE___ { _Set( rhs.get() );	return *this; }
		Self&  operator = (Ref<T> rhs)						__NE___ { _Set( &rhs );			return *this; }
		Self&  operator = (const RC<T> &rhs)				__NE___ { _Set( rhs.get() );	return *this; }

		Self&  operator = (const Self &rhs)					__NE___;
		Self&  operator = (Self &&rhs)						__NE___;
		Self&  operator = (RC<T> &&rhs)						__NE___;

		template <typename B, usize C>
				  requires( IsBaseOfNotSame< T, B >)
		Self&  operator = (PackedRC<B,C> &&rhs)				__NE___;

		template <typename B, usize C>
				  requires( IsBaseOfNotSame< T, B >)
		Self&  operator = (const PackedRC<B,C> &rhs)		__NE___;

		NdCx__ bool  operator == (const T* rhs)				C_NE___ { return get() == rhs; }
		NdCx__ bool  operator == (Ptr<T> rhs)				C_NE___ { return get() == rhs.get(); }
		NdCx__ bool  operator == (Ref<T> rhs)				C_NE___ { return get() == &rhs; }
		NdCx__ bool  operator == (const RC<T> &rhs)			C_NE___ { return get() == rhs.get(); }
		NdCx__ bool  operator == (std::nullptr_t)			C_NE___ { return get() == null; }
		NdCx__ bool  operator == (Default_t)				C_NE___ { return get() == null; }

		template <typename B>
		NdCx__ bool  operator != (const B& rhs)				C_NE___ { return not (*this == rhs); }

		NdCx__ bool  operator <  (const RC<T> &rhs)			C_NE___ { return get() <  rhs.get(); }
		NdCx__ bool  operator >  (const RC<T> &rhs)			C_NE___ { return get() >  rhs.get(); }
		NdCx__ bool  operator <= (const RC<T> &rhs)			C_NE___ { return get() <= rhs.get(); }
		NdCx__ bool  operator >= (const RC<T> &rhs)			C_NE___ { return get() >= rhs.get(); }

		NdCx__ T *		operator -> ()						C_NE___ { NonNull( get() );  return get(); }
		NdCx__ T &		operator *  ()						C_NE___	{ NonNull( get() );  return *get(); }

		NdCx__ T *		get ()								C_NE___ { return BitCast<T*>( _value & ~_mask ); }
		NdCx__ T *		release ()							__NE___ { T* p = get();		_value = 0;  return p; }	// TODO: detach?
		Nd____ int		use_count ()						C_NE___ { auto* p = get();	return p != null ? RefCounterUtils::UseCount( *p ) : 0; }

		NdCx__ explicit operator bool ()					C_NE___ { return get() != null; }

		template <typename B>
		NdCx__ explicit operator RC<B> ()					C_NE___	{ return RC<B>{ get() }; }

			void		attach (T* ptr)						__NE___ {				_Dec();  _SetPtr( ptr ); }
			void		reset (T* ptr)						__NE___ { _Inc( ptr );	_Dec();  _SetPtr( ptr ); }
			void		reset ()							__NE___ {				_Dec();  _value = 0; }

			void		Swap (INOUT Self &rhs)				__NE___;

		ND_ usize		Extra ()							C_NE___	{ return _value & _mask; }
			void		SetExtra (usize value)				__NE___	{ _value &= ~_mask;  _value |= value & _mask;  ASSERT( Extra() == value ); }

		NdCx__ usize		GetRawData ()					C_NE___	{ return _value; }

	private:
			void	_Set (T* ptr)							__NE___;
			void	_Inc (T* ptr)							__NE___;
			void	_Dec ()									__NE___;

			void	_SetPtr (T* p)							__NE___	{ _value = BitCast<usize>( p );  ASSERT( Extra() == 0 ); }
	};


/*
=================================================
	_Set
----
	inc other ref
	dec self
	copy ref to self
=================================================
*/
	template <typename T, usize C>
	void  PackedRC<T,C>::_Set (T* ptr) __NE___
	{
		StaticAssert( IsBaseOf< EnableRCBase, T >);

		uint	uid = 0;

		if_likely( ptr != null )
		{
		  #ifdef AE_RC_TRACK_ALL_REFS
			RefCounterUtils::IncRef( *ptr, OUT uid );
		  #else
			RefCounterUtils::IncRef( *ptr );
			Unused( uid );
		  #endif
		}

		_Dec();

		_SetPtr( ptr );
		RC_TRACK_ALL_REFS( _uid = uid; )
	}

/*
=================================================
	_Inc (self)
=================================================
*/
	template <typename T, usize C>
	void  PackedRC<T,C>::_Inc (T* ptr) __NE___
	{
		StaticAssert( IsBaseOf< EnableRCBase, T >);

		if_likely( ptr != null )
		{
		  #ifdef AE_RC_TRACK_ALL_REFS
			RefCounterUtils::IncRef( *ptr, OUT _uid );
		  #else
			RefCounterUtils::IncRef( *ptr );
		  #endif
		}
	}

/*
=================================================
	_Dec (self)
=================================================
*/
	template <typename T, usize C>
	void  PackedRC<T,C>::_Dec () __NE___
	{
		StaticAssert( IsBaseOf< EnableRCBase, T >);

		T*	ptr = get();
		if_unlikely( RefCounterUtils::DecRefAndRelease( INOUT ptr  RC_TRACK_ALL_REFS(, _uid )) == 1 )
		{
			_value = 0;
		}
		RC_TRACK_ALL_REFS( _uid = 0; )
	}

/*
=================================================
	Swap
=================================================
*/
	template <typename T, usize C>
	void  PackedRC<T,C>::Swap (INOUT PackedRC<T,C> &rhs) __NE___
	{
		std::swap( _value, rhs._value );
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T, usize C>
	PackedRC<T,C>&  PackedRC<T,C>::operator = (const Self &rhs) __NE___
	{
		_Inc( rhs.get() );
		RC_TRACK_ALL_REFS( uint  uid = _uid;)

		_Dec();
		RC_TRACK_ALL_REFS( _uid = uid; )

		_value = rhs._value;
		return *this;
	}

	template <typename T, usize C>
	PackedRC<T,C>&  PackedRC<T,C>::operator = (PackedRC<T,C> &&rhs) __NE___
	{
		ASSERT( this != &rhs );

		_Dec();
		_value = rhs._value;

		Unused(rhs.release());
		RC_TRACK_ALL_REFS( _uid = rhs._uid;  rhs._uid = 0; )
		return *this;
	}

	template <typename T, usize C>
	PackedRC<T,C>&  PackedRC<T,C>::operator = (RC<T> &&rhs) __NE___
	{
		_Dec();
		_SetPtr( rhs.release() );
		RC_TRACK_ALL_REFS( _uid = rhs._uid;  rhs._uid = 0; )
		return *this;
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T, usize C>
	template <typename B, usize S>
		requires( IsBaseOfNotSame< T, B >)
	PackedRC<T,C>&  PackedRC<T,C>::operator = (PackedRC<B,S> &&rhs) __NE___
	{
		_Dec();
		_value = rhs.GetRawData();

		Unused(rhs.release());
		RC_TRACK_ALL_REFS( _uid = rhs._uid;  rhs._uid = 0; )
		return *this;
	}

	template <typename T, usize C>
	template <typename B, usize S>
		requires( IsBaseOfNotSame< T, B >)
	PackedRC<T,C>&  PackedRC<T,C>::operator = (const PackedRC<B,S> &rhs) __NE___
	{
		_Inc( static_cast<T*>(rhs.get()) );
		RC_TRACK_ALL_REFS( uint  uid = _uid;)

		_Dec();
		RC_TRACK_ALL_REFS( _uid = uid; )

		_value = rhs.GetRawData();
		return *this;
	}

} // AE::Base
