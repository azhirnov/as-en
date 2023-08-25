// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Does not require Metal headers.
*/

#ifdef AE_ENABLE_METAL
# include "graphics/Public/RayTracingEnums.h"

namespace AE::Graphics
{

/*
=================================================
    MEnumCast (ERTInstanceOpt) -> MTLAccelerationStructureInstanceOptions
=================================================
*/
    ND_ inline uint  MEnumCast (ERTInstanceOpt value) __NE___
    {
        constexpr uint  OptionDisableTriangleCulling                = 1 << 0;
        constexpr uint  TriangleFrontFacingWindingCounterClockwise  = 1 << 1;
        constexpr uint  OptionOpaque                                = 1 << 2;
        constexpr uint  OptionNonOpaque                             = 1 << 3;

        uint    result = 0;

        while ( value != Zero )
        {
            auto    t = ExtractBit( INOUT value );

            BEGIN_ENUM_CHECKS();
            switch ( t )
            {
                case ERTInstanceOpt::TriangleCullDisable :  result |= OptionDisableTriangleCulling;                 break;
                case ERTInstanceOpt::TriangleFrontCCW :     result |= TriangleFrontFacingWindingCounterClockwise;   break;
                case ERTInstanceOpt::ForceOpaque :          result |= OptionOpaque;                                 break;
                case ERTInstanceOpt::ForceNonOpaque :       result |= OptionNonOpaque;                              break;

                case ERTInstanceOpt::All :
                case ERTInstanceOpt::_Last :
                case ERTInstanceOpt::Unknown :
                default_unlikely :                          RETURN_ERR( "unknown RT instance options", 0 );
            }
            END_ENUM_CHECKS();
        }
        return result;
    }


} // AE::Graphics

#endif // AE_ENABLE_METAL
