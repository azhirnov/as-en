// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_OPENXR

# if not defined(AE_ENABLE_VULKAN)
#	error OpenXR requires Vulkan API.
# endif

# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "platform/OpenXR/VRSessionOpenXR.h"

namespace AE::App
{
	using namespace AE::Graphics;

/*
=================================================
	GetTargets
=================================================
*/
	bool  VRSessionOpenXR::VRRenderSurface::GetTargets (OUT RenderTargets_t &targets) C_NE___
	{
		const usize	idx = 0;//_projIdx.load();

		if_likely( VRSurface::GetTargets( targets ))
		{
			for (usize i = 0; i < targets.size(); ++i)
			{
				targets[i].finalState	= EResourceState::BlitSrc;
				targets[i].projection	= &_vrSession._projections[i + idx];
			}
			return true;
		}
		return false;
	}

/*
=================================================
	_SubmitImageTask
=================================================
*/
	AsyncCoro  VRSessionOpenXR::VRRenderSurface::_SubmitImageTask (VRRenderSurface &surface, const EQueueType lastQueue) __NE___
	{
		EXLOCK( surface._guard );


		return null;
	}

/*
=================================================
	Begin
=================================================
*/
	AsyncTask  VRSessionOpenXR::VRRenderSurface::Begin (CommandBatchPtr, CommandBatchPtr, ArrayView<AsyncTask>) __NE___
	{
		EXLOCK( _guard );

		return null;
	}

/*
=================================================
	End
=================================================
*/
	AsyncTask  VRSessionOpenXR::VRRenderSurface::End (ArrayView<AsyncTask>) __NE___
	{
		EXLOCK( _guard );

		return null;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VRSessionOpenXR::VRSessionOpenXR (ApplicationBase &app, Unique<IWndListener> listener, IInputActions* dst) __NE___ :
		VRSessionBase{ app, RVRef(listener) },
		_input{ InputActionsBase::GetQueue( dst )},
		_surface{ *this },
		_device{ True{"enable log"} }
	{}

/*
=================================================
	destructor
=================================================
*/
	VRSessionOpenXR::~VRSessionOpenXR () __NE___
	{
		_Destroy();
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRSessionOpenXR::Create () __NE___
	{
		DRC_EXLOCK( _drCheck );

		OpenXRDeviceInitializer::InstanceCreateInfo	info;

		CHECK_ERR( _device.CreateInstance( info ));

		//_device.CreateDebugCallback();

		CHECK_ERR( _device.CreateSession() );

		return true;
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  VRSessionOpenXR::_Destroy () __NE___
	{
		DRC_EXLOCK( _drCheck );

		_device.DestroySession();
		_device.DestroyDebugCallback();
		_device.DestroyInstance();
	}

/*
=================================================
	Setup
=================================================
*/
	bool  VRSessionOpenXR::Setup (const Settings &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		return false;
	}

/*
=================================================
	Update
=================================================
*
	bool  VRSessionOpenXR::Update (Duration_t timeSinceStart) __NE___
	{
		if_unlikely( not _isRunning.load() )
			return false;

		DRC_EXLOCK( _drCheck );


		return true;
	}

/*
=================================================
	GetRequiredVkPhysicalDevice
=================================================
*/
#ifdef AE_ENABLE_VULKAN
	VkPhysicalDevice  VRSessionOpenXR::GetRequiredVkPhysicalDevice (VkInstance inst) __NE___
	{
		DRC_EXLOCK( _drCheck );


		return Default;
	}
#endif

/*
=================================================
	CreateRenderSurface
=================================================
*
	bool  VRSessionOpenXR::CreateRenderSurface (const VRImageDesc &desc) __NE___
	{
		switch ( desc.format )
		{
			case EPixelFormat::RGBA8_UNorm :
			case EPixelFormat::sRGB8_A8 :
			case EPixelFormat::BGRA8_UNorm :
			case EPixelFormat::sBGR8_A8 :
			case EPixelFormat::RGBA32F :
			case EPixelFormat::RGBA16F :
			case EPixelFormat::RGB10_A2_UNorm :
				break;	// OK
			default :
				RETURN_ERR( "unsupported image format for OpenXR" );
		}

		CHECK_ERR( AllBits( desc.usage, EImageUsage::TransferSrc | EImageUsage::Sampled ));

		return _surface.Create( desc );
	}
*/

/*
=================================================
	Close
=================================================
*/
	void  VRSessionOpenXR::Close () __NE___
	{
	}

/*
=================================================
	CreateRenderSurface
=================================================
*/
	bool  VRSessionOpenXR::CreateRenderSurface (const Graphics::SwapchainDesc &desc) __NE___
	{
		return false;
	}

/*
=================================================
	ProcessMessages
=================================================
*/
	bool  VRSessionOpenXR::ProcessMessages () __NE___
	{
		return false;
	}

/*
=================================================
	_CreateSwapchain
=================================================
*/
	void  VRSessionOpenXR::_CreateSwapchain () __NE___
	{
	}

/*
=================================================
	_DestroySwapchain
=================================================
*/
	void  VRSessionOpenXR::_DestroySwapchain () __NE___
	{
	}


} // AE::App

#endif // AE_ENABLE_OPENXR
