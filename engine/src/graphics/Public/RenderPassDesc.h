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
			EResourceState	initial		= Default;	// allow to transit from known state to required by render pass
			EResourceState	final		= Default;	// allow to transit from render pass state to expected state
		};

		struct Viewport
		{
			RectF		rect;
			float		minDepth	= 0.0f;
			float		maxDepth	= 1.0f;
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
		PipelinePackID	packId;

		// TODO:
		//	- tile size	(Metal)
		//	- image block size (Metal)
		//	- threadgroup size (Metal)
		//	- RRM (Metal)


	// methods
	public:
		RenderPassDesc () __NE___ {}

		template <typename T> RenderPassDesc (const RenderPassName &rpName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)											__NE___;
		template <typename T> RenderPassDesc (const RenderPassName &rpName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)											__NE___;

		template <typename T> RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)				__NE___;
		template <typename T> RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)				__NE___;
		
		template <typename T> RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)					__NE___;
		template <typename T> RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)					__NE___;

		template <typename T> RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const SubpassName &spName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)__NE___;
		template <typename T> RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const SubpassName &spName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)__NE___;

		template <typename T> RenderPassDesc (const RenderTechPipelinesPtr &rtech, const RenderTechPassName &pass, const Rectangle<T> &rect, ImageLayer layers = 1_layer)	__NE___;
		template <typename T> RenderPassDesc (const RenderTechPipelinesPtr &rtech, const RenderTechPassName &pass, const Vec<T,2>     &size, ImageLayer layers = 1_layer)	__NE___;


		// render target
		Self&  AddTarget (AttachmentName id, ImageViewID imageView)													__NE___;
		Self&  AddTarget (AttachmentName id, ImageViewID imageView, EResourceState initial, EResourceState final)	__NE___;
		
		template <typename ClearVal>
		Self&  AddTarget (AttachmentName id, ImageViewID imageView, const ClearVal &clearValue)						__NE___;
		
		template <typename ClearVal>
		Self&  AddTarget (AttachmentName id, ImageViewID imageView, const ClearVal &clearValue,
						  EResourceState initial, EResourceState final)												__NE___;


		// viewport
		template <typename T>
		Self&  AddViewport (const Rectangle<T> &rect, float minDepth = 0.0f, float maxDepth = 1.0f)					__NE___;
		
		template <typename T>
		Self&  AddViewport (const Vec<T,2> &size, float minDepth = 0.0f, float maxDepth = 1.0f)						__NE___;

		Self&  DefaultViewport ()																					__NE___;
	};
//-----------------------------------------------------------------------------

	

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ packId, rpName, SubpassName{}, rect, layers }
	{}
	
	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ packId, rpName, RectI{int2{0}, int2(size)}, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, SubpassName{}, rect, layers }
	{}
	
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, RectI{int2{0}, int2(size)}, layers }
	{}

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const SubpassName &spName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		area{RectI(rect)}, layerCount{layers}, renderPassName{rpName}, subpassName{spName}, packId{packId}
	{
		ASSERT( area.IsValid() );
		ASSERT( renderPassName.IsDefined() );
		ASSERT( layerCount.Get() >= 1 );
	}
	
	template <typename T>
	RenderPassDesc::RenderPassDesc (PipelinePackID packId, const RenderPassName &rpName, const SubpassName &spName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ packId, rpName, spName, RectI{int2{0}, int2(size)}, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, spName, rect, layers }
	{}

	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ Default, rpName, spName, RectI{int2{0}, int2(size)}, layers }
	{}

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderTechPipelinesPtr &rtech, const RenderTechPassName &pass, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		area{RectI(rect)}, layerCount{layers}
	{
		ASSERT( rtech );
		ASSERT( area.IsValid() );
		ASSERT( layerCount.Get() >= 1 );

		if_likely( rtech )
		{
			auto	info = rtech->GetPass( pass );
			ASSERT( info.type == IRenderTechPipelines::EPassType::Graphics );
			ASSERT( info.renderPass.IsDefined() );
			
			if_likely( info.type == IRenderTechPipelines::EPassType::Graphics )
			{
				renderPassName	= RenderPassName{ info.renderPass };
				subpassName		= SubpassName{ info.subpass };
				packId			= info.packId;
			}
		}
	}
	
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderTechPipelinesPtr &rtech, const RenderTechPassName &pass, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ rtech, pass, RectI{int2{0}, int2(size)}, layers }
	{}
	
/*
=================================================
	AddTarget
=================================================
*/
	inline RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName id, ImageViewID imageView) __NE___
	{
		return AddTarget( id, imageView, EResourceState::Unknown, EResourceState::Unknown );
	}

	inline RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName id, ImageViewID imageView, EResourceState initial, EResourceState final) __NE___
	{
		ASSERT( imageView );

		bool inserted = attachments.try_insert_or_assign( id, Attachment{ imageView, NullUnion{}, initial, final }).second;
		ASSERT( inserted );	Unused( inserted );

		return *this;
	}
	
	template <typename ClearVal>
	RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName id, ImageViewID imageView, const ClearVal &clearValue) __NE___
	{
		return AddTarget( id, imageView, clearValue, EResourceState::Unknown, EResourceState::Unknown );
	}

	template <typename ClearVal>
	RenderPassDesc&  RenderPassDesc::AddTarget (AttachmentName id, ImageViewID imageView, const ClearVal &clearValue, EResourceState initial, EResourceState final) __NE___
	{
		ASSERT( imageView );

		bool inserted = attachments.try_insert_or_assign( id, Attachment{ imageView, clearValue, initial, final }).second;
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
	inline RenderPassDesc&  RenderPassDesc::DefaultViewport () __NE___
	{
		ASSERT( viewports.empty() );

		Viewport&	dst = viewports.emplace_back();
		dst.rect		= RectF{area};
		dst.minDepth	= 0.0f;
		dst.maxDepth	= 1.0f;
		return *this;
	}


} // AE::Graphics
