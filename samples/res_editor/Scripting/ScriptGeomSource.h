// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptDynamicVars.h"

#include "res_editor/GeomSource/IGeomSource.h"

namespace AE::ResEditor
{

	//
	// Base Geometry Source
	//
	class ScriptGeomSource : public EnableScriptRC
	{
	// types
	public:
		using PipelineNames_t		= Array< PipelineName >;
		using CppStructsFromShaders	= ScriptBasePass::CppStructsFromShaders;
		

	// methods
	public:
		ScriptGeomSource () {}

		static void  Bind (const ScriptEnginePtr &se)													__Th___;

		ND_ virtual RC<IGeomSource>		ToGeomSource ()													__Th___ = 0;
		ND_ virtual PipelineNames_t		FindMaterialPipeline ()											C_Th___	= 0;
		ND_ virtual void				AddLayoutReflection ()											C_Th___	= 0;
		ND_ virtual RC<IGSMaterials>	ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)	C_Th___ = 0;

	protected:
		template <typename B>
		static void  _BindBase (B &binder)																__Th___;
	};



	//
	// Spherical Cube Geometry Source
	//
	class ScriptSphericalCube final : public ScriptGeomSource
	{
	// types
	private:
		struct TextureInfo
		{
			ScriptImagePtr		image;
			String				sampler;
		};
		using TextureMap_t	= FlatHashMap< String, TextureInfo >;


	// variables
	private:
		TextureMap_t		_textures;
		uint				_minLod		= 0;
		uint				_maxLod		= 0;

		RC<IGeomSource>		_geomSrc;


	// methods
	public:
		ScriptSphericalCube () {}

		void  AddTexture1 (const String &name, const ScriptImagePtr &tex)								__Th___;
		void  AddTexture2 (const String &name, const ScriptImagePtr &tex, const String &sampler)		__Th___;
		
		void  SetDetailLevel1 (uint maxLod)																__Th___;
		void  SetDetailLevel2 (uint minLod, uint maxLod)												__Th___;

		static void  Bind (const ScriptEnginePtr &se)													__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)										__Th___;
		
	// ScriptGeomSource //
		ND_ RC<IGeomSource>		ToGeomSource ()															__Th_OV;
		ND_ PipelineNames_t		FindMaterialPipeline ()													C_Th_OV;
		ND_ void				AddLayoutReflection ()													C_Th_OV {}
		ND_ RC<IGSMaterials>	ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)			C_Th_OV;

	private:
		ND_ static auto  _CreateUBType ()																__Th___;
	};



	//
	// Unified Geometry Source
	//
	class ScriptUniGeometry final : public ScriptGeomSource
	{
	// types
	public:
		struct DrawCmd3
		{
			ScriptDynamicUIntPtr	dynVertexCount;
			ScriptDynamicUIntPtr	dynInstanceCount;
			uint					vertexCount				= 0;
			uint					instanceCount			= 1;
			uint					firstVertex				= 0;
			uint					firstInstance			= 0;

			void  SetDynVertexCount (const ScriptDynamicUIntPtr &);
			void  SetDynInstanceCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndexedCmd3
		{
			EIndex					indexType				= Default;
			ScriptBufferPtr			indexBuffer;
			ulong					indexBufferOffset		= 0;
			ScriptDynamicUIntPtr	dynIndexCount;
			ScriptDynamicUIntPtr	dynInstanceCount;
			uint					indexCount				= 0;
			uint					instanceCount			= 1;
			uint					firstIndex				= 0;
			int						vertexOffset			= 0;
			uint					firstInstance			= 0;
			
			void  SetDynIndexCount (const ScriptDynamicUIntPtr &);
			void  SetDynInstanceCount (const ScriptDynamicUIntPtr &);
			void  SetIndexBuffer1 (EIndex type, const ScriptBufferPtr &ibuf);
			void  SetIndexBuffer2 (EIndex type, const ScriptBufferPtr &ibuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndirectCmd3
		{
			ScriptBufferPtr			indirectBuffer;
			ulong					indirectBufferOffset	= 0;
			uint					drawCount				= 1;
			ScriptDynamicUIntPtr	dynDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndirectCommand);
			
			void  SetDynDrawCount (const ScriptDynamicUIntPtr &);
			void  SetIndirectBuffer1 (const ScriptBufferPtr &ibuf);
			void  SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndexedIndirectCmd3
		{
			EIndex					indexType				= Default;
			ScriptBufferPtr			indexBuffer;
			ulong					indexBufferOffset		= 0;
			ScriptBufferPtr			indirectBuffer;
			ulong					indirectBufferOffset	= 0;
			uint					drawCount				= 1;
			ScriptDynamicUIntPtr	dynDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndexedIndirectCommand);
			
			void  SetDynDrawCount (const ScriptDynamicUIntPtr &);
			void  SetIndexBuffer1 (EIndex type, const ScriptBufferPtr &ibuf);
			void  SetIndexBuffer2 (EIndex type, const ScriptBufferPtr &ibuf, ulong offset);
			void  SetIndirectBuffer1 (const ScriptBufferPtr &ibuf);
			void  SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawMeshTasksCmd3
		{
			ScriptDynamicUInt3Ptr	dynTaskCount;
			packed_uint3			taskCount				{1};
			
			void  SetDynTaskCount (const ScriptDynamicUInt3Ptr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawMeshTasksIndirectCmd3
		{
			ScriptBufferPtr			indirectBuffer;
			ulong					indirectBufferOffset	= 0;
			uint					drawCount				= 1;
			ScriptDynamicUIntPtr	dynDrawCount;
			uint					stride					= sizeof(Graphics::DrawMeshTasksIndirectCommand);
			
			void  SetDynDrawCount (const ScriptDynamicUIntPtr &);
			void  SetIndirectBuffer1 (const ScriptBufferPtr &ibuf);
			void  SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndirectCountCmd3
		{
			ScriptBufferPtr			indirectBuffer;
			ulong					indirectBufferOffset	= 0;
			ScriptBufferPtr			countBuffer;
			ulong					countBufferOffset		= 0;
			uint					maxDrawCount			= 1;
			ScriptDynamicUIntPtr	dynMaxDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndirectCommand);
			
			void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);
			void  SetIndirectBuffer1 (const ScriptBufferPtr &ibuf);
			void  SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset);
			void  SetCountBuffer1 (const ScriptBufferPtr &cbuf);
			void  SetCountBuffer2 (const ScriptBufferPtr &cbuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndexedIndirectCountCmd3
		{
			EIndex					indexType				= Default;
			ScriptBufferPtr			indexBuffer;
			ulong					indexBufferOffset		= 0;
			ScriptBufferPtr			indirectBuffer;
			ulong					indirectBufferOffset	= 0;
			ScriptBufferPtr			countBuffer;
			ulong					countBufferOffset		= 0;
			uint					maxDrawCount			= 1;
			ScriptDynamicUIntPtr	dynMaxDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndexedIndirectCommand);
			
			void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);
			void  SetIndexBuffer1 (EIndex type, const ScriptBufferPtr &ibuf);
			void  SetIndexBuffer2 (EIndex type, const ScriptBufferPtr &ibuf, ulong offset);
			void  SetIndirectBuffer1 (const ScriptBufferPtr &ibuf);
			void  SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset);
			void  SetCountBuffer1 (const ScriptBufferPtr &cbuf);
			void  SetCountBuffer2 (const ScriptBufferPtr &cbuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawMeshTasksIndirectCountCmd3
		{
			ScriptBufferPtr			indirectBuffer;
			ulong					indirectBufferOffset	= 0;
			ScriptBufferPtr			countBuffer;
			ulong					countBufferOffset		= 0;
			uint					maxDrawCount			= 1;
			ScriptDynamicUIntPtr	dynMaxDrawCount;
			uint					stride					= sizeof(Graphics::DrawMeshTasksIndirectCommand);
			
			void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);
			void  SetIndirectBuffer1 (const ScriptBufferPtr &ibuf);
			void  SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset);
			void  SetCountBuffer1 (const ScriptBufferPtr &cbuf);
			void  SetCountBuffer2 (const ScriptBufferPtr &cbuf, ulong offset);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};


	private:
		using Meshes_t			= FlatHashMap< String, ScriptBufferPtr >;
		using Textures_t		= FlatHashMap< String, ScriptImagePtr >;
		using DrawCommand_t		= Union< DrawCmd3, DrawIndexedCmd3, DrawIndirectCmd3, DrawIndexedIndirectCmd3,
										 DrawMeshTasksCmd3, DrawMeshTasksIndirectCmd3, DrawIndirectCountCmd3,
										 DrawIndexedIndirectCountCmd3, DrawMeshTasksIndirectCountCmd3 >;
		using DrawCommands_t	= Array< DrawCommand_t >;


	// variables
	private:
		Meshes_t			_meshes;
		ScriptBufferPtr		_indexBuffer;
		Textures_t			_textures;
		DrawCommands_t		_drawCommands;

		RC<IGeomSource>		_geomSrc;


	// methods
	public:
		ScriptUniGeometry ();

		// geometry
		void  AddBuffer (const String &unName, const ScriptBufferPtr &buf)								__Th___;

		// material
		void  AddTexture (const String &unName, const ScriptImagePtr &img)								__Th___;

		// draw commands
		void  Draw1 (const DrawCmd3 &);
		void  Draw2 (const DrawIndexedCmd3 &);
		void  Draw3 (const DrawIndirectCmd3 &);
		void  Draw4 (const DrawIndexedIndirectCmd3 &);
		void  Draw5 (const DrawMeshTasksCmd3 &);
		void  Draw6 (const DrawMeshTasksIndirectCmd3 &);
		void  Draw7 (const DrawIndirectCountCmd3 &);
		void  Draw8 (const DrawIndexedIndirectCountCmd3 &);
		void  Draw9 (const DrawMeshTasksIndirectCountCmd3 &);


		static void  Bind (const ScriptEnginePtr &se)													__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)										__Th___;
		
	// ScriptGeomSource //
		ND_ RC<IGeomSource>		ToGeomSource ()															__Th_OV;
		ND_ PipelineNames_t		FindMaterialPipeline ()													C_Th_OV;
		ND_ void				AddLayoutReflection ()													C_Th_OV;
		ND_ RC<IGSMaterials>	ToMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)			C_Th_OV;

	private:
		ND_ static auto  _CreateUBType ()																__Th___;
	};


} // AE::ResEditor
