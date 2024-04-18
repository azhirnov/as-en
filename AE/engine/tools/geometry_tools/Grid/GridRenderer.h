// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS
# include "geometry_tools/Grid/GridGen.h"

namespace AE::GeometryTools
{
	using namespace AE::Graphics;


	//
	// Grid Renderer
	//

	class GridRenderer
	{
	// types
	public:
		using Vertex	= GridGen::Vertex;
		using Index		= GridGen::Index;


	// variables
	private:
		Strong<BufferID>	_vertexBuffer;
		Strong<BufferID>	_indexBuffer;
		uint				_indexCount		= 0;


	// methods
	public:
		GridRenderer ()																		__NE___	{}
		~GridRenderer ()																	__NE___;

		ND_ bool  Create (IResourceManager &, ITransferContext &ctx, GfxMemAllocatorPtr,
						  uint vertsPerEdge, uint patchSize = 3)							__NE___;
			void  Destroy (IResourceManager &)												__NE___;

		ND_ bool  IsCreated ()																C_NE___	{ return _vertexBuffer and _indexBuffer; }

			template <typename DrawCtx>
			void  Draw (DrawCtx &ctx, uint instanceCount = 1, uint firstInstance = 0)		C_NE___;
	};


/*
=================================================
	Draw
=================================================
*/
	template <typename DrawCtx>
	void  GridRenderer::Draw (DrawCtx &ctx, uint instanceCount, uint firstInstance) C_NE___
	{
		ctx.BindVertexBuffer( 0, _vertexBuffer, 0_b );
		ctx.BindIndexBuffer( _indexBuffer, 0_b, EIndex::UShort );

		DrawIndexedCmd	cmd;
		cmd.indexCount		= _indexCount;
		cmd.instanceCount	= instanceCount;
		cmd.firstIndex		= 0;
		cmd.vertexOffset	= 0;
		cmd.firstInstance	= firstInstance;

		ctx.DrawIndexed( cmd );
	}


} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
