// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/Drawable.h"

#ifndef AE_UI_NO_GRAPHICS
# include "graphics_hl/UI/System.h"
#endif

namespace AE::UI
{
    using namespace AE::Graphics;
    using namespace AE::Serializing;

/*
=================================================
    _GetColorAnimSpeed
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
    float  IDrawable::_GetColorAnimSpeed () C_NE___
    {
        return UIStyleCollection().GetSettings().colorAnimSpeed;
    }
#endif

/*
=================================================
    Material::operator ==
=================================================
*/
    bool  IDrawable::Material::operator == (const Material &rhs) C_NE___
    {
        return  //stencilRef        == rhs.stencilRef       and
            //  topology        == rhs.topology         and
            //  dynamicOffset   == rhs.dynamicOffset    and
                ppln            == rhs.ppln;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    SetStyle
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
    bool  RectangleDrawable::SetStyle (StyleName::Ref name) __NE___
    {
        _data.stylePtr  = UIStyleCollection().GetColorStyle( name );
        CHECK_ERR( _data.stylePtr );

        _style          = name;
        _data.mtr.ppln  = _data.stylePtr->pipeline;

        _data.currColor = _data.stylePtr->GetColor( _data.currStyle );
        _data.prevColor = _data.currColor;

        return true;
    }
#else
    bool  RectangleDrawable::SetStyle (StyleName::Ref name) __NE___
    {
        _style = name;
        return true;
    }
#endif

/*
=================================================
    Draw
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
    void  RectangleDrawable::Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th___
    {
        // flush canvas if different materials
        if_unlikely( params.mtr != _data.mtr and not canvas.IsEmpty() )
        {
            ctx.BindPipeline( params.mtr->ppln );
            ctx.BindDescriptorSet( params.mtr.dsIndex, params.mtr.ds, {params.mtr.globalDynOffset} );   // TODO
        //  ctx.SetStencilReference( params.mtr->stencilRef );
            canvas.Flush( ctx, params.mtr.topology );
        }
        params.mtr = _data.mtr;

        // change state
        if_unlikely( _data.currStyle != params.style )
        {
            _data.currStyle = params.style;
            _data.prevColor = (_data.factor < 1.0f ? Lerp( _data.prevColor, _data.currColor, _data.factor ) : _data.currColor);
            _data.currColor = _data.stylePtr->GetColor( _data.currStyle );
            _data.factor    = 0.0f;
        }

        RGBA8u  color = _data.currColor;

        // color animation
        if_unlikely( _data.factor < 1.0f )
        {
            _data.factor    = Min( 1.0f, _data.factor + params.dt * _GetColorAnimSpeed() );
            color           = Lerp( _data.prevColor, _data.currColor, _data.factor );
        }

        canvas.Draw( Rectangle2D{ params.clipRect, RectF{}, color });
    }
#else
    void  RectangleDrawable::Draw (const DrawParams &, Canvas &, DrawContext_t &) __Th___
    {}
#endif

/*
=================================================
    Serialize / Deserialize
=================================================
*/
    bool  RectangleDrawable::Serialize (Serializer &ser) C_NE___
    {
        return ser( _GetDrawableID(GetType()), _style );
    }

    bool  RectangleDrawable::Deserialize (Deserializer &des) __NE___
    {
        if_unlikely( not des( OUT _style ))
            return false;

        return SetStyle( StyleName{_style} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    DrawableSerializer
=================================================
*/
namespace
{
    template <typename T>
    struct DrawableSerializer
    {
        static bool  Serialize (Serializer &, const void*) __NE___ { return false; }

        static bool  Deserialize (Deserializer &des, INOUT void* &ptr, Ptr<IAllocator> alloc) __NE___
        {
            CHECK_ERR( ptr == null and alloc != null );

            ptr = alloc->Allocate<T>();
            CHECK_ERR( ptr != null );

            return PlacementNew<T>( OUT ptr, alloc )->Deserialize( des );
        }
    };
}
/*
=================================================
    RegisterDrawables
=================================================
*/
    bool  IDrawable::RegisterDrawables (ObjectFactory &factory) __NE___
    {
        switch_enum( EType::Unknown )
        {
            case EType::Unknown :
            case EType::Image :
            case EType::NinePatch :
            case EType::Text :
            case EType::_Count :
            #define REG( _name_ )                                                           \
                case EType::_name_ :                                                        \
                    CHECK_ERR( factory.Register< _name_##Drawable >(                        \
                                    SerializedID{ #_name_ "Drawable" },                     \
                                    &DrawableSerializer< _name_##Drawable >::Serialize,     \
                                    &DrawableSerializer< _name_##Drawable >::Deserialize ));
            REG( Rectangle )
        //  REG( Image )
        //  REG( NinePatch )
        //  REG( Text )
            #undef REG

            default : break;
        }
        switch_end
        return true;
    }

/*
=================================================
    _GetDrawableID
=================================================
*/
    SerializedID  IDrawable::_GetDrawableID (EType type) __NE___
    {
        switch_enum( type )
        {
            #define REG( _name_ )   case EType::_name_ : return SerializedID{ #_name_ "Drawable" };
            REG( Rectangle )
            REG( Image )
            REG( NinePatch )
            REG( Text )
            #undef REG
            case EType::_Count :
            case EType::Unknown :   break;
        }
        switch_end
        return Default;
    }


} // AE::UI
