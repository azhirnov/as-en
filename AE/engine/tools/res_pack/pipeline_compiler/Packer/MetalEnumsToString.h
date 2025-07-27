#pragma once

#if defined(AE_BUILD_PIPELINE_COMPILER) or defined(AE_TEST_PIPELINE_COMPILER)
# include "res_pack/pipeline_compiler/Packer/MetalEnums.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{
	using AE::PipelineCompiler::MtlPixelFormat;
	using AE::PipelineCompiler::MtlLoadAction;
	using AE::PipelineCompiler::MtlStoreAction;
	using AE::PipelineCompiler::MtlStoreActionOptions;
	using AE::PipelineCompiler::MtlMultisampleDepthResolveFilter;
	using AE::PipelineCompiler::MtlMultisampleStencilResolveFilter;
	using AE::PipelineCompiler::MtlAttachmentFlags;

/*
=================================================
	ToString (MtlPixelFormat)
=================================================
*/
	ND_ inline StringView  ToString (MtlPixelFormat value)
	{
#		define FMT_BUILDER( _engineFmt_, _mtlFormat_, ... ) \
			case MtlPixelFormat::_mtlFormat_ : return AE_TOSTRING( MTLPixelFormat ## _mtlFormat_ );

		switch ( value )
		{
			AE_PRIVATE_MTLPIXELFORMATS( FMT_BUILDER )
			case MtlPixelFormat::SwapchainColor :	return "SwapchainColor";
		}

#		undef FMT_BUILDER

		RETURN_ERR( "invalid pixel format" );
	}

/*
=================================================
	ToString (MtlLoadAction)
=================================================
*/
	ND_ inline StringView  ToString (MtlLoadAction value)
	{
		switch_enum( value )
		{
			case MtlLoadAction::DontCare :	return "MTLLoadActionDontCare";
			case MtlLoadAction::Load :		return "MTLLoadActionLoad";
			case MtlLoadAction::Clear :		return "MTLLoadActionClear";
		}
		switch_end
		RETURN_ERR( "invalid load actions" );
	}

/*
=================================================
	ToString (MtlStoreAction)
=================================================
*/
	ND_ inline StringView  ToString (MtlStoreAction value)
	{
		switch_enum( value )
		{
			case MtlStoreAction::DontCare :						return "MTLStoreActionDontCare";
			case MtlStoreAction::Store :						return "MTLStoreActionStore";
			case MtlStoreAction::MultisampleResolve :			return "MTLStoreActionMultisampleResolve";
			case MtlStoreAction::StoreAndMultisampleResolve :	return "MTLStoreActionStoreAndMultisampleResolve";
			case MtlStoreAction::Unknown :						return "MTLStoreActionUnknown";
			case MtlStoreAction::CustomSampleDepthStore :		return "MTLStoreActionCustomSampleDepthStore";
		}
		switch_end
		RETURN_ERR( "invalid store actions" );
	}

/*
=================================================
	ToString (MtlStoreActionOptions)
=================================================
*/
	ND_ inline StringView  ToString (MtlStoreActionOptions value)
	{
		switch_enum( value )
		{
			case MtlStoreActionOptions::None :					return "MTLStoreActionOptionsNone";
			case MtlStoreActionOptions::CustomSamplePositions :	return "MTLStoreActionOptionsCustomSamplePositions";
		}
		switch_end
		RETURN_ERR( "invalid store actions options" );
	}

/*
=================================================
	ToString (MtlMultisampleDepthResolveFilter)
=================================================
*/
	ND_ inline StringView  ToString (MtlMultisampleDepthResolveFilter value)
	{
		switch_enum( value )
		{
			case MtlMultisampleDepthResolveFilter::Sample0 :	return "MTLMultisampleDepthResolveFilterSample0";
			case MtlMultisampleDepthResolveFilter::Min :		return "MTLMultisampleDepthResolveFilterMin";
			case MtlMultisampleDepthResolveFilter::Max :		return "MTLMultisampleDepthResolveFilterMax";
		}
		switch_end
		RETURN_ERR( "invalid multisample depth resolve filter" );
	}

/*
=================================================
	ToString (MtlMultisampleStencilResolveFilter)
=================================================
*/
	ND_ inline StringView  ToString (MtlMultisampleStencilResolveFilter value)
	{
		switch_enum( value )
		{
			case MtlMultisampleStencilResolveFilter::Sample0 :				return "MTLMultisampleStencilResolveFilterSample0";
			case MtlMultisampleStencilResolveFilter::DepthResolvedSample :	return "MTLMultisampleStencilResolveFilterDepthResolvedSample";
		}
		switch_end
		RETURN_ERR( "invalid multisample stencil resolve filter" );
	}

/*
=================================================
	ToString (MtlAttachmentFlags)
=================================================
*/
	ND_ inline String  ToString (MtlAttachmentFlags values)
	{
		String	str;
		for (auto t : BitfieldIterate( values ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case MtlAttachmentFlags::Color :	str << "Color";		break;
				case MtlAttachmentFlags::Depth :	str << "Depth";		break;
				case MtlAttachmentFlags::Stencil :	str << "Stencil";	break;
				case MtlAttachmentFlags::Unknown :
				case MtlAttachmentFlags::_BITOPS_ :
				default :							RETURN_ERR( "invalid attachment flags" );
			}
			switch_end
		}
		return str;
	}

} // AE::Base

#endif // AE_BUILD_PIPELINE_COMPILER or AE_TEST_PIPELINE_COMPILER
