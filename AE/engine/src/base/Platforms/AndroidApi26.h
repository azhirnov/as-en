// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <android/hardware_buffer.h>
# include <media/NdkImage.h>
# include <media/NdkImageReader.h>
# include "base/Defines/Undef.h"

# include "base/Platforms/AndroidUtils.h"

namespace AE::Base
{

    //
    // Android API 26 functions
    //

    struct AndroidApi26
    {
    // types
    private:
        using HwBuf_Allocate                    = int  (*) (const AHardwareBuffer_Desc* desc, AHardwareBuffer** outBuffer);
        using HwBuf_Acquire                     = void (*) (AHardwareBuffer* buffer);
        using HwBuf_Describe                    = void (*) (const AHardwareBuffer* buffer, AHardwareBuffer_Desc* outDesc);
        using HwBuf_Lock                        = int  (*) (AHardwareBuffer* buffer, uint64_t usage, int32_t fence, const ARect* rect, void** outVirtualAddress);
        using HwBuf_RecvHandleFromUnixSocket    = int  (*) (int socketFd, AHardwareBuffer** outBuffer);
        using HwBuf_Release                     = void (*) (AHardwareBuffer* buffer);
        using HwBuf_SendHandleToUnixSocket      = int  (*) (const AHardwareBuffer* buffer, int socketFd);
        using HwBuf_Unlock                      = int  (*) (AHardwareBuffer* buffer, int32_t* fence);

        using ImgReader_NewWithUsage            = media_status_t (*) (int32_t width, int32_t height, int32_t format, uint64_t usage, int32_t maxImages, AImageReader** reader);
        using ImgReader_AcquireNextImageAsync   = media_status_t (*) (AImageReader* reader, AImage** image, int* acquireFenceFd);
        using ImgReader_AcquireLatestImageAsync = media_status_t (*) (AImageReader* reader, AImage** image, int* acquireFenceFd);

        using Img_GetHardwareBuffer             = media_status_t (*) (const AImage* image, AHardwareBuffer** buffer);
        using Img_DeleteAsync                   = void (*) (AImage* image, int releaseFenceFd);


    // variables
    private:
        bool                            _loaded = false;
    public:
        struct {
            HwBuf_Allocate                  allocate                    = null;
            HwBuf_Acquire                   acquire                     = null;
            HwBuf_Release                   release                     = null;
            HwBuf_Describe                  describe                    = null;
        //  HwBuf_Lock                      lock                        = null;     // unused
        //  HwBuf_Unlock                    unlock                      = null;     // unused
        //  HwBuf_RecvHandleFromUnixSocket  recvHandleFromUnixSocket    = null;     // unused
        //  HwBuf_SendHandleToUnixSocket    sendHandleToUnixSocket      = null;     // unused
        }                               hwbuf;
        struct {
            ImgReader_NewWithUsage              newWithUsage                = null;
            ImgReader_AcquireNextImageAsync     acquireNextImageAsync       = null;
            ImgReader_AcquireLatestImageAsync   acquireLatestImageAsync     = null;
        }                               imgReader;
        struct {
            Img_GetHardwareBuffer           getHardwareBuffer           = null;
            Img_DeleteAsync                 deleteAsync                 = null;
        }                               img;


    // methods
    public:
        ND_ static AndroidApi26&  Instance ()   __NE___;

        ND_ bool  IsLoaded ()                   C_NE___ { return _loaded; }

    private:
        AndroidApi26 ()                         __NE___;
    };


} // AE::Base

#endif // AE_PLATFORM_ANDROID
