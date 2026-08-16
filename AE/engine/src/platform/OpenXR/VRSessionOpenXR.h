// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "platform/OpenXR/OpenXRCommon.h"

#ifdef AE_ENABLE_OPENXR
# include "platform/Private/WindowBase.h"
# include "platform/Private/VRSurface.h"
# include "platform/Private/ProjectionImpl.h"
# include "platform/OpenXR/InputActionsOpenXR.h"
# include "platform/OpenXR/OpenXRDevice.h"
# include "platform/OpenXR/OpenXRSwapchain.h"

namespace AE::App
{

	//
	// OpenXR Session
	//

	class VRSessionOpenXR final : public VRSessionBase
	{
	// types
	public:
		static constexpr ControllerID	HMD_ID			= ControllerID::_VRFirst;
		static constexpr ControllerID	LeftHand_ID		= ControllerID(uint(ControllerID::_VRFirst) + 1);
		static constexpr ControllerID	RightHand_ID	= ControllerID(uint(ControllerID::_VRFirst) + 2);

	private:

		//
		// VR Render Surface
		//
		class VRRenderSurface final : public VRSurface
		{
		// variables
		private:
			VRSessionOpenXR &	_vrSession;

		// methods
		public:
			explicit VRRenderSurface (VRSessionOpenXR &vr)										__NE___: _vrSession{vr} {}

			// IOutputSurface //
			AsyncTask			Begin (CommandBatchPtr, CommandBatchPtr, ArrayView<AsyncTask>)	__NE_OV;
			bool				GetTargets (OUT RenderTargets_t &targets)						C_NE_OV;
			AsyncTask			End (ArrayView<AsyncTask> deps)									__NE_OV;

			bool				SetSurfaceMode (const SurfaceInfo &)							__NE_OV	{ return false; }

			SurfaceFormats_t	GetSurfaceFormats ()											C_NE_OV	{ return Default; }
			PresentModes_t		GetPresentModes ()												C_NE_OV	{ return Default; }
			SurfaceInfo			GetSurfaceInfo ()												C_NE_OV	{ return Default; }

		private:
			static AsyncCoro	_SubmitImageTask (VRRenderSurface &surface, const Graphics::EQueueType lastQueue) __NE___;
		};


		struct ControllerAxis
		{
			float2		value;
			bool		pressed	= false;
		};
		using AxisStates_t	= StaticArray< ControllerAxis, 1 >;
		using Keys_t		= StaticArray< bool, 1 >;
		using TimePoint_t	= HighResClock::time_point;

		struct Controller
		{
			ControllerID	id;
			uint			lastPacket	= ~0u;
			Keys_t			keys;
			AxisStates_t	axis;
			TimePoint_t		lastUpdate;
		};
		using Controllers_t	= FixedMap< /*tracked device index*/uint, Controller, 8 >;

		using Projections_t = StaticArray< ProjectionImpl, 4 >;	// 2 eyes with double buffering


	// variables
	private:
		Controllers_t			_controllers;

		InputActionsOpenXR		_input;
		VRRenderSurface			_surface;
		Projections_t			_projections;

		OpenXRDeviceInitializer	_device;


	// methods
	public:
		VRSessionOpenXR (ApplicationBase &, Unique<IWndListener>,
					  IInputActions* dst = null)								__NE___;
		~VRSessionOpenXR ()														__NE___;

		ND_ bool  Create ()														__NE___;


		ND_ OpenXRDevice const&	GetDevice ()									C_NE___	{ return _device; }


	// IWindow //
		void  Close ()															__NE_OV;
		bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc)			__NE_OV;
		bool  SetBrightness (Percent)											__NE_OV	{ return false; }
		bool  SetColorSpace (EColorSpace)										C_NE_OV	{ return false; }

		uint2				GetSurfaceSize ()									C_NE_OV	{ return {}; }
		Monitor				GetMonitor ()										C_NE_OV	{ return {}; }
		NativeWindow		GetNative ()										C_NE_OV	{ return {}; }

		IInputActions&		InputActions ()										__NE_OV	{ return _input; }
		IOutputSurface&		GetSurface ()										__NE_OV	{ return _surface; }


	// IVRSession //
		EDeviceType			GetDeviceType ()									C_NE_OV	{ return EDeviceType::OpenXR; }

		#ifdef AE_ENABLE_VULKAN
		  ND_ VkPhysicalDevice  GetRequiredVkPhysicalDevice (VkInstance inst)	__NE___;
		#endif

		bool  Setup (const Settings &)											__NE_OV;


	private:
			void  _Destroy ()													__NE___;

	// WindowBase //
		bool  ProcessMessages ()												__NE_OV;
		void  _CreateSwapchain ()												__NE_OV;
		void  _DestroySwapchain ()												__NE_OV;
	};


} // AE::App

#endif // AE_ENABLE_OPENXR
