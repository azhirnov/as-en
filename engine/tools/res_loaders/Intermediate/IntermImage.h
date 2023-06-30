// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/RefCounter.h"
#include "base/Memory/SharedMem.h"

//#include "threading/Memory/TsSharedMem.h" // TODO

#include "graphics/Public/ImageMemView.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/MipmapLevel.h"
#include "graphics/Public/ImageLayer.h"
#include "graphics/Public/ImageDesc.h"

#include "res_loaders/Common.h"

namespace AE::ResLoader
{

    //
    // Intermediate Image
    //

    class IntermImage final : public EnableRC<IntermImage>
    {
    // types
    public:
        struct Level
        {
        public:
            uint3               dimension;
            EPixelFormat        format      = Default;
            ImageLayer          layer       = 0_layer;
            MipmapLevel         mipmap      = 0_mipmap;
            Bytes               rowPitch;
            Bytes               slicePitch;
        private:
            void*               _pixels     = null;
            RC<SharedMem>       _storage;   // can be null

        public:
            ND_ bool    operator == (const Level &)                                     C_NE___;

            ND_ bool    SetPixelData (RC<SharedMem> storage)                            __NE___;
            ND_ bool    SetPixelData (RC<SharedMem> storage, void* pixels)              __NE___;
            ND_ bool    SetPixelData (RC<SharedMem> storage, void* pixels, Bytes size)  __NE___;

            ND_ bool    SetPixelDataRef (void* pixels)                                  __NE___;
            ND_ bool    SetPixelDataRef (void* pixels, Bytes size)                      __NE___;

            ND_ void*   PixelData ()                                                    C_NE___ { return _pixels; }
            ND_ Bytes   DataSize ()                                                     C_NE___ { return slicePitch * dimension.z; }
            ND_ bool    Empty ()                                                        C_NE___ { return _pixels == null; }
            ND_ bool    IsPixelDataRef ()                                               C_NE___ { return _pixels != null and _storage; }

            ND_ ArrayView<ubyte>  Pixels ()                                             C_NE___;
        };

        using ArrayLayers_t     = Array< Level >;           // size == 1 for non-array images
        using Mipmaps_t         = Array< ArrayLayers_t >;


    // variables
    private:
        Path            _srcPath;

        Mipmaps_t       _data;                      // mipmaps[] { layers[] { level } }
        EImage          _imageType      = Default;

        bool            _immutable      = false;


    // methods
    public:
        IntermImage ()                                                                                              __NE___ {}
        IntermImage (IntermImage &&)                                                                                __NE___;
        explicit IntermImage (Path path)                                                                            : _srcPath{RVRef(path)} {}
        IntermImage (Mipmaps_t data, EImage type, StringView path = Default)                                        : _srcPath{path}, _data{RVRef(data)}, _imageType{type} {}

        IntermImage&  operator = (IntermImage &&)                                                                   __NE___;

            void  MakeImmutable ()                                                                                  __NE___ { _immutable = true; }
            void  ReleaseData ()                                                                                    __NE___ { Reconstruct( INOUT _data ); }

        ND_ bool  SetData (Mipmaps_t data, EImage type)                                                             __NE___;
        ND_ bool  SetData (const ImageMemView &memView, RC<SharedMem> storage)                                      __NE___;

        ND_ bool  Copy (const ImageMemView &memView, SharedMem::Allocator_t allocator)                              __NE___;
        ND_ bool  Copy (const ImageMemView &memView)                                                                __NE___;

        ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers,
                            MipmapLevel mipmaps, SharedMem::Allocator_t allocator)                                  __NE___;
        ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, SharedMem::Allocator_t allocator)      __NE___;

        // with default allocator
        ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps)__NE___;
        ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim)                                        __NE___;

        ND_ bool  Reserve (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps) __NE___;

        ND_ ImageMemView        ToView (MipmapLevel mipmap, ImageLayer layer)                                       __NE___;
        ND_ ImageMemView        ToView (MipmapLevel mipmap)                                                         __NE___ { return ToView( mipmap, Default ); }
        ND_ ImageMemView        ToView (ImageLayer layer)                                                           __NE___ { return ToView( Default, layer ); }
        ND_ ImageMemView        ToView ()                                                                           __NE___ { return ToView( Default, Default ); }

        ND_ bool                IsValid ()                                                                          C_NE___;
        ND_ bool                IsImmutable ()                                                                      C_NE___ { return _immutable; }
        ND_ bool                IsMutable ()                                                                        C_NE___ { return not _immutable; }

        ND_ Path const&         GetPath ()                                                                          C_NE___ { return _srcPath; }
        ND_ Mipmaps_t const&    GetData ()                                                                          C_NE___ { return _data; }
        ND_ Mipmaps_t *         GetMutableData ()                                                                   __NE___ { return IsMutable() ? &_data : null; }
        ND_ EImage              GetType ()                                                                          C_NE___ { return _imageType; }
        ND_ EImageDim           GetImageDim ()                                                                      C_NE___;

        ND_ EPixelFormat        PixelFormat ()                                                                      C_NE___ { return _data.size() and _data[0].size() ? _data[0][0].format : Default; }
        ND_ uint                MipLevels ()                                                                        C_NE___ { return uint(_data.size()); }
        ND_ uint                ArrayLayers ()                                                                      C_NE___ { return _data.size() ? uint(_data[0].size()) : 0u; }
        ND_ uint3               Dimension ()                                                                        C_NE___ { return _data.size() and _data[0].size() ? _data[0][0].dimension : Default; }
        ND_ Bytes               RowPitch ()                                                                         C_NE___ { return _data.size() and _data[0].size() ? _data[0][0].rowPitch : Default; }
    };


} // AE::ResLoader
