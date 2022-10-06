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

			ND_ uint2  RegionSize ()	const	{ return uint2(region.Size()); }
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
		virtual ~IOutputSurface () {}
		

		// Returns 'true' if surface is initialized.
		//   Thread safe: yes
		//
		ND_ virtual bool  IsInitialized () const = 0;
		

		// Returns attachment parameters for render pass.
		//   Thread safe: yes
		//
		ND_ virtual RenderPassInfo  GetRenderPassInfo () const = 0;

		// Begin rendering.
		// Returns 'false' if surface is not initialized.
		//   Thread safe: yes
		//
		ND_ virtual bool  Begin (Graphics::CommandBatch &cmdBatch) = 0;
		

		// Get render taqrgets.
		// Must be used between 'Begin()' / 'End()'.
		//   Thread safe: yes
		//
			virtual bool  GetTargets (OUT RenderTargets_t &targets) const = 0;


		// End rendering and present frame.
		//   Thread safe: yes
		//
		ND_ virtual bool  End (Graphics::CommandBatch &cmdBatch, ArrayView<AsyncTask> deps) = 0;
	};


} // AE::App
