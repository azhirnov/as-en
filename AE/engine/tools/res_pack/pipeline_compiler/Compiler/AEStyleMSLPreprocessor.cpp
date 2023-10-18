// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Compiler/AEStyleMSLPreprocessor.h"

namespace AE::PipelineCompiler
{
namespace {
    ND_ bool  IsPartOfWord (const char c)
    {
        return  (c == '_') |
                ((c >= 'a') & (c <= 'z')) |
                ((c >= 'A') & (c <= 'Z')) |
                ((c >= '0') & (c <= '9'));
    }
}

/*
=================================================
    constructor
=================================================
*/
    AEStyleMSLPreprocessor::AEStyleMSLPreprocessor ()
    {
        // TODO
    }

/*
=================================================
    Process
=================================================
*/
    bool  AEStyleMSLPreprocessor::Process (EShader, const PathAndLine &, usize headerLines, StringView inStr, OUT String &outStr)
    {
        usize   hdr_size = 0;
        Parser::MoveToLine( inStr, INOUT hdr_size, headerLines );

        StringView  header = inStr.substr( 0, hdr_size );
        StringView  source = inStr.substr( hdr_size );
        //ASSERT( (String{header} << source) == inStr );

        outStr.reserve( inStr.size() );
        outStr = source;

        for (auto& [src, dst] : _typeMap)
        {
            for (usize i = 0; i < outStr.size();)
            {
                usize   pos = outStr.find( src, i );
                if_unlikely( pos == StringView::npos )
                    break;

                const char  c0 = outStr[ pos - 1 ];
                const char  c1 = outStr[ pos + src.size() ];

                if ( IsPartOfWord( c0 ) or IsPartOfWord( c1 ) or (c0 == '.') )
                {
                    i = pos + src.size();
                    continue;
                }

                outStr.replace( pos, src.length(), dst.data() );
                i = pos + dst.length();
            }
        }

        header >> outStr;
        return true;
    }


} // AE::PipelineCompiler
