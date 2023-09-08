// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/GeomSource/ModelGeomSource.h"
#include "res_editor/Passes/Renderer.h"
#include "res_editor/Resources/Image.h"

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
    MeshDataInRAM::DataSize
=================================================
*/
    Bytes  ModelGeomSource::Mesh::MeshDataInRAM::DataSize () const
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
        _intermScene{ scene }, _initialTransform{ initialTransform }
    {
        _uploadStatus.store( EUploadStatus::InProgress );

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

        _meshInfoArr.resize( mesh_cnt );

        _meshDataSize   = mesh_data_size;
        _meshData       = res_mngr.CreateBuffer( BufferDesc{ mesh_data_size,
                                                             EBufferUsage::Storage | EBufferUsage::TransferDst |
                                                             EBufferUsage::Vertex | EBufferUsage::Index },
                                                 "Vertices & Indices", r.GetAllocator() );
        CHECK_THROW( _meshData );

        _nodeDataSize   = SizeOf<ShaderTypes::ModelNode> * node_cnt;
        _nodeBuffer     = res_mngr.CreateBuffer( BufferDesc{ _nodeDataSize,
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

        for (auto& [mesh, idx] : _intermScene->Meshes())
        {
            const MeshDataInRAM md      = _Convert( *mesh );
            MeshDataInGPU&      info    = _meshInfoArr[idx];

            info.indexCount = uint(md.indices.size());
            info.positions  = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.positions )) return EUploadStatus::NoMemory;
            info.normals    = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.normals ))   return EUploadStatus::NoMemory;
            info.texcoords  = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.texcoord0 )) return EUploadStatus::NoMemory;
            info.indices    = mesh_data_off;    if ( not _UploadData( INOUT mesh_data_off, ctx, md.indices ))   return EUploadStatus::NoMemory;
        }

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

        if ( view.empty() )
            return true;

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
            [this, &nodes] (StringView, const ResLoader::IntermScene::NodeData_t &data, const TTransformation<float> &tr)
            {
                Visit( data,
                    [this, &tr, &nodes] (const ResLoader::IntermScene::ModelData &model)
                    {
                        auto    mesh    = model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
                        auto    mtr     = model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;
                        auto&   node    = nodes.emplace_back();

                        CHECK_THROW( mesh and mtr );

                        node.transform      = _initialTransform * tr.ToMatrix();
                        node.normalMat      = float3x3{node.transform};
                        node.meshIdx        = _intermScene->IndexOfMesh( mesh );
                        node.materialIdx    = _intermScene->IndexOfMaterial( mtr );

                        auto&       draw    = _drawCalls.emplace_back();
                        draw.nodeIdx        = uint(nodes.size()-1);
                        draw.meshIdx        = node.meshIdx;
                    },
                    [] (const NullUnion &) {}
                );
                return true;
            });

        CHECK_ERR( _nodeDataSize == ArraySizeOf(nodes) );
        CHECK_ERR( mem_view.CopyFrom( nodes ) == _nodeDataSize );

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

        MeshDataInRAM   result;
        result.positions    = mesh.GetData< packed_float3 >( ResLoader::VertexAttributeName::Position );
        result.normals      = mesh.GetDataOpt< packed_float3 >( ResLoader::VertexAttributeName::Normal );
        result.texcoord0    = mesh.GetDataOpt< packed_float2 >( ResLoader::VertexAttributeName::TextureUVs[0] );
        result.indices      = mesh.GetIndexData< uint >();

        CHECK_THROW( not result.positions.empty() and not result.indices.empty() );

        ASSERT( result.normals.empty() or result.positions.size() == result.normals.size() );
        ASSERT( result.texcoord0.empty() or result.positions.size() == result.texcoord0.size() );

        return result;
    }

/*
=================================================
    Bind
=================================================
*/
    bool  ModelGeomSource::Mesh::Bind (DescriptorUpdater &updater) C_NE___
    {
        CHECK_ERR( updater.BindBuffer( UniformName{"un_Nodes"}, _nodeBuffer ));
        return true;
    }

/*
=================================================
    Draw
=================================================
*/
    void  ModelGeomSource::Mesh::Draw (DirectCtx::Draw &ctx, PushConstantIndex pcIndex, ArrayView<GraphicsPipelineID> pipelines) __Th___
    {
        // TODO: multidraw / draw indirect ?

        CHECK_ERRV( _drawCalls.size() <= pipelines.size() );

        for (usize i = 0; i < _drawCalls.size(); ++i)
        {
            const auto&     dc      = _drawCalls[i];
            const auto&     mesh    = _meshInfoArr[ dc.meshIdx ];
            auto            ppln    = pipelines[i];

            ShaderTypes::model_pc   pc;
            pc.nodeIdx  = dc.nodeIdx;

            ctx.BindPipeline( ppln );
            ctx.PushConstant( pcIndex, pc );

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
        ctx.ResourceState( _nodeBuffer, EResourceState::ShaderStorage_Read | EResourceState::AllGraphicsShaders );
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
        _uploadStatus.store( EUploadStatus::Complete );

        auto    dummy2d = Image::CreateDummy2D( r, "" ); // throw

        _albedoMaps.resize( _maxTextures, dummy2d );

        for (auto& [mtr, idx] : scene->Materials())
        {
            if ( auto* albedo = UnionGet< ResLoader::IntermMaterial::MtrTexture >( mtr->GetSettings().albedo );
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
    file:///<path>/AE/samples/res_editor/_data/pipelines/ModelShared.as
=================================================
*/
    bool  ModelGeomSource::Textures::Bind (DescriptorUpdater &updater) C_NE___
    {
        Array<ImageViewID>  views;
        views.resize( _maxTextures );

        const auto  BindImages = [this, &views, &updater] (const UniformName &un, ArrayView<RC<Image>> images) -> bool
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

/*
=================================================
    Upload
=================================================
*
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
    ModelGeomSource::ModelGeomSource (Renderer                      &r,
                                      RC<ResLoader::IntermScene>    scene,
                                      const float4x4                &initialTransform,
                                      ArrayView<Path>               texSearchDirs,
                                      uint                          maxTextures) __Th___ :
        IGeomSource{ r },
        _meshData{ new Mesh{ r, scene, initialTransform }},
        _textures{ new Textures{ r, scene, texSearchDirs, maxTextures }}
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

        ctx.BindPipeline( mtr.pplns[0] );
        ctx.BindDescriptorSet( mtr.passDSIndex, in.passDS );
        ctx.BindDescriptorSet( mtr.mtrDSIndex,  mtr_ds );

        _meshData->Draw( ctx, mtr.pcIndex, mtr.pplns );

        return true;
    }

/*
=================================================
    Update
=================================================
*/
    bool  ModelGeomSource::Update (const UpdateData &in) __NE___
    {
        auto&   ctx = in.ctx;
        auto&   mtr = RefCast<Material>(in.mtr);

        // update descriptors
        {
            DescriptorUpdater   updater;
            DescriptorSetID     mtr_ds  = mtr.descSets[ ctx.GetFrameId().Index() ];

            CHECK_ERR( updater.Set( mtr_ds, EDescUpdateMode::Partialy ));

            CHECK_ERR( _meshData->Bind( updater ));
            CHECK_ERR( _textures->Bind( updater ));

            CHECK_ERR( updater.Flush() );
        }
        return true;
    }


} // AE::ResEditor
