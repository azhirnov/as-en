// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/Parser.h"
#include "base/Math/Math.h"

namespace AE::Base
{

/*
=================================================
    ToEndOfLine
=================================================
*/
    void  Parser::ToEndOfLine (StringView str, INOUT usize &pos) __NE___
    {
        if ( pos < str.length() and ((str[pos] == '\n') | (str[pos] == '\r')) )
            return;

        while ( pos < str.length() )
        {
            const char  n = (pos+1) >= str.length() ? 0 : str[pos+1];

            ++pos;

            if_unlikely( (n == '\n') | (n == '\r') )
                return;
        }
    }

/*
=================================================
    ToBeginOfLine
=================================================
*/
    void  Parser::ToBeginOfLine (StringView str, INOUT usize &pos) __NE___
    {
        pos = Min( pos, str.length() );

        while ( pos <= str.length() )
        {
            const char  p = (pos-1) >= str.length() ? '\0' : str[pos-1];

            if_unlikely( (p == '\n') | (p == '\r') | (p == '\0') )
                return;

            --pos;
        }
        pos = 0;
    }

/*
=================================================
    IsBeginOfLine
=================================================
*/
    bool  Parser::IsBeginOfLine (StringView str, const usize pos) __NE___
    {
        usize   p = pos;
        ToBeginOfLine( str, INOUT p );
        return p == pos;
    }

/*
=================================================
    IsEndOfLine
=================================================
*/
    bool  Parser::IsEndOfLine (StringView str, const usize pos) __NE___
    {
        usize   p = pos;
        ToEndOfLine( str, INOUT p );
        return p == pos;
    }

/*
=================================================
    ToNextLine
=================================================
*/
    void  Parser::ToNextLine (StringView str, INOUT usize &pos) __NE___
    {
        while ( pos < str.length() )
        {
            const char  c = str[pos];
            const char  n = (pos+1) >= str.length() ? 0 : str[pos+1];

            ++pos;

            // windows style "\r\n"
            if_unlikely( (c == '\r') & (n == '\n') )
            {
                ++pos;
                return;
            }

            // linux style "\n" (or mac style "\r")
            if_unlikely( (c == '\n') | (c == '\r') )
                return;
        }
    }

/*
=================================================
    ToPrevLine
=================================================
*/
    void  Parser::ToPrevLine (StringView str, INOUT usize &pos) __NE___
    {
        pos = Min( pos, str.length() );

        while ( pos <= str.length() )
        {
            const char  c = str[pos];
            const char  p = (pos-1) >= str.length() ? 0 : str[pos-1];

            --pos;

            // windows style "\r\n"
            if_unlikely( (p == '\r') & (c == '\n') )
            {
                --pos;
                return;
            }

            // linux style "\n" (or mac style "\r")
            if_unlikely( (p == '\n') | (p == '\r') )
                return;
        }
    }

/*
=================================================
    CalculateNumberOfLines
=================================================
*/
    usize  Parser::CalculateNumberOfLines (StringView str) __NE___
    {
        usize   lines = 0;

        for (usize pos = 0; pos < str.length();)
        {
            const char  c = str[pos];
            const char  n = (pos+1) >= str.length() ? 0 : str[pos+1];

            ++pos;

            // windows style "\r\n"
            if_unlikely( (c == '\r') & (n == '\n') )
            {
                if ( lines == 0 and pos > 0 ) ++lines;
                ++lines;
                ++pos;
                continue;
            }

            // linux style "\n" (or mac style "\r")
            if_unlikely( (c == '\n') | (c == '\r') )
            {
                if ( lines == 0 and pos > 0 ) ++lines;
                ++lines;
            }
        }

        if ( lines == 0 and not str.empty() ) ++lines;

        return lines;
    }

/*
=================================================
    MoveToLine
=================================================
*/
    bool  Parser::MoveToLine (StringView str, INOUT usize &pos, usize lineNumber) __NE___
    {
        usize   lines = 0;

        for (; (pos < str.length()) & (lines < lineNumber); ++lines)
        {
            ToNextLine( str, INOUT pos );
        }
        return lines == lineNumber;
    }

/*
=================================================
    ReadCurrLine
---
    Read line from begin of line to end of line
    and move to next line.
    New line symbol is not included.
=================================================
*/
    void  Parser::ReadCurrLine (StringView str, INOUT usize &pos, OUT StringView &result) __NE___
    {
        ToBeginOfLine( str, INOUT pos );

        ReadLineToEnd( str, INOUT pos, OUT result );
    }

/*
=================================================
    ReadLineToEnd
----
    Read line from current position to end of line
    and move to next line.
    New line symbol is not included.
=================================================
*/
    void  Parser::ReadLineToEnd (StringView str, INOUT usize &pos, OUT StringView &result) __NE___
    {
        const usize prev_pos = pos;

        ToEndOfLine( str, INOUT pos );

        result = str.substr( prev_pos, pos - prev_pos );

        ToNextLine( str, INOUT pos );
    }

/*
=================================================
    ReadString
----
    read string from " to "
=================================================
*/
    bool  Parser::ReadString (StringView str, INOUT usize &pos, OUT StringView &result) __NE___
    {
        result = Default;

        for (; pos < str.length(); ++pos)
        {
            if_unlikely( str[pos] == '"' )
                break;
        }

        CHECK_ERR( str[pos] == '"' );

        const usize begin = ++pos;

        for (; pos < str.length(); ++pos)
        {
            const char  c = str[pos];

            if_unlikely( c == '"' )
            {
                result = StringView{ str.data() + begin, pos - begin };
                ++pos;
                return true;
            }
        }

        RETURN_ERR( "no pair for bracket \"" );
    }

/*
=================================================
    DivideLines
=================================================
*/
    void  Parser::DivideLines (StringView str, OUT Array<StringView> &lines) __Th___
    {
        lines.clear();

        usize   pos = 0;
        usize   prev = 0;

        while ( pos < str.length() )
        {
            ToEndOfLine( str, INOUT pos );

            if ( pos != prev ) {
                lines.push_back( str.substr( prev, pos-prev ));
            }

            ToNextLine( str, INOUT pos );

            prev = pos;
        }
    }

/*
=================================================
    Tokenize
=================================================
*/
    void  Parser::Tokenize (StringView str, const char divisor, OUT Array<StringView> &tokens) __Th___
    {
        usize   begin = 0;

        tokens.clear();

        for (usize i = 0; i < str.length(); ++i)
        {
            const char  c = str[i];

            if ( c == divisor )
            {
                tokens.push_back( StringView{ str.data() + begin, i - begin });
                begin = i+1;
            }
        }

        tokens.push_back( StringView{ str.data() + begin, str.length() - begin });
    }

//-----------------------------------------------------------------------------
namespace {


    //
    // C-Style Parser
    //

    struct CStyleParser
    {
    private:
        enum EMode {
            NONE,
            WORD,
            NUMBER,
            OPERATOR,
        };

        uint    _size;
        char    _prev;
        char    _prevPrev;
        EMode   _mode;

    public:
        CStyleParser ()                                                 __NE___;

        ND_ bool  IsBegin (char c)                                      __NE___;
        ND_ bool  IsEnd (char c)                                        __NE___;
        ND_ bool  IsUnused (char c)                                     __NE___;
        ND_ bool  OnUnknown (char)                                      __NE___ { return false; } // return true to continue parsing, false - to exit

        ND_ static bool  _IsWordBegin (char c)                          __NE___;
        ND_ static bool  _IsWord (char c)                               __NE___;
        ND_ static bool  _IsNumberBegin (char c)                        __NE___;
        ND_ static bool  _IsNumber (char c)                             __NE___;
        ND_ static bool  _IsOperator (char c)                           __NE___;
        ND_ static bool  _IsBinaryOperator (char p, char c)             __NE___;
        ND_ static bool  _IsTernaryOperator (char pp, char p, char c)   __NE___;
    };


/*
=================================================
    constructor
=================================================
*/
    inline CStyleParser::CStyleParser () __NE___ : _size{0}, _prev{' '}, _prevPrev{' '}, _mode{NONE}
    {}

/*
=================================================
    IsBegin
=================================================
*/
    inline bool  CStyleParser::IsBegin (char c) __NE___
    {
        _mode       = NONE;
        _size       = 0;
        _prevPrev   = _prev;
        _prev       = c;

        if ( _IsWordBegin( c ))     { _mode = WORD;     return true; }
        if ( _IsNumberBegin( c ))   { _mode = NUMBER;   return true; }
        if ( _IsOperator( c ))      { _mode = OPERATOR; return true; }

        return false;
    }

/*
=================================================
    IsEnd
=================================================
*/
    inline bool  CStyleParser::IsEnd (char c) __NE___
    {
        ++_size;

        switch ( _mode )
        {
            case WORD :
            {
                return not _IsWord( c );
            }
            case NUMBER :
            {
                return not _IsNumber( c );
            }
            case OPERATOR :
            {
                if ( _size == 1 )
                    return not _IsBinaryOperator( _prev, c );
                else
                if ( _size == 2 )
                    return not _IsTernaryOperator( _prevPrev, _prev, c );
                else
                    return true;
            }
            case NONE :
                return true;
        }

        _prevPrev   = _prev;
        _prev       = c;

        return true;
    }

/*
=================================================
    IsUnused
=================================================
*/
    inline bool  CStyleParser::IsUnused (char c) __NE___
    {
        return  (c == ' ') | (c == '\t') | (c == '@') | (c == '$') |
                (c == '\n') | (c == '\r')
                /*| (c == '\\')*/;
    }

/*
=================================================
    _IsWordBegin
=================================================
*/
    inline bool  CStyleParser::_IsWordBegin (char c) __NE___
    {
        return  ((c >= 'A') & (c <= 'Z')) |
                ((c >= 'a') & (c <= 'z')) |
                (c == '_');
    }

/*
=================================================
    _IsWord
=================================================
*/
    inline bool  CStyleParser::_IsWord (char c) __NE___
    {
        return  _IsWordBegin( c ) | _IsNumberBegin( c );
    }

/*
=================================================
    _IsNumberBegin
=================================================
*/
    inline bool  CStyleParser::_IsNumberBegin (char c) __NE___
    {
        return  (c >= '0') & (c <= '9');
    }

/*
=================================================
    _IsNumber
=================================================
*/
    inline bool  CStyleParser::_IsNumber (char c) __NE___
    {
        return  ((c >= '0') & (c <= '9')) |
                ((c >= 'A') & (c <= 'F')) |
                ((c >= 'a') & (c <= 'f')) |
                (c == '.')  | (c == 'x')  |
                (c == 'X');
    }

/*
=================================================
    _IsOperator
=================================================
*/
    inline bool  CStyleParser::_IsOperator (char c) __NE___
    {
        return  (c == '-') | (c == '+') | (c == '*') | (c == '/') |
                (c == '|') | (c == '&') | (c == '^') | (c == '<') |
                (c == '>') | (c == '?') | (c == ':') | (c == ';') |
                (c == ',') | (c == '.') | (c == '!') | (c == '~') |
                (c == '[') | (c == ']') | (c == '(') | (c == ')') |
                (c == '=') | (c == '%') | (c == '"') | (c == '#') |
                (c == '{') | (c == '}') | (c == '\'') | (c == '\\');
    }

/*
=================================================
    _IsBinaryOperator
=================================================
*/
    inline bool  CStyleParser::_IsBinaryOperator (char p, char c) __NE___
    {
    #   define PAIR_CMP( _pair_ )   (( (p == _pair_[0]) & (c == _pair_[1]) ))

        return  PAIR_CMP( "//" ) | PAIR_CMP( "/*" ) | PAIR_CMP( "*/" ) |
                PAIR_CMP( "&&" ) | PAIR_CMP( "||" ) | PAIR_CMP( "^^" ) |
                PAIR_CMP( "::" ) | PAIR_CMP( "==" ) | PAIR_CMP( "!=" ) |
                PAIR_CMP( ">=" ) | PAIR_CMP( "<=" ) | PAIR_CMP( "##" ) |
                PAIR_CMP( ">>" ) | PAIR_CMP( "<<" ) | PAIR_CMP( "|=" ) |
                PAIR_CMP( "&=" ) | PAIR_CMP( "^=" ) | PAIR_CMP( "+=" ) |
                PAIR_CMP( "-=" ) | PAIR_CMP( "*=" ) | PAIR_CMP( "/=" ) |
                PAIR_CMP( "%=" );

    #   undef PAIR_CMP
    }

/*
=================================================
    _IsTernaryOperator
=================================================
*/
    inline bool  CStyleParser::_IsTernaryOperator (char pp, char p, char c) __NE___
    {
    #   define TRIPLE_CMP( _triple_ )   (( (pp == _triple_[0]) & (p == _triple_[1]) & (c == _triple_[2]) ))

        return  TRIPLE_CMP( ">>=" ) | TRIPLE_CMP( "<<=" );

    #   undef TRIPLE_CMP
    }

} // namespace


/*
=================================================
    DivideString_CPP
=================================================
*/
    bool  Parser::DivideString_CPP (StringView str, OUT Array<StringView> &tokens) __Th___
    {
        CStyleParser    parser;

        bool    is_word = false;
        usize   begin   = 0;

        tokens.clear();

        for (usize i = 0; i < str.length(); ++i)
        {
            const char  c = str[i];

            if ( is_word )
            {
                if ( parser.IsEnd( c ))
                {
                    is_word = false;
                    tokens.push_back( StringView{ str.data() + begin, i - begin });
                    --i;
                }
                continue;
            }

            if ( parser.IsUnused( c ))
                continue;

            if ( parser.IsBegin( c ))
            {
                is_word = true;
                begin   = i;
                continue;
            }

            //if ( not parser.OnUnknown( c ))
            //  RETURN_ERR( "invalid char '"s << c << "'" );
        }

        if ( is_word )
            tokens.push_back( StringView{ str.data() + begin, str.length() - begin });

        return true;
    }

//-----------------------------------------------------------------------------
namespace {

    struct WordParser
    {
    private:
        enum EMode {
            NONE,
            WORD,
            NUMBER,
        };

        EMode   _mode;

    public:
        WordParser ()                   __NE___ : _mode(NONE) {}

        ND_ bool  IsBegin (char c)      __NE___;
        ND_ bool  IsEnd (char c)        __NE___;
        ND_ bool  IsUnused (char)       __NE___ { return false; }
        ND_ bool  OnUnknown (char)      __NE___ { return false; } // return true to continue parsing, false - to exit
    };

/*
=================================================
    IsBegin
=================================================
*/
    inline bool  WordParser::IsBegin (char c) __NE___
    {
        if ( (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') )
            _mode = WORD;
        else
        if ( c >= '0' and c <= '9' )
            _mode = NUMBER;
        else
            _mode = NONE;

        return true;
    }

/*
=================================================
    IsEnd
=================================================
*/
    inline bool  WordParser::IsEnd (char c) __NE___
    {
        switch ( _mode )
        {
            case WORD :     return not ((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or (c >= '0' and c <= '9'));
            case NUMBER :   return not (c >= '0' and c <= '9');
            case NONE :     break;
        }

        return true;
    }

} // namespace

/*
=================================================
    DivideString_Words
=================================================
*/
    bool  Parser::DivideString_Words (StringView str, OUT Array<StringView> &tokens) __Th___
    {
        WordParser  parser;

        bool    is_word = false;
        usize   begin   = 0;

        tokens.clear();

        for (usize i = 0; i < str.length(); ++i)
        {
            const char  c = str[i];

            if ( is_word )
            {
                if ( parser.IsEnd( c ))
                {
                    is_word = false;
                    tokens.push_back( StringView{ str.data() + begin, i - begin });
                    --i;
                }
                continue;
            }

            if ( parser.IsUnused( c ))
                continue;

            if ( parser.IsBegin( c ))
            {
                is_word = true;
                begin   = i;
                continue;
            }

            if ( not parser.OnUnknown( c )) {
                RETURN_ERR( "invalid char '"s << c << "'" );
            }
        }

        if ( is_word )
            tokens.push_back( StringView{ str.data() + begin, str.length() - begin });

        return true;
    }

/*
=================================================
    Preprocessor_CPP
=================================================
*/
namespace
{
    inline void  SkipSpaces (StringView str, INOUT usize &pos)
    {
        for (; pos < str.size(); ++pos)
        {
            const char  c = str[pos];
            if ( not ((c == ' ') | (c == '\t')) )
                break;
        }
    }
}
    void  Parser::Preprocessor_CPP (StringView str, ArrayView<StringView> defines, OUT Array<StringView> &result)
    {
        result.clear();

        int     macro_depth = 0;
        usize   begin_block = UMax;

        const auto CheckMacro = [str, defines, &begin_block, &result] (INOUT usize &pos)
        {{
            SkipSpaces( str, INOUT pos );

            if ( CStyleParser::_IsWordBegin( str[pos] ))
            {
                const usize begin = pos;

                // move to word end
                for (; CStyleParser::_IsWord( str[pos] ); ++pos) {}

                StringView  macro_name  = str.substr( begin, pos - begin );
                bool        include     = false;

                for (auto def : defines) {
                    if ( def == macro_name ) {
                        include = true;
                        break;
                    }
                }

                if ( not include and begin_block != UMax )
                {
                    usize tmp = pos;
                    ToPrevLine( str, INOUT tmp );

                    result.push_back( str.substr( begin_block, tmp - begin_block ));
                    begin_block = UMax;
                }

                if ( include )
                {
                    usize tmp = pos;
                    ToNextLine( str, INOUT tmp );
                    begin_block = tmp;
                }
            }
        }};

        for (usize pos = 1; pos < str.size();)
        {
            const char  c = str[pos-1];
            const char  n = str[pos];

            // single line comment
            if_unlikely( (c == '/') & (n == '/') )
            {
                ToNextLine( str, INOUT pos );
                continue;
            }

            // multi line comment
            if_unlikely( (c == '/') & (n == '*') )
            {
                pos += 2;
                for (; pos < str.size(); ++pos)
                {
                    const char  a = str[pos-1];
                    const char  b = str[pos];

                    if_unlikely( (a == '*') & (b == '/') )
                    {
                        pos += 2;
                        break;
                    }
                }
                continue;
            }

            // string
            if_unlikely( c == '"' )
            {
                ++pos;
                for (; pos < str.size(); ++pos)
                {
                    const char  a = str[pos-1];
                    const char  b = str[pos];

                    if_unlikely( (a != '\\') & (b == '"') )
                    {
                        pos += 2;
                        break;
                    }
                }
                continue;
            }

            // macros
            if_unlikely( (c != '#') & (n == '#') )
            {
                SkipSpaces( str, INOUT ++pos );

                if ( str.substr( pos, 5 ) == "ifdef" )
                {
                    ++macro_depth;
                    pos += 5;
                    CheckMacro( INOUT pos );
                }
                else
                if ( str.substr( pos, 2 ) == "if" )
                {
                    ++macro_depth;
                    pos += 2;
                    CheckMacro( INOUT pos );
                }
                else
                if ( str.substr( pos, 5 ) == "endif" )
                {
                    --macro_depth;
                    ASSERT( macro_depth >= 0 );

                    if ( begin_block != UMax )
                    {
                        usize tmp = pos;
                        ToPrevLine( str, INOUT tmp );

                        result.push_back( str.substr( begin_block, tmp - begin_block ));
                        begin_block = UMax;
                    }
                }
                #ifdef AE_DEBUG
                    else if ( str.substr( pos, 5 ) == "undef" )     {}
                    else if ( str.substr( pos, 6 ) == "pragma" )    {}
                    else if ( str.substr( pos, 9 ) == "extension" ) {}  // GLSL
                    else DBG_WARNING( "unknown macros" );
                #endif

                ToNextLine( str, INOUT pos );
                continue;
            }

            ++pos;
        }

        ASSERT( macro_depth == 0 );
        ASSERT( begin_block == UMax );
    }

/*
=================================================
    ValidateVarName_CPP
=================================================
*/
    void  Parser::ValidateVarName_CPP (StringView name, OUT String &result) __NE___
    {
        NOTHROW_ERRV( result.resize( name.size() ));

        for (usize i = 0; i < name.size(); ++i)
        {
            result[i] = CStyleParser::_IsWord( name[i] ) ? name[i] : '_';
        }
    }

    String  Parser::ValidateVarName_CPP (StringView name) __NE___
    {
        String  dst;
        ValidateVarName_CPP( name, OUT dst );
        return dst;
    }

/*
=================================================
    TabsToSpaces
=================================================
*/
    void  Parser::TabsToSpaces (OUT String &dst, StringView src, const uint tabSize) __Th___
    {
        dst.reserve( src.length() );  // throw

        for (usize i = 0, col = 0; i < src.size(); ++i)
        {
            const char  c = src[i];

            if_unlikely( c == '\n' )
            {
                col = 0;
                dst << c;  // throw
            }
            else
            if_unlikely( c == '\t' )
            {
                usize   j = col;
                col = ((col + tabSize) / tabSize) * tabSize;

                for (; j < col; ++j)
                    dst << ' ';  // throw
            }
            else
            {
                ++col;
                dst << c;  // throw
            }
        }
    }

    String  Parser::TabsToSpaces (StringView src, uint tabSize) __Th___
    {
        String  dst;
        TabsToSpaces( OUT dst, src, tabSize );
        return dst;
    }

/*
=================================================
    IsWhiteSpacesOnly
=================================================
*/
    bool  Parser::IsWhiteSpacesOnly (StringView str) __NE___
    {
        for (char c : str)
        {
            if_unlikely( (c != ' ') & (c != '\t') )
                return false;
        }
        return true;
    }


} // AE::Base
