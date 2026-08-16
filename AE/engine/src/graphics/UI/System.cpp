// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "graphics/UI/System.h"

namespace AE::UI
{
	using namespace AE::Graphics;


/*
=================================================
	Recycle
=================================================
*/
	void  SystemImpl::ScreenApi::Recycle (uint indexInPool) __NE___
	{
		auto&	sys = UISystem();
		sys._screenPool.Unassign( indexInPool );
	}

/*
=================================================
	constructor
=================================================
*/
	SystemImpl::SystemImpl () __NE___
	{
		_screenPool.ForEach([] (auto& scr, uint idx) __NE___ { scr._indexInPool = CheckCast{idx}; });
	}

/*
=================================================
	destructor
=================================================
*/
	SystemImpl::~SystemImpl () __NE___
	{
		_styleCollection.Deinitialize();
		_screenPool.Release( True{"check for assigned"} );
	}

/*
=================================================
	_Initialize
=================================================
*/
	bool  SystemImpl::_Initialize (const StyleCollectionCI &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _styleCollection.Initialize( ci ));

		return true;
	}

	AsyncTask  SystemImpl::_Initialize (StyleCollectionAsyncCI &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );

		AsyncTask	task = _styleCollection.InitializeAsync( ci );
		CHECK_ERR( task );

		return task;
	}

/*
=================================================
	CreateScreen
=================================================
*/
	RC<Screen>  SystemImpl::CreateScreen () __NE___
	{
		uint	index;
		CHECK_ERR( _screenPool.Assign( OUT index ));

		auto&	scr = _screenPool[ index ];

		CHECK( scr._SetMaterial( Default, _styleCollection._sharedDescSet, _styleCollection._dynamicUBuf, _styleCollection._dynamicUBufSize ));

		return RC<Screen>{ &scr };
	}

/*
=================================================
	_Instance
=================================================
*/
	INTERNAL_LINKAGE( InPlace<SystemImpl>  s_SystemImpl );

	SystemImpl&  SystemImpl::_Instance () __NE___
	{
		return s_SystemImpl.Ref();
	}

/*
=================================================
	InstanceCtor
=================================================
*/
	bool  SystemImpl::InstanceCtor::Create (const StyleCollectionCI &ci) __NE___
	{
		s_SystemImpl.Create();

		MemoryBarrier( EMemoryOrder::Release );

		return s_SystemImpl->_Initialize( ci );
	}

	AsyncTask  SystemImpl::InstanceCtor::Create (StyleCollectionAsyncCI &ci) __NE___
	{
		s_SystemImpl.Create();

		MemoryBarrier( EMemoryOrder::Release );

		return s_SystemImpl->_Initialize( ci );
	}

	void  SystemImpl::InstanceCtor::Destroy () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );

		s_SystemImpl.Destroy();

		MemoryBarrier( EMemoryOrder::Release );
	}


} // AE::UI
