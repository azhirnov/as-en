// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/IPass.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Resources/ResourceArray.cpp.h"

namespace AE::ResEditor
{

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
    IPass::~IPass ()
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

            STATIC_ASSERT( sizeof(dstFloats)    == sizeof(sliders->floatSliders) );
            STATIC_ASSERT( sizeof(dstInts)      == sizeof(sliders->intSliders) );
            STATIC_ASSERT( sizeof(dstColors)    == sizeof(sliders->colors) );

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
        STATIC_ASSERT( sizeof(dstFloats)    == sizeof(float4) * Constants::MaxCount );
        STATIC_ASSERT( sizeof(dstInts)      == sizeof(int4) * Constants::MaxCount );

        for (usize i = 0; i < c.f.size(); ++i) {
            dstFloats[i] = (c.f[i] ? c.f[i]->Get() : float4{});
        }
        for (usize i = 0; i < c.i.size(); ++i) {
            dstInts[i] = (c.i[i] ? c.i[i]->Get() : int4{});
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

        BEGIN_ENUM_CHECKS();
        switch ( _enablePass.op )
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
        END_ENUM_CHECKS();
        return true;
    }


} // AE::ResEditor
