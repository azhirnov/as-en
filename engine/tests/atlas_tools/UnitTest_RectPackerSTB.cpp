// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_STB

#include "../shared/UnitTest_Shared.h"
#include "atlas_tools/RectPackerSTB.h"
using namespace AE::AtlasTools;

namespace
{
    static void  RectPackerSTB_Test1 ()
    {
        RectPackerSTB   packer;

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


extern void UnitTest_RectPackerSTB ()
{
    RectPackerSTB_Test1();

    TEST_PASSED();
}

#endif // AE_ENABLE_STB
