// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "profiler/Common.h"

namespace AE::Profiler
{
	using namespace AE::Base;


	//
	// Log Window
	//

	class LogWindow final : public ILogger
	{
	// types
	private:
		struct LogWindowRef
		{
		private:
			LogWindow *		_ptr	= null;
			SharedMutex *	_lock	= null;

		public:
			LogWindowRef ()								{}
			LogWindowRef (LogWindow &p, SharedMutex &m) : _ptr{&p}, _lock{&m} {}
			LogWindowRef (LogWindowRef &&other)			: _ptr{other._ptr}, _lock{other._lock} { other._lock = null; }
			~LogWindowRef ()							{ if ( _lock ) _lock->unlock_shared(); }

			ND_ explicit operator bool ()	const		{ return _lock != null; }
			ND_ LogWindow*  operator -> ()				{ return _ptr; }
		};

		struct WeakPtrObj
		{
		private:
			Atomic<int>		_refCount	{1};
			LogWindow &		_ptr;
			SharedMutex		_guard;

		public:
			WeakPtrObj (LogWindow &wnd) : _ptr{wnd} {}

			void  IncRef ()						{ _refCount.fetch_add( 1 ); }
			void  ReleaseRef ();

			ND_ SharedMutex&	Guard ()		{ return _guard; }
			ND_ LogWindowRef	LockShared ();
		};

		static constexpr uint		_bufSize	= uint{1_MiB};
		static constexpr uint		_maxLines	= 64;


	public:
		struct WeakPtr
		{
		private:
			Ptr<WeakPtrObj>		_obj;

		public:
			WeakPtr ()										{}
			WeakPtr (WeakPtrObj &obj) : _obj{&obj}			{ obj.IncRef(); }
			WeakPtr (WeakPtr &&other) : _obj{other._obj}	{ other._obj = null; }
			~WeakPtr ()										{ if ( _obj ) _obj->ReleaseRef(); }

			WeakPtr&  operator = (WeakPtr &&other)			{ _obj = other._obj;  other._obj = null;  return *this; }

			ND_ LogWindowRef  Lock ()						{ return _obj ? _obj->LockShared() : Default; }
		};


	// variables
	private:
		Ptr<WeakPtrObj>		_strongRef;
		ELevel				_minLevel		= ELevel::Info;

		Atomic<bool>		_hasError		{false};

		uint				_lastLine		= 0;

		uint				_lines	[_maxLines];
		char				_buffer [_bufSize];


	// methods
	public:
		LogWindow (OUT WeakPtr &ref)				__NE___;
		~LogWindow ()								__NE___;

		EResult  Process (const MessageInfo &info) __Th_OV;

	  #ifdef AE_ENABLE_IMGUI
		void  DrawImGUI ();
	  #endif
	};


} // AE::Profiler
