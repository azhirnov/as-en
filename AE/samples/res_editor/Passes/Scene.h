// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/Postprocess.h"
#include "res_editor/Passes/RayTracingPass.h"
#include "res_editor/GeomSource/IGeomSource.h"

namespace AE::ResEditor
{

	//
	// Scene Data
	//
	class SceneData final : public EnableRC< SceneData >
	{
		friend class SceneGraphicsPass;
		friend class SceneRayTracingPass;
		friend class ScriptScene;

	// types
	private:
		struct GeometryInstance
		{
			RC<IGeomSource>		geometry;
			float4x4			transform;
		};
		using GeomInstances_t = Array< GeometryInstance >;


	// variables
	private:
		GeomInstances_t		_geomInstances;


	// methods
	public:
	};



	//
	// Scene Graphics Pass
	//
	class SceneGraphicsPass final : public IPass
	{
		friend class ScriptSceneGraphicsPass;

	// types
	private:
		using Materials_t	= Array< RC<IGSMaterials> >;


	// variables
	private:
		RTechInfo				_rtech;

		RC<SceneData>			_scene;
		Materials_t				_materials;

		RenderPassDesc			_rpDesc;
		float2					_depthRange			{0.f, 1.f};
		ERenderLayer			_renderLayer;

		ResourceArray			_resources;			// per pass
		RenderTargets_t			_renderTargets;

		Strong<BufferID>		_ubuffer;
		PerFrameDescSet_t		_descSets;
		DescSetBinding			_dsIndex;


	// methods
	public:
		SceneGraphicsPass ()											__NE___	{}
		~SceneGraphicsPass ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV;
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;
	};



	//
	// Scene Ray Tracing Pass
	//
	class SceneRayTracingPass final : public IPass
	{
		friend class ScriptSceneRayTracingPass;

	// types
	private:
		using Materials_t	= Array< RC<IGSMaterials> >;
		using Iterations_t	= Array< RayTracingPass::Iteration >;


	// variables
	private:
		RTechInfo				_rtech;

		RayTracingPipelineID	_pipeline;
		RTShaderBindingID		_sbt;

		Iterations_t			_iterations;

		RC<SceneData>			_scene;

		ResourceArray			_resources;

		PerFrameDescSet_t		_passDescSets;
		PerFrameDescSet_t		_objDescSets;

		Strong<BufferID>		_ubuffer;

		DescSetBinding			_passDSIndex;
		DescSetBinding			_objDSIndex;


	// methods
	public:
		SceneRayTracingPass ()											__NE___ {}
		~SceneRayTracingPass ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV;
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;
	};


} // AE::ResEditor
