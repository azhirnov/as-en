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

				void	Set (T* ptr)						{ _DecRef();  _ptr = ptr;  _IncRef(); }
				void	Attach (T* ptr)						{ _DecRef();  _ptr = ptr; }
		};


		template <typename T>
		static constexpr bool	IsSharedPtr = IsSpecializationOf< T, SharedPtr >;
		
		template <typename T>
		static constexpr bool	IsSharedPtrNoQual = IsSpecializationOf< RemoveAllQualifiers<T>, SharedPtr >;


		template <typename T>
		static T *  FactoryCreate ()
		{
			STATIC_ASSERT( not IsBaseOf< SimpleRefCounter, T > );
			return new T{};
		}

		template <typename T>
		static T *  FactoryCreateRC ()
		{
			return SharedPtr<T>{ new T{} }.Detach();
		}


		template <typename T>
		static void  Constructor (AngelScript::asIScriptGeneric *gen)
		{
			PlacementNew<T>( OUT gen->GetObject() );
		}

		
		template <typename T>
		static void  CopyConstructor (AngelScript::asIScriptGeneric *gen)
		{
			T const*	src = static_cast< const T *>( gen->GetArgObject(0) );
			void *		dst = gen->GetObject();
			PlacementNew<T>( OUT dst, *src );
		}
		

		template <typename T>
		static void  Destructor (AngelScript::asIScriptGeneric *gen)
		{
			static_cast<T *>(gen->GetObject())->~T();
		}


		template <typename T>
		static void  CopyAssign (AngelScript::asIScriptGeneric *gen)
		{
			T const*	src = static_cast< const T *>( gen->GetArgObject(0) );
			T*			dst = static_cast< T *>( gen->GetObject() );

			dst->~T();
			PlacementNew<T>( OUT dst, *src );
		}

	}; // AngelScriptHelper



	//
	// Script Function Descriptor
	//

	namespace _hidden_
	{

		template <typename T>
		struct GlobalFunction
		{
			static void  GetDescriptor (OUT String &, StringView, uint, uint);
		};

		template <typename T>
		struct MemberFunction
		{
			static void  GetDescriptor (OUT String &, StringView, uint, uint);
		};
		

		struct ArgsToString_Func
		{
			String &	result;
			const uint	first;
			const uint	last;
		
			ArgsToString_Func (uint first, uint last, INOUT String &str) : 
				result(str), first(first), last(last)
			{
				ASSERT( first <= last );
			}

			template <typename T, usize Index>
			void  operator () ()
			{
				if ( Index < first or Index > last )	return;
				if ( Index > first )					result += ", ";
				ScriptTypeInfo<T>::ArgName( INOUT result );
			}
		};

		template <typename Typelist>
		struct ArgsToString
		{
			static void  Get (OUT String &str, uint first, uint last)
			{
				ArgsToString_Func	func( first, last, INOUT str );
				Typelist::Visit( func );
			}
			
			static void  GetArgs (OUT String &str, uint offsetFromStart, uint offsetFromEnd)
			{
				ASSERT( offsetFromEnd < Typelist::Count );

				str += '(';
				Get( INOUT str, offsetFromStart, Typelist::Count - offsetFromEnd );
				str += ')';
			}
		};


		template <typename Ret, typename ...Types>
		struct GlobalFunction < Ret (AE_CDECL *) (Types...) >
		{
			using TypeList_t	= TypeList< Types... >;
			using Result_t		= Ret;
				
			static void  GetDescriptor (OUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				// can not convert between R* to SharedPtr<R>
				STATIC_ASSERT( not AngelScriptHelper::IsSharedPtrNoQual< Result_t >);

				ScriptTypeInfo< Result_t >::Name( OUT str );
				(str += ' ') += name;
				GetArgs( INOUT str, offsetFromStart, offsetFromEnd );
			}

			static void  GetArgs (OUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				ArgsToString< TypeList_t >::GetArgs( OUT str, offsetFromStart, offsetFromEnd );
			}
		};

		template <typename Ret>
		struct GlobalFunction < Ret (AE_CDECL *) () >
		{
			using TypeList_t	= TypeList<>;
			using Result_t		= Ret;
				
			static void  GetDescriptor (OUT String &str, StringView name, uint = 0, uint = 0)
			{
				// can not convert between R* to SharedPtr<R>
				STATIC_ASSERT( not AngelScriptHelper::IsSharedPtrNoQual< Result_t >);

				ScriptTypeInfo< Result_t >::Name( OUT str );
				((str += ' ') += name) += "()";
			}

			static void  GetArgs (OUT String &str, uint = 0, uint = 0)
			{
				str += "()";
			}
		};
			
		template <typename Ret, typename ...Types>
		struct GlobalFunction < Ret (Types...) > : GlobalFunction< Ret (AE_CDECL *) (Types...) >
		{};

		template <typename C, typename Ret, typename ...Types>
		struct MemberFunction < Ret (AE_THISCALL C:: *) (Types...) >
		{
			using TypeList_t	= TypeList< Types... >;
			using Result_t		= Ret;
				
			static void  GetDescriptor (OUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) (Types...) >::GetDescriptor( OUT str, name, offsetFromStart, offsetFromEnd );
			}

			static void  GetArgs (OUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) (Types...) >::GetArgs( OUT str, offsetFromStart, offsetFromEnd );
			}
		};
			
		template <typename C, typename Ret>
		struct MemberFunction < Ret (AE_THISCALL C:: *) () >
		{
			using TypeList_t	= TypeList<>;
			using Result_t		= Ret;
				
			static void  GetDescriptor (OUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) () >::GetDescriptor( OUT str, name, offsetFromStart, offsetFromEnd );
			}

			static void  GetArgs (OUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) () >::GetArgs( OUT str, offsetFromStart, offsetFromEnd );
			}
		};
			
		template <typename C, typename Ret, typename ...Types>
		struct MemberFunction < Ret (AE_THISCALL C:: *) (Types...) const >
		{
			using TypeList_t	= TypeList< Types... >;
			using Result_t		= Ret;
				
			static void  GetDescriptor (OUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) (Types...) >::GetDescriptor( OUT str, name, offsetFromStart, offsetFromEnd );
				str += " const";
			}

			static void  GetArgs (OUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) (Types...) >::GetArgs( OUT str, offsetFromStart, offsetFromEnd );
			}
		};
			
		template <typename C, typename Ret>
		struct MemberFunction < Ret (AE_THISCALL C:: *) () const >
		{
			using TypeList_t	= TypeList<>;
			using Result_t		= Ret;
				
			static void  GetDescriptor (OUT String &str, StringView name, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) () >::GetDescriptor( OUT str, name, offsetFromStart, offsetFromEnd );
				str += " const";
			}

			static void  GetArgs (OUT String &str, uint offsetFromStart = 0, uint offsetFromEnd = 0)
			{
				GlobalFunction< Result_t (*) () >::GetArgs( OUT str, offsetFromStart, offsetFromEnd );
			}
		};

	} // _hidden_

	

	//
	// Global Function
	//

	template <typename T>
	struct GlobalFunction : Scripting::_hidden_::GlobalFunction<T>
	{};


	//
	// Member Function
	//

	template <typename T>
	struct MemberFunction : Scripting::_hidden_::MemberFunction<T>
	{};


	namespace _hidden_
	{

		//
		// Context Setter Getter
		//
		template <typename T, bool IsObject>
		struct ContextSetterGetter_Var
		{
			ND_ static T	Get (AngelScript::asIScriptContext *ctx)					{ return *static_cast<T *>(ctx->GetReturnObject()); }
				static int	Set (AngelScript::asIScriptContext *ctx, int arg, T& value)	{ return ctx->SetArgObject( arg, static_cast<void *>(&value) ); }
		};

		template <typename T>
		struct ContextSetterGetter_Var< AngelScriptHelper::SharedPtr<T>, true >
		{
			STATIC_ASSERT( ScriptTypeInfo<T*>::is_ref_counted );

			ND_ static AngelScriptHelper::SharedPtr<T>  Get (AngelScript::asIScriptContext *ctx) {
				return AngelScriptHelper::SharedPtr<T>{ static_cast<T *>(ctx->GetReturnObject()) };
			}

			static int  Set (AngelScript::asIScriptContext *ctx, int arg, const AngelScriptHelper::SharedPtr<T> &ptr) {
				return ctx->SetArgObject( arg, reinterpret_cast<void *>(ptr.Get()) );
			}
		};

#		define DECL_CONTEXT_RESULT( _type_, _get_, _set_ ) \
			template <> \
			struct ContextSetterGetter_Var< _type_, false > \
			{ \
				ND_ static _type_ Get (AngelScript::asIScriptContext *ctx)									{ return _type_(ctx->_get_()); } \
					static int    Set (AngelScript::asIScriptContext *ctx, int arg, const _type_ &value)	{ return ctx->_set_( arg, value ); } \
			}

		DECL_CONTEXT_RESULT( sbyte,		GetReturnByte,		SetArgByte );
		DECL_CONTEXT_RESULT( ubyte,		GetReturnByte,		SetArgByte );
		DECL_CONTEXT_RESULT( sshort,	GetReturnWord,		SetArgWord );
		DECL_CONTEXT_RESULT( ushort,	GetReturnWord,		SetArgWord );
		DECL_CONTEXT_RESULT( int32_t,	GetReturnDWord,		SetArgDWord );
		DECL_CONTEXT_RESULT( uint,		GetReturnDWord,		SetArgDWord );
		DECL_CONTEXT_RESULT( slong,		GetReturnQWord,		SetArgQWord );
		DECL_CONTEXT_RESULT( ulong,		GetReturnQWord,		SetArgQWord );
		DECL_CONTEXT_RESULT( float,		GetReturnFloat,		SetArgFloat );
		DECL_CONTEXT_RESULT( double,	GetReturnDouble,	SetArgDouble );
#		undef DECL_CONTEXT_RESULT
	
		template <typename T, bool IsObject>
		struct _ContextSetterGetter_Ptr
		{
			ND_ static T *  Get (AngelScript::asIScriptContext *ctx)					{ return static_cast<T *>(ctx->GetReturnAddress()); }
				static int  Set (AngelScript::asIScriptContext *ctx, int arg, T* ptr)	{ return ctx->SetArgAddress( arg, (void *)(ptr) ); }
		};

		template <typename T>
		struct _ContextSetterGetter_Ptr < T, true >
		{
			ND_ static T *  Get (AngelScript::asIScriptContext *ctx)
			{
				T* result = static_cast<T *>(ctx->GetReturnObject());
				if ( result ) result->__AddRef();
				return result;
			}

			static int  Set (AngelScript::asIScriptContext *ctx, int arg, T* ptr)	{ return ctx->SetArgObject( arg, static_cast<void *>(ptr) ); }
		};
		
		template <typename T>
		struct ContextSetterGetter :
			ContextSetterGetter_Var< RemoveCVRef<T>, ScriptTypeInfo<RemoveCVRef<T>>::is_object >
		{};
		
		template <>
		struct ContextSetterGetter <void>
		{
			static void	 Get (AngelScript::asIScriptContext *)		{}
			static int	 Set (AngelScript::asIScriptContext *, int)	{ return 0; }
		};

		template <typename T>
		struct ContextSetterGetter < T * > :
			_ContextSetterGetter_Ptr< T, ScriptTypeInfo<T*>::is_ref_counted >
		{};


		template <typename T>
		inline void  ValidateRC (const T &arg)
		{
			if constexpr( IsBaseOf< AngelScriptHelper::SimpleRefCounter, RemoveAllQualifiers<T> >)
			{
				if constexpr( IsPointer<T> ) {
					CHECK( arg != null and arg->__Counter() > 0 );
				}else {
					CHECK(false);
				}
			}
		}


		//
		// Set Context Args
		//
		template <typename ...Args>
		struct SetContextArgs;
			
		template <typename Arg0, typename ...Args>
		struct SetContextArgs< Arg0, Args...>
		{
			static void  Set (AngelScript::asIScriptContext *ctx, int index, Arg0&& arg0, Args&& ...args)
			{
				ValidateRC( arg0 );
				AS_CALL( ContextSetterGetter<Arg0>::Set( ctx, index, arg0 ));
				SetContextArgs<Args...>::Set( ctx, index+1, FwdArg<Args>(args)... );
			}
		};

		template <typename Arg0>
		struct SetContextArgs< Arg0 >
		{
			static void  Set (AngelScript::asIScriptContext *ctx, int index, Arg0&& arg0)
			{
				ValidateRC( arg0 );
				AS_CALL( ContextSetterGetter<Arg0>::Set( ctx, index, FwdArg<Arg0>(arg0) ));
			}
		};

		template <>
		struct SetContextArgs<>
		{
			static void  Set (AngelScript::asIScriptContext *, int)
			{}
		};



		//
		// Check Input Arg Types
		//
		template <typename L, typename R>
		struct _IsSame {
			static constexpr bool	value = IsSameTypes< L, R >;
		};
		
		template <typename L, typename R>
		struct _IsSame< AngelScriptHelper::SharedPtr<L>, R* > {
			static constexpr bool	value = IsSameTypes< L, R >;
		};

		template <typename L, typename R>
		struct _IsSame< L*, AngelScriptHelper::SharedPtr<R> > {
			static constexpr bool	value = IsSameTypes< L, R >;
		};


		template <typename TL1, typename TL2, usize Idx>
		struct CheckInputArgTypes1
		{
			using T1 = RemoveCVRef< typename TL1::template Get< Idx-1 >>;
			using T2 = RemoveCVRef< typename TL2::template Get< Idx-1 >>;

			STATIC_ASSERT( _IsSame< T1, T2 >::value );

			static constexpr bool	value = _IsSame< T1, T2 >::value and CheckInputArgTypes1< TL1, TL2, Idx-1 >::value;
		};

		template <typename TL1, typename TL2>
		struct CheckInputArgTypes1< TL1, TL2, 0 >
		{
			static constexpr bool	value = true;
		};

		template <typename TL1, typename TL2>
		struct CheckInputArgTypes
		{
			STATIC_ASSERT( TL1::Count == TL2::Count );

			static constexpr bool	value = CheckInputArgTypes1< TL1, TL2, TL1::Count >::value;
		};


	} // _hidden_

} // AE::Scripting


namespace std
{
	template <typename T>
	struct hash< AE::Scripting::AngelScriptHelper::SharedPtr<T> > {
		ND_ size_t  operator () (const AE::Scripting::AngelScriptHelper::SharedPtr<T> &key) const {
			return size_t(key.Get());
		}
	};

} // std
