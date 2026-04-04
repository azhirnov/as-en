// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Passes/Postprocess.h"
#include "Passes/ComputePass.h"
#include "Passes/RayTracingPass.h"
#include "GeomSource/IGeomSource.h"

namespace AE::ResEditor
{

	//
	// Scene Data
	//
	class SceneData final : public EnableRC< SceneData >
	{
		friend class SceneGraphicsPass;
		friend class SceneGraphicsSubpass;
		friend class SceneRayTracingPass;
		friend class SceneRayQueryPass;
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
	// Scene Graphics Subpass
	//
	class SceneGraphicsSubpass final : public IPass
	{
		friend class SceneGraphicsPass;
		friend class ScriptSceneGraphicsPass;
		friend class ScriptSceneGraphicsSubpass;

	// types
	private:
		using Materials_t			= Array< RC<IGSMaterials> >;
		using FScissors_t			= FixedArray< RectF, GraphicsConfig::MaxViewports >;
		using Scissors_t			= FixedArray< RectI, GraphicsConfig::MaxViewports >;
		using Viewports_t			= FixedArray< Viewport, GraphicsConfig::MaxViewports >;

	public:
		struct ShadingRate
		{
			EShadingRate			rate		= EShadingRate(0);
			EShadingRateCombinerOp	primitiveOp	= Default;
			EShadingRateCombinerOp	textureOp	= Default;

			ND_ explicit operator bool ()	const { return rate != EShadingRate(0) and primitiveOp != Default and textureOp != Default; }
		};


	// variables
	private:
		RC<SceneData>			_scene;
		Materials_t				_materials;

		uint2					_dimension;			// updated by main pass
		ERenderLayer			_renderLayer;
		FScissors_t				_scissors;
		Viewports_t				_viewports;

		ResourceArray			_resources;			// per pass

		Strong<BufferID>		_ubuffer;
		PerFrameDescSet_t		_descSets;
		DescSetBinding			_dsIndex;

		ShadingRate				_shadingRate;


	// methods
	public:
		SceneGraphicsSubpass ()											__NE___	{}
		~SceneGraphicsSubpass ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV { return false; }
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;
	};



	//
	// Scene Graphics Pass
	//
	class SceneGraphicsPass final : public IPass
	{
		friend class ScriptSceneGraphicsPass;
		friend class ScriptSceneGraphicsSubpass;

	// types
	private:
		using Subpasses_t	= Array< RC<SceneGraphicsSubpass> >;
		using Scissors_t	= SceneGraphicsSubpass::Scissors_t;
		using Viewports_t	= SceneGraphicsSubpass::Viewports_t;
		using PplnToObjID_t	= IGeomSource::DebugPrepareData::PplnToObjID_t;


	// variables
	private:
		RTechInfo				_rtech;

		RC<SceneData>			_scene;
		RenderPassDesc			_rpDesc;
		RenderTargets_t			_renderTargets;

		Subpasses_t				_subpasses;

		PplnToObjID_t			_tempPplnToObjID;


	// methods
	public:
		SceneGraphicsPass ()											__NE___	{}
		~SceneGraphicsPass ()													{}

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



	//
	// Scene Ray Query Pass
	//
	class SceneRayQueryPass final : public IPass
	{
		friend class ScriptSceneRayQueryPass;

	// types
	private:
		using Iteration			= ComputePass::Iteration;
		using Iterations_t		= ComputePass::Iterations_t;
		using PipelineMap_t		= FixedMap< EDebugMode, ComputePipelineID, uint(EDebugMode::_Count) >;


	// variables
	private:
		RTechInfo				_rtech;

		PipelineMap_t			_pipelines;

		RC<SceneData>			_scene;

		ResourceArray			_resources;
		Iterations_t			_iterations;
		uint3					_localSize;

		PerFrameDescSet_t		_passDescSets;
		PerFrameDescSet_t		_objDescSets;

		Strong<BufferID>		_ubuffer;

		DescSetBinding			_passDSIndex;
		DescSetBinding			_objDSIndex;
		PushConstantIndex		_pcIndex;


	// methods
	public:
		SceneRayQueryPass ()											__NE___ {}
		~SceneRayQueryPass ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV;
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;
	};


} // AE::ResEditor
