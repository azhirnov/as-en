// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermSound.h"

namespace AE::ResLoader
{

/*
=================================================
	constructor
=================================================
*/
	IntermSound::IntermSound (IntermSound &&other) __NE___ :
		_srcPath{ RVRef(other._srcPath) },
		_data{ other._data },
		_storage{ RVRef(other._storage) },
		_desc{ other._desc },
		_immutable{ other._immutable }
	{}

/*
=================================================
	operator =
=================================================
*/
	IntermSound&  IntermSound::operator = (IntermSound &&rhs) __NE___
	{
		_srcPath	= RVRef(rhs._srcPath);
		_data		= rhs._data;
		_storage	= RVRef(rhs._storage);
		_desc		= rhs._desc;
		_immutable	= rhs._immutable;
		return *this;
	}

/*
=================================================
	SetData
=================================================
*/
	bool  IntermSound::SetData (const AudioDataDesc &desc, RC<SharedMem> storage) __NE___
	{
		CHECK_ERR( storage );
		CHECK_ERR( desc.size <= storage->Size() );
		CHECK_ERR( desc.IsValid() );

		_data		= storage->Data();
		_storage	= RVRef(storage);
		_desc		= desc;

		return true;
	}

	bool  IntermSound::SetData (const AudioDataDesc &desc, void* data) __NE___
	{
		CHECK_ERR( data != null );
		CHECK_ERR( desc.size > 0 );
		CHECK_ERR( desc.IsValid() );

		_data		= data;
		_storage	= null;
		_desc		= desc;

		return true;
	}

/*
=================================================
	Allocate
=================================================
*/
	bool  IntermSound::Allocate (const AudioDataDesc &desc, RC<IAllocator> allocator) __NE___
	{
		CHECK_ERR( desc.size > 0 );
		CHECK_ERR( desc.IsValid() );

		_storage = SharedMem::Create( RVRef(allocator), desc.size );
		CHECK_ERR( _storage );

		_data	= _storage->Data();
		_desc	= desc;
		return true;
	}

	bool  IntermSound::Allocate (const AudioDataDesc &desc) __NE___
	{
		return Allocate( desc, AE::GetDefaultAllocator() );
	}


} // AE::ResLoader
