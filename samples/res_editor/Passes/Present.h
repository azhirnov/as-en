// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

    //
    // Present pass
    //

    class Present final : public IPass
    {
    // variables
    private:
        Array<RC<Image>>            _src;
        String                      _dbgName;
        RC<DynamicDim>              _dynSize;       // mutable

        mutable RC<IVideoEncoder>   _videoEncoder;


    // methods
    public:
        explicit Present (Array<RC<Image>> src, StringView dbgName, RC<DynamicDim> dynSize) :
            _src{RVRef(src)}, _dbgName{dbgName}, _dynSize{dynSize} {}

    // IPass //
        EPassType   GetType ()                              C_NE_OV { return EPassType::Present; }
        StringView  GetName ()                              C_NE_OV { return _dbgName; }
        AsyncTask   PresentAsync (const PresentPassData &)  __NE_OV;

    private:
        ND_ static RenderTaskCoro  _Blit (RC<Present> self, IOutputSurface &);

        static void  _SaveScreenshot (const ImageMemView &, EImageFormat fmt);

        RC<IVideoEncoder>  _CreateEncoder (float bitrate, EVideoFormat, EVideoCodec, EVideoColorPreset) const;
    };



    //
    // Debug View
    //

    class DebugView final : public IPass
    {
    // types
    public:
        enum class EFlags : uint
        {
            Unknown     = 0,
            NoCopy,
            Histogram,
            _Count
        };

    private:
        class IAdditionalPass
        {
        public:
            virtual ~IAdditionalPass () {}
            virtual bool  Execute (const Image &src, const Image &copy, SyncPassData &) const = 0;
        };

        class Histogram final : public IAdditionalPass
        {
        private:
            RenderTechPipelinesPtr  _rtech;

            ComputePipelineID       _ppln1;
            ComputePipelineID       _ppln2;
            GraphicsPipelineID      _ppln3;
            uint2                   _ppln1LS;
            uint                    _ppln2LS;

            DescSetBinding          _ppln1DSIdx;
            PerFrameDescSet_t       _ppln1DS;       // 1 & 2

            DescSetBinding          _ppln3DSIdx;
            PerFrameDescSet_t       _ppln3DS;

            Strong<BufferID>        _ssb;

        public:
            Histogram (Renderer* renderer, const Image &src, const Image &copy) __Th___;
            ~Histogram ();

            bool  Execute (const Image &src, const Image &copy, SyncPassData &) C____OV;
        };


    // variables
    private:
        RC<Image>       _src;
        RC<Image>       _copy;      // make a copy if '_src' will be changed

        uint            _index      = UMax;
        EFlags          _flags      = Default;
        String          _dbgName;

        Unique<IAdditionalPass> _pass;


    // methods
    public:
        explicit DebugView (RC<Image> src, uint idx, EFlags flags,
                            ImageLayer layer, MipmapLevel mipmap,
                            Renderer* renderer, StringView dbgName) __Th___;
        ~DebugView ();

        ND_ RC<Image>   GetImage ()                     const   { return _copy; }

    // IPass //
        EPassType       GetType ()                      C_NE_OV { return EPassType::Sync; }
        StringView      GetName ()                      C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)        __NE_OV;

    private:
            void  _InitHistogram ()                     __Th___;

        ND_ bool  _CopyImage (SyncPassData &pd)         const;
        ND_ bool  _CalcHistogram (SyncPassData &pd)     const;
    };



    //
    // Generate Mipmaps pass
    //

    class GenerateMipmaps final : public IPass
    {
    // variables
    private:
        RC<Image>   _image;
        String      _dbgName;


    // methods
    public:
        explicit GenerateMipmaps (RC<Image> image, StringView dbgName) :
            _image{RVRef(image)}, _dbgName{dbgName} {}

    // IPass //
        EPassType   GetType ()                      C_NE_OV { return EPassType::Sync; }
        StringView  GetName ()                      C_NE_OV { return _dbgName; }
        bool        Execute (SyncPassData &)        __NE_OV;
    };


} // AE::ResEditor
