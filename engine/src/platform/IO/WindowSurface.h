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
	using AE::Threading::SharedMutex;


	//
	// Window Output Surface
	//

	class WindowSurface final : public IOutputSurface
	{
	// types
	private:
		class AcquireNextImageTask;
		class PresentImageTask;

	  #if defined(AE_ENABLE_VULKAN)
		using Swapchain_t = InPlace< Graphics::VSwapchainInitializer >;
	  #elif defined(AE_ENABLE_METAL)
		using Swapchain_t = InPlace< Graphics::MSwapchainInitializer >;
	  #endif


	// variables
	private:
		mutable SharedMutex		_guard;

		bool					_initialized	{false};
		Atomic<ushort>			_imageIndex;
		int2					_surfaceSize;				// pretected by '_guard'
		float2					_pixToMm;					// pretected by '_guard'

		CommandBatchPtr			_endCmdBatch;				// pretected by '_guard'
		AsyncTask				_prevTask;					// pretected by '_guard'
		
		Graphics::SwapchainDesc	_desc;						// pretected by '_guard'
		Ptr<const IWindow >		_window;					// pretected by '_guard'

		Atomic<bool>			_recreate		{false};

		mutable SharedMutex		_swGuard;
		Swapchain_t				_swapchain;					// pretected by '_swGuard' and '_guard'


		static constexpr auto	_targetType		= ETargetType::Final2D;


	// methods
	public:
		WindowSurface ();
		~WindowSurface ();

		ND_ bool  Init (IWindow &wnd, const Graphics::SwapchainDesc &desc);


	// IOutputSurface //
		bool			IsInitialized ()		const override;
		RenderPassInfo	GetRenderPassInfo ()	const override;

		AsyncTask	Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps) override;
		bool		GetTargets (OUT RenderTargets_t &targets) const override;
		AsyncTask	End (ArrayView<AsyncTask> deps) override;
		

	// IWindow private api
	public:
		void  CreateSwapchain ();
		void  ResizeSwapchain (const uint2 &newSize);
		void  DestroySwapchain ();


	private:
		void  _UpdateMonitor ();	// must be pretected by '_guard'

	  #if defined(AE_ENABLE_VULKAN)
		ND_ static	Graphics::VResourceManager&	_GetResMngr ();
		ND_ static	Graphics::VDevice const&	_GetDevice ();
	  #elif defined(AE_ENABLE_METAL)
		ND_ static	Graphics::MResourceManager&	_GetResMngr ();
		ND_ static	Graphics::MDevice const&	_GetDevice ();
	  #endif
	};


} // AE::App

