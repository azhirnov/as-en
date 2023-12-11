// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    ICSMessageProducer
        thread-safe: no

    ICSMessageConsumer
        thread-safe: no

    MessageFactory
        thread-safe: ???
*/

#pragma once

#include "networking/HighLevel/DataDecoder.h"
#include "networking/HighLevel/DataEncoder.h"

namespace AE::Networking
{
    using AE::Threading::RWSpinLock;

    enum class CSMessageUID : ushort {};
    enum class CSMessageGroupID : ushort {};

    enum class EChannel : uint
    {
        Reliable    = 0,    // reliable & ordered channel
        Unreliable  = 1,    // unreliable (unordered?) channel
        _Count
    };

    enum class EClientLocalID : ushort
    {
        Unknown     = 0xFFFF
    };

    using CSMessagePtr = Ptr< struct CSMessage >;



    //
    // Client/Server Message interface
    //
    struct CSMessage
    {
        friend class MessageFactory;

    // types
    public:
        enum class EncodeError : ubyte
        {
            OK,
            NoMemory,
            Failed,
        };

        using EncodeFn_t = EncodeError (*) (const void* self, DataEncoder &)    __NE___;

    protected:
        static constexpr uint   _MaxGroupID         = (1u << 9) - 1;
        static constexpr uint   _GroupMaskOffset    = CT_SizeOfInBits<CSMessageUID> - IntLog2( _MaxGroupID+1 );
        static constexpr uint   _GroupMask          = _MaxGroupID << _GroupMaskOffset;
        static constexpr uint   _MaxMessageID       = (1u << _GroupMaskOffset) - 1;

        template <uint GroupID, uint MsgID>
        ND_ static constexpr CSMessageUID  _PackUID ()                          __NE___
        {
            StaticAssert( GroupID <= _MaxGroupID );
            StaticAssert( MsgID   <= _MaxMessageID );
            return _PackUID( GroupID, MsgID );
        }

        ND_ static constexpr CSMessageUID  _PackUID (uint group, uint id)       __NE___
        {
            ASSERT( group <= _MaxGroupID );
            ASSERT( id <= _MaxMessageID );
            return CSMessageUID((group << _GroupMaskOffset) | id);
        }

        ND_ static constexpr Tuple<uint, uint>  _UnpackUID (CSMessageUID uid)   __NE___
        {
            return Tuple{ (uint(uid) >> _GroupMaskOffset) & _MaxGroupID, uint(uid) & ~_GroupMask };
        }


    // variables
    private:
        const CSMessageUID      _uid;
        const EClientLocalID    _clientId;


    // methods
    protected:
        CSMessage (CSMessageUID uid, EClientLocalID cid)            __NE___ : _uid{uid}, _clientId{cid} {}

    public:
        ND_ EncodeError         Serialize (DataEncoder &enc)        C_NE___;

        // utils //
        ND_ static constexpr uint  GroupCount ()                    __NE___ { return _MaxGroupID+1; }
        ND_ static constexpr auto  UnpackGroupID (CSMessageUID uid) __NE___ { return CSMessageGroupID( (uint(uid) >> _GroupMaskOffset) & _MaxGroupID ); }


        template <typename T>
        ND_ T const*            As ()                               C_NE___ { ASSERT( _uid == T::UID );  return Cast<T>( this ); }

        ND_ CSMessageGroupID    GroupId ()                          C_NE___ { return UnpackGroupID( _uid ); }
    //  ND_ uint                LocalId ()                          C_NE___ { return uint(_uid) & _MaxMessageID; }
        ND_ CSMessageUID        UniqueId ()                         C_NE___ { return _uid; }

        ND_ EClientLocalID      ClientId ()                         C_NE___ { return _clientId; }
    };



    //
    // Message Constructor
    //
    template <typename T>
    struct CSMessageCtor
    {
        static bool  CreateForEncode (OUT CSMessagePtr &, IAllocator &, Bytes extraSize)                __NE___;
        static bool  CreateAndDecode (OUT CSMessagePtr &, IAllocator &, EClientLocalID, DataDecoder &)  __NE___;
    };



    //
    // Message Producer interface
    //
    class ICSMessageProducer : public EnableRC<ICSMessageProducer>
    {
    // interface
    public:
        ND_ virtual EnumBitSet<EChannel>    GetChannels ()              C_NE___ = 0;

        ND_ virtual ChunkList<CSMessagePtr>  Produce (FrameUID frameId) __NE___ = 0;
    };



    //
    // Message Consumer interface
    //
    class ICSMessageConsumer : public EnableRC<ICSMessageConsumer>
    {
    // interface
    public:
        ND_ virtual CSMessageGroupID  GetGroupID ()                     C_NE___ = 0;

            virtual void  Consume (ChunkList<const CSMessagePtr>)       __NE___ = 0;
    };



    //
    // Message Factory
    //
    class MessageFactory : public EnableRC<MessageFactory>
    {
    // types
    public:
        using MsgPtr_t              = CSMessagePtr; // allocated by '_dbAlloc' and memory will be auto-released in 'NextFrame()'
        using CreateForEncode_t     = bool (*) (OUT MsgPtr_t &, IAllocator &, Bytes);
        using CreateAndDecode_t     = bool (*) (OUT MsgPtr_t &, IAllocator &, EClientLocalID, DataDecoder &);

    private:
        using MsgTypeMap_t          = FlatHashMap< uint, Tuple< CreateForEncode_t, CreateAndDecode_t >>;
        using Allocator_t           = Threading::LfLinearAllocator< usize{4_Mb}, usize{16_b}, 16 >;

        using DoubleBufAlloc_t      = StaticArray< RC<Allocator_t>, 2 >;
        using DoubleBufFrameId_t    = StaticArray< AtomicFrameUID, 2 >;

        struct MsgGroup
        {
            RWSpinLock      guard;
            bool            immutable   = false;
            MsgTypeMap_t    map;
        };
        using MsgTypes_t    = StaticArray< MsgGroup, CSMessage::_MaxGroupID+1 >;

        using MsgAndCtor_t  = Tuple< CSMessageUID, CreateForEncode_t, CreateAndDecode_t >;


    // variables
    private:
        DoubleBufAlloc_t        _dbAlloc;

        MsgTypes_t              _msgTypes;

        DEBUG_ONLY(
            DoubleBufFrameId_t  _dbFrameId;
        )


    // methods
    public:
        MessageFactory ()                                                               __NE___;
        ~MessageFactory ()                                                              __NE___ {}

        // register messages //
        template <typename T>
            bool    Register ()                                                         __NE___;

        template <typename T0, typename ...Types>
            bool    Register (Bool lockGroup)                                           __NE___;

            bool    Register (CSMessageUID uid, CreateForEncode_t, CreateAndDecode_t)   __NE___;
            bool    Register (CSMessageGroupID groupId, ArrayView<MsgAndCtor_t>,
                              Bool lockGroup = False{})                                 __NE___;

        // message constructors //
        template <typename T>
        ND_ Ptr<T>  CreateMsg (FrameUID, Bytes extraSize = 0_b)                         __NE___;
        ND_ bool    CreateMsg (FrameUID, CSMessageUID, Bytes extraSize, OUT MsgPtr_t &) __NE___;

        ND_ bool    DeserializeMsg (FrameUID, CSMessageUID, EClientLocalID,
                                    OUT MsgPtr_t &, DataDecoder &)                      __NE___;


        // client/server api //
            void    NextFrame (FrameUID frameId)                                        __NE___;

        // utils //
        ND_ IAllocator&  GetAllocator (FrameUID frameId)                                __NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    DECL_CSMSG
----
    Helper for client/server message declaration.
=================================================
*/
    #define DECL_CSMSG( _name_, _group_, /*fields*/... )                                                                        \
        struct CSMsg_ ## _name_ final : CSMessage                                                                               \
        {                                                                                                                       \
            friend struct CSMessageCtor< CSMsg_ ## _name_ >;                                                                    \
                                                                                                                                \
            static constexpr auto   UID = _PackUID< uint(CSMessageGroup::_group_),                                              \
                                                    uint(CSMessageGroup:: _group_ ## _MsgCounter.next< CSMsg_ ## _name_ >())    \
                                                  >();                                                                          \
            __VA_ARGS__                                                                                                         \
                                                                                                                                \
        private:                                                                                                                \
            CSMsg_ ## _name_ ()                                     __NE___ : CSMessage{ UID, Default } {}                      \
            explicit CSMsg_ ## _name_ (EClientLocalID cid)          __NE___ : CSMessage{ UID, cid } {}                          \
                                                                                                                                \
            inline EncodeError  _Serialize (DataEncoder &)          C_NE___;                                                    \
            inline bool         _Deserialize (DataDecoder &)        __NE___;                                                    \
                                                                                                                                \
            static EncodeError  _EncodeFn (const void* self, DataEncoder &enc) __NE___ {                                        \
                return Cast< CSMsg_ ## _name_ >(self)->_Serialize( enc );                                                       \
            }                                                                                                                   \
        };                                                                                                                      \
        //StaticAssert( IsTriviallyDestructible< CSMsg_ ## _name_ >);

/*
=================================================
    CSMSG_ENC_DEC
----
    Helper for client/server message encoding/decoding.
=================================================
*/
    #define CSMSG_ENC_DEC( _name_, /*fields*/... )                                              \
        inline CSMessage::EncodeError  CSMsg_ ## _name_::_Serialize (DataEncoder &enc) C_NE___  \
        {                                                                                       \
            return  enc( __VA_ARGS__ ) ? EncodeError::OK : EncodeError::NoMemory;               \
        }                                                                                       \
                                                                                                \
        inline bool  CSMsg_ ## _name_::_Deserialize (DataDecoder &dec) __NE___                  \
        {                                                                                       \
            return  dec( OUT __VA_ARGS__ );                                                     \
        }

/*
=================================================
    CSMSG_ENC_DEC_INPLACEARR
----
    Helper for client/server message encoding/decoding,
    when message contains in place array.
=================================================
*/
    #define CSMSG_ENC_DEC_INPLACEARR( _name_, _inPlaceArrSize_, _inPlaceArr_, _fields_ )                            \
        inline CSMessage::EncodeError  CSMsg_ ## _name_::_Serialize (DataEncoder &enc) C_NE___                      \
        {                                                                                                           \
            StaticAssert( sizeof(_inPlaceArrSize_) <= sizeof(ushort) );                                             \
            StaticAssert( IsUnsignedInteger< decltype(_inPlaceArrSize_) > or                                        \
                          IsBytes< decltype(_inPlaceArrSize_) >);                                                   \
                                                                                                                    \
            bool    ok = enc( _fields_ );                                                                           \
                    ok = ok and enc.Write( _inPlaceArr_, _inPlaceArrSize_ );                                        \
            return  ok ? EncodeError::OK : EncodeError::NoMemory;                                                   \
        }                                                                                                           \
                                                                                                                    \
        inline bool  CSMsg_ ## _name_::_Deserialize (DataDecoder &dec) __NE___                                      \
        {                                                                                                           \
            return  dec( OUT _fields_ );                                                                            \
        }                                                                                                           \
                                                                                                                    \
        template <>                                                                                                 \
        inline bool  CSMessageCtor< CSMsg_ ## _name_ >::CreateAndDecode (OUT CSMessagePtr &msg, IAllocator &alloc,  \
                                                                         EClientLocalID, DataDecoder &dec) __NE___  \
        {                                                                                                           \
            using T = CSMsg_ ## _name_;                                                                             \
                                                                                                                    \
            constexpr Bytes  msg_size {offsetof( CSMsg_ ## _name_, _inPlaceArr_ )};                                 \
                                                                                                                    \
            T   temp;                                                                                               \
            if_likely( temp._Deserialize( dec ))                                                                    \
            {                                                                                                       \
                auto*   t_msg = Cast<T>( alloc.Allocate( SizeAndAlign{                                              \
                                            msg_size + temp._inPlaceArrSize_, AlignOf<T> }));                       \
                                                                                                                    \
                if_likely( t_msg != null )                                                                          \
                {                                                                                                   \
                    msg = t_msg;                                                                                    \
                    new(t_msg) T{ RVRef(temp) };                                                                    \
                                                                                                                    \
                    return dec.Read( t_msg->_inPlaceArr_, t_msg->_inPlaceArrSize_ );                                \
                }                                                                                                   \
            }                                                                                                       \
            return false;                                                                                           \
        }

/*
=================================================
    CSMSG_EMPTY_ENC_DEC
----
    Helper for client/server message encoding/decoding for empty message.
=================================================
*/
    #define CSMSG_EMPTY_ENC_DEC( _name_ )                                                       \
        inline CSMessage::EncodeError  CSMsg_ ## _name_::_Serialize (DataEncoder &) C_NE___     \
        {                                                                                       \
            return  EncodeError::OK;                                                            \
        }                                                                                       \
                                                                                                \
        inline bool  CSMsg_ ## _name_::_Deserialize (DataDecoder &) __NE___                     \
        {                                                                                       \
            return  true;                                                                       \
        }
//-----------------------------------------------------------------------------



/*
=================================================
    Serialize
=================================================
*/
    inline CSMessage::EncodeError  CSMessage::Serialize (DataEncoder &enc) C_NE___
    {
        struct MsgAndFn {
            DEBUG_ONLY( usize   magic;)
            EncodeFn_t          fn;
            CSMessage           msg;
        };
        auto*   msg_fn = BitCast<MsgAndFn *>( usize(this) - offsetof( MsgAndFn, msg ));
        ASSERT( msg_fn->magic == usize(this) );
        ASSERT( &msg_fn->msg == this );
        ASSERT( msg_fn->fn != null );
        return msg_fn->fn( this, enc );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    CreateForEncode
=================================================
*/
    template <typename T>
    bool  CSMessageCtor<T>::CreateForEncode (OUT CSMessagePtr &msg, IAllocator &alloc, Bytes extraSize) __NE___
    {
        struct MsgAndFn {
            DEBUG_ONLY( usize       magic;)
            typename T::EncodeFn_t  fn;
            T                       msg;
        };

        auto*   msg_fn = Cast<MsgAndFn>( alloc.Allocate( SizeAndAlign{ SizeOf<MsgAndFn> + extraSize, AlignOf<MsgAndFn> }));
        if_likely( msg_fn != null )
        {
            DEBUG_ONLY( msg_fn->magic = usize(&msg_fn->msg) );

            msg_fn->fn = &T::_EncodeFn;
            new(&msg_fn->msg) T{};

            msg = &msg_fn->msg;
            return true;
        }
        return false;
    }

/*
=================================================
    CreateAndDecode
=================================================
*/
    template <typename T>
    bool  CSMessageCtor<T>::CreateAndDecode (OUT CSMessagePtr &msg, IAllocator &alloc, EClientLocalID clientId, DataDecoder &dec) __NE___
    {
        auto*   t_msg = alloc.Allocate<T>();
        ASSERT( t_msg != null );

        if_likely( t_msg != null )
        {
            msg = t_msg;
            new(t_msg) T{ clientId };
            return t_msg->_Deserialize( dec );
        }
        return false;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Register
=================================================
*/
    template <typename T>
    bool  MessageFactory::Register () __NE___
    {
        StaticAssert( IsTriviallyDestructible<T> );

        return Register( T::UID, &CSMessageCtor<T>::CreateForEncode, &CSMessageCtor<T>::CreateAndDecode );
    }

    template <typename T0, typename ...Types>
    bool  MessageFactory::Register (Bool lockGroup) __NE___
    {
        // Destructor may not be used because message allocated in linear allocator and memory will be discarded.
        StaticAssert( IsTriviallyDestructible<T0> );
        StaticAssert( TypeList<Types...>::template ForEach_And< TTriviallyDestructible >() );

        return Register( CSMessageGroupID(CSMessage::_UnpackUID( T0::UID ).template Get<0>()),
                         ArrayView< MsgAndCtor_t >{
                            MsgAndCtor_t{ T0::UID,      &CSMessageCtor<T0>::CreateForEncode,    &CSMessageCtor<T0>::CreateAndDecode },
                            MsgAndCtor_t{ Types::UID,   &CSMessageCtor<Types>::CreateForEncode, &CSMessageCtor<Types>::CreateAndDecode }... },
                         lockGroup );
    }

/*
=================================================
    CreateMsg
=================================================
*/
    template <typename T>
    Ptr<T>  MessageFactory::CreateMsg (FrameUID frameId, Bytes extraSize) __NE___
    {
        MsgPtr_t    tmp;
        if_likely( CreateMsg( frameId, T::UID, extraSize, OUT tmp ))
            return Cast<T>( tmp );
        return Default;
    }

/*
=================================================
    GetAllocator
=================================================
*/
    inline IAllocator&  MessageFactory::GetAllocator (const FrameUID frameId) __NE___
    {
        const uint  id = frameId.Remap2();
        return *_dbAlloc[id];
    }


} // AE::Networking
