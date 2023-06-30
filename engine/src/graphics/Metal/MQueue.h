// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/Queue.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

    //
    // Metal Queue
    //

    struct MQueue
    {
    // types
        using Handle_t  = NS::ObjStrongPtr< NS::ObjectRef >;


    // variables
        mutable RecursiveMutex      guard;
        Handle_t                    handle;
        MetalIOQueueRC              ioHandle;
        EQueueType                  type        = Default;
        DebugName_t                 debugName;


    // methods
        MQueue ()                                           __NE___ {}

        MQueue (MQueue &&other)                             __NE___ :
            handle{ RVRef(other.handle) }, type{other.type},
            debugName{other.debugName}
        {}

        MQueue (const MQueue &other)                        __NE___ :
            handle{other.handle}, type{other.type},
            debugName{other.debugName}
        {}

        ND_ MetalQueue    Handle ()     C_NE___ { return MetalQueue{ type != EQueueType::AsyncTransfer ? handle.Ptr() : null }; }
        ND_ MetalIOQueue  IOHandle ()   C_NE___ { return MetalIOQueue{ type == EQueueType::AsyncTransfer ? handle.Ptr() : null }; }
    };

    using MQueuePtr = Ptr< const MQueue >;


} // AE::Graphics

#endif // AE_ENABLE_METAL
