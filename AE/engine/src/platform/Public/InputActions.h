// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/AppEnums.h"

// helper for:
//      ActionQueueReader::Header   hdr;
//      for (; reader.ReadHeader( OUT hdr );)
//      {
//          switch_IA( hdr.name )
//          {
//              case ...
//          }
//          switch_end
//      }
#define switch_IA2( _ia_, _name_ )      switch_enum( RemoveReference<decltype(_ia_)>::Bindings(uint{_name_}) )
#define switch_IA( _name_ )             switch_IA2( IA, (_name_) )

namespace AE::Base
{
    class MemRefRStream;
}
namespace AE::App
{

    //
    // Input Actions interface
    //

    class IInputActions : public Serializing::ISerializable
    {
    // types
    public:
        using Duration_t = std::chrono::duration< int, std::milli >;    // +-596 h

        struct GNSData
        {
            double      latitude;           // deg
            double      longitude;          // deg
            double      altitude;           // in meters above the WGS84 reference ellipsoid
            ulong       time;               // accuracy <30ns + 3ns (= 1m/light_speed) * accuracy_in_meters
            float       bearing;            // deg [0, 360), <0 if not presented
            float       speed;              // m/s, <0 if not presented

            // accuracy
            float       horizontalAccuracy; // m
            float       verticalAccuracy;   // m
            float       bearingAccuracy;    // deg
            float       speedAccuracy;      // m/s
        };
        StaticAssert( sizeof(GNSData) == 56 );


        struct Chars
        {
            uint        length;
            CharUtf32   chars [1];  // including position in target text field and backspace/delete keys
        };
        static constexpr auto   Chars_Move  = CharUtf32(0xFFFF'FFFF);   // 2 symbols: move, offset


        enum class EValueType : ubyte
        {
            Float,      // float,   unorm,      snorm
            Float2,     // float2,  unorm2,     snorm2
            Float3,     // float3,  unorm3,     snorm3
            Float4,     // float4
            Quat,
            Float4x4,
            Chars,      // 'Chars' type
            GNS,        // 'GNSData' type

            _Count,
            Unknown = 0xFF,
        };


        struct KeyReflection
        {
            //ImageInAtlasName  imageName;  // for UI
            //FixedString<32>   name;       // TODO: hash to get string from localization
        };

        struct Reflection
        {
            EValueType      valueType   = Default;
            EGestureType    gesture     = Default;

            FixedArray< KeyReflection, 8 >  keys;
        };


        //
        // Thread-safe Action Queue
        //
        struct TsActionQueue
        {
        // types
        public:
            struct Header
            {
                InputActionName name;
                Bytes32u        offset;         // TODO: use ushort ?
                ControllerID    controllerId;
                EGestureState   state;
            };
            static constexpr Bytes  _DataAlign {4};


        // variables
        private:
            Atomic<uint>    _writePos   {0};
            Atomic<uint>    _readPos    {0};

            Header *        _headers    = null;
            void *          _data       = null;

            Bytes32u        _dataPos;
            const uint      _maxHeaders = 0;
            const Bytes32u  _dataSize;


        // methods
        public:
            TsActionQueue ()                                                                        __NE___ {}
            TsActionQueue (TsActionQueue &&)                                                        __NE___;
            TsActionQueue (void* ptr, Bytes headerSize, Bytes dataSize)                             __NE___;

            bool  Insert (InputActionName::Ref, ControllerID, EGestureState, const void*, Bytes)    __NE___;
            bool  Insert (InputActionName::Ref, ControllerID, EGestureState)                        __NE___;

            void  Reset ()                                                                          __NE___;

            ND_ uint            ReadPos ()                                                          C_NE___ { return _readPos.load(); }
            ND_ Header const&   GetHeader (uint idx)                                                C_NE___ { return _headers[idx]; }
            ND_ void const*     GetData ()                                                          C_NE___ { return _data; }
        };


        //
        // Action Queue Reader
        //
        struct ActionQueueReader
        {
        // types
        public:
            using Header = TsActionQueue::Header;

        // variables
        private:
            uint                    _pos    = 0;
            uint                    _count  = 0;
            TsActionQueue const*    _queue  = null;

        // methods
        public:
            ActionQueueReader ()                                __NE___ {}
            ActionQueueReader (const ActionQueueReader &)       __NE___ = default;
            ActionQueueReader (ActionQueueReader &&)            __NE___ = default;

            explicit ActionQueueReader (const TsActionQueue &)  __NE___;

            ND_ bool  ReadHeader (OUT Header &)                 __NE___;

            template <typename T>
            ND_ T const&  Data (Bytes offset)                   C_NE___;

            template <typename T>
            ND_ T  DataCopy (Bytes offset)                      C_NE___;

                void  Restart ()                                __NE___ { _pos = 0; }
        };


    // interface
    public:

        // TODO
        //   Thread safe: yes
        //
        ND_ virtual ActionQueueReader  ReadInput (FrameUID frameId)                                                 C_NE___ = 0;

        // TODO
        //   Thread safe: yes
        //
            virtual void  NextFrame (FrameUID frameId)                                                              __NE___ = 0;

        // TODO
        //   Thread safe: no
        //
        ND_ virtual bool  SetMode (InputModeName::Ref value)                                                        __NE___ = 0;

        // TODO
        //   Thread safe: no
        //
        ND_ virtual InputModeName  GetMode ()                                                                       C_NE___ = 0;

        // TODO
        //   Thread safe: no
        //
        ND_ virtual bool  LoadSerialized (MemRefRStream &stream)                                                    __NE___ = 0;

        // TODO: get reflection for UI


        // Serialize key/gesture to action.
        //   Thread safe: no
        //
        //ND_ virtual bool  Serialize (struct Serializer &) const                                                   __NE___ = 0;
        //ND_ virtual bool  Deserialize (struct Deserializer &)                                                     __NE___ = 0;

        // TODO
        //ND_ virtual bool  SetScale (const float4 &scale)                                                          __NE___ = 0;

        // TODO
        //   Thread safe: no
        //
        ND_ virtual bool  GetReflection (InputModeName::Ref mode, InputActionName::Ref action, OUT Reflection &)    C_NE___ = 0;


    // bind action to input key/gesture //

        // Begin user input recording.
        //   Thread safe: no
        //
            virtual bool  BeginBindAction (InputModeName::Ref mode, InputActionName::Ref action,
                                           EValueType type, EGestureType gesture)                                   __NE___ = 0;

        // Stop recording and create key/gesture to action mapping.
        //   Thread safe: no
        //
            virtual bool  EndBindAction ()                                                                          __NE___ = 0;

        // Returns 'true' if used between 'BeginBindAction()' and 'EndBindAction()',
        // but can returns 'false' when first key/gesture are recorded.
        //   Thread safe: no
        //
        ND_ virtual bool  IsBindActionActive ()                                                                     C_NE___ = 0;
    };



    //
    // Thread-safe Input Actions
    //
    struct TsInputActions
    {
    public:
        using ActionQueueReader = IInputActions::ActionQueueReader;

    private:
        Ptr<IInputActions>  _ia;

    public:
        TsInputActions ()                                       __NE___ {}
        TsInputActions (Ptr<IInputActions> ia)                  __NE___ : _ia{ia} {}
        TsInputActions (const TsInputActions &other)            __NE___ : _ia{other._ia} {}

        ND_ ActionQueueReader   ReadInput (FrameUID frameId)    C_NE___ { return _ia->ReadInput( frameId ); }
            void                NextFrame (FrameUID frameId)    __NE___ { return _ia->NextFrame( frameId ); }

        ND_ explicit operator bool ()                           C_NE___ { return bool{_ia}; }

        ND_ Ptr<IInputActions>  Unsafe ()                       C_NE___ { return _ia; }
    };
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    inline IInputActions::ActionQueueReader::ActionQueueReader (const TsActionQueue &q) __NE___ :
        _count{ q.ReadPos() },
        _queue{ &q }
    {
        // invalidate cache for 'q._headers' and 'q._data' for '_count' elements
        Threading::MemoryBarrier( EMemoryOrder::Acquire );
    }

/*
=================================================
    ReadHeader
=================================================
*/
    inline bool  IInputActions::ActionQueueReader::ReadHeader (OUT Header &header) __NE___
    {
        if_likely( _pos < _count )
        {
            header = _queue->GetHeader( _pos++ );
            return true;
        }

        // try to get new actions
        _count = _queue->ReadPos();

        if ( _pos < _count )
        {
            // invalidate cache for '_queue->GetHeader()' and '_queue->GetData()' for new '_count' elements
            Threading::MemoryBarrier( EMemoryOrder::Acquire );

            header = _queue->GetHeader( _pos++ );
            return true;
        }

        return false;
    }

/*
=================================================
    Data
=================================================
*/
    template <typename T>
    T const&  IInputActions::ActionQueueReader::Data (Bytes offset) C_NE___
    {
        StaticAssert( AlignOf<T> <= IInputActions::TsActionQueue::_DataAlign );
        return *Cast<T>( _queue->GetData() + offset );
    }

/*
=================================================
    DataCopy
=================================================
*/
    template <typename T>
    T  IInputActions::ActionQueueReader::DataCopy (Bytes offset) C_NE___
    {
        if constexpr( IsSimdVec<T> )
        {
            using V = PackedVec< typename T::value_type, VecSize<T> >;
            return T{Data<V>( offset )};
        }
        else
        if constexpr( IsSimdQuat<T> )
        {
            using V = TQuat< typename T::Value_t, GLMPackedQualifier >;
            return T{Data<V>( offset )};
        }
        else
            return Data<T>( offset );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    inline IInputActions::TsActionQueue::TsActionQueue (void* ptr, Bytes headerSize, Bytes dataSize) __NE___ :
        _headers{ Cast<Header>( ptr )},
        _data{ ptr + headerSize },
        _maxHeaders{ headerSize / SizeOf<Header> },
        _dataSize{ dataSize }
    {}

    inline IInputActions::TsActionQueue::TsActionQueue (TsActionQueue &&other) __NE___ :
        _headers{ other._headers },
        _data{ other._data },
        _maxHeaders{ other._maxHeaders },
        _dataSize{ other._dataSize }
    {}

/*
=================================================
    Insert
=================================================
*/
    inline bool  IInputActions::TsActionQueue::Insert (InputActionName::Ref name, ControllerID id, EGestureState state, const void* data, Bytes dataSize) __NE___
    {
        const uint      hdr_idx     = _writePos.fetch_add( 1 );
        const Bytes     data_pos    = AlignUp( _dataPos, _DataAlign );

        if_unlikely( hdr_idx >= _maxHeaders )
        {
            //AE_LOG_DBG( "input actions queue overflow" );
            return false;
        }
        if_unlikely( data_pos + dataSize > _dataSize )
        {
            //AE_LOG_DBG( "input actions queue overflow" );
            return false;
        }

        auto&       hdr     = _headers[hdr_idx];
        hdr.name            = name;
        hdr.offset          = data_pos;
        hdr.controllerId    = id;
        hdr.state           = state;

        std::memcpy( OUT _data + data_pos, data, usize(dataSize) );

        _dataPos = data_pos + dataSize;
        _readPos.store( hdr_idx+1, EMemoryOrder::Release ); // TODO: fetch_add ???
        return true;
    }

    inline bool  IInputActions::TsActionQueue::Insert (InputActionName::Ref name, ControllerID id, EGestureState state) __NE___
    {
        const uint  hdr_idx = _writePos.fetch_add( 1 );

        if_unlikely( hdr_idx >= _maxHeaders )
        {
            //AE_LOG_DBG( "input actions queue overflow" );
            return false;
        }

        auto&       hdr     = _headers[hdr_idx];
        hdr.name            = name;
        hdr.offset          = UMax;
        hdr.controllerId    = id;
        hdr.state           = state;

        _readPos.store( hdr_idx+1, EMemoryOrder::Release );
        return true;
    }

/*
=================================================
    Reset
=================================================
*/
    inline void  IInputActions::TsActionQueue::Reset () __NE___
    {
        _dataPos = 0_b;
        _writePos.store( 0 );
        _readPos.store( 0, EMemoryOrder::Release );
    }


} // AE::App


namespace AE::Base
{
    template <>
    struct TTriviallySerializable< App::IInputActions::GNSData >
    {
        static constexpr bool   value = true;
    };

} // AE::Base
