// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Scripting/ScriptPassArgs.h"

#include "res_editor/GeomSource/IGeomSource.h"

namespace AE::ResLoader {
	class IntermScene;
	class IntermVertexAttribs;
}

namespace AE::ResEditor
{

	//
	// Base Geometry Source
	//
	class ScriptGeomSource : public EnableScriptRC
	{
	// types
	public:
		using EDebugMode	= IPass::EDebugMode;

		struct PplnNameAndObjectId
		{
			PipelineName	pplnName;
			usize			objId		= 0;
			EDebugMode		dbgMode		= Default;

			PplnNameAndObjectId ()														= default;
			PplnNameAndObjectId (const PplnNameAndObjectId &)							= default;
			explicit PplnNameAndObjectId (PipelineName::Ref name)						: pplnName{name} {}
			PplnNameAndObjectId (PipelineName::Ref name, usize objId, EDebugMode dbg)	: pplnName{name}, objId{objId}, dbgMode{dbg} {}
		};

		using PipelineNames_t			= Array< PplnNameAndObjectId >;
		using CppStructsFromShaders		= ScriptBasePass::CppStructsFromShaders;

		using EGeometryType				= IGeomSource::ERTGeometryType;
		using RTScriptGeometryTypes_t	= StaticArray< ScriptRTGeometryPtr, uint(EGeometryType::_Count) >;
		using RTGeometryTypes_t			= StaticArray< RC<RTGeometry>, uint(EGeometryType::_Count) >;


	// variables
	protected:
		ScriptPassArgs		_args;


	// methods
	public:
		ScriptGeomSource ();

		// resources
		void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)										__Th___	{ _args.ArgSceneIn( name, scene ); }

		void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)											__Th___	{ _args.ArgBufferIn( name, buf ); }
		void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)											__Th___	{ _args.ArgBufferOut( name, buf ); }
		void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)										__Th___	{ _args.ArgBufferInOut( name, buf ); }

		void  ArgImageIn (const String &name, const ScriptImagePtr &img)											__Th___	{ _args.ArgImageIn( name, img ); }
		void  ArgImageOut (const String &name, const ScriptImagePtr &img)											__Th___	{ _args.ArgImageOut( name, img ); }
		void  ArgImageInOut (const String &name, const ScriptImagePtr &img)											__Th___	{ _args.ArgImageInOut( name, img ); }

		void  ArgImageArrIn (const String &name, const ScriptArray<ScriptImagePtr> &arr)							__Th___	{ _args.ArgImageArrIn( name, Array<ScriptImagePtr>{arr} ); }
		void  ArgImageArrOut (const String &name, const ScriptArray<ScriptImagePtr> &arr)							__Th___	{ _args.ArgImageArrOut( name, Array<ScriptImagePtr>{arr} ); }
		void  ArgImageArrInOut (const String &name, const ScriptArray<ScriptImagePtr> &arr)							__Th___	{ _args.ArgImageArrInOut( name, Array<ScriptImagePtr>{arr} ); }

		void  ArgTextureIn (const String &name, const ScriptImagePtr &tex)											__Th___	{ _args.ArgTextureIn( name, tex ); }
		void  ArgTextureIn2 (const String &name, const ScriptImagePtr &tex, const String &samplerName)				__Th___	{ _args.ArgTextureIn2( name, tex, samplerName ); }
		void  ArgTextureArrIn (const String &name, const ScriptArray<ScriptImagePtr> &arr)							__Th___	{ _args.ArgTextureArrIn( name, Array<ScriptImagePtr>{arr} ); }
		void  ArgTextureArrIn2 (const String &name, const ScriptArray<ScriptImagePtr> &arr, const String &sampName)	__Th___	{ _args.ArgTextureArrIn2( name, Array<ScriptImagePtr>{arr}, sampName ); }

		void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)			__Th___	{ _args.ArgVideoIn( name, tex, samplerName ); }

		static void  Bind (const ScriptEnginePtr &se)																__Th___;

		ND_ virtual ScriptRTGeometryPtr  GetRTGeometry (EGeometryType)												__Th___ { return null; }

		ND_ virtual RC<IGeomSource>		ToGeomSource ()																__Th___ = 0;
		ND_ virtual PipelineNames_t		FindMaterialGraphicsPipelines (ERenderLayer)								C_Th___	= 0;
			virtual void				AddLayoutReflection ()														C_Th___	{ _args.AddLayoutReflection(); }
		ND_ virtual RC<IGSMaterials>	ToMaterial (ERenderLayer, RenderTechPipelinesPtr, const PipelineNames_t &)	C_Th___ = 0;


	protected:
		template <typename B>
		static void  _BindBase (B &binder)																			__Th___;

		virtual void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)												C_Th___ = 0;
	};



	//
	// Spherical Cube Geometry Source
	//
	class ScriptSphericalCube final : public ScriptGeomSource
	{
	// variables
	private:
		uint				_minLod		= 0;
		uint				_maxLod		= 0;

		RC<IGeomSource>		_geomSrc;


	// methods
	public:
		ScriptSphericalCube () {}
		~ScriptSphericalCube ();

		void  SetDetailLevel1 (uint maxLod)																	__Th___;
		void  SetDetailLevel2 (uint minLod, uint maxLod)													__Th___;

		static void  Bind (const ScriptEnginePtr &se)														__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)											__Th___;

	// ScriptGeomSource //
		ND_ RC<IGeomSource>		ToGeomSource ()																__Th_OV;
		ND_ PipelineNames_t		FindMaterialGraphicsPipelines (ERenderLayer layer)							C_Th_OV;
		ND_ RC<IGSMaterials>	ToMaterial (ERenderLayer, RenderTechPipelinesPtr, const PipelineNames_t &)	C_Th_OV;

	private:
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)												C_Th_OV;

		ND_ static auto  _CreateUBType ()																	__Th___;
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
			String					_pplnHint;
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
			String					_pplnHint;
			EIndex					_indexType				= Default;
			ScriptBufferPtr			_indexBuffer;
			ulong					_indexBufferOffset		= 0;
			String					_indexBufferField;
			ScriptDynamicUIntPtr	dynIndexCount;
			ScriptDynamicUIntPtr	dynInstanceCount;
			uint					indexCount				= 0;
			uint					instanceCount			= 1;
			uint					firstIndex				= 0;
			int						vertexOffset			= 0;
			uint					firstInstance			= 0;

			void  SetDynIndexCount (const ScriptDynamicUIntPtr &);
			void  SetDynInstanceCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndirectCmd3
		{
			String					_pplnHint;
			ScriptBufferPtr			_indirectBuffer;
			ulong					_indirectBufferOffset	= 0;
			String					_indirectBufferField;
			uint					drawCount				= 1;
			ScriptDynamicUIntPtr	dynDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndirectCommand);

			void  SetDynDrawCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndexedIndirectCmd3
		{
			String					_pplnHint;
			EIndex					_indexType				= Default;
			ScriptBufferPtr			_indexBuffer;
			ulong					_indexBufferOffset		= 0;
			String					_indexBufferField;
			ScriptBufferPtr			_indirectBuffer;
			ulong					_indirectBufferOffset	= 0;
			String					_indirectBufferField;
			uint					drawCount				= 1;
			ScriptDynamicUIntPtr	dynDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndexedIndirectCommand);

			void  SetDynDrawCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawMeshTasksCmd3
		{
			String					_pplnHint;
			ScriptDynamicUInt3Ptr	dynTaskCount;
			packed_uint3			taskCount				{1};

			void  SetDynTaskCount (const ScriptDynamicUInt3Ptr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawMeshTasksIndirectCmd3
		{
			String					_pplnHint;
			ScriptBufferPtr			_indirectBuffer;
			ulong					_indirectBufferOffset	= 0;
			String					_indirectBufferField;
			uint					drawCount				= 1;
			ScriptDynamicUIntPtr	dynDrawCount;
			uint					stride					= sizeof(Graphics::DrawMeshTasksIndirectCommand);

			void  SetDynDrawCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndirectCountCmd3
		{
			String					_pplnHint;
			ScriptBufferPtr			_indirectBuffer;
			ulong					_indirectBufferOffset	= 0;
			String					_indirectBufferField;
			ScriptBufferPtr			_countBuffer;
			ulong					_countBufferOffset		= 0;
			String					_countBufferField;
			uint					maxDrawCount			= 1;
			ScriptDynamicUIntPtr	dynMaxDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndirectCommand);

			void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawIndexedIndirectCountCmd3
		{
			String					_pplnHint;
			EIndex					_indexType				= Default;
			ScriptBufferPtr			_indexBuffer;
			ulong					_indexBufferOffset		= 0;
			String					_indexBufferField;
			ScriptBufferPtr			_indirectBuffer;
			ulong					_indirectBufferOffset	= 0;
			String					_indirectBufferField;
			ScriptBufferPtr			_countBuffer;
			ulong					_countBufferOffset		= 0;
			String					_countBufferField;
			uint					maxDrawCount			= 1;
			ScriptDynamicUIntPtr	dynMaxDrawCount;
			uint					stride					= sizeof(Graphics::DrawIndexedIndirectCommand);

			void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};

		struct DrawMeshTasksIndirectCountCmd3
		{
			String					_pplnHint;
			ScriptBufferPtr			_indirectBuffer;
			ulong					_indirectBufferOffset	= 0;
			String					_indirectBufferField;
			ScriptBufferPtr			_countBuffer;
			ulong					_countBufferOffset		= 0;
			String					_countBufferField;
			uint					maxDrawCount			= 1;
			ScriptDynamicUIntPtr	dynMaxDrawCount;
			uint					stride					= sizeof(Graphics::DrawMeshTasksIndirectCommand);

			void  SetDynMaxDrawCount (const ScriptDynamicUIntPtr &);

			static void  Bind (const ScriptEnginePtr &se) __Th___;
		};


	private:
		using DrawCommand_t		= Union< DrawCmd3, DrawIndexedCmd3, DrawIndirectCmd3, DrawIndexedIndirectCmd3,
										 DrawMeshTasksCmd3, DrawMeshTasksIndirectCmd3, DrawIndirectCountCmd3,
										 DrawIndexedIndirectCountCmd3, DrawMeshTasksIndirectCountCmd3 >;
		using DrawCommands_t	= Array< DrawCommand_t >;


	// variables
	private:
		DrawCommands_t		_drawCommands;

		RC<IGeomSource>		_geomSrc;


	// methods
	public:
		ScriptUniGeometry () {}
		~ScriptUniGeometry ();

		// draw commands
		void  Draw1 (const DrawCmd3 &)																		__Th___;
		void  Draw2 (const DrawIndexedCmd3 &)																__Th___;
		void  Draw3 (const DrawIndirectCmd3 &)																__Th___;
		void  Draw4 (const DrawIndexedIndirectCmd3 &)														__Th___;
		void  Draw5 (const DrawMeshTasksCmd3 &)																__Th___;
		void  Draw6 (const DrawMeshTasksIndirectCmd3 &)														__Th___;
		void  Draw7 (const DrawIndirectCountCmd3 &)															__Th___;
		void  Draw8 (const DrawIndexedIndirectCountCmd3 &)													__Th___;
		void  Draw9 (const DrawMeshTasksIndirectCountCmd3 &)												__Th___;

		static void  Bind (const ScriptEnginePtr &se)														__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)											__Th___;

		ScriptUniGeometry*  Clone ()																		C_Th___;

	// ScriptGeomSource //
		ND_ RC<IGeomSource>		ToGeomSource ()																__Th_OV;
		ND_ PipelineNames_t		FindMaterialGraphicsPipelines (ERenderLayer layer)							C_Th_OV;
		ND_ RC<IGSMaterials>	ToMaterial (ERenderLayer, RenderTechPipelinesPtr, const PipelineNames_t &)	C_Th_OV;

	private:
		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)												C_Th_OV;

		ND_ static auto  _CreateUBType ()																	__Th___;
	};



	//
	// Model Geometry Source
	//
	class ScriptModelGeometrySrc final : public ScriptGeomSource
	{
	// types
	private:
		struct OmniLight
		{
			float3		pos;
			float3		atten;
			RGBA32f		color;
		};

		struct ConeLight
		{
			float3		pos;
			float3		dir;
			float3		atten;
			float2		cone;
			RGBA32f		color;
		};

		struct DirLight
		{
			float3		dir;
			float3		atten;
			RGBA32f		color;
		};


	// variables
	private:
		Path							_scenePath;
		String							_dbgName;
		Array<Path>						_texSearchDirs;

		RTScriptGeometryTypes_t			_rtGeometries;

		Array<OmniLight>				_omiLights;
		Array<ConeLight>				_coneLights;
		Array<DirLight>					_dirLights;

		RC< ResLoader::IntermScene >	_intermScene;
		Transformation					_initialTransform;

		const uint						_maxTextures	= 128;

		RC<IGeomSource>					_geomSrc;


	// methods
	public:
		ScriptModelGeometrySrc () = delete;
		ScriptModelGeometrySrc (const String &filename)														__Th___;
		~ScriptModelGeometrySrc ();

			void  Name (const String &name)																	__Th___;
			void  AddTextureSearchDir (const String &value)													__Th___;
			void  SetInitialTransform1 (const packed_float4x4 &value)										__Th___;
			void  SetInitialTransform2 (const packed_float3 &position,
										const packed_float3 &rotation,
										float scale)														__Th___;

			void  AddOmniLight (const packed_float3	&pos,
								const packed_float3	&atten,
								const RGBA32f		&color)													__Th___;

			void  AddConeLight (const packed_float3	&pos,
								const packed_float3	&dir,
								const packed_float3	&atten,
								const packed_float2	&cone,
								const RGBA32f		&color)													__Th___;

			void  AddDirLight (const packed_float3	&dir,
							   const packed_float3	&atten,
							   const RGBA32f		&color)													__Th___;


		ND_ ScriptRTGeometryPtr  GetRTGeometry (EGeometryType)												__Th_OV;

		static void  Bind (const ScriptEnginePtr &se)														__Th___;
		static void  GetShaderTypes (INOUT CppStructsFromShaders &)											__Th___;

	// ScriptGeomSource //
		ND_ RC<IGeomSource>		ToGeomSource ()																__Th_OV;
		ND_ PipelineNames_t		FindMaterialGraphicsPipelines (ERenderLayer layer)							C_Th_OV;
		ND_ RC<IGSMaterials>	ToMaterial (ERenderLayer, RenderTechPipelinesPtr, const PipelineNames_t &)	C_Th_OV;

	private:
		ND_ PipelineNames_t		_FindPostProcessPipelines ()												C_Th___;
		ND_ RC<IGSMaterials>	_ToPostProcessMaterial (RenderTechPipelinesPtr, const PipelineNames_t &)	C_Th___;

		void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)												C_Th_OV;

		ND_ static String  _AttribsToVBName (const ResLoader::IntermVertexAttribs &)						__Th___;
	};


} // AE::ResEditor
