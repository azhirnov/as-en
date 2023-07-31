// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptController.h"
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
                packed_int4     intRange    [2] = {};
                packed_float4   floatRange  [2];
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

        Constants_t             _constants;

        String                  _dbgName;
        RGBA8u                  _dbgColor   = HtmlColor::Red;

        ScriptDynamicDimPtr     _dynamicDim;

        ScriptBaseControllerPtr _controller;

    private:
        UniqueSliderNames_t     _uniqueSliderNames;             


    // interface
    public:
        ND_ virtual RC<IPass>   ToPass ()                                                       C_Th___ = 0;


    // methods
    public:
        void  SetDebugLabel1 (const String &name)                                               __Th___;
        void  SetDebugLabel2 (const String &name, const RGBA8u &color)                          __Th___;

        void  SliderI0 (const String &name)                                                     __Th___;
        void  SliderI1 (const String &name, int min, int max)                                   __Th___;
        void  SliderI2 (const String &name, const packed_int2 &min, const packed_int2 &max)     __Th___;
        void  SliderI3 (const String &name, const packed_int3 &min, const packed_int3 &max)     __Th___;
        void  SliderI4 (const String &name, const packed_int4 &min, const packed_int4 &max)     __Th___;

        void  SliderF0 (const String &name)                                                     __Th___;
        void  SliderF1 (const String &name, float min, float max)                               __Th___;
        void  SliderF2 (const String &name, const packed_float2 &min, const packed_float2 &max) __Th___;
        void  SliderF3 (const String &name, const packed_float3 &min, const packed_float3 &max) __Th___;
        void  SliderF4 (const String &name, const packed_float4 &min, const packed_float4 &max) __Th___;

        void  ColorSelector (const String &name)                                                __Th___;

        void  ConstantF4 (const String &name, const ScriptDynamicFloat4Ptr &value)              __Th___;

        void  ConstantI4 (const String &name, const ScriptDynamicInt4Ptr &value)                __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                           __Th___;

    private:
        template <typename T>
        void  _Slider (const String &name, const T &min, const T &max, ESlider type)            __Th___;

    protected:
        explicit ScriptBasePass (EFlags flags)                                                  __Th___;

        void  _AddSlidersToUIInteraction (IPass* pass)                                          const;
        void  _CopyConstants (OUT IPass::Constants &)                                           const;

        ND_ ScriptDynamicDim*   _Dimension ()                                                   __Th___ { return ScriptDynamicDimPtr{_dynamicDim}.Detach(); }
        void  _SetDynamicDimension (const ScriptDynamicDimPtr &)                                __Th___;
        void  _SetConstDimension (const uint3 &dim)                                             __Th___;

        template <typename B>
        static void  _BindBase (B &binder)                                                      __Th___;
    };

    AE_BIT_OPERATORS( ScriptBasePass::EFlags );


} // AE::ResEditor
