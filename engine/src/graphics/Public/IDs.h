// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "base/Utils/HandleTmpl.h"
#include "base/Utils/NamedID.h"

namespace AE::Graphics::_hidden_
{
	// tag: HandleTmpl UID
	static constexpr uint	GraphicsIDs_Start	= 1 << 24;
	static constexpr uint	VulkanIDs_Start		= 2 << 24;
	static constexpr uint	MetalIDs_Start		= 3 << 24;

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
	
	//using MaterialID			= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 20 >;
	using ImageInAtlasID		= HandleTmpl< 16, 16, Graphics::_hidden_::GraphicsIDs_Start + 21 >;


	using UniformName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 1,  AE_OPTIMIZE_IDS >;
	using PushConstantName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 2,  AE_OPTIMIZE_IDS >;
	using SpecializationName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 3,  AE_OPTIMIZE_IDS >;
	using DescriptorSetName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 4,  AE_OPTIMIZE_IDS >;
	using PipelineLayoutName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 5,  AE_OPTIMIZE_IDS >;
	using PipelineName			= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 6,  AE_OPTIMIZE_IDS >;	// precompiled pipeline
	using PipelineTmplName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 7,  AE_OPTIMIZE_IDS >;	// pipeline template to create custom specialization
	using RenderTechName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 8,  AE_OPTIMIZE_IDS >;
	using RenderTechPassName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 9,  AE_OPTIMIZE_IDS >;
	using VertexBufferName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 11, AE_OPTIMIZE_IDS >;
	using RayTracingGroupName	= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 12, AE_OPTIMIZE_IDS >;
	//using RTGeometryName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 13, AE_OPTIMIZE_IDS >;
	//using RTInstanceName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 14, AE_OPTIMIZE_IDS >;
	using RTShaderBindingName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 15, AE_OPTIMIZE_IDS >;
	using SamplerName			= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 20, AE_OPTIMIZE_IDS >;
	using AttachmentName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 21, AE_OPTIMIZE_IDS >;
	using RenderPassName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 22, AE_OPTIMIZE_IDS >;
	using CompatRenderPassName	= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 23, AE_OPTIMIZE_IDS >;
	using SubpassName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 24, AE_OPTIMIZE_IDS >;
	using ShaderIOName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 25, AE_OPTIMIZE_IDS >;	// input attachment / render target name for Fragment / Tile shader
	using FeatureSetName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 26, AE_OPTIMIZE_IDS >;
	using ShaderStructName		= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 27, true >;
	using DSLayoutName			= NamedID< 32, Graphics::_hidden_::NamedIDs_Start + 28, AE_OPTIMIZE_IDS >;
	
	using ImageInAtlasName		= NamedID< 64, Graphics::_hidden_::NamedIDs_Start + 40, AE_OPTIMIZE_IDS >;


	static constexpr AttachmentName		Attachment_Depth		{"Depth"};
	static constexpr AttachmentName		Attachment_DepthStencil	{"DepthStencil"};

	static constexpr SubpassName		Subpass_ExternalIn		{"ExternalIn"};
	static constexpr SubpassName		Subpass_ExternalOut		{"ExternalOut"};


	//
	// Image & ImageView weak reference
	//
	struct ImageAndViewID
	{
		ImageID			image;
		ImageViewID		view;

		constexpr ImageAndViewID () __NE___ {}
		constexpr ImageAndViewID (ImageID image, ImageViewID view) __NE___ : image{image}, view{view} {}

		ND_ explicit operator bool ()	C_NE___	{ return image and view; }
	};
	

	//
	// Image & ImageView strong reference
	//
	struct StrongImageAndViewID
	{
		Strong<ImageID>			image;
		Strong<ImageViewID>		view;
		
		constexpr StrongImageAndViewID () __NE___ {}
		constexpr StrongImageAndViewID (Strong<ImageID> image, Strong<ImageViewID> view) __NE___ : image{RVRef(image)}, view{RVRef(view)} {}

		constexpr StrongImageAndViewID (StrongImageAndViewID &&) __NE___ = default;

		StrongImageAndViewID&  operator = (StrongImageAndViewID &&) __NE___ = default;

		operator ImageAndViewID ()		C_NE___	{ return { image, view }; }
		ND_ explicit operator bool ()	C_NE___	{ return image and view; }
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

	// variables
	private:
		Strong<ID_t>	_id;


	// methods
	public:
		GAutorelease ()						__NE___	{}
		GAutorelease (Strong<ID_t> id)		__NE___	: _id{RVRef(id)} {}
		~GAutorelease ()					__NE___;
		
		ND_ ID_t		Get ()				C_NE___	{ return _id.Get(); }
		ND_ ID_t		Release ()			__NE___	{ return _id.Release(); }
		ND_ bool		IsValid ()			C_NE___	{ return _id.IsValid(); }
		
		ND_ explicit	operator bool ()	C_NE___	{ return IsValid(); }

		ND_ 			operator ID_t ()	C_NE___	{ return _id.Get(); }
	};
	
	
	template <typename IDType>
	GAutorelease (Strong<IDType>) -> GAutorelease<IDType>;


} // AE::Graphics
