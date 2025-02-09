// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	SyncPoint - triggered when object is destroyed (always use reference counter!).

	Use 'OnComplete()' to subscribe on event.
	'AsyncTask' supports unlimited number of output tasks (subscriptions).
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

		ND_ AsyncTask  OnComplete ()								C_NE___	{ return _task; }
	};


} // AE::Threading
