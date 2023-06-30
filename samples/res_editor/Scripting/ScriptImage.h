// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

    //
    // Image
    //

    class ScriptImage final : public EnableScriptRC
    {
    // types
    public:
        enum class EImageType : uint
        {
            Unknown,
            ConstDataFromFile,
            Storage,
            RenderTarget,
        };

        using ELoadOpFlags = Image::ELoadOpFlags;

    private:
        using LoadOps_t = Array< Image::LoadOp >;


    // variables
    public:
        ImageDesc               _desc;
        ImageViewDesc           _viewDesc;
        const bool              _descDefined    = false;

        EResourceUsage          _resUsage       = Default;
        uint                    _imageType      = 0;        // PipelineCompiler::EImageType
        LoadOps_t               _loadOps;
        String                  _dbgName;

        ScriptDynamicDimPtr     _inDynSize;     // image dimension depends on it
        ScriptDynamicDimPtr     _outDynSize;    // image dimension will change this value

        RC<Image>               _resource;
        ScriptImagePtr          _base;


    // methods
    public:
        ScriptImage () {}
        ScriptImage (uint imageType, const String &filename)                        __Th___;
        ScriptImage (EPixelFormat format, const ScriptDynamicDimPtr &ds)            __Th___;
        ScriptImage (EPixelFormat format, const packed_uint3 &dim)                  __Th___;
        ScriptImage (EPixelFormat format, const packed_uint3 &dim, const ImageLayer &layers, const MipmapLevel &mipmaps)        __Th___;
        ScriptImage (EPixelFormat format, const ScriptDynamicDimPtr &ds, const ImageLayer &layers, const MipmapLevel &mipmaps)  __Th___;

        void  Name (const String &name)                                             __Th___;

        void  AddUsage (EResourceUsage usage)                                       __Th___;

        void  LoadLayer1 (const String &filename, uint layer)                       __Th___;
        void  LoadLayer2 (const String &filename, uint layer, ELoadOpFlags flags)   __Th___;
        void  LoadLayer3 (const String &filename, uint layer, uint flags)           __Th___;

        ND_ packed_uint2        Dimension2 ()                                       __Th___ { return packed_uint2(Dimension3()); }
        ND_ packed_uint3        Dimension2_Layers ()                                __Th___ { return packed_uint3{Dimension2(), ArrayLayers()}; }
        ND_ packed_uint3        Dimension3 ()                                       __Th___;

        ND_ bool                IsMutableDimension ()                               __Th___;
        ND_ ScriptDynamicDim*   Dimension ()                                        __Th___;
        ND_ RC<DynamicDim>      DynamicDimension ()                                 __Th___;

        ND_ uint                ArrayLayers ()                                      __Th___;
        ND_ uint                MipmapCount ()                                      __Th___;

        ND_ ImageDesc           Description ()                                      C_NE___;
        ND_ uint                ImageType ()                                        C_NE___ { return _imageType; }

        ND_ bool                IsColor ()                                          C_NE___;
        ND_ bool                IsDepthStencil ()                                   C_NE___;

            ScriptImage*        CreateView1 (EImage             viewType,
                                             const MipmapLevel& baseMipmap,
                                             uint               mipmapCount,
                                             const ImageLayer&  baseLayer,
                                             uint               layerCount)         __Th___;

            ScriptImage*        CreateView2 (EImage             viewType)           __Th___;

            ScriptImage*        CreateView3 (EImage             viewType,
                                             const MipmapLevel& baseMipmap,
                                             uint               mipmapCount)        __Th___;

            ScriptImage*        CreateView4 (EImage             viewType,
                                             const ImageLayer&  baseLayer,
                                             uint               layerCount)         __Th___;


        static void  Bind (const ScriptEnginePtr &se)                               __Th___;

        ND_ RC<Image>  ToResource ()                                                __Th___;

    private:
        void  _Load (const String &filename, MipmapLevel mipmap,
                     ImageLayer layer, ELoadOpFlags flags)                          __Th___;

        friend class ScriptVideoImage;
        static void  _ValidateResourceUsage (EResourceUsage usage)                  __Th___;
    };


} // AE::ResEditor
