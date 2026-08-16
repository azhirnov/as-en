// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Remote/Resources/RIndirectCommandsLayout.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RIndirectCommandsLayout::~RIndirectCommandsLayout () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _id == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RIndirectCommandsLayout::Create (ResourceManager &resMngr, const IndirectCommandsLayoutDesc &desc, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _id == Default );
		CHECK_ERR( not desc.tokens.empty() );
		CHECK_ERR( desc.stages != Default );

		// TODO

		_usage	= desc.usage;
		_stages	= desc.stages;

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RIndirectCommandsLayout::Destroy (ResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// TODO

		_id		= Default;
		_usage	= Default;
		_stages	= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

} // AE::Graphics
#endif // AE_ENABLE_REMOTE_GRAPHICS
