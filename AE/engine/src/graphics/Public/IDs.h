// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics::_hidden_
{
	// tag: HandleTmpl UID
	static constexpr uint	GraphicsIDs_Start	= 1 << 24;
	static constexpr uint	VulkanIDs_Start		= 2 << 24;
	static constexpr uint	MetalIDs_Start		= 3 << 24;
	static constexpr uint	RemoteIDs_Start		= 4 << 24;

	// tag: NamedID UID
	static constexpr uint	NamedIDs_Start		= 1 << 24;

} // AE::Graphics::_hidden_


namespace AE::Graphics
{
	using GraphicsPipelineID	= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  1 >;
	using MeshPipelineID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  2 >;
	using ComputePipelineID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  3 >;
	using RayTracingPipelineID	= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  4 >;
	using TilePipelineID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  5 >;
	using PipelineCacheID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  6 >;
	using PipelinePackID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  7 >;
	using DescriptorSetID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  8 >;
	using DescriptorSetLayoutID	= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start +  9 >;

	using BufferID				= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 10 >;
	using ImageID				= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 11 >;
	using BufferViewID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 12 >;
	using ImageViewID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 13 >;
	using RTGeometryID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 14 >;		// bottom-level AS
	using RTSceneID				= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 15 >;		// top-level AS
	using RTShaderBindingID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 16 >;		// shader binding table

	// for internal usage
	using SamplerID				= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 17 >;
	using PipelineLayoutID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 18 >;
	using RenderPassID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 19 >;
	using MemoryID				= HandleTmpl< 32, 32, Graphics::_hidden_::GraphicsIDs_Start + 20 >;

	using ImageInAtlasID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 30 >;

	using VideoSessionID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 40 >;
	using VideoBufferID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 41 >;
	using VideoImageID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 42 >;


	using UniformName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 1,  AE_OPTIMIZE_IDS >;
	using PushConstantName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 2,  AE_OPTIMIZE_IDS >;
	using SpecializationName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 3,  AE_OPTIMIZE_IDS >;
	using DescriptorSetName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 4,  AE_OPTIMIZE_IDS >;
	using PipelineLayoutName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 5,  AE_OPTIMIZE_IDS >;
	using PipelineName			= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 6,  AE_OPTIMIZE_IDS >;	// precompiled pipeline
	using PipelineTmplName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 7,  AE_OPTIMIZE_IDS >;	// pipeline template to create custom specialization
	using RenderTechName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 8,  AE_OPTIMIZE_IDS >;
	using RenderTechPassName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 9,  AE_OPTIMIZE_IDS >;
	using VertexBufferName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 11, AE_OPTIMIZE_IDS >;
	using RayTracingGroupName	= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 12, AE_OPTIMIZE_IDS >;
	using RTShaderBindingName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 15, AE_OPTIMIZE_IDS >;
	using SamplerName			= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 20, AE_OPTIMIZE_IDS >;
	using AttachmentName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 21, AE_OPTIMIZE_IDS >;
	using RenderPassName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 22, AE_OPTIMIZE_IDS >;
	using CompatRenderPassName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 23, AE_OPTIMIZE_IDS >;
	using SubpassName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 24, AE_OPTIMIZE_IDS >;
	using ShaderIOName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 25, AE_OPTIMIZE_IDS >;	// input attachment / render target name for Fragment / Tile shader
	using FeatureSetName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 26, AE_OPTIMIZE_IDS >;
	using ShaderStructName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 27, true >;
	using DSLayoutName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 28, AE_OPTIMIZE_IDS >;

	using RenderTargetName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 39, AE_OPTIMIZE_IDS >;
	using ImageInAtlasName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 40, AE_OPTIMIZE_IDS >;


	using PipelineIDTypes		= TypeList< GraphicsPipelineID, MeshPipelineID, ComputePipelineID, RayTracingPipelineID, TilePipelineID >;


	static constexpr AttachmentName		Attachment_Depth		{"Depth"};
	static constexpr AttachmentName		Attachment_DepthStencil	{"DepthStencil"};
	static constexpr AttachmentName		Attachment_ShadingRate	{"ShadingRate"};

	static constexpr SubpassName		Subpass_ExternalIn		{"ExternalIn"};
	static constexpr SubpassName		Subpass_ExternalOut		{"ExternalOut"};


	//
	// Image & ImageView weak reference
	//
	struct ImageAndViewID
	{
		ImageID			image;
		ImageViewID		view;

		constexpr ImageAndViewID ()									__NE___ {}
		constexpr ImageAndViewID (ImageID image, ImageViewID view)	__NE___ : image{image}, view{view} {}

		ND_ explicit operator bool ()								C_NE___	{ return image and view; }
	};


	//
	// Image & ImageView strong reference
	//
	struct StrongImageAndViewID
	{
		Strong<ImageID>			image;
		Strong<ImageViewID>		view;

		constexpr StrongImageAndViewID ()													__NE___ {}
		constexpr StrongImageAndViewID (Strong<ImageID> image, Strong<ImageViewID> view)	__NE___ : image{RVRef(image)}, view{RVRef(view)} {}

		constexpr StrongImageAndViewID (StrongImageAndViewID &&)							__NE___ = default;

		StrongImageAndViewID&  operator = (StrongImageAndViewID &&)							__NE___ = default;

		ND_ operator ImageAndViewID ()														C_NE___	{ return { image, view }; }
		ND_ explicit operator bool ()														C_NE___	{ return image and view; }
	};


	//
	// Autorelease Graphics Handle
	//
	template <typename IDType>
	struct GAutorelease;

	template <usize IndexSize, usize GenerationSize, uint UID>
	struct GAutorelease < HandleTmpl< IndexSize, GenerationSize, UID >>
	{
	// types
	public:
		using ID_t	= HandleTmpl< IndexSize, GenerationSize, UID >;
		using Self	= GAutorelease< ID_t >;


	// variables
	private:
		Strong<ID_t>	_id;


	// methods
	public:
		GAutorelease ()							__NE___	{}
		GAutorelease (Strong<ID_t> id)			__NE___	: _id{ RVRef(id) }			{}
		GAutorelease (Self &&other)				__NE___	: _id{ RVRef(other._id) }	{}
		~GAutorelease ()						__NE___	{ _ReleaseRef(); }

		GAutorelease (const Self &)				= delete;
		Self&  operator = (const Self &)		= delete;

		Self&  operator = (Strong<ID_t> rhs)	__NE___	{ _ReleaseRef();  _id = RVRef(rhs);		return *this; }
		Self&  operator = (Self &&rhs)			__NE___	{ _ReleaseRef();  _id = RVRef(rhs._id);	return *this; }

		ND_ ID_t			Get ()				C_NE___	{ return _id.Get(); }
		ND_ Strong<ID_t>	Release ()			__NE___	{ Strong<ID_t> tmp = RVRef(_id);  return tmp; }
		ND_ bool			IsValid ()			C_NE___	{ return _id.IsValid(); }

		ND_ explicit		operator bool ()	C_NE___	{ return IsValid(); }

		ND_ 				operator ID_t ()	C_NE___	{ return _id.Get(); }

	private:
		void  _ReleaseRef ()					__NE___;
	};


	template <typename IDType>
	GAutorelease (Strong<IDType>) -> GAutorelease<IDType>;


} // AE::Graphics
