// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptController.h"
#include "res_editor/Scripting/ScriptPassArgs.h"
#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

    //
    // Base Pass
    //

    class ScriptBasePass : public EnableScriptRC
    {
    // types
    public:
        struct CppStructsFromShaders
        {
            HashSet< String >   uniqueTypes;
            String              cpp;
        };

        enum class ESlider : ubyte
        {
            Int,
            Float,
            Color,
            _Count
        };

        enum class EFlags : ubyte
        {
            Unknown                 = 0,

            Enable_ShaderTrace      = 1 << 0,
            Enable_ShaderFnProf     = 1 << 1,
            Enable_ShaderTmProf     = 1 << 2,

            _Last,
            All                     = ((_Last - 1) << 1) - 1,

            Enable_AllShaderDbg     = Enable_ShaderTrace | Enable_ShaderFnProf | Enable_ShaderTmProf,
        };

        struct Slider
        {
            String              name;
            uint                index       = UMax;
            ESlider             type        = ESlider::_Count;
            ubyte               count       = 0;
            union {
                packed_int4     intRange    [3] = {};   // min, max, initial
                packed_float4   floatRange  [3];        // min, max, initial
            };

            Slider ();
            Slider (const Slider &);
            Slider (Slider &&);
        };


    protected:
        using Sliders_t             = Array< Slider >;
        using UniqueSliderNames_t   = FlatHashSet< String >;
        using SliderCounter_t       = StaticArray< uint, uint(ESlider::_Count) >;

        using Controllers_t         = Array< RC<IController> >;
        using ControllersView_t     = ArrayView< RC<IController> >;

        struct Constant
        {
            String              name;
            uint                index       = UMax;
            ESlider             type        = ESlider::_Count;
            union {
                ScriptDynamicFloat4Ptr  f4;
                ScriptDynamicInt4Ptr    i4;
            };

            Constant ();
            Constant (const Constant &);
            Constant (Constant &&);
            ~Constant ();
        };
        using Constants_t       = Array< Constant >;


    // variables
    protected:
        const EFlags            _baseFlags;

        Sliders_t               _sliders;
        SliderCounter_t         _sliderCounter      {};

        String                  _defines;

        Constants_t             _constants;

        String                  _dbgName;
        RGBA8u                  _dbgColor   = HtmlColor::Red;

        ScriptDynamicDimPtr     _dynamicDim;

        ScriptBaseControllerPtr _controller;
        ScriptPassArgs          _args;

    private:
        UniqueSliderNames_t     _uniqueSliderNames;


    // interface
    public:
        ND_ virtual RC<IPass>   ToPass ()                                                                                   C_Th___ = 0;


    // methods
    public:
        void  SetDebugLabel1 (const String &name)                                                                           __Th___;
        void  SetDebugLabel2 (const String &name, const RGBA8u &color)                                                      __Th___;

        void  SliderI0 (const String &name)                                                                                 __Th___;
        void  SliderI1 (const String &name, int min, int max)                                                               __Th___;
        void  SliderI2 (const String &name, const packed_int2 &min, const packed_int2 &max)                                 __Th___;
        void  SliderI3 (const String &name, const packed_int3 &min, const packed_int3 &max)                                 __Th___;
        void  SliderI4 (const String &name, const packed_int4 &min, const packed_int4 &max)                                 __Th___;

        void  SliderI1a (const String &name, int min, int max, int val)                                                     __Th___;
        void  SliderI2a (const String &name, const packed_int2 &min, const packed_int2 &max, const packed_int2 &val)        __Th___;
        void  SliderI3a (const String &name, const packed_int3 &min, const packed_int3 &max, const packed_int3 &val)        __Th___;
        void  SliderI4a (const String &name, const packed_int4 &min, const packed_int4 &max, const packed_int4 &val)        __Th___;

        void  SliderF0 (const String &name)                                                                                 __Th___;
        void  SliderF1 (const String &name, float min, float max)                                                           __Th___;
        void  SliderF2 (const String &name, const packed_float2 &min, const packed_float2 &max)                             __Th___;
        void  SliderF3 (const String &name, const packed_float3 &min, const packed_float3 &max)                             __Th___;
        void  SliderF4 (const String &name, const packed_float4 &min, const packed_float4 &max)                             __Th___;

        void  SliderF1a (const String &name, float min, float max, float val)                                               __Th___;
        void  SliderF2a (const String &name, const packed_float2 &min, const packed_float2 &max, const packed_float2 &val)  __Th___;
        void  SliderF3a (const String &name, const packed_float3 &min, const packed_float3 &max, const packed_float3 &val)  __Th___;
        void  SliderF4a (const String &name, const packed_float4 &min, const packed_float4 &max, const packed_float4 &val)  __Th___;

        void  ColorSelector1 (const String &name)                                                                           __Th___;
        void  ColorSelector2 (const String &name, const RGBA32f &val)                                                       __Th___;
        void  ColorSelector3 (const String &name, const RGBA8u &val)                                                        __Th___;

        void  ConstantF4 (const String &name, const ScriptDynamicFloat4Ptr &value)                      __Th___;

        void  ConstantI4 (const String &name, const ScriptDynamicInt4Ptr &value)                        __Th___;

        void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)                            __Th___ { _args.ArgSceneIn( name, scene ); }

        void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)                              __Th___ { _args.ArgBufferIn( name, buf ); }
        void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)                             __Th___ { _args.ArgBufferOut( name, buf ); }
        void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)                           __Th___ { _args.ArgBufferInOut( name, buf ); }

        void  ArgImageIn (const String &name, const ScriptImagePtr &img)                                __Th___ { _args.ArgImageIn( name, img ); }
        void  ArgImageOut (const String &name, const ScriptImagePtr &img)                               __Th___ { _args.ArgImageOut( name, img ); }
        void  ArgImageInOut (const String &name, const ScriptImagePtr &img)                             __Th___ { _args.ArgImageInOut( name, img ); }

        void  ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName)   __Th___ { _args.ArgTextureIn( name, tex, samplerName ); }
        void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)__Th___ { _args.ArgVideoIn( name, tex, samplerName ); }
        void  ArgController (const ScriptBaseControllerPtr &)                                           __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;


    private:
        template <typename T>
        void  _Slider (const String &name, const T &min, const T &max, const T &val, ESlider type)      __Th___;


    protected:
        explicit ScriptBasePass (EFlags flags)                                                          __Th___;

        void  _AddSlidersToUIInteraction (IPass* pass)                                                  const;
        void  _CopyConstants (OUT IPass::Constants &)                                                   const;

        ND_ ScriptDynamicDim*   _Dimension ()                                                           __Th___ { return ScriptDynamicDimPtr{_dynamicDim}.Detach(); }
        void  _SetDynamicDimension (const ScriptDynamicDimPtr &)                                        __Th___;
        void  _SetConstDimension (const uint3 &dim)                                                     __Th___;

        template <typename B>
        static void  _BindBase (B &binder)                                                              __Th___;

        virtual void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)                                   C_Th___ = 0;

        static void  _AddDefines (StringView defines, INOUT String &header)                             __Th___;

        void  _AddSliders (INOUT String &header)                                                        C_Th___;
    };

    AE_BIT_OPERATORS( ScriptBasePass::EFlags );


} // AE::ResEditor
