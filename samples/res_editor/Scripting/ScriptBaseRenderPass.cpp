// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
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
        auto&   dst = _output.emplace_back();
        uint    idx = 0;

        if ( args.IsArg< String const& >(idx) ) {
            dst.name = args.Arg< String const& >(idx++);
        }else{
            dst.name = "Color"s << ToString(_output.size()-1);
        }

        if ( args.IsArg< ScriptImagePtr const& >(idx) )
        {
            dst.rt = args.Arg< ScriptImagePtr const& >(idx++);
            CHECK_THROW_MSG( dst.rt );
            dst.rt->AddUsage( dst.rt->IsDepthStencil() ? EResourceUsage::DepthStencil : EResourceUsage::ColorAttachment );

            if ( dst.rt->IsDepthStencil() )
                dst.name = "DepthStencil";
        }
        else
            CHECK_THROW_MSG( false, "image is not defined" );

        if ( args.IsArg< ImageLayer const& >(idx) )
        {
            dst.layer = args.Arg< ImageLayer const& >(idx++);
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
            CHECK_THROW_MSG( dst.rt->IsDepthStencil() );
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
        auto&   dst = _output.emplace_back();
        uint    idx = 0;

        FixedArray< EBlendFactor, 4 >   b_factors;  // src, dst | srcRGB, dstRGB, srcA, dstA
        FixedArray< EBlendOp, 2 >       b_ops;      // op       | opRGB, opA

        if ( args.IsArg< String const& >(idx) ) {
            dst.name = args.Arg< String const& >(idx++);
        }else{
            dst.name = "Color"s << ToString(_output.size()-1);
        }

        if ( args.IsArg< ScriptImagePtr const& >(idx) )
        {
            dst.rt = args.Arg< ScriptImagePtr const& >(idx++);
            CHECK_THROW_MSG( dst.rt );
            CHECK_THROW_MSG( dst.rt->IsColor() );
            dst.rt->AddUsage( EResourceUsage::ColorAttachment );
        }
        else
            CHECK_THROW_MSG( false, "image is not defined" );

        if ( args.IsArg< ImageLayer const& >(idx) )
        {
            dst.layer = args.Arg< ImageLayer const& >(idx++);
        }
        if ( args.IsArg< MipmapLevel const& >(idx) )
        {
            dst.mipmap = args.Arg< MipmapLevel const& >(idx++);
        }

        for (; idx < args.ArgCount();)
        {
            if ( args.IsArg< EBlendFactor >(idx) )
            {
                CHECK_THROW_MSG( b_factors.try_push_back( args.Arg<EBlendFactor>(idx++) ));
            }
            else
            if ( args.IsArg< EBlendOp >(idx) )
            {
                CHECK_THROW_MSG( b_ops.try_push_back( args.Arg<EBlendOp>(idx++) ));
            }
            else
                CHECK_THROW_MSG( false, "unsupported arg type" );
        }
        ASSERT( idx == args.ArgCount() );

        CHECK_THROW_MSG( (b_factors.size() == 2 and b_ops.size() == 1) or
                         (b_factors.size() == 4 and b_ops.size() == 2),
            "unsupported fn signature" );

        if ( b_factors.size() == 2 and b_ops.size() == 1 )
        {
            dst.srcFactorRGB    = dst.srcFactorA    = b_factors[0];
            dst.dstFactorRGB    = dst.dstFactorA    = b_factors[1];
            dst.blendOpRGB      = dst.blendOpA      = b_ops[0];
        }

        if ( b_factors.size() == 4 and b_ops.size() == 2 )
        {
            dst.srcFactorRGB    = b_factors[0];
            dst.srcFactorA      = b_factors[1];
            dst.dstFactorRGB    = b_factors[2];
            dst.dstFactorA      = b_factors[3];
            dst.blendOpRGB      = b_ops[0];
            dst.blendOpA        = b_ops[1];
        }
    }


} // AE::ResEditor
