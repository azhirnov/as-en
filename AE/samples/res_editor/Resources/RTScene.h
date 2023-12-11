// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"
#include "res_editor/Resources/DataTransferQueue.h"
#include "res_editor/Resources/ContentVersion.h"

namespace AE::ResEditor
{

    //
    // RayTracing Geometry
    //

    class RTGeometry final : public IResource
    {
        friend class ScriptRTGeometry;

    // types
    public:
        enum class EBuildMode : uint
        {
            Direct,
            Indirect,
            IndirectEmulated,
        };

        struct TriangleMesh : RTGeometryBuild::TrianglesInfo
        {
            RC<Buffer>      vbuffer;
            RC<Buffer>      ibuffer;
            Byte32u         vertexStride;
            Bytes           vertexDataOffset;
            Bytes           indexDataOffset;

            void  operator = (const RTGeometryBuild::TrianglesInfo &rhs)    { TrianglesInfo::operator = (rhs); }
        };
        using TriangleMeshes_t  = Array< TriangleMesh >;

    private:
        using Allocator_t       = LinearAllocator<>;


    // variables
    private:
        StrongAtom<RTGeometryID>        _geomId;
        Strong<BufferID>                _scratchBuffer;         // can be null

        RC<Buffer>                      _indirectBuffer;
        Strong<BufferID>                _indirectBufferHostVis; // ASBuildIndirectCommand [GeometryCount * MaxFrames]
        ASBuildIndirectCommand const*   _indirectBufferMem      = null;

        TriangleMeshes_t                _triangleMeshes;

        ContentVersion                  _version;
        const bool                      _isMutable              = false;
        const ERTASOptions              _options                = ERTASOptions::PreferFastBuild;

        const String                    _dbgName;


    // methods
    public:
        RTGeometry (TriangleMeshes_t    triangleMeshes,
                    RC<Buffer>          indirectBuffer,
                    Renderer &          renderer,
                    StringView          dbgName,
                    Bool                allowUpdate)                                __Th___;

        RTGeometry (Renderer &          renderer,
                    StringView          dbgName)                                    __Th___;

        ~RTGeometry () override;

        ND_ RTGeometryID    GetGeometryId (FrameUID)                                const   { return _geomId.Get(); }

        ND_ ulong           GetVersion (uint fid)                                   const   { return _version.Get( fid ); }
        ND_ ulong           GetVersion (FrameUID fid)                               const   { return _version.Get( fid ); }

        ND_ bool            Build (DirectCtx::ASBuild &, EBuildMode)                __Th___;

            void            Reset (Strong<RTGeometryID> geomId);
            void            CompleteUploading ();

        ND_ bool            IsMutable ()                                            const   { return _isMutable; }


    // IResource //
        bool                Resize (TransferCtx_t &)                                __Th_OV { return true; }
        bool                RequireResize ()                                        C_Th_OV { return false; }
        EUploadStatus       Upload (TransferCtx_t &)                                __Th_OV;
        EUploadStatus       Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Completed; }


    private:
        ND_ bool  _GetTriangles (OUT RTGeometryBuild &, FrameUID, Allocator_t &)            const;

        ND_ bool  _BuildIndirectEmulated (DirectCtx::ASBuild &, RTGeometryID, Allocator_t &) const;
    };



    //
    // RayTracing Scene
    //

    class RTScene final : public IResource
    {
        friend class ScriptRTScene;

    // types
    public:
        using EBuildMode        = RTGeometry::EBuildMode;

    private:
        struct Instance
        {
            RC<RTGeometry>      geometry;
            RTMatrixStorage     transform;
            uint                instanceCustomIndex;
            uint                mask;
            uint                instanceSBTOffset;
            ERTInstanceOpt      flags;
        };

        using Instances_t       = Array< Instance >;
        using GeomVerMap_t      = FlatHashMap< RC<RTGeometry>, ulong >;


    // variables
    private:
        Strong<RTSceneID>               _sceneId;
        Strong<BufferID>                _scratchBuffer;
        RC<Buffer>                      _instanceBuffer;

        RC<Buffer>                      _indirectBuffer;
        Strong<BufferID>                _indirectBufferHostVis; // ASBuildIndirectCommand [MaxFrames]
        ASBuildIndirectCommand const*   _indirectBufferMem      = null;

        Instances_t                     _instances;
        GeomVerMap_t                    _uniqueGeometries;

        const ERTASOptions              _options                = ERTASOptions::PreferFastBuild;
        const bool                      _isMutable              = false;

        const String                    _dbgName;


    // methods
    public:
        RTScene (Instances_t    instances,
                 RC<Buffer>     instanceBuffer,
                 RC<Buffer>     indirectBuffer,
                 Renderer &     renderer,
                 StringView     dbgName,
                 Bool           allowUpdate)                                    __Th___;

        ~RTScene () override;

        ND_ RTSceneID   GetSceneId (FrameUID)                                   const   { return _sceneId; }
            void        Validate (FrameUID fid)                                 const;

        ND_ bool        Build (DirectCtx::ASBuild &, EBuildMode)                __Th___;


    // IResource //
        bool            Resize (TransferCtx_t &)                                __Th_OV { return true; }
        bool            RequireResize ()                                        C_Th_OV { return false; }
        EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV;
        EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Completed; }

    private:
        ND_ bool        _Resize ();
        ND_ bool        _UploadInstances (TransferCtx_t &);

        ND_ bool        _BuildIndirectEmulated (DirectCtx::ASBuild &, RTSceneBuild &, RTSceneID) const;
    };


} // AE::ResEditor
