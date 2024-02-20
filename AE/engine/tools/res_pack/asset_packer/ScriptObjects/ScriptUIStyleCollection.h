// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"

#include "graphics_hl/UI/StyleCollection.h"

namespace AE::AssetPacker
{

    //
    // Script UI Style Collection
    //

    class ScriptUIStyleCollection final : public EnableScriptRC
    {
    // types
    public:
        using StyleCollection   = UI::StyleCollection;
        using ImageAtlasInfo    = ObjectStorage::ImageAtlasInfo;
        using EType             = StyleCollection::EType;


        //
        // Color Style
        //
        class ScriptColorStyle final : public EnableScriptRC
        {
        private:
            ScriptUIStyleCollection &       _collection;
            const String                    _styleName;
            StyleCollection::ColorStyle     _internal;

        public:
            explicit ScriptColorStyle (ScriptUIStyleCollection &c) : _collection{c} {}

            void  SetPipeline (const String &name)                          __Th___;
            void  SetDisabled (const RGBA8u &col)                           __Th___;
            void  SetEnabled (const RGBA8u &col)                            __Th___;
            void  SetMouseOver (const RGBA8u &col)                          __Th___;
            void  SetTouchDown (const RGBA8u &col)                          __Th___;
            void  SetSelected (const RGBA8u &col)                           __Th___;

            static void  Bind (const ScriptEnginePtr &se)                   __Th___;

            void  Serialize (Serializing::Serializer &)                     __Th___;
        };
        using ScriptColorStylePtr = ScriptRC< ScriptColorStyle >;


        //
        // Image Style
        //
        class ScriptImageStyle final : public EnableScriptRC
        {
        private:
            ScriptUIStyleCollection &       _collection;
            const String                    _styleName;
            StyleCollection::ImageStyle     _internal;

        public:
            explicit ScriptImageStyle (ScriptUIStyleCollection &c) : _collection{c} {}

            void  SetDisabled (const RGBA8u &col, const String &imgName)    __Th___;
            void  SetEnabled (const RGBA8u &col, const String &imgName)     __Th___;
            void  SetMouseOver (const RGBA8u &col, const String &imgName)   __Th___;
            void  SetTouchDown (const RGBA8u &col, const String &imgName)   __Th___;
            void  SetSelected (const RGBA8u &col, const String &imgName)    __Th___;
            void  SetPipeline (const String &name)                          __Th___;

            static void  Bind (const ScriptEnginePtr &se)                   __Th___;

            void  Serialize (Serializing::Serializer &)                     __Th___;
        };
        using ScriptImageStylePtr = ScriptRC< ScriptImageStyle >;


        //
        // Font Style
        //
        class ScriptFontStyle final : public EnableScriptRC
        {
        private:
            ScriptUIStyleCollection &       _collection;
            const String                    _styleName;
            StyleCollection::FontStyle      _internal;
            String                          _fontName;

        public:
            explicit ScriptFontStyle (ScriptUIStyleCollection &c) : _collection{c} {}

            void  SetDisabled (const RGBA8u &col)                           __Th___;
            void  SetEnabled (const RGBA8u &col)                            __Th___;
            void  SetMouseOver (const RGBA8u &col)                          __Th___;
            void  SetTouchDown (const RGBA8u &col)                          __Th___;
            void  SetSelected (const RGBA8u &col)                           __Th___;
            void  SetFont (const String &fontName)                          __Th___;
            void  SetPipeline (const String &name)                          __Th___;

            static void  Bind (const ScriptEnginePtr &se)                   __Th___;

            void  Serialize (Serializing::Serializer &)                     __Th___;
        };
        using ScriptFontStylePtr = ScriptRC< ScriptFontStyle >;


    private:
        using StyleMap_t    = HashMap< String, ScriptRC<EnableScriptRC> >;


    // variables
    private:
        StyleMap_t          _styleMap;

        RC<ImageAtlasInfo>  _imageAtlas;

        PipelineName        _dbgPplnName;


    // methods
    public:
        ScriptUIStyleCollection ();
        ~ScriptUIStyleCollection ();

        void  SetAtlas (const String &atlasName)                            __Th___;
        void  SetDebugPipeline (const String &pplnName)                     __Th___;

        ScriptColorStyle*   AddColorStyle (const String &name)              __Th___;
        ScriptImageStyle*   AddImageStyle (const String &name)              __Th___;
        ScriptFontStyle*    AddFontStyle (const String &name)               __Th___;

        void  Store (const String &nameInArchive)                           __Th___;

        static void  Bind (const ScriptEnginePtr &se)                       __Th___;

    private:
        void  _Pack (RC<WStream> stream)                                    __Th___;
        void  _CheckImageInAtlas (const String &name)                       __Th___;
        void  _CheckPipeline (const String &name)                           __Th___;
    };

    using ScriptUIStyleCollectionPtr = ScriptRC< ScriptUIStyleCollection >;


} // AE::AssetPacker
