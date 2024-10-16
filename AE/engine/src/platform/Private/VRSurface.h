// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/OutputSurface.h"
#include "platform/Public/VRDevice.h"

namespace AE::App
{

	//
	// VR Render Surface
	//

	class VRSurface : public IOutputSurface
	{
	// types
	private:
		using Images_t		= StaticArray< Strong< ImageID >, 2 >;
		using ImageViews_t	= StaticArray< Strong< ImageViewID >, 2 >;
		using VRImageDesc	= IVRDevice::VRImageDesc;



	// variables
	protected:
		mutable RecursiveMutex	_guard;

		Images_t				_images;
		ImageViews_t			_views;

		VRImageDesc				_desc;

		static constexpr auto	_colorSpace		= EColorSpace::Unknown;


	// methods
	public:
		VRSurface ()											__NE___;
		~VRSurface ()											__NE___;

		ND_ bool  Create (const VRImageDesc &desc)				__NE___;
			void  Destroy ()									__NE___;


	// IOutputSurface //
		bool			IsInitialized ()						C_NE_OV;
		RenderPassInfo	GetRenderPassInfo ()					C_NE_OV;

		bool			GetTargets (OUT RenderTargets_t &)		C_NE_OV;

		TargetInfos_t	GetTargetInfo ()						C_NE_OV;
	};


} // AE::App

