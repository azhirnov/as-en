// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../shared/UnitTest_Shared.h"
#include "geometry_tools/SphericalCube/SphericalCubeMath.h"
using namespace AE::GeometryTools;

namespace
{
	template <typename Projection>
	void Test_ForwardInverseProjection ()
	{
		static constexpr uint	lod = 12;

		for (uint face = 0; face < 6; ++face)
		{
			for (uint y = 1; y < lod+2; ++y)
			for (uint x = 1; x < lod+2; ++x)
			{
				const double2  ncoord = ToSNorm( double2{x,y} / (lod+2) );

				const double2	fwd_2d	= Projection::Forward( ncoord );
				const double2	inv_2d	= Projection::Inverse( fwd_2d );

				TEST(Equal( ncoord.x, inv_2d.x, 1_pct ));
				TEST(Equal( ncoord.x, inv_2d.x, 1_pct ));

				const double3  forward = Projection::Forward( ncoord, ECubeFace(face) );

				auto[inv, inv_face] = Projection::Inverse( forward );

				TEST(Equal( ncoord.x, inv.x, 1_pct ));
				TEST(Equal( ncoord.y, inv.y, 1_pct ));
				TEST( uint(inv_face) == face );
			}
		}
	}
}

extern void UnitTest_SphericalCubeMath ()
{
	Test_ForwardInverseProjection< SCProj2_Cube< SCProj1_Identity >>();
	Test_ForwardInverseProjection< SCProj2_Spherical< SCProj1_Identity >>();
	Test_ForwardInverseProjection< SCProj2_Spherical< SCProj1_Tangential >>();
	Test_ForwardInverseProjection< SCProj2_Spherical< SCProj1_Everitt >>();
	Test_ForwardInverseProjection< SCProj2_Spherical< SCProj1_5thPoly >>();
	Test_ForwardInverseProjection< SCProj2_Spherical< SCProj1_COBE >>();
	Test_ForwardInverseProjection< SCProj2_Spherical< SCProj1_Arvo >>();

	TEST_PASSED();
}
