// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/UI/Widget.h"

namespace AE::UI
{

	//
	// Screen
	//

	class Screen final : public EnableRC<Screen>
	{
		friend class SystemImpl;

	// types
	public:
		using PreDrawParams		= Widget::PreDrawParams;
		using Draw1Params		= Widget::Draw1Params;
		using Draw2Params		= Widget::Draw2Params;

	private:
		using ViewStack_t		= Array< RC<Widget> >;
		using DrawQueue_t		= Array< Widget* >;
		using ActionQueueReader	= App::IInputActions::ActionQueueReader;

		class ProcessInputTask;


	// variables
	private:
		SurfaceDimensions		_surfDim;
		IController::InputState	_inputState;

		ViewStack_t				_stack;
		DrawQueue_t				_drawQueue;


		FrameUID				_frameId;
		GraphicsPipelineID		_fillStencilPpln;
		DescriptorSetID			_descSet;
		BufferID				_ubuffer;
		Bytes16u				_ubufferSize;
		ubyte					_indexInPool	= UMax;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		Screen ()																			__NE___;

		bool  Add (RC<Widget>)																__NE___;
		bool  Remove (RC<Widget>)															__NE___;

		void  SetDimensions (const SurfaceDimensions &)										__NE___;
		void  SetDimensions (const App::IOutputSurface::RenderTarget &rt)					__NE___;

		bool  Update ()																		__NE___;
		void  PreDraw (const PreDrawParams &params, TransferContext_t &ctx)					__Th___;
		void  DrawPass1 (const Draw1Params &params, Canvas &canvas, DrawContext_t &ctx)		__Th___;	// write to stencil
		void  DrawPass2 (const Draw2Params &params, Canvas &canvas, DrawContext_t &ctx)		__Th___;	// draw content with stencil test
		void  DbgDrawLayouts (Canvas &canvas, DrawContext_t &ctx)							__Th___;

		ND_ AsyncTask	ProcessInputAsync (ActionQueueReader, ArrayView<AsyncTask> deps)	__NE___;
		void			ProcessInput (ActionQueueReader)									__NE___;


	protected:
		ND_ bool  _IsInitialized ()															C_NE___	{ return _indexInPool != UMax; }

		bool  _SetMaterial (GraphicsPipelineID ppln, DescriptorSetID descSet,
							BufferID ubuf, Bytes16u ubSize)									__NE___;

		void  _ReleaseObject ()																__NE_OV;

		ND_ uint  _GlobalUBufOffset ()														C_NE___;
	};


} // AE::UI
