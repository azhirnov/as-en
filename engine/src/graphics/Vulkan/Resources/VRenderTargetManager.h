// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ImageDesc.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Render Target Manager
	//

	class VRenderTargetManager
	{
	// types
	private:
		struct RTInfo
		{
			Strong<ImageID>			image;
			Strong<ImageViewID>		view;
			ImageDesc				desc;
			FractionalI16			scale;
		};
		using RTMap_t		= FlatHashMap< RenderTargetName::Optimized_t, RTInfo >;

		using RTPerPass_t	= FixedArray< RenderTargetName::Optimized_t, GraphicsConfig::MaxAttachments >;
		using Passes_t		= Array< RTPerPass_t >;

		enum class EState : uint
		{
			Ready,
			Recording,
		};


	// variables
	private:
		EState			_state			= EState::Ready;
		
		uint2			_surfaceSize;
		FrameUID		_frameId;

		RTMap_t			_rtMap;
		Passes_t		_passes [2];
		
		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		VRenderTargetManager ()															__NE___;
		~VRenderTargetManager ()														__NE___;

		ND_ bool  CreateRT (const RenderTargetName &name, const ImageDesc &desc,
							FractionalI16 surfScale, StringView dbgName)				__NE___;

		ND_ ImageAndViewID  Get (const RenderTargetName &name)							__NE___;


		ND_ bool  Begin (const uint2 &surfaceSize)										__NE___;
			void  AddUsage (const RenderTargetName &rt, uint exeOrder)					__NE___;
		ND_ bool  End ()																__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
