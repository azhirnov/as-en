// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/ResourceLoaders.pch.h"

namespace AE::ResLoader
{
    using namespace AE::Base;


    //
    // Image Format
    //
    enum class EImageFormat : uint
    {
        Unknown         = 0,
        DDS,            // .dds
        BMP,            // .bmp
        JPG,            // .jpg .jpe .jpeg
        PCX,            // .pcx
        PNG,            // .png
        TGA,            // .tga
        TIF,            // .tif .tiff
        PSD,            // .psd
        RadianceHDR,    // .hdr
        OpenEXR,        // .exr
        KTX,            // .ktx
        AEImg,          // .aeimg
        _Count
    };

    ND_ EImageFormat    PathToImageFileFormat (const Path &path)    __NE___;
    ND_ StringView      ImageFileFormatToExt (EImageFormat)         __NE___;



    //
    // Model Format
    //
    enum class EModelFormat : uint
    {
        Unknown     = 0,
        GLTF,       // .gltf
    };

    ND_ EModelFormat    PathToModelFormat (const Path &path)        __NE___;
    ND_ StringView      ModelFormatToExt (EModelFormat)             __NE___;


    //
    // Audio Format
    //
    using AE::Audio::EAudioFormat;

    ND_ EAudioFormat    PathToSoundFileFormat (const Path &path)    __NE___;
    ND_ StringView      SoundFormatToExt (EAudioFormat)             __NE___;

    using AE::Audio::AudioDataDesc;


} // AE::ResLoader
