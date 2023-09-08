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

        //
        // Material
        //
        class Material final : public IGSMaterials
        {
        // types
        private:
            //using PplnID_t    = Union< NullUnion, GraphicsPipelineID, MeshPipelineID >;
            using PplnPerObj_t  = Array< GraphicsPipelineID >;


        // variables
        public:
            RenderTechPipelinesPtr      rtech;

            PplnPerObj_t                pplns;
            PerFrameDescSet_t           descSets;

            DescSetBinding              passDSIndex;
            DescSetBinding              mtrDSIndex;
            PushConstantIndex           pcIndex;

            Strong<BufferID>            ubuffer;


        // methods
        public:
            Material () {}
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
                StructView< packed_float3 >     positions;
                StructView< packed_float3 >     normals;
                StructView< packed_float2 >     texcoord0;
                StructView< uint >              indices;

                ND_ Bytes  DataSize ()  const;
            };

            struct MeshDataInGPU
            {
                uint        indexCount  = 0;
                Bytes       positions;
                Bytes       normals;
                Bytes       texcoords;
                Bytes       indices;
            };

            struct DrawCall
            {
                uint        nodeIdx;
                uint        meshIdx;
            };


        // variables
        private:
            Strong<BufferID>            _meshData;      // vertices and indices
            Strong<BufferID>            _meshInfo;      // link to vertices and indices
            Strong<BufferID>            _nodeBuffer;

            Array<DrawCall>             _drawCalls;
            Array<MeshDataInGPU>        _meshInfoArr;

            Bytes                       _meshDataOffset;
            Bytes                       _meshDataSize;
            Bytes                       _meshInfoOffset;
            Bytes                       _meshInfoSize;
            Bytes                       _nodeDataSize;

            RC<ResLoader::IntermScene>  _intermScene;
            const float4x4              _initialTransform;


        // methods
        public:
            Mesh (Renderer &r, RC<ResLoader::IntermScene> scene,
                  const float4x4 &initialTransform)                                 __Th___;
            ~Mesh ();

            ND_ bool  Bind (DescriptorUpdater &updater)                             C_NE___;

                void  StateTransition (GraphicsCtx_t &)                             C_NE___;
                void  Draw (DirectCtx::Draw                 &ctx,
                            PushConstantIndex               pcIndex,
                            ArrayView<GraphicsPipelineID>   pipelines)              __Th___;

            // IResource //
            bool            Resize (TransferCtx_t &)                                __Th_OV { return true; }
            bool            RequireResize ()                                        C_Th_OV { return false; }
            EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV;
            EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Canceled; }


        private:
            ND_ static MeshDataInRAM  _Convert (const ResLoader::IntermMesh &)      __Th___;

            template <typename T>
            ND_ bool  _UploadData (INOUT Bytes &, TransferCtx_t &, StructView<T>)   __Th___;

            template <typename T>
            ND_ bool  _UploadInfo (INOUT Bytes &, TransferCtx_t &, Array<T> &)      __Th___;

            ND_ bool  _UploadNodes (TransferCtx_t &)                                __Th___;
        };


        //
        // Textures
        //
        class Textures final : public IResource
        {
        // variables
        private:
            Array< RC<Image> >      _albedoMaps;
            const uint              _maxTextures;


        // methods
        public:
            Textures (Renderer                      &r,
                      RC<ResLoader::IntermScene>    scene,
                      ArrayView<Path>               texSearchDirs,
                      uint                          maxTextures)                    __Th___;

            ND_ bool  Bind (DescriptorUpdater &updater)                             C_NE___;

            // IResource //
            bool            Resize (TransferCtx_t &)                                __Th_OV { return true; }
            bool            RequireResize ()                                        C_Th_OV { return false; }
            EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV { return EUploadStatus::Complete; }
            EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Canceled; }
        };


    // variables
    private:
        RC<Mesh>            _meshData;
        RC<Textures>        _textures;


    // methods
    public:
        ModelGeomSource (Renderer                   &r,
                         RC<ResLoader::IntermScene> scene,
                         const float4x4             &initialTransform,
                         ArrayView<Path>            texSearchDirs,
                         uint                       maxTextures)            __Th___;
        ~ModelGeomSource ();


    // IGeomSource //
        void  StateTransition (IGSMaterials &, GraphicsCtx_t &)             __NE_OV;
        void  StateTransition (IGSMaterials &, RayTracingCtx_t &)           __NE_OV;

        bool  Draw (const DrawData &)                                       __NE_OV;
        bool  Update (const UpdateData &)                                   __NE_OV;
    };


} // AE::ResEditor
