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

		bool					_submitted		= true;

		static constexpr auto	_targetType		= ETargetType::Final3D;


	// methods
	public:
		VRSurface ();
		~VRSurface ();

		ND_ bool  Create (const VRImageDesc &desc);
			void  Destroy ();


	// IOutputSurface //
		bool			IsInitialized ()		const override;
		RenderPassInfo	GetRenderPassInfo ()	const override;

		bool  GetTargets (OUT RenderTargets_t &targets) const override;
		

	private:
		ND_ static	Graphics::IResourceManager&  _GetResMngr ();
	};


}	// AE::App

