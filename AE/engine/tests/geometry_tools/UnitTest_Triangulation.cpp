// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../shared/UnitTest_Shared.h"
#include "geometry_tools/Triangulation/CDT_Triangulator.h"

namespace
{
#ifdef AE_ENABLE_CDT
	using namespace AE::GeometryTools;

	using CDTFlags = CDT_Triangulator::EFlags;


	static void  CDT_Triangulation_Test1 ()
	{
		//     (1,2)
		//            (2,1)
		//  (0,0)
		const float2	in_vertices [] = { float2{0.f, 0.f}, float2{2.f, 1.f}, float2{1.f, 2.f} };

		CDT_Triangulator::OutVertices_t		vertices;
		CDT_Triangulator::OutIndices_t		indices;

		TEST( CDT_Triangulator::Triangulate( in_vertices,
											 CDTFlags::ConformingDelaunayTriangulation | CDTFlags::ResolveIntersections | CDTFlags::LineStrip,
											 OUT vertices, OUT indices ));

		TEST( vertices.size() == 3 );
		TEST( indices.size() == 3 );
		TEST( ArrayView<float2>{in_vertices} == vertices );
		TEST( ArrayView<uint>{ 2, 0, 1 } == indices );
	}


	static void  CDT_Triangulation_Test2 ()
	{
		//     (1,2)
		//            (2,1)
		//  (0,0)
		const float2	in_vertices [] = { float2{0.f, 0.f}, float2{2.f, 1.f}, float2{1.f, 2.f} };
		const uint		in_indices []  = { 0, 1, 2 };

		CDT_Triangulator::OutVertices_t		vertices;
		CDT_Triangulator::OutIndices_t		indices;

		TEST( CDT_Triangulator::Triangulate( in_vertices, in_indices,
											 CDTFlags::ConformingDelaunayTriangulation | CDTFlags::ResolveIntersections | CDTFlags::IndexedLineStrip,
											 OUT vertices, OUT indices ));

		TEST( vertices.size() == 3 );
		TEST( indices.size() == 3 );
		TEST( ArrayView<float2>{in_vertices} == vertices );
		TEST( ArrayView<uint>{ 2, 0, 1 } == indices );
	}


	static void  CDT_Triangulation_Test3 ()
	{
		//     (1,2)
		//            (2,1)
		//  (0,0)
		const float2	in_vertices [] = { float2{0.f, 0.f}, float2{2.f, 1.f}, float2{1.f, 2.f} };
		const uint		in_indices []  = {	0, 1,
											1, 2,
											2, 0 };

		CDT_Triangulator::OutVertices_t		vertices;
		CDT_Triangulator::OutIndices_t		indices;

		TEST( CDT_Triangulator::Triangulate( in_vertices, in_indices,
											 CDTFlags::ConformingDelaunayTriangulation | CDTFlags::ResolveIntersections | CDTFlags::IndexedLineList,
											 OUT vertices, OUT indices ));

		TEST( vertices.size() == 3 );
		TEST( indices.size() == 3 );
		TEST( ArrayView<float2>{in_vertices} == vertices );
		TEST( ArrayView<uint>{ 2, 0, 1 } == indices );
	}


	static void  CDT_Triangulation_Test4 ()
	{
		//         5(2,4)
		//         2(2,2)
		//
		//    1(1,1)     3(3,1)
		//  0(0,0)         4(4,0)

		const float2	in_vertices [] = { float2{0.f, 0.f}, float2{1.2f, 1.f}, float2{2.f, 2.f}, float2{2.8f, 1.f}, float2{4.f, 0.f}, float2{2.f, 4.f} };
		const uint		in_indices []  = {	0,5,  5,4,  4,0,
											1,2,  2,3,  3,1 };

		CDT_Triangulator::OutVertices_t		vertices;
		CDT_Triangulator::OutIndices_t		indices;

		TEST( CDT_Triangulator::Triangulate( in_vertices, in_indices,
											 CDTFlags::ConformingDelaunayTriangulation | CDTFlags::ResolveIntersections | CDTFlags::IndexedLineList,
											 OUT vertices, OUT indices ));

		TEST( vertices.size() == 6 );
		TEST( ArrayView<float2>{in_vertices} == vertices );
		TEST( indices.size() == 18 );
		TEST( ArrayView<uint>{ 3,1,0,  1,5,0,  5,1,2,  3,4,5,  3,5,2,  3,0,4 } == indices );
	}


	static void  CDT_Triangulation_Test5 ()
	{
		//         5(2,4)
		//         2(2,2)
		//
		//    1(1,1)     3(3,1)
		//  0(0,0)         4(4,0)

		const float2	in_vertices [] = { float2{0.f, 0.f}, float2{1.2f, 1.f}, float2{2.f, 2.f}, float2{2.8f, 1.f}, float2{4.f, 0.f}, float2{2.f, 4.f} };
		const uint		in_indices []  = {	0,5,  5,4,  4,0,
											1,2,  2,3,  3,1 };

		CDT_Triangulator::OutVertices_t			vertices;
		CDT_Triangulator::OutIndices_t			indices;
		CDT_Triangulator::OutBoundaryEdges_t	boundary;

		TEST( CDT_Triangulator::Triangulate( in_vertices, in_indices,
											 CDTFlags::ConformingDelaunayTriangulation | CDTFlags::ResolveIntersections | CDTFlags::IndexedLineList,
											 OUT vertices, OUT indices, OUT &boundary ));

		TEST( vertices.size() == 6 );
		TEST( ArrayView<float2>{in_vertices} == vertices );

		TEST( indices.size() == 18 );
		TEST( ArrayView<uint>{ 3,1,0,  1,5,0,  5,1,2,  3,4,5,  3,5,2,  3,0,4 } == indices );

		TEST( boundary.size() == 12 );
		TEST( ArrayView<uint>{ 3,1,  5,0,  1,2,  4,5,  2,3,  0,4 } == boundary );
	}

#endif // AE_ENABLE_CDT
}

extern void UnitTest_Triangulation ()
{
  #ifdef AE_ENABLE_CDT
	CDT_Triangulation_Test1();
	CDT_Triangulation_Test2();
	CDT_Triangulation_Test3();
	CDT_Triangulation_Test4();
	CDT_Triangulation_Test5();
  #endif

	TEST_PASSED();
}
