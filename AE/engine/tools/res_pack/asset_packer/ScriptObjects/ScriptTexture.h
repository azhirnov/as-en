// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"
#include "graphics/Public/ResourceEnums.h"
#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::AssetPacker
{
    using AE::Graphics::EPixelFormat;
    using AE::Graphics::ImageLayer;
    using AE::Graphics::MipmapLevel;


    //
    // Script Texture
    //

    class ScriptTexture final : public EnableScriptRC
    {
    // variables
    private:
        Unique<ResLoader::IntermImage>      _imgData;

        EPixelFormat            _dstFormat      = EPixelFormat::RGBA8_UNorm;
        EPixelFormat            _intermFormat   = EPixelFormat::RGBA8_UNorm;


    // methods
    public:
        ScriptTexture ();
        ~ScriptTexture ();

        void  Alloc1 (const packed_uint2 &dim, EPixelFormat fmt)                                                        __Th___;
        void  Alloc2 (const packed_uint2 &dim, EPixelFormat fmt, const ImageLayer &layers)                              __Th___;
        void  Alloc3 (const packed_uint2 &dim, EPixelFormat fmt, const MipmapLevel &mipmaps)                            __Th___;
        void  Alloc4 (const packed_uint2 &dim, EPixelFormat fmt, const ImageLayer &layers, const MipmapLevel &mipmaps)  __Th___;
        void  Alloc5 (const packed_uint3 &dim, EPixelFormat fmt)                                                        __Th___;
        void  Alloc6 (const packed_uint3 &dim, EPixelFormat fmt, const MipmapLevel &mipmaps)                            __Th___;
        void  AllocCube1 (const packed_uint2 &dim, EPixelFormat fmt)                                                    __Th___;
        void  AllocCube2 (const packed_uint2 &dim, EPixelFormat fmt, const MipmapLevel &mipmaps)                        __Th___;

        void  Load1 (const String &imageFile)                                                                           __Th___;
        void  Load2 (const String &imageFile, bool flipY)                                                               __Th___;
        void  Load3 (const String &imageFile, const RectU &region)                                                      __Th___;

        void  LoadChannel1 (const String &imageFile, const String &srcSwizzle, const String &dstSwizzle)                __Th___;

        // for array, cubemap, cubemap array
        void  AddLayer1 (const String &imageFile, uint layer)                                                           __Th___;
        void  AddLayer2 (const String &imageFile, uint layer, bool flipY)                                               __Th___;
        void  AddLayer3 (const String &imageFile, const RectU &region, uint layer)                                      __Th___;

        void  Store (const String &nameInArchive)                                                                       __Th___;
        void  SetFormat (EPixelFormat fmt)                                                                              __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                                   __Th___;

    private:
        ND_ bool  _Pack (const String &nameInArchive, RC<WStream> stream);

            void  _AddLayer (ResLoader::IntermImage &img, uint layer)                                                   __Th___;

        ND_ static Unique<ResLoader::IntermImage>  _Load (const String &imageFile, bool flipY)                          __Th___;
        ND_ static Unique<ResLoader::IntermImage>  _Load (const String &imageFile, const RectU &region)                 __Th___;
    };

    using ScriptTexturePtr = ScriptRC< ScriptTexture >;


} // AE::AssetPacker
