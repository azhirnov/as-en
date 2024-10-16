// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/SphericalCube/SphericalCubeMath.h"

namespace AE::GeometryTools
{

	//
	// Spherical Cube Generator
	//

	class SphericalCubeGen final : public SphericalCubeProjection< SCProj2_Spherical< SCProj1_Tangential >,
																   SCProj2_Texture< SCProj1_Identity >>
	{
	// types
	public:
		struct Vertex
		{
			packed_short4	position;	// position == normal, w - face/5
			packed_short4	texcoord;
			packed_short4	tangent;
			packed_short4	bitangent;
		};
		StaticAssert( sizeof(Vertex) == 32 );

		struct UnpackedVertex
		{
			float3		position;	// in normalized coords, position == normal
			float3		texcoord;
			float3		tangent;
			float3		bitangent;

			UnpackedVertex () = default;
			UnpackedVertex (const Vertex &v);
		};

		using Index = uint;		// TODO: ushort ?

	private:
		static constexpr uint	FaceCount	= 6;
		static constexpr uint	MaxLod		= 32;


	// variables
	protected:
		Array<Vertex>		_vertices;
		Array<Index>		_indices;

		uint				_minLod			= 0;
		uint				_maxLod			= 0;
		bool				_quads			= false;	// quads for tessellation


	// methods
	public:
		ND_	bool  Create (uint minLod, uint maxLod, Bool quads, Bool cubeMap)				__NE___;

			bool  GetVertices (uint lod, OUT ArrayView<Vertex> &result)						C_NE___;
			bool  GetIndices (uint lod, OUT ArrayView<Index> &result)						C_NE___;

			bool  GetVertices (uint lod, uint face, OUT ArrayView<Vertex> &result)			C_NE___;
			bool  GetIndices (uint lod, uint face, OUT ArrayView<Index> &result)			C_NE___;

		ND_ static uint2 CalcFaceVertCount2 (uint lod)										__NE___;
		ND_ static uint  CalcFaceVertCount (uint lod)										__NE___;
		ND_ static uint  CalcVertCount (uint lod)											__NE___;

		ND_ static uint  CalcFaceIndexCount (uint lod, Bool useQuads)						__NE___;
		ND_ static uint  CalcIndexCount (uint lod, Bool useQuads)							__NE___;

		ND_ static bool  RayCast (const float3 &center, float radius, const float3 &begin,
								  const float3 &end, OUT float3 &outIntersection)			__NE___;

		ND_ ArrayView<Vertex>	GetVertices ()												C_NE___	{ return _vertices; }
		ND_ ArrayView<Index>	GetIndices ()												C_NE___	{ return _indices; }
		ND_ uint2				LODRange ()													C_NE___	{ return uint2{ _minLod, _maxLod }; }
		ND_ bool				IsQuads ()													C_NE___	{ return _quads; }
	};


} // AE::GeometryTools
