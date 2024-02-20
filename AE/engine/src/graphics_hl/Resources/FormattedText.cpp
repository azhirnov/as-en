// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/GraphicsHL.pch.h"
#include "graphics_hl/Resources/FormattedText.h"

#ifndef AE_ENABLE_UTF8PROC
#   error AE_ENABLE_UTF8PROC required
#endif

namespace AE::Graphics
{
namespace {

    enum class EChunkType
    {
        Unknown,
        Initial,
        Bold,
        Italic,
        Underline,
        Strikeout,
        Style,
    };

    static constexpr uint   DefaultTextHeight = 16;
}

/*
=================================================
    Append
=================================================
*/
    FormattedText&  FormattedText::Append (const FormattedText &other)
    {
        Chunk*  last_chunk = null;
        for (last_chunk = _first; last_chunk; last_chunk = const_cast<Chunk*>(last_chunk->next))
        {}

        for (Chunk const* curr = other._first; curr; curr = curr->next)
        {
            Bytes   size    {sizeof(Chunk) + curr->length};
            Chunk*  chunk   = Cast<Chunk>( _alloc.Allocate( SizeAndAlign{ size, Bytes{alignof(Chunk)} }));
            CHECK_THROW( chunk != null, std::bad_alloc{} );

            MemCopy( OUT chunk, curr, size );

            if ( not _first )   _first = chunk;
            else                last_chunk->next = chunk;
            last_chunk = chunk;
        }

        return *this;
    }

/*
=================================================
    Append
=================================================
*/
    FormattedText&  FormattedText::Append (StringView str)
    {
        return Append( U8StringView{ Cast<CharUtf8>(str.data()), str.length() });
    }

    FormattedText&  FormattedText::Append (U8StringView str)
    {
        struct State
        {
            EChunkType  type        = Default;
            RGBA8u      color;
            uint        length      : 16;
            uint        height      : 9;
            uint        bold        : 1;
            uint        italic      : 1;
            uint        underline   : 1;
            uint        strikeout   : 1;

            State () : length{0}, height{0}, bold{0}, italic{0}, underline{0}, strikeout{0} {}
            State (const State &) = default;
        };
        //---------------------------------------------------------------------------

        static constexpr uint   tag_bold        = 'b' | (']' << 8);
        static constexpr uint   tag_italic      = 'i' | (']' << 8);
        static constexpr uint   tag_underline   = 'u' | (']' << 8);
        static constexpr uint   tag_strikeout   = 's' | (']' << 8);

        Array<State>    states;

        const auto  ParseOpeningTag = [&states, str] (INOUT usize &pos) -> bool
        {
            const uint  u = uint((str.data() + pos)[0]) | (uint((str.data() + pos)[1]) << 8);
            switch ( u )
            {
                case tag_bold : {
                    State   state = states.back();
                    state.type = EChunkType::Bold;
                    state.bold = true;
                    states.push_back( state );
                    pos += 2;
                    return true;
                }
                case tag_italic : {
                    State   state = states.back();
                    state.type = EChunkType::Italic;
                    state.italic = true;
                    states.push_back( state );
                    pos += 2;
                    return true;
                }
                case tag_underline : {
                    State   state = states.back();
                    state.type = EChunkType::Underline;
                    state.underline = true;
                    states.push_back( state );
                    pos += 2;
                    return true;
                }
                case tag_strikeout : {
                    State   state = states.back();
                    state.type = EChunkType::Strikeout;
                    state.strikeout = true;
                    states.push_back( state );
                    pos += 2;
                    return true;
                }
            }

            // parse 'style ...'
            constexpr char  tag_style[] = "style";
            if_unlikely( MemEqual( str.data() + pos, tag_style, Sizeof(tag_style)-1 ))
            {
                pos += sizeof(tag_style)-1;

                usize   end     = Min( str.find( ']', pos ), str.length() );
                State   state   = states.back();
                state.type = EChunkType::Style;

                // search for 'size' and 'color' between 'start' and 'end'
                constexpr char  tag_size[]  = "size=";
                constexpr char  tag_color[] = "color=#";

                for (; pos < end;)
                {
                    const char  c = str[pos];
                    if ( (c == ' ') or (c == '\t') )
                    {
                        ++pos;
                        continue;
                    }

                    // 'size=x'
                    if_unlikely( MemEqual( str.data()+pos, tag_size, Sizeof(tag_size)-1 ))
                    {
                        state.height = 0;
                        for (pos += sizeof(tag_size)-1; pos < end;)
                        {
                            const char  k = str[pos];
                            if ( (k >= '0') and (k <= '9') )
                                state.height = state.height*10 + (k-'0');
                            else
                                break;
                            ++pos;
                        }
                        continue;
                    }

                    // 'color=#RRGGBBAA'
                    if_unlikely( MemEqual( str.data()+pos, tag_color, Sizeof(tag_color)-1 ))
                    {
                        uint    color = 0;
                        for (pos += sizeof(tag_color)-1; pos < end;)
                        {
                            const char  k = str[pos];
                            if ( (k >= '0') and (k <= '9') )    color = (color << 4) + (k-'0');     else
                            if ( (k >= 'A') and (k <= 'F') )    color = (color << 4) + (k-'A'+10);  else
                            if ( (k >= 'a') and (k <= 'f') )    color = (color << 4) + (k-'a'+10);  else
                                                                break;
                            ++pos;
                        }
                        state.color = RGBA8u{ ubyte((color >> 24) & 0xFF), ubyte((color >> 16) & 0xFF),
                                              ubyte((color >>  8) & 0xFF), ubyte((color & 0xFF)) };
                        continue;
                    }

                    ASSERT(false);
                    break;
                }
                pos = end+1;
                states.push_back( state );
                return true;
            }
            return false;
        };

        const auto  ParseClosingTag = [&states, str] (INOUT usize &pos) -> bool
        {
            if ( states.size() < 2 ) {
                ASSERT(false);
                return false;
            }
            const uint  u = *(str.data() + pos) | ((str.data() + pos)[1] << 8);
            switch ( u )
            {
                case tag_bold :
                    if ( states.back().type == EChunkType::Bold ) {
                        states.pop_back();
                        pos += 2;
                        return true;
                    }
                    return false;

                case tag_italic :
                    if ( states.back().type == EChunkType::Italic ) {
                        states.pop_back();
                        pos += 2;
                        return true;
                    }
                    return false;

                case tag_underline :
                    if ( states.back().type == EChunkType::Underline ) {
                        states.pop_back();
                        pos += 2;
                        return true;
                    }
                    return false;

                case tag_strikeout :
                    if ( states.back().type == EChunkType::Strikeout ) {
                        states.pop_back();
                        pos += 2;
                        return true;
                    }
                    return false;
            }

            // parse 'style'
            constexpr char  tag_style[] = "style]";
            if ( states.back().type == EChunkType::Style and
                 MemEqual( str.data() + pos, tag_style, Sizeof(tag_style)-1 ))
            {
                states.pop_back();
                pos += sizeof(tag_style)-1;
                return true;
            }
            return false;
        };

        Chunk*  last_chunk = null;
        const auto  AddChunk = [this, &last_chunk] (const CharUtf8* tag, const usize length, const State &state)
        {{
            if ( length == 0 )
                return;

            void*   ptr = _alloc.Allocate( SizeAndAlign{ Bytes{sizeof(Chunk) + length}, Bytes{alignof(Chunk)} });
            CHECK_THROW( ptr != null, std::bad_alloc{} );

            auto*   chunk   = PlacementNew<Chunk>( OUT ptr );

            MemCopy( OUT chunk->string, tag, Bytes{length} );
            chunk->string[length] = '\0';
            chunk->length       = uint(length);
            chunk->color        = state.color;
            chunk->height       = state.height;
            chunk->bold         = state.bold;
            chunk->italic       = state.italic;
            chunk->underline    = state.underline;
            chunk->strikeout    = state.strikeout;

            _maxChars += uint(Utf8CharCount( chunk->Text() ));
            _maxChars += state.underline;
            _maxChars += state.strikeout;

            if ( not _first )   _first = chunk;
            else                last_chunk->next = chunk;
            last_chunk = chunk;
        }};
        //---------------------------------------------------------------------------



        // set default state
        {
            State&  def_state   = states.emplace_back();
            def_state.color     = HtmlColor::White;
            def_state.height    = DefaultTextHeight;
            def_state.type      = EChunkType::Initial;
        }

        usize   start = 0, pos = 0;

        for (; pos < str.length();)
        {
            const usize old_pos = pos;

            // search pattern '[<key>...]' or '[/<key>]'
            const CharUtf32 c = Utf8Decode( str, INOUT pos );

            if_unlikely( c == '[' )
            {
                const usize     tmp     = pos;
                const CharUtf32 n       = Utf8Decode( str, INOUT pos );
                const State     state   = states.back();
                bool            processed;

                if ( n == '/' )
                    processed = ParseClosingTag( INOUT pos );
                else {
                    pos = tmp;
                    processed = ParseOpeningTag( INOUT pos );
                }

                if ( processed )
                {
                    AddChunk( str.data() + start, old_pos - start, state );
                    start = pos;
                    continue;
                }
            }
        }

        AddChunk( str.data() + start, pos - start, states.back() );

        //ASSERT( states.size() == 1 and states.back().type == EChunkType::Initial );

        return *this;
    }

/*
=================================================
    ToString
=================================================
*/
    U8String  FormattedText::ToString () C_NE___
    {
        TRY{
            return _ToString();
        }
        CATCH_ALL(
            return Default;
        )
    }

    U8String  FormattedText::_ToString () C_Th___
    {
        struct StackState
        {
            EChunkType      type;
            RGBA8u          color;
            uint            height;

            StackState () {}
            StackState (EChunkType type, RGBA8u color, uint height) : type{type}, color{color}, height{height} {}
        };

        U8String            result;
        Array<StackState>   stack;
        Chunk               state = {};

        const auto  CloseTag = [&] (Chunk const* chunk)
        {
            for (; stack.size();)
            {
                switch_enum( stack.back().type )
                {
                    case EChunkType::Bold :
                        if ( state.bold and (state.bold != chunk->bold) ) {
                            result << u8"[/b]";
                            stack.pop_back();
                            break;
                        }
                        return;

                    case EChunkType::Italic :
                        if ( state.italic and (state.italic != chunk->italic) ) {
                            result << u8"[/i]";
                            stack.pop_back();
                            break;
                        }
                        return;

                    case EChunkType::Underline :
                        if ( state.underline and (state.underline != chunk->underline) ) {
                            result << u8"[/u]";
                            stack.pop_back();
                            break;
                        }
                        return;

                    case EChunkType::Strikeout :
                        if ( state.strikeout and (state.strikeout != chunk->strikeout) ) {
                            result << u8"[/s]";
                            stack.pop_back();
                            break;
                        }
                        return;

                    case EChunkType::Style :
                        if ( (state.color != chunk->color) or (state.height != chunk->height) ) {
                            result << u8"[/style]";
                            stack.pop_back();
                            break;
                        }
                        return;

                    case EChunkType::Unknown :
                    case EChunkType::Initial :
                    default :
                        return;
                }
                switch_end
            }
        };

        const auto  HexToString = [] (INOUT U8String &str, ubyte x)
        {
            str << CharUtf8( (x >>  4) > 9 ? ('A' + ((x >>  4)-10)) : ('0' + (x >>  4)) );
            str << CharUtf8( (x & 0xF) > 9 ? ('A' + ((x & 0xF)-10)) : ('0' + (x & 0xF)) );
        };
        //---------------------------------------------------------------------------


        state.color  = HtmlColor::White;
        state.height = DefaultTextHeight;

        result.reserve( 1u << 10 );
        stack.emplace_back( EChunkType::Initial, state.color, uint(state.height) );

        for (auto* chunk = GetFirst(); chunk; chunk = chunk->next)
        {
            CloseTag( chunk );

            // open tag
            if ( chunk->bold and (state.bold != chunk->bold) ) {
                stack.emplace_back( EChunkType::Bold, chunk->color, chunk->height );
                result << u8"[b]";
            }

            if ( chunk->italic and (state.italic != chunk->italic) ) {
                stack.emplace_back( EChunkType::Italic, chunk->color, chunk->height );
                result << u8"[i]";
            }

            if ( chunk->underline and (state.underline != chunk->underline) ) {
                stack.emplace_back( EChunkType::Underline, chunk->color, chunk->height );
                result << u8"[u]";
            }

            if ( chunk->strikeout and (state.strikeout != chunk->strikeout) ) {
                stack.emplace_back( EChunkType::Strikeout, chunk->color, chunk->height );
                result << u8"[s]";
            }

            if ( (stack.back().color != chunk->color) or (stack.back().height != chunk->height) )
            {
                result << u8"[style";
                if ( stack.back().color != chunk->color ) {
                    result << u8" color=#";
                    HexToString( INOUT result, chunk->color.r );
                    HexToString( INOUT result, chunk->color.g );
                    HexToString( INOUT result, chunk->color.b );
                    HexToString( INOUT result, chunk->color.a );
                }
                if ( stack.back().height != chunk->height )
                {
                    auto    tmp = Base::ToString( chunk->height );
                    result << u8" size=" << U8StringView{ Cast<CharUtf8>(tmp.data()), tmp.size() };
                }
                result << u8']';
                stack.emplace_back( EChunkType::Style, chunk->color, chunk->height );
            }

            result << U8StringView{ chunk->string, chunk->length };

            state = *chunk;
        }

        ASSERT( stack.size() == 1 and stack.back().type == EChunkType::Initial );
        return result;
    }

/*
=================================================
    Clear
=================================================
*/
    void  FormattedText::Clear () __NE___
    {
        _alloc.Release();
        _first      = null;
        _maxChars   = 0;
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  FormattedText::Serialize (Serializing::Serializer &ser) C_NE___
    {
        bool    result;
        uint    count  = 0;
        for (Chunk const* chunk = _first; chunk; chunk = chunk->next, ++count)
        {}

        result = ser.stream.Write( _maxChars ) and
                 ser.stream.Write( count );

        for (Chunk const* chunk = _first; result and (chunk != null); chunk = chunk->next)
        {
            const uint  str_length = chunk->length;

            result = ser.stream.Write( str_length ) and
                     ser.stream.Write( &chunk->color, Bytes{sizeof(Chunk) - offsetof(Chunk, color) + str_length} );
        }
        return result;
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  FormattedText::Deserialize (Serializing::Deserializer &des) __NE___
    {
        bool    result;
        uint    count  = 0;

        _first      = null;
        _maxChars   = 0;
        _alloc.Discard();

        result = des.stream.Read( OUT _maxChars ) and
                 des.stream.Read( OUT count );

        for (uint i = 0; result and (i < count); ++i)
        {
            uint    str_length = 0;
            result = des.stream.Read( OUT str_length );

            void*   ptr     = _alloc.Allocate( SizeAndAlign{ Bytes{sizeof(Chunk) + str_length}, Bytes{alignof(Chunk)} });
            CHECK_ERR( ptr != null );

            Chunk*  chunk   = PlacementNew<Chunk>( OUT ptr );

            result = result and des.stream.Read( &chunk->color, Bytes{sizeof(Chunk) - offsetof(Chunk, color) + str_length} );
        }
        return result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Serialize
=================================================
*/
    bool  PrecalculatedFormattedText::Serialize (Serializing::Serializer &ser) C_NE___
    {
        return ser( _text, _spacing, _wordWrap );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  PrecalculatedFormattedText::Deserialize (Serializing::Deserializer &des) __NE___
    {
        return des( _text, _spacing, _wordWrap );
    }


} // AE::Graphics
