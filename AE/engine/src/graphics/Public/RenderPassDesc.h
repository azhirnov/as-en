// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/MultiSamples.h"
#include "graphics/Public/ImageLayer.h"
#include "graphics/Public/ResourceManager.h"

namespace AE::Graphics
{
	struct Viewport
	{
		RectF		rect;
		float		minDepth	= 0.0f;
		float		maxDepth	= 1.0f;
	};
}
namespace AE::Base {
	template <> struct TTriviallySerializable< Graphics::Viewport > : CT_True {};
	template <> struct TTriviallyDestructible< Graphics::Viewport > : CT_True {};
}
namespace AE::Graphics
{

	//
	// Render Pass description
	//
	struct RenderPassDesc
	{
	// types
	public:
		using Self			= RenderPassDesc;
		using ClearValue_t	= Union< NullUnion, RGBA32f, RGBA32u, RGBA32i, DepthStencil >;

		struct Attachment
		{
			ImageViewID		imageView;
			ClearValue_t	clearValue;				// default is black color

			// State transition helper //

			// Allow to transit from known state to required by render pass.
			// Keep 'Default' to disable auto-transition.
			EResourceState	initial		= Default;

			// Allow to transit from render pass state to expected state.
			// Keep 'Default' to disable auto-transition.
			EResourceState	final		= Default;

			// If 'true' and known state is 'ColorAttachment_RW' and render pass require 'ColorAttachment_RW' state
			// then no barriers are issued in spite of write -> write hazard.
			bool			relaxedStateTransition	= true;
		};

	private:
		using Attachments_t	= FixedMap< AttachmentName, Attachment, GraphicsConfig::MaxAttachments >;
		using Viewports_t	= FixedArray< Viewport, GraphicsConfig::MaxViewports >;


	// variables
	public:
		Attachments_t	attachments;
		Viewports_t		viewports;
		RectI			area;
		ImageLayer		layerCount		= 1_layer;
		RenderPassName	renderPassName;
		SubpassName		subpassName;		// optional
		PipelinePackID	packId;				// optional, required if 'renderPassName' is not exists in default pipeline pack

		// TODO:
		//	- tile size	(Metal)
		//	- image block size (Metal)
		//	- threadgroup size (Metal)
		//	- RRM (Metal)


	// methods
	public:
		RenderPassDesc () __NE___ {}

		template <typename T> RenderPassDesc (RenderPassName::Ref rpName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)										__NE___;
		template <typename T> RenderPassDesc (RenderPassName::Ref rpName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)										__NE___;

		template <typename T> RenderPassDesc (RenderPassName::Ref rpName, SubpassName::Ref spName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)				__NE___;
		template <typename T> RenderPassDesc (RenderPassName::Ref rpName, SubpassName::Ref spName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)				__NE___;

		template <typename T> RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)					__NE___;
		template <typename T> RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)					__NE___;

		template <typename T> RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, SubpassName::Ref spName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)__NE___;
		template <typename T> RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, SubpassName::Ref spName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)__NE___;

		template <typename T> RenderPassDesc (const IRenderTechPipelines &rtech, RenderTechPassName::Ref pass, const Rectangle<T> &rect, ImageLayer layers = 1_layer)	__NE___;
		template <typename T> RenderPassDesc (const IRenderTechPipelines &rtech, RenderTechPassName::Ref pass, const Vec<T,2>     &size, ImageLayer layers = 1_layer)	__NE___;


		// render target
		Self&  AddTarget (AttachmentName::Ref id, ImageViewID imageView)											__NE___;
		Self&  AddTarget (AttachmentName::Ref id, ImageViewID imageView,
						  EResourceState initial, EResourceState final, Bool relaxedStateTransition = True{})		__NE___;

		template <typename ClearVal>
		Self&  AddTarget (AttachmentName::Ref id, ImageViewID imageView, const ClearVal &clearValue)				__NE___;

		template <typename ClearVal>
		Self&  AddTarget (AttachmentName::Ref id, ImageViewID imageView, const ClearVal &clearValue,
						  EResourceState initial, EResourceState final, Bool relaxedStateTransition = True{})		__NE___;


		// viewport
		template <typename T>
		Self&  AddViewport (const Rectangle<T> &rect, float minDepth = 0.0f, float maxDepth = 1.0f)					__NE___;

		template <typename T>
		Self&  AddViewport (const Vec<T,2> &size, float minDepth = 0.0f, float maxDepth = 1.0f)						__NE___;

		Self&  DefaultViewport ()																					__NE___	{ return DefaultViewport( 0.f, 1.f ); }
		Self&  DefaultViewport (float minDepth, float maxDepth)														__NE___;
	};


/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ packId, rpName, SubpassName{}, rect, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ packId, rpName, RectI{int2{0}, int2(size)}, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (RenderPassName::Ref rpName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, SubpassName{}, rect, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (RenderPassName::Ref rpName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, RectI{int2{0}, int2(size)}, layers }
	{}

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, SubpassName::Ref spName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		area{RectI(rect)}, layerCount{layers}, renderPassName{rpName}, subpassName{spName}, packId{packId}
	{
		ASSERT( area.IsValid() );
		ASSERT( renderPassName.IsDefined() );
		ASSERT( layerCount.Get() >= 1 );
	}

	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, RenderPassName::Ref rpName, SubpassName::Ref spName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ packId, rpName, spName, RectI{int2{0}, int2(size)}, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (RenderPassName::Ref rpName, SubpassName::Ref spName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, spName, rect, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (RenderPassName::Ref rpName, SubpassName::Ref spName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, spName, RectI{int2{0}, int2(size)}, layers }
	{}

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (const IRenderTechPipelines &rtech, RenderTechPassName::Ref pass, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		area{RectI(rect)}, layerCount{layers}
	{
		ASSERT( area.IsValid() );
		ASSERT( layerCount.Get() >= 1 );

		auto	info = rtech.GetPass( pass );
		ASSERT( info.type == IRenderTechPipelines::EPassType::Graphics );
		ASSERT( info.renderPass.IsDefined() );

		if_likely( info.type == IRenderTechPipelines::EPassType::Graphics )
		{
			renderPassName	= RenderPassName{ info.renderPass };
			subpassName		= SubpassName{ info.subpass };
			packId			= info.packId;
		}
	}

	template <typename T>
	RenderPassDesc::RenderPassDesc (const IRenderTechPipelines &rtech, RenderTechPassName::Ref pass, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ rtech, pass, RectI{int2{0}, int2(size)}, layers }
	{}

/*
=================================================
	AddTarget
=================================================
*/
	inline RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName::Ref id, ImageViewID imageView) __NE___
	{
		return AddTarget( id, imageView, EResourceState::Unknown, EResourceState::Unknown );
	}

	inline RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName::Ref id, ImageViewID imageView,
													   EResourceState initial, EResourceState final, Bool relaxedStateTransition) __NE___
	{
		ASSERT( imageView );

		bool inserted = attachments.insert_or_assign( id, Attachment{ imageView, NullUnion{}, initial, final, relaxedStateTransition }).second;
		ASSERT( inserted );	Unused( inserted );

		return *this;
	}

	template <typename ClearVal>
	RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName::Ref id, ImageViewID imageView, const ClearVal &clearValue) __NE___
	{
		return AddTarget( id, imageView, clearValue, EResourceState::Unknown, EResourceState::Unknown );
	}

	template <typename ClearVal>
	RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName::Ref id, ImageViewID imageView, const ClearVal &clearValue,
												EResourceState initial, EResourceState final, Bool relaxedStateTransition) __NE___
	{
		ASSERT( imageView );

		bool inserted = attachments.insert_or_assign( id, Attachment{ imageView, clearValue, initial, final, relaxedStateTransition }).second;
		ASSERT( inserted );	Unused( inserted );

		return *this;
	}

/*
=================================================
	AddViewport
=================================================
*/
	template <typename T>
	RenderPassDesc&  RenderPassDesc::AddViewport (const Rectangle<T> &rect, float minDepth, float maxDepth) __NE___
	{
		ASSERT( rect.IsValid() );
		ASSERT( All( area.LeftTop() <= int2(rect.LeftTop()) ));
		ASSERT( All( area.RightBottom() >= int2(rect.RightBottom()) ));

		viewports.try_push_back({ RectF{rect}, minDepth, maxDepth });
		return *this;
	}

	template <typename T>
	RenderPassDesc&  RenderPassDesc::AddViewport (const Vec<T,2> &size, float minDepth, float maxDepth) __NE___
	{
		return AddViewport( Rectangle<T>{size}, minDepth, maxDepth );
	}

/*
=================================================
	DefaultViewport
=================================================
*/
	inline RenderPassDesc&  RenderPassDesc::DefaultViewport (float minDepth, float maxDepth) __NE___
	{
		ASSERT( viewports.empty() );
		ASSERT( minDepth < maxDepth );

		Viewport&	dst = viewports.emplace_back();
		dst.rect		= RectF{area};
		dst.minDepth	= minDepth;
		dst.maxDepth	= maxDepth;
		return *this;
	}


} // AE::Graphics
