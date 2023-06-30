// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "video/Public/VideoEncoder.h"
# include "graphics/Public/GraphicsImpl.h"

namespace AE::Video
{
    using namespace AE::Graphics;


    //
    // Graphics HW Video Encoder
    //

    class GfxVideoEncoder final : public IVideoEncoder
    {
    // variables
    private:
        Strong<VideoSessionID>      _sessionId;


    // methods
    public:
        GfxVideoEncoder ()                                                  __NE___;
        ~GfxVideoEncoder ()                                                 __NE_OV;

        bool  Begin (const Config &cfg, const Path &filename)               __NE_OV;
        bool  Begin (const Config &cfg, RC<WStream> temp, RC<WStream> dst)  __NE_OV;

        bool  AddFrame (const ImageMemView &view, Bool endOnError)          __NE_OV;
        bool  AddFrame (VideoImageID id, Bool endOnError)                   __NE_OV;

        bool  End ()                                                        __NE_OV;

        bool        IsEncoding ()                                           C_NE_OV;
        Config      GetConfig ()                                            C_NE_OV;

        // stateless
        StringView  GetFileExtension (EVideoCodec codec)                    C_NE_OV;
    };


} // AE::Video

#endif // AE_ENABLE_VULKAN
