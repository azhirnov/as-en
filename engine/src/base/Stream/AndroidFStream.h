// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"

#ifdef AE_PLATFORM_ANDROID

# include <asset_manager.h>
# include <asset_manager_jni.h>

namespace AE::Base
{

	//
	// Read-only Android File Stream
	//

	class AFileRStream final : public RStream
	{
	// variables
	private:
		AAsset *	_asset		= null;
		Bytes		_fileSize;
		Bytes		_position;


	// methods
	public:
		explicit AFileRStream (AAsset* asset) :
			_asset{ asset },
			_fileSize{ asset ? AAsset_getLength(asset) : 0 }
		{}

		~AFileRStream ()
		{
			if ( _asset )
				AAsset_close( _asset );
		}

		bool	IsOpen ()	const override		{ return _asset != null; }
		Bytes	Position ()	const override		{ return _position; }
		Bytes	Size ()		const override		{ return _fileSize; }
		
		bool	SeekSet (Bytes pos) override
		{
			return AAsset_seek( _asset, usize(pos), SEEK_SET ) != -1;
		}

		Bytes	Read2 (OUT void *buffer, Bytes size) override
		{
			return Bytes{ AAsset_read( _asset, buffer, usize(size) )};
		}
	};

}	// AE::Base

#endif // AE_PLATFORM_ANDROID
