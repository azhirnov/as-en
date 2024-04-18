// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "graphics_hl/GraphicsHL.pch.h"

struct ImGuiContext;
struct ImDrawData;

namespace AE::Graphics
{

	//
	// ImGui Renderer
	//

	class ImGuiRenderer
	{
	// types
	public:
		struct imgui_ub
		{
			static constexpr auto  TypeName = ShaderStructName{HashVal32{0xb41e4542u}};  // 'imgui_ub'

			float2  scale;
			float2  translate;
		};
		StaticAssert( offsetof(imgui_ub, scale) == 0 );
		StaticAssert( offsetof(imgui_ub, translate) == 8 );
		StaticAssert( sizeof(imgui_ub) == 16 );

		using PipelineInfo_t = ArrayView< Tuple< EPixelFormat, RenderTechPassName, PipelineName >>;

	private:
		struct PipelineSet
		{
			RenderTechPassName::Optimized_t		pass;
			GraphicsPipelineID					ppln;
		};
		using PipelineMap_t	= FixedMap< EPixelFormat, PipelineSet, 8 >;


	// variables
	public:
		float2						mousePos;
		float2						mouseWheel;
		bool						mouseLBDown			= false;
		bool						touchActive			= false;

	private:
		// imgui
		ImGuiContext*				_imguiCtx			= null;
		bool						_fontInitialized	= false;

		float						_pixToUI			= 1.f;		// surface coords to UI coords
		float						_uiToPix			= 1.f;		// UI coords to surface coords
		float						_scale;

		RenderTechPipelinesPtr		_rtech;
		PipelineMap_t				_pplnMap;
		Strong<DescriptorSetID>		_descSet;

		const DescSetBinding		_dsIndex			{0};
		const PushConstantIndex		_pcIndex			{ 0_b, EShader::Vertex };

		StrongImageAndViewID		_font;


	// methods
	public:
		explicit ImGuiRenderer (ImGuiContext* ctx = null)										__NE___;
		~ImGuiRenderer ()																		__NE___;

		ND_ bool  Initialize (GfxMemAllocatorPtr		gfxAlloc,
							  RenderTechPipelinesPtr	rtech,
							  PipelineInfo_t			pplnInfo,
							  const DescriptorSetName	&dsName = DescriptorSetName{"imgui.ds"},
							  UniformName::Ref			unTexture = UniformName{"un_Texture"})	__NE___;
			void  Deinitialize ()																__NE___;

			void  SetScale (float scale)														__NE___;

		ND_ bool	IsInitialized ()															C_NE___	{ return bool{_rtech}; }
		ND_ auto	GetRenderTech ()															__NE___	{ return Ptr{_rtech.get()}; }
		ND_ auto	GetContext ()																__NE___	{ return _imguiCtx; }


		// v1
		ND_ bool  Draw (RenderTask									&rtask,
						App::IOutputSurface							&surface,
						const Function< void () >					&updateUI,
						const Function< void (DirectCtx::Draw &) >	&drawBefore,
						const RenderPassDesc::ClearValue_t			&clearValue = RGBA32f{})	__Th___;

		ND_ bool  Draw (RenderTask									&rtask,
						DirectCtx::CommandBuffer					cmdbuf,
						const App::IOutputSurface::RenderTarget		&rt,
						const Function< void () >					&updateUI,
						const Function< void (DirectCtx::Draw &) >	&drawBefore,
						const RenderPassDesc::ClearValue_t			&clearValue = RGBA32f{})	__Th___;

		// v2
		ND_ bool  Upload (DirectCtx::Transfer						&ctx)						__Th___;
		ND_ bool  Render (DirectCtx::Draw							&ctx,
						  const App::IOutputSurface::RenderTarget	&rt,
						  const Function<void()>					&updateUI)					__Th___;


	private:
		ND_ bool  _Initialize (GfxMemAllocatorPtr gfxAlloc, RenderTechPipelinesPtr rtech);
		ND_ bool  _Update (const App::IOutputSurface::RenderTarget &rt, const Function<void()> &ui);
			bool  _DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData, GraphicsPipelineID ppln);
		ND_ bool  _UploadVB (DirectCtx::Draw &dctx, const ImDrawData &drawData);
		ND_ bool  _Upload (DirectCtx::Transfer &copyCtx);

			void  _UpdateScale (float pixToMm);
	};


} // AE::Graphics

#endif // AE_ENABLE_IMGUI
