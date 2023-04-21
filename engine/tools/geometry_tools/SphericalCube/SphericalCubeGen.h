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

		using Index_t	= uint;


	// variables
	protected:
		Array<Vertex>	_vertices;
		Array<Index_t>	_indices;

		uint			_minLod			= 0;
		uint			_maxLod			= 0;
		bool			_quads			= false;


	// methods
	public:
		ND_	bool  Create (uint minLod, uint maxLod, bool quads)								__NE___;

			bool  GetVertices (uint lod, uint face, OUT ArrayView<Vertex> &result)			C_NE___;
			bool  GetIndices (uint lod, uint face, OUT ArrayView<Index_t> &result)			C_NE___;

		ND_ static uint  CalcFaceVertCount (uint lod)										__NE___;
		ND_ static uint  CalcVertCount (uint lod)											__NE___;
	
		ND_ static uint  CalcFaceIndexCount (uint lod, bool useQuads)						__NE___;
		ND_ static uint  CalcIndexCount (uint lod, bool useQuads)							__NE___;
		
		ND_ static bool  RayCast (const float3 &center, float radius, const float3 &begin,
								  const float3 &end, OUT float3 &outIntersection)			__NE___;

		ND_ ArrayView<Vertex>	GetVertices ()												C_NE___	{ return _vertices; }
		ND_ ArrayView<Index_t>	GetIndices ()												C_NE___	{ return _indices; }
	};


} // AE::GeometryTools
