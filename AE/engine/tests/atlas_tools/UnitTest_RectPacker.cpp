// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../shared/UnitTest_Shared.h"
#include "atlas_tools/RectPacker.h"
using namespace AE::AtlasTools;

namespace
{
    static void  RectPacker_Test1 ()
    {
        RectPacker  packer;

        packer.Add( uint2{16,16} );
        packer.Add( uint2{16,32} );
        packer.Add( uint2{16,8} );
        packer.Add( uint2{4,2} );
        packer.Add( uint2{8,4} );
        packer.Add( uint2{14,14} );
        packer.Add( uint2{8,16} );

        TEST( packer.Pack() );

        AE_LOGI( "Size "s << ToString( packer.TargetSize() ) << ", rate: " << ToString( packer.PackingRate() ));
    }
}


extern void UnitTest_RectPacker ()
{
    RectPacker_Test1();

    TEST_PASSED();
}
