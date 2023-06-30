// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/QueryManager.h"
# include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{

    //
    // Vulkan Query Manager
    //

    class VQueryManager final : public IQueryManager
    {
    // types
    public:
        struct Query
        {
            VkQueryPool     pool        = Default;
            ushort          first       = UMax;
            ushort          count       = 0;
            ushort          numPasses   = 0;
            EQueryType      type        = Default;
            EQueueType      queue       = Default;

            ND_ explicit operator bool ()   C_NE___ { return pool != Default and count > 0; }
        };

        struct PipelineStatistic
        {
            //ulong inputAssemblyPrimitives;    // VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT
            ulong   beforeClipping;             // VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT
            ulong   afterClipping;              // VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT
            ulong   fragShaderInvocations;      // VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT
            // VK_QUERY_PIPELINE_STATISTIC_TASK_SHADER_INVOCATIONS_BIT_EXT 
            // VK_QUERY_PIPELINE_STATISTIC_MESH_SHADER_INVOCATIONS_BIT_EXT
            // VK_QUERY_TYPE_MESH_PRIMITIVES_GENERATED_EXT
        };

    private:
        using PerFrameCount_t = StaticArray< uint, GraphicsConfig::MaxFrames+1 >;

        struct QueryPool
        {
            mutable Atomic<uint>    count       {0};
            VkQueryPool             handle      = Default;
            uint                    maxCount    = 0;        // per frame
            uint                    numPasses   = 1;
            PerFrameCount_t         countArr    {};

            ND_ explicit operator bool ()   C_NE___ { return handle != Default; }
        };

        using PoolArr_t = StaticArray< QueryPool, uint(EQueryType::_Count) >;

        struct Result64
        {
            ulong   result;
            ulong   available;

            ND_ bool  IsAvailable ()    C_NE___ { return available != 0; }
        };

        struct PipelineStatisticResult : PipelineStatistic
        {
            ulong   available;

            ND_ bool  IsAvailable ()    C_NE___ { return available != 0; }
        };

        using TimestampBitsPerQueue_t = StaticArray< uint, uint(EQueueType::_Count) >;

        struct PackedIdx
        {
            uint    writeIndex  : 8;
            uint    readIndex   : 8;

            PackedIdx ()                __NE___ : writeIndex{0}, readIndex{0} {}
            PackedIdx (uint w, uint r)  __NE___ : writeIndex{w}, readIndex{r} {}
        };


    // variables
    private:
        VDevice const&          _device;

        PoolArr_t               _poolArr            {};
        EQueueMask              _timestampAllowed   = Default;
        float                   _timestampPeriod    = 1.f;  // nanoseconds

        TimestampBitsPerQueue_t _tsBits;

        BitAtomic<PackedIdx>    _packedIdx;
        uint                    _maxFrames      : 8;

        uint                    _hostReset      : 1;
        uint                    _perfQuery      : 1;
        uint                    _calibratedTs   : 1;

        Atomic<bool>            _perfLockAcquired   {false};

        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:
        VQueryManager ()                                                                                            __NE___;
        ~VQueryManager ()                                                                                           __NE___;

        ND_ bool  Initialize (uint maxFrames)                                                                       __NE___;
            void  Deinitialize ()                                                                                   __NE___;

            void  NextFrame (FrameUID frameId)                                                                      __NE___;
            void  ResetQueries (VkCommandBuffer cmdbuf)                                                             __NE___;

        ND_ uint    WriteIndex ()                                                                                   C_NE___ { return _packedIdx.load().writeIndex; }
        ND_ uint    ReadIndex ()                                                                                    C_NE___ { return _packedIdx.load().readIndex; }
        ND_ uint2   ReadAndWriteIndices ()                                                                          C_NE___ { auto tmp = _packedIdx.load();  return uint2{tmp.readIndex, tmp.writeIndex}; }

        ND_ bool  AcquireProfilingLock ()                                                                           __NE___;
            bool  ReleaseProfilingLock ()                                                                           __NE___;

        ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1)                                   C_NE___;

        ND_ bool  SupportsCalibratedTimestamps ()                                                                   C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _calibratedTs; }

            bool  GetTimestamp (const Query &q, OUT ulong* result, Bytes size)                                      C_NE___;    // raw
            bool  GetTimestamp (const Query &q, OUT double* result, Bytes size)                                     C_NE___;    // nanoseconds in GPU-space
            bool  GetTimestamp (const Query &q, OUT nanosecondsd* result, Bytes size)                               C_NE___;    // nanoseconds in GPU-space

            bool  GetTimestampCalibrated (const Query &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes size)               C_NE___;    // nanoseconds in CPU-space
            bool  GetTimestampCalibrated (const Query &q, OUT double* result, OUT double* maxDeviation, Bytes size)             C_NE___;    // nanoseconds in CPU-space
            bool  GetTimestampCalibrated (const Query &q, OUT nanosecondsd* result, OUT nanosecondsd* maxDeviation, Bytes size) C_NE___;    // nanoseconds in CPU-space

        //  bool  GetPerformanceCounter (const Query &q, OUT VkPerformanceCounterResultKHR* result, Bytes size)     C_NE___;
            bool  GetPipelineStatistic (const Query &q, OUT PipelineStatistic* result, Bytes size)                  C_NE___;
            bool  GetRTASProperty (const Query &q, OUT Bytes64u* result, Bytes size)                                C_NE___;

    private:
        static void  _ResetPoolOnHost (const VDevice &, uint idx, QueryPool &pool, uint count)                      __NE___;
        static void  _ResetPool (const VDevice &, VkCommandBuffer cmdbuf, uint idx, QueryPool &pool, uint count)    __NE___;

        template <typename T>
        ND_ bool  _GetTimestamp (const Query &q, OUT T* result, Bytes size)                                         C_NE___;

        template <typename T>
        ND_ bool  _GetTimestampCalibrated (const Query &q, OUT T* result, OUT T* maxDeviation, Bytes size)          C_NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
