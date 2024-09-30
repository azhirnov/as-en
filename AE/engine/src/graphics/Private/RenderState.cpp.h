// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/RenderState.h"

namespace AE::Graphics
{

/*
=================================================
	ColorBuffer::operator ==
=================================================
*/
	bool  RenderState::ColorBuffer::operator == (const ColorBuffer &rhs) C_NE___
	{
		return	(blend == rhs.blend)	and
				(blend ?
					((srcBlendFactor	== rhs.srcBlendFactor)	and
					 (dstBlendFactor	== rhs.dstBlendFactor)	and
					 (blendOp			== rhs.blendOp)			and
					 ((BitCast<ubyte>(colorMask) & 0xF) == (BitCast<ubyte>(rhs.colorMask) & 0xF)) )
				 : true);
	}

/*
=================================================
	ColorBuffer::CalcHash
=================================================
*/
	HashVal  RenderState::ColorBuffer::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( blend );
		result << HashOf( srcBlendFactor.color );
		result << HashOf( srcBlendFactor.alpha );
		result << HashOf( dstBlendFactor.color );
		result << HashOf( dstBlendFactor.alpha );
		result << HashOf( blendOp.color );
		result << HashOf( blendOp.alpha );
		result << HashOf( BitCast<ubyte>(colorMask) & 0xF );
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	ColorBuffersState::operator ==
=================================================
*/
	bool  RenderState::ColorBuffersState::operator == (const ColorBuffersState &rhs) C_NE___
	{
		return	(buffers	== rhs.buffers)		and
				(logicOp	== rhs.logicOp)		and
				(blendColor	== rhs.blendColor);
	}

/*
=================================================
	ColorBuffersState::CalcHash
=================================================
*/
	HashVal  RenderState::ColorBuffersState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( blendColor );
		result << HashOf( logicOp );
		result << HashOf( buffers.size() );

		for (auto& cb : buffers) {
			result << cb.CalcHash();
		}
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	StencilFaceState::operator ==
=================================================
*/
	bool  RenderState::StencilFaceState::operator == (const StencilFaceState &rhs) C_NE___
	{
		return	(failOp			== rhs.failOp)		and
				(depthFailOp	== rhs.depthFailOp)	and
				(passOp			== rhs.passOp)		and
				(compareOp		== rhs.compareOp)	and
				(reference		== rhs.reference)	and
				(writeMask		== rhs.writeMask)	and
				(compareMask	== rhs.compareMask);
	}

/*
=================================================
	StencilFaceState::CalcHash
=================================================
*/
	HashVal  RenderState::StencilFaceState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( failOp );
		result << HashOf( depthFailOp );
		result << HashOf( passOp );
		result << HashOf( compareOp );
		result << HashOf( reference );
		result << HashOf( writeMask );
		result << HashOf( compareMask );
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	StencilBufferState::operator ==
=================================================
*/
	bool  RenderState::StencilBufferState::operator == (const StencilBufferState &rhs) C_NE___
	{
		return	(enabled	== rhs.enabled)	and
				(enabled ?
					((front	== rhs.front)	and
					 (back	== rhs.back))	: true);
	}

/*
=================================================
	StencilBufferState::CalcHash
=================================================
*/
	HashVal  RenderState::StencilBufferState::CalcHash () C_NE___
	{
		HashVal	result;
		if ( enabled )
		{
			result << front.CalcHash();
			result << back.CalcHash();
		}
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DepthBufferState::operator ==
=================================================
*/
	bool  RenderState::DepthBufferState::operator == (const DepthBufferState &rhs) C_NE___
	{
		return	(compareOp		== rhs.compareOp)			and
				(bounds			== rhs.bounds)				and
				(bounds ?
					((minBounds == rhs.minBounds) and
					 (maxBounds == rhs.maxBounds)) : true)	and
				(write			== rhs.write)				and
				(test			== rhs.test);
	}

/*
=================================================
	DepthBufferState::CalcHash
=================================================
*/
	HashVal  RenderState::DepthBufferState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( minBounds );
		result << HashOf( maxBounds );
		result << HashOf( compareOp );
		result << HashOf( bounds );
		result << HashOf( test );
		result << HashOf( write );
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	InputAssemblyState::operator ==
=================================================
*/
	bool  RenderState::InputAssemblyState::operator == (const InputAssemblyState &rhs) C_NE___
	{
		return	(topology			== rhs.topology)		and
				(primitiveRestart	== rhs.primitiveRestart);
	}

/*
=================================================
	InputAssemblyState::CalcHash
=================================================
*/
	HashVal  RenderState::InputAssemblyState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( topology );
		result << HashOf( primitiveRestart );
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	RasterizationState::operator ==
=================================================
*/
	bool  RenderState::RasterizationState::operator == (const RasterizationState &rhs) C_NE___
	{
		return	(polygonMode				==	rhs.polygonMode)			and
				Equal(	depthBiasConstFactor,	rhs.depthBiasConstFactor )	and
				Equal(	depthBiasClamp,			rhs.depthBiasClamp )		and
				Equal(	depthBiasSlopeFactor,	rhs.depthBiasSlopeFactor )	and
				(depthBias					==	rhs.depthBias)				and
				(depthClamp					==	rhs.depthClamp)				and
				(rasterizerDiscard			==	rhs.rasterizerDiscard)		and
				(cullMode					==	rhs.cullMode)				and
				(frontFaceCCW				==	rhs.frontFaceCCW);
	}

/*
=================================================
	RasterizationState::CalcHash
=================================================
*/
	HashVal  RenderState::RasterizationState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( polygonMode );
		result << HashOf( depthBiasConstFactor );
		result << HashOf( depthBiasClamp );
		result << HashOf( depthBiasSlopeFactor );
		result << HashOf( depthBias );
		result << HashOf( depthClamp );
		result << HashOf( rasterizerDiscard );
		result << HashOf( cullMode );
		result << HashOf( frontFaceCCW );
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	MultisampleState::operator ==
=================================================
*/
	bool  RenderState::MultisampleState::operator == (const MultisampleState &rhs) C_NE___
	{
		return	(sampleMask				==	rhs.sampleMask)			and
				(samples				==	rhs.samples)			and
				Equal(	minSampleShading,	rhs.minSampleShading )	and
				(sampleShading			==	rhs.sampleShading)		and
				(alphaToCoverage		==	rhs.alphaToCoverage)	and
				(alphaToOne				==	rhs.alphaToOne);
	}

/*
=================================================
	MultisampleState::CalcHash
=================================================
*/
	HashVal  RenderState::MultisampleState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( sampleMask );
		result << HashOf( samples );
		result << HashOf( minSampleShading );
		result << HashOf( sampleShading );
		result << HashOf( alphaToCoverage );
		result << HashOf( alphaToOne );
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	operator ==
=================================================
*/
	bool  RenderState::operator == (const RenderState &rhs) C_NE___
	{
		return	(color			== rhs.color)			and
				(depth			== rhs.depth)			and
				(stencil		== rhs.stencil)			and
				(inputAssembly	== rhs.inputAssembly)	and
				(rasterization	== rhs.rasterization)	and
				(multisample	== rhs.multisample);
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  RenderState::CalcHash () C_NE___
	{
		HashVal	result;
		result << color.CalcHash();
		result << depth.CalcHash();
		result << stencil.CalcHash();
		result << inputAssembly.CalcHash();
		result << rasterization.CalcHash();
		result << multisample.CalcHash();
		return result;
	}

/*
=================================================
	Set
=================================================
*/
	void  RenderState::Set (const MDepthStencilState &ds, const MDynamicRenderState &rs) __NE___
	{
		this->depth		= ds.depth;
		this->stencil	= ds.stencil;

		this->inputAssembly.topology				= rs.topology;
		this->rasterization.polygonMode				= rs.polygonMode;
		this->rasterization.frontFaceCCW			= rs.frontFaceCCW;
		this->rasterization.cullMode				= rs.cullMode;
		this->rasterization.depthClamp				= rs.depthClamp;
		this->rasterization.depthBiasConstFactor	= rs.depthBiasConstFactor;
		this->rasterization.depthBiasClamp			= rs.depthBiasClamp;
		this->rasterization.depthBiasSlopeFactor	= rs.depthBiasSlopeFactor;
	}

/*
=================================================
	SetDefault
=================================================
*/
	void  RenderState::SetDefault (EPipelineDynamicState dstate) __NE___
	{
		for (auto t : BitfieldIterate( dstate ))
		{
			switch_enum( t )
			{
				case EPipelineDynamicState::StencilCompareMask :
					stencil.front.compareMask	= 0xFF;
					stencil.back.compareMask	= 0xFF;
					break;

				case EPipelineDynamicState::StencilWriteMask :
					stencil.front.writeMask		= 0xFF;
					stencil.back.writeMask		= 0xFF;
					break;

				case EPipelineDynamicState::StencilReference :
					stencil.front.reference		= 0xFF;
					stencil.back.reference		= 0xFF;
					break;

				case EPipelineDynamicState::DepthBias :
					rasterization.depthBiasConstFactor	= 0.f;
					rasterization.depthBiasClamp		= 0.f;
					rasterization.depthBiasSlopeFactor	= 0.f;
					break;

				case EPipelineDynamicState::BlendConstants :
					color.blendColor = RGBA32f{};
					break;

				//case EPipelineDynamicState::DepthBounds :
				//	depth.minBounds	= 0.f;
				//	depth.maxBounds	= 1.f;
				//	break;

				case EPipelineDynamicState::RTStackSize :
				case EPipelineDynamicState::FragmentShadingRate :
				case EPipelineDynamicState::ViewportWScaling :
					break;	// ignore

				case EPipelineDynamicState::Unknown :
				case EPipelineDynamicState::GraphicsPipelineMask :
				case EPipelineDynamicState::_Last :
				case EPipelineDynamicState::All :
				default_unlikely :
					DBG_WARNING( "unknown dynamic state" );
					break;
			}
			switch_end
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  MDepthStencilState::operator == (const MDepthStencilState &rhs) C_NE___
	{
		return	(depth		== rhs.depth)	and
				(stencil	== rhs.stencil);
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  MDepthStencilState::CalcHash () C_NE___
	{
		HashVal	result;
		result << depth.CalcHash();
		result << stencil.CalcHash();
		return result;
	}

/*
=================================================
	Set
=================================================
*/
	void  MDepthStencilState::Set (const RenderState &rs) __NE___
	{
		depth	= rs.depth;
		stencil	= rs.stencil;

		if ( not stencil.enabled )
			stencil = {};
		else
		{
			stencil.front.reference	= 0;	// dynamic state
			stencil.back.reference	= 0;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  MDynamicRenderState::operator == (const MDynamicRenderState &rhs) C_NE___
	{
		return	(topology				== rhs.topology)				and
				(polygonMode			== rhs.polygonMode)				and
				(frontFaceCCW			== rhs.frontFaceCCW)			and
				(cullMode				== rhs.cullMode)				and
				(depthClamp				== rhs.depthClamp)				and
				(depthBiasConstFactor	== rhs.depthBiasConstFactor)	and
				(depthBiasClamp			== rhs.depthBiasClamp)			and
				(depthBiasSlopeFactor	== rhs.depthBiasSlopeFactor);
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  MDynamicRenderState::CalcHash () C_NE___
	{
		HashVal	result;
		result << HashOf( topology );
		result << HashOf( polygonMode );
		result << HashOf( frontFaceCCW );
		result << HashOf( cullMode );
		result << HashOf( depthClamp );
		result << HashOf( depthBiasConstFactor );
		result << HashOf( depthBiasClamp );
		result << HashOf( depthBiasSlopeFactor );
		return result;
	}

/*
=================================================
	Set
=================================================
*/
	void  MDynamicRenderState::Set (const RenderState &rs) __NE___
	{
		topology				= rs.inputAssembly.topology;
		polygonMode				= rs.rasterization.polygonMode;
		frontFaceCCW			= rs.rasterization.frontFaceCCW;
		cullMode				= rs.rasterization.cullMode;
		depthClamp				= rs.rasterization.depthClamp;
		depthBiasConstFactor	= rs.rasterization.depthBiasConstFactor;
		depthBiasClamp			= rs.rasterization.depthBiasClamp;
		depthBiasSlopeFactor	= rs.rasterization.depthBiasSlopeFactor;
	}


} // AE::Graphics
