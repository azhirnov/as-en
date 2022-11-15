// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "SphericalCubeMath.h"

namespace AE::GeometryTools
{

	//
	// Spherical Cube Generator
	//

	class SphericalCubeGen : public SphericalCubeProjection< TangentialSphericalCube, TextureProjection >
	{
	// types
	public:
		struct Vertex
		{
			packed_float3	position;	// in normalized coords
			float			_padding1;
			packed_float3	texcoord;
			float			_padding2;
			//packed_float3	tangent;	// TODO: use it for distortion correction and for tessellation (instead of linear interpolation)

			Vertex () {}
			Vertex (const packed_float3 &pos, const packed_float3 &texc) : position{pos}, texcoord{texc} {}
		};
		STATIC_ASSERT( sizeof(Vertex) == 32 );


	// variables
	protected:
		Array<Vertex>	_vertices;
		Array<uint>		_indices;

		uint			_minLod			= 0;
		uint			_maxLod			= 0;
		bool			_quads			= false;


	// methods
	public:
		bool  Create (uint minLod, uint maxLod, bool quads);

		bool  GetVertices (uint lod, uint face, OUT ArrayView<Vertex> &result) const;
		bool  GetIndices (uint lod, uint face, OUT ArrayView<uint> &result) const;

		ND_ static uint  CalcFaceVertCount (uint lod);
		ND_ static uint  CalcVertCount (uint lod);
	
		ND_ static uint  CalcFaceIndexCount (uint lod, bool useQuads);
		ND_ static uint  CalcIndexCount (uint lod, bool useQuads);
		
		ND_ static bool  RayCast (const float3 &center, float radius, const float3 &begin, const float3 &end, OUT float3 &outIntersection);
	};


} // AE::GeometryTools
