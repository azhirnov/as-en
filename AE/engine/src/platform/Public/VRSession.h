// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread safe:  see method description.
*/

#pragma once

#include "platform/Public/Window.h"
#include "platform/Public/OutputSurface.h"

namespace AE::App
{
	using Graphics::ImageDim2_t;


	//
	// VR Session interface
	//

	class IVRSession
	{
	// types
	public:
		struct Settings
		{
			// TODO: stand/seat position
			packed_float2	cameraClipPlanes;
		};


		struct VRImageDesc : Graphics::SwapchainDesc
		{
			ImageDim2_t		dimension;
			MultiSamples	samples;

			VRImageDesc ()									__NE___	{}
			VRImageDesc (const SwapchainDesc &other)		__NE___	: SwapchainDesc{other} {}
			VRImageDesc (const VRImageDesc &)				__NE___ = default;
			VRImageDesc&  operator = (const VRImageDesc &)	__NE___ = default;
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

		// Change VR device settings.
		//   Thread safe: yes ???
		//
		ND_	virtual bool  Setup (const Settings &)						__NE___	= 0;


		// Returns VR device type.
		//   Thread safe: yes
		//
		ND_ virtual EDeviceType  GetDeviceType ()						C_NE___ = 0;

		// TODO: load controller model
	};


} // AE::App
