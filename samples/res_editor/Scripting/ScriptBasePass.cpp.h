// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptCommon.h"

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

                binder.Comment( "ShaderTrace - record all variables, function result, etc and save it to file.\n"
                                "It is very useful to debug shaders. In UI select 'Debugging' menu, select pass,"
                                "'Trace' and shader stage then click 'G' key to record trace for pixel under cursor.\n"
                                "Reference to the last recorded trace will be added to console and IDE log, click on it to open file." );
                binder.AddValue( "Enable_ShaderTrace",  EFlags::Enable_ShaderTrace );

                binder.Comment( "ShaderFunctionProfiling - record time of user function calls, sort it and save to file.\n" );
                binder.AddValue( "Enable_ShaderFnProf", EFlags::Enable_ShaderFnProf );

                //binder.AddValue( "Enable_ShaderTmProf",   EFlags::Enable_ShaderTmProf );  // not supported yet

                binder.Comment( "Enable alll debug features." );
                binder.AddValue( "Enable_AllShaderDbg", EFlags::Enable_AllShaderDbg );
                STATIC_ASSERT( uint(EFlags::All) == 7 );
            }
        }

        using T = typename B::Class_t;
        classBinder.Operators().ImplCast( &ScriptBasePass_ToBase<T> );

        classBinder.Comment( "Set debug label and color. It is used in graphics profiler." );
        classBinder.AddMethod( &ScriptBasePass::SetDebugLabel1,     "SetDebugLabel" );
        classBinder.AddMethod( &ScriptBasePass::SetDebugLabel2,     "SetDebugLabel" );

        classBinder.Comment( "Add slider to UI. Data passed to all shaders in the current pass." );
        classBinder.AddMethod( &ScriptBasePass::SliderI0,           "SliderI"       );
        classBinder.AddMethod( &ScriptBasePass::SliderI1,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI2,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI3,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI4,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI1a,          "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI2a,          "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI3a,          "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderI4a,          "Slider"        );

        classBinder.AddMethod( &ScriptBasePass::SliderF0,           "SliderF"       );
        classBinder.AddMethod( &ScriptBasePass::SliderF1,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF2,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF3,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF4,           "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF1a,          "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF2a,          "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF3a,          "Slider"        );
        classBinder.AddMethod( &ScriptBasePass::SliderF4a,          "Slider"        );

        classBinder.AddMethod( &ScriptBasePass::ColorSelector1,     "ColorSelector" );
        classBinder.AddMethod( &ScriptBasePass::ColorSelector2,     "ColorSelector" );
        classBinder.AddMethod( &ScriptBasePass::ColorSelector3,     "ColorSelector" );

        classBinder.Comment( "TODO" );
        classBinder.AddMethod( &ScriptBasePass::ConstantF4,         "Constant"      );
        classBinder.AddMethod( &ScriptBasePass::ConstantI4,         "Constant"      );

        classBinder.Comment( "Returns dynamic dimension of the pass.\n"
                             "It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches." );
        classBinder.AddMethod( &ScriptBasePass::_Dimension,         "Dimension"     );

        classBinder.Comment( "Add resource to all shaders in the current pass.\n"
                             "In - resource is used for read access.\n"
                             "Out - resource is used for write access.\n" );
        classBinder.AddMethod( &ScriptBasePass::ArgSceneIn,         "ArgIn"         );

        classBinder.AddMethod( &ScriptBasePass::ArgBufferIn,        "ArgIn"         );
        classBinder.AddMethod( &ScriptBasePass::ArgBufferOut,       "ArgOut"        );
        classBinder.AddMethod( &ScriptBasePass::ArgBufferInOut,     "ArgInOut"      );

        classBinder.AddMethod( &ScriptBasePass::ArgImageIn,         "ArgIn"         );
        classBinder.AddMethod( &ScriptBasePass::ArgImageOut,        "ArgOut"        );
        classBinder.AddMethod( &ScriptBasePass::ArgImageInOut,      "ArgInOut"      );

        classBinder.AddMethod( &ScriptBasePass::ArgTextureIn,       "ArgIn"         );
        classBinder.AddMethod( &ScriptBasePass::ArgVideoIn,         "ArgIn"         );
        classBinder.AddMethod( &ScriptBasePass::ArgController,      "ArgIn"         );
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
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
        }

        // with name
        {
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output" );

            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output" );
        }

        // with blend
        if ( withBlending )
        {
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );
            classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend" );

        }
    }


} // AE::ResEditor
