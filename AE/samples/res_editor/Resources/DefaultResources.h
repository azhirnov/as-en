// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/DataTransferQueue.h"

namespace AE::ResEditor
{

    //
    // Default Resources
    //

    class DefaultResources : public EnableRC< DefaultResources >
    {
    // variables
    private:
        RC<DataTransferQueue>   _dtQueue;

        GfxMemAllocatorPtr      _gfxLinearAlloc;
        GfxMemAllocatorPtr      _gfxDynamicAlloc;

        struct {
            StrongImageAndViewID    image2D;
            StrongImageAndViewID    imageCube;
            StrongImageAndViewID    image3D;

            Strong<RTGeometryID>    rtGeometry;
            Strong<RTSceneID>       rtScene;
        }                       _dummyRes;


    // methods
    public:
        ~DefaultResources ();

        ND_ DataTransferQueue&      GetDataTransferQueue ()             C_NE___ { return *_dtQueue; }
        ND_ GfxMemAllocatorPtr      GetAllocator ()                     C_NE___ { return _gfxLinearAlloc; }
        ND_ GfxMemAllocatorPtr      GetDynamicAllocator ()              C_NE___ { return _gfxDynamicAlloc; }

        ND_ StrongImageAndViewID    GetDummyImage (const ImageDesc &)   C_NE___;
        ND_ Strong<RTGeometryID>    GetDummyRTGeometry ()               C_NE___;
        ND_ Strong<RTSceneID>       GetDummyRTScene ()                  C_NE___;

    protected:
        DefaultResources ();

    private:
        void  _CreateDummyImage2D (OUT StrongImageAndViewID &, GfxMemAllocatorPtr)      const;
        void  _CreateDummyImage3D (OUT StrongImageAndViewID &, GfxMemAllocatorPtr)      const;
        void  _CreateDummyImageCube (OUT StrongImageAndViewID &, GfxMemAllocatorPtr)    const;
        void  _CreateDummyRTGeometry (OUT Strong<RTGeometryID> &, GfxMemAllocatorPtr)   const;
        void  _CreateDummyRTScene (OUT Strong<RTSceneID> &, GfxMemAllocatorPtr)         const;
    };


} // AE::ResEditor
