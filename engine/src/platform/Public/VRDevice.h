// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IWindow.h"
#include "platform/Public/OutputSurface.h"

namespace AE::App
{

	//
	// VR Device interface
	//

	class IVRDevice
	{
	// types
	public:
		// states:
		//   Started		- HMD is turned ON
		//   InBackground	- HMD is active but not mounted
		//   InForeground	- HMD is mounted

		using EState = IWindow::EState;

		class IVRDeviceEventListener
		{
		// types
		public:
			using EState = IWindow::EState;

		// interface
		public:
			virtual ~IVRDeviceEventListener () {}

			virtual void  OnUpdate (IVRDevice &vr) = 0;
			virtual void  OnStateChanged (IVRDevice &vr, EState state) = 0;
		};


		struct Settings
		{
			// TODO: stand/seat position
			packed_float2	cameraClipPlanes;
		};


		struct VRImageDesc
		{
			uint2			dimension;
			EPixelFormat	format		= Default;
			EImageUsage		usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;
			MultiSamples	samples;
		};

		enum class EDeviceType : ubyte
		{
			Unknown,
			Emulator,
			OpenVR,
			OpenXR,
		};


	// interface
	public:

		// Terminate VR device.
		//   Thread safe: yes
		//
			virtual void  Terminate () = 0;
		
			
		// Change VR device settings.
		//   Thread safe: yes ???
		//
		ND_	virtual bool  Setup (const Settings &) = 0;
		

		// Returns current state.
		//   Thread safe: no
		//
		ND_ virtual EState  GetState () const = 0;


		// Returns input actions class.
		//   Thread safe: yes
		//
		ND_ virtual IInputActions&  InputActions () = 0;
		

		// Returns VR API name.
		//   Thread safe: yes
		//
		ND_ virtual StringView  GetApiName () const = 0;

		// TODO: load controller model


	// surface api
		
		// Create or recreate rendering surface (swapchain).
		//   Thread safe: main thread only, must be synchronized with 'GetSurface()'
		//
		ND_ virtual bool  CreateRenderSurface (const VRImageDesc &desc) = 0;
		
		
		// Returns render surface reference.
		// Surface must be successfully created using 'CreateRenderSurface()'.
		//   Thread safe: must be synchronized with 'CreateRenderSurface()'
		//
		ND_ virtual IOutputSurface&  GetSurface () = 0;
	};


} // AE::App
