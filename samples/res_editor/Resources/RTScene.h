// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"
#include "res_editor/Resources/ResourceQueue.h"

namespace AE::ResEditor
{

    //
    // RayTracing Geometry
    //

    class RTGeometry final : public IResource
    {
        friend class ScriptRTGeometry;

    // types
    private:
        struct TriangleMesh : RTGeometryBuild::TrianglesInfo
        {
            RC<Buffer>      vbuffer;
            RC<Buffer>      ibuffer;
            ulong           vbufferVer      = 0;
            ulong           ibufferVer      = 0;
            Bytes32u        vertexStride;
            Bytes           vertexDataOffset;
            Bytes           indexDataOffset;
            bool            isMutable       = false;

            void  operator = (const RTGeometryBuild::TrianglesInfo &rhs)    { TrianglesInfo::operator = (rhs); }
        };
        using TriangleMeshes_t  = Array< TriangleMesh >;

        using Allocator_t       = LinearAllocator<>;
        using GeometryIDs_t     = StaticArray< Strong<RTGeometryID>, 2 >;
        using GeomDevAddr_t     = StaticArray< VDeviceAddress, 2 >;


    // variables
    private:
        GeometryIDs_t               _geomIds;
        GeomDevAddr_t               _address;
        Strong<BufferID>            _scratchBuffer;

        TriangleMeshes_t            _triangleMeshes;

        Mutex                       _loadOpGuard;
        Atomic<EUploadStatus>       _uploadStatus   {EUploadStatus::Canceled};

        Atomic<ulong>               _version        {0};
        bool                        _isMutable      = false;
        const ERTASOptions          _options        = ERTASOptions::PreferFastBuild;

        const String                _dbgName;


    // methods
    public:
        RTGeometry (TriangleMeshes_t    triangleMeshes,
                    Renderer &          renderer,
                    StringView          dbgName)                                __Th___;

        ~RTGeometry () override;

        ND_ RTGeometryID    GetGeometryId (FrameUID fid)                        const   { return _geomIds[ ulong(fid.Unique()) & 1 ]; }
        ND_ VDeviceAddress  GetDeviceAddress (FrameUID fid)                     const   { return _address[ ulong(fid.Unique()) & 1 ]; }

        ND_ bool            BuildIndirect (DirectCtx::ASBuild &)                __Th___;

        ND_ ulong           GetVersion ()                                       const   { return _version.load(); }
            void            Validate ()                                         const;

        ND_ bool            IsMutable ()                                        const   { return false; }


    // IResource //
        bool            Resize (TransferCtx_t &)                                __Th_OV { return false; }
        EUploadStatus   GetStatus ()                                            C_NE_OV { return _uploadStatus.load(); }
        EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV;
        EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Complete; }
        void            Cancel ()                                               __Th_OV {}


    private:
        ND_ bool        _GetTriangles (OUT RTGeometryBuild &, Allocator_t &)    const;
        ND_ bool        _Build (DirectCtx::ASBuild &);
    };



    //
    // RayTracing Scene
    //

    class RTScene final : public IResource
    {
        friend class ScriptRTScene;

    // types
    private:
        struct Instance
        {
            RC<RTGeometry>      geometry;
            float3x4            transform;
            uint                instanceCustomIndex;
            uint                mask;
            uint                instanceSBTOffset;
            ERTInstanceOpt      flags;
        };

        using Instances_t       = Array< Instance >;
        using GeomVerMap_t      = FlatHashMap< RC<RTGeometry>, ulong >;
        using SceneIDs_t        = StaticArray< Strong<RTSceneID>, 2 >;
        using InstanceBufIDs_t  = StaticArray< Strong<BufferID>, 2 >;


    // variables
    private:
        SceneIDs_t              _sceneIds;
        Strong<BufferID>        _scratchBuffer;
        InstanceBufIDs_t        _instanceBuffers;

        Instances_t             _instances;
        GeomVerMap_t            _uniqueGeometries;

        Mutex                   _loadOpGuard;
        Atomic<EUploadStatus>   _uploadStatus   {EUploadStatus::Canceled};

        const ERTASOptions      _options        = ERTASOptions::PreferFastBuild;

        const String            _dbgName;


    // methods
    public:
        RTScene (Instances_t    instances,
                 Renderer &     renderer,
                 StringView     dbgName)                                        __Th___;

        ~RTScene () override;

        ND_ RTSceneID   GetSceneId (FrameUID fid)                               const   { return _sceneIds[ ulong(fid.Unique()) & 1 ]; }
            void        Validate ()                                             const;

        ND_ bool        BuildIndirect (DirectCtx::ASBuild &)                    __Th___;


    // IResource //
        bool            Resize (TransferCtx_t &)                                __Th_OV { return false; }
        EUploadStatus   GetStatus ()                                            C_NE_OV { return _uploadStatus.load(); }
        EUploadStatus   Upload (TransferCtx_t &)                                __Th_OV;
        EUploadStatus   Readback (TransferCtx_t &)                              __Th_OV { return EUploadStatus::Complete; }
        void            Cancel ()                                               __Th_OV {}

    private:
        ND_ bool        _Resize ();
        ND_ bool        _Build (DirectCtx::ASBuild &);
        ND_ bool        _UploadInstances (TransferCtx_t &);
    };


} // AE::ResEditor
