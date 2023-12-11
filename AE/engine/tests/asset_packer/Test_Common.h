// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pipeline_compiler/PipelineCompiler.pch.h"
#include "pipeline_compiler/PipelineCompilerImpl.h"

#include "input_actions/InputActionsBinding.h"

#include "asset_packer/AssetPacker.pch.h"
#include "asset_packer/AssetPackerImpl.h"

#include "../shared/UnitTest_Shared.h"

using namespace AE::Base;
using namespace AE::PipelineCompiler;
using namespace AE::InputActions;
using namespace AE::AssetPacker;

using LinearAlloc_t = IAllocatorAdaptor< LinearAllocator<> >;


template <typename T>
ND_ inline String  ArrayToString (StringView name, const T &arr, const HashToName &hashToName)
{
    String  str;
    str << "\n" << name << " {";
    for (usize i = 0; i < arr.size(); ++i)
    {
        str << "\n  [" << ToString(i) << "]";
        str << arr[i].ToString( hashToName );
    }
    str << "\n}\n";
    return str;
}


ND_ inline bool  CompareWithDump (StringView serializedStr, const Path &refFilename, bool forceUpdate)
{
    bool        is_ok   = true;
    bool        update  = forceUpdate;
    {
        FileRStream rfile   {refFilename};
        String      ref_str;

        if ( rfile.IsOpen() )
        {
            TEST( rfile.Read( rfile.RemainingSize(), OUT ref_str ));
            is_ok = Parser::CompareLineByLine( serializedStr, ref_str,
                        [refFilename] (uint lline, StringView lstr, uint rline, StringView rstr) {
                            AE_LOGE( "in: "s << refFilename.string() << "\n\n"
                                        << "line mismatch:" << "\n(" << ToString( lline ) << "): " << lstr
                                        << "\n(" << ToString( rline ) << "): " << rstr );
                        },
                        [refFilename] () { AE_LOGE( "in: "s << refFilename.string() << "\n\n" << "sizes of dumps are not equal!" ); });
        }
        else
            update = true;
    }

    if ( update )
    {
        FileWStream wfile {refFilename};
        CHECK_ERR( wfile.IsOpen() );
        CHECK_ERR( wfile.Write( serializedStr ));
    }

    return is_ok;
}
