// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptBasePass.h"

namespace AE::ResEditor
{

	//
	// Render Pass
	//

	class ScriptBaseRenderPass : public ScriptBasePass
	{
	// types
	protected:
		using ClearValue_t		= RenderPassDesc::ClearValue_t;
		using Viewports_t		= FixedArray< Viewport, GraphicsConfig::MaxViewports >;
		using Scissors_t		= FixedArray< RectF, GraphicsConfig::MaxViewports >;

		struct Output
		{
			String				name;			// only for color attachment
			String				inName;			// only for input attachment
			ScriptImagePtr		rt;
			ImageLayer			layer;
			uint				layerCount		= UMax;
			MipmapLevel			mipmap;
			EBlendFactor		srcFactorRGB	= Default;
			EBlendFactor		srcFactorA		= Default;
			EBlendFactor		dstFactorRGB	= Default;
			EBlendFactor		dstFactorA		= Default;
			EBlendOp			blendOpRGB		= Default;
			EBlendOp			blendOpA		= Default;
			bool				enableBlend		= false;
			ClearValue_t		clear;
			EAttachmentLoadOp	loadOp			= EAttachmentLoadOp::Load;
			EAttachmentStoreOp	storeOp			= EAttachmentStoreOp::Store;
			EResourceUsage		usage			= Default;		// single bit
		};


	// variables
	protected:
		Array<Output>		_output;
		float2				_depthRange		{0.f, 1.f};
		Viewports_t			_viewports;
		Scissors_t			_scissors;
	private:
		bool				_subsampled		= false;


	// methods
	protected:
		template <typename B>
		static void  _BindBaseRenderPass (B &binder, Bool withBlending, Bool withRWAtt) __Th___;

		static void  _Output (Scripting::ScriptArgList args)						__Th___;
		static void  _OutputBlend (Scripting::ScriptArgList args)					__Th___;
		static void  _InOut (Scripting::ScriptArgList args)							__Th___;

		static void  _FragmentShadingRate (Scripting::ScriptArgList args)			__Th___;
		static void  _FragmentDensityMap (Scripting::ScriptArgList args)			__Th___;

		void  _MoveTo (OUT ScriptBaseRenderPass &dst)								__NE___;

	private:
		void  _Output2 (Scripting::ScriptArgList args)								__Th___;
		void  _OutputBlend2 (Scripting::ScriptArgList args)							__Th___;

		void  _InOut2 (Scripting::ScriptArgList args)								__Th___;
		void  _Input (const String &, const ScriptImagePtr &, const String&)		__Th___;

		void  _SetDepthRange (float min, float max)									__Th___;

		void  _AddViewport1 (const RectF &rect, float minDepth, float maxDepth)		__Th___;
		void  _AddViewport2 (const RectF &rect)										__Th___;
		void  _AddViewport3 (float left, float top, float right, float bottom)		__Th___;
		void  _AddViewport4 (const RectF &rect, float minDepth, float maxDepth,
							 const RectF &scissor)									__Th___;
	};


} // AE::ResEditor
