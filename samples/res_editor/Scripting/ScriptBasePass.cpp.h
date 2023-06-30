// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::ResEditor
{
namespace
{
/*
=================================================
    ScriptBasePass_ToBase
=================================================
*/
    template <typename T>
    static ScriptBasePass*  ScriptBasePass_ToBase (T* ptr)
    {
        STATIC_ASSERT( IsBaseOf< ScriptBasePass, T >);

        ScriptBasePassPtr  result{ ptr };
        return result.Detach();
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    _BindBase
=================================================
*/
    template <typename B>
    void  ScriptBasePass::_BindBase (B &classBinder) __Th___
    {
        // enums
        {
            Scripting::EnumBinder<EFlags>   binder{ classBinder.GetEngine() };
            if ( not binder.IsRegistred() )
            {
                binder.Create();
                binder.AddValue( "None",                EFlags::Unknown );
                binder.AddValue( "Enable_ShaderTrace",  EFlags::Enable_ShaderTrace );
                binder.AddValue( "Enable_ShaderFnProf", EFlags::Enable_ShaderFnProf );
                binder.AddValue( "Enable_ShaderTmProf", EFlags::Enable_ShaderTmProf );
                binder.AddValue( "Enable_AllShaderDbg", EFlags::Enable_AllShaderDbg );
                STATIC_ASSERT( uint(EFlags::All) == 7 );
            }
        }

        using T = typename B::Class_t;
        classBinder.Operators().ImplCast( &ScriptBasePass_ToBase<T> );

        classBinder.AddMethod( &ScriptBasePass::SetDebugLabel1,     "SetDebugLabel" );
        classBinder.AddMethod( &ScriptBasePass::SetDebugLabel2,     "SetDebugLabel" );

        classBinder.AddMethod( &ScriptBasePass::SliderI0,           "SliderI"       );
        classBinder.AddMethod( &ScriptBasePass::SliderI1,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI2,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI3,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI4,           "Slider"        );

        classBinder.AddMethod( &ScriptBasePass::SliderF0,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF1,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF2,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF3,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF4,           "Slider"        );

        classBinder.AddMethod( &ScriptBasePass::ColorSelector,      "ColorSelector" );

        classBinder.AddMethod( &ScriptBasePass::ConstantF4,         "Constant"      );
        classBinder.AddMethod( &ScriptBasePass::ConstantI4,         "Constant"      );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _BindBaseRenderPass
=================================================
*/
    template <typename B>
    void  ScriptBaseRenderPass::_BindBaseRenderPass (B &classBinder, Bool withBlending) __Th___
    {
        using C = typename B::Class_t;

        ScriptBasePass::_BindBase( classBinder );

        // without name
        {
            classBinder.AddGenericMethod< void (const ScriptImagePtr &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
        }

        // with name
        {
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
        }

        // with blend
        if ( withBlending )
        {
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

            classBinder.AddGenericMethod< void (const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

        }
    }


} // AE::ResEditor
