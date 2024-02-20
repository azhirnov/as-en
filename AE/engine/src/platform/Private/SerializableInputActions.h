// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Scripting.h"
#include "platform/Public/InputActions.h"

namespace AE::App
{
    static constexpr uint   InputActions_Name   = "InAct"_Hash;


    //
    // Serializable Input Actions
    //

    class SerializableInputActions : public Serializing::ISerializable
    {
        friend class InputActionsBase;

    // types
    public:
        using EValueType        = IInputActions::EValueType;
        using PackedScale_t     = half;
        using PackedScale4_t    = PackedVec< PackedScale_t, 4 >;
        using InputType_t       = ushort;

        enum class ESensorBits : uint { Unknown = 0 };


        struct ScriptActionInfo
        {
            FixedString<32>     action;     // InputActionName
            EValueType          type        = Default;
            EGestureType        gesture     = Default;
            VecSwizzle          swizzle     = VecSwizzle::VecDefault(4);
            PackedScale4_t      scale       {float4{1.0f}};

            ScriptActionInfo () __NE___ {}
            ScriptActionInfo (StringView a, EValueType t, EGestureType g, VecSwizzle sw, const PackedScale4_t &sc) __NE___ :
                action{a}, type{t}, gesture{g}, swizzle{sw}, scale{sc} {}
        };


        struct Reflection
        {
        // variables
        private:
            FlatHashMap< InputModeName::Optimized_t, String >       _modeMap;
            FlatHashMap< InputActionName::Optimized_t, String >     _actionMap;

        // methods
        public:
            Reflection ()                                                   __NE___ {}

                bool    Add (InputModeName::Ref name, StringView str)       __Th___ { return _Add( name, str, _modeMap ); }
                bool    Add (InputActionName::Ref name, StringView str)     __Th___ { return _Add( name, str, _actionMap ); }

            ND_ String  Get (InputModeName::Ref name)                       C_Th___ { return _Get( name, _modeMap ); }
            ND_ String  Get (InputActionName::Ref name)                     C_Th___ { return _Get( name, _actionMap ); }

        private:
            template <typename N, typename M>   static bool     _Add (const N &name, StringView str, M &map)    __Th___;
            template <typename N, typename M>   static String   _Get (const N &name, const M &map)              __Th___;
        };


    protected:
        static constexpr uint   _MaxModes           = 32;
        static constexpr uint   _MaxActionsPerMode  = 1024;

        enum class InputKey : uint {};

        struct ActionInfo
        {
            InputActionName     name;
            EValueType          valueType   = Default;
            EGestureType        gesture     = Default;
            VecSwizzle          swizzle     = VecSwizzle::VecDefault(4);
            PackedScale4_t      scale       {float4{1.0f}};

            ND_ packed_float4   Transform (const float4 &in)    C_NE___;
            ND_ float4          GetScale ()                     C_NE___;
        };

        using ActionMap_t = FlatHashMap< InputKey, ActionInfo >;

        struct InputMode
        {
            ActionMap_t     actions;
            bool            lockAndHideCursor   = false;    // lock into window (for shooter game)
            ESensorBits     enableSensors       = Default;

            InputMode ()                    __NE___ = default;
            InputMode (const InputMode &)   __NE___ = default;
            InputMode (InputMode &&)        __NE___ = default;
        };

        using ModeMap_t = FixedMap< InputModeName, InputMode, _MaxModes >;

        // for serialization
        static constexpr uint   _BaseVersion    = 3;


      #ifdef AE_ENABLE_SCRIPTING
        //
        // Action Bindings
        //
        struct ScriptActionBindingsBase
        {
        private:
            SerializableInputActions *  _self   = null;
            Reflection *                _refl   = null;

        public:
            ScriptActionBindingsBase () {}
            ScriptActionBindingsBase (SerializableInputActions& self, Reflection &refl) : _self{&self}, _refl{&refl} {}

            template <typename T>
            ND_ T*  _CreateMode (const String &name) __Th___;
        };

        //
        // Base Bindings Mode
        //
        struct ScriptBindingsModeBase : Scripting::AngelScriptHelper::SimpleRefCounter
        {
        protected:
            SerializableInputActions *  _self   = null;
            InputMode *                 _mode   = null;
            Reflection *                _refl   = null;

        protected:
            ScriptBindingsModeBase () {}
            ScriptBindingsModeBase (SerializableInputActions& self, InputMode& mode, Reflection &refl) :
                _self{&self}, _mode{&mode}, _refl{&refl} {}

            void  _Add (InputType_t type, const ScriptActionInfo &value)        __Th___;
            void  _Inherit (const String &name)                         __Th___;
        };
      #endif


    // variables
    protected:
        ModeMap_t                   _modeMap;

        const uint                  _version;

        NamedID_HashCollisionCheck  _hashCollisionCheck;


    // methods
    public:
        SerializableInputActions (uint ver)                             __NE___ : _version{ver} {}
        virtual ~SerializableInputActions ()                            __NE___;

        ND_ virtual bool  IsKey (InputType_t)                           C_NE___ = 0;
        ND_ virtual bool  IsKeyOrTouch (InputType_t)                    C_NE___ = 0;
        ND_ virtual bool  IsVec1D (InputType_t)                         C_NE___ = 0;
        ND_ virtual bool  IsVec2D (InputType_t)                         C_NE___ = 0;
        ND_ virtual bool  IsVec3D (InputType_t)                         C_NE___ = 0;

        ND_ virtual EValueType  RequiredValueType (InputType_t)         C_NE___ = 0;
        ND_ virtual String      InputTypeToString (InputType_t)         C_Th___ = 0;
        ND_ virtual String      SensorBitsToString (ESensorBits)        C_Th___ = 0;

      #ifdef AE_ENABLE_SCRIPTING
        ND_ virtual bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script,
                                          ArrayView<Path> includeDirs, const SourceLoc &loc,
                                          INOUT Reflection &refl)       __NE___ = 0;

        static bool  BindBase (const Scripting::ScriptEnginePtr &se)    __Th___;
      #endif

        ND_ bool  Merge (const SerializableInputActions &other);

        ND_ virtual String      ToString (const Reflection &refl)       C_Th___;
        ND_ virtual StringView  GetApiName ()                           C_NE___ = 0;

        ND_ ModeMap_t const&    GetModes ()                             C_NE___ { return _modeMap; }

        ND_ static bool  LoadSerialized (OUT ModeMap_t &modeMap, uint version, uint nameHash, MemRefRStream &stream);


    // ISerializable //
        bool  Serialize (Serializing::Serializer &)                     C_NE_OF;
        bool  Deserialize (Serializing::Deserializer &)                 __NE_OF;

        ND_ static bool  Serialize (const ModeMap_t &modeMap, uint version, Serializing::Serializer &)  __NE___;
        ND_ static bool  Deserialize (OUT ModeMap_t &modeMap, uint version, Serializing::Deserializer &)__NE___;


    protected:
        template <typename T>
        ND_ static constexpr InputKey   _Pack (T key, EGestureType gesture, EGestureState state = EGestureState::Update) __NE___;

        ND_ static constexpr auto       _Unpack (InputKey key) __NE___ -> Tuple< InputType_t, EGestureType, EGestureState >;

        ND_ static Array<Pair<InputKey,      const ActionInfo *>>   _ToArray (const ActionMap_t &actions);
        ND_ static Array<Pair<InputModeName, const InputMode *>>    _ToArray (const ModeMap_t &modes);
    };

/*
=================================================
    _Pack
=================================================
*/
    template <typename T>
    constexpr SerializableInputActions::InputKey
        SerializableInputActions::_Pack (T key, EGestureType gesture, EGestureState state) __NE___
    {
        StaticAssert( sizeof(key) == sizeof(InputType_t) );
        ASSERT( uint(gesture) <= 0xF );
        ASSERT( uint(state) <= 0xF );

        return InputKey( uint(key) | (uint(gesture) << 24) | (uint(state) << 28) );
    }

/*
=================================================
    _Unpack
=================================================
*/
    inline constexpr auto  SerializableInputActions::_Unpack (InputKey key) __NE___ -> Tuple< InputType_t, EGestureType, EGestureState >
    {
        return Tuple{   InputType_t(uint(key) & 0xFFFF),
                        EGestureType( (uint(key) >> 24) & 0xF ),
                        EGestureState( (uint(key) >> 28) & 0xF )};
    }

/*
=================================================
    ActionInfo::Transform / GetScale
=================================================
*/
    inline packed_float4  SerializableInputActions::ActionInfo::Transform (const float4 &value) C_NE___
    {
        ASSERT( (valueType == Default) == swizzle.IsUndefined() );

        return  value * swizzle * GetScale();
    }

    inline float4  SerializableInputActions::ActionInfo::GetScale () C_NE___
    {
        return float4{ scale.x.GetFast(), scale.y.GetFast(), scale.z.GetFast(), scale.w.GetFast() };
    }

/*
=================================================
    ScriptActionBindingsBase::_CreateMode
=================================================
*/
#ifdef AE_ENABLE_SCRIPTING
    template <typename T>
    T*  SerializableInputActions::ScriptActionBindingsBase::_CreateMode (const String &name) __Th___
    {
        CHECK_THROW_MSG( _self != null );

        const InputModeName     name_id {name};

        _refl->Add( name_id, name );
        _self->_hashCollisionCheck.Add( name_id, name );

        InputMode&  mode = _self->_modeMap( name_id );
        ASSERT( mode.actions.empty() );

        Scripting::AngelScriptHelper::SharedPtr< T >    ptr{ new T{ *_self, mode, *_refl }};
        return ptr.Detach();
    }
#endif

/*
=================================================
    Reflection::_Add
=================================================
*/
    template <typename N, typename M>
    bool  SerializableInputActions::Reflection::_Add (const N &name, StringView str, M &map) __Th___
    {
        return map.emplace( typename N::Optimized_t{name}, String{str} ).second;
    }

/*
=================================================
    Reflection::_Get
=================================================
*/
    template <typename N, typename M>
    String  SerializableInputActions::Reflection::_Get (const N &name, const M &map) __Th___
    {
        auto    it = map.find( typename N::Optimized_t{name} );
        if_likely( it != map.end() )
            return it->second;

        #if not AE_OPTIMIZE_IDS
        if_likely( not name.GetName().empty() )
            return String{name.GetName()};
        #endif

        return Base::ToString<16>( uint(name.GetHash32()) );
    }

/*
=================================================
    ESensorBits operators
=================================================
*/
    inline constexpr SerializableInputActions::ESensorBits&  operator |= (SerializableInputActions::ESensorBits &lhs, const SerializableInputActions::ESensorBits rhs) __NE___
    {
        lhs = BitCast<SerializableInputActions::ESensorBits>( ToNearUInt( lhs ) | ToNearUInt( rhs ));
        return lhs;
    }

} // AE::App


#ifdef AE_ENABLE_SCRIPTING
    AE_DECL_SCRIPT_OBJ(     AE::App::SerializableInputActions::ScriptActionInfo,    "ActionInfo"    );
    AE_DECL_SCRIPT_TYPE(    AE::App::EGestureState,                                 "EGestureState" );
    AE_DECL_SCRIPT_TYPE(    AE::App::EGestureType,                                  "EGestureType"  );
    AE_DECL_SCRIPT_TYPE(    AE::App::SerializableInputActions::EValueType,          "EValueType"    );
#endif
