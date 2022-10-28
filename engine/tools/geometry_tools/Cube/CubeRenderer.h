// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS

#include "CubeGen.h"
#include "graphics/Public/CommandBuffer.h"
#include "graphics/Public/ResourceManager.h"

namespace AE::GeometryTools
{
	using namespace AE::Graphics;


	//
	// Cube Renderer
	//

	class CubeRenderer
	{
	// types
	public:
		using Vertex	= CubeGen::Vertex;


	// variables
	private:
		Strong<BufferID>	_vertexBuffer;
		Strong<BufferID>	_indexBuffer;

		static const uint	_indexCount = 36;


	// methods
	public:
		CubeRenderer () {}
		~CubeRenderer ();

		ND_ bool  Create (IResourceManager &, ITransferContext &ctx);
			void  Destroy (IResourceManager &);

		ND_ bool  IsCreated () const	{ return _vertexBuffer and _indexBuffer; }

			template <typename DrawCtx>
			void  Draw (DrawCtx &ctx, uint instanceCount = 1, uint firstInstance = 0) const;
	};

	
	template <typename DrawCtx>
	void  CubeRenderer::Draw (DrawCtx &ctx, uint instanceCount, uint firstInstance) const
	{
		ctx.BindVertexBuffer( 0, _vertexBuffer, 0_b );
		ctx.BindIndexBuffer( _indexBuffer, 0_b, EIndex::UInt );

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
