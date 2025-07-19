// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
		class Task final : public IAsyncTask
		{
		public:
			explicit Task (ETaskQueue type)	__NE___	: IAsyncTask{ type } {}
			void  Run ()					__Th_OV	{ DBG_WARNING("never used"); }
			StringView  DbgName ()			C_NE_OV	{ return "SyncPointV1"; }
			void  MakeCompleted ()			__NE___	{ _MakeCompletedSafe(); }
		};


	// variables
	private:
		RC<Task>	_task;


	// methods
	public:
		explicit SyncPoint (ETaskQueue type = ETaskQueue::PerFrame)	__NE___	: _task{ new Task{ type }} {}

		~SyncPoint ()												__NE_OV	{ _task->MakeCompleted(); }

		ND_ StrongDep  OnComplete ()								C_NE___	{ return StrongDep{_task}; }
	};


} // AE::Threading
