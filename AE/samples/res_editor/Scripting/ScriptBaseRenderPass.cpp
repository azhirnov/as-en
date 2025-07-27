// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptBaseRenderPass.h"
#include "Scripting/ScriptExe.h"

namespace AE::ResEditor
{

/*
=================================================
	_Output
=================================================
*/
	void  ScriptBaseRenderPass::_Output (Scripting::ScriptArgList args) __Th___
	{
		auto*	obj = args.GetObject< ScriptBaseRenderPass >();
		obj->_Output2( args );
		
		auto&	dst = obj->_output.back();
		dst.rt->AddUsage( dst.usage );

		if ( obj->_subsampled )
			dst.rt->AddUsage( EResourceUsage::SubsampledAttachment );

		if ( dst.rt->IsMutableDimension() )
			obj->_SetDynamicDimension( dst.rt->DimensionRC() );
	}
	
/*
=================================================
	_Output2
=================================================
*/
	void  ScriptBaseRenderPass::_Output2 (Scripting::ScriptArgList args) __Th___
	{
		auto&	dst		 = _output.emplace_back();
		uint	idx		 = 0;
		bool	def_name = false;

		if ( args.IsArg< String const& >(idx) ) {
			dst.name = args.Arg< String const& >(idx++);
		}else{
			dst.name = "out_Color"s << ToString(_output.size()-1);
			def_name = true;
		}

		if ( args.IsArg< ScriptImagePtr const& >(idx) )
		{
			dst.rt = args.Arg< ScriptImagePtr const& >(idx++);
			CHECK_THROW_MSG( dst.rt );
			dst.usage = (dst.rt->IsDepthOrStencil() ? EResourceUsage::DepthStencil : EResourceUsage::ColorAttachment);

			if ( dst.rt->IsDepthOrStencil() and def_name )
				dst.name = "DepthStencil";
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
			dst.clear	= args.Arg< RGBA32f const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}
		else
		if ( args.IsArg< RGBA32u const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear	= args.Arg< RGBA32u const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}
		else
		if ( args.IsArg< RGBA32i const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear	= args.Arg< RGBA32i const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}
		else
		if ( args.IsArg< DepthStencil const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsDepthOrStencil() );
			dst.clear	= args.Arg< DepthStencil const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}

		if ( args.IsArg< EAttachmentLoadOp >(idx) )
		{
			CHECK_THROW_MSG( IsNullUnion(dst.clear),
				"clear value is defined, loadOp is set to 'clear'" );

			dst.loadOp = args.Arg<EAttachmentLoadOp>(idx++);

			CHECK_THROW_MSG( dst.loadOp != EAttachmentLoadOp::Clear,
				"can not define 'clear' loadOp, use clear value instead" );
		}
		if ( args.IsArg< EAttachmentStoreOp >(idx) )
		{
			dst.storeOp = args.Arg<EAttachmentStoreOp>(idx++);
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
		auto*	obj = args.GetObject< ScriptBaseRenderPass >();
		obj->_OutputBlend2( args );
		
		auto&	dst = obj->_output.back();
		dst.rt->AddUsage( dst.usage );
		
		if ( obj->_subsampled )
			dst.rt->AddUsage( EResourceUsage::SubsampledAttachment );

		if ( dst.rt->IsMutableDimension() )
			obj->_SetDynamicDimension( dst.rt->DimensionRC() );
	}
	
/*
=================================================
	_OutputBlend2
=================================================
*/
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
			dst.usage = EResourceUsage::ColorAttachment;
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
	_FragmentShadingRate
=================================================
*/
	 void  ScriptBaseRenderPass::_FragmentShadingRate (Scripting::ScriptArgList args) __Th___
	 {
		auto*	obj = args.GetObject< ScriptBaseRenderPass >();
		
		for (auto& out : obj->_output)
		{
			CHECK_THROW_MSG( out.usage != EResourceUsage::FragShadingRate,
				"'FragShadingRate' already added" );
			
			CHECK_THROW_MSG( out.usage != EResourceUsage::FragDensityMap,
				"can not combine 'FragShadingRate' with 'FragDensityMap'" );
		}

		obj->_Output2( args );

		auto&	dst = obj->_output.back();
		dst.name	= "ShadingRate";
		dst.usage	= EResourceUsage::FragShadingRate;
		dst.loadOp	= EAttachmentLoadOp::Load;
		dst.storeOp	= EAttachmentStoreOp::None;
		dst.rt->AddUsage( dst.usage );
	 }
	 
/*
=================================================
	_FragmentDensityMap
=================================================
*/
	 void  ScriptBaseRenderPass::_FragmentDensityMap (Scripting::ScriptArgList args) __Th___
	 {
		auto*	obj = args.GetObject< ScriptBaseRenderPass >();

		for (auto& out : obj->_output)
		{
			CHECK_THROW_MSG( out.usage != EResourceUsage::FragDensityMap,
				"'FragDensityMap' already added" );
			
			CHECK_THROW_MSG( out.usage != EResourceUsage::FragShadingRate,
				"can not combine 'FragShadingRate' with 'FragDensityMap'" );

			out.rt->AddUsage( EResourceUsage::SubsampledAttachment );
		}

		obj->_Output2( args );
		
		auto&	dst = obj->_output.back();
		dst.name	= "FragmentDensity";
		dst.usage	= EResourceUsage::FragDensityMap;
		dst.loadOp	= EAttachmentLoadOp::Load;
		dst.storeOp = EAttachmentStoreOp::None;
		dst.rt->AddUsage( dst.usage );

		obj->_subsampled = true;
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
		CHECK_THROW_MSG( rect.bottom <= 1.f and rect.right <= 1.f, "must be in unorm coords" );

		auto&	vp	= _viewports.emplace_back();
		vp.rect		= rect;
		vp.minDepth	= minDepth;
		vp.maxDepth	= maxDepth;

		_wScaling.push_back( wScale );
		_scissors.push_back( scissor );
	}

	void  ScriptBaseRenderPass::_AddViewport1 (const RectF &rect, float minDepth, float maxDepth) __Th___
	{
		CHECK_THROW_MSG( rect.bottom <= 1.f and rect.right <= 1.f, "must be in unorm coords" );

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
		CHECK_THROW_MSG( rect.bottom <= 1.f and rect.right <= 1.f, "must be in unorm coords" );

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
	void  ScriptBaseRenderPass::_InOut (Scripting::ScriptArgList args) __Th___
	{
		auto*	obj = args.GetObject< ScriptBaseRenderPass >();
		obj->_InOut2( args );
	}

	void  ScriptBaseRenderPass::_InOut2 (Scripting::ScriptArgList args) __Th___
	{
		auto&	dst	= _output.emplace_back();
		uint	idx	= 0;
		
		if ( args.IsArg< String const& >(idx) ){
			dst.inName = args.Arg< String const& >(idx++);
		}
		if ( args.IsArg< String const& >(idx) ){
			dst.name = args.Arg< String const& >(idx++);
		}

		CHECK_THROW_MSG( not dst.inName.empty() );
		CHECK_THROW_MSG( dst.inName != dst.name );
		
		if ( args.IsArg< ScriptImagePtr const& >(idx) )
		{
			dst.rt = args.Arg< ScriptImagePtr const& >(idx++);
			CHECK_THROW_MSG( dst.rt );

			dst.usage  = (dst.rt->IsDepthOrStencil() ? EResourceUsage::DepthStencil : EResourceUsage::ColorAttachment);
			dst.usage |= EResourceUsage::InputAttachment;
			dst.rt->AddUsage( dst.usage );
		}
		else
			CHECK_THROW_MSG( false, "image is not defined" );

		if ( args.IsArg< RGBA32f const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear	= args.Arg< RGBA32f const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}
		else
		if ( args.IsArg< RGBA32u const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear	= args.Arg< RGBA32u const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}
		else
		if ( args.IsArg< RGBA32i const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsColor() );
			dst.clear	= args.Arg< RGBA32i const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}
		else
		if ( args.IsArg< DepthStencil const& >(idx) )
		{
			CHECK_THROW_MSG( dst.rt->IsDepthOrStencil() );
			dst.clear	= args.Arg< DepthStencil const& >(idx++);
			dst.loadOp	= EAttachmentLoadOp::Clear;
		}

		CHECK_THROW_MSG( idx == args.ArgCount() );

		if ( dst.rt->IsDepthOrStencil() and dst.name.empty() )
			dst.name = "DepthStencil";
		else
			CHECK_THROW_MSG( not dst.name.empty() );

		if ( dst.rt->IsMutableDimension() )
			_SetDynamicDimension( dst.rt->DimensionRC() );
	}
	
/*
=================================================
	_Input
=================================================
*/
	void  ScriptBaseRenderPass::_Input (const String &inName, const ScriptImagePtr &rt, const String &attName) __Th___
	{
		CHECK_THROW_MSG( rt );
		CHECK_THROW_MSG( not inName.empty() );
		CHECK_THROW_MSG( inName != attName );

		auto&	dst		= _output.emplace_back();
		bool	is_ds	= rt->IsDepthOrStencil();

		dst.name	= attName;
		dst.inName	= inName;
		dst.rt		= rt;
		dst.usage	= EResourceUsage::InputAttachment;

		dst.rt->AddUsage( dst.usage );
		
		if ( is_ds and attName.empty() )
			dst.name = "DepthStencil";
		else
			CHECK_THROW_MSG( not attName.empty() );

		if ( rt->IsMutableDimension() )
			_SetDynamicDimension( rt->DimensionRC() );
	}

/*
=================================================
	_MoveTo
=================================================
*/
	void  ScriptBaseRenderPass::_MoveTo (OUT ScriptBaseRenderPass &dst) __NE___
	{
		#ifdef AE_COMPILER_MSVC
		# if _ITERATOR_DEBUG_LEVEL == 0
			StaticAssert64( sizeof(ScriptBaseRenderPass) == 800 );
		# endif
		#endif

		ScriptBasePass::_MoveTo( OUT dst );

		dst._output		= RVRef( this->_output );		this->_output.clear();
		dst._depthRange	= this->_depthRange;			this->_depthRange = {0.f, 1.f};
		dst._viewports	= RVRef( this->_viewports );	this->_viewports.clear();
		dst._wScaling	= RVRef( this->_wScaling );		this->_wScaling.clear();
		dst._scissors	= RVRef( this->_scissors );		this->_scissors.clear();
		dst._subsampled	= this->_subsampled;			this->_subsampled = false;
	}


} // AE::ResEditor
