// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/Monitor.h"

namespace AE::App
{

/*
=================================================
    Print
=================================================
*/
    void  Monitor::Print () C_NE___
    {
        AE_LOGI(
            "\n"
            "Monitor:      '"s << StringView{name} << "'\n"
            "ID:           " << ToString( uint(id) ) << "\n"
            "workArea:     " << ToString( workArea.pixels ) << " pix\n"
            "region:       " << ToString( region.pixels ) << " pix\n"
            "physicalSize: " << ToString( physicalSize.meters ) << " meters\n"
            "ppi:          " << ToString( ppi ) << " pixels/inch\n"
            "frequency:    " << ToString( freq ) << " Hz\n"
            //"orientation:  " << 
            "external:     " << ToString( isExternal ) << "\n"
        );
    }


} // AE::App
