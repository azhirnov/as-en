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
		AnyTypeRef () {}
		AnyTypeRef (AnyTypeRef &&) = default;
		AnyTypeRef (const AnyTypeRef &) = default;

		template <typename T>	AnyTypeRef (T &value, NonAnyTypeRef<T> = 0) : _typeId{ typeid(T) }, _ref{ std::addressof(value) } {}

		template <typename T>	ND_ bool	Is ()			const	{ return _typeId == typeid(T); }
		template <typename T>	ND_ bool	Is (const T &)	const	{ return _typeId == typeid(T); }

		template <typename T>	ND_ T *		GetIf ()		const	{ return Is<T>() ? static_cast<T*>( _ref ) : null; }
		template <typename T>	ND_ T &		As ()			const	{ ASSERT( Is<T>() );  return *static_cast<T*>( _ref ); }

		ND_ std::type_index		GetType ()					const	{ return _typeId; }
		ND_ StringView			GetTypeName ()				const	{ return _typeId.name(); }
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
		AnyTypeCRef () {}
		AnyTypeCRef (AnyTypeCRef &&) = default;
		AnyTypeCRef (const AnyTypeCRef &) = default;

		template <typename T>	AnyTypeCRef (T &value, NonAnyTypeRef<T> = 0) : _typeId{ typeid(T) }, _ref{ std::addressof(value) } {}
		template <typename T>	explicit AnyTypeCRef (const T &value, NonAnyTypeCRef<T> = 0) : _typeId{ typeid(T) }, _ref{ std::addressof(value) } {}

		template <typename T>	ND_ bool		Is ()			const	{ return _typeId == typeid(T); }
		template <typename T>	ND_ bool		Is (const T &)	const	{ return _typeId == typeid(T); }

		template <typename T>	ND_ T const *	GetIf ()		const	{ return Is<T>() ? static_cast<T const*>( _ref ) : null; }
		template <typename T>	ND_ T const &	As ()			const	{ ASSERT( Is<T>() );  return *static_cast<T const*>( _ref ); }
		
		ND_ std::type_index		GetType ()						const	{ return _typeId; }
		ND_ StringView			GetTypeName ()					const	{ return _typeId.name(); }
	};


} // AE::Base
