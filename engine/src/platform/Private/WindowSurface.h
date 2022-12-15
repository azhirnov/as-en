// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/OutputSurface.h"
#include "platform/Public/IWindow.h"

#if defined(AE_ENABLE_VULKAN)
#	include "graphics/Vulkan/VSwapchain.h"
#elif defined(AE_ENABLE_METAL)
#	include "graphics/Metal/MSwapchain.h"
#else
#	error not implemented
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
	  #else
	  #	error not implemented
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
		WindowSurface ()																							__NE___;
		~WindowSurface ()																							__NE___;

		ND_ bool  Init (IWindow &wnd, const Graphics::SwapchainDesc &desc)											__NE___;


	// IOutputSurface //
		bool			IsInitialized ()																			C_NE_OV;
		RenderPassInfo	GetRenderPassInfo ()																		C_NE_OV;

		AsyncTask	Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)	__NE_OV;
		bool		GetTargets (OUT RenderTargets_t &targets)														C_NE_OV;
		AsyncTask	End (ArrayView<AsyncTask> deps)																	__NE_OV;
		

	// IWindow private api
	public:
		void  CreateSwapchain ()																					__NE___;
		void  ResizeSwapchain (const uint2 &newSize)																__NE___;
		void  DestroySwapchain ()																					__NE___;


	private:
		// must be pretected by '_guard'
		void  _UpdateMonitor ()																						__NE___;

	  #if defined(AE_ENABLE_VULKAN)
		ND_ static	Graphics::VResourceManager&	_GetResMngr ()														__NE___;
		ND_ static	Graphics::VDevice const&	_GetDevice ()														__NE___;
	  #elif defined(AE_ENABLE_METAL)
		ND_ static	Graphics::MResourceManager&	_GetResMngr ()														__NE___;
		ND_ static	Graphics::MDevice const&	_GetDevice ()														__NE___;
	  #else
	  #	error not implemented
	  #endif
	};


} // AE::App

