// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	SyncPoint - triggered when object is destroyed (always use reference counter!).
	Use 'OnComplete()' to subscribe on event.

	Algorithm:
	* create 'SyncPoint' with reference counting: MakeRC<SyncPoint>()
	* keep reference to 'SyncPoint'
	* other tasks which wait for event must use 'SyncPoint::OnComplete()' as a strong dependency
		- it is similar to 'CV.wait()'
		- 'AsyncTask' supports unlimited number of output tasks (subscriptions).
	* release reference when need to trigger event
	* when released all references:
		- 'SyncPoint' marks internal task as completed
		- all tasks which depends on internal task can be executed
		- it is similar to 'CV.notify_all()'
*/

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Sync Point
	//
	class SyncPoint final : public EnableRC<SyncPoint>
	{
	// types
	private:
		struct Coro
		{
			class promise_type final : public _Coro_::AsyncTaskImpl
			{
			public:
					void		SetQueue (ETaskQueue type)					__NE___	{ _SetQueueType( type ); }
					void		MakeCompleted ()							__NE___	{ _MakeCompletedSafe(); }

				ND_ auto		initial_suspend ()							C_NE___	{ return std::suspend_never{}; }	// start immediately
				ND_ auto		final_suspend ()							C_NE___	{ return std::suspend_always{}; }
					void		return_void ()								C_NE___	{}

				ND_ auto		get_return_object ()						__NE___	{ return Coro{*this}; }
				ND_ static auto	get_return_object_on_allocation_failure ()	__NE___	{ return Coro{}; }
			};
			using CoroHandle_t = std::coroutine_handle< promise_type >;

			RC<promise_type>	_coro;

			Coro ()									__NE___	{}
			explicit Coro (promise_type &p)			__NE___ : _coro{ p.GetRC<promise_type>() } {}
			explicit Coro (CoroHandle_t handle)		__NE___ : _coro{ handle.promise().GetRC<promise_type>() } {}

			ND_ operator AsyncTask ()				C_NE___	{ return _coro; }

			ND_ auto*	operator -> ()				C_NE___	{ return _coro.operator->(); }
		};


	// variables
	private:
		Coro	_task;


	// methods
	private:
		static Coro  _CreateTask () { co_return; }

	public:
		explicit SyncPoint (ETaskQueue type = ETaskQueue::PerFrame)	__NE___	: _task{_CreateTask()} { _task->SetQueue( type ); }

		~SyncPoint ()												__NE_OV	{ _task->MakeCompleted(); }

		ND_ StrongDep  OnComplete ()								C_NE___	{ return StrongDep{_task._coro}; }

		// TODO co_await
	};


} // AE::Threading
