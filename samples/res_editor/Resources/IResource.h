// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Common.h"
#include "res_editor/Dynamic/DynamicDimension.h"
#include "res_editor/Dynamic/DynamicVec.h"
#include "res_editor/Dynamic/DynamicScalar.h"

namespace AE::ResEditor
{
    class Buffer;
    class Image;
    class RTGeometry;
    class RTScene;
    class Renderer;
    class ResourceQueue;



    //
    // Resource interface
    //
    class IResource : public EnableRC< IResource >
    {
    // types
    public:
        enum class EUploadStatus : uint
        {
            Complete,
            Canceled,       // or failed
            InProgress,
            NoMemory,
        };

        using TransferCtx_t = RG::DirectCtx::Transfer;


    // variables
    private:
        Renderer &                  _renderer;
    protected:
        Atomic<EUploadStatus>       _uploadStatus   {EUploadStatus::Canceled};


    // interface
    public:
            virtual bool            Resize (TransferCtx_t &ctx) __Th___ = 0;

        // GPU <-> CPU
        ND_ virtual EUploadStatus   GetStatus ()                C_NE___ { return _uploadStatus.load(); }
        ND_ virtual EUploadStatus   Upload (TransferCtx_t &)    __Th___ = 0;
        ND_ virtual EUploadStatus   Readback (TransferCtx_t &)  __Th___ = 0;
            virtual void            Cancel ()                   __Th___;


    // methods
    protected:
        explicit IResource (Renderer &r) : _renderer{r} {}

        ND_ Renderer&           _Renderer ()            const   { return _renderer; }
        ND_ ResourceQueue&      _ResQueue ()            const;
        ND_ GfxMemAllocatorPtr  _GfxAllocator ()        const;
        ND_ GfxMemAllocatorPtr  _GfxDynamicAllocator () const;

        void  _SetUploadStatus (EUploadStatus);
    };



    //
    // Image Resource interface
    //
    class IImageResource : public IResource
    {
    // interface
    public:
        ND_ virtual ImageID         GetImageId ()       C_NE___ = 0;
        ND_ virtual ImageViewID     GetViewId ()        C_NE___ = 0;


    protected:
        explicit IImageResource (Renderer &r) : IResource{r} {}
    };
//-----------------------------------------------------------------------------



/*
=================================================
    Cancel
=================================================
*/
    inline void  IResource::Cancel () __Th___
    {
        for (auto status = _uploadStatus.load();;)
        {
            if ( AnyEqual( status, EUploadStatus::Complete, EUploadStatus::Canceled ))
                break;

            if ( _uploadStatus.CAS( INOUT status, EUploadStatus::Canceled ))
                break;
        }
    }

/*
=================================================
    _SetUploadStatus
=================================================
*/
    inline void  IResource::_SetUploadStatus (EUploadStatus newStatus)
    {
        for (auto status = _uploadStatus.load();;)
        {
            if ( AnyEqual( status, EUploadStatus::Complete, EUploadStatus::Canceled, newStatus ))
                break;

            if ( _uploadStatus.CAS( INOUT status, newStatus ))
                break;
        }
    }


} // AE::ResEditor
