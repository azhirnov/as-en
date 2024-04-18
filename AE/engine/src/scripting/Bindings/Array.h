// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptTypes.h"

namespace AE::Scripting::_hidden_
{

	//
	// POD Array
	//
	template <typename T>
	class ScriptPODArray final : protected AngelScript::CScriptArray
	{
	// types
	private:
		StaticAssert( not (ScriptTypeInfo<T>::is_object or ScriptTypeInfo<T>::is_ref_counted) );

		using Self				= ScriptPODArray< T >;
		using View_t			= StructView< T >;
	public:
		using iterator			= typename View_t::large_iterator;
		using const_iterator	= iterator;


	// methods
	public:
		ScriptPODArray ()							= delete;
		ScriptPODArray (Self &&)					= delete;
		ScriptPODArray (const Self &)				= delete;

			Self&		operator = (Self &&)		= delete;
			Self&		operator = (const Self &)	= delete;

		ND_ iterator	begin ()					C_NE___	{ return _Arr().begin(); }
		ND_ iterator	end ()						C_NE___	{ return _Arr().end(); }

		ND_ usize		size ()						C_NE___	{ return this->GetSize(); }
		ND_ bool		empty ()					C_NE___	{ return this->IsEmpty(); }

		ND_ operator View_t ()						C_NE___	{ return _Arr(); }

		ND_ explicit operator ArrayView<T> ()		C_NE___
		{
			CHECK_ERR( this->GetElementSize() == sizeof(T) );
			return ArrayView<T>{ Cast<T>( const_cast< Self *>(this)->GetBuffer() ), size() };
		}

		ND_ explicit operator Array<T> ()			C_Th___
		{
			return Array<T>{ View_t{ *this }};
		}

		ND_ T &			operator [] (usize i)		__NE___	{ ASSERT( i < size() );  return *Cast<T>( this->At( uint(i) )); }
		ND_ T const &	operator [] (usize i)		C_NE___	{ ASSERT( i < size() );  return *Cast<T>( this->At( uint(i) )); }

			void  push_back (T value)				__NE___	{ this->InsertLast( &value ); }

			template <typename ...Args>
			void  emplace_back (Args&& ...args)		__NE___	{ push_back(T{ FwdArg<Args>(args)... }); }

			void  clear ()							__NE___	{ this->Resize( 0 ); }
			void  resize (usize newSize)			__NE___	{ this->Resize( uint(newSize) ); }
			void  reserve (usize newSize)			__NE___	{ this->Reserve( uint(newSize) ); }

	private:
		ND_ View_t  _Arr ()							C_NE___
		{
			return	StructView<T>{
						Cast<T>( const_cast< Self *>(this)->GetBuffer() ),
						size(),
						Bytes{uint(this->GetElementSize())}
					};
		}
	};



	//
	// Object Array
	//
	template <typename T>
	class ScriptObjArray final : protected AngelScript::CScriptArray
	{
	// types
	private:
		StaticAssert( ScriptTypeInfo<T>::is_object or ScriptTypeInfo<T>::is_ref_counted );

		using Self	= ScriptObjArray< T >;


		struct iterator
		{
			friend class ScriptObjArray;

		private:
			Self *		_arr	= null;
			usize		_index	= UMax;

			iterator (Self &arr, usize i)								__NE___	: _arr{&arr}, _index{i} {}

		public:
			iterator ()													__NE___	{}
			iterator (const iterator &)									__NE___	= default;

			ND_ T&				operator * ()							__NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }
			ND_ T const&		operator * ()							C_NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }

			ND_ bool			operator == (const iterator &rhs)		C_NE___ { return (_arr == rhs._arr) and (_index == rhs._index); }
			ND_ bool			operator != (const iterator &rhs)		C_NE___ { return not (*this == rhs); }

				iterator&		operator ++ ()							__NE___	{ ++_index;  return *this; }
				iterator		operator ++ (int)						__NE___	{ iterator res{*this};  ++_index;  return res; }
		};


		struct const_iterator
		{
			friend class ScriptObjArray;

		private:
			Self const*		_arr	= null;
			usize			_index	= UMax;

			const_iterator (const Self &arr, usize i)					__NE___	: _arr{&arr}, _index{i} {}

		public:
			const_iterator ()											__NE___	{}
			const_iterator (const const_iterator &)						__NE___	= default;

			ND_ T const&		operator * ()							__NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }
			ND_ T const&		operator * ()							C_NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }

			ND_ bool			operator == (const const_iterator &rhs) C_NE___	{ return (_arr == rhs._arr) and (_index == rhs._index); }
			ND_ bool			operator != (const const_iterator &rhs) C_NE___	{ return not (*this == rhs); }

				const_iterator&	operator ++ ()							__NE___	{ ++_index;  return *this; }
				const_iterator	operator ++ (int)						__NE___	{ const_iterator res{*this};  ++_index;  return res; }
		};


	// methods
	public:
		ScriptObjArray ()									= delete;
		ScriptObjArray (Self &&)							= delete;
		ScriptObjArray (const Self &)						= delete;

			Self&			operator = (Self &&)			= delete;
			Self&			operator = (const Self &)		= delete;

		ND_ usize			size ()							C_NE___	{ return this->GetSize(); }
		ND_ bool			empty ()						C_NE___	{ return this->IsEmpty(); }

		ND_ iterator		begin ()						__NE___	{ return iterator{ *this, 0 }; }
		ND_ const_iterator	begin ()						C_NE___	{ return const_iterator{ *this, 0 }; }
		ND_ iterator		end ()							__NE___	{ return iterator{ *this, size() }; }
		ND_ const_iterator	end ()							C_NE___	{ return const_iterator{ *this, size() }; }

		ND_ T &				operator [] (usize i)			__NE___	{ ASSERT( i < size() );  return *static_cast<T *>( this->At( uint(i) )); }
		ND_ T const &		operator [] (usize i)			C_NE___	{ ASSERT( i < size() );  return *static_cast<T const *>( this->At( uint(i) )); }

			void  push_back (T value)						__NE___	{ this->InsertLast( &value ); }

			template <typename ...Args>
			void  emplace_back (Args&& ...args)				__NE___	{ push_back(T{ FwdArg<Args>(args)... }); }

			void  clear ()									__NE___	{ this->Resize( 0 ); }
			void  resize (usize newSize)					__NE___	{ this->Resize( uint(newSize) ); }
			void  reserve (usize newSize)					__NE___	{ this->Reserve( uint(newSize) ); }

		ND_ explicit operator Array<T> ()					C_Th___
		{
			Array<T>	res;	res.resize( size() );  // throw
			for (usize i = 0, cnt = size(); i < cnt; ++i)
				res[i] = (*this)[i];
			return res;
		}
	};



	//
	// Array of String
	//
	class ScriptStringArray final : protected AngelScript::CScriptArray
	{
	// types
	public:
		struct iterator
		{
			friend class ScriptStringArray;

		private:
			ScriptStringArray *		_arr	= null;
			usize					_index	= UMax;

			iterator (ScriptStringArray &arr, usize i)					__NE___	: _arr{&arr}, _index{i} {}

		public:
			iterator ()													__NE___	{}
			iterator (const iterator &)									__NE___	= default;

			ND_ String&			operator * ()							__NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }
			ND_ String const&	operator * ()							C_NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }

			ND_ bool			operator == (const iterator &rhs)		C_NE___ { return (_arr == rhs._arr) and (_index == rhs._index); }
			ND_ bool			operator != (const iterator &rhs)		C_NE___ { return not (*this == rhs); }

				iterator&		operator ++ ()							__NE___	{ ++_index;  return *this; }
				iterator		operator ++ (int)						__NE___	{ iterator res{*this};  ++_index;  return res; }
		};


		struct const_iterator
		{
			friend class ScriptStringArray;

		private:
			ScriptStringArray const*	_arr	= null;
			usize						_index	= UMax;

			const_iterator (const ScriptStringArray &arr, usize i)		__NE___	: _arr{&arr}, _index{i} {}

		public:
			const_iterator ()											__NE___	{}
			const_iterator (const const_iterator &)						__NE___	= default;

			ND_ String const&	operator * ()							__NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }
			ND_ String const&	operator * ()							C_NE___	{ ASSERT( _arr ); return (*_arr)[_index]; }

			ND_ bool			operator == (const const_iterator &rhs) C_NE___	{ return (_arr == rhs._arr) and (_index == rhs._index); }
			ND_ bool			operator != (const const_iterator &rhs) C_NE___	{ return not (*this == rhs); }

				const_iterator&	operator ++ ()							__NE___	{ ++_index;  return *this; }
				const_iterator	operator ++ (int)						__NE___	{ const_iterator res{*this};  ++_index;  return res; }
		};

		using Self	= ScriptStringArray;


	// methods
	public:
		ScriptStringArray ()							= delete;
		ScriptStringArray (Self &&)						= delete;
		ScriptStringArray (const Self &)				= delete;

			Self&			operator = (Self &&)		= delete;
			Self&			operator = (const Self &)	= delete;

		ND_ usize			size ()						C_NE___	{ return this->GetSize(); }
		ND_ bool			empty ()					C_NE___	{ return this->IsEmpty(); }

		ND_ iterator		begin ()					__NE___	{ return iterator{ *this, 0 }; }
		ND_ const_iterator	begin ()					C_NE___	{ return const_iterator{ *this, 0 }; }
		ND_ iterator		end ()						__NE___	{ return iterator{ *this, size() }; }
		ND_ const_iterator	end ()						C_NE___	{ return const_iterator{ *this, size() }; }

		ND_ String &		operator [] (usize i)		__NE___	{ ASSERT( i < size() );  return *static_cast<String *>( this->At( uint(i) )); }
		ND_ String const &	operator [] (usize i)		C_NE___	{ ASSERT( i < size() );  return *static_cast<String const *>( this->At( uint(i) )); }

			void  push_back (const String &value)		__NE___	{ this->InsertLast( const_cast<String *>( &value )); }

			void  clear ()								__NE___	{ this->Resize( 0 ); }
			void  resize (usize newSize)				__NE___	{ this->Resize( uint(newSize) ); }
			void  reserve (usize newSize)				__NE___	{ this->Reserve( uint(newSize) ); }

		ND_ explicit operator Array<String> ()			C_Th___
		{
			Array<String>	res;	res.resize( size() );  // throw
			for (usize i = 0, cnt = size(); i < cnt; ++i)
				res[i] = (*this)[i];
			return res;
		}
	};



	template <typename T>
	struct TScriptArray
	{
		using type = Conditional< (IsSameTypes< T, String >),
						ScriptStringArray,
						Conditional< (ScriptTypeInfo<T>::is_object or ScriptTypeInfo<T>::is_ref_counted),
							ScriptObjArray<T>,
							ScriptPODArray<T> >>;
	};



	template <typename T>
	struct TScriptTypeInfo
	{
		using type = T;

		static constexpr bool is_object			= true;
		static constexpr bool is_ref_counted	= true;

		static void  Name (INOUT String &s)		{ s << "array<"; ScriptTypeInfo<T>::Name( INOUT s ); s << ">"; }
		static void  ArgName (INOUT String &s)	{ s << "array<"; ScriptTypeInfo<T>::Name( INOUT s ); s << ">"; }
		static void  CppArg (INOUT String &s)	{ s << "array<"; ScriptTypeInfo<T>::Name( INOUT s ); s << ">"; }
	};

} // AE::Scripting::_hidden_


namespace AE::Scripting
{
	template <typename T>
	using ScriptArray = typename Scripting::_hidden_::TScriptArray<T>::type;


	//
	// Script Type Info
	//
	template <>
	struct ScriptTypeInfo< Scripting::_hidden_::ScriptStringArray > : Scripting::_hidden_::TScriptTypeInfo< String > {};

	template <typename T>
	struct ScriptTypeInfo< Scripting::_hidden_::ScriptObjArray<T> > : Scripting::_hidden_::TScriptTypeInfo<T> {};

	template <typename T>
	struct ScriptTypeInfo< Scripting::_hidden_::ScriptPODArray<T> > : Scripting::_hidden_::TScriptTypeInfo<T> {};


} // AE::Scripting
