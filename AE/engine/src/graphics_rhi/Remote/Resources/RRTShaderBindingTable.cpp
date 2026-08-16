// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Resources/RRTShaderBindingTable.h"
# include "graphics_rhi/Remote/RResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RRTShaderBindingTable::~RRTShaderBindingTable () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _sbt.id );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RRTShaderBindingTable::Create (ResourceManager &, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _sbt.id );

		_sbt = ci.sbt;
		CHECK_ERR( _sbt.id );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RRTShaderBindingTable::Destroy (ResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_sbt	= Default;
		_desc	= Default;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
