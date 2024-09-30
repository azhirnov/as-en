// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptBaseRenderPass.h"
#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{

/*
=================================================
	_Output
=================================================
*/
	void  ScriptBaseRenderPass::_Output (Scripting::ScriptArgList args) __Th___
	{
		args.GetObject< ScriptBaseRenderPass >()->_Output2( args );
	}

	void  ScriptBaseRenderPass::_Output2 (Scripting::ScriptArgList args) __Th___
	{
		auto&	dst	= _output.emplace_back();
		uint	idx	= 0;

		if ( args.IsArg< String const& >(idx) ) {
			dst.name = args.Arg< String const& >(idx++);
		}else{
			dst.name = "out_Color"s << ToString(_output.size()-1);
		}

		if ( args.IsArg< ScriptImagePtr const& >(idx) )
		{
			dst.rt = args.Arg< ScriptImagePtr const& >(idx++);
			CHECK_THROW_MSG( dst.rt );
			dst.rt->AddUsage( dst.rt->IsDepthOrStencil() ? EResourceUsage::DepthStencil : EResourceUsage::ColorAttachment );

			if ( dst.rt->IsDepthOrStencil() )
				dst.name = "DepthStencil";

			if ( dst.rt->IsMutableDimension() )
				_SetDynamicDimension( dst.rt->DimensionRC() );
		}
		else
			CHECK_THROW_MSG( false, "image is not defined" );

		if ( args.IsArg< ImageLayer const& >(idx) )
		{
			dst.layer = args.Arg< ImageLayer const& >(idx++);
			if ( args.IsArg<uint>(idx) )
				dst.layerCount = args.Arg<uint>(idx++);
		}

		if ( args.IsArg< MipmapLevel const& >(idx) )
		{
			dst.mipmap = args.Arg< MipmapLevel const& >(idx++);
		}

		if ( args.IsArg< RGBA32f const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear = args.Arg< RGBA32f const& >(idx++);
		}
		else
		if ( args.IsArg< RGBA32u const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear = args.Arg< RGBA32u const& >(idx++);
		}
		else
		if ( args.IsArg< RGBA32i const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear = args.Arg< RGBA32i const& >(idx++);
		}
		else
		if ( args.IsArg< DepthStencil const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsDepthOrStencil() );
			dst.clear = args.Arg< DepthStencil const& >(idx++);
		}

		CHECK_THROW_MSG( idx == args.ArgCount() );
	}

/*
=================================================
	_OutputBlend
=================================================
*/
	void  ScriptBaseRenderPass::_OutputBlend (Scripting::ScriptArgList args) __Th___
	{
		args.GetObject< ScriptBaseRenderPass >()->_OutputBlend2( args );
	}

	void  ScriptBaseRenderPass::_OutputBlend2 (Scripting::ScriptArgList args) __Th___
	{
		auto&	dst	= _output.emplace_back();
		uint	idx	= 0;

		FixedArray< EBlendFactor, 4 >	b_factors;	// src, dst	| srcRGB, dstRGB, srcA, dstA
		FixedArray< EBlendOp, 2 >		b_ops;		// op		| opRGB, opA

		if ( args.IsArg< String const& >(idx) ) {
			dst.name = args.Arg< String const& >(idx++);
		}else{
			dst.name = "out_Color"s << ToString(_output.size()-1);
		}

		if ( args.IsArg< ScriptImagePtr const& >(idx) )
		{
			dst.rt = args.Arg< ScriptImagePtr const& >(idx++);
			CHECK_THROW_MSG( dst.rt );
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.rt->AddUsage( EResourceUsage::ColorAttachment );

			if ( dst.rt->IsMutableDimension() )
				_SetDynamicDimension( dst.rt->DimensionRC() );
		}
		else
			CHECK_THROW_MSG( false, "image is not defined" );

		if ( args.IsArg< ImageLayer const& >(idx) )
		{
			dst.layer = args.Arg< ImageLayer const& >(idx++);
			if ( args.IsArg<uint>(idx) )
				dst.layerCount = args.Arg<uint>(idx++);
		}

		if ( args.IsArg< MipmapLevel const& >(idx) )
		{
			dst.mipmap = args.Arg< MipmapLevel const& >(idx++);
		}

		for (; idx < args.ArgCount();)
		{
			if ( args.IsArg< EBlendFactor >(idx) )
			{
				CHECK_THROW_MSG( b_factors.try_push_back( args.Arg<EBlendFactor>(idx++) ), "too many 'EBlendFactor'" );
			}
			else
			if ( args.IsArg< EBlendOp >(idx) )
			{
				CHECK_THROW_MSG( b_ops.try_push_back( args.Arg<EBlendOp>(idx++) ), "too many 'EBlendOp'" );
			}
			else
				CHECK_THROW_MSG( false, "unsupported arg type '"s << args.GetArgTypename(idx) << "' in arg (" << ToString(idx) << ")" );
		}
		CHECK_THROW_MSG( idx == args.ArgCount() );

		if ( b_factors.size() == 2 and b_ops.size() == 1 )
		{
			dst.srcFactorRGB	= dst.srcFactorA	= b_factors[0];
			dst.dstFactorRGB	= dst.dstFactorA	= b_factors[1];
			dst.blendOpRGB		= dst.blendOpA		= b_ops[0];
			dst.enableBlend		= true;
		}
		else
		if ( b_factors.size() == 4 and b_ops.size() == 2 )
		{
			dst.srcFactorRGB	= b_factors[0];
			dst.srcFactorA		= b_factors[2];
			dst.dstFactorRGB	= b_factors[1];
			dst.dstFactorA		= b_factors[3];
			dst.blendOpRGB		= b_ops[0];
			dst.blendOpA		= b_ops[1];
			dst.enableBlend		= true;
		}
		else
		{
			CHECK_THROW_MSG( b_factors.empty() and b_ops.empty(),
				"unsupported fn signature" );
		}
	}

/*
=================================================
	_SetDepthRange
=================================================
*/
	void  ScriptBaseRenderPass::_SetDepthRange (float min, float max) __Th___
	{
		CHECK_THROW_MSG( min < max );

		_depthRange = float2{min, max};
	}

/*
=================================================
	_AddViewport
=================================================
*/
	void  ScriptBaseRenderPass::_AddViewport0 (const RectF &rect, float minDepth, float maxDepth, const RectF &scissor, const packed_float2 &wScale) __Th___
	{
		CHECK_THROW_MSG( _viewports.size() == _wScaling.size() );
		CHECK_THROW_MSG( _viewports.size() == _scissors.size() );
		CHECK_THROW_MSG( GraphicsScheduler().GetFeatureSet().clipSpaceWScalingNV == FeatureSet::EFeature::RequireTrue,
			"'clipSpaceWScalingNV' feature is not supported" );

		auto&	vp	= _viewports.emplace_back();
		vp.rect		= rect;
		vp.minDepth	= minDepth;
		vp.maxDepth	= maxDepth;

		_wScaling.push_back( wScale );
		_scissors.push_back( scissor );
	}

	void  ScriptBaseRenderPass::_AddViewport1 (const RectF &rect, float minDepth, float maxDepth) __Th___
	{
		auto&	vp	= _viewports.emplace_back();
		vp.rect		= rect;
		vp.minDepth	= minDepth;
		vp.maxDepth	= maxDepth;
	}

	void  ScriptBaseRenderPass::_AddViewport2 (const RectF &rect) __Th___
	{
		_AddViewport1( rect, _depthRange.x, _depthRange.y );
	}

	void  ScriptBaseRenderPass::_AddViewport3 (float left, float top, float right, float bottom) __Th___
	{
		_AddViewport1( RectF{left, top, right, bottom}, _depthRange.x, _depthRange.y );
	}

	void  ScriptBaseRenderPass::_AddViewport4 (const RectF &rect, float minDepth, float maxDepth, const RectF &scissor) __Th___
	{
		CHECK_THROW_MSG( _viewports.size() == _scissors.size() );

		auto&	vp	= _viewports.emplace_back();
		vp.rect		= rect;
		vp.minDepth	= minDepth;
		vp.maxDepth	= maxDepth;

		_scissors.push_back( scissor );
	}

/*
=================================================
	_InOut
=================================================
*/
	void  ScriptBaseRenderPass::_InOut (const String &inName, const String& outName, const ScriptImagePtr &rt) __Th___
	{
		CHECK_THROW_MSG( rt );
		CHECK_THROW_MSG( not inName.empty() );
		CHECK_THROW_MSG( not outName.empty() );
		CHECK_THROW_MSG( inName != outName );

		auto&	dst	= _output.emplace_back();

		dst.name	= outName;
		dst.inName	= inName;
		dst.rt		= rt;

		rt->AddUsage( rt->IsDepthOrStencil() ? EResourceUsage::DepthStencil : EResourceUsage::ColorAttachment );
		rt->AddUsage( EResourceUsage::InputAttachment );

		if ( rt->IsMutableDimension() )
			_SetDynamicDimension( rt->DimensionRC() );
	}


} // AE::ResEditor
