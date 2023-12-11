// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/SerializableInputActions.h"
#include "pch/Scripting.h"

namespace AE::App
{
    using namespace AE::Serializing;

/*
=================================================
    destructor
=================================================
*/
    SerializableInputActions::~SerializableInputActions () __NE___
    {
        // will generate error if has hash collisions
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  SerializableInputActions::Serialize (Serializer &ser) C_NE___
    {
        return Serialize( _modeMap, _version, ser );
    }

    bool  SerializableInputActions::Serialize (const ModeMap_t &modeMap, const uint version, Serializer &ser) __NE___
    {
        bool    res = ser( version, uint(modeMap.size()) );
        CHECK_ERR( res );

        for (auto [name, mode] : modeMap)
        {
            res &= ser( name, mode.lockAndHideCursor, uint(mode.actions.size()) );

            for (auto& [type, info] : mode.actions)
            {
                res &= ser( type, info.name, info.valueType, info.gesture, info.swizzle, info.scale );
            }
        }

        CHECK_ERR( res );
        return true;
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  SerializableInputActions::Deserialize (Deserializer &des) __NE___
    {
        return Deserialize( OUT _modeMap, _version, des );
    }

    bool  SerializableInputActions::Deserialize (OUT ModeMap_t &modeMap, const uint version, Deserializer &des) __NE___
    {
        modeMap.clear();

        bool    res         = true;
        uint    mode_count  = 0;
        uint    ver         = 0;

        res = des( OUT ver );
        CHECK_ERR( res and ver == version );

        res = des( OUT mode_count );
        CHECK_ERR( res and mode_count <= _MaxModes );

        for (uint i = 0; res & (i < mode_count); ++i)
        {
            InputModeName   mode_name;
            uint            act_count   = 0;
            bool            lock_cursor = false;

            res = des( OUT mode_name, OUT lock_cursor, OUT act_count );
            CHECK_ERR( res and act_count <= _MaxActionsPerMode );

            auto&   mode = modeMap( mode_name );

            mode.lockAndHideCursor = lock_cursor;
            mode.actions.reserve( act_count );

            for (uint j = 0; res & (j < act_count); ++j)
            {
                InputKey    key;
                ActionInfo  info;

                res = des( OUT key, OUT info.name, OUT info.valueType, OUT info.gesture, OUT info.swizzle, OUT info.scale );

                CHECK( mode.actions.insert_or_assign( key, info ).second );
            }
        }

        CHECK_ERR( res );
        return true;
    }

/*
=================================================
    Merge
=================================================
*/
    bool  SerializableInputActions::Merge (const SerializableInputActions &other)
    {
        for (auto [name, mode] : other._modeMap)
        {
            CHECK_ERR( not _modeMap.contains( name ));
            CHECK_ERR( _modeMap.emplace( name, mode ).first != Default );
        }

        _hashCollisionCheck.Merge( other._hashCollisionCheck );
        return true;
    }

/*
=================================================
    LoadSerialized
=================================================
*/
    bool  SerializableInputActions::LoadSerialized (OUT ModeMap_t &modeMap, const uint version, const uint nameHash, MemRefRStream &stream)
    {
        CHECK_ERR( stream.IsOpen() );

        uint    name;
        CHECK_ERR( stream.Read( OUT name ) and name == InputActions_Name );

        Array<uint> hashes;
        Array<uint> offsets;

        // read header
        {
            uint    count;
            CHECK_ERR( stream.Read( OUT count ));

            CHECK_ERR( stream.Read( count, OUT hashes ));
            CHECK_ERR( stream.Read( count, OUT offsets ));

            offsets.push_back( uint(stream.Size()) );
        }

        for (usize i = 0; i < hashes.size(); ++i)
        {
            ASSERT( offsets[i] < offsets[i+1] );

            if ( hashes[i] == nameHash )
            {
                Deserializer    des{ stream.ToSubStream( Bytes{offsets[i]}, Bytes{offsets[i+1] - offsets[i]} )};
                CHECK_ERR( Deserialize( OUT modeMap, version, des ));

                ASSERT( des.IsEnd() );
                return true;
            }
        }
        return false;
    }

/*
=================================================
    _ToArray
=================================================
*/
    Array<Pair<SerializableInputActions::InputKey, const SerializableInputActions::ActionInfo *>>
        SerializableInputActions::_ToArray (const ActionMap_t &actions)
    {
        Array<Pair<InputKey, const ActionInfo *>>   result;
        result.reserve( actions.size() );

        for (auto& [key, info] : actions) {
            result.emplace_back( key, &info );
        }

        std::sort( result.begin(), result.end(), [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; });
        return result;
    }

/*
=================================================
    _ToArray
=================================================
*/
    Array<Pair<InputModeName, const SerializableInputActions::InputMode *>>
        SerializableInputActions::_ToArray (const ModeMap_t &modeMap)
    {
        Array<Pair<InputModeName, const InputMode *>>   result;
        result.reserve( modeMap.size() );

        for (auto [name, mode] : modeMap) {
            result.emplace_back( name, &mode );
        }

        std::sort( result.begin(), result.end(), [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; });
        return result;
    }
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_SCRIPTING
    static const float  PackedScaleEpsilon  = 8.0e-5f;

/*
=================================================
    BindBase
=================================================
*/
    bool  SerializableInputActions::BindBase (const Scripting::ScriptEnginePtr &se)
    {
        using namespace AE::Scripting;

        CoreBindings::BindStdTypes( se );
        CoreBindings::BindScalarMath( se );
        CoreBindings::BindVectorMath( se );
        CoreBindings::BindString( se );
        CoreBindings::BindVectorSwizzle( se );

        // EGestureState
        {
            EnumBinder<EGestureState>   binder{ se };
            binder.Create();
            binder.AddValue( "Begin",   EGestureState::Begin    );
            binder.AddValue( "Update",  EGestureState::Update   );
            binder.AddValue( "End",     EGestureState::End      );
        }

        // EGestureType
        {
            EnumBinder<EGestureType>    binder{ se };
            binder.Create();
            binder.AddValue( "Hold",            EGestureType::Hold              );
            binder.AddValue( "Move",            EGestureType::Move              );
            binder.AddValue( "LongPress_Move",  EGestureType::LongPress_Move    );
            binder.AddValue( "ScaleRotate2D",   EGestureType::ScaleRotate2D     );
            binder.AddValue( "Down",            EGestureType::Down              );
            binder.AddValue( "Click",           EGestureType::Click             );
            binder.AddValue( "LongPress",       EGestureType::LongPress         );
            binder.AddValue( "DoubleClick",     EGestureType::DoubleClick       );
            StaticAssert( uint(EGestureType::_Count) == 8 );
        }

        // EValueType
        {
            EnumBinder<EValueType>      binder{ se };
            binder.Create();
            binder.AddValue( "Unknown",     EValueType::Unknown     );
            binder.AddValue( "Bool",        EValueType::Bool        );
            binder.AddValue( "Int",         EValueType::Int         );
            binder.AddValue( "Float",       EValueType::Float       );
            binder.AddValue( "Float2",      EValueType::Float2      );
            binder.AddValue( "Float3",      EValueType::Float3      );
            binder.AddValue( "Float4",      EValueType::Float4      );
            binder.AddValue( "Quat",        EValueType::Quat        );
            binder.AddValue( "Float4x4",    EValueType::Float4x4    );
            binder.AddValue( "String",      EValueType::String      );
            StaticAssert( uint(EValueType::_Count) == 9 );
        }

        // ScriptActionInfo
        {
            struct Utils {
                ND_ static VecSwizzle  VelueTypeToSwizzle (EValueType type)
                {
                    BEGIN_ENUM_CHECKS();
                    switch ( type )
                    {
                        case EValueType::Bool :
                        case EValueType::Int :
                        case EValueType::Float :    return VecSwizzle::VecDefault( 1 );
                        case EValueType::Float2 :   return VecSwizzle::VecDefault( 2 );
                        case EValueType::Float3 :   return VecSwizzle::VecDefault( 3 );
                        case EValueType::Float4 :   return VecSwizzle::VecDefault( 4 );
                        case EValueType::Quat :
                        case EValueType::Float4x4 :
                        case EValueType::String :   break;  // default
                        case EValueType::_Count :
                        case EValueType::Unknown :
                        default :                   DBG_WARNING( "unknown value type" ); break;
                    }
                    END_ENUM_CHECKS();
                    return Default;
                }

                ND_ static PackedScale4_t  DefaultScale () {
                    return PackedScale4_t{ float4{1.0f} };
                }

                ND_ static PackedScale4_t  ConvertScale (const packed_float4 &scale)
                {
                    if constexpr( IsUnsignedFloatPoint< PackedScale_t >)
                        CHECK_THROW_MSG( All( scale > packed_float4{0.f} ), "negative scale is not supported" );

                    CHECK_THROW_MSG( All( (Abs(scale) > packed_float4{PackedScaleEpsilon}) | (scale == packed_float4{}) ), "scale is to small" );

                    PackedScale4_t  packed{ scale };
                    packed_float4   temp{ packed };

                    CHECK_THROW_MSG( Equal( temp.x, scale.x, PackedScaleEpsilon ));
                    CHECK_THROW_MSG( Equal( temp.y, scale.y, PackedScaleEpsilon ));
                    CHECK_THROW_MSG( Equal( temp.z, scale.z, PackedScaleEpsilon ));
                    CHECK_THROW_MSG( Equal( temp.w, scale.w, PackedScaleEpsilon ));

                    return packed;
                }


                static void  Ctor1 (OUT void* mem, const String &action) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, EValueType::Unknown, EGestureType::Unknown, VecSwizzle{}, DefaultScale() );
                }
                static void  Ctor2 (OUT void* mem, const String &action, EValueType type) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, type, EGestureType::Unknown, VelueTypeToSwizzle(type), DefaultScale() );
                }
                static void  Ctor2_1 (OUT void* mem, const String &action, EValueType type, const packed_float4 &scale) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, type, EGestureType::Unknown, VelueTypeToSwizzle(type), ConvertScale( scale ));
                }
                static void  Ctor3 (OUT void* mem, const String &action, EGestureType gesture) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, EValueType::Unknown, gesture, VecSwizzle{}, DefaultScale() );
                }
                static void  Ctor4 (OUT void* mem, const String &action, EValueType type, EGestureType gesture) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, VelueTypeToSwizzle(type), DefaultScale() );
                }
                static void  Ctor4_1 (OUT void* mem, const String &action, EValueType type, EGestureType gesture, const packed_float4 &scale) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, VelueTypeToSwizzle(type), ConvertScale( scale ));
                }
                static void  Ctor5 (OUT void* mem, const String &action, EValueType type, EGestureType gesture, const VecSwizzle &swizzle) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, swizzle, DefaultScale() );
                }
                static void  Ctor5_1 (OUT void* mem, const String &action, EValueType type, EGestureType gesture, const VecSwizzle &swizzle, const packed_float4 &scale) {
                    PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, swizzle, ConvertScale( scale ));
                }
            };

            ClassBinder<ScriptActionInfo>   binder{ se };
            binder.CreateClassValue();

            //binder.AddConstructor( &Utils::Ctor1 );
            binder.AddConstructor( &Utils::Ctor2 );
            binder.AddConstructor( &Utils::Ctor2_1 );
            binder.AddConstructor( &Utils::Ctor3 );
            binder.AddConstructor( &Utils::Ctor4 );
            binder.AddConstructor( &Utils::Ctor4_1 );
            binder.AddConstructor( &Utils::Ctor5 );
            binder.AddConstructor( &Utils::Ctor5_1 );
            //binder.AddProperty( &ScriptActionInfo::action,    "action"    );
            binder.AddProperty( &ScriptActionInfo::type,        "type"      );
            binder.AddProperty( &ScriptActionInfo::gesture,     "gesture"   );
            binder.AddProperty( &ScriptActionInfo::swizzle,     "swizzle"   );
        }

        return true;
    }

/*
=================================================
    ScriptBindingsModeBase::_Add
=================================================
*/
    void  SerializableInputActions::ScriptBindingsModeBase::_Add (ushort type, const ScriptActionInfo &value) __Th___
    {
        CHECK_THROW_MSG( _self != null and _mode != null );

        ActionInfo  info;
        info.name       = InputActionName{ value.action };
        info.valueType  = value.type;
        info.gesture    = value.gesture;
        info.swizzle    = value.swizzle;
        info.scale      = value.scale;

        CHECK_THROW_MSG( Equal( info.scale.x.GetFast(), info.scale.x.Get(), PackedScaleEpsilon ));
        CHECK_THROW_MSG( Equal( info.scale.y.GetFast(), info.scale.y.Get(), PackedScaleEpsilon ));
        CHECK_THROW_MSG( Equal( info.scale.z.GetFast(), info.scale.z.Get(), PackedScaleEpsilon ));
        CHECK_THROW_MSG( Equal( info.scale.w.GetFast(), info.scale.w.Get(), PackedScaleEpsilon ));

        _refl->Add( info.name, value.action );
        _self->_hashCollisionCheck.Add( info.name, value.action );

        BEGIN_ENUM_CHECKS();
        switch ( info.gesture )
        {
            case EGestureType::Down :
            case EGestureType::Click :
            case EGestureType::DoubleClick :
            {
                CHECK_THROW_MSG( _self->IsKeyOrTouch( type ));
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End   ), info ).second );
                break;
            }
            case EGestureType::LongPress :
            {
                CHECK_THROW_MSG( info.valueType == EValueType::Float or info.valueType == Default,
                                 "gesture 'LongPress' requires 'Unknown/Float' types" );

                CHECK_THROW_MSG( _self->IsKeyOrTouch( type ));
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin  ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End    ), info ).second );
                break;
            }
            case EGestureType::Hold :
            {
                CHECK_THROW_MSG( _self->IsKey( type ));
                CHECK_THROW_MSG( info.valueType == Default or
                                (info.valueType >= EValueType::Float and info.valueType <= EValueType::Float4) );

                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin  ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End    ), info ).second );
                break;
            }

            case EGestureType::Move :
            case EGestureType::LongPress_Move :
            {
                if ( info.valueType == EValueType::Float ){
                    CHECK_THROW_MSG( _self->IsCursor1D( type ));
                }else
                if ( info.valueType == EValueType::Float2 or info.valueType == EValueType::Float3 ){
                    CHECK_THROW_MSG( _self->IsCursor2D( type ));
                }else
                    CHECK_THROW_MSG( false, "gesture 'Move' requires 'Float1/2/3' types" );

                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin  ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second );
                CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End    ), info ).second );
                break;
            }

            case EGestureType::ScaleRotate2D :
            {
                if ( info.valueType == EValueType::Float2 or info.valueType == EValueType::Float4 )
                {
                    CHECK_THROW_MSG( _self->IsCursor2D( type ));
                    CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second );
                }
                else
                    CHECK_THROW_MSG( false, "gesture 'ScaleRotate2D' requires 'Float2/4' type" );
                break;
            }

            case EGestureType::_Count :
            case EGestureType::Unknown :
            default :
                CHECK_THROW_MSG( false, "unknown gesture type" );
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    ScriptBindingsModeBase::_Inherit
=================================================
*/
    void  SerializableInputActions::ScriptBindingsModeBase::_Inherit (const String &name) __Th___
    {
        CHECK_THROW_MSG( _self != null and _mode != null );

        auto    it = _self->_modeMap.find( InputModeName{ name });
        CHECK_THROW_MSG( it != _self->_modeMap.end(),
                         "BindingsMode '"s << name << "' is not defined" );

        _mode->lockAndHideCursor = it->second.lockAndHideCursor;

        for (auto& [key, val] : it->second.actions) {
            CHECK_THROW_MSG( _mode->actions.insert_or_assign( key, val ).second );
        }
    }

#endif // AE_ENABLE_SCRIPTING

} // AE::App
