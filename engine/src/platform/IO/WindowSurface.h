// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/OutputSurface.h"
#include "platform/Public/IWindow.h"

#if defined(AE_ENABLE_VULKAN)
#	include "graphics/Vulkan/VSwapchain.h"
#elif defined(AE_ENABLE_METAL)
#	include "graphics/Metal/MSwapchain.h"
#endif

namespace AE::App
{

	//
	// Window Output Surface
	//

	class WindowSurface final : public IOutputSurface
	{
	// types
	private:
		class PresentImageTask;

	  #if defined(AE_ENABLE_VULKAN)
		using Swapchain_t = InPlace< Graphics::VSwapchainInitializer >;
	  #elif defined(AE_ENABLE_METAL)
		using Swapchain_t = InPlace< Graphics::MSwapchainInitializer >;
	  #endif


	// variables
	private:
		mutable RecursiveMutex	_guard;

		Swapchain_t				_swapchain;

		bool					_initialized	= false;
		bool					_recreate		= false;

		Graphics::SwapchainDesc	_desc;
		float2					_pixToMm;

		static constexpr auto	_targetType		= ETargetType::Final2D;


	// methods
	public:
		WindowSurface ();
		~WindowSurface ();

		ND_ bool  Init (IWindow &wnd, const Graphics::SwapchainDesc &desc);


	// IOutputSurface //
		bool			IsInitialized ()		const override;
		RenderPassInfo	GetRenderPassInfo ()	const override;

		bool  Begin (Graphics::CommandBatch &cmdBatch) override;
		bool  GetTargets (OUT RenderTargets_t &targets) const override;
		bool  End (Graphics::CommandBatch &cmdBatch, ArrayView<AsyncTask> deps) override;
		

	private:
	  #if defined(AE_ENABLE_VULKAN)
		ND_ static	Graphics::VResourceManager&	_GetResMngr ();
		ND_ static	Graphics::VDevice const&	_GetDevice ();
	  #elif defined(AE_ENABLE_METAL)
		ND_ static	Graphics::MResourceManager&	_GetResMngr ();
		ND_ static	Graphics::MDevice const&	_GetDevice ();
	  #endif
		

	// IWindow private api
	public:
		void  CreateSwapchain (IWindow &wnd);
		void  ResizeSwapchain (const uint2 &newSize);
		void  DestroySwapchain ();
	};


}	// AE::App

