// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BASS
# include "audio/Public/IAudioData.h"

namespace AE::Audio
{

    //
    // BASS Audio Data
    //

    class AudioDataBASS final : public IAudioData
    {
    // variables
    private:
        uint            _streamId;              // DWORD or HSAMPLE
        RC<RStream>     _dataStream;            // if loaded with 'Async' flag
        AudioDataDesc   _desc;


    // methods
    public:
        AudioDataBASS (uint id, RC<RStream> stream, ESoundFlags flags) __NE___;
        ~AudioDataBASS ()                       __NE_OV;

        ND_ uint                Handle ()       C_NE___ { return _streamId; }


        // IAudioData //
        bool                    IsStream ()     __NE_OV { return _dataStream != null; }
        RC<RStream>             Stream ()       __NE_OV { return _dataStream; }

        AudioDataDesc const&    Description ()  C_NE_OV { return _desc; }
    };


} // AE::Audio

#endif // AE_ENABLE_BASS
