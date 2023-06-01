// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "demo/Core/ISample.h"

struct ImGuiContext;
struct ImDrawData;

namespace AE::Samples::Demo
{

	//
	// ImGui Renderer
	//

	class ImGuiRenderer
	{
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
		GraphicsPipelineID			_ppln;
		Strong<DescriptorSetID>		_descSet;

		const DescSetBinding		_dsIndex			{0};
		const PushConstantIndex		_pcIndex			{ 0_b, EShader::Vertex };

		StrongImageAndViewID		_font;


	// methods
	public:
		explicit ImGuiRenderer (ImGuiContext* ctx);
		~ImGuiRenderer ();

		ND_ bool  Init (GfxMemAllocatorPtr gfxAlloc, RenderTechPipelinesPtr rtech);
			void  SetScale (float scale);
			
		// v1
		ND_ bool  Draw (RenderTask &rtask,
						IOutputSurface &surface,
						const Function< void () > &ui,
						const Function< void (DirectCtx::Draw &) > &draw);

		ND_ bool  Draw (RenderTask &rtask,
						DirectCtx::CommandBuffer cmdbuf,
						const RenderTechPassName &passName,
						const IOutputSurface::RenderTarget &rt,
						const Function< void () > &ui,
						const Function< void (DirectCtx::Draw &) > &draw);

		ND_ bool  Draw (RenderTask &rtask,
						DirectCtx::CommandBuffer cmdbuf,
						const RenderTechPassName &passName,
						const IOutputSurface::RenderTarget &rt,
						const RenderPassDesc::ClearValue_t &clearValue,
						const Function< void () > &ui,
						const Function< void (DirectCtx::Draw &) > &draw);

		// v2
		ND_ bool  Upload (DirectCtx::Transfer &ctx);
		ND_ bool  Render (DirectCtx::Draw &ctx,
						  const IOutputSurface::RenderTarget &rt,
						  const Function<void()> &ui);


	private:
		ND_ bool  _Update (const IOutputSurface::RenderTarget &rt, const Function<void()> &ui);
			bool  _DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData);
		ND_ bool  _UploadVB (DirectCtx::Draw &dctx, const ImDrawData &drawData);
		ND_ bool  _Upload (DirectCtx::Transfer &copyCtx);

			void  _UpdateScale (float2 pixToMm);
	};


} // AE::Samples::Demo
