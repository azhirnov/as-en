// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumBitSet.h"
#include "threading/Common.h"

namespace AE::Threading
{

	enum class ETaskQueue : ubyte
	{
		Main,		// thread with window message loop
		Worker,
		Renderer,	// single thread for opengl, multiple for vulkan (can be mixed with 'Worker')	// TODO: RendererHi, RendererLow
		Network,
		_Count
	};


	enum class EThread : ubyte
	{
		Main		= ubyte(ETaskQueue::Main),
		Worker		= ubyte(ETaskQueue::Worker),
		Renderer	= ubyte(ETaskQueue::Renderer),
		Network		= ubyte(ETaskQueue::Network),
		_Last		= ubyte(ETaskQueue::_Count),

		FileIO,		// thread can not process tasks
		_Count
	};
	STATIC_ASSERT( sizeof(EThread) == sizeof(ETaskQueue) );
	STATIC_ASSERT( uint(ETaskQueue::_Count) == 4 );



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


		ND_ constexpr EnumBitSet<ETaskQueue>  ToQueueMask ()		C_NE___
		{
			EnumBitSet<ETaskQueue>		result;
			for (auto tt : *this)
			{
				if_likely( tt < EThread::_Last )
					result.insert( ETaskQueue(tt) );
			}
			return result;
		}


		ND_ constexpr EnumBitSet<EThread>  ToThreadMask ()			C_NE___
		{
			EnumBitSet<EThread>	result;
			for (auto tt : *this) {
				result.insert( tt );
			}
			return result;
		}


	private:
		template <typename Arg0, typename ...Args>
		constexpr void  _Insert (Arg0 arg0, Args ...args)			__NE___
		{
			STATIC_ASSERT(( IsSameTypes< Arg0, ETaskQueue > or IsSameTypes< Arg0, EThread > ));

			_arr.try_push_back( EThread(arg0) );

			if constexpr( sizeof...(args) > 0 )
				return _Insert( args... );
		}
	};

} // AE::Threading
