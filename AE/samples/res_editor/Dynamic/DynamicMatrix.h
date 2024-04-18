// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
#pragma once

#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

	//
	// Dynamic Matrix
	//

	template <typename T, uint Columns, uint Rows>
	class TDynamicMatrix final : public EnableRC< TDynamicMatrix< T, Columns, Rows >>
	{
	// types
	private:
		using Mat_t	= Matrix< T, Columns, Rows >;

	// variables
	private:
		mutable RWSpinLock	_guard;
		Mat_t				_mat;


	// methods
	public:
		TDynamicMatrix ()						__NE___	{}

			void	Set (const Mat_t &m)	__NE___	{ EXLOCK( _guard );  _mat = m; }
		ND_ Mat_t	Get ()					C_NE___	{ SHAREDLOCK( _guard );  return _mat; }
	};


	using DynamicMatrix4x4	= TDynamicMatrix< float, 4, 4 >;


} // AE::ResEditor
*/
