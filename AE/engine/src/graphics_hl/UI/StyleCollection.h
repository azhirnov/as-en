// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/VFS.h"
#include "graphics_hl/UI/LayoutEnums.h"

namespace AE::UI
{

    //
    // Style Collection
    //

    class StyleCollection final
    {
        friend class SystemImpl;

    // types
    public:
        enum class EType : ubyte
        {
            ColorStyle,
            ImageStyle,
            FontStyle,
            _Count
        };


        class _BaseStyle
        {
        // variables
        public:
            Graphics::PipelineName      _pplnName;
            GraphicsPipelineID          pipeline;

        // methods
        public:
            bool  Serialize (Serializing::Serializer &)     C_NE___;
            bool  Deserialize (Serializing::Deserializer &) __NE___;

        protected:
            bool  _UpdatePipeline ()                        __NE___;
        };


        class ColorStyle : public _BaseStyle
        {
        // variables
        public:
            struct {
                RGBA8u          disabled;
                RGBA8u          enabled;
                RGBA8u          mouseOver;
                RGBA8u          touchDown;
                RGBA8u          selected;
            }   color;

        // methods
        public:
            ColorStyle ()                                   __NE___ {}

            ND_ RGBA8u  GetColor (EStyleState state)        C_NE___;

            bool  Serialize (Serializing::Serializer &)     C_NE___;
            bool  Deserialize (Serializing::Deserializer &) __NE___;
        };


        class ImageStyle final : public ColorStyle
        {
        // variables
        public:
            struct {
                ImageInAtlasName::Optimized_t   disabled;
                ImageInAtlasName::Optimized_t   enabled;
                ImageInAtlasName::Optimized_t   mouseOver;
                ImageInAtlasName::Optimized_t   touchDown;
                ImageInAtlasName::Optimized_t   selected;
            }   image;

        // methods
        public:
            ImageStyle ()                                   __NE___ {}

            ND_ auto  GetImage (EStyleState state)          C_NE___ -> Pair< ImageInAtlasName::Optimized_t, RGBA8u >;

            bool  Serialize (Serializing::Serializer &)     C_NE___;
            bool  Deserialize (Serializing::Deserializer &) __NE___;
        };


        class FontStyle final : public ColorStyle
        {
        // variables
        public:
            RC<RasterFont>      font;

        // methods
        public:
            FontStyle ()                                    __NE___ {}

            bool  Serialize (Serializing::Serializer &)     C_NE___;
            bool  Deserialize (Serializing::Deserializer &) __NE___;
        };


        struct AnimationSettings
        {
            float               colorAnimSpeed  = 10.0f;        // 1 / seconds
        };

        static constexpr uint   MaxScreens  = 32;   // TODO: minimize


    private:
        using Style_t       = Union< NullUnion, ColorStyle, ImageStyle, FontStyle >;
        using StyleMap_t    = FlatHashMap< StyleName::Optimized_t, Style_t >;


    // variables
    private:
        RenderTechPipelinesPtr      _rtech;

        StyleMap_t                  _styleMap;
        AnimationSettings           _settings;

        Strong<DescriptorSetID>     _sharedDescSet;
        Strong<BufferID>            _dynamicUBuf;
        Bytes16u                    _dynamicUBufSize;

        GraphicsPipelineID          _dbgPpln;

        VFS::FileName::Optimized_t  _imageAtlasRGBAName;
        VFS::FileName::Optimized_t  _imageAtlasAlphaName;

        RC<StaticImageAtlas>        _imageAtlasRGBA;
        RC<StaticImageAtlas>        _imageAtlasAlpha;


    // methods
    public:
        StyleCollection ()                                                                              __NE___;
        ~StyleCollection ()                                                                             __NE___;

        ND_ bool  Initialize (RenderTechPipelinesPtr rtech, Bytes ubSize, RC<RStream> stream)           __NE___;
            void  Deinitialize ()                                                                       __NE___;

        ND_ Ptr<const ColorStyle>   GetColorStyle (StyleName::Ref id)                                   C_NE___;
        ND_ Ptr<const ImageStyle>   GetImageStyle (StyleName::Ref id)                                   C_NE___;
        ND_ Ptr<const FontStyle>    GetFontStyle (StyleName::Ref id)                                    C_NE___;
        ND_ GraphicsPipelineID      GetDebugDrawPipeline ()                                             C_NE___ { return _dbgPpln; }

        ND_ AnimationSettings const&    GetSettings ()                                                  C_NE___ { return _settings; }


    private:
        template <typename StyleType>
        ND_ StyleType const*  _GetStyle (StyleName::Ref id)                                             C_NE___;

        ND_ bool  _Deserialize (Serializing::Deserializer &)                                            __Th___;
    };


} // AE::UI
