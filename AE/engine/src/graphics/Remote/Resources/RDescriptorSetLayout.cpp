// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RDescriptorSetLayout.h"
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/REnumCast.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RDescriptorSetLayout::~RDescriptorSetLayout () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _dslId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RDescriptorSetLayout::Create (const RDevice &, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _dslId );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  RDescriptorSetLayout::Create (const RDevice &, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _dslId );

		_dslId		= ci.id;
		_usage		= ci.usage;
		_uniforms	= ci.uniforms;
		_unOffsets	= ci.unOffsets;

		GFX_DBG_ONLY( _debugName = ci.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RDescriptorSetLayout::Destroy (RResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// don't release ref on device side!

		_unOffsets.fill( 0 );
		_dslId		= Default;
		_usage		= Default;
		_uniforms	= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
