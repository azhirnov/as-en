// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/RenderPassDesc.h"
# include "graphics/Remote/RCommon.h"
# include "Packer/RenderPassPack.h"

namespace AE::Graphics
{

	//
	// Remote Render Pass
	//

	class RRenderPass final
	{
	// types
	public:
		using CreateInfo			= RemoteGraphics::Msg::ResMngr_GetRenderPass_Response;
	private:
		using AttachmentPixFormat_t = CreateInfo::PixFormatMap_t;
		using AttachmentStates_t	= PipelineCompiler::SerializableRenderPass::AttachmentStates_t;


	// variables
	private:
		ubyte						_subpassCount	= 0;
		AttachmentPixFormat_t		_pixFormats;
		AttachmentStates_t			_attStates;

		DRC_ONLY(	RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RRenderPass ()															__NE___ {}
		~RRenderPass ()															__NE___;

		ND_ bool  Create (const CreateInfo &ci)									__NE___;
			void  Destroy (RResourceManager &)									__NE___;

		ND_ usize						SubpassCount ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassCount; }
		ND_ EPixelFormat				GetPixelFormat (AttachmentName::Ref)	C_NE___;
		ND_ AttachmentStates_t const&	AttachmentStates ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _attStates; }
		ND_ uint						GetAttachmentIndex (AttachmentName::Ref)C_NE___;

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()							C_NE___	{ return Default; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
