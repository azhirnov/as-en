// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

#include "res_loaders/Intermediate/IntermMesh.h"
#include "res_loaders/CSVMesh/CSVMeshLoader.h"

using namespace AE::Graphics;
using namespace AE::ResLoader;

namespace
{
	static void  CSVMeshLoader_TestRenderDocFormat1 ()
	{
		const char	csv[] = R"(VTX, IDX, in_Position.x, in_Position.y, in_Position.z, in_Position.w, in_Texcoord.x, in_Texcoord.y, in_Texcoord.z, in_Texcoord.w, in_Tangent.x, in_Tangent.y, in_Tangent.z, in_Tangent.w, in_BiTangent.x, in_BiTangent.y, in_BiTangent.z, in_BiTangent.w
0, 0, 0.5773, 0.5773, 0.5773, 0.0000, 0.5773, 0.5773, 0.5773, 0.0000, -0.4082, -0.4082, 0.8165, 0.0000, 0.4082, -0.8165, 0.4082, 0.0000
1, 3, 0.6513, 0.5365, 0.5365, 0.0000, 0.6225, 0.5534, 0.5534, 0.0000, -0.4141, -0.3411, 0.8438, 0.0000, 0.4141, -0.8438, 0.3411, 0.0000
2, 1, 0.6110, 0.6110, 0.5033, 0.0000, 0.5987, 0.5987, 0.5322, 0.0000, -0.3559, -0.3559, 0.8641, 0.0000, 0.4716, -0.7916, 0.3885, 0.0000
3, 0, 0.5773, 0.5773, 0.5773, 0.0000, 0.5773, 0.5773, 0.5773, 0.0000, -0.4082, -0.4082, 0.8165, 0.0000, 0.4082, -0.8165, 0.4082, 0.0000
4, 2, 0.6110, 0.5033, 0.6110, 0.0000, 0.5987, 0.5322, 0.5987, 0.0000, -0.4716, -0.3885, 0.7916, 0.0000, 0.3559, -0.8641, 0.3559, 0.0000
5, 3, 0.6513, 0.5365, 0.5365, 0.0000, 0.6225, 0.5534, 0.5534, 0.0000, -0.4141, -0.3411, 0.8438, 0.0000, 0.4141, -0.8438, 0.3411, 0.0000
6, 1, 0.6110, 0.6110, 0.5033, 0.0000, 0.5987, 0.5987, 0.5322, 0.0000, -0.3559, -0.3559, 0.8641, 0.0000, 0.4716, -0.7916, 0.3885, 0.0000
)";

		MemRefRStream	stream{ csv, Sizeof(csv) };
		CSVMeshLoader	loader;
		IntermMesh		mesh;

		TEST( loader.LoadMesh( OUT mesh, stream, Default ));

		TEST( mesh.IsValid() );
		TEST( mesh.Topology() == EPrimitive::TriangleList );
		TEST( mesh.IndexType() == EIndex::UInt );
		TEST_Eq( mesh.VertexCount(), 4 );
		TEST_Eq( mesh.IndexCount(), 7 );

		auto*	attribs = mesh.Attribs();
		TEST( attribs != null );
		TEST_Eq( attribs->Vertices().size(), 4 );
		TEST( attribs->Vertices().contains( VertexAttributeName::Position ));
		TEST( attribs->Vertices().contains( VertexAttributeName::TextureUVs[0] ));
		TEST( attribs->Vertices().contains( VertexAttributeName::Tangent ));
		TEST( attribs->Vertices().contains( VertexAttributeName::BiTangent ));

		{
			StructView<float4>	arr = mesh.GetData< float4 >( VertexAttributeName::Position );
			TEST( arr.size() == 4 );
			TEST( All( Equal( arr[0], float4{0.5773f, 0.5773f, 0.5773f, 0.0000f} )));
			TEST( All( Equal( arr[1], float4{0.6110f, 0.6110f, 0.5033f, 0.0000f} )));
			TEST( All( Equal( arr[2], float4{0.6110f, 0.5033f, 0.6110f, 0.0000f} )));
			TEST( All( Equal( arr[3], float4{0.6513f, 0.5365f, 0.5365f, 0.0000f} )));
		}{
			StructView<float4>	arr = mesh.GetData< float4 >( VertexAttributeName::TextureUVs[0] );
			TEST( arr.size() == 4 );
			TEST( All( Equal( arr[0], float4{0.5773f, 0.5773f, 0.5773f, 0.0000f} )));
			TEST( All( Equal( arr[1], float4{0.5987f, 0.5987f, 0.5322f, 0.0000f} )));
			TEST( All( Equal( arr[2], float4{0.5987f, 0.5322f, 0.5987f, 0.0000f} )));
			TEST( All( Equal( arr[3], float4{0.6225f, 0.5534f, 0.5534f, 0.0000f} )));
		}{
			StructView<float4>	arr = mesh.GetData< float4 >( VertexAttributeName::Tangent );
			TEST( arr.size() == 4 );
			TEST( All( Equal( arr[0], float4{-0.4082f, -0.4082f, 0.8165f, 0.0000f} )));
			TEST( All( Equal( arr[1], float4{-0.3559f, -0.3559f, 0.8641f, 0.0000f} )));
			TEST( All( Equal( arr[2], float4{-0.4716f, -0.3885f, 0.7916f, 0.0000f} )));
			TEST( All( Equal( arr[3], float4{-0.4141f, -0.3411f, 0.8438f, 0.0000f} )));
		}{
			StructView<float4>	arr = mesh.GetData< float4 >( VertexAttributeName::BiTangent );
			TEST( arr.size() == 4 );
			TEST( All( Equal( arr[0], float4{0.4082f, -0.8165f, 0.4082f, 0.0000f} )));
			TEST( All( Equal( arr[1], float4{0.4716f, -0.7916f, 0.3885f, 0.0000f} )));
			TEST( All( Equal( arr[2], float4{0.3559f, -0.8641f, 0.3559f, 0.0000f} )));
			TEST( All( Equal( arr[3], float4{0.4141f, -0.8438f, 0.3411f, 0.0000f} )));
		}{
			ArrayView<uint>		arr = mesh.GetIndexData<uint>();
			TEST( arr.size() == 7 );
			TEST( arr[0] == 0 );
			TEST( arr[1] == 3 );
			TEST( arr[2] == 1 );
			TEST( arr[3] == 0 );
			TEST( arr[4] == 2 );
			TEST( arr[5] == 3 );
			TEST( arr[6] == 1 );
		}
	}


	static void  CSVMeshLoader_TestNSightFormat1 ()
	{
		const char	csv[] = R"(IB Offset,Index,Index + Base,0 - POSITION0 (R16G16B16A16_SINT) : Component 0,0 - POSITION0 (R16G16B16A16_SINT) : Component 1,0 - POSITION0 (R16G16B16A16_SINT) : Component 2,0 - POSITION0 (R16G16B16A16_SINT) : Component 3,1 - NORMAL0 (R8G8B8A8_UINT) : Component 0,1 - NORMAL0 (R8G8B8A8_UINT) : Component 1,1 - NORMAL0 (R8G8B8A8_UINT) : Component 2,1 - NORMAL0 (R8G8B8A8_UINT) : Component 3,2 - TANGENT0 (R8G8B8A8_UINT) : Component 0,2 - TANGENT0 (R8G8B8A8_UINT) : Component 1,2 - TANGENT0 (R8G8B8A8_UINT) : Component 2,2 - TANGENT0 (R8G8B8A8_UINT) : Component 3,3 - TEXCOORD0 (R16G16_SNORM) : Component 0,3 - TEXCOORD0 (R16G16_SNORM) : Component 1
0,0,0,-3209,3240,-73,-57, 253,127,138,255, 127,254,127,128, 0.006,0.025
1,1,1,-3209,-3240,-73,-57, 253,127,138,255, 127,254,127,128, 0.257,0.025
2,2,2,-1557,-1556,-18624,-57, 253,127,138,255, 126,253,126,128, 0.192,0.742
3,2,2,-1557,-1556,-18624,-57, 253,127,138,255, 126,253,126,128, 0.192,0.742
4,3,3,-1557,1556,-18624,-57, 253,125,138,255, 128,253,127,128, 0.071,0.742
5,0,0,-3209,3240,-73,-57, 253,127,138,255, 127,254,127,128, 0.006,0.025
6,4,4,-3209,-3240,-73,-57, 127,253,138,255, 254,127,127,128, 0.486,0.742
)";
		MemRefRStream	stream{ csv, Sizeof(csv) };
		CSVMeshLoader	loader;
		IntermMesh		mesh;

		TEST( loader.LoadMesh( OUT mesh, stream, Default ));

		TEST( mesh.IsValid() );
		TEST( mesh.Topology() == EPrimitive::TriangleList );
		TEST( mesh.IndexType() == EIndex::UInt );
		TEST_Eq( mesh.VertexCount(), 5 );
		TEST_Eq( mesh.IndexCount(), 7 );

		auto*	attribs = mesh.Attribs();
		TEST( attribs != null );
		TEST_Eq( attribs->Vertices().size(), 4 );
		TEST( attribs->Vertices().contains( VertexAttributeName::Position ));
		TEST( attribs->Vertices().contains( VertexAttributeName::Normal ));
		TEST( attribs->Vertices().contains( VertexAttributeName::Tangent ));
		TEST( attribs->Vertices().contains( VertexAttributeName::TextureUVs[0] ));

		{
			StructView<short4>	arr = mesh.GetData< short4 >( VertexAttributeName::Position );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], short4{-3209,  3240, -73,    -57} )));
			TEST( All( Equal( arr[1], short4{-3209, -3240, -73,    -57} )));
			TEST( All( Equal( arr[2], short4{-1557, -1556, -18624, -57} )));
			TEST( All( Equal( arr[3], short4{-1557,  1556, -18624, -57} )));
			TEST( All( Equal( arr[4], short4{-3209, -3240, -73,    -57} )));
		}{
			StructView<ubyte4>	arr = mesh.GetData< ubyte4 >( VertexAttributeName::Normal );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], ubyte4{253, 127, 138, 255} )));
			TEST( All( Equal( arr[1], ubyte4{253, 127, 138, 255} )));
			TEST( All( Equal( arr[2], ubyte4{253, 127, 138, 255} )));
			TEST( All( Equal( arr[3], ubyte4{253, 125, 138, 255} )));
			TEST( All( Equal( arr[4], ubyte4{127, 253, 138, 255} )));
		}{
			StructView<ubyte4>	arr = mesh.GetData< ubyte4 >( VertexAttributeName::Tangent );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], ubyte4{127, 254, 127, 128} )));
			TEST( All( Equal( arr[1], ubyte4{127, 254, 127, 128} )));
			TEST( All( Equal( arr[2], ubyte4{126, 253, 126, 128} )));
			TEST( All( Equal( arr[3], ubyte4{128, 253, 127, 128} )));
			TEST( All( Equal( arr[4], ubyte4{254, 127, 127, 128} )));
		}{
			StructView<float2>	arr = mesh.GetData< float2 >( VertexAttributeName::TextureUVs[0] );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], float2{0.006f, 0.025f} )));
			TEST( All( Equal( arr[1], float2{0.257f, 0.025f} )));
			TEST( All( Equal( arr[2], float2{0.192f, 0.742f} )));
			TEST( All( Equal( arr[3], float2{0.071f, 0.742f} )));
			TEST( All( Equal( arr[4], float2{0.486f, 0.742f} )));
		}{
			ArrayView<uint>		arr = mesh.GetIndexData<uint>();
			TEST( arr.size() == 7 );
			TEST( arr[0] == 0 );
			TEST( arr[1] == 1 );
			TEST( arr[2] == 2 );
			TEST( arr[3] == 2 );
			TEST( arr[4] == 3 );
			TEST( arr[5] == 0 );
			TEST( arr[6] == 4 );
		}
	}


	static void  CSVMeshLoader_TestRenderDocFormat2 ()
	{
		const char	csv[] = R"(VTX, IDX, in_Position.x, in_Position.y, in_Position.z, in_Position.w, in_Texcoord.x, in_Texcoord.y, in_Texcoord.z, in_Texcoord.w, in_Tangent.x, in_Tangent.y, in_Tangent.z, in_Tangent.w, in_BiTangent.x, in_BiTangent.y, in_BiTangent.z, in_BiTangent.w
0, 0, 0.5773, 0.5773, 0.5773, 0.0000, 0.5773, 0.5773, 0.5773, 0.0000, -0.4082, -0.4082, 0.8165, 0.0000, 0.4082, -0.8165, 0.4082, 0.0000
1, 3, 0.6513, 0.5365, 0.5365, 0.0000, 0.6225, 0.5534, 0.5534, 0.0000, -0.4141, -0.3411, 0.8438, 0.0000, 0.4141, -0.8438, 0.3411, 0.0000
2, 1, 0.6110, 0.6110, 0.5033, 0.0000, 0.5987, 0.5987, 0.5322, 0.0000, -0.3559, -0.3559, 0.8641, 0.0000, 0.4716, -0.7916, 0.3885, 0.0000
3, 0, 0.5773, 0.5773, 0.5773, 0.0000, 0.5773, 0.5773, 0.5773, 0.0000, -0.4082, -0.4082, 0.8165, 0.0000, 0.4082, -0.8165, 0.4082, 0.0000
4, 2, 0.6110, 0.5033, 0.6110, 0.0000, 0.5987, 0.5322, 0.5987, 0.0000, -0.4716, -0.3885, 0.7916, 0.0000, 0.3559, -0.8641, 0.3559, 0.0000
5, 3, 0.6513, 0.5365, 0.5365, 0.0000, 0.6225, 0.5534, 0.5534, 0.0000, -0.4141, -0.3411, 0.8438, 0.0000, 0.4141, -0.8438, 0.3411, 0.0000
6, 1, 0.6110, 0.6110, 0.5033, 0.0000, 0.5987, 0.5987, 0.5322, 0.0000, -0.3559, -0.3559, 0.8641, 0.0000, 0.4716, -0.7916, 0.3885, 0.0000
)";

		MemRefRStream	stream{ csv, Sizeof(csv) };
		CSVMeshLoader	loader;
		IntermMesh		mesh2;

		TEST( loader.LoadMesh( OUT mesh2, stream, Default ));

		IntermMesh		mesh;
		TEST( mesh2.ConvertToFloatPointFormat( OUT mesh ));

		TEST( mesh.IsValid() );
		TEST( mesh.Topology() == EPrimitive::TriangleList );
		TEST( mesh.IndexType() == EIndex::UInt );
		TEST_Eq( mesh.VertexCount(), 4 );
		TEST_Eq( mesh.IndexCount(), 7 );

		auto*	attribs = mesh.Attribs();
		TEST( attribs != null );
		TEST_Eq( attribs->Vertices().size(), 4 );
		TEST( attribs->Vertices().contains( VertexAttributeName::Position ));
		TEST( attribs->Vertices().contains( VertexAttributeName::TextureUVs[0] ));
		TEST( attribs->Vertices().contains( VertexAttributeName::Tangent ));
		TEST( attribs->Vertices().contains( VertexAttributeName::BiTangent ));

		float3	fp;
		{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::Position );
			TEST( arr.size() == 4 );
			fp = arr[0];	TEST( All( Equal( fp, float3{0.5773f, 0.5773f, 0.5773f} )));
			fp = arr[1];	TEST( All( Equal( fp, float3{0.6110f, 0.6110f, 0.5033f} )));
			fp = arr[2];	TEST( All( Equal( fp, float3{0.6110f, 0.5033f, 0.6110f} )));
			fp = arr[3];	TEST( All( Equal( fp, float3{0.6513f, 0.5365f, 0.5365f} )));
		}{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::TextureUVs[0] );
			TEST( arr.size() == 4 );
			fp = arr[0];	TEST( All( Equal( fp, float3{0.5773f, 0.5773f, 0.5773f} )));
			fp = arr[1];	TEST( All( Equal( fp, float3{0.5987f, 0.5987f, 0.5322f} )));
			fp = arr[2];	TEST( All( Equal( fp, float3{0.5987f, 0.5322f, 0.5987f} )));
			fp = arr[3];	TEST( All( Equal( fp, float3{0.6225f, 0.5534f, 0.5534f} )));
		}{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::Tangent );
			TEST( arr.size() == 4 );
			fp = arr[0];	TEST( All( Equal( fp, float3{-0.4082f, -0.4082f, 0.8165f} )));
			fp = arr[1];	TEST( All( Equal( fp, float3{-0.3559f, -0.3559f, 0.8641f} )));
			fp = arr[2];	TEST( All( Equal( fp, float3{-0.4716f, -0.3885f, 0.7916f} )));
			fp = arr[3];	TEST( All( Equal( fp, float3{-0.4141f, -0.3411f, 0.8438f} )));
		}{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::BiTangent );
			TEST( arr.size() == 4 );
			fp = arr[0];	TEST( All( Equal( fp, float3{0.4082f, -0.8165f, 0.4082f} )));
			fp = arr[1];	TEST( All( Equal( fp, float3{0.4716f, -0.7916f, 0.3885f} )));
			fp = arr[2];	TEST( All( Equal( fp, float3{0.3559f, -0.8641f, 0.3559f} )));
			fp = arr[3];	TEST( All( Equal( fp, float3{0.4141f, -0.8438f, 0.3411f} )));
		}{
			ArrayView<uint>		arr = mesh.GetIndexData<uint>();
			TEST( arr.size() == 7 );
			TEST( arr[0] == 0 );
			TEST( arr[1] == 3 );
			TEST( arr[2] == 1 );
			TEST( arr[3] == 0 );
			TEST( arr[4] == 2 );
			TEST( arr[5] == 3 );
			TEST( arr[6] == 1 );
		}
	}


	static void  CSVMeshLoader_TestNSightFormat2 ()
	{
		const char	csv[] = R"(IB Offset,Index,Index + Base,0 - POSITION0 (R16G16B16A16_SINT) : Component 0,0 - POSITION0 (R16G16B16A16_SINT) : Component 1,0 - POSITION0 (R16G16B16A16_SINT) : Component 2,0 - POSITION0 (R16G16B16A16_SINT) : Component 3,1 - NORMAL0 (R8G8B8A8_UINT) : Component 0,1 - NORMAL0 (R8G8B8A8_UINT) : Component 1,1 - NORMAL0 (R8G8B8A8_UINT) : Component 2,1 - NORMAL0 (R8G8B8A8_UINT) : Component 3,2 - TANGENT0 (R8G8B8A8_UINT) : Component 0,2 - TANGENT0 (R8G8B8A8_UINT) : Component 1,2 - TANGENT0 (R8G8B8A8_UINT) : Component 2,2 - TANGENT0 (R8G8B8A8_UINT) : Component 3,3 - TEXCOORD0 (R16G16_SNORM) : Component 0,3 - TEXCOORD0 (R16G16_SNORM) : Component 1
0,0,0,-3209,3240,-73,-57, 253,127,138,255, 127,254,127,128, 0.006,0.025
1,1,1,-3209,-3240,-73,-57, 253,127,138,255, 127,254,127,128, 0.257,0.025
2,2,2,-1557,-1556,-18624,-57, 253,127,138,255, 126,253,126,128, 0.192,0.742
3,2,2,-1557,-1556,-18624,-57, 253,127,138,255, 126,253,126,128, 0.192,0.742
4,3,3,-1557,1556,-18624,-57, 253,125,138,255, 128,253,127,128, 0.071,0.742
5,0,0,-3209,3240,-73,-57, 253,127,138,255, 127,254,127,128, 0.006,0.025
6,4,4,-3209,-3240,-73,-57, 127,253,138,255, 254,127,127,128, 0.486,0.742
)";
		MemRefRStream	stream{ csv, Sizeof(csv) };
		CSVMeshLoader	loader;
		IntermMesh		mesh2;

		TEST( loader.LoadMesh( OUT mesh2, stream, Default ));

		IntermMesh		mesh;
		TEST( mesh2.ConvertToFloatPointFormat( OUT mesh ));

		TEST( mesh.IsValid() );
		TEST( mesh.Topology() == EPrimitive::TriangleList );
		TEST( mesh.IndexType() == EIndex::UInt );
		TEST_Eq( mesh.VertexCount(), 5 );
		TEST_Eq( mesh.IndexCount(), 7 );

		auto*	attribs = mesh.Attribs();
		TEST( attribs != null );
		TEST_Eq( attribs->Vertices().size(), 4 );
		TEST( attribs->Vertices().contains( VertexAttributeName::Position ));
		TEST( attribs->Vertices().contains( VertexAttributeName::Normal ));
		TEST( attribs->Vertices().contains( VertexAttributeName::Tangent ));
		TEST( attribs->Vertices().contains( VertexAttributeName::TextureUVs[0] ));

	//	3,0518509475997192297128208258309e-5

		{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::Position );
			TEST( arr.size() == 5 );
			float3	fp;
			float	err = 1.0e-4f;
			fp = arr[0];	TEST( All( Equal( fp, float3{-0.09793f,  0.09887f, -0.00222f}, err )));
			fp = arr[1];	TEST( All( Equal( fp, float3{-0.09793f, -0.09887f, -0.00222f}, err )));
			fp = arr[2];	TEST( All( Equal( fp, float3{-0.04751f, -0.04748f, -0.56837f}, err )));
			fp = arr[3];	TEST( All( Equal( fp, float3{-0.04751f,  0.04748f, -0.56837f}, err )));
			fp = arr[4];	TEST( All( Equal( fp, float3{-0.09793f, -0.09887f, -0.00222f}, err )));
		}
		/*{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::Normal );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], ubyte4{253, 127, 138, 255} )));
			TEST( All( Equal( arr[1], ubyte4{253, 127, 138, 255} )));
			TEST( All( Equal( arr[2], ubyte4{253, 127, 138, 255} )));
			TEST( All( Equal( arr[3], ubyte4{253, 125, 138, 255} )));
			TEST( All( Equal( arr[4], ubyte4{127, 253, 138, 255} )));
		}{
			StructView<float3>	arr = mesh.GetData< float3 >( VertexAttributeName::Tangent );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], ubyte4{127, 254, 127, 128} )));
			TEST( All( Equal( arr[1], ubyte4{127, 254, 127, 128} )));
			TEST( All( Equal( arr[2], ubyte4{126, 253, 126, 128} )));
			TEST( All( Equal( arr[3], ubyte4{128, 253, 127, 128} )));
			TEST( All( Equal( arr[4], ubyte4{254, 127, 127, 128} )));
		}{
			StructView<float2>	arr = mesh.GetData< float2 >( VertexAttributeName::TextureUVs[0] );
			TEST( arr.size() == 5 );
			TEST( All( Equal( arr[0], float2{0.006f, 0.025f} )));
			TEST( All( Equal( arr[1], float2{0.257f, 0.025f} )));
			TEST( All( Equal( arr[2], float2{0.192f, 0.742f} )));
			TEST( All( Equal( arr[3], float2{0.071f, 0.742f} )));
			TEST( All( Equal( arr[4], float2{0.486f, 0.742f} )));
		}*/
		{
			ArrayView<uint>		arr = mesh.GetIndexData<uint>();
			TEST( arr.size() == 7 );
			TEST( arr[0] == 0 );
			TEST( arr[1] == 1 );
			TEST( arr[2] == 2 );
			TEST( arr[3] == 2 );
			TEST( arr[4] == 3 );
			TEST( arr[5] == 0 );
			TEST( arr[6] == 4 );
		}
	}
}


extern void  UnitTest_CSVMeshLoader ()
{
	CSVMeshLoader_TestRenderDocFormat1();
	CSVMeshLoader_TestRenderDocFormat2();

	CSVMeshLoader_TestNSightFormat1();
	CSVMeshLoader_TestNSightFormat2();

	TEST_PASSED();
}
