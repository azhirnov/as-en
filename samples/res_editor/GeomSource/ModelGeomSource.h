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
        friend class ScriptSceneGeometry;

    // types
    private:

        //
        // Material
        //
        class Material final : public IGSMaterials
        {
        // types
        private:
            using PplnID_t      = Union< NullUnion, GraphicsPipelineID, MeshPipelineID >;
            using PplnPerDraw_t = Array< PplnID_t >;


        // variables
        public:
            RenderTechPipelinesPtr      rtech;

            GraphicsPipelineID          ppln;
            PerFrameDescSet_t           descSets;

            DescSetBinding              passDSIndex;
            DescSetBinding              mtrDSIndex;

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
            struct MeshData
            {
                StructView< packed_float3 >     positions;
                StructView< packed_float3 >     normals;
                StructView< packed_float2 >     texcoord0;
                StructView< uint >              indices;

                ND_ Bytes  DataSize ()  const;
            };

            struct MeshInfo
            {
                uint        indexCount;
                Bytes       positions;
                Bytes       normals;
                Bytes       texcoords;
                Bytes       indices;
            };

            struct DrawCall
            {
                float4x4    transform;
                uint        nodeIdx;
                uint        meshIdx;
                uint        materialIdx;
            };


        // variables
        private:
            Strong<BufferID>            _meshData;      // vertices and indices
            Strong<BufferID>            _meshInfo;      // link to vertices and indices
            Strong<BufferID>            _nodeBuffer;

            Array<DrawCall>             _drawCalls;
            Array<MeshInfo>             _meshInfoArr;

            PushConstantIndex           _pcIndex;

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

            void            StateTransition (GraphicsCtx_t &)                       C_NE___;
            void            Draw (DirectCtx::Draw &, const float4x4 &)              __Th___;

            bool            Resize (TransferCtx_t &)                                __Th_OV { return true; }
            bool            RequireResize ()                                        C_Th_OV { return false; }
            EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV;
            EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Canceled; }

            ND_ BufferID    ModelNodeArray ()                                       C_NE___ { return _meshInfo; }
            ND_ BufferID    ModelMeshArray ()                                       C_NE___ { return _nodeBuffer; }

        private:
            ND_ static MeshData  _Convert (const ResLoader::IntermMesh &)           __Th___;

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


        // methods
        public:
            Textures (Renderer &r, RC<ResLoader::IntermScene> scene)                __Th___;

            bool            Resize (TransferCtx_t &)                                __Th_OV { return true; }
            bool            RequireResize ()                                        C_Th_OV { return false; }
            EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV;
            EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Canceled; }
        };


    // variables
    private:
        RC<Mesh>                    _meshData;
        RC<Textures>                _textures;


    // methods
    public:
        ModelGeomSource (Renderer &r, RC<ResLoader::IntermScene> scene,
                         const float4x4 &initialTransform)                  __Th___;
        ~ModelGeomSource ();


    // IGeomSource //
        void  StateTransition (IGSMaterials &, GraphicsCtx_t &)             __NE_OV;
        void  StateTransition (IGSMaterials &, RayTracingCtx_t &)           __NE_OV;

        bool  Draw (const DrawData &)                                       __NE_OV;
        bool  Update (const UpdateData &)                                   __NE_OV;
    };


} // AE::ResEditor
