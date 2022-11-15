// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Function Async Task
	//

	class FunctionTask final : public IAsyncTask
	{
	// types
	public:
		using Func_t	= Function< void () >;

	// variables
	private:
		Func_t	_fn;

	// methods
	public:
		template <typename Fn>
		explicit FunctionTask (Fn &&fn, EThread type = EThread::Worker) :
			IAsyncTask{ type }, _fn{ FwdArg<Fn>(fn) }
		{}

	private:
		void Run () override
		{
			return _fn();
		}

		StringView  DbgName ()	C_NE_OV	{ return "function"; }
	};


} // AE::Threading
