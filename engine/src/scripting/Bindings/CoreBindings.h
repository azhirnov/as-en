// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.inl.h"
#include "scriptarray.h"

#include "base/Math/Vec.h"
#include "base/Math/Color.h"
#include "base/Utils/Noninstancable.h"
#include "base/Containers/StructView.h"

namespace AngelScript
{
	class CScriptArray;
}

namespace AE::Scripting
{

	//
	// Core Bindings
	//

	struct CoreBindings final : Noninstancable
	{
		static void  BindScalarMath (const ScriptEnginePtr &se);
		static void  BindVectorMath (const ScriptEnginePtr &se);
		static void  BindColor (const ScriptEnginePtr &se);
		static void  BindString (const ScriptEnginePtr &se);
		static void  BindArray (const ScriptEnginePtr &se);
		static void  BindLog (const ScriptEnginePtr &se);
		// TODO: physical types
	};


	//
	// Math
	//

	AE_DECL_SCRIPT_TYPE( AE::Math::packed_bool2,		"bool2"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_sbyte2,		"sbyte2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ubyte2,		"ubyte2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_short2,		"short2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ushort2,		"ushort2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_int2,			"int2"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_uint2,		"uint2"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_slong2,		"slong2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ulong2,		"ulong2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_float2,		"float2"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_double2,		"double2"	);
	
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_bool3,		"bool3"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_sbyte3,		"sbyte3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ubyte3,		"ubyte3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_short3,		"short3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ushort3,		"ushort3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_int3,			"int3"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_uint3,		"uint3"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_slong3,		"slong3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ulong3,		"ulong3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_float3,		"float3"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_double3,		"double3"	);
	
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_bool4,		"bool4"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_sbyte4,		"sbyte4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ubyte4,		"ubyte4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_short4,		"short4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ushort4,		"ushort4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_int4,			"int4"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_uint4,		"uint4"		);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_slong4,		"slong4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_ulong4,		"ulong4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_float4,		"float4"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::packed_double4,		"double4"	);
	
	AE_DECL_SCRIPT_TYPE( AE::Math::RGBA32f,				"RGBA32f"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::RGBA32u,				"RGBA32u"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::RGBA32i,				"RGBA32i"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::RGBA8u,				"RGBA8u"	);
	AE_DECL_SCRIPT_TYPE( AE::Math::DepthStencil,		"DepthStencil" );
	AE_DECL_SCRIPT_TYPE( AE::Math::HSVColor,			"HSVColor"	);
	

	//
	// Array
	//
	template <typename T>
	class ScriptArray final : protected AngelScript::CScriptArray
	{
	// types
	private:
		static constexpr bool	is_pod	= not (ScriptTypeInfo<T>::is_object or ScriptTypeInfo<T>::is_ref_counted);

		using Self				= ScriptArray< T >;
		using View_t			= StructView< T >;
	public:
		using iterator			= typename View_t::large_iterator;
		using const_iterator	= iterator;


	// methods
	public:
		ScriptArray () = delete;
		ScriptArray (ScriptArray &&) = delete;
		ScriptArray (const ScriptArray &) = delete;
		
		ScriptArray&  operator = (ScriptArray &&) = delete;
		ScriptArray&  operator = (const ScriptArray &) = delete;

		ND_ EnableIf<is_pod, iterator>	begin ()	const	{ return _Arr().begin(); }
		ND_ EnableIf<is_pod, iterator>	end ()		const	{ return _Arr().end(); }

		ND_ usize	size ()		const	{ return this->GetSize(); }
		ND_ bool	empty ()	const	{ return this->IsEmpty(); }

		ND_ operator View_t ()	const	{ return _Arr(); }

		ND_ EnableIf<is_pod, T &>		operator [] (usize i)			{ ASSERT( i < size() );  return *Cast<T>( this->At( uint(i) )); }
		ND_ EnableIf<is_pod, T const &>	operator [] (usize i)	const	{ ASSERT( i < size() );  return *Cast<T>( this->At( uint(i) )); }

		// TODO: object types

		void  push_back (T value)		{ this->InsertLast( &value ); }

		void  clear ()					{ this->Resize( 0 ); }
		void  resize (usize newSize)	{ this->Resize( uint(newSize) ); }
		void  reserve (usize newSize)	{ this->Reserve( uint(newSize) ); }

	private:
		ND_ EnableIf<is_pod, View_t>  _Arr () const
		{
			return	StructView<T>{
						Cast<T>( const_cast< Self *>(this)->GetBuffer() ),
						size(),
						uint(this->GetElementSize())
					};
		}
	};
	

	//
	// Array of String
	//
	template <>
	class ScriptArray< String > final : protected AngelScript::CScriptArray
	{
	// types
	public:
		struct iterator
		{
			friend class ScriptArray<String>;

		private:
			ScriptArray<String> *	_arr	= null;
			usize					_index	= UMax;

			iterator (ScriptArray<String> &arr, usize i) : _arr{&arr}, _index{i} {}
			
		public:
			iterator () {}
			iterator (const iterator &) = default;

			ND_ String&			operator * ()					{ ASSERT( _arr ); return (*_arr)[_index]; }
			ND_ String const&	operator * ()			  const	{ ASSERT( _arr ); return (*_arr)[_index]; }

			ND_ bool	operator == (const iterator &rhs) const { return (_arr == rhs._arr) and (_index == rhs._index); }
			ND_ bool	operator != (const iterator &rhs) const { return not (*this == rhs); }

			iterator&			operator ++ ()					{ ++_index;  return *this; }
			iterator			operator ++ (int)				{ iterator res{*this};  ++_index;  return res; }
		};
		

		struct const_iterator
		{
			friend class ScriptArray<String>;

		private:
			ScriptArray<String> const*	_arr	= null;
			usize						_index	= UMax;

			const_iterator (const ScriptArray<String> &arr, usize i) : _arr{&arr}, _index{i} {}
			
		public:
			const_iterator () {}
			const_iterator (const const_iterator &) = default;
			
			ND_ String const&	operator * ()			{ ASSERT( _arr ); return (*_arr)[_index]; }
			ND_ String const&	operator * ()	const	{ ASSERT( _arr ); return (*_arr)[_index]; }
			
			ND_ bool	operator == (const const_iterator &rhs) const { return (_arr == rhs._arr) and (_index == rhs._index); }
			ND_ bool	operator != (const const_iterator &rhs) const { return not (*this == rhs); }

			const_iterator&		operator ++ ()			{ ++_index;  return *this; }
			const_iterator		operator ++ (int)		{ const_iterator res{*this};  ++_index;  return res; }
		};


	// methods
	public:
		ScriptArray () = delete;
		ScriptArray (ScriptArray &&) = delete;
		ScriptArray (const ScriptArray &) = delete;
		
		ScriptArray&  operator = (ScriptArray &&) = delete;
		ScriptArray&  operator = (const ScriptArray &) = delete;
		
		ND_ usize			size ()					const	{ return this->GetSize(); }
		ND_ bool			empty ()				const	{ return this->IsEmpty(); }

		ND_ iterator		begin ()						{ return iterator{ *this, 0 }; }
		ND_ const_iterator	begin ()				const	{ return const_iterator{ *this, 0 }; }
		ND_ iterator		end ()							{ return iterator{ *this, size() }; }
		ND_ const_iterator	end ()					const	{ return const_iterator{ *this, size() }; }

		ND_ String &		operator [] (usize i)			{ ASSERT( i < size() );  return *static_cast<String *>( this->At( uint(i) )); }
		ND_ String const &	operator [] (usize i)	const	{ ASSERT( i < size() );  return *static_cast<String const *>( this->At( uint(i) )); }
		
		void  push_back (const String &value)				{ this->InsertLast( const_cast<String *>( &value )); }
		
		void  clear ()										{ this->Resize( 0 ); }
		void  resize (usize newSize)						{ this->Resize( uint(newSize) ); }
		void  reserve (usize newSize)						{ this->Reserve( uint(newSize) ); }
	};


	//
	// Script Type Info
	//
	template <typename T>
	struct ScriptTypeInfo< ScriptArray<T> >
	{
		using type = T;

		static constexpr bool is_object = true;
		static constexpr bool is_ref_counted = false;

		static void  Name (INOUT String &s)		{ s+= "array<"; ScriptTypeInfo<T>::Name( INOUT s ); s += ">"; }
		static void  ArgName (INOUT String &s)	{ s+= "array<"; ScriptTypeInfo<T>::Name( INOUT s ); s += ">"; }
	};


}	// AE::Scripting
