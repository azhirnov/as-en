// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/GeomSource/IGeomSource.h"

namespace AE::ResLoader {
	class IntermScene;
	class IntermMesh;
}
namespace AE::ResEditor
{

	//
	// Model Geometry Source
	//

	class ModelGeomSource final : public IGeomSource
	{
		friend class ScriptModelGeometrySrc;

	// types
	private:
		using RTGeometryTypes_t	= StaticArray< RC<RTGeometry>, uint(ERTGeometryType::_Count) >;


		//
		// Material
		//
		class Material final : public IGSMaterials
		{
		// types
		public:
			using GPplnGroups_t		= FlatHashMap< GraphicsPipelineID, Array<uint> >;
			using MPplnGroups_t		= FlatHashMap< MeshPipelineID, Array<uint> >;
			using PplnGroups_t		= Union< NullUnion, GPplnGroups_t, MPplnGroups_t >;


		// variables
		public:
			RenderTechPipelinesPtr		rtech;

			PplnGroups_t				drawGroups;
			PerFrameDescSet_t			descSets;

			DescSetBinding				passDSIndex;
			DescSetBinding				mtrDSIndex;

			Strong<BufferID>			ubuffer;


		// methods
		public:
			Material ()		__NE___ {}
			~Material ();
		};


		//
		// Mesh
		//
		class Mesh final : public IResource
		{
		// types
		private:
			struct MeshDataInRAM
			{
				StructView< packed_float3 >		positions;
				StructView< packed_float3 >		normals;
				StructView< packed_float2 >		texcoord0;
				StructView< uint >				indices;
			};

			struct MeshDataInGPU
			{
				uint		indexCount	= 0;

				usize		firstIndex;
				usize		vertexOffset;

				Bytes		positions;
				Bytes		normals;
				Bytes		texcoords;
				Bytes		indices;
			};

			struct DrawCall
			{
				uint		nodeIdx;
				uint		vertexOffset;
				uint		firstIndex;
				uint		indexCount;
			};

			struct TmpDataForUploading
			{
				usize		meshIndexCount		= 0;
				Bytes		positionsOffset;
				Bytes		normalsOffset;
				Bytes		texcoord0Offset;
				Bytes		indicesOffset;

				Bytes		nodeDataSize;
				Bytes		materialDataSize;
				Bytes		rtInstancesDataSize;
				usize		nodeCount			= 0;

				Array<MeshDataInGPU>	meshInfoArr;
			};

			// MtrFlag_* in [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/shaders/ModelMaterial.glsl)
			enum class EMtrFlags : uint
			{
				AlphaTest	= 0,
			};

			static constexpr inline float	_RGBM_MaxRange	= 64.f;


		// variables
		private:
			Strong<BufferID>			_meshData;		// vertices and indices
			Strong<BufferID>			_nodeBuffer;
			Strong<BufferID>			_materials;
			Strong<BufferID>			_rtInstances;

			Strong<BufferID>			_lights;

			Array<DrawCall>				_drawCalls;

			usize						_meshVertexCount	= 0;

			Unique<TmpDataForUploading>	_temp;
			RC<ResLoader::IntermScene>	_intermScene;
			const Transformation		_initialTransform;

			RTGeometryTypes_t			_rtGeometries;


		// methods
		public:
			Mesh (Renderer						&r,
				  RC<ResLoader::IntermScene>	scene,
				  const Transformation			&initialTransform,
				  RTGeometryTypes_t &&			rtGeoms)									__Th___;
			~Mesh ();

			ND_ bool  BindForGraphics (DescriptorUpdater &updater)							C_NE___;
			ND_ bool  BindForRayTracing (DescriptorUpdater &updater)						C_NE___;

			template <typename Ctx>
				void  StateTransition (Ctx &)												C_Th___;

				void  Draw (DirectCtx::Draw					&ctx,
							const Material::GPplnGroups_t	&drawGroups)					C_Th___;

				void  Draw (DirectCtx::Draw					&ctx,
							const Material::MPplnGroups_t	&drawGroups)					C_Th___;

				void  BindBuffers (DirectCtx::Draw	&ctx)									C_Th___;

			// IResource //
			bool			Resize (TransferCtx_t &)										__Th_OV	{ return true; }
			bool			RequireResize ()												C_Th_OV	{ return false; }
			EUploadStatus	Upload (TransferCtx_t &)										__Th_OV;
			EUploadStatus	Readback (TransferCtx_t &)										__Th_OV	{ return EUploadStatus::Canceled; }


		private:
			ND_ static MeshDataInRAM  _Convert (const ResLoader::IntermMesh &)				__Th___;

			template <typename T>
			ND_ bool  _UploadData (Bytes&, Bytes&, Bytes, TransferCtx_t&, StructView<T>)	C_Th___;

			template <typename T>
			ND_ bool  _UploadInfo (Bytes &, TransferCtx_t &, Array<T> &)					__Th___;

			ND_ bool  _UploadNodes (TransferCtx_t &)										__Th___;
			ND_ bool  _UploadMaterials (TransferCtx_t &)									__Th___;
			ND_ bool  _UploadLights (TransferCtx_t &)										C_Th___;

			ND_ bool  _BuildRTGeometries (TransferCtx_t &)									__Th___;
			ND_ bool  _BuildRTGeometry (TransferCtx_t &, RTGeometry &, ERTGeometryType)		C_Th___;
			ND_ bool  _UploadRTInstances (TransferCtx_t &)									C_Th___;

			ND_ static uint  _PackRGBM (const float3 &col)									__NE___;
		};


		//
		// Textures
		//
		class Textures final : public IResource
		{
		// variables
		private:
			Array< RC<Image> >		_albedoMaps;
			const uint				_maxTextures;


		// methods
		public:
			Textures (Renderer						&r,
					  RC<ResLoader::IntermScene>	scene,
					  ArrayView<Path>				texSearchDirs,
					  uint							maxTextures)					__Th___;

			ND_ bool  Bind (DescriptorUpdater &updater)								C_NE___;

			// IResource //
			bool			Resize (TransferCtx_t &)								__Th_OV	{ return true; }
			bool			RequireResize ()										C_Th_OV	{ return false; }
			EUploadStatus	Upload (TransferCtx_t &)								__Th_OV	{ return EUploadStatus::Completed; }
			EUploadStatus	Readback (TransferCtx_t &)								__Th_OV	{ return EUploadStatus::Canceled; }
		};


	// variables
	private:
		RC<Mesh>			_meshData;
		RC<Textures>		_textures;


	// methods
	public:
		ModelGeomSource (Renderer					&r,
						 RC<ResLoader::IntermScene> scene,
						 const Transformation		&initialTransform,
						 ArrayView<Path>			texSearchDirs,
						 uint						maxTextures,
						 RTGeometryTypes_t &&		rtGeoms)				__Th___;
		~ModelGeomSource ();


	// IGeomSource //
		void  StateTransition (IGSMaterials &, DirectCtx::Graphics &)		__Th_OV;
		void  StateTransition (DirectCtx::RayTracing &)						__Th_OV;

		bool  Draw (const DrawData &)										__Th_OV;
		bool  PostProcess (const DrawData &)								__Th_OV;
		bool  Update (const UpdateData &)									__Th_OV;
		bool  RTUpdate (const UpdateRTData &)								__Th_OV;
	};


} // AE::ResEditor
