// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Remote/Resources/RIndirectExecutionSet.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RIndirectExecutionSet::~RIndirectExecutionSet () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _id == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RIndirectExecutionSet::Create (ResourceManager &resMngr, const CreateInfo &desc) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _id == Default );

		// TODO

		_initialState = desc.initialState;

		GFX_DBG_ONLY( _debugName = desc.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RIndirectExecutionSet::Destroy (ResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// TODO

		_id				= Default;
		_initialState	= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

} // AE::Graphics
#endif // AE_ENABLE_REMOTE_GRAPHICS
