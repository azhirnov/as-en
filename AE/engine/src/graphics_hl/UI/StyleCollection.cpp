// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/StyleCollection.cpp.h"
#include "graphics_hl/UI/Drawable.h"
#include "graphics_hl/UI/System.h"

namespace AE::UI
{
    using namespace AE::Graphics;

/*
=================================================
    Serialize / Deserialize
=================================================
*/
    bool  StyleCollection::_BaseStyle::Serialize (Serializing::Serializer &ser) C_NE___
    {
        return Serialize_BaseStyle( *this, ser );
    }

    bool  StyleCollection::_BaseStyle::Deserialize (Serializing::Deserializer &des) __NE___
    {
        return  Deserialize_BaseStyle( OUT *this, des ) and
                _UpdatePipeline();
    }

    bool  StyleCollection::_BaseStyle::_UpdatePipeline () __NE___
    {
        pipeline = UIStyleCollection()._rtech->GetGraphicsPipeline( _pplnName );
        CHECK_ERR( pipeline );
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    GetColor
=================================================
*/
    RGBA8u  StyleCollection::ColorStyle::GetColor (EStyleState state) C_NE___
    {
        switch ( state )
        {
            case EStyleState::Selected :                            return color.selected;
            case EStyleState::Selected | EStyleState::MouseOver :   return color.selected;
            case EStyleState::Selected | EStyleState::TouchDown :   return color.selected;
            case EStyleState::MouseOver :                           return color.mouseOver;
            case EStyleState::TouchDown :                           return color.touchDown;
            case EStyleState::Disabled :                            return color.disabled;
        }
        return color.enabled;
    }

/*
=================================================
    Serialize / Deserialize
=================================================
*/
    bool  StyleCollection::ColorStyle::Serialize (Serializing::Serializer &ser) C_NE___
    {
        return Serialize_ColorStyle( *this, ser );
    }

    bool  StyleCollection::ColorStyle::Deserialize (Serializing::Deserializer &des) __NE___
    {
        return  Deserialize_ColorStyle( OUT *this, des ) and
                _UpdatePipeline();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    GetImage
=================================================
*/
    Pair< ImageInAtlasName::Optimized_t, RGBA8u >  StyleCollection::ImageStyle::GetImage (EStyleState state) C_NE___
    {
        switch ( state )
        {
            case EStyleState::Selected :                            return { image.selected,    color.selected  };
            case EStyleState::Selected | EStyleState::MouseOver :   return { image.selected,    color.selected  };
            case EStyleState::Selected | EStyleState::TouchDown :   return { image.selected,    color.selected  };
            case EStyleState::MouseOver :                           return { image.mouseOver,   color.mouseOver };
            case EStyleState::TouchDown :                           return { image.touchDown,   color.touchDown };
            case EStyleState::Disabled :                            return { image.disabled,    color.disabled  };
        }
        return { image.enabled, color.enabled };
    }

/*
=================================================
    Serialize / Deserialize
=================================================
*/
    bool  StyleCollection::ImageStyle::Serialize (Serializing::Serializer &ser) C_NE___
    {
        return Serialize_ImageStyle( *this, ser );
    }

    bool  StyleCollection::ImageStyle::Deserialize (Serializing::Deserializer &des) __NE___
    {
        return  Deserialize_ImageStyle( OUT *this, des ) and
                _UpdatePipeline();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Serialize / Deserialize
=================================================
*/
    bool  StyleCollection::FontStyle::Serialize (Serializing::Serializer &ser) C_NE___
    {
        return Serialize_FontStyle( *this, ser );
    }

    bool  StyleCollection::FontStyle::Deserialize (Serializing::Deserializer &des) __NE___
    {
        return  Deserialize_FontStyle( OUT *this, des ) and
                _UpdatePipeline();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor / destructor
=================================================
*/
    StyleCollection::StyleCollection () __NE___
    {}

    StyleCollection::~StyleCollection () __NE___
    {}

/*
=================================================
    _GetStyle
=================================================
*/
    template <typename StyleType>
    StyleType const*  StyleCollection::_GetStyle (StyleName::Ref id) C_NE___
    {
        auto    it = _styleMap.find( id );
        if_likely( it != _styleMap.end() )
        {
            if_likely( auto* style = UnionGet<StyleType>( it->second ))
                return style;
        }
        return null;
    }

    Ptr<const StyleCollection::ColorStyle>  StyleCollection::GetColorStyle (StyleName::Ref id)  C_NE___ { return _GetStyle<ColorStyle>( id ); }
    Ptr<const StyleCollection::ImageStyle>  StyleCollection::GetImageStyle (StyleName::Ref id)  C_NE___ { return _GetStyle<ImageStyle>( id ); }
    Ptr<const StyleCollection::FontStyle>   StyleCollection::GetFontStyle (StyleName::Ref id)   C_NE___ { return _GetStyle<FontStyle>( id ); }

/*
=================================================
    Initialize
=================================================
*/
    bool  StyleCollection::Initialize (RenderTechPipelinesPtr rtech, const Bytes ubSize, RC<RStream> stream) __NE___
    {
        CHECK_ERR( rtech );
        CHECK_ERR( stream );
        CHECK_ERR( AllBits( stream->GetSourceType(), IDataSource::ESourceType::Buffered ));

        _rtech = RVRef(rtech);

        const auto      info        = _rtech->GetPass( RenderTechPassName{"Main"} );
        auto&           rts         = GraphicsScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        const auto      ub_align    = DeviceLimits.res.minUniformBufferOffsetAlign;
        const Bytes     buf_size    = AlignUp( ubSize, ub_align ) * MaxScreens * rts.GetMaxFrames();

        _dynamicUBufSize = AlignUp( ubSize, ub_align );

        _dynamicUBuf = res_mngr.CreateBuffer( BufferDesc{ buf_size, EBufferUsage::TransferDst | EBufferUsage::Uniform }.SetMemory( EMemoryType::DeviceLocal ),
                                              "UI dynamic ubuffer" );
        CHECK_ERR( _dynamicUBuf );

        CHECK_ERR( info.dsLayoutId.IsValid() );

        _sharedDescSet = res_mngr.CreateDescriptorSet( info.dsLayoutId, null, "UI global DS" );
        CHECK_ERR( _sharedDescSet );
        CHECK_ERR( info.dsIndex == IDrawable::GlobalMaterial::dsIndex );
        {
            DescriptorUpdater   updater;
            CHECK_ERR( updater.Set( _sharedDescSet, EDescUpdateMode::Partialy ));
            CHECK_ERR( updater.BindBuffer( UniformName{"globalUB"}, _dynamicUBuf, 0_b, ubSize ));
            //CHECK_ERR( updater.BindBuffer( UniformName{"mtrUB"},  _dynamicUBuf, 0_b, ubSize ));
            CHECK_ERR( updater.Flush() );
        }

        // deserialize
        {
            Serializing::Deserializer   des {RVRef(stream)};
            NOTHROW_ERR(
                CHECK_ERR( _Deserialize( des ));
            )
        }

        return true;
    }

/*
=================================================
    _Deserialize
=================================================
*/
    bool  StyleCollection::_Deserialize (Serializing::Deserializer &des) __Th___
    {
        PipelineName    dbg_ppln_name;
        uint            style_count     = 0;

        CHECK_ERR( des( OUT _imageAtlasRGBAName, /*OUT _imageAtlasAlphaName,*/ OUT dbg_ppln_name, OUT style_count ));

        if ( dbg_ppln_name.IsDefined() )
        {
            _dbgPpln = _rtech->GetGraphicsPipeline( dbg_ppln_name );
            CHECK_ERR( _dbgPpln );
        }

        _styleMap.reserve( style_count );   // throw

        bool    result = true;
        for (uint i = 0; result and (i < style_count); ++i)
        {
            StyleName::Optimized_t  name;
            EType                   type;
            Style_t                 style;

            result = des( OUT name, OUT type );

            switch_enum( type )
            {
                case EType::ColorStyle :
                    result = result and style.emplace<ColorStyle>().Deserialize( des ); break;

                case EType::ImageStyle :
                    result = result and style.emplace<ImageStyle>().Deserialize( des ); break;

                case EType::FontStyle :
                    result = result and style.emplace<FontStyle>().Deserialize( des );  break;

                case EType::_Count :
                default :
                    result = false;     break;
            }
            switch_end

            result = result and _styleMap.emplace( name, style ).second;    // throw
        }

        return result;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  StyleCollection::Deinitialize () __NE___
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();

        _rtech = null;

        res_mngr.DelayedReleaseResources( _dynamicUBuf, _sharedDescSet );
    }


} // AE::UI
