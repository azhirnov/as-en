// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/Messages.h"
# include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{
    using namespace AE::Threading;
    using namespace AE::Networking;

    //
    // Network Storage Base
    //
    class NetworkStorageBase
    {
    // types
    protected:
        using Index_t       = uint;

        using ReqGen_t      = NDSRequestID::Generation_t;
        using DSGen_t       = NetDataSourceID::Generation_t;
        using FileNameRef   = IVirtualFileStorage::FileNameRef;

        using _SpinLock_t   = TRWSpinLock< false, true >;

        static constexpr uint   _FileCount  = 1u << 8;
        static constexpr uint   _ReqCount   = 1u << 10;


        class MsgProducer final : public AsyncCSMessageProducer< LfLinearAllocator< usize{4_Mb}, usize{16_b}, 16 >>
        {
        public:
            EnumBitSet<EChannel>  GetChannels ()    C_NE_OV { return {EChannel::Reliable}; }
        };


        static constexpr Bytes  _partSize = AlignDown(  Min( Min( NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_VFS_ReadResult>,
                                                                  MaxValue< decltype(CSMsg_VFS_ReadResult::size) >() ),
                                                             Min( NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_VFS_WritePart>,
                                                                  MaxValue< decltype(CSMsg_VFS_WritePart::size) >() )),
                                                        64 );
        static constexpr uint   _maxParts = 32;


    // variables
    protected:
        StaticRC<MsgProducer>   _msgProducer;


    // methods
    protected:
        template <typename T>
        ND_ auto  _CreateMsg (Bytes extraSize = 0_b)    __NE___ { return _msgProducer->CreateMsg<T>( extraSize ); }

        template <typename T>
        ND_ auto  _CreateMsgOpt (Bytes extraSize = 0_b) __NE___ { return _msgProducer->CreateMsgOpt<T>( extraSize ); }

        template <typename T>
        ND_ bool  _AddMessage (T &msg)                  __NE___ { return _msgProducer->AddMessage( msg ); }
    };



} // AE::VFS

#endif // AE_ENABLE_NETWORKING
