// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_OPENVR

# include "base/Platforms/Platform.h"

# include "platform/Private/VRDeviceBase.h"
# include "platform/IO/VRSurface.h"
# include "platform/OpenVR/InputActionsOpenVR.h"

# include "openvr_capi.h"

namespace AE::App
{

	//
	// OpenVR Device
	//

	class OpenVRDevice final : public VRDeviceBase
	{
	// types
	public:
		static constexpr ControllerID	HMD_ID			= ControllerID::_VRFirst;
		static constexpr ControllerID	LeftHand_ID		= ControllerID(uint(ControllerID::_VRFirst) + 1);
		static constexpr ControllerID	RightHand_ID	= ControllerID(uint(ControllerID::_VRFirst) + 2);

	private:
		using IVRSystemPtr			= intptr_t;
		using IVRSystemTable		= VR_IVRSystem_FnTable *;
		using IVRCompositorTable	= VR_IVRCompositor_FnTable *;


		//
		// OpenVR Loader
		//
		struct OpenVRLoader
		{
		// types
			using VR_InitInternal_t							= intptr_t (*) (EVRInitError *peError, EVRApplicationType eType);
			using VR_ShutdownInternal_t						= void (*) ();
			using VR_IsHmdPresent_t							= int (*) ();
			using VR_GetGenericInterface_t					= intptr_t (*) (const char *pchInterfaceVersion, EVRInitError *peError);
			using VR_IsRuntimeInstalled_t					= int (*) ();
			using VR_GetVRInitErrorAsSymbol_t				= const char* (*) (EVRInitError error);
			using VR_GetVRInitErrorAsEnglishDescription_t	= const char* (*) (EVRInitError error);

		// variables
			VR_InitInternal_t							InitInternal;
			VR_ShutdownInternal_t						ShutdownInternal;
			VR_IsHmdPresent_t							IsHmdPresent;
			VR_GetGenericInterface_t					GetGenericInterface;
			VR_IsRuntimeInstalled_t						IsRuntimeInstalled;
			VR_GetVRInitErrorAsSymbol_t					GetVRInitErrorAsSymbol;
			VR_GetVRInitErrorAsEnglishDescription_t		GetVRInitErrorAsEnglishDescription;
		};


		//
		// VR Render Surface
		//
		class VRRenderSurface final : public VRSurface
		{
		// types
		private:
			class SubmitImageTask;

		// variables
		private:
			OpenVRDevice &	_vrDev;

		// methods
		public:
			explicit VRRenderSurface (OpenVRDevice &vr) : _vrDev{vr} {}

			bool  Begin (Graphics::CommandBatch &cmdBatch) override;
			bool  GetTargets (OUT RenderTargets_t &targets) const override;
			bool  End (Graphics::CommandBatch &cmdBatch, ArrayView<AsyncTask> deps) override;
		};

		
		struct ControllerAxis
		{
			float2		value;
			bool		pressed	= false;
		};
		using AxisStates_t	= StaticArray< ControllerAxis, k_unControllerStateAxisCount >;
		using Keys_t		= StaticArray< bool, EVRButtonId_k_EButton_Max >;
		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;

		struct Controller
		{
			ControllerID	id;
			uint			lastPacket	= ~0u;
			Keys_t			keys;
			AxisStates_t	axis;
			TimePoint_t		lastUpdate;
		};
		using Controllers_t	= FixedMap< /*tracked device index*/uint, Controller, 8 >;


	// variables
	private:
		IVRSystemPtr			_hmd			= Zero;
		IVRSystemTable			_vrSystem		= null;
		IVRCompositorTable		_vrCompositor	= null;
		
		TrackedDevicePose_t		_trackedDevicePose [k_unMaxTrackedDeviceCount];
		
		Controllers_t			_controllers;

		InputActionsOpenVR		_input;
		VRRenderSurface			_surface;

		OpenVRLoader			_ovr;
		Library					_openVRLib;


	// methods
	public:
		OpenVRDevice (VRDeviceListener, IInputActions* dst);
		~OpenVRDevice ();


	// IVRDevice //
		bool  Setup (const Settings &) override;

		ND_ bool  Create ();
		ND_ bool  Update (Duration_t timeSinceStart) override;
		
		StringView			GetApiName ()	const	override	{ return "openvr"; }
		IInputActions&		InputActions ()			override	{ return _input; }
		IOutputSurface&		GetSurface ()			override	{ return _surface; }
		
		bool  CreateRenderSurface (const VRImageDesc &desc) override;

		#ifdef AE_ENABLE_VULKAN
		  ND_ VkPhysicalDevice  GetRequiredVkPhysicalDevice (VkInstance inst);
		#endif


	private:
			void  _Destroy ();
		ND_ bool  _LoadLib ();
		
		void  _ProcessHmdEvents (const VREvent_t &);
		void  _ProcessControllerEvents (INOUT Controller&, const VREvent_t &);
		void  _UpdateHMDMatrixPose ();
		void  _InitControllers ();
		
		ND_ ControllerID  _GetControllerID (uint tdi) const;

		ND_ String  _GetTrackedDeviceString (TrackedDeviceIndex_t unDevice, TrackedDeviceProperty prop, TrackedPropertyError *peError = null) const;
	};


} // AE::App

#endif // AE_ENABLE_OPENVR
