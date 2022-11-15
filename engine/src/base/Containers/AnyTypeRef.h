// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{
	class AnyTypeRef;
	class AnyTypeCRef;


	//
	// Reference to Any type
	//

	class AnyTypeRef
	{
	// types
	private:
		template <typename T>
		using NonAnyTypeRef = DisableIf< IsConst<T> or IsSameTypes< T, AnyTypeRef > or IsSameTypes< T, AnyTypeCRef >, int >;


	// variables
	private:
		std::type_index		_typeId	= typeid(void);
		void *				_ref	= null;


	// methods
	public:
		AnyTypeRef ()										__NE___ {}
		AnyTypeRef (AnyTypeRef &&)							__NE___ = default;
		AnyTypeRef (const AnyTypeRef &)						__NE___ = default;

		template <typename T>	AnyTypeRef (T &value, NonAnyTypeRef<T> = 0) __NE___ : _typeId{ typeid(T) }, _ref{ std::addressof(value) } {}

		template <typename T>	ND_ bool	Is ()			C_NE___	{ return _typeId == typeid(T); }
		template <typename T>	ND_ bool	Is (const T &)	C_NE___	{ return _typeId == typeid(T); }

		template <typename T>	ND_ T *		GetIf ()		C_NE___	{ return Is<T>() ? static_cast<T*>( _ref ) : null; }
		template <typename T>	ND_ T &		As ()			C_NE___	{ ASSERT( Is<T>() );  return *static_cast<T*>( _ref ); }

		ND_ std::type_index		GetType ()					C_NE___	{ return _typeId; }
		ND_ StringView			GetTypeName ()				C_NE___	{ return _typeId.name(); }
	};
	


	//
	// Const Reference to Any type
	//

	class AnyTypeCRef
	{
	// types
	private:
		template <typename T>
		using NonAnyTypeRef = DisableIf< IsConst<T> or IsSameTypes< T, AnyTypeRef > or IsSameTypes< T, AnyTypeCRef >, int >;
		
		template <typename T>
		using NonAnyTypeCRef = DisableIf< IsSameTypes< T, AnyTypeRef > or IsSameTypes< T, AnyTypeCRef >, int >;


	// variables
	private:
		std::type_index		_typeId	= typeid(void);
		void const *		_ref	= null;


	// methods
	public:
		AnyTypeCRef ()											__NE___	{}
		AnyTypeCRef (AnyTypeCRef &&)							__NE___ = default;
		AnyTypeCRef (const AnyTypeCRef &)						__NE___ = default;

		template <typename T>	AnyTypeCRef (T &value, NonAnyTypeRef<T> = 0)				 __NE___ : _typeId{ typeid(T) }, _ref{ std::addressof(value) } {}
		template <typename T>	explicit AnyTypeCRef (const T &value, NonAnyTypeCRef<T> = 0) __NE___ : _typeId{ typeid(T) }, _ref{ std::addressof(value) } {}

		template <typename T>	ND_ bool		Is ()			C_NE___	{ return _typeId == typeid(T); }
		template <typename T>	ND_ bool		Is (const T &)	C_NE___	{ return _typeId == typeid(T); }

		template <typename T>	ND_ T const *	GetIf ()		C_NE___	{ return Is<T>() ? static_cast<T const*>( _ref ) : null; }
		template <typename T>	ND_ T const &	As ()			C_NE___	{ ASSERT( Is<T>() );  return *static_cast<T const*>( _ref ); }
		
		ND_ std::type_index		GetType ()						C_NE___	{ return _typeId; }
		ND_ StringView			GetTypeName ()					C_NE___	{ return _typeId.name(); }
	};


} // AE::Base
