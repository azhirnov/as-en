// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"

namespace AE::Graphics::_hidden_
{

    //
    // Vulkan Draw Command Context Barrier Manager
    //

    class VDrawBarrierManager
    {
    // types
    private:
        using ImageMemoryBarriers_t = FixedArray< VkImageMemoryBarrier2, GraphicsConfig::MaxAttachments/2 >;


    // variables
    private:
        const VPrimaryCmdBufState   _primaryState;
        Ptr<VDrawCommandBatch>      _batch;         // can be null

        VResourceManager &          _resMngr;

        VkDependencyInfo            _barrier        = {};
        ImageMemoryBarriers_t       _imageBarriers;


    // methods
    public:
        explicit VDrawBarrierManager (Ptr<VDrawCommandBatch> batch)             __NE___;
        explicit VDrawBarrierManager (const VPrimaryCmdBufState &primaryState)  __NE___;

        template <typename ...IDs>
        ND_ decltype(auto)          Get (IDs ...ids)                            __Th___ { return _resMngr.GetResourcesOrThrow( ids... ); }

        ND_ VDevice const&          GetDevice ()                                C_NE___ { return _resMngr.GetDevice(); }
        ND_ VResourceManager&       GetResourceManager ()                       C_NE___ { return _resMngr; }
        ND_ FrameUID                GetFrameId ()                               C_NE___ { return _primaryState.frameId; }
        ND_ EQueueType              GetQueueType ()                             C_NE___ { return EQueueType::Graphics; }
        ND_ auto const&             GetPrimaryCtxState ()                       C_NE___ { return _primaryState; }

        ND_ bool                    IsSecondary ()                              C_NE___ { return _batch != null; }
        ND_ Ptr<VDrawCommandBatch>  GetBatchPtr ()                              C_NE___ { return _batch.get(); }

        ND_ const VkDependencyInfo* GetBarriers ()                              __NE___;
        ND_ bool                    NoPendingBarriers ()                        C_NE___ { return _imageBarriers.empty(); }
        ND_ bool                    HasPendingBarriers ()                       C_NE___ { return not NoPendingBarriers(); }

        ND_ uint                    GetAttachmentIndex (AttachmentName name)    C_NE___;

        void  ClearBarriers ()                                                  __NE___;
        void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState) __NE___;

    private:
        void  _Init ()                                                          __NE___;
    };



/*
=================================================
    GetBarriers
=================================================
*/
    forceinline const VkDependencyInfo*  VDrawBarrierManager::GetBarriers () __NE___
    {
        if_unlikely( HasPendingBarriers() )
        {
            _barrier.imageMemoryBarrierCount    = uint(_imageBarriers.size());
            _barrier.pImageMemoryBarriers       = _imageBarriers.data();

            return &_barrier;
        }
        else
            return null;
    }

/*
=================================================
    ClearBarriers
=================================================
*/
    forceinline void  VDrawBarrierManager::ClearBarriers () __NE___
    {
        _imageBarriers.clear();
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
