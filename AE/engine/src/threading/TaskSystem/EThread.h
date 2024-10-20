// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{

	enum class ETaskQueue : ubyte
	{
		Main,			// Thread with window message loop, UI thread, OpenGL thread.

		PerFrame,		// Worker thread with high priority, allows access to graphics API.
						// You should not use tasks which is allowed for 'Background' thread.

		Renderer,		// Same as 'PerFrame', used to limit number of threads which can access to graphics command pools.
						// Allowed:
						//		- RenderTask, RenderTaskCoro
						// Note:
						//		Command pool allocated per thread, so number of threads with active command pools should be small
						//		to minimize memory usage. Software command buffers can be used in any thread.

		Background,		// Worker thread with low priority.
						// Allowed:
						//		- Graphics: pipeline compilation, mem allocation, resource creation
						//		- File IO
						//		- Network

		_Count,
		Unknown		= _Count
	};


	enum class EThread : ubyte
	{
		Main			= ubyte(ETaskQueue::Main),
		PerFrame		= ubyte(ETaskQueue::PerFrame),
		Renderer		= ubyte(ETaskQueue::Renderer),
		Background		= ubyte(ETaskQueue::Background),
		_Last			= ubyte(ETaskQueue::_Count),

		FileIO,			// Thread can not process tasks.
						// Used only to check if OS complete async IO and mark task dependency as complete.
		_Count
	};
	StaticAssert( sizeof(EThread) == sizeof(ETaskQueue) );
	StaticAssert( uint(ETaskQueue::_Count) == 4 );
	StaticAssert( uint(EThread::_Count) == 6 );


	using ETaskQueueBits	= EnumSet< ETaskQueue >;
	using EThreadBits		= EnumSet< EThread >;



	//
	// Thread types Array with priority
	//
	struct EThreadArray
	{
	// variables
	private:
		FixedArray< EThread, uint(EThread::_Count) >	_arr;


	// methods
	public:
		constexpr EThreadArray ()									__NE___	{}
		constexpr EThreadArray (EThreadArray &&)					__NE___	= default;
		constexpr EThreadArray (const EThreadArray &)				__NE___	= default;

		EThreadArray&  operator = (EThreadArray &&)					__NE___	= default;
		EThreadArray&  operator = (const EThreadArray &)			__NE___	= default;

		template <typename ...Args>
		constexpr explicit EThreadArray (Args ...args)				__NE___	{ _Insert( args... ); }

		template <typename ...Args>
		constexpr EThreadArray&	insert (Args ...args)				__NE___	{ _Insert( args... );  return *this; }

		ND_ constexpr bool		empty ()							C_NE___	{ return _arr.empty(); }

		ND_ constexpr auto		begin ()							C_NE___	{ return _arr.begin(); }
		ND_ constexpr auto		end ()								C_NE___	{ return _arr.end(); }


		ND_ constexpr ETaskQueueBits  ToQueueMask ()				C_NE___
		{
			ETaskQueueBits	result;
			for (auto tt : *this)
			{
				if_likely( tt < EThread::_Last )
					result.insert( ETaskQueue(tt) );
			}
			return result;
		}


		ND_ constexpr EThreadBits  ToThreadMask ()					C_NE___
		{
			EThreadBits		result;
			for (auto tt : *this) {
				result.insert( tt );
			}
			return result;
		}


	private:
		template <typename Arg0, typename ...Args>
		constexpr void  _Insert (Arg0 arg0, Args ...args)			__NE___
		{
			StaticAssert(( IsSameTypes< Arg0, ETaskQueue > or IsSameTypes< Arg0, EThread > ));

			_arr.try_push_back( EThread(arg0) );

			if constexpr( CountOf<Args...>() > 0 )
				return _Insert( args... );
		}
	};

} // AE::Threading


#ifdef AE_ENABLE_LOGS
namespace AE::Base
{

	ND_ inline StringView  ToString (Threading::ETaskQueue type)
	{
		using Threading::ETaskQueue;
		switch_enum( type )
		{
			case ETaskQueue::Main :			return "Main";
			case ETaskQueue::PerFrame :		return "PerFrame";
			case ETaskQueue::Renderer :		return "Renderer";
			case ETaskQueue::Background :	return "Background";
			case ETaskQueue::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown task queue type" );
	}

	ND_ inline StringView  ToString (Threading::EThread type)
	{
		using Threading::EThread;
		switch_enum( type )
		{
			case EThread::Main :			return "Main";
			case EThread::PerFrame :		return "PerFrame";
			case EThread::Renderer :		return "Renderer";
			case EThread::Background :		return "Background";
			case EThread::FileIO :			return "FileIO";
			case EThread::_Last :
			case EThread::_Count :			break;
		}
		switch_end
		RETURN_ERR( "unknown thread type" );
	}

} // AE::Base
#endif // AE_ENABLE_LOGS
