// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    [b][/b] - bold
    [i][/i] - italic
    [u][/u] - underline
    [s][/s] - strikethrough

    [style ...][/style]
        color=#00000000 - RRGGBBAA
        size=10         - size in pixels
*/

#pragma once

#include "pch/Serializing.h"

namespace AE::Graphics
{
    using namespace AE::Base;


    //
    // Formatted Text
    //

    class FormattedText final : public Serializing::ISerializable
    {
    // types
    public:
        struct Chunk
        {
            Chunk const*    next        = null;
            RGBA8u          color;
            uint            length      : 16;
            uint            height      : 9;    // pixels
            uint            bold        : 1;    // [b]
            uint            italic      : 1;    // [i]
            uint            underline   : 1;    // [u]
            uint            strikeout   : 1;    // [s]
            CharUtf8        string[1];          // null terminated utf8 string

            Chunk ()                        __NE___ = default;

            ND_ float           Height ()   C_NE___ { return float(height); }
            ND_ U8StringView    Text ()     C_NE___ { return U8StringView{ string, length }; }
        };

        using Self          = FormattedText;
        using Allocator_t   = LinearAllocator<>;    // TODO: optimize


    // variables
    private:
        Chunk*          _first      = null;
        uint            _maxChars   = 0;
        Allocator_t     _alloc;


    // methods
    public:
        FormattedText ()                                        __NE___ { _alloc.SetBlockSize( 512_b ); }
        explicit FormattedText (StringView str)                 __Th___ : FormattedText()   { Append( str ); }
        explicit FormattedText (U8StringView str)               __Th___ : FormattedText()   { Append( str ); }
        FormattedText (const FormattedText &other)              __Th___ : FormattedText()   { Append( other ); }
        FormattedText (FormattedText &&other)                   __NE___ : _first{other._first}, _alloc{RVRef(other._alloc)} { other._first = null; }
        ~FormattedText ()                                       __NE___ {}

        Self&  Append (const FormattedText &)                   __Th___;
        Self&  Append (U8StringView str)                        __Th___;
        Self&  Append (StringView str)                          __Th___;

        void   Clear ()                                         __NE___;

        FormattedText&  operator = (const FormattedText &rhs)   __Th___ { _alloc.Discard();  _first = null;  Append( rhs );  return *this; }
        FormattedText&  operator = (FormattedText &&rhs)        __NE___ { _alloc = RVRef(rhs._alloc);  _first = rhs._first;  rhs._first = null;  return *this; }

        ND_ bool            Empty ()                            C_NE___ { return _first == null; }
        ND_ Chunk const*    GetFirst ()                         C_NE___ { return _first; }
        ND_ uint            GetMaxChars ()                      C_NE___ { return _maxChars; }

        ND_ U8String        ToString ()                         C_NE___;


        // ISerializable //
        bool  Serialize (Serializing::Serializer &)             C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &)         __NE_OV;

    private:
        ND_ U8String        _ToString ()                        C_Th___;
    };



    //
    // Precalculated Formatted Text
    //

    class PrecalculatedFormattedText final : public Serializing::ISerializable
    {
        friend class RasterFont;

    // types
    private:
        using Self  = PrecalculatedFormattedText;


    // variables
    private:
        Array<float>    _lineHeight;        // in pixels    // TODO: small vector
        float2          _size;              // in pixels
        FormattedText   _text;
        float           _spacing    = 1.0f; // in normalized coords
        bool            _wordWrap   = false;


    // methods
    public:
        PrecalculatedFormattedText ()                           __NE___ {}
        ~PrecalculatedFormattedText ()                          __NE___ {}

        Self&  SetText (FormattedText value)                    __NE___ { _text = RVRef(value); return *this; }
        Self&  SetSpacing (float value)                         __NE___ { _spacing  = value;    return *this; }
        Self&  SetWordWrap (bool value)                         __NE___ { _wordWrap = value;    return *this; }

        ND_ FormattedText const&    Text ()                     C_NE___ { return _text; }
        ND_ float                   Spacing ()                  C_NE___ { return _spacing; }
        ND_ float2 const&           RegionSize ()               C_NE___ { return _size; }
        ND_ bool                    IsWordWrap ()               C_NE___ { return _wordWrap; }
        ND_ ArrayView<float>        LineHeights ()              C_NE___ { return _lineHeight; }

        // ISerializable //
        bool  Serialize (Serializing::Serializer &)             C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &)         __NE_OV;
    };


} // AE::Graphics
