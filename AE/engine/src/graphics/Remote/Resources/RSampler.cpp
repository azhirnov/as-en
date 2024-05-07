// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RSampler.h"
# include "graphics/Remote/RResourceManager.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RSampler::~RSampler () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _sampId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RSampler::Create (const RResourceManager &, RmSamplerID id) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _sampId );

		_sampId = id;
		CHECK_ERR( _sampId );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RSampler::Destroy (RResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// don't release ref on device side!

		_sampId = Default;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
