// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/ModelGeomSource.h"
#include "res_editor/Passes/Renderer.h"

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
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        res_mngr.ReleaseResourceArray( INOUT descSets );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    MeshData::DataSize
=================================================
*/
    Bytes  ModelGeomSource::Mesh::MeshData::DataSize () const
    {
        return  positions.DataSize() +
                normals.DataSize() +
                texcoord0.DataSize() +
                indices.DataSize();
    }

/*
=================================================
    constructor
=================================================
*/
    ModelGeomSource::Mesh::Mesh (Renderer &r, RC<ResLoader::IntermScene> scene, const float4x4 &initialTransform) __Th___ :
        IResource{ r },
        _pcIndex{ 0_b, EShader::Vertex },
        _intermScene{ scene }, _initialTransform{ initialTransform }
    {
        auto&           res_mngr        = RenderGraph().GetStateTracker();
        const usize     mesh_cnt        = _intermScene->Meshes().size();
        const usize     node_cnt        = _intermScene->ModelNodeCount();
        Bytes           mesh_data_size;

        for (auto& [mesh, idx] : _intermScene->Meshes())
        {
            mesh_data_size += _Convert( *mesh ).DataSize();
        }

        CHECK_THROW( mesh_data_size > 0 );
        CHECK_THROW( mesh_cnt > 0 );
        CHECK_THROW( node_cnt > 0 );

        _meshDataSize = mesh_data_size;
        _meshData = res_mngr.CreateBuffer( BufferDesc{ mesh_data_size,
                                                       EBufferUsage::Storage | EBufferUsage::TransferDst |
                                                       EBufferUsage::Vertex | EBufferUsage::Index },
                                            "Vertices & Indices", r.GetAllocator() );
        CHECK_THROW( _meshData );

        /*_meshInfoSize = SizeOf<ShaderTypes::ModelMesh> * mesh_cnt;
        _meshInfo = res_mngr.CreateBuffer( BufferDesc{ _meshInfoSize,
                                                       EBufferUsage::Storage | EBufferUsage::TransferDst },
                                            "ModelMeshes", r.GetAllocator() );
        CHECK_THROW( _meshInfo );*/

        _nodeDataSize = SizeOf<ShaderTypes::ModelNode> * node_cnt;
        _nodeBuffer = res_mngr.CreateBuffer( BufferDesc{ _nodeDataSize,
                                                         EBufferUsage::Storage | EBufferUsage::TransferDst },
                                             "ModelNodes", r.GetAllocator() );
        CHECK_THROW( _nodeBuffer );
    }

/*
=================================================
    destructor
=================================================
*/
    ModelGeomSource::Mesh::~Mesh ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        res_mngr.ReleaseResources( _meshData, _meshInfo, _nodeBuffer );
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

        Bytes   mesh_data_off;
        Bytes   mesh_info_off;
        usize   mesh_info_idx   = 0;

        //const DeviceAddress           addr            = ctx.GetResourceManager().GetDeviceAddress( _meshData );
        //const usize                   mesh_info_cnt   = 10;
        //Array<ShaderTypes::ModelMesh> mesh_infos;     mesh_infos.reserve( mesh_info_cnt );

        for (auto& [mesh, idx] : _intermScene->Meshes())
        {
            const MeshData          md  = _Convert( *mesh );
            /*ShaderTypes::ModelMesh    info;
            info.vertexCount= uint(md.positions.size());
            info.indexCount = uint(md.indices.size());
            info.positions  = addr + mesh_data_off;
            info.normals    = addr + mesh_data_off;
            info.texcoords  = addr + mesh_data_off;
            info.indices    = addr + mesh_data_off;*/

            MeshInfo    info;
            info.indexCount = uint(md.indices.size());
            info.positions  = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.positions )) return EUploadStatus::NoMemory;
            info.normals    = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.normals ))   return EUploadStatus::NoMemory;
            info.texcoords  = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.texcoord0 )) return EUploadStatus::NoMemory;
            info.indices    = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.indices ))   return EUploadStatus::NoMemory;

            if ( mesh_info_idx == _meshInfoArr.size() )
            {
                _meshInfoArr.push_back( info );
            }
            ++mesh_info_idx;

            /*if ( _meshInfoOffset < mesh_info_off )
            {
                mesh_infos.push_back( info );
                if ( mesh_infos.size() >= mesh_info_cnt )
                {
                    if ( not _UploadInfo( INOUT mesh_info_off, ctx, INOUT mesh_infos ))
                        return EUploadStatus::NoMemory;
                }
            }*/
        }

        ASSERT( _meshInfoArr.size() == _intermScene->Meshes().size() );

        //if ( not _UploadInfo( INOUT mesh_info_off, ctx, INOUT mesh_infos ))
        //  return EUploadStatus::NoMemory;

        if ( not _UploadNodes( ctx ))
            return EUploadStatus::NoMemory;

        _uploadStatus.store( EUploadStatus::Complete );
        return EUploadStatus::Complete;
    }

/*
=================================================
    _UploadData
=================================================
*/
    template <typename T>
    bool  ModelGeomSource::Mesh::_UploadData (INOUT Bytes &dstOffset, TransferCtx_t &ctx, StructView<T> view) __Th___
    {
        const Bytes dst_off = dstOffset;
        dstOffset += view.DataSize();

        if ( not IsIntersects( _meshDataOffset, _meshDataSize, dst_off, dst_off + view.DataSize() ))
            return true;    // skip

        BufferMemView   mem_view;
        ctx.UploadBuffer( _meshData, _meshDataOffset, view.DataSize(), OUT mem_view, EStagingHeapType::Dynamic );

        if ( mem_view.DataSize() < view.DataSize() )
            return false;   // out of memory

        _meshDataOffset += view.DataSize();

        auto    it = view.begin();
        for (auto& part : mem_view.Parts())
        {
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
    _UploadInfo
=================================================
*
    template <typename T>
    bool  ModelGeomSource::Mesh::_UploadInfo (INOUT Bytes &dstOffset, TransferCtx_t &ctx, INOUT Array<T> &arr) __Th___
    {
        const Bytes     arr_size    = ArraySizeOf(arr);
        const Bytes     dst_off     = dstOffset;
        dstOffset += arr_size;

        if ( not IsIntersects( _meshInfoOffset, _meshInfoSize, dst_off, dst_off + arr_size ))
            return true;    // skip

        BufferMemView   mem_view;
        ctx.UploadBuffer( _meshInfo, _meshInfoOffset, _meshInfoSize, OUT mem_view, EStagingHeapType::Dynamic );

        if ( mem_view.DataSize() < arr_size )
            return false;   // out of memory

        _meshInfoOffset += arr_size;

        Unused( mem_view.CopyFrom( arr ));
        arr.clear();

        return true;
    }

/*
=================================================
    _UploadNodes
=================================================
*/
    bool  ModelGeomSource::Mesh::_UploadNodes (TransferCtx_t &ctx) __Th___
    {
        BufferMemView   mem_view;
        ctx.UploadBuffer( _nodeBuffer, 0_b, _nodeDataSize, OUT mem_view, EStagingHeapType::Dynamic );

        if ( mem_view.DataSize() < _nodeDataSize )
            return false;   // out of memory

        Array<ShaderTypes::ModelNode>   nodes;
        nodes.reserve( usize(_nodeDataSize / SizeOf<ShaderTypes::ModelNode>) );

        _intermScene->ForEachNode(
            [this, &nodes] (StringView, const ResLoader::IntermScene::NodeData_t &data, const Transformation<float> &tr)
            {
                Visit( data,
                    [this, &tr, &nodes] (const ResLoader::IntermScene::ModelData &model)
                    {
                        auto    mesh    = model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
                        auto    mtr     = model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
                        auto&   node    = nodes.emplace_back();

                        CHECK_THROW( mesh and mtr );

                        node.transform      = _initialTransform * tr.ToMatrix();
                        node.meshIdx        = _intermScene->IndexOfMesh( mesh );
                        node.materialIdx    = _intermScene->IndexOfMaterial( mtr );

                        auto&       draw    = _drawCalls.emplace_back();
                        draw.transform      = _initialTransform * tr.ToMatrix();
                        draw.meshIdx        = _intermScene->IndexOfMesh( mesh );
                        draw.materialIdx    = _intermScene->IndexOfMaterial( mtr );
                        draw.nodeIdx        = uint(nodes.size()-1);
                    },
                    [] (const NullUnion &) {}
                );
                return true;
            });

        CHECK( _nodeDataSize == ArraySizeOf(nodes) );

        Unused( mem_view.CopyFrom( nodes ));
        return true;
    }

/*
=================================================
    _Convert
=================================================
*/
    ModelGeomSource::Mesh::MeshData  ModelGeomSource::Mesh::_Convert (const ResLoader::IntermMesh &mesh) __Th___
    {
        CHECK_THROW_MSG( mesh.Attribs() != null );

        MeshData    result;
        result.positions    = mesh.GetData< packed_float3 >( ResLoader::VertexAttributeName::Position );
        result.normals      = mesh.GetData< packed_float3 >( ResLoader::VertexAttributeName::Normal );
        result.texcoord0    = mesh.GetData< packed_float2 >( ResLoader::VertexAttributeName::TextureUVs[0] );
        result.indices      = mesh.GetIndexData< uint >();

        ASSERT( result.positions.size() == result.normals.size() );
        ASSERT( result.positions.size() == result.texcoord0.size() );

        return result;
    }

/*
=================================================
    Draw
=================================================
*/
    void  ModelGeomSource::Mesh::Draw (DirectCtx::Draw &ctx, const float4x4 &cameraPos) __Th___
    {
        // TODO: multidraw / draw indirect ?

        for (auto& d : _drawCalls)
        {
            const auto& mesh = _meshInfoArr[d.meshIdx];

            ShaderTypes::ModelNode  pc;
            pc.transform    = cameraPos * d.transform;
            pc.meshIdx      = d.meshIdx;
            pc.materialIdx  = d.materialIdx;

            ctx.PushConstant( _pcIndex, pc );

            ctx.BindVertexBuffer( 0, _meshData, mesh.positions );
            ctx.BindVertexBuffer( 1, _meshData, mesh.normals );
            ctx.BindVertexBuffer( 2, _meshData, mesh.texcoords );
            ctx.BindIndexBuffer( _meshData, mesh.indices, EIndex::UInt );

            ctx.DrawIndexed( mesh.indexCount );
        }
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  ModelGeomSource::Mesh::StateTransition (GraphicsCtx_t &ctx) C_NE___
    {
        ctx.ResourceState( _meshData,   EResourceState::VertexBuffer );
        ctx.ResourceState( _meshData,   EResourceState::IndexBuffer );

    //  ctx.ResourceState( _meshData,   EResourceState::ShaderAddress_Read | EResourceState::AllGraphicsShaders );
    //  ctx.ResourceState( _meshInfo,   EResourceState::ShaderStorage_Read | EResourceState::AllGraphicsShaders );
    //  ctx.ResourceState( _nodeBuffer, EResourceState::ShaderStorage_Read | EResourceState::AllGraphicsShaders );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ModelGeomSource::Textures::Textures (Renderer &r, RC<ResLoader::IntermScene> scene) __Th___ :
        IResource{ r }
    {}

/*
=================================================
    Upload
=================================================
*/
    IResource::EUploadStatus  ModelGeomSource::Textures::Upload (TransferCtx_t &) __Th___
    {
        if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
            return stat;


        _uploadStatus.store( EUploadStatus::Complete );
        return EUploadStatus::Complete;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ModelGeomSource::ModelGeomSource (Renderer &r, RC<ResLoader::IntermScene> scene, const float4x4 &initialTransform) __Th___ :
        IGeomSource{ r },
        _meshData{ new Mesh{ r, scene, initialTransform }},
        _textures{ new Textures{ r, scene }}
    {
        r.GetResourceQueue().EnqueueForUpload( _meshData );
        r.GetResourceQueue().EnqueueForUpload( _textures );
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
    void  ModelGeomSource::StateTransition (IGSMaterials &, GraphicsCtx_t &ctx) __NE___
    {
        _meshData->StateTransition( ctx );
    }

/*
=================================================
    StateTransition
=================================================
*/
    void  ModelGeomSource::StateTransition (IGSMaterials &, RayTracingCtx_t &) __NE___
    {
    }

/*
=================================================
    Draw
=================================================
*/
    bool  ModelGeomSource::Draw (const DrawData &in) __NE___
    {
        auto&           ctx     = in.ctx;
        auto&           mtr     = RefCast<Material>(in.mtr);
        DescriptorSetID mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];
        const float4x4  c_pos   = float4x4::Translated( in.position );

        ctx.BindPipeline( mtr.ppln );
        ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
        ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );

        _meshData->Draw( ctx, c_pos );

        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  ModelGeomSource::Update (const UpdateData &) __NE___
    {
        /*auto& ctx = in.ctx;
        auto&   mtr = Cast<Material>(in.mtr);

        // update descriptors
        {
            DescriptorUpdater   updater;
            DescriptorSetID     mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

            CHECK_ERR( updater.Set( mtr_ds, EDescUpdateMode::Partialy ));

            updater.BindBuffer( UniformName{"un_Nodes"}, _meshData->ModelNodeArray() );
            updater.BindBuffer( UniformName{"un_Meshes"}, _meshData->ModelMeshArray() );

            CHECK_ERR( updater.Flush() );
        }*/
        return true;
    }


} // AE::ResEditor
