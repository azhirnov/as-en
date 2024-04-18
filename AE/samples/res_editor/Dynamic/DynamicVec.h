// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Core/RenderGraph.h"

namespace AE::ResEditor
{

	//
	// Dynamic Vector
	//

	template <typename T, int I>
	class TDynamicVec final : public EnableRC< TDynamicVec< T, I >>
	{
	// types
	private:
		using Vec_t	= Vec< T, I >;

	// variables
	private:
		mutable RWSpinLock	_guard;
		Vec_t				_vec;


	// methods
	public:
		TDynamicVec ()									__NE___	{}
		explicit TDynamicVec (const Vec_t &v)			__NE___	: _vec{v} {}

			void	Set (const Vec_t &v)				__NE___	{ EXLOCK( _guard );  _vec = v; }
		ND_ Vec_t	Get ()								C_NE___	{ SHAREDLOCK( _guard );  return _vec; }

		ND_ bool	IsChanged (INOUT Vec_t &oldVal)		C_NE___;
	};


	using DynamicFloat2	= TDynamicVec< float, 2 >;
	using DynamicFloat3	= TDynamicVec< float, 3 >;
	using DynamicFloat4	= TDynamicVec< float, 4 >;

	using DynamicInt2	= TDynamicVec< int, 2 >;
	using DynamicInt3	= TDynamicVec< int, 3 >;
	using DynamicInt4	= TDynamicVec< int, 4 >;

	using DynamicUInt2	= TDynamicVec< uint, 2 >;
	using DynamicUInt3	= TDynamicVec< uint, 3 >;
	using DynamicUInt4	= TDynamicVec< uint, 4 >;


/*
=================================================
	IsChanged
=================================================
*/
	template <typename T, int I>
	bool  TDynamicVec<T,I>::IsChanged (INOUT Vec_t &oldVal) C_NE___
	{
		const Vec_t	new_val = Get();

		if ( Any( new_val != oldVal ))
		{
			oldVal = new_val;
			return true;
		}
		return false;
	}


} // AE::ResEditor
