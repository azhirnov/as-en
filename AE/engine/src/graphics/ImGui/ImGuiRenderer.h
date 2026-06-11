// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "graphics/Common.h"

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
		struct imgui_vs_pc
		{
			static constexpr auto   TypeName = ShaderStructName{HashVal32{0xc83f6c98u}};  // 'imgui.vs.pc'

			float2  transform_c0;
			float2  transform_c1;
			float2  transform_c2;
		};
		StaticAssert( offsetof(imgui_vs_pc, transform_c0) == 0 );
		StaticAssert( offsetof(imgui_vs_pc, transform_c1) == 8 );
		StaticAssert( offsetof(imgui_vs_pc, transform_c2) == 16 );
		StaticAssert( sizeof(imgui_vs_pc) == 24 );

		struct imgui_fs_pc
		{
			static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa8dffb1au}};  // 'imgui.fs.pc'

			uint	texIndex;
		};
		StaticAssert( sizeof(imgui_fs_pc) == 4 );

		using MouseDownBits = BitSet<5>;


	public:
		static constexpr uint	TextureCount = 16;

		using PipelineInfo_t = ArrayView< Tuple< EPixelFormat, RenderTechPassName, PipelineName >>;

		struct StyleScope : Noncopyable
		{
		protected:
			ImGuiContext*	_imguiCtx;
			const int		_stackSize;

		public:
			explicit StyleScope (ImGuiContext* ctx)											__NE___;
			~StyleScope ()																	__NE___;
		};

		struct AEStyleScope : StyleScope {
			explicit AEStyleScope (ImGuiContext* ctx, Bool sRGB = True{})					__NE___;
		};

		struct AEStyleScope_StartBtn : StyleScope {
			explicit AEStyleScope_StartBtn (ImGuiContext* ctx = null, Bool sRGB = True{})	__NE___;
		};
		struct AEStyleScope_StopBtn : StyleScope {
			explicit AEStyleScope_StopBtn (ImGuiContext* ctx = null, Bool sRGB = True{})	__NE___;
		};

		struct DrawUtils
		{
			static void  DrawCursor (float2 pos, float scale = 1.f)							__NE___;
		};


	private:
		struct PipelineSet
		{
			RenderTechPassName::Optimized_t		pass;
			GraphicsPipelineID					ppln;
		};
		using PipelineMap_t		= FixedMap< EPixelFormat, PipelineSet, 8 >;
		using RenderTaskRef		= _Coro_::RenderTaskImpl::UserApi;
		using DescSetArray_t	= FixedArray< Strong<DescriptorSetID>, GraphicsConfig::MaxFrames >;

		enum class EScaleType
		{
			Unknown,	// without scale
			Fixed,
			Adaptive,
			AdaptiveFract,
		};


	// variables
	public:
		float2						mousePos;
		float2						mouseWheel;
		MouseDownBits				mouseBtnDown;
		bool						touchActive			= false;
		U8String					inputText;
		Array<Pair< int, bool >>	keyStates;			// ImGuiKey

	private:
		// imgui
		ImGuiContext*				_imguiCtx			= null;
		bool						_fontInitialized	= false;
		EScaleType					_scaleType			= Default;

		float						_pixToUI			= 1.f;		// surface coords to UI coords
		float						_uiToPix			= 1.f;		// UI coords to surface coords
		float						_scale				= -1.f;		// disable adaptive scaling

		RenderTechPipelinesPtr		_rtech;
		PipelineMap_t				_pplnMap;
		DescSetArray_t				_descSets;
		UniformName::Optimized_t	_texUniform;

		DescSetBinding				_dsIndex;
		PushConstantIndex			_vsPCIndex;						// imgui_vs_pc
		PushConstantIndex			_fsPCIndex;						// imgui_fs_pc

		StrongImageAndViewID		_font;


	// methods
	public:
		explicit ImGuiRenderer (ImGuiContext* ctx = null)										__NE___;
		~ImGuiRenderer ()																		__NE___;

		ND_ bool  Initialize (GfxMemAllocatorPtr		gfxAlloc,
							  RenderTechPipelinesPtr	rtech,
							  PipelineInfo_t			pplnInfo,
							  const DescriptorSetName	&dsName   = DescriptorSetName{"imgui.ds"},
							  UniformName::Ref			unTexture = UniformName{"un_Textures"})	__NE___;
			void  Deinitialize ()																__NE___;

			void  SetScale (float scale)														__NE___;
			void  SetAdaptiveScale (float scale, bool round = true)								__NE___;
			void  DisableScale ()																__NE___;

		// Convert screen space position to ui space.
		// Must be used inside 'updateUI' callback.
		//
		ND_ float	ToScreen (float  uiSpace)													C_NE___	{ return uiSpace * _uiToPix; }
		ND_ float2	ToScreen (float2 uiSpace)													C_NE___	{ return uiSpace * _uiToPix; }

		// Convert ui space position to screen space.
		// Must be used inside 'updateUI' callback.
		//
		ND_ float	ToUI (float  screenSpace)													C_NE___	{ return screenSpace * _pixToUI; }
		ND_ float2	ToUI (float2 screenSpace)													C_NE___	{ return screenSpace * _pixToUI; }

		ND_ bool  IsInitialized ()																C_NE___	{ return bool{_rtech}; }
		ND_ auto  GetRenderTech ()																__NE___	{ return Ptr{_rtech.get()}; }
		ND_ auto  GetContext ()																	__NE___	{ return _imguiCtx; }

		// Override all previous textures.
		// Index '0' is reserved for font texture.
		// Maximal size is 'TextureCount-1'.
		//
			bool  BindTextures (FrameUID				currentFrameId,
								ArrayView<ImageViewID>	ids)									__NE___;

		// v1 //
		ND_ bool  Draw (RenderTaskRef								rtask,
						App::IOutputSurface							&surface,
						const Function< void () >					&updateUI,
						const Function< void (DirectCtx::Draw &) >	&drawBefore = Default,
						const RenderPassDesc::ClearValue_t			&clearValue = RGBA32f{})	__Th___;

		ND_ bool  Draw (RenderTaskRef								rtask,
						DirectCtx::CommandBuffer					cmdbuf,
						const App::IOutputSurface::RenderTarget		&rt,
						const Function< void () >					&updateUI,
						const Function< void (DirectCtx::Draw &) >	&drawBefore	= Default,
						const RenderPassDesc::ClearValue_t			&clearValue = RGBA32f{})	__Th___;

		// v2 //
		ND_ bool  NeedUpload ()																	C_NE___	{ return not _fontInitialized; }
		ND_ bool  Upload (DirectCtx::Transfer						&ctx)						__Th___;

		ND_ bool  Render (DirectCtx::Draw							&ctx,
						  const App::IOutputSurface::RenderTarget	&rt,
						  const Function<void()>					&updateUI)					__Th___;

		ND_ bool  Render2 (DirectCtx::Graphics						&ctx,
						   App::IOutputSurface						&surface,
						   const Function<void()>					&updateUI,
						   const RenderPassDesc::ClearValue_t		&clearValue = RGBA32f{})	__Th___;


	private:
		ND_ bool  _Initialize (GfxMemAllocatorPtr gfxAlloc, RenderTechPipelinesPtr rtech);
		ND_ bool  _Update (const App::IOutputSurface::RenderTarget &rt, const Function<void()> &ui);
			bool  _DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData, GraphicsPipelineID, ESurfaceTransform);
		ND_ bool  _UploadVB (DirectCtx::Draw &dctx, const ImDrawData &drawData);
		ND_ bool  _Upload (DirectCtx::Transfer &copyCtx);

			void  _UpdateScale (float pixToMm);
	};


} // AE::Graphics

#endif // AE_ENABLE_IMGUI
