// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/IPass.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Resources/ResourceArray.cpp.h"

namespace AE::ResEditor
{
namespace
{
    template <typename B>
    struct CopyConstant
    {
        B&      _dst;
        RC<>    _rc;

        CopyConstant (B& dst, RC<> rc) : _dst{dst}, _rc{RVRef(rc)}
        {}

        template <typename T, usize I>
        void  operator () () __NE___
        {
            if ( auto* dyn_val = DynCast<T>( _rc.get() ))
            {
                auto    val = dyn_val->Get();
                std::memcpy( OUT &_dst, &val, sizeof(val) );
            }
        }
    };

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    constructor / destructor
=================================================
*/
    ResourceArray::ResourceArray ()
    {}

    ResourceArray::~ResourceArray ()
    {}
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    IPass::IPass ()
    {}

    IPass::IPass (StringView dbgName, RGBA8u dbgColor) :
        _dbgName{dbgName}, _dbgColor{dbgColor}
    {}

/*
=================================================
    destructor
=================================================
*/
    IPass::~IPass () __NE___
    {
        UIInteraction::Instance().RemovePass( this );
    }

/*
=================================================
    _CopySliders
=================================================
*/
    void  IPass::_CopySliders (OUT StaticArray<float4, 4>   &dstFloats,
                               OUT StaticArray<int4, 4>     &dstInts,
                               OUT StaticArray<float4, 4>   &dstColors) const
    {
        if ( auto p_sliders = UIInteraction::Instance().GetSliders( this ))
        {
            auto    sliders = p_sliders->ReadNoLock();
            SHAREDLOCK( sliders );

            StaticAssert( sizeof(dstFloats) == sizeof(sliders->floatSliders) );
            StaticAssert( sizeof(dstInts)       == sizeof(sliders->intSliders) );
            StaticAssert( sizeof(dstColors) == sizeof(sliders->colors) );

            std::memcpy( OUT dstFloats.data(),  sliders->floatSliders.data(),   sizeof(dstFloats) );
            std::memcpy( OUT dstInts.data(),    sliders->intSliders.data(),     sizeof(dstInts) );
            std::memcpy( OUT dstColors.data(),  sliders->colors.data(),         sizeof(dstColors) );
        }
    }

/*
=================================================
    _CopyConstants
=================================================
*/
    void  IPass::_CopyConstants (const Constants            &c,
                                 OUT StaticArray<float4, 4> &dstFloats,
                                 OUT StaticArray<int4, 4>   &dstInts) const
    {
        StaticAssert( sizeof(dstFloats) == sizeof(float4) * Constants::MaxCount );
        StaticAssert( sizeof(dstInts)       == sizeof(int4) * Constants::MaxCount );

        for (usize i = 0; i < c.f.size(); ++i)
        {
            if ( c.f[i] )
                DynamicFloatTypes_t::Visit( CopyConstant{ OUT dstFloats[i], c.f[i] });
        }
        for (usize i = 0; i < c.i.size(); ++i)
        {
            if ( c.i[i] )
                DynamicIntTypes_t::Visit( CopyConstant{ OUT dstInts[i], c.i[i] });
        }
    }

/*
=================================================
    _IsEnabled
=================================================
*/
    bool  IPass::_IsEnabled () const
    {
        if_likely( not _enablePass.dynamic )
            return true;

        const uint  lhs = _enablePass.dynamic->Get();
        const uint  rhs = _enablePass.ref;

        switch_enum( _enablePass.op )
        {
            case ECompareOp::Less :         return lhs <  rhs;
            case ECompareOp::Equal :        return lhs == rhs;
            case ECompareOp::LEqual :       return lhs <= rhs;
            case ECompareOp::Greater :      return lhs >  rhs;
            case ECompareOp::NotEqual :     return lhs != rhs;
            case ECompareOp::GEqual :       return lhs >= rhs;
            case ECompareOp::Always :       return true;

            case ECompareOp::Never :
            case ECompareOp::_Count :
            case ECompareOp::Unknown :      break;
        }
        switch_end
        return true;
    }


} // AE::ResEditor
