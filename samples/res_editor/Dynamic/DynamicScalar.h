// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/FrameGraph.h"

namespace AE::ResEditor
{

	//
	// Dynamic Scalar
	//
	
	template <typename T>
	class TDynamicScalar final : public EnableRC< TDynamicScalar<T> >
	{
	// types
	private:
		using Scalar_t	= T;

	// variables
	private:
		mutable RWSpinLock	_guard;
		Scalar_t			_value;


	// methods
	public:
		TDynamicScalar ()									__NE___	{}
		explicit TDynamicScalar (Scalar_t val)				__NE___	: _value{val} {}

			void		Set (Scalar_t val)					__NE___	{ EXLOCK( _guard );  _value = val; }
		ND_ Scalar_t	Get ()								C_NE___	{ SHAREDLOCK( _guard );  return _value; }

		ND_ bool		IsChanged (INOUT Scalar_t &oldVal)	C_NE___;
	};

	
	using DynamicFloat	= TDynamicScalar< float >;
	using DynamicInt	= TDynamicScalar< int >;
	using DynamicUInt	= TDynamicScalar< uint >;
	using DynamicULong	= TDynamicScalar< ulong >;
	
	
/*
=================================================
	IsChanged
=================================================
*/
	template <typename T>
	bool  TDynamicScalar<T>::IsChanged (INOUT Scalar_t &oldVal) C_NE___
	{
		const Scalar_t	new_val = Get();

		if ( new_val != oldVal )
		{
			oldVal = new_val;
			return true;
		}
		return false;
	}


} // AE::ResEditor
