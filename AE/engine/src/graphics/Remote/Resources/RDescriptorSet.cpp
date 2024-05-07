// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RDescriptorSet.h"
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
	RDescriptorSet::~RDescriptorSet () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _descSetId );
		ASSERT( not _layoutId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RDescriptorSet::Create (RResourceManager &resMngr, RmDescriptorSetID dsId, DescriptorSetLayoutID layoutId,
								  DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _descSetId );
		CHECK_ERR( not _layoutId );

		_descSetId	= dsId;
		_layoutId	= resMngr.AcquireResource( layoutId );

		CHECK_ERR( _descSetId and _layoutId );
		GFX_DBG_ONLY( _debugName = dbgName );
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RDescriptorSet::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _descSetId )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_descSetId} ));

		resMngr.ImmediatelyRelease( INOUT _layoutId );

		_descSetId = Default;
		GFX_DBG_ONLY( _debugName.clear() );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
