// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Impl/EnumToString.cpp.h"
#include "video/Public/VideoDecoder.h"

namespace AE::Video
{

/*
=================================================
    Properties::ToString
=================================================
*/
    String  IVideoDecoder::Properties::ToString () C_Th___
    {
        String  str;
        for (auto& info : streams)
        {
            str << "\n  codecName:    " << StringView{info.codecName}
                << "\n  index:  . . . " << Base::ToString( info.index )
                << "\n  type:         " << Base::ToString( info.type )
                << "\n  codec:  . . . " << Base::ToString( info.codec )
                << "\n  format:       " << Base::ToString( info.format )
                << "\n  colorPreset:  " << Base::ToString( info.colorPreset )
                << "\n  frameCount:   " << Base::ToString( info.frameCount )
                << "\n  duration: . . " << Base::ToString_HMS( info.duration.GetScaled() )
                << "\n  avgFrameRate: " << Base::ToString( info.avgFrameRate )
                << "\n  minFrameRate: " << Base::ToString( info.minFrameRate )
                << "\n  bitrate:      " << Base::ToString( info.bitrate.GetScaled() )
                << "\n  dimension:  . " << Base::ToString( info.dimension )
                << "\n--------";
        }
        return str;
    }

/*
=================================================
    Properties::GetStream
=================================================
*/
    IVideoDecoder::StreamInfo const*  IVideoDecoder::Properties::GetStream (const int idx) C_NE___
    {
        for (auto& s : streams)
        {
            if_unlikely( s.index == idx )
                return &s;
        }
        return null;
    }


} // AE::Video
