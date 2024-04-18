// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"

namespace AE::Graphics
{

	//
	// Resource Wrapper
	//

	template <typename ResType, typename ID>
	class alignas(AE_CACHE_LINE) ResourceBase final
	{
	// types
	private:
		enum class EState : ubyte
		{
			Destroyed	= 0,
			Failed,
			Created,
		};

	public:
		using Self			= ResourceBase< ResType, ID >;
		using Resource_t	= ResType;
		using Generation_t	= typename ID::Generation_t;


	// variables
	private:
		// instance counter used to detect deprecated handles
		Atomic<Generation_t>	_generation	{0};

		Atomic<EState>			_state		{EState::Destroyed};

		ResType					_data;

		mutable Atomic<int>		_refCounter	{0};


	// methods
	public:
		ResourceBase ()									__NE___	{}

		ResourceBase (Self &&)									= delete;
		ResourceBase (const Self &)								= delete;

		Self& operator = (Self &&)								= delete;
		Self& operator = (const Self &)							= delete;

		~ResourceBase ()								__NE___	{ ASSERT( IsDestroyed() ); }

			void			AddRef ()					C_NE___	{ _refCounter.fetch_add( 1 ); }
		ND_ int				ReleaseRef (int refCount)	C_NE___	{ return (_refCounter.fetch_sub( refCount ) - refCount); }


		ND_ bool			IsCreated ()				C_NE___	{ return _GetState() == EState::Created; }
		ND_ bool			IsDestroyed ()				C_NE___	{ return _GetState() == EState::Destroyed; }

		ND_ Generation_t	GetGeneration ()			C_NE___	{ return _generation.load(); }
		ND_ int				GetRefCount ()				C_NE___	{ return _refCounter.load(); }

		ND_ ResType&		Data ()						__NE___	{ return _data; }
		ND_ ResType const&	Data ()						C_NE___	{ return _data; }


		template <typename ...Args>
		ND_ bool  Create (Args&& ...args)				__NE___
		{
			ASSERT( IsDestroyed() );
			ASSERT( GetRefCount() == 0 );

			bool	result = _data.Create( FwdArg<Args &&>( args )... );

			// set state and flush cache
			_state.store( result ? EState::Created : EState::Failed, EMemoryOrder::Release );

			return result;
		}

		template <typename ...Args>
		void  Destroy (Args&& ...args)					__NE___
		{
			ASSERT( AnyEqual( _GetState(), EState::Created, EState::Failed ));
			//ASSERT( GetRefCount() == 0 );

			_data.Destroy( FwdArg<Args>( args )... );

			// flush cache
			MemoryBarrier( EMemoryOrder::Release );

			// update atomics
			_refCounter.store( 0 );
			_state.store( EState::Destroyed );

			constexpr Generation_t	max_gen = ID::MaxGeneration();

			for (Generation_t exp = _generation.load();;)
			{
				if_likely( _generation.CAS( INOUT exp, (exp < max_gen ? exp + 1 : 0) ))
					break;

				ThreadUtils::Pause();
			}
		}

	private:
		ND_ EState	_GetState ()						C_NE___	{ return _state.load(); }
	};


} // AE::Graphics
