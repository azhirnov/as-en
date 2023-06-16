// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Supported types for 'SyncObj':
		RWSpinLock, SharedMutex
*/

#pragma once

#include "base/CompileTime/TypeList.h"
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
		using Types_t		= TypeList< T0, T1, Types... >;
		
		STATIC_ASSERT( Types_t::AllNothrowDtor );
		//STATIC_ASSERT( Types_t::AllNothrowMoveCtor );

	private:
		using TypesNoRC_t	= typename Types_t::template Apply< RemoveRC >;
		using TypesNoPtr_t	= typename Types_t::template Apply< RemovePtr >;
		using TypesNoPtr2_t	= typename Types_t::template Apply< RemovePointer >;
		using Tuple_t		= typename Types_t::AsTuple::type;

		template <typename T>
		ND_ static constexpr usize  _IndexOf ()
		{
			if constexpr( Types_t::template HasType<T> )
			{
				STATIC_ASSERT( Types_t::template HasSingle<T> );
				return Types_t::template Index<T>;
			}else
			if constexpr( TypesNoRC_t::template HasType<T> )
			{
				STATIC_ASSERT( TypesNoRC_t::template HasSingle<T> );
				return TypesNoRC_t::template Index<T>;
			}else
			if constexpr( TypesNoPtr_t::template HasType<T> )
			{
				STATIC_ASSERT( TypesNoPtr_t::template HasSingle<T> );
				return TypesNoPtr_t::template Index<T>;
			}else
			if constexpr( TypesNoPtr2_t::template HasType<T> )
			{
				STATIC_ASSERT( TypesNoPtr2_t::template HasSingle<T> );
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
			ReadNoLock_t (const Self &ref)					__NE___	: _ref{ref} {}
			ReadNoLock_t (const ReadNoLock_t &)				= delete;
			ReadNoLock_t (ReadNoLock_t && other)			__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~ReadNoLock_t ()								__NE___	{ if ( _locked ) unlock_shared(); }

			ReadNoLock_t&  operator = (const ReadNoLock_t &)= delete;
			ReadNoLock_t&  operator = (ReadNoLock_t &&)		= delete;

			ND_ bool	try_lock_shared ()					__NE___	{ ASSERT( not _locked );  return (_locked = _ref._sync.try_lock_shared()); }
				void	lock_shared ()						__NE___	{ ASSERT( not _locked );  _ref._sync.lock_shared();    _locked = true;  }
				void	unlock_shared ()					__NE___	{ ASSERT( _locked );      _ref._sync.unlock_shared();  _locked = false; }

			ND_ auto&	operator * ()						__NE___	{ ASSERT( _locked );  return _ref._values; }
			
			template <typename	T,
					  usize		Index	= _IndexOf<T>(),
					  typename	RawT	= typename Types_t::template Get<Index>
					 >
			ND_ RawT const&  Get ()							C_NE___
			{
				ASSERT( _locked );
				return _ref._values.template Get<Index>();
			}
			
			template <usize		Index,
					  typename	RawT	= typename Types_t::template Get<Index>
					 >
			ND_ RawT const&  Get ()							C_NE___
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
			WriteNoLock_t (Self &ref)						__NE___	: _ref{ref} {}
			WriteNoLock_t (const WriteNoLock_t &)			= delete;
			WriteNoLock_t (WriteNoLock_t && other)			__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~WriteNoLock_t ()								__NE___	{ if ( _locked ) unlock(); }

			WriteNoLock_t&  operator = (const WriteNoLock_t &)= delete;
			WriteNoLock_t&  operator = (WriteNoLock_t &&)	= delete;
			
			ND_ bool	try_lock ()							__NE___	{ ASSERT( not _locked );  return (_locked = _ref._sync.try_lock()); }
				void	lock ()								__NE___	{ ASSERT( not _locked );  _ref._sync.lock();    _locked = true;  }
				void	unlock ()							__NE___	{ ASSERT( _locked );      _ref._sync.unlock();  _locked = false; }

			ND_ auto&	operator * ()						__NE___	{ ASSERT( _locked );  return _ref._values; }
			
			template <typename	T,
					  usize		Index	= _IndexOf<T>(),
					  typename	RawT	= typename Types_t::template Get<Index>
					 >
			ND_ RawT&	Get ()								__NE___
			{
				ASSERT( _locked );
				return _ref._values.template Get<Index>();
			}
			
			template <usize		Index,
					  typename	RawT	= typename Types_t::template Get<Index>
					 >
			ND_ RawT&	Get ()								__NE___
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
		Synchronized ()						noexcept(Types_t::AllNothrowDefaultCtor)
		{}

		Synchronized (Self &&other)			__NE___ :
			_values{ RVRef(other).Extract() }
		{}
		
		explicit Synchronized (T0 &&arg0, T1 &&arg1, Types&&... args) noexcept(Types_t::AllNothrowDefaultCtor) :
			_values{ FwdArg<T0>(arg0), FwdArg<T1>(arg1), FwdArg<Types>(args)... }
		{}

		Synchronized (const Self &other)	noexcept(Types_t::AllNothrowCopyCtor) :
			_values{ other.ReadAll() }	// throw
		{}

		~Synchronized ()					__NE___
		{
		  DEBUG_ONLY(
			ASSERT( _sync.try_lock() );
			_sync.unlock();
		)}


		Self&  operator = (Self &&rhs)		__NE___
		{
			EXLOCK( this->_sync, rhs._sync );
			this->_values = RVRef(rhs._values);
			return *this;
		}

		Self&  operator = (const Self &rhs)	noexcept(Types_t::AllNothrowCopyAssignable)
		{
			EXLOCK( this->_sync, rhs._sync );
			this->_values = rhs._values;	// throw
			return *this;
		}


		template <typename	T,
				  usize		Index			= _IndexOf<T>(),
				  typename	RawT			= typename Types_t::template Get<Index>
				 >
		ND_ RawT  Read ()					const noexcept(IsNothrowCopyCtor<RawT>)
		{
			SHAREDLOCK( _sync );
			return _values.template Get<Index>();
		}
		
		template <usize		Index,
				  typename	RawT			= typename Types_t::template Get<Index>
				 >
		ND_ RawT  Read ()					const noexcept(IsNothrowCopyCtor<RawT>)
		{
			SHAREDLOCK( _sync );
			return _values.template Get<Index>();
		}

		ND_ Tuple_t  ReadAll ()				const noexcept(Types_t::AllNothrowCopyCtor)
		{
			SHAREDLOCK( _sync );
			return _values;
		}

		
		template <typename T,
				  typename RawT				= RemoveCVRef<T>
				 >
		void  Write (T &&value)				noexcept(IsNothrowCopyCtor<RawT>)
		{
			STATIC_ASSERT( Types_t::template HasSingle<RawT> );
			EXLOCK( _sync );
			_values.template Get<RawT>() = FwdArg<T>(value);
		}

		template <typename ...Args>
		void  WriteAll (Args&& ...args)		noexcept(Types_t::AllNothrowCopyCtor)
		{
			EXLOCK( _sync );
			_values.Set( FwdArg<Args>(args)... );
		}

		
		template <typename	T,
				  usize		Index			= _IndexOf<T>(),
				  typename	RawT			= typename Types_t::template Get<Index>
				 >
		void  Reset ()						noexcept(IsNothrowCopyCtor<RawT>)
		{
			EXLOCK( _sync );
			_values.template Get<Index>() = RawT{};
		}
		
		template <usize		Index,
				  typename	RawT			= typename Types_t::template Get<Index>
				 >
		void  Reset ()						noexcept(IsNothrowCopyCtor<RawT>)
		{
			EXLOCK( _sync );
			_values.template Get<Index>() = RawT{};
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
	};

	

	//
	// Synchronized (single type)
	//

	template <typename SyncObj, typename T>
	class Synchronized< SyncObj, T >
	{
		STATIC_ASSERT( IsNothrowDtor< T >);
		//STATIC_ASSERT( IsNothrowMoveCtor< T >);

	// types
	public:
		using Self	= Synchronized< SyncObj, T >;
		
		class ReadNoLock_t
		{
		// variables
		private:
			Self const&	_ref;
			bool		_locked	= false;

		// methods
		public:
			ReadNoLock_t (const Self &ref)					__NE___	: _ref{ref} {}
			ReadNoLock_t (const ReadNoLock_t &)				= delete;
			ReadNoLock_t (ReadNoLock_t && other)			__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~ReadNoLock_t ()								__NE___	{ if ( _locked ) unlock_shared(); }

			ReadNoLock_t&  operator = (const ReadNoLock_t &)= delete;
			ReadNoLock_t&  operator = (ReadNoLock_t &&)		= delete;
		
			ND_ bool		try_lock_shared ()				__NE___	{ ASSERT( not _locked );  return (_locked = _ref._sync.try_lock_shared()); }
				void		lock_shared ()					__NE___	{ ASSERT( not _locked );  _ref._sync.lock_shared();    _locked = true;  }
				void		unlock_shared ()				__NE___	{ ASSERT( _locked );      _ref._sync.unlock_shared();  _locked = false; }

			ND_ T const&	operator *  ()					__NE___	{ ASSERT( _locked );  return _ref._value; }
			ND_ T const*	operator -> ()					__NE___	{ ASSERT( _locked );  return &_ref._value; }
		};


		class WriteNoLock_t
		{
		// variables
		private:
			Self &		_ref;
			bool		_locked	= false;

		// methods
		public:
			WriteNoLock_t (Self &ref)						__NE___	: _ref{ref} {}
			WriteNoLock_t (const WriteNoLock_t &)			= delete;
			WriteNoLock_t (WriteNoLock_t && other)			__NE___ : _ref{other._ref}, _locked{other._locked} { other._locked = false; }
			~WriteNoLock_t ()								__NE___	{ if ( _locked ) unlock(); }

			WriteNoLock_t&  operator = (const WriteNoLock_t &)= delete;
			WriteNoLock_t&  operator = (WriteNoLock_t &&)	= delete;

			ND_ bool	try_lock ()							__NE___	{ ASSERT( not _locked );  return (_locked = _ref._sync.try_lock()); }
				void	lock ()								__NE___	{ ASSERT( not _locked );  _ref._sync.lock();    _locked = true;  }
				void	unlock ()							__NE___	{ ASSERT( _locked );      _ref._sync.unlock();  _locked = false; }
				
			ND_ T &		operator *  ()						__NE___	{ ASSERT( _locked );  return _ref._value; }
			ND_ T *		operator -> ()						__NE___	{ ASSERT( _locked );  return &_ref._value; }
		};


	// variables
	private:
		mutable SyncObj		_sync;
		T					_value;


	// methods
	public:
		Synchronized ()							noexcept(IsNothrowDefaultCtor<T>)
		{}

		Synchronized (Self &&other)				__NE___ :
			_value{ RVRef(other).Extract() }
		{}
		
		explicit Synchronized (T &&value)		__NE___ :
			_value{ RVRef(value) }
		{}

		Synchronized (const Self &other)		noexcept(IsNothrowCopyCtor<T>) :
			_value{ other.Read() }  // throw
		{}

		explicit Synchronized (const T &value)	noexcept(IsNothrowCopyCtor<T>) :
			_value{ value }  // throw
		{}

		~Synchronized ()						__NE___
		{
		  DEBUG_ONLY(
			ASSERT( _sync.try_lock() );
			_sync.unlock();
		)}


		Self&  operator = (Self && rhs)			__NE___
		{
			EXLOCK( this->_sync, rhs._sync );
			this->_value = RVRef(rhs._value);
			return *this;
		}

		Self&  operator = (const Self &rhs)		noexcept(IsNothrowCopyAssignable<T>)
		{
			EXLOCK( this->_sync, rhs._sync );
			this->_value = rhs._value;	// throw
			return *this;
		}


		ND_ T  Read ()							const noexcept(IsNothrowCopyCtor<T>)
		{
			SHAREDLOCK( _sync );
			return _value;
		}


		void  Write (const T &value)			noexcept(IsNothrowCopyCtor<T>)
		{
			EXLOCK( _sync );
			_value = value;
		}
		
		void  Write (T &&value)					__NE___
		{
			EXLOCK( _sync );
			_value = RVRef(value);
		}


		void  Reset ()							noexcept(IsNothrowCopyCtor<T>)
		{
			EXLOCK( _sync );
			_value = T{};
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
	};
	

	
	//
	// Synchronized (unsupported)
	//

	template <typename SyncObj>
	class Synchronized< SyncObj >
	{};


} // AE::Threading
