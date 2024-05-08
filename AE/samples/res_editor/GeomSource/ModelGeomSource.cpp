// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/ModelGeomSource.h"
#include "res_editor/Passes/Renderer.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/RTScene.h"

#include "res_loaders/Intermediate/IntermScene.h"

#include "res_editor/_ui_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	destructor
=================================================
*/
	ModelGeomSource::Material::~Material ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResourceArray( INOUT descSets );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ModelGeomSource::Mesh::Mesh (Renderer					&r,
								 RC<ResLoader::IntermScene>	scene,
								 const Transformation		&initialTransform,
								 RTGeometryTypes_t &&		rtGeoms) __Th___ :
		IResource{ r }, _temp{new TmpDataForUploading{}},
		_intermScene{ RVRef(scene) }, _initialTransform{ initialTransform },
		_rtGeometries{ RVRef(rtGeoms) }
	{
		_uploadStatus.store( EUploadStatus::InProgress );

		auto&			res_mngr	= RenderGraph().GetStateTracker();
		const usize		mesh_cnt	= _intermScene->Meshes().size();
		const bool		use_rtas	= [this](){ bool b = false;  for (auto& g : _rtGeometries) b |= (g != null);  return b; }();
		EBufferUsage	usage		= EBufferUsage::TransferDst | EBufferUsage::Storage |
									  (use_rtas ? EBufferUsage::ASBuild_ReadOnly | EBufferUsage::ShaderAddress : Default);

		_temp->nodeCount			= _intermScene->ModelNodeCount();

		for (auto& [mesh, idx] : _intermScene->Meshes())
		{
			_meshVertexCount		+= mesh->VertexCount();
			_temp->meshIndexCount	+= mesh->IndexCount();
		}

		_temp->positionsOffset		= 0_b;
		_temp->normalsOffset		= SizeOf<decltype(MeshDataInRAM::positions)::Value_t> * _meshVertexCount + _temp->positionsOffset;
		_temp->texcoord0Offset		= SizeOf<decltype(MeshDataInRAM::normals  )::Value_t> * _meshVertexCount + _temp->normalsOffset;
		_temp->indicesOffset		= SizeOf<decltype(MeshDataInRAM::texcoord0)::Value_t> * _meshVertexCount + _temp->texcoord0Offset;
		const Bytes	mesh_data_size	= SizeOf<decltype(MeshDataInRAM::indices  )::Value_t> * _temp->meshIndexCount + _temp->indicesOffset;


		CHECK_THROW( mesh_data_size > 0 );
		CHECK_THROW( mesh_cnt > 0 );
		CHECK_THROW( _temp->nodeCount > 0 );

		_temp->meshInfoArr.resize( mesh_cnt );

		_meshData = res_mngr.CreateBuffer( BufferDesc{ mesh_data_size, usage | EBufferUsage::Vertex | EBufferUsage::Index },
											"Vertices & Indices", r.ChooseAllocator( False{"static"}, mesh_data_size ));
		CHECK_THROW( _meshData );

		_temp->nodeDataSize	= SizeOf<ShaderTypes::ModelNode> * _temp->nodeCount;
		_nodeBuffer			= res_mngr.CreateBuffer( BufferDesc{ _temp->nodeDataSize, usage },
													 "ModelNodes", r.ChooseAllocator( False{"static"}, _temp->nodeDataSize ));
		CHECK_THROW( _nodeBuffer );

		_temp->materialDataSize	= SizeOf<ShaderTypes::ModelMaterial> * _intermScene->Materials().size();
		_materials				= res_mngr.CreateBuffer( BufferDesc{ _temp->materialDataSize, usage },
														 "ModelMaterials", r.ChooseAllocator( False{"static"}, _temp->materialDataSize ));
		CHECK_THROW( _materials );

		_lights = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::SceneLights>, EBufferUsage::Storage | EBufferUsage::TransferDst },
										 "SceneLights", r.GetStaticAllocator() );
		CHECK_THROW( _lights );

		if ( use_rtas )
		{
			Bytes	size = SizeOf<ShaderTypes::ModelRTInstances>;
					size =  AlignUp( size, alignof(ShaderTypes::ModelRTMesh) );
					size += SizeOf<ShaderTypes::ModelRTMesh> * _temp->nodeCount;	// meshesPerInstance
					size =  AlignUp( size, alignof(uint) );
					size += SizeOf<uint> * _temp->nodeCount;						// materialsPerInstance
					size =  AlignUp( size, alignof(float3x3) );
					size += SizeOf<float3x3> * _temp->nodeCount;					// normalMatPerInstance

			_temp->rtInstancesDataSize	= size;
			_rtInstances				= res_mngr.CreateBuffer( BufferDesc{ size, usage }, "ModelRTInstances",
																 r.ChooseAllocator( False{"static"}, size ));
			CHECK_THROW( _rtInstances );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	ModelGeomSource::Mesh::~Mesh ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResources( _meshData, _nodeBuffer, _materials, _rtInstances, _lights );
	}

/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  ModelGeomSource::Mesh::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		const Bytes	pos_end		= SizeOf<decltype(MeshDataInRAM::positions)::Value_t> * _meshVertexCount;
		const Bytes	norm_end	= SizeOf<decltype(MeshDataInRAM::normals  )::Value_t> * _meshVertexCount + pos_end;
		const Bytes	tex0_end	= SizeOf<decltype(MeshDataInRAM::texcoord0)::Value_t> * _meshVertexCount + norm_end;
		const Bytes	idx_end		= SizeOf<decltype(MeshDataInRAM::indices  )::Value_t> * _temp->meshIndexCount + tex0_end;

		Bytes		pos_off;
		Bytes		norm_off	= pos_end;
		Bytes		tex0_off	= norm_end;
		Bytes		idx_off		= tex0_end;
		usize		vert_count	= 0;
		usize		idx_count	= 0;

		for (auto& [mesh, idx] : _intermScene->Meshes())
		{
			const MeshDataInRAM	md		= _Convert( *mesh );
			MeshDataInGPU&		info	= _temp->meshInfoArr[idx];

			info.indexCount	= uint(md.indices.size());
			info.firstIndex	= idx_count;
			info.vertexOffset= vert_count;
			info.positions	= pos_off;	if ( not _UploadData( INOUT pos_off,  INOUT _temp->positionsOffset, pos_end,  ctx, md.positions )) return EUploadStatus::NoMemory;
			info.normals	= norm_off;	if ( not _UploadData( INOUT norm_off, INOUT _temp->normalsOffset,   norm_end, ctx, md.normals   )) return EUploadStatus::NoMemory;
			info.texcoords	= tex0_off;	if ( not _UploadData( INOUT tex0_off, INOUT _temp->texcoord0Offset, tex0_end, ctx, md.texcoord0 )) return EUploadStatus::NoMemory;
			info.indices	= idx_off;	if ( not _UploadData( INOUT idx_off,  INOUT _temp->indicesOffset,   idx_end,  ctx, md.indices   )) return EUploadStatus::NoMemory;

			vert_count	+= md.positions.size();
			idx_count	+= md.indices.size();
		}

		if ( not _UploadNodes( ctx ))
			return EUploadStatus::NoMemory;

		if ( not _UploadMaterials( ctx ))
			return EUploadStatus::NoMemory;

		if ( not _UploadLights( ctx ))
			return EUploadStatus::NoMemory;

		if ( not _BuildRTGeometries( ctx ))
			return EUploadStatus::NoMemory;

		_temp		 = null;
		_intermScene = null;

		_uploadStatus.store( EUploadStatus::Completed );
		return EUploadStatus::Completed;
	}

/*
=================================================
	_UploadData
=================================================
*/
	template <typename T>
	bool  ModelGeomSource::Mesh::_UploadData (INOUT Bytes &srcOffset, INOUT Bytes &dstOffset, const Bytes dstEnd, TransferCtx_t &ctx, StructView<T> view) C_Th___
	{
		const Bytes	src_off = srcOffset;
		srcOffset += view.DataSize();

		if ( view.empty() )
			return true;

		if ( not IsIntersects( dstOffset, dstEnd, src_off, src_off + view.DataSize() ))
			return true;  // skip

		BufferMemView	mem_view;
		ctx.UploadBuffer( _meshData, UploadBufferDesc{ dstOffset, view.DataSize() }.DynamicHeap().BlockSize( SizeOf<T> ), OUT mem_view );

		if ( mem_view.DataSize() < view.DataSize() )
			return false;  // out of memory

		dstOffset += view.DataSize();

		auto	it = view.begin();
		for (auto& part : mem_view.Parts())
		{
			ASSERT( IsMultipleOf( part.size, SizeOf<T> ));

			for (Bytes off; (off < part.size) and (it != view.end()); ++it, off += SizeOf<T>)
			{
				MemCopy( OUT part.ptr + off, &(*it), SizeOf<T> );
			}
		}
		ASSERT( it == view.end() );

		return true;
	}

/*
=================================================
	_UploadNodes
=================================================
*/
	bool  ModelGeomSource::Mesh::_UploadNodes (TransferCtx_t &ctx) __Th___
	{
		BufferMemView	mem_view;
		ctx.UploadBuffer( _nodeBuffer, UploadBufferDesc{ 0_b, _temp->nodeDataSize }.DynamicHeap().MaxBlockSize(), OUT mem_view );

		if ( mem_view.DataSize() < _temp->nodeDataSize )
			return false;	// out of memory

		Array<ShaderTypes::ModelNode>	nodes;
		nodes.reserve( usize(_temp->nodeDataSize / SizeOf<ShaderTypes::ModelNode>) );

		_intermScene->ForEachNode(
			[this, &nodes] (StringView, const ResLoader::IntermScene::NodeData_t &data, const Transformation &tr)
			{
				Visit( data,
					[this, &tr, &nodes] (const ResLoader::IntermScene::ModelData &model)
					{
						auto		mesh		= model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
						auto		mtr			= model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
						auto&		node		= nodes.emplace_back();
						const auto	cur_tr		= _initialTransform + tr;
						const auto	mesh_idx	= _intermScene->IndexOfMesh( mesh );
						const auto&	mesh_info	= _temp->meshInfoArr[ mesh_idx ];

						CHECK_THROW( mesh and mtr );

						node.transform		= cur_tr.ToMatrix();
						node.normalMat		= float3x3{cur_tr.orientation.Inversed()}.Transpose();
						node.meshIdx		= mesh_idx;
						node.materialIdx	= _intermScene->IndexOfMaterial( mtr );

						auto&		draw	= _drawCalls.emplace_back();
						draw.nodeIdx		= uint(nodes.size()-1);
						draw.vertexOffset	= CheckCast<uint>(mesh_info.vertexOffset);
						draw.firstIndex		= CheckCast<uint>(mesh_info.firstIndex);
						draw.indexCount		= CheckCast<uint>(mesh->IndexCount());
					},
					[] (const NullUnion &) {}
				);
				return true;
			});

		CHECK_THROW( _temp->nodeDataSize == ArraySizeOf(nodes) );
		CHECK_THROW( mem_view.CopyFrom( nodes ) == _temp->nodeDataSize );

		return true;
	}

/*
=================================================
	_PackRGBM
=================================================
*/
	uint  ModelGeomSource::Mesh::_PackRGBM (const float3 &col) __NE___
	{
		float	m = Ceil( Max( col.x, col.y, col.z ) / _RGBM_MaxRange * 255.f ) / 255.f;
		uint	res = UNorm8x4ToUInt( float4{ col * (1.f / (m * _RGBM_MaxRange)), m });
		DEBUG_ONLY(
			float4	c2 = UIntToUNorm8x4( res );
			c2 = c2 * (c2.w * _RGBM_MaxRange);
			ASSERT( All(Equal( col, float3{c2}, 0.002f )));
		)
		return res;
	}

/*
=================================================
	_UploadMaterials
=================================================
*/
	bool  ModelGeomSource::Mesh::_UploadMaterials (TransferCtx_t &ctx) __Th___
	{
		BufferMemView	mem_view;
		ctx.UploadBuffer( _materials, UploadBufferDesc{ 0_b, _temp->materialDataSize }.DynamicHeap().MaxBlockSize(), OUT mem_view );

		if ( mem_view.DataSize() < _temp->materialDataSize )
			return false;	// out of memory

		Array<ShaderTypes::ModelMaterial>	materials;
		materials.resize( _intermScene->Materials().size() );

		const auto	PackMapAndSampler = [] (usize mapIdx, usize samplerIdx)
		{{
			return uint(mapIdx & 0xFFFF) | uint((samplerIdx & 0xFFFF) << 16);
		}};

		using EKey = ResLoader::IntermMaterial::EKey;

		const auto	PackRGBM = [] (const ResLoader::IntermMaterial &mtr, EKey key)
		{{
			float4	col {1.f};
			Unused( mtr.Get( key, OUT col ));
			return _PackRGBM( float3{col} );
		}};

		for (auto& [src, idx] : _intermScene->Materials())
		{
			auto&	dst		= materials[idx];

			dst.flags		= ToBit<uint>( src->GetSettings().HasAlphaTest(), uint(EMtrFlags::AlphaTest) );

			dst.albedoMap	= PackMapAndSampler( idx, 0 );
			dst.albedoRGBM	= PackRGBM( *src, EKey::PBR_BaseColor );
		}

		CHECK_THROW( _temp->materialDataSize == ArraySizeOf(materials) );
		CHECK_THROW( mem_view.CopyFrom( materials ) == _temp->materialDataSize );

		return true;
	}

/*
=================================================
	_UploadLights
=================================================
*/
	bool  ModelGeomSource::Mesh::_UploadLights (TransferCtx_t &ctx) C_Th___
	{
		using ELightType = ResLoader::IntermLight::ELightType;

		BufferMemView	mem_view;
		ctx.UploadBuffer( _lights, UploadBufferDesc{ 0_b, SizeOf<ShaderTypes::SceneLights> }.DynamicHeap().MaxBlockSize(), OUT mem_view );

		if ( mem_view.DataSize() < SizeOf<ShaderTypes::SceneLights> )
			return false;	// out of memory

		ShaderTypes::SceneLights	lights_data;
		ZeroMem( lights_data );

		for (auto& light : _intermScene->Lights())
		{
			const auto&	src = light.first->Get();

			switch_enum( src.type )
			{
				case ELightType::Point :
					if ( lights_data.omniCount+1 < lights_data.omni.size() )
					{
						auto&	dst = lights_data.omni[ lights_data.omniCount++ ];
						dst.position	= src.position;
						dst.attenuation	= src.attenuation;
						dst.colorRGBM	= _PackRGBM( src.diffuseColor );
					}
					break;

				case ELightType::Spot :
					if ( lights_data.coneCount+1 < lights_data.cone.size() )
					{
						auto&	dst = lights_data.cone[ lights_data.coneCount++ ];
						dst.position	= src.position;
						dst.direction	= src.direction;
						dst.attenuation	= src.attenuation;
						dst.cone		= src.coneAngleInnerOuter;
						dst.colorRGBM	= _PackRGBM( src.diffuseColor );
					}
					break;

				case ELightType::Directional :
					if ( lights_data.directionalCount+1 < lights_data.directional.size() )
					{
						auto&	dst = lights_data.directional[ lights_data.directionalCount++ ];
						dst.direction	= src.direction;
						dst.attenuation	= src.attenuation;
						dst.colorRGBM	= _PackRGBM( src.diffuseColor );
					}
					break;

				case ELightType::Unknown :
				case ELightType::Ambient :
				case ELightType::Area :		break;
			}
			switch_end
		}

		CHECK_THROW( mem_view.CopyFrom( 0_b, &lights_data, Sizeof(lights_data) ) == SizeOf<ShaderTypes::SceneLights> );
		return true;
	}

/*
=================================================
	_BuildRTGeometries
=================================================
*/
	bool  ModelGeomSource::Mesh::_BuildRTGeometries (TransferCtx_t &ctx) __Th___
	{
		for (auto i : IndicesOnly( _rtGeometries ))
		{
			auto&	geom = _rtGeometries[i];

			if ( geom and geom->GetStatus() == EUploadStatus::InProgress )
			{
				if ( not _BuildRTGeometry( ctx, *geom, ERTGeometryType(i) ))
					return false;
			}
		}

		if ( _rtInstances )
		{
			if ( not _UploadRTInstances( ctx ))
				return false;
		}
		return true;
	}

/*
=================================================
	_BuildRTGeometry
=================================================
*/
	bool  ModelGeomSource::Mesh::_BuildRTGeometry (TransferCtx_t &ctx, RTGeometry &rtGeometry, const ERTGeometryType mode) C_Th___
	{
		const usize				node_count		= _intermScene->ModelNodeCount();
		auto&					res_mngr		= RenderGraph().GetStateTracker();
		const Bytes				rtmat_size		= SizeOf<RTMatrixStorage> * node_count;
		GAutorelease<BufferID>	transform_buf	= res_mngr.CreateBuffer( BufferDesc{ rtmat_size, EBufferUsage::TransferDst | EBufferUsage::ASBuild_ReadOnly },
																		 Default, _Renderer().ChooseAllocator( True{"dynamic"}, rtmat_size ));
		CHECK_THROW( transform_buf );

		BufferMemView	mem_view;
		ctx.UploadBuffer( transform_buf, UploadBufferDesc{ 0_b, rtmat_size }.DynamicHeap().MaxBlockSize(), OUT mem_view );

		if ( mem_view.DataSize() < rtmat_size )
			return false;	// out of memory

		Array<RTGeometryBuild::TrianglesInfo>	tri_infos;	tri_infos.reserve( node_count );
		Array<RTGeometryBuild::TrianglesData>	tri_data;	tri_data.reserve( node_count );
		Array<RTMatrixStorage>					mat_arr;	mat_arr.reserve( node_count );

		_intermScene->ForEachNode(
			[&] (StringView, const ResLoader::IntermScene::NodeData_t &data, const Transformation &tr)
			{
				Visit( data,
					[&] (const ResLoader::IntermScene::ModelData &model)
					{
						auto	mesh	= model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
						auto	mtr		= model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
						CHECK_THROW( mesh and mtr );

						const bool	translucent		= mtr->GetSettings().IsTranslucent();
						const bool	dual_sided		= mtr->GetSettings().cullMode == ECullMode::None;

						if ( mode == ERTGeometryType::Opaque			and (translucent or dual_sided) )		return;
						if ( mode == ERTGeometryType::OpaqueDualSided	and (translucent or not dual_sided) )	return;
						if ( mode == ERTGeometryType::Translucent		and (not translucent) )					return;
						if ( mode == ERTGeometryType::Volumetric )												return;	// TODO

						const auto&	mesh_info		= _temp->meshInfoArr[ _intermScene->IndexOfMesh( mesh )];
						const auto	cur_tr			= _initialTransform + tr;
						auto&		dst_info		= tri_infos.emplace_back();
						auto&		dst_data		= tri_data.emplace_back();
						const auto	vert_stride		= SizeOf< decltype(MeshDataInRAM::positions)::Value_t >;

						dst_info.options			= Default;	// TODO
						dst_info.maxPrimitives		= mesh_info.indexCount / 3;
						dst_info.maxVertex			= uint((mesh_info.normals - mesh_info.positions) / vert_stride);
						dst_info.vertexFormat		= EVertexType::Float3;
						dst_info.indexType			= EIndex::UInt;
						dst_info.allowTransforms	= true;

						dst_data.vertexData			= _meshData;
						dst_data.indexData			= _meshData;
						dst_data.transformData		= transform_buf;
						dst_data.vertexStride		= vert_stride;
						dst_data.vertexDataOffset	= mesh_info.positions;
						dst_data.indexDataOffset	= mesh_info.indices;
						dst_data.transformDataOffset= ArraySizeOf( mat_arr );

						mat_arr.push_back( RTMatrixStorage{ cur_tr.ToMatrix() });
					},
					[] (const NullUnion &) {}
				);
				return true;
			});

		if ( mat_arr.empty() )
		{
			rtGeometry.CompleteUploading();
			return true;  // keep dummy geometry
		}

		RTGeometryBuild				geom_build	{ tri_infos, tri_data, Default, Default, ERTASOptions::PreferFastTrace };
		const auto					sizes		= res_mngr.GetRTGeometrySizes( geom_build );
		GAutorelease<BufferID>		scratch_buf	= res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::ASBuild_Scratch },
																		 Default, _Renderer().ChooseAllocator( True{"dynamic"}, sizes.buildScratchSize ));
		GAutorelease<RTGeometryID>	geom_id		= res_mngr.CreateRTGeometry( RTGeometryDesc{ sizes.rtasSize, geom_build.options }, Default,
																			 _Renderer().ChooseAllocator( False{"static"}, sizes.rtasSize ));

		CHECK_THROW( geom_id and scratch_buf );
		CHECK_THROW( mem_view.CopyFrom( mat_arr ) == ArraySizeOf(mat_arr) );

		DirectCtx::ASBuild	as_ctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

		geom_build.SetScratchBuffer( scratch_buf );
		as_ctx.Build( geom_build, geom_id );

		Reconstruct( INOUT ctx, as_ctx.GetRenderTask(), as_ctx.ReleaseCommandBuffer() );

		rtGeometry.Reset( geom_id.Release() );
		return true;
	}

/*
=================================================
	_UploadRTInstances
=================================================
*/
	bool  ModelGeomSource::Mesh::_UploadRTInstances (TransferCtx_t &ctx) C_Th___
	{
		BufferMemView	mem_view;
		ctx.UploadBuffer( _rtInstances, UploadBufferDesc{ 0_b, _temp->rtInstancesDataSize }.DynamicHeap().MaxBlockSize(), OUT mem_view );

		if ( mem_view.DataSize() < _temp->rtInstancesDataSize )
			return false;	// out of memory

		ShaderTypes::ModelRTInstances													inst_to_mesh;
		StaticArray< Array<ShaderTypes::ModelRTMesh>, uint(ERTGeometryType::_Count) >	rt_meshes;
		StaticArray< Array<uint>, uint(ERTGeometryType::_Count) >						rt_materials;
		StaticArray< Array<float3x3>, uint(ERTGeometryType::_Count) >					rt_norm_mats;

		StaticAssert( inst_to_mesh.meshesPerInstance.size() == rt_meshes.size() );
		StaticAssert( inst_to_mesh.materialsPerInstance.size() == rt_materials.size() );
		StaticAssert( inst_to_mesh.normalMatPerInstance.size() == rt_norm_mats.size() );

		const DeviceAddress		mesh_addr = GraphicsScheduler().GetResourceManager().GetDeviceAddress( _meshData );
		const DeviceAddress		inst_addr = GraphicsScheduler().GetResourceManager().GetDeviceAddress( _rtInstances );
		CHECK_ERR( mesh_addr != Default and inst_addr != Default );

		_intermScene->ForEachNode(
			[&] (StringView, const ResLoader::IntermScene::NodeData_t &data, const Transformation &tr)
			{
				Visit( data,
					[&] (const ResLoader::IntermScene::ModelData &model)
					{
						auto	mesh	= model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
						auto	mtr		= model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
						CHECK_THROW( mesh and mtr );

						const bool	translucent		= mtr->GetSettings().IsTranslucent();
						const bool	dual_sided		= mtr->GetSettings().cullMode == ECullMode::None;
						const auto	mtr_id			= _intermScene->IndexOfMaterial( mtr );
						const auto&	mesh_info		= _temp->meshInfoArr[ _intermScene->IndexOfMesh( mesh ) ];
						const auto	norm_mat		= float3x3{(_initialTransform + tr).orientation.Inversed()}.Transpose();

						ShaderTypes::ModelRTMesh	rt_mesh;
						rt_mesh.normals		= mesh_addr + mesh_info.normals;
						rt_mesh.texcoords	= mesh_addr + mesh_info.texcoords;
						rt_mesh.indices		= mesh_addr + mesh_info.indices;

						if ( translucent ){
							rt_meshes[    uint(ERTGeometryType::Translucent) ].push_back( rt_mesh );
							rt_materials[ uint(ERTGeometryType::Translucent) ].push_back( mtr_id );
							rt_norm_mats[ uint(ERTGeometryType::Translucent) ].push_back( norm_mat );
						}else
						if ( dual_sided ){
							rt_meshes[    uint(ERTGeometryType::OpaqueDualSided) ].push_back( rt_mesh );
							rt_materials[ uint(ERTGeometryType::OpaqueDualSided) ].push_back( mtr_id );
							rt_norm_mats[ uint(ERTGeometryType::OpaqueDualSided) ].push_back( norm_mat );
						}else{
							rt_meshes[    uint(ERTGeometryType::Opaque) ].push_back( rt_mesh );
							rt_materials[ uint(ERTGeometryType::Opaque) ].push_back( mtr_id );
							rt_norm_mats[ uint(ERTGeometryType::Opaque) ].push_back( norm_mat );
						}
					},
					[] (const NullUnion &) {}
				);
				return true;
			});

		Bytes	offset	= Sizeof(inst_to_mesh);
		usize	count	= 0;

		offset = AlignUp( offset, alignof(ShaderTypes::ModelRTMesh) );

		for (auto i : IndicesOnly( rt_meshes ))
		{
			CHECK_THROW( mem_view.CopyFrom( offset, rt_meshes[i].data(), ArraySizeOf(rt_meshes[i]) ) == ArraySizeOf(rt_meshes[i]) );

			inst_to_mesh.meshesPerInstance[i] = inst_addr + offset;
			offset	+= ArraySizeOf( rt_meshes[i] );
			count	+= rt_meshes[i].size();
		}
		CHECK( count == _temp->nodeCount );

		offset = AlignUp( offset, alignof(uint) );

		for (auto i : IndicesOnly( rt_materials ))
		{
			CHECK_THROW( mem_view.CopyFrom( offset, rt_materials[i].data(), ArraySizeOf(rt_materials[i]) ) == ArraySizeOf(rt_materials[i]) );

			inst_to_mesh.materialsPerInstance[i] = inst_addr + offset;
			offset	+= ArraySizeOf( rt_materials[i] );
		}

		offset = AlignUp( offset, alignof(float3x3) );

		for (auto i : IndicesOnly( rt_norm_mats ))
		{
			CHECK_THROW( mem_view.CopyFrom( offset, rt_norm_mats[i].data(), ArraySizeOf(rt_norm_mats[i]) ) == ArraySizeOf(rt_norm_mats[i]) );

			inst_to_mesh.normalMatPerInstance[i] = inst_addr + offset;
			offset	+= ArraySizeOf( rt_norm_mats[i] );
		}

		CHECK( offset == _temp->rtInstancesDataSize );
		CHECK_THROW( mem_view.CopyFrom( 0_b, &inst_to_mesh, Sizeof(inst_to_mesh) ) == Sizeof(inst_to_mesh) );

		return true;
	}

/*
=================================================
	_Convert
=================================================
*/
	ModelGeomSource::Mesh::MeshDataInRAM  ModelGeomSource::Mesh::_Convert (const ResLoader::IntermMesh &mesh) __Th___
	{
		CHECK_THROW( mesh.Attribs() != null );

		MeshDataInRAM	result;
		result.positions	= mesh.GetData< packed_float3 >( ResLoader::VertexAttributeName::Position );
		result.normals		= mesh.GetDataOpt< packed_float3 >( ResLoader::VertexAttributeName::Normal );
		result.texcoord0	= mesh.GetDataOpt< packed_float2 >( ResLoader::VertexAttributeName::TextureUVs[0] );
		result.indices		= mesh.GetIndexData< uint >();

		CHECK_THROW( not result.positions.empty() and not result.indices.empty() );

		ASSERT( result.normals.empty()   or result.positions.size() == result.normals.size() );
		ASSERT( result.texcoord0.empty() or result.positions.size() == result.texcoord0.size() );

		return result;
	}

/*
=================================================
	BindForGraphics / BindForRayTracing
=================================================
*/
	bool  ModelGeomSource::Mesh::BindForGraphics (DescriptorUpdater &updater) C_NE___
	{
		CHECK_ERR( updater.BindBuffer( UniformName{"un_Nodes"},		_nodeBuffer ));
		CHECK_ERR( updater.BindBuffer( UniformName{"un_Materials"},	_materials ));
		CHECK_ERR( updater.BindBuffer( UniformName{"un_Lights"},	_lights ));
		return true;
	}

	bool  ModelGeomSource::Mesh::BindForRayTracing (DescriptorUpdater &updater) C_NE___
	{
		CHECK_ERR( updater.BindBuffer( UniformName{"un_Materials"},		_materials ));
		CHECK_ERR( updater.BindBuffer( UniformName{"un_RTInstances"},	_rtInstances ));
		CHECK_ERR( updater.BindBuffer( UniformName{"un_Lights"},		_lights ));
		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	void  ModelGeomSource::Mesh::Draw (DirectCtx::Draw &ctx, const Material::GPplnGroups_t &drawGroups) C_Th___
	{
		if_unlikely( _drawCalls.empty() )
			return;  // not uploaded yet

		BindBuffers( ctx );

		for (auto& [ppln, obj_ids] : drawGroups)
		{
			ctx.BindPipeline( ppln );

			for (auto id : obj_ids)
			{
				const auto&		dc = _drawCalls[id];

				ctx.DrawIndexed( dc.indexCount, 1u, dc.firstIndex, dc.vertexOffset, dc.nodeIdx );
			}
		}
	}

/*
=================================================
	BindBuffers
=================================================
*/
	void  ModelGeomSource::Mesh::BindBuffers (DirectCtx::Draw &ctx) C_Th___
	{
		const Bytes	pos_off		= 0_b;
		const Bytes	norm_off	= SizeOf<decltype(MeshDataInRAM::positions)::Value_t> * _meshVertexCount + pos_off;
		const Bytes	tex0_off	= SizeOf<decltype(MeshDataInRAM::normals  )::Value_t> * _meshVertexCount + norm_off;
		const Bytes	idx_off		= SizeOf<decltype(MeshDataInRAM::texcoord0)::Value_t> * _meshVertexCount + tex0_off;

		ctx.BindVertexBuffer( 0, _meshData, pos_off );
		ctx.BindVertexBuffer( 1, _meshData, norm_off );
		ctx.BindVertexBuffer( 2, _meshData, tex0_off );
		ctx.BindIndexBuffer( _meshData, idx_off, EIndex::UInt );
	}

/*
=================================================
	StateTransition
=================================================
*/
	template <typename Ctx>
	void  ModelGeomSource::Mesh::StateTransition (Ctx &ctx) C_Th___
	{
		EResourceState	stages;

		if constexpr( IsBaseOf< IGraphicsContext, Ctx >)
		{
			stages = EResourceState::AllGraphicsShaders;

			ctx.ResourceState( _meshData,	EResourceState::VertexBuffer );
			ctx.ResourceState( _meshData,	EResourceState::IndexBuffer );
		}
		if constexpr( IsBaseOf< IRayTracingContext, Ctx >)
		{
			stages = EResourceState::RayTracingShaders;
		}

		ctx.ResourceState( _nodeBuffer,	EResourceState::ShaderStorage_Read | stages );
		ctx.ResourceState( _materials,	EResourceState::ShaderStorage_Read | stages );

		if ( _rtInstances )
		{
			ctx.ResourceState( _meshData,		EResourceState::ShaderAddress_Read | stages );
			ctx.ResourceState( _rtInstances,	EResourceState::ShaderStorage_Read | stages );
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ModelGeomSource::Textures::Textures (Renderer &r, RC<ResLoader::IntermScene> scene, ArrayView<Path> texSearchDirs, uint maxTextures) __Th___ :
		IResource{ r },
		_maxTextures{ maxTextures }
	{
		_uploadStatus.store( EUploadStatus::Completed );

		auto	dummy2d = Image::CreateDummy2D( r, "" ); // throw

		_albedoMaps.resize( _maxTextures, dummy2d );

		using EKey = ResLoader::IntermMaterial::EKey;

		for (auto& [mtr, idx] : scene->Materials())
		{
			if ( auto albedo = mtr->Get( EKey::PBR_BaseColor);
				 albedo != null and albedo->image and idx < _albedoMaps.size() )
			{
				_albedoMaps[idx] = Image::CreateAndLoad( r, albedo->image, albedo->name, Image::ELoadOpFlags::GenMipmaps, texSearchDirs );  // throw
			}
		}
	}

/*
=================================================
	Bind
----
	Samplers defined in 'InitPipelineLayout()' in
	[file](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/ModelReflection.as)
=================================================
*/
	bool  ModelGeomSource::Textures::Bind (DescriptorUpdater &updater) C_NE___
	{
		Array<ImageViewID>	views;
		views.resize( _maxTextures );

		const auto	BindImages = [&views, &updater] (UniformName::Ref un, ArrayView<RC<Image>> images) -> bool
		{{
			CHECK_ERR( views.size() == images.size() );

			for (usize i = 0; i < views.size(); ++i) {
				views[i] = images[i]->GetViewId();
			}
			return updater.BindImages( un, views );
		}};

		CHECK_ERR( BindImages( UniformName{"un_AlbedoMaps"}, _albedoMaps ));
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ModelGeomSource::ModelGeomSource (Renderer						&r,
									  RC<ResLoader::IntermScene>	scene,
									  const Transformation			&initialTransform,
									  ArrayView<Path>				texSearchDirs,
									  uint							maxTextures,
									  RTGeometryTypes_t &&			rtGeoms) __Th___ :
		IGeomSource{ r },
		_meshData{ new Mesh{ r, scene, initialTransform, RVRef(rtGeoms) }},
		_textures{ new Textures{ r, scene, texSearchDirs, maxTextures }}
	{
		r.GetDataTransferQueue().EnqueueForUpload( _meshData );
		r.GetDataTransferQueue().EnqueueForUpload( _textures );
	}

/*
=================================================
	destructor
=================================================
*/
	ModelGeomSource::~ModelGeomSource ()
	{
	}

/*
=================================================
	StateTransition
=================================================
*/
	void  ModelGeomSource::StateTransition (IGSMaterials &, DirectCtx::Graphics &ctx) __Th___
	{
		_meshData->StateTransition( ctx );
	}

	void  ModelGeomSource::StateTransition (DirectCtx::RayTracing &ctx) __Th___
	{
		_meshData->StateTransition( ctx );
	}

/*
=================================================
	Draw
=================================================
*/
	bool  ModelGeomSource::Draw (const DrawData &in) __Th___
	{
		auto&			ctx		= in.ctx;
		auto&			mtr		= RefCast<Material>(in.mtr);
		DescriptorSetID	mtr_ds	= mtr.descSets[ ctx.GetFrameId().Index() ];

		Visit( mtr.drawGroups,
			[&] (Material::GPplnGroups_t const &drawGroups)
			{
				ctx.BindPipeline( drawGroups.begin()->first );
				ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
				ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );

				_meshData->Draw( ctx, drawGroups );
			},
			[] (Material::MPplnGroups_t const &) {
				CHECK_MSG( false, "mesh pipeline is not supported" );
			},
			[] (NullUnion) {}
		);
		return true;
	}

/*
=================================================
	PostProcess
=================================================
*/
	bool  ModelGeomSource::PostProcess (const DrawData &in) __Th___
	{
		auto&			ctx		= in.ctx;
		auto&			mtr		= RefCast<Material>(in.mtr);
		DescriptorSetID	mtr_ds	= mtr.descSets[ ctx.GetFrameId().Index() ];

		Visit( mtr.drawGroups,
			[&] (Material::GPplnGroups_t const &drawGroups)
			{
				ASSERT( drawGroups.size() == 1 );
				ctx.BindPipeline( drawGroups.begin()->first );
				ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
				ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );

				ctx.Draw( 3 );
			},
			[] (Material::MPplnGroups_t const &) {
				CHECK_MSG( false, "mesh pipeline is not supported" );
			},
			[] (NullUnion) {}
		);
		return true;
	}

/*
=================================================
	Update
=================================================
*/
	bool  ModelGeomSource::Update (const UpdateData &in) __Th___
	{
		auto&	ctx	= in.ctx;
		auto&	mtr = RefCast<Material>(in.mtr);

		if ( IsNullUnion( mtr.drawGroups ))
			return true;

		// update descriptors
		{
			DescriptorUpdater	updater;
			DescriptorSetID		mtr_ds	= mtr.descSets[ ctx.GetFrameId().Index() ];

			CHECK_ERR( updater.Set( mtr_ds, EDescUpdateMode::Partialy ));

			CHECK_ERR( _meshData->BindForGraphics( updater ));
			CHECK_ERR( _textures->Bind( updater ));

			CHECK_ERR( updater.Flush() );
		}
		return true;
	}

/*
=================================================
	RTUpdate
=================================================
*/
	bool  ModelGeomSource::RTUpdate (const UpdateRTData &in) __Th___
	{
		CHECK_ERR( _meshData->BindForRayTracing( in.updater ));
		CHECK_ERR( _textures->Bind( in.updater ));

		return true;
	}


} // AE::ResEditor
