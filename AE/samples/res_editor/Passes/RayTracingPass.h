// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Passes/IPass.h"
#include "Resources/IResource.h"

namespace AE::ResEditor
{

	//
	// Ray Tracing Pass
	//

	class RayTracingPass final : public IPass
	{
		friend class ScriptRayTracingPass;

	// types
	private:
		using PipelineMap_t		= FixedMap< EDebugMode, Tuple< RayTracingPipelineID, RTShaderBindingID >, uint(EDebugMode::_Count) >;

	public:
		using IterationDim_t	= Union< uint3, RC<DynamicDim>, RC<DynamicUInt>, RC<DynamicUInt3> >;

		struct Iteration
		{
			IterationDim_t		dim;
			RC<Buffer>			indirect;
			Bytes				indirectOffset;

			ND_ uint3  Dimension () const;

			ND_ static uint3  FindMaxConstDimension (ArrayView<Iteration>);
		};
		using Iterations_t	= Array< Iteration >;


	// variables
	private:
		RTechInfo				_rtech;

		PipelineMap_t			_pipelines;
		PerFrameDescSet_t		_descSets;
		DescSetBinding			_dsIndex;

		Strong<BufferID>		_ubuffer;

		ResourceArray			_resources;
		Iterations_t			_iterations;

		RC<DynamicUInt>			_maxRayRecursion;
		RC<DynamicUInt>			_maxCallRecursion;

		Bytes16u				_rayGenStackMax;
		Bytes16u				_closestHitStackMax;
		Bytes16u				_missStackMax;
		Bytes16u				_intersectionStackMax;
		Bytes16u				_anyHitStackMax;
		Bytes16u				_callableStackMax;


	// methods
	public:
		RayTracingPass ()												__NE___	{}
		~RayTracingPass ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV;
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;
	};


} // AE::ResEditor
