// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/EditorUI.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    ScriptBasePass::Slider::Slider ()
    {}

    ScriptBasePass::Slider::Slider (const Slider &other) :
        name{ other.name },
        index{ other.index },
        type{ other.type },
        count{ other.count }
    {
        std::memcpy( OUT intRange, other.intRange, sizeof(intRange) );
    }

    ScriptBasePass::Slider::Slider (Slider && other) :
        name{ RVRef(other.name) },
        index{ other.index },
        type{ other.type },
        count{ other.count }
    {
        std::memcpy( OUT intRange, other.intRange, sizeof(intRange) );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    ScriptBasePass::Constant::Constant ()
    {}

    ScriptBasePass::Constant::Constant (const Constant &other) :
        name{ other.name },
        index{ other.index },
        type{ other.type }
    {
        switch ( type ) {
            case ESlider::Float :   this->f4 = other.f4;    break;
            case ESlider::Int :     this->i4 = other.i4;    break;
        }
    }

    ScriptBasePass::Constant::Constant (Constant && other) :
        name{ RVRef(other.name) },
        index{ other.index },
        type{ other.type }
    {
        switch ( type ) {
            case ESlider::Float :   this->f4 = RVRef(other.f4); break;
            case ESlider::Int :     this->i4 = RVRef(other.i4); break;
        }
    }

    ScriptBasePass::Constant::~Constant ()
    {
        switch ( type ) {
            case ESlider::Float :   f4 = null;  break;
            case ESlider::Int :     i4 = null;  break;
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptBasePass::ScriptBasePass (EFlags flags) __Th___ :
        _baseFlags{flags},
        _dynamicDim{ new ScriptDynamicDim{ MakeRC<DynamicDim>( uint2{1} )}}
    {}

/*
=================================================
    _Slider
=================================================
*/
    template <typename T>
    void  ScriptBasePass::_Slider (const String &name, const T &min, const T &max, ESlider type) __Th___
    {
        CHECK_THROW_MSG( _uniqueSliderNames.insert( name ).second,
            "Slider '"s << name << "' is already exists" );

        uint    idx = _sliderCounter[ uint(type) ]++;
        CHECK_THROW_MSG( idx < UIInteraction::MaxSlidersPerType,
            "Slider count "s << ToString(idx) << " must be less than " << ToString(UIInteraction::MaxSlidersPerType) );

        auto&   dst = _sliders.emplace_back();

        dst.name    = name;
        dst.index   = idx;
        dst.count   = sizeof(T) / sizeof(int);
        dst.type    = type;

        std::memcpy( OUT &dst.intRange[0], &min, sizeof(min) );
        std::memcpy( OUT &dst.intRange[1], &max, sizeof(max) );
    }

/*
=================================================
    SliderI
=================================================
*/
    void  ScriptBasePass::SliderI0 (const String &name) __Th___
    {
        int min = 0, max = 1024;
        return _Slider( name, min, max, ESlider::Int );
    }

    void  ScriptBasePass::SliderI1 (const String &name, int min, int max) __Th___
    {
        return _Slider( name, min, max, ESlider::Int );
    }

    void  ScriptBasePass::SliderI2 (const String &name, const packed_int2 &min, const packed_int2 &max) __Th___
    {
        return _Slider( name, min, max, ESlider::Int );
    }

    void  ScriptBasePass::SliderI3 (const String &name, const packed_int3 &min, const packed_int3 &max) __Th___
    {
        return _Slider( name, min, max, ESlider::Int );
    }

    void  ScriptBasePass::SliderI4 (const String &name, const packed_int4 &min, const packed_int4 &max) __Th___
    {
        return _Slider( name, min, max, ESlider::Int );
    }

/*
=================================================
    SliderF
=================================================
*/
    void  ScriptBasePass::SliderF0 (const String &name) __Th___
    {
        float min = 0.f, max = 1.f;
        return _Slider( name, min, max, ESlider::Float );
    }

    void  ScriptBasePass::SliderF1 (const String &name, float min, float max) __Th___
    {
        return _Slider( name, min, max, ESlider::Float );
    }

    void  ScriptBasePass::SliderF2 (const String &name, const packed_float2 &min, const packed_float2 &max) __Th___
    {
        return _Slider( name, min, max, ESlider::Float );
    }

    void  ScriptBasePass::SliderF3 (const String &name, const packed_float3 &min, const packed_float3 &max) __Th___
    {
        return _Slider( name, min, max, ESlider::Float );
    }

    void  ScriptBasePass::SliderF4 (const String &name, const packed_float4 &min, const packed_float4 &max) __Th___
    {
        return _Slider( name, min, max, ESlider::Float );
    }

/*
=================================================
    ColorSelector
=================================================
*/
    void  ScriptBasePass::ColorSelector (const String &name) __Th___
    {
        RGBA32f min{0.f}, max{1.f};
        return _Slider( name, min, max, ESlider::Color );
    }

/*
=================================================
    ConstantF*
=================================================
*/
    void  ScriptBasePass::ConstantF4 (const String &, const ScriptDynamicFloat4Ptr &) __Th___
    {
        // TODO
    }

/*
=================================================
    ConstantI*
=================================================
*/
    void  ScriptBasePass::ConstantI4 (const String &, const ScriptDynamicInt4Ptr &) __Th___
    {
        // TODO
    }

/*
=================================================
    SetDebugLabel
=================================================
*/
    void  ScriptBasePass::SetDebugLabel1 (const String &name) __Th___
    {
        _dbgName    = name;
    }

    void  ScriptBasePass::SetDebugLabel2 (const String &name, const RGBA8u &color) __Th___
    {
        _dbgName    = name;
        _dbgColor   = color;
    }

/*
=================================================
    _AddSlidersToUIInteraction
=================================================
*/
    void  ScriptBasePass::_AddSlidersToUIInteraction (IPass* pass) const
    {
        if ( _sliders.empty() )
            return;

        UIInteraction::PerPassSlidersInfo   info;
        SliderCounter_t                     slider_idx {};

        for (const auto& slider : _sliders)
        {
            const uint  idx = slider_idx[ uint(slider.type) ]++;
            BEGIN_ENUM_CHECKS();
            switch ( slider.type )
            {
                case ESlider::Int :
                    info.intRange[ idx * 2 + 0 ]                        = slider.intRange[0];
                    info.intRange[ idx * 2 + 1 ]                        = slider.intRange[1];
                    info.intVecSize[ idx ]                              = slider.count;
                    info.names[ UIInteraction::IntSlidersOffset + idx ] = slider.name;
                    break;

                case ESlider::Float :
                    info.floatRange[ idx * 2 + 0 ]                      = slider.floatRange[0];
                    info.floatRange[ idx * 2 + 1 ]                      = slider.floatRange[1];
                    info.floatVecSize[ idx ]                            = slider.count;
                    info.names[ UIInteraction::FloatSlidersOffset+idx ] = slider.name;
                    break;

                case ESlider::Color :
                    info.names[ UIInteraction::ColorSelectorOffset+idx ] = slider.name;
                    break;

                case ESlider::_Count :  break;
            }
            END_ENUM_CHECKS();
        }

        info.passName = this->_dbgName;
        UIInteraction::Instance().AddSliders( pass, RVRef(info) );
    }

/*
=================================================
    _CopyConstants
=================================================
*/
    void  ScriptBasePass::_CopyConstants (OUT IPass::Constants &dst) const
    {
        for (auto& c : _constants)
        {
            switch ( c.type )
            {
                case ESlider::Float :
                    dst.f[ c.index ] = c.f4->Get();
                    break;

                case ESlider::Int :
                    dst.i[ c.index ] = c.i4->Get();
                    break;
            }
        }
    }

/*
=================================================
    _SetDynamicDimension
=================================================
*/
    void  ScriptBasePass::_SetDynamicDimension (const ScriptDynamicDimPtr &dynDim) __Th___
    {
        CHECK_THROW_MSG( dynDim and dynDim->Get() );

        if ( _dynamicDim == dynDim or _dynamicDim->Get() == dynDim->Get() )
            return;

        CHECK_THROW_MSG( _dynamicDim.UseCount() == 1 and _dynamicDim->Get().use_count() == 2,
            "Previous dynamic dimension is already used" );

        _dynamicDim = dynDim;
    }

/*
=================================================
    _SetConstDimension
=================================================
*/
    void  ScriptBasePass::_SetConstDimension (const uint3 &) __Th___
    {
        // TODO
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptBasePass::Bind (const ScriptEnginePtr &se) __Th___
    {
        using namespace Scripting;

        ClassBinder<ScriptBasePass>     binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
    }


} // AE::ResEditor
