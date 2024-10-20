// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/GeomSource/IGeomSource.h"
#include "geometry_tools/SphericalCube/SphericalCubeRenderer.h"

namespace AE::ResEditor
{

	//
	// Spherical Cube Geometry Source
	//

	class SphericalCube final : public IGeomSource
	{
		friend class ScriptSphericalCube;

	// types
	private:
		using PipelineMap_t	= FixedMap< Tuple< EDebugMode, EShaderStages >, PplnID_t, uint(EDebugMode::_Count)*2 >;

		class Material final : public IGSMaterials
		{
		// variables
		public:
			RenderTechPipelinesPtr		rtech;

			PipelineMap_t				pplnMap;
			PerFrameDescSet_t			descSets;

			DescSetBinding				passDSIndex;
			DescSetBinding				mtrDSIndex;

			Strong<BufferID>			ubuffer;


		// methods
		public:
			Material ()		__NE___	{}
			~Material ();

			DebugModeBits  GetDebugModeBits ()	C_NE_OV;
		};


	// variables
	private:
		GeometryTools::SphericalCubeRenderer	_cube;
		ResourceArray							_resources;

		const uint								_minLod			= 0;
		const uint								_maxLod			= 0;
		const uint								_instanceCount	= 1;


	// methods
	public:
		SphericalCube (Renderer &r, uint minLod, uint maxLod, uint inst)__NE___;
		~SphericalCube ();


	// IGeomSource //
		void  StateTransition (IGSMaterials &, DirectCtx::Graphics &)	__Th_OV;
		using IGeomSource::StateTransition;

		bool  Draw (const DrawData &)									__Th_OV;
		bool  Update (const UpdateData &)								__Th_OV;
		void  PrepareForDebugging (INOUT DebugPrepareData &)			__Th_OV;
	};


} // AE::ResEditor
