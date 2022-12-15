// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Output surface can be:
		- window / display
		- video stream
		- render target
		- VR device
*/

#pragma once

#include "platform/Public/Projection.h"

#include "graphics/Public/IDs.h"
#include "graphics/Public/ImageLayer.h"
#include "graphics/Public/MultiSamples.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/GraphicsImpl.h"

namespace AE::App
{
	using Graphics::ImageID;
	using Graphics::ImageViewID;
	using Graphics::ImageLayer;
	using Graphics::EResourceState;
	using Graphics::MultiSamples;
	using Graphics::EPixelFormat;
	using Graphics::EImageUsage;
	using Graphics::CommandBatchPtr;



	//
	// Output Surface
	//

	class IOutputSurface
	{
	// types
	public:
		enum class ESurfaceType : ubyte
		{
			Unknown		= 0,
			Screen,
			CurvedScreen,
			VR,
		};

		enum class ETargetType : ubyte
		{
			Albedo,
			Normal,
			Depth,
			Final2D,	// to swapchain
			Final3D,	// to VR device
			Unknown,
		};


		//
		// Render Target
		//
		struct RenderTarget
		{
			ImageID					imageId;
			ImageViewID				viewId;			// 2D with single mipmap, shared 2D array with specified 'layer'

			RectI					region;			// for texture atlas
			ImageLayer				layer;
			
			float2					pixToMm;		// pixels to millimeters, used for touch screen, should not be used for VR

			EResourceState			initialState	= Default;
			EResourceState			finalState		= Default;

			ETargetType				type			= Default;

			Ptr<const IProjection>	projection;

			ND_ uint2  RegionSize ()	C_NE___	{ return uint2(region.Size()); }
		};
		using RenderTargets_t = FixedArray< RenderTarget, Graphics::GraphicsConfig::MaxAttachments >;


		//
		// Render Pass Info
		//
		struct RenderPassInfo
		{
		// types
			struct Attachment
			{
				EPixelFormat	format		= Default;
				MultiSamples	samples;
				ETargetType		type		= Default;
			};
			using Attachments_t = FixedArray< Attachment, Graphics::GraphicsConfig::MaxAttachments >;

		// variables
			Attachments_t	attachments;
		};


	// interface
	public:
		virtual ~IOutputSurface ()																								__NE___	{}
		

		// Returns 'true' if surface is initialized.
		//   Thread safe: yes
		//
		ND_ virtual bool  IsInitialized ()																						C_NE___ = 0;
		

		// Returns attachment parameters for render pass.
		//   Thread safe: yes
		//
		ND_ virtual RenderPassInfo  GetRenderPassInfo ()																		C_NE___ = 0;

		// Begin rendering.
		// Returns image acquire task which is implicitly synchronized with present/blit task which returned by 'End()', returns 'null' on error.
		// 'beginCmdBatch'	- batch where render targets will be rendered.
		// 'endCmdBatch'	- batch where render targets was rendered.
		// 'deps'			- list of tasks which must be executed before.
		//   Thread safe: yes
		//
		ND_ virtual AsyncTask  Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)	__NE___	= 0;
		

		// Get render taqrgets.
		// Must be used between 'Begin()' / 'End()'.
		//   Thread safe: yes
		//
			virtual bool  GetTargets (OUT RenderTargets_t &targets)																C_NE___ = 0;


		// End rendering and present frame.
		// Returns present/blit task, returns 'null' on error.
		// 'deps'		- list of tasks which must be executed before, 'CmdBatchOnSubmit{endCmdBatch}' is imlicitlly added.
		//   Thread safe: yes
		//
		ND_ virtual AsyncTask  End (ArrayView<AsyncTask> deps)																	__NE___	= 0;
	};


} // AE::App
