// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Resources/RSampler.h"
# include "graphics_rhi/Remote/RResourceManager.h"

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
	bool  RSampler::Create (const ResourceManager &, RmSamplerID id) __NE___
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
	void  RSampler::Destroy (ResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// don't release ref on device side!

		_sampId = Default;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
