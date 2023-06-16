// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptBasePass.h"

namespace AE::ResEditor
{

	//
	// Render Pass
	//

	class ScriptBaseRenderPass : public ScriptBasePass
	{
	// types
	protected:
		using ClearValue_t = RenderPassDesc::ClearValue_t;

		struct Output
		{
			String			name;
			ScriptImagePtr	rt;
			ImageLayer		layer;
			MipmapLevel		mipmap;
			EBlendFactor	srcFactorRGB	= Default;
			EBlendFactor	srcFactorA		= Default;
			EBlendFactor	dstFactorRGB	= Default;
			EBlendFactor	dstFactorA		= Default;
			EBlendOp		blendOpRGB		= Default;
			EBlendOp		blendOpA		= Default;
			bool			enableBlend		= false;
			ClearValue_t	clear;

			ND_ bool  HasClearValue ()	C_NE___	{ return not IsNullUnion( clear ); }
		};


	// variables
	protected:
		Array<Output>		_output;


	// methods
	protected:
		explicit ScriptBaseRenderPass (EFlags flags)													__Th___ : ScriptBasePass{flags} {}

		template <typename B>
		static void  _BindBaseRenderPass (B &binder, Bool withBlending)									__Th___;
		
		static void  _Output (Scripting::ScriptArgList args)											__Th___;
		static void  _OutputBlend (Scripting::ScriptArgList args)										__Th___;

	private:
		void  _Output2 (Scripting::ScriptArgList args)													__Th___;
		void  _OutputBlend2 (Scripting::ScriptArgList args)												__Th___;
	};

	


} // AE::ResEditor
