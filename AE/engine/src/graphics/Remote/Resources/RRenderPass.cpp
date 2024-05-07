// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RRenderPass.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RRenderPass::~RRenderPass () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _pixFormats.empty() );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RRenderPass::Create (const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_subpassCount	= ci.subpassCount;
		_pixFormats		= ci.pixFormats;

		for (usize i = 0; i < _attStates.size(); ++i)
		{
			_attStates[i].initial	= ci.attStates[i].first;
			_attStates[i].final		= ci.attStates[i].second;
		}

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RRenderPass::Destroy (RResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_subpassCount = 0;
		_pixFormats.clear();
	}

/*
=================================================
	GetPixelFormat
=================================================
*/
	EPixelFormat  RRenderPass::GetPixelFormat (AttachmentName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pixFormats.find( name );
		if_likely( it != _pixFormats.end() )
		{
			return it->second.first;
		}
		return Default;
	}

/*
=================================================
	GetAttachmentIndex
=================================================
*/
	uint  RRenderPass::GetAttachmentIndex (AttachmentName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pixFormats.find( name );
		if_likely( it != _pixFormats.end() )
		{
			return it->second.second;
		}
		return UMax;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
