// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS

#include "SphericalCubeGen.h"
#include "graphics/Public/IDs.h"

namespace AE::GeometryTools
{
	using namespace AE::Graphics;


	//
	// Spherical Cube Renderer
	//

	class SphericalCubeRenderer final : public SphericalCubeProjection< TangentialSphericalCube, TextureProjection >
	{
	// variables
	private:
		Strong<BufferID>	_vertexBuffer;
		Strong<BufferID>	_indexBuffer;

		uint				_minLod			= 0;
		uint				_maxLod			= 0;
		bool				_quads			= false;


	// methods
	public:
		//SphericalCube () {}
		//~SphericalCube ();

		//bool  Create (const CommandBuffer &cmdbuf, uint minLod, uint maxLod, bool quads);
		//void  Destroy (const FrameGraph &fg);
		
		ND_ bool  IsCreated () const	{ return _vertexBuffer and _indexBuffer; }

		//void  Draw (IDrawContext &ctx, uint lod) const;

		//	bool  GetVertexBuffer (uint lod, uint face, OUT RawBufferID &id, OUT BytesU &offset, OUT BytesU &size, OUT uint2 &vertCount) const;
		//	bool  GetIndexBuffer (uint lod, uint face, OUT RawBufferID &id, OUT BytesU &offset, OUT BytesU &size, OUT uint &indexCount) const;

		//ND_ bool  RayCast (const float3 &center, float radius, const float3 &begin, const float3 &end, OUT float3 &outIntersection) const;

		//ND_ static VertexInputState  GetAttribs ();
	};


} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
