// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/GeomSource/IGeomSource.h"
#include "res_editor/Dynamic/DynamicScalar.h"
#include "res_editor/Dynamic/DynamicVec.h"

namespace AE::ResEditor
{

	//
	// Unified Geometry Source
	//

	class UnifiedGeometry final : public IGeomSource
	{
		friend class ScriptUniGeometry;

	// types
	private:
		class Material final : public IGSMaterials
		{
		// types
		private:
			using PplnID_t		= Union< NullUnion, GraphicsPipelineID, MeshPipelineID >;
			using PplnPerDraw_t	= Array< PplnID_t >;


		// variables
		public:
			RenderTechPipelinesPtr		rtech;

			PplnPerDraw_t				pplns;
			PerFrameDescSet_t			descSets;
			
			DescSetBinding				passDSIndex;
			DescSetBinding				mtrDSIndex;
			
			Strong<BufferID>			ubuffer;


		// methods
		public:
			Material () {}
			~Material ();
		};


		//-------------------------------------------------------
		struct DrawCmd2 : Graphics::DrawCmd
		{
			RC<DynamicUInt>		dynVertexCount;
			RC<DynamicUInt>		dynInstanceCount;
		};

		struct DrawIndexedCmd2 : Graphics::DrawIndexedCmd
		{
			EIndex				indexType			= Default;
			RC<Buffer>			indexBufferPtr;
			Bytes				indexBufferOffset;
			RC<DynamicUInt>		dynIndexCount;
			RC<DynamicUInt>		dynInstanceCount;
		};

		struct DrawIndirectCmd2 : Graphics::DrawIndirectCmd
		{
			RC<Buffer>			indirectBufferPtr;
			RC<DynamicUInt>		dynDrawCount;
		};

		struct DrawIndexedIndirectCmd2 : Graphics::DrawIndexedIndirectCmd
		{
			EIndex				indexType			= Default;
			RC<Buffer>			indexBufferPtr;
			Bytes				indexBufferOffset;
			RC<Buffer>			indirectBufferPtr;
			RC<DynamicUInt>		dynDrawCount;
		};
		
		struct DrawMeshTasksCmd2
		{
			RC<DynamicUInt3>	dynTaskCount;
			uint3				taskCount;
		};

		struct DrawMeshTasksIndirectCmd2 : Graphics::DrawMeshTasksIndirectCmd
		{
			RC<Buffer>			indirectBufferPtr;
			RC<DynamicUInt>		dynDrawCount;
		};

		struct DrawIndirectCountCmd2 : Graphics::DrawIndirectCountCmd
		{
			RC<Buffer>			indirectBufferPtr;
			RC<Buffer>			countBufferPtr;
			RC<DynamicUInt>		dynMaxDrawCount;
		};

		struct DrawIndexedIndirectCountCmd2 : Graphics::DrawIndexedIndirectCountCmd
		{
			EIndex				indexType			= Default;
			RC<Buffer>			indexBufferPtr;
			Bytes				indexBufferOffset;
			RC<Buffer>			indirectBufferPtr;
			RC<Buffer>			countBufferPtr;
			RC<DynamicUInt>		dynMaxDrawCount;
		};

		struct DrawMeshTasksIndirectCountCmd2 : Graphics::DrawMeshTasksIndirectCountCmd
		{
			RC<Buffer>			indirectBufferPtr;
			RC<Buffer>			countBufferPtr;
			RC<DynamicUInt>		dynMaxDrawCount;
		};

		using DrawCommand_t		= Union< DrawCmd2, DrawIndexedCmd2, DrawIndirectCmd2, DrawIndexedIndirectCmd2,
										 DrawMeshTasksCmd2, DrawMeshTasksIndirectCmd2, DrawIndirectCountCmd2,
										 DrawIndexedIndirectCountCmd2, DrawMeshTasksIndirectCountCmd2 >;
		using DrawCommands_t	= Array< DrawCommand_t >;
		//-------------------------------------------------------


	// variables
	private:
		Buffers_t			_meshes;
		Textures_t			_textures;
		DrawCommands_t		_drawCommands;


	// methods
	public:
		UnifiedGeometry (Renderer &r)										__Th___;
		~UnifiedGeometry ();


	// IGeomSource //
		void  StateTransition (IGSMaterials &, GraphicsCtx_t &)				__NE_OV;
		bool  Draw (const DrawData &)										__NE_OV;
		bool  Update (const UpdateData &)									__NE_OV;
	};


} // AE::ResEditor
