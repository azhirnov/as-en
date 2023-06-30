// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/QueryManager.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{

    //
    // Metal Query Manager
    //

    class MQueryManager final : public IQueryManager
    {
    // types
    public:
        struct Query
        {
            ND_ explicit operator bool ()   C_NE___;
        };

        struct PipelineStatistic
        {
        };

    private:
        struct PackedIdx
        {
            uint    writeIndex  : 8;
            uint    readIndex   : 8;

            PackedIdx ()                __NE___ : writeIndex{0}, readIndex{0} {}
            PackedIdx (uint w, uint r)  __NE___ : writeIndex{w}, readIndex{r} {}
        };


    // variables
    private:
        BitAtomic<PackedIdx>    _packedIdx;
        uint                    _maxCount       : 8;


    // methods
    public:
        MQueryManager ()                                                                                            __NE___;
        ~MQueryManager ()                                                                                           __NE___;

        ND_ bool  Initialize (uint maxFrames)                                                                       __NE___;
            void  Deinitialize ()                                                                                   __NE___;

            void  NextFrame (FrameUID frameId)                                                                      __NE___;

        ND_ uint    WriteIndex ()                                                                                   C_NE___ { return _packedIdx.load().writeIndex; }
        ND_ uint    ReadIndex ()                                                                                    C_NE___ { return _packedIdx.load().readIndex; }
        ND_ uint2   ReadAndWriteIndices ()                                                                          C_NE___ { auto tmp = _packedIdx.load();  return uint2{tmp.readIndex, tmp.writeIndex}; }

        ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1)                                   C_NE___;

            bool  GetTimestamp (const Query &q, OUT ulong* result, Bytes size)                                      C_NE___;    // raw
            bool  GetTimestamp (const Query &q, OUT double* result, Bytes size)                                     C_NE___;    // nanoseconds
            bool  GetTimestamp (const Query &q, OUT nanosecondsd* result, Bytes size)                               C_NE___;
            bool  GetTimestampCalibrated (const Query &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes size)   C_NE___;
        //  bool  GetPerformanceCounter (const Query &q, OUT VkPerformanceCounterResultKHR* result, Bytes size)     C_NE___;
            bool  GetPipelineStatistic (const Query &q, OUT PipelineStatistic* result, Bytes size)                  C_NE___;
        //  bool  GetRTASProperty (const Query &q, OUT Bytes64u* result, Bytes size)                                C_NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_METAL
