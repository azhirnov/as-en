// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Some kind of std::synchronized_value<>

	Supported types for 'SyncObj':
		RWSpinLock, SharedMutex
*/

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{
namespace _hidden_
{
	template <typename SyncObj, typename T>
	class Synchronized_ConstPtr
	{
	// types
	private:
		using Self = Synchronized_ConstPtr< SyncObj, T >;

	// variables
	private:
		SyncObj &	_sync;
		T &			_ref;

	// methods
	public:
		Synchronized_ConstPtr ()						= delete;
		Synchronized_ConstPtr (Self &&)					= delete;
		Synchronized_ConstPtr (const Self &)			= delete;

		Self&  operator = (Self &&)						= delete;
		Self&  operator = (const Self &)				= delete;

		Synchronized_ConstPtr (SyncObj &sync, T &ref)	__NE___ : _sync{sync}, _ref{ref} { _sync.lock_shared(); }
		~Synchronized_ConstPtr ()						__NE___	{ _sync.unlock_shared(); }

		T const*  operator -> ()						C_NE___	{ return &_ref; }
		T const&  operator *  ()						C_NE___	{ return _ref; }
	};


	template <typename SyncObj, typename T>
	class Synchronized_MutablePtr
	{
	// types
	private:
		using Self = Synchronized_MutablePtr< SyncObj, T >;

	// variables
	private:
		SyncObj &	_sync;
		T &			_ref;

	// methods
	public:
		Synchronized_MutablePtr ()						= delete;
		Synchronized_MutablePtr (Self &&)				= delete;
		Synchronized_MutablePtr (const Self &)			= delete;

		Self&  operator = (Self &&)						= delete;
		Self&  operator = (const Self &)				= delete;

		Synchronized_MutablePtr (SyncObj &sync, T &ref)	__NE___ : _sync{sync}, _ref{ref} { _sync.lock(); }
		~Synchronized_MutablePtr ()						__NE___	{ _sync.unlock(); }

		T *  operator -> ()								__NE___	{ return &_ref; }
		T &  operator *  ()								__NE___	{ return _ref; }
	};

} // _hidden_


	template <typename SyncObj, typename ...Types>
	class Synchronized;



	//
	// Synchronized (multiple types)
	//

	template <typename SyncObj, typename T0, typename T1, typename ...Types>
	class Synchronized< SyncObj, T0, T1, Types... >
	{
	// types
	public:
		using Self			= Synchronized< SyncObj, T0, T1, Types... >;
		using SyncObj_t		= SyncObj;
		using ValueTypes_t	= TypeList< T0, T1, Types... >;

		//StaticAssert(( AllNothrowMoveCtor< T0, T1, Types... >));
		//StaticAssert(( AllNothrowDefaultCtor< T0, T1, Types... >));

	private:
		using TypesNoRC_t	= typename ValueTypes_t::template Apply< RemoveRC >;
		using TypesNoPtr_t	= typename ValueTypes_t::template Apply< RemovePtr >;
		using TypesNoPtr2_t	= typename ValueTypes_t::template Apply< RemovePointer >;
		using Tuple_t		= typename ValueTypes_t::AsTuple::type;

		template <typename T>
		ND_ static constexpr usize  _IndexOf () __NE___
		{
			if constexpr( ValueTypes_t::template HasType<T> )
			{
				StaticAssert( ValueTypes_t::template HasSingle<T> );
				return ValueTypes_t::template Index<T>;
			}else
			if constexpr( TypesNoRC_t::template HasType<T> )
			{
				StaticAssert( TypesNoRC_t::template HasSingle<T> );
				return TypesNoRC_t::template Index<T>;
			}else
			if constexpr( TypesNoPtr_t::template HasType<T> )
			{
				StaticAssert( TypesNoPtr_t::template HasSingle<T> );
				return TypesNoPtr_t::template Index<T>;
			}else
			if constexpr( TypesNoPtr2_t::template HasType<T> )
			{
				StaticAssert( TypesNoPtr2_t::template HasSingle<T> );
				return TypesNoPtr2_t::template Index<T>;
			}
		}


	public:
		class ReadNoLock_t
		{
		// variables
		private:
			Self const&	_ref;
			bool		_locked	= false;

		// methods
		public:
			explicit ReadNoLock_t (const Self &ref)				__NE___	: _ref{ref} {}
			ReadNoLock_t (const Self &ref, int)					__NE___	: _ref{ref} { lock_shared(); }
			ReadNoLock_t (const ReadNoLock_t &)					= delete;
			ReadNoLock_t (ReadNoLock_t &&other)					__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~ReadNoLock_t ()									__NE___	{ if_likely( _locked ) unlock_shared(); }

			ReadNoLock_t&  operator = (const ReadNoLock_t &)	= delete;
			ReadNoLock_t&  operator = (ReadNoLock_t &&)			= delete;

			ND_ bool	try_lock_shared ()						__NE___	{ ASSERT( not _locked );	return (_locked = _ref._sync.try_lock_shared()); }
				void	lock_shared ()							__NE___	{ ASSERT( not _locked );	_ref._sync.lock_shared();		_locked = true;  }
				void	unlock_shared ()						__NE___	{ ASSERT( _locked );		_ref._sync.unlock_shared();		_locked = false; }

			ND_ auto&	operator * ()							rvNE___	= delete;
			ND_ auto&	operator * ()							r_NE___	{ ASSERT( _locked );		return _ref._values; }

			template <typename	T,
					  usize		Index	= _IndexOf<T>(),
					  typename	RawT	= typename ValueTypes_t::template Get<Index>
					 >
			ND_ RawT const&  Get ()								C_NE___
			{
				ASSERT( _locked );
				return _ref._values.template Get<Index>();
			}

			template <usize		Index,
					  typename	RawT	= typename ValueTypes_t::template Get<Index>
					 >
			ND_ RawT const&  Get ()								C_NE___
			{
				ASSERT( _locked );
				return _ref._values.template Get<Index>();
			}
		};


		class WriteNoLock_t
		{
		// variables
		private:
			Self &		_ref;
			bool		_locked	= false;

		// methods
		public:
			explicit WriteNoLock_t (Self &ref)					__NE___	: _ref{ref} {}
			WriteNoLock_t (Self &ref, int)						__NE___	: _ref{ref} { lock(); }
			WriteNoLock_t (const WriteNoLock_t &)				= delete;
			WriteNoLock_t (WriteNoLock_t &&other)				__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~WriteNoLock_t ()									__NE___	{ if_likely( _locked ) unlock(); }

			WriteNoLock_t&  operator = (const WriteNoLock_t &)	= delete;
			WriteNoLock_t&  operator = (WriteNoLock_t &&)		= delete;

			ND_ bool	try_lock ()								__NE___	{ ASSERT( not _locked );	return (_locked = _ref._sync.try_lock()); }
				void	lock ()									__NE___	{ ASSERT( not _locked );	_ref._sync.lock();    _locked = true;  }
				void	unlock ()								__NE___	{ ASSERT( _locked );		_ref._sync.unlock();  _locked = false; }

			ND_ auto&	operator * ()							rvNE___	= delete;
			ND_ auto&	operator * ()							r_NE___	{ ASSERT( _locked );		return _ref._values; }

			template <typename	T,
					  usize		Index	= _IndexOf<T>(),
					  typename	RawT	= typename ValueTypes_t::template Get<Index>
					 >
			ND_ RawT&	Get ()									__NE___
			{
				ASSERT( _locked );
				return _ref._values.template Get<Index>();
			}

			template <usize		Index,
					  typename	RawT	= typename ValueTypes_t::template Get<Index>
					 >
			ND_ RawT&	Get ()									__NE___
			{
				ASSERT( _locked );
				return _ref._values.template Get<Index>();
			}
		};



	// variables
	private:
		mutable SyncObj		_sync;
		Tuple_t				_values;


	// methods
	public:
		Synchronized ()						__NE___
		{}

		Synchronized (Self &&other)			__NE___ :
			_values{ RVRef(other).Extract() }
		{}

		explicit Synchronized (T0 &&arg0, T1 &&arg1, Types&&... args) __NE___ :
			_values{ FwdArg<T0>(arg0), FwdArg<T1>(arg1), FwdArg<Types>(args)... }
		{}

		Synchronized (const Self &other)	NoExcept(AllNothrowCopyCtor<T0,T1,Types...>) :
			_values{ other.ReadAll() }	// throw
		{}

		~Synchronized ()					__NE___
		{
		  DEBUG_ONLY(
			CHECK( _sync.try_lock() );	// must be unlocked
			_sync.unlock();
		)}


		Self&  operator = (Self &&rhs)		__NE___
		{
			EXLOCK( this->_sync, rhs._sync );	// TODO: sharedlock for 'rhs'
			this->_values.~Tuple_t();
			PlacementNew<Tuple_t>( OUT std::addressof(this->_values), RVRef(rhs._values) );
			return *this;
		}

		Self&  operator = (const Self &rhs)	__NE___
		{
			EXLOCK( this->_sync, rhs._sync );	// TODO: sharedlock for 'rhs'
			this->_values.~Tuple_t();
			PlacementNew<Tuple_t>( OUT std::addressof(this->_values), rhs._values );
			return *this;
		}


		template <typename	T,
				  usize		Index			= _IndexOf<T>(),
				  typename	RawT			= typename ValueTypes_t::template Get<Index>
				 >
		ND_ RawT  Read ()					CNoExcept(IsNothrowCopyCtor<RawT>)
		{
			SHAREDLOCK( _sync );
			return _values.template Get<Index>();
		}

		template <usize		Index,
				  typename	RawT			= typename ValueTypes_t::template Get<Index>
				 >
		ND_ RawT  Read ()					CNoExcept(IsNothrowCopyCtor<RawT>)
		{
			SHAREDLOCK( _sync );
			return _values.template Get<Index>();
		}

		ND_ Tuple_t  ReadAll ()				CNoExcept(AllNothrowCopyCtor<T0,T1,Types...>)
		{
			SHAREDLOCK( _sync );
			return _values;
		}

		template <typename Fn>
		exact_t  ReadAll (Fn &&fn)	CNoExcept(IsNothrowInvocable< Fn, T0&, T1&, Types&... >)
		{
			SHAREDLOCK( _sync );
			return _values.Apply( FwdArg<Fn>( fn ));
		}


		template <typename T,
				  typename RawT				= RemoveCVRef<T>
				 >
		void  Write (T &&value)				__NE___
		{
			StaticAssert( ValueTypes_t::template HasSingle<RawT> );
			EXLOCK( _sync );
			auto&	dst = _values.template Get<RawT>();
			dst.~RawT();
			PlacementNew<RawT>( OUT std::addressof(dst), FwdArg<T>(value) );
		}

		template <typename ...Args>
		____IA void  WriteAll (Args&& ...args)  NoExcept(AllNothrowCopyCtor<T0,T1,Types...>)
		{
			EXLOCK( _sync );
			_values.Set( FwdArg<Args>(args)... );
		}

		// read / write access
		template <typename Fn>
		____IA exact_t  Modify (Fn &&fn)	NoExcept(IsNothrowInvocable< Fn, T0&, T1&, Types&... >)
		{
			EXLOCK( _sync );
			return _values.Apply( FwdArg<Fn>( fn ));
		}

		template <typename	T,
				  usize		Index			= _IndexOf<T>(),
				  typename	RawT			= typename ValueTypes_t::template Get<Index>
				 >
		void  Reset ()						__NE___
		{
			EXLOCK( _sync );
			auto&	dst = _values.template Get<Index>();
			dst.~RawT();
			PlacementNew<RawT>( OUT std::addressof( dst ));
		}

		template <usize		Index,
				  typename	RawT			= typename ValueTypes_t::template Get<Index>
				 >
		void  Reset ()						__NE___
		{
			EXLOCK( _sync );
			auto&	dst = _values.template Get<Index>();
			dst.~RawT();
			PlacementNew<RawT>( OUT std::addressof( dst ));
		}


		ND_ Tuple_t  Extract ()				rvNE___
		{
			EXLOCK( _sync );
			return RVRef(_values);
		}


		template <typename	T,
				  usize		Index			= _IndexOf<T>()
				 >
		ND_ auto  ConstPtr ()				C_NE___	{ return Threading::_hidden_::Synchronized_ConstPtr{ _sync, _values.template Get<Index>() }; }

		template <usize Index>
		ND_ auto  ConstPtr ()				C_NE___	{ return Threading::_hidden_::Synchronized_ConstPtr{ _sync, _values.template Get<Index>() }; }

		template <typename	T>
		ND_ auto  Ptr ()					C_NE___	{ return ConstPtr<T>(); }

		template <usize Index>
		ND_ auto  Ptr ()					C_NE___	{ return ConstPtr<Index>(); }


		template <typename	T,
				  usize		Index			= _IndexOf<T>()
				 >
		ND_ auto  MutablePtr ()				__NE___	{ return Threading::_hidden_::Synchronized_MutablePtr{ _sync, _values.template Get<Index>() }; }

		template <usize Index>
		ND_ auto  MutablePtr ()				__NE___	{ return Threading::_hidden_::Synchronized_MutablePtr{ _sync, _values.template Get<Index>() }; }

		template <typename	T>
		ND_ auto  Ptr ()					__NE___	{ return MutablePtr<T>(); }

		template <usize Index>
		ND_ auto  Ptr ()					__NE___	{ return MutablePtr<Index>(); }


		ND_ auto  ReadNoLock ()				C_NE___	{ return ReadNoLock_t{ *this }; }
		ND_ auto  WriteNoLock ()			__NE___	{ return WriteNoLock_t{ *this }; }

		ND_ auto  ReadLock ()				C_NE___	{ return ReadNoLock_t{ *this, 0 }; }
		ND_ auto  WriteLock ()				__NE___	{ return WriteNoLock_t{ *this, 0 }; }
	};



	//
	// Synchronized (single type)
	//

	template <typename SyncObj, typename T>
	class Synchronized< SyncObj, T >
	{
		//CheckNothrow( IsNothrowDefaultCtor< T >);
		//CheckNothrow( IsNothrowMoveCtor< T >);

	// types
	public:
		using Self		= Synchronized< SyncObj, T >;
		using SyncObj_t	= SyncObj;
		using Value_t	= T;

		class ReadNoLock_t
		{
		// variables
		private:
			Self const&	_ref;
			bool		_locked	= false;

		// methods
		public:
			explicit ReadNoLock_t (const Self &ref)				__NE___	: _ref{ref} {}
			ReadNoLock_t (const Self &ref, int)					__NE___	: _ref{ref} { lock_shared(); }
			ReadNoLock_t (const ReadNoLock_t &)					= delete;
			ReadNoLock_t (ReadNoLock_t &&other)					__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~ReadNoLock_t ()									__NE___	{ if_likely( _locked ) unlock_shared(); }

			ReadNoLock_t&  operator = (const ReadNoLock_t &)	= delete;
			ReadNoLock_t&  operator = (ReadNoLock_t &&)			= delete;

			ND_ bool		try_lock_shared ()					__NE___	{ ASSERT( not _locked );	return (_locked = _ref._sync.try_lock_shared()); }
				void		lock_shared ()						__NE___	{ ASSERT( not _locked );	_ref._sync.lock_shared();    _locked = true;  }
				void		unlock_shared ()					__NE___	{ ASSERT( _locked );		_ref._sync.unlock_shared();  _locked = false; }

			ND_ T const&	operator *  ()						rvNE___	= delete;
			ND_ T const&	operator *  ()						r_NE___	{ ASSERT( _locked );		return _ref._value; }
			ND_ T const*	operator -> ()						__NE___	{ ASSERT( _locked );		return &_ref._value; }
		};


		class WriteNoLock_t
		{
		// variables
		private:
			Self &		_ref;
			bool		_locked	= false;

		// methods
		public:
			explicit WriteNoLock_t (Self &ref)					__NE___	: _ref{ref} {}
			WriteNoLock_t (Self &ref, int)						__NE___	: _ref{ref} { lock(); }
			WriteNoLock_t (const WriteNoLock_t &)				= delete;
			WriteNoLock_t (WriteNoLock_t &&other)				__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~WriteNoLock_t ()									__NE___	{ if_likely( _locked ) unlock(); }

			WriteNoLock_t&  operator = (const WriteNoLock_t &)	= delete;
			WriteNoLock_t&  operator = (WriteNoLock_t &&)		= delete;

			ND_ bool	try_lock ()								__NE___	{ ASSERT( not _locked );	return (_locked = _ref._sync.try_lock()); }
				void	lock ()									__NE___	{ ASSERT( not _locked );	_ref._sync.lock();    _locked = true;  }
				void	unlock ()								__NE___	{ ASSERT( _locked );		_ref._sync.unlock();  _locked = false; }

			ND_ T &		operator *  ()							rvNE___	= delete;
			ND_ T &		operator *  ()							r_NE___	{ ASSERT( _locked );		return _ref._value; }
			ND_ T *		operator -> ()							__NE___	{ ASSERT( _locked );		return &_ref._value; }
		};


	// variables
	private:
		mutable SyncObj		_sync;
		T					_value;


	// methods
	public:
		Synchronized ()							__NE___
		{}

		Synchronized (Self &&other)				__NE___ :
			_value{ RVRef(other).Extract() }
		{}

		explicit Synchronized (T &&value)		__NE___ :
			_value{ RVRef(value) }
		{}

		Synchronized (const Self &other)		NoExcept(IsNothrowCopyCtor<T>) :
			_value{ other.Read() }  // throw
		{}

		explicit Synchronized (const T &value)	NoExcept(IsNothrowCopyCtor<T>) :
			_value{ value }  // throw
		{}

		template <typename ...Args, ENABLEIF( IsConstructible< T, Args... >)>
		explicit Synchronized (Args&& ...args)	__Th___ :
			_value{ FwdArg<Args>(args)... }
		{}

		~Synchronized ()						__NE___
		{
		  DEBUG_ONLY(
			CHECK( _sync.try_lock() );	// must be unlocked
			_sync.unlock();
		)}


		Self&  operator = (Self &&rhs)			__NE___
		{
			EXLOCK( this->_sync, rhs._sync );	// TODO: sharedlock for 'rhs'
			this->_value.~T();
			PlacementNew<T>( OUT std::addressof(this->_value), RVRef(rhs._value) );
			return *this;
		}

		Self&  operator = (const Self &rhs)		__NE___
		{
			EXLOCK( this->_sync, rhs._sync );	// TODO: sharedlock for 'rhs'
			this->_value.~T();
			PlacementNew<T>( OUT std::addressof(this->_value), rhs._value );
			return *this;
		}


		ND_ T  Read ()							CNoExcept(IsNothrowCopyCtor<T>)
		{
			SHAREDLOCK( _sync );
			return _value;
		}

		template <typename Fn>
		exact_t  Read (Fn &&fn)					CNoExcept(IsNothrowInvocable< Fn, T >)
		{
			SHAREDLOCK( _sync );
			return fn( _value );
		}


		void  Write (const T &value)			__NE___
		{
			EXLOCK( _sync );
			this->_value.~T();
			PlacementNew<T>( OUT std::addressof(this->_value), value );
		}

		void  Write (T &&value)					__NE___
		{
			EXLOCK( _sync );
			this->_value.~T();
			PlacementNew<T>( OUT std::addressof(this->_value), RVRef(value) );
		}

		// read / write access
		template <typename Fn>
		____IA exact_t  Modify (Fn &&fn)		NoExcept(IsNothrowInvocable< Fn, T >)
		{
			EXLOCK( _sync );
			return fn( _value );
		}


		void  Reset ()							__NE___
		{
			EXLOCK( _sync );
			this->_value.~T();
			PlacementNew<T>( OUT std::addressof(this->_value) );
		}

		ND_ T  Extract ()						rvNE___
		{
			EXLOCK( _sync );
			return RVRef(_value);
		}


		ND_ auto  ConstPtr ()					C_NE___	{ return Threading::_hidden_::Synchronized_ConstPtr{ _sync, _value }; }
		ND_ auto  Ptr ()						C_NE___	{ return ConstPtr(); }
		ND_ auto  operator -> ()				C_NE___	{ return ConstPtr(); }

		ND_ auto  MutablePtr ()					__NE___	{ return Threading::_hidden_::Synchronized_MutablePtr{ _sync, _value }; }
		ND_ auto  Ptr ()						__NE___	{ return MutablePtr(); }
		ND_ auto  operator -> ()				__NE___	{ return MutablePtr(); }


		ND_ auto  ReadNoLock ()					C_NE___	{ return ReadNoLock_t{ *this }; }
		ND_ auto  WriteNoLock ()				__NE___	{ return WriteNoLock_t{ *this }; }

		ND_ auto  ReadLock ()					C_NE___	{ return ReadNoLock_t{ *this, 0 }; }
		ND_ auto  WriteLock ()					__NE___	{ return WriteNoLock_t{ *this, 0 }; }
	};



	//
	// Synchronized (unsupported)
	//

	template <typename SyncObj>
	class Synchronized< SyncObj >
	{};


} // AE::Threading
