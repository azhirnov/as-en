// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"

namespace AE::Scripting
{

	//
	// Script Type Info
	//

	template <typename T>
	struct ScriptTypeInfo;
	/*{
		using type = T;

		static constexpr bool is_object		 = false;
		static constexpr bool is_ref_counted = false;

		static void  Name (INOUT String &);
		static void  ArgName (INOUT String &s);
		static uint  SizeOf ();
	};*/

	template <>
	struct ScriptTypeInfo <void>
	{
		using type = void;

		static constexpr bool is_object		 = false;
		static constexpr bool is_ref_counted = false;

		static void  Name (INOUT String &s)		{ s += "void"; }
		static void  ArgName (INOUT String &s)	{ s += "void"; }
	};

#	define AE_DECL_SCRIPT_TYPE( _type_, _name_ ) \
		template <> \
		struct ScriptTypeInfo < _type_ > \
		{ \
			using type = _type_; \
			\
			static constexpr bool is_object		 = false; \
			static constexpr bool is_ref_counted = false; \
			\
			static void  Name (INOUT String &s)		{ s += (_name_); } \
			static void  ArgName (INOUT String &s)	{ s += (_name_); } \
		}
	
#	define AE_DECL_SCRIPT_OBJ( _type_, _name_ ) \
		template <> \
		struct ScriptTypeInfo < _type_ > \
		{ \
			using type = _type_; \
			\
			static constexpr bool is_object		 = true; \
			static constexpr bool is_ref_counted = false; \
			\
			static void  Name (INOUT String &s)		{ s += (_name_); } \
			static void  ArgName (INOUT String &s)	{ s += (_name_); } \
		}

#	define AE_DECL_SCRIPT_OBJ_RC( _type_, _name_ ) \
		template <> \
		struct ScriptTypeInfo < _type_ > \
		{ \
			using type = _type_; \
			\
			static constexpr bool is_object		 = true; \
			static constexpr bool is_ref_counted = false; \
			\
			static void  Name (INOUT String &s)		{ s += (_name_); } \
			static void  ArgName (INOUT String &s)	{ s += (_name_); } \
		}; \
		\
		template <> \
		struct ScriptTypeInfo < _type_* > \
		{ \
			using type   = _type_ *; \
			using Base_t = ScriptTypeInfo< _type_ >; \
			\
			static constexpr bool is_object		 = true; \
			static constexpr bool is_ref_counted = true; \
			\
			static void  Name (INOUT String &s)		{ s += _name_; s += '@'; } \
			static void  ArgName (INOUT String &s)	{ s += _name_; s += '@'; } \
		}; \
		\
		template <> \
		struct ScriptTypeInfo < AngelScriptHelper::SharedPtr< _type_ > > \
		{ \
			using type   = AngelScriptHelper::SharedPtr< _type_ >; \
			using Base_t = ScriptTypeInfo< _type_ >; \
			\
			static constexpr bool is_object		 = true; \
			static constexpr bool is_ref_counted = true; \
			\
			static void  Name (INOUT String &s)		{ s += _name_; s += '@'; } \
			static void  ArgName (INOUT String &s)	{ s += _name_; s += '@'; } \
		}; \
		\
		template <> struct ScriptTypeInfo < const _type_* > {}; \
		template <> struct ScriptTypeInfo < _type_& > {}; \
		template <> struct ScriptTypeInfo < const _type_& > {}


#	define DECL_SCRIPT_TYPE( _type_ )	AE_DECL_SCRIPT_TYPE( _type_, AE_TOSTRING( _type_ ))
	DECL_SCRIPT_TYPE( bool );
	DECL_SCRIPT_TYPE( float );
	DECL_SCRIPT_TYPE( double );
	DECL_SCRIPT_TYPE( int );
	DECL_SCRIPT_TYPE( uint );
	AE_DECL_SCRIPT_TYPE( sbyte,		"int8" );
	AE_DECL_SCRIPT_TYPE( ubyte,		"uint8" );
	AE_DECL_SCRIPT_TYPE( sshort,	"int16" );
	AE_DECL_SCRIPT_TYPE( ushort,	"uint16" );
	AE_DECL_SCRIPT_TYPE( slong,		"int64" );
	AE_DECL_SCRIPT_TYPE( ulong,		"uint64" );
	AE_DECL_SCRIPT_OBJ(  String,	"string" );
#	undef DECL_SCRIPT_TYPE


	// only 'in' and 'inout' are supported
#	define AE_DECL_SCRIPT_WRAP( _templ_, _buildName_, _buildArg_ ) \
		template <typename T> \
		struct ScriptTypeInfo < _templ_ > \
		{ \
			using type   = _templ_; \
			using Base_t = ScriptTypeInfo<T>; \
			\
			static constexpr bool is_object		 = false; \
			static constexpr bool is_ref_counted = false; \
			\
			static void  Name (INOUT String &s)		{ _buildName_; } \
			static void  ArgName (INOUT String &s)	{ _buildArg_; } \
		}

#	define MULTILINE_ARG( ... )  __VA_ARGS__
	AE_DECL_SCRIPT_WRAP( const T,
						 MULTILINE_ARG(
							s += "const ";
							Base_t::Name( s );
						 ),
						 MULTILINE_ARG(
							s += "const ";
							Base_t::Name( s );
						 ));

	AE_DECL_SCRIPT_WRAP( const T &,
						 MULTILINE_ARG(
							s += "const ";
							Base_t::Name( s );
							s += " &";
						 ),
						 MULTILINE_ARG(
							s += "const ";
							Base_t::Name( s );
							s += " &in";
						 ));
		
	AE_DECL_SCRIPT_WRAP( T &,
						 MULTILINE_ARG(
							Base_t::Name( s );
							s += " &";
						 ),
						 MULTILINE_ARG(
							Base_t::Name( s );
							s += " &inout";
						 ));
		
	AE_DECL_SCRIPT_WRAP( const T *,
						 MULTILINE_ARG(
							s += "const ";
							Base_t::Name( s );
							s += " &";
						 ),
						 MULTILINE_ARG(
							s += "const ";
							Base_t::Name( s );
							s += " &in";
						 ));
		
	AE_DECL_SCRIPT_WRAP( T *,
						 MULTILINE_ARG(
							Base_t::Name( s );
							s += " &";
						 ),
						 MULTILINE_ARG(
							Base_t::Name( s );
							s += " &inout";
						 ));
#	undef MULTILINE_ARG




	//
	// Angel Script Helper
	//

	struct AngelScriptHelper final
	{
		struct SimpleRefCounter
		{
		// variables
		private:
			int		_counter	= 0;
			
		public:
			DEBUG_ONLY( static inline std::atomic<slong>  _dbgTotalCount {0}; )


		// methods
		private:
			SimpleRefCounter (SimpleRefCounter &&) = delete;
			SimpleRefCounter (const SimpleRefCounter &) = delete;
			void operator = (SimpleRefCounter &&) = delete;
			void operator = (const SimpleRefCounter &) = delete;

		public:
			SimpleRefCounter ()			{ DEBUG_ONLY( _dbgTotalCount.fetch_add( 1, std::memory_order_relaxed ); )}
			virtual ~SimpleRefCounter (){ ASSERT( _counter == 0 );  DEBUG_ONLY( _dbgTotalCount.fetch_sub( 1, std::memory_order_relaxed ); )}

			void  __AddRef ()			{ ASSERT( _counter >= 0 );  ++_counter; }
			void  __Release ()			{ ASSERT( _counter >= 0 );  if_unlikely( (--_counter) == 0 ) { delete this; }}
			int   __Counter () const	{ return _counter; }
		};


		template <typename T>
		struct SharedPtr
		{
			//STATIC_ASSERT( IsBaseOf< SimpleRefCounter, T > );

		// variables
		private:
			T *		_ptr = null;


		// methods
		private:
			void _IncRef ()	const	{ if_likely( _ptr ) { _ptr->__AddRef(); }}
			void _DecRef ()			{ if_likely( _ptr )	{ _ptr->__Release();  _ptr = null; }}

		public:
			SharedPtr ()												{}
			explicit SharedPtr (T *ptr) : _ptr{ptr}						{ _IncRef(); }
			SharedPtr (const SharedPtr<T> &other) : _ptr{other._ptr}	{ _IncRef(); }
			SharedPtr (SharedPtr<T> &&other) : _ptr{other._ptr}			{ other._ptr = null; }
			~SharedPtr ()												{ _DecRef(); }

			template <typename B>
			SharedPtr (SharedPtr<B> &&other) : _ptr{static_cast<T*>(other.Detach())}	{}
			
			template <typename B>
			SharedPtr (const SharedPtr<B> &other) : _ptr{static_cast<T*>(other.Get())}	{ _IncRef(); }

			SharedPtr<T>&  operator = (const SharedPtr<T> &rhs)			{ _DecRef();  _ptr = rhs._ptr;  _IncRef();   return *this; }
			SharedPtr<T>&  operator = (SharedPtr<T> &&rhs)				{ _DecRef();  _ptr = rhs._ptr;  rhs._ptr = null;  return *this; }

			ND_ bool  operator == (const SharedPtr<T> &rhs)	const		{ return _ptr == rhs._ptr; }
			ND_ bool  operator >  (const SharedPtr<T> &rhs)	const		{ return _ptr >  rhs._ptr; }
			ND_ bool  operator <  (const SharedPtr<T> &rhs)	const		{ return _ptr <  rhs._ptr; }

			ND_ T*  operator -> ()			const			{ ASSERT( _ptr );  return _ptr; }
			ND_ T&  operator *  ()			const			{ ASSERT( _ptr );  return *_ptr; }
			ND_ T*  Get ()					const			{ return _ptr; }

			ND_ explicit operator bool ()	const			{ return _ptr != null; }

			ND_ int		UseCount ()			const			{ return _ptr ? _ptr->__Counter() : 0; }
			ND_ T*		Detach ()							{ T* tmp = _ptr;  _ptr = null;  return tmp; }
			ND_ T*		Retain ()			const			{ _IncRef();  return _ptr; }

				void	Attach (T* ptr)						{ _DecRef();  _ptr = ptr; }
		};


		template <typename T>
		static constexpr bool	IsSharedPtr = IsSpecializationOf< T, SharedPtr >;
		
		template <typename T>
		static constexpr bool	IsSharedPtrNoQual = IsSpecializationOf< RemoveAllQualifiers<T>, SharedPtr >;

	}; // AngelScriptHelper


}	// AE::Scripting


namespace std
{
	template <typename T>
	struct hash< AE::Scripting::AngelScriptHelper::SharedPtr<T> > {
		ND_ size_t  operator () (const AE::Scripting::AngelScriptHelper::SharedPtr<T> &key) const {
			return size_t(key.Get());
		}
	};

} // std
