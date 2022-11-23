// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumBitSet.h"
#include "threading/Common.h"

namespace AE::Threading
{

	enum class EThread : ubyte
	{
		Main,		// thread with window message loop
		Worker,
		Renderer,	// single thread for opengl, multiple for vulkan (can be mixed with 'Worker')	// TODO: RendererHi, RendererLow
		Network,
		_Count
	};


	enum class EThread2 : ubyte
	{
		Main		= EThread::Main,
		Worker		= EThread::Worker,
		Renderer	= EThread::Renderer,
		Network		= EThread::Network,
		_Last		= EThread::_Count,

		FileIO,		// thread can not process tasks
		_Count
	};
	STATIC_ASSERT( sizeof(EThread2) == sizeof(EThread) );
	STATIC_ASSERT( uint(EThread::_Count) == 4 );



	//
	// Thread types Array with priority
	//
	struct EThreadArray
	{
	// variables
	private:
		FixedArray< EThread2, uint(EThread2::_Count) >	_arr;
			

	// methods
	public:
		constexpr EThreadArray ()									__NE___	{}
		constexpr EThreadArray (EThreadArray &&)					__NE___	= default;
		constexpr EThreadArray (const EThreadArray &)				__NE___	= default;

		constexpr EThreadArray&  operator = (EThreadArray &&)		__NE___	= default;
		constexpr EThreadArray&  operator = (const EThreadArray &)	__NE___	= default;

		template <typename ...Args>
		constexpr explicit EThreadArray (Args ...args)				__NE___	{ _Insert( args... ); }
			
		template <typename ...Args>
		constexpr EThreadArray&	insert (Args ...args)				__NE___	{ _Insert( args... );  return *this; }

		ND_ constexpr bool		empty ()							C_NE___	{ return _arr.empty(); }

		ND_ constexpr auto		begin ()							C_NE___	{ return _arr.begin(); }
		ND_ constexpr auto		end ()								C_NE___	{ return _arr.end(); }


		ND_ constexpr EnumBitSet<EThread>  ToThreadMask ()				C_NE___
		{
			EnumBitSet<EThread>		result;
			for (auto tt : *this)
			{
				if_likely( tt < EThread2::_Last )
					result.insert( EThread(tt) );
			}
			return result;
		}


		ND_ constexpr EnumBitSet<EThread2>  ToThread2Mask ()				C_NE___
		{
			EnumBitSet<EThread2>	result;
			for (auto tt : *this) {
				result.insert( tt );
			}
			return result;
		}


	private:
		template <typename Arg0, typename ...Args>
		constexpr void  _Insert (Arg0 arg0, Args ...args)			__NE___
		{
			STATIC_ASSERT(( IsSameTypes< Arg0, EThread > or IsSameTypes< Arg0, EThread2 > ));

			_arr.try_push_back( EThread2(arg0) );

			if constexpr( sizeof...(args) > 0 )
				return _Insert( args... );
		}
	};

} // AE::Threading
