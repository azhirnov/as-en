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
		bool						mouseLBDown		= false;
		bool						touchActive		= false;


	private:
		// imgui
		ImGuiContext*				_imguiCtx		= null;

		float						_pixToUI		= 1.f;		// surface coords to UI coords
		float						_uiToPix		= 1.f;		// UI coords to surface coords
		float						_scale;

		RenderTechPipelinesPtr		_rtech;
		GraphicsPipelineID			_ppln;
		Strong<DescriptorSetID>		_descSet;
		const DescSetBinding		_dsIndex			{0};

		Strong<BufferID>			_ub;
		StrongImageAndViewID		_font;


	// methods
	public:
		explicit ImGuiRenderer (ImGuiContext* ctx);
		~ImGuiRenderer ();

		ND_ bool  Init (RC<GfxLinearMemAllocator> gfxAlloc, RenderTechPipelinesPtr rtech);
			void  SetScale (float scale);
			
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

		ND_ bool  Upload (DirectCtx::Transfer &copyCtx);

	private:
		ND_ bool  _Update (const IOutputSurface::RenderTarget &rt, const Function<void()> &ui);
			bool  _DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData);
		ND_ bool  _UploadVB (DirectCtx::Draw &dctx, const ImDrawData &drawData);

			void  _UpdateScale (float2 pixToMm);
	};


} // AE::Samples::Demo
