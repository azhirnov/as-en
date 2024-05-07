// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Does not require Metal headers.
*/

#include "graphics/Public/RayTracingEnums.h"

namespace AE::Graphics
{

/*
=================================================
	MEnumCast (ERTInstanceOpt) -> MTLAccelerationStructureInstanceOptions
=================================================
*/
	ND_ inline uint  MEnumCast (ERTInstanceOpt value) __NE___
	{
		constexpr uint	OptionDisableTriangleCulling				= 1 << 0;
		constexpr uint	TriangleFrontFacingWindingCounterClockwise	= 1 << 1;
		constexpr uint	OptionOpaque								= 1 << 2;
		constexpr uint	OptionNonOpaque								= 1 << 3;

		uint	result = 0;

		for (auto t : BitfieldIterate( value ))
		{
			switch_enum( t )
			{
				case ERTInstanceOpt::TriangleCullDisable :	result |= OptionDisableTriangleCulling;					break;
				case ERTInstanceOpt::TriangleFrontCCW :		result |= TriangleFrontFacingWindingCounterClockwise;	break;
				case ERTInstanceOpt::ForceOpaque :			result |= OptionOpaque;									break;
				case ERTInstanceOpt::ForceNonOpaque :		result |= OptionNonOpaque;								break;

				case ERTInstanceOpt::All :
				case ERTInstanceOpt::_Last :
				case ERTInstanceOpt::Unknown :
				default_unlikely :							RETURN_ERR( "unknown RT instance options", 0 );
			}
			switch_end
		}
		return result;
	}


} // AE::Graphics
