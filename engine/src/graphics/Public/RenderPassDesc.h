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
			EResourceState	final		= Default;
		};

		struct Viewport
		{
			RectF		rect;
			float		minDepth	= 0.0f;
			float		maxDepth	= 1.0f;
		};

		using Attachments_t	= FixedMap< AttachmentName, Attachment, GraphicsConfig::MaxAttachments >;
		using Viewports_t	= FixedArray< Viewport, GraphicsConfig::MaxViewports >;


	// variables
	public:
		Attachments_t	attachments;
		Viewports_t		viewports;
		RectI			area;
		ImageLayer		layerCount;
		RenderPassName	renderPassName;
		SubpassName		subpassName;		// optional

		// TODO:
		//	- tile size	(Metal)
		//	- image block size (Metal)
		//	- threadgroup size (Metal)
		//	- RRM (Metal)
		//	- time profiling


	// methods
	public:
		template <typename T> RenderPassDesc (const RenderPassName &rpName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)											__NE___;
		template <typename T> RenderPassDesc (const RenderPassName &rpName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)											__NE___;

		template <typename T> RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Rectangle<T> &rect, ImageLayer layers = 1_layer)				__NE___;
		template <typename T> RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Vec<T,2>     &size, ImageLayer layers = 1_layer)				__NE___;
		
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
	};
//-----------------------------------------------------------------------------

	

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		RenderPassDesc{ rpName, SubpassName{}, rect, layers }
	{}
	
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ rpName, RectI{int2{0}, int2(size)}, layers }
	{}

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Rectangle<T> &rect, ImageLayer layers) __NE___ :
		area{RectI(rect)}, layerCount{layers}, renderPassName{rpName}, subpassName{spName}
	{
		ASSERT( area.IsValid() );
		ASSERT( renderPassName.IsDefined() );
		ASSERT( layerCount.Get() >= 1 );
	}
	
	template <typename T>
	RenderPassDesc::RenderPassDesc (const RenderPassName &rpName, const SubpassName &spName, const Vec<T,2> &size, ImageLayer layers) __NE___ :
		RenderPassDesc{ rpName, spName, RectI{int2{0}, int2(size)}, layers }
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

			renderPassName	= RenderPassName{ info.renderPass };
			subpassName		= SubpassName{ info.subpass };
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

		attachments.insert_or_assign( id, Attachment{ imageView, NullUnion{}, initial, final });
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

		attachments.insert_or_assign( id, Attachment{ imageView, clearValue, initial, final });
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

		viewports.push_back({ RectF{rect}, minDepth, maxDepth });
		return *this;
	}

	template <typename T>
	RenderPassDesc&  RenderPassDesc::AddViewport (const Vec<T,2> &size, float minDepth, float maxDepth) __NE___
	{
		ASSERT( All( int2(size) > 0 ));
		ASSERT( All( area.LeftTop() <= 0 ));
		ASSERT( All( area.RightBottom() >= int2(size) ));

		viewports.push_back({ RectF{float2(), float2(size)}, minDepth, maxDepth });
		return *this;
	}


} // AE::Graphics
