// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Remote/RDevice.h"
# include "graphics/Remote/Resources/RRenderPass.h"

namespace AE::Graphics
{

	//
	// Remote Primary Command buffer State
	//

	struct RPrimaryCmdBufState
	{
	// types
		using ImageArr_t = FixedArray< ImageID, GraphicsConfig::MaxAttachments >;

	// variables
		Ptr<const RRenderPass>		renderPass;
		FrameUID					frameId;
		ulong						subpassIndex		: 8;
		ulong						hasViewLocalDeps	: 1;	// for multiview rendering
		ulong						useSecondaryCmdbuf	: 1;
		void*						userData;
		ImageArr_t					fbImages;					// framebuffer

	// methods
		RPrimaryCmdBufState () 	__NE___ :
			subpassIndex{0xFF}, hasViewLocalDeps{false}, useSecondaryCmdbuf{false}, userData{null}
		{}

		ND_ bool  IsValid () 	C_NE___	{ return (renderPass != null) and frameId.IsValid(); }

		ND_ bool  operator == (const RPrimaryCmdBufState &rhs) C_NE___;
	};


/*
=================================================
	operator ==
=================================================
*/
	inline bool  RPrimaryCmdBufState::operator == (const RPrimaryCmdBufState &rhs) C_NE___
	{
		// ignore 'userData'
		return	(renderPass			== rhs.renderPass)			and
				(frameId			== rhs.frameId)				and
				(subpassIndex		== rhs.subpassIndex)		and
				(hasViewLocalDeps	== rhs.hasViewLocalDeps)	and
				(useSecondaryCmdbuf	== rhs.useSecondaryCmdbuf)	and
				(fbImages			== rhs.fbImages);
	}

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
