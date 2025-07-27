// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_SCRIPTING
namespace AE::App
{
	static const float	PackedScaleEpsilon	= 8.0e-5f;

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
			EnumBinder<EGestureState>	binder{ se };
			binder.Create();
			switch_enum( EGestureState::Unknown )
			{
				case EGestureState::Unknown :
				#define BIND( _name_ )		case EGestureState::_name_ : binder.AddValue( #_name_, EGestureState::_name_ );
				BIND( Begin )
				BIND( Update )
				BIND( End )
				BIND( Cancel )
				BIND( Outside )
				#undef BIND
				default : break;
			}
			switch_end
		}

		// EGestureType
		{
			EnumBinder<EGestureType>	binder{ se };
			binder.Create();
			switch_enum( EGestureType::Unknown )
			{
				case EGestureType::Unknown :
				case EGestureType::_Count :
				#define BIND( _name_ )	case EGestureType::_name_ : binder.AddValue( #_name_, EGestureType::_name_ );
				BIND( Hold )
				BIND( Move )
				BIND( LongPress_Move )
				BIND( ScaleRotate2D )
				BIND( Down )
				BIND( Click );
				BIND( LongPress )
				BIND( DoubleClick )
				#undef BIND
				default : break;
			}
			switch_end
		}

		// EValueType
		{
			EnumBinder<EValueType>		binder{ se };
			binder.Create();
			switch_enum( EValueType::_Count )
			{
				case EValueType::_Count :
				#define BIND( _name_ )	case EValueType::_name_ : binder.AddValue( #_name_, EValueType::_name_ );
				BIND( Unknown )
				BIND( Float )
				BIND( Float2 )
				BIND( Float3 )
				BIND( Float4 )
				BIND( Quat )
				BIND( Float4x4 )
				BIND( Chars )
				BIND( GNS )
				#undef BIND
				default : break;
			}
			switch_end
		}

		// ScriptActionInfo
		{
			struct Utils {
				ND_ static VecSwizzle  ValueTypeToSwizzle (EValueType type)
				{
					switch_enum( type )
					{
						case EValueType::Float :	return VecSwizzle::VecDefault( 1 );
						case EValueType::Float2 :	return VecSwizzle::VecDefault( 2 );
						case EValueType::Float3 :	return VecSwizzle::VecDefault( 3 );
						case EValueType::Float4 :	return VecSwizzle::VecDefault( 4 );
						case EValueType::Quat :
						case EValueType::Float4x4 :
						case EValueType::Chars :
						case EValueType::GNS :		break;	// default
						case EValueType::_Count :
						case EValueType::Unknown :
						default :					DBG_WARNING( "unknown value type" ); break;
					}
					switch_end
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

					PackedScale4_t	packed{ scale };
					packed_float4	temp{ packed };

					CHECK_THROW_MSG( Equal( temp.x, scale.x, PackedScaleEpsilon ));
					CHECK_THROW_MSG( Equal( temp.y, scale.y, PackedScaleEpsilon ));
					CHECK_THROW_MSG( Equal( temp.z, scale.z, PackedScaleEpsilon ));
					CHECK_THROW_MSG( Equal( temp.w, scale.w, PackedScaleEpsilon ));

					return packed;
				}


				static void  Ctor2 (OUT void* mem, const String &action, EValueType type) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, type, EGestureType::Unknown, ValueTypeToSwizzle(type), DefaultScale() );
				}
				static void  Ctor2_1 (OUT void* mem, const String &action, EValueType type, const packed_float4 &scale) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, type, EGestureType::Unknown, ValueTypeToSwizzle(type), ConvertScale( scale ));
				}
				static void  Ctor3 (OUT void* mem, const String &action, EGestureType gesture) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, EValueType::Unknown, gesture, VecSwizzle{}, DefaultScale() );
				}
				static void  Ctor4 (OUT void* mem, const String &action, EValueType type, EGestureType gesture) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, ValueTypeToSwizzle(type), DefaultScale() );
				}
				static void  Ctor4_1 (OUT void* mem, const String &action, EValueType type, EGestureType gesture, const packed_float4 &scale) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, ValueTypeToSwizzle(type), ConvertScale( scale ));
				}
				static void  Ctor5 (OUT void* mem, const String &action, EValueType type, EGestureType gesture, const VecSwizzle &swizzle) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, swizzle, DefaultScale() );
				}
				static void  Ctor5_1 (OUT void* mem, const String &action, EValueType type, EGestureType gesture, const VecSwizzle &swizzle, const packed_float4 &scale) {
					PlacementNew<ScriptActionInfo>( OUT mem, action, type, gesture, swizzle, ConvertScale( scale ));
				}
			};

			ClassBinder<ScriptActionInfo>	binder{ se };
			binder.CreateClassValue();

			binder.AddConstructor( &Utils::Ctor2 );
			binder.AddConstructor( &Utils::Ctor2_1 );
			binder.AddConstructor( &Utils::Ctor3 );
			binder.AddConstructor( &Utils::Ctor4 );
			binder.AddConstructor( &Utils::Ctor4_1 );
			binder.AddConstructor( &Utils::Ctor5 );
			binder.AddConstructor( &Utils::Ctor5_1 );
			binder.AddProperty( &ScriptActionInfo::type,		"type"		);
			binder.AddProperty( &ScriptActionInfo::gesture,		"gesture"	);
			binder.AddProperty( &ScriptActionInfo::swizzle,		"swizzle"	);
		}

		return true;
	}

/*
=================================================
	ScriptBindingsModeBase::_Add
=================================================
*/
	void  SerializableInputActions::ScriptBindingsModeBase::_Add (const InputType_t type, const ScriptActionInfo &value) __Th___
	{
		CHECK_THROW_MSG( _self != null and _mode != null );

		ActionInfo	info;
		info.name		= InputActionName{ value.action };
		info.valueType	= value.type;
		info.gesture	= value.gesture;
		info.swizzle	= value.swizzle;
		info.scale		= value.scale;

		CHECK_THROW_MSG( Equal( info.scale.x.GetFast(), info.scale.x.Get(), PackedScaleEpsilon ));
		CHECK_THROW_MSG( Equal( info.scale.y.GetFast(), info.scale.y.Get(), PackedScaleEpsilon ));
		CHECK_THROW_MSG( Equal( info.scale.z.GetFast(), info.scale.z.Get(), PackedScaleEpsilon ));
		CHECK_THROW_MSG( Equal( info.scale.w.GetFast(), info.scale.w.Get(), PackedScaleEpsilon ));

		_refl->Add( info.name, value.action );
		_self->_hashCollisionCheck.Add( info.name, value.action );

		const EValueType	val_type = _self->RequiredValueType( type );

		if ( info.valueType == Default )
			info.valueType = val_type;

		const bool	allow_swizzle	= (info.valueType >= EValueType::Float and info.valueType <= EValueType::Float4);
		const bool	allow_scale		= allow_swizzle;

		const auto	GetMsgTemplate = [&] () {
			return "Input action '"s << StringView{value.action} << "' with key '" << _self->GetApiName() << '_' <<  _self->InputTypeToString(type) << "' ";
		};

		if ( allow_swizzle and info.swizzle.IsUndefined() )
		{
			AE_LOGI( GetMsgTemplate() << "has undefined vector swizzle, result will be always 0." );
			switch ( info.valueType ) {
				case EValueType::Float :	info.swizzle = VecSwizzle::VecDefault( 1 );	break;
				case EValueType::Float2 :	info.swizzle = VecSwizzle::VecDefault( 2 );	break;
				case EValueType::Float3 :	info.swizzle = VecSwizzle::VecDefault( 3 );	break;
				case EValueType::Float4 :	info.swizzle = VecSwizzle::VecDefault( 4 );	break;
			}
		}

		if ( not allow_swizzle )
		{
			CHECK_THROW_MSG( info.swizzle.IsUndefined(),
				GetMsgTemplate() << "has vector swizzle which is allowed only for Float[1..4] types" );
		}

		if ( not allow_scale )
		{
			CHECK_THROW_MSG( All(Equal( float4{value.scale}, float4{1.f} )),
				GetMsgTemplate() << "has scale which is allowed only for Float[1..4] types" );
		}

		if ( not allow_swizzle and val_type != Default )
		{
			CHECK_THROW_MSG( val_type == info.valueType,
				GetMsgTemplate() << "requires '" << Base::ToString(val_type) << "' but '" << Base::ToString(info.valueType) << "' provided." );
			info.valueType = val_type;
		}

		switch_enum( info.gesture )
		{
			case EGestureType::Down :
			case EGestureType::Click :
			case EGestureType::DoubleClick :
			{
				CHECK_THROW_MSG( _self->IsKeyOrTouch( type ),
					GetMsgTemplate() << "has gesture '" << Base::ToString(info.gesture) << "' which requires Key or Touch type." );

				CHECK_THROW_MSG(
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End   ), info ).second,
					GetMsgTemplate() << "with gesture '" << Base::ToString(info.gesture) << "' override 'Begin / End' events." );
				break;
			}

			case EGestureType::LongPress :
			{
				CHECK_THROW_MSG( _self->IsKeyOrTouch( type ),
					GetMsgTemplate() << "has gesture 'LongPress' which requires Key or Touch type." );

				if ( _self->IsKey( type )) {
					CHECK_THROW_MSG( info.valueType == EValueType::Float or info.valueType == Default,
						GetMsgTemplate() << "has gesture 'LongPress' which requires 'Unknown / Float' types." );
				}else
				if ( _self->IsKeyOrTouch( type )) {
					CHECK_THROW_MSG( info.valueType == EValueType::Float3 or info.valueType == Default,
						GetMsgTemplate() << "has gesture 'LongPress' which requires 'Unknown / Float3' types." );
				}

				CHECK_THROW_MSG(
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin  ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End    ), info ).second,
					GetMsgTemplate() << "with gesture '" << Base::ToString(info.gesture) << "' override 'Begin / Update / End' events." );
				break;
			}

			case EGestureType::Hold :
			{
				CHECK_THROW_MSG( _self->IsKey( type ),
					GetMsgTemplate() << "has gesture 'Hold' which requires Key type." );

				CHECK_THROW_MSG( info.valueType == Default or (info.valueType >= EValueType::Float and info.valueType <= EValueType::Float4),
					GetMsgTemplate() << "has gesture 'Hold' which requires 'Unknown / Float[1..4]' types. " );

				CHECK_THROW_MSG(
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin  ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End    ), info ).second,
					GetMsgTemplate() << "with gesture '" << Base::ToString(info.gesture) << "' override 'Begin / Update / End' events." );
				break;
			}

			case EGestureType::Move :
			case EGestureType::LongPress_Move :
			{
				CHECK_THROW_MSG(
					info.valueType == EValueType::Quat or info.valueType == EValueType::Float4x4 or
					(info.valueType >= EValueType::Float and info.valueType <= EValueType::Float4),
					GetMsgTemplate() << "has gesture 'Move' which requires 'Float[1..4] / Quat / Float4x4' types. " );

				const packed_uint4	sw		= value.swizzle.ToVec();
				uint				max_c	= 0;

				for (int i = 0; i < 4; ++i)
				{
					uint	x = sw[i];
					if ( x >= 1 and x <= 4 )
						AssignMax( INOUT max_c, x-1 );
				}

				switch ( max_c ) {
					case 0 :	break;
					case 1 :	CHECK_THROW_MSG( _self->IsVec1D( type ) or _self->IsVec2D( type ) or _self->IsVec3D( type ));	break;
					case 2 :	CHECK_THROW_MSG( _self->IsVec2D( type ) or _self->IsVec3D( type ));								break;
					case 3 :	CHECK_THROW_MSG( _self->IsVec3D( type ));														break;
					default :	CHECK_THROW_MSG( false );	break;
				}

				CHECK_THROW_MSG(
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Begin  ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second and
					_mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::End    ), info ).second,
					GetMsgTemplate() << "with gesture '" << Base::ToString(info.gesture) << "' override 'Begin / Update / End' events." );
				break;
			}

			case EGestureType::ScaleRotate2D :
			{
				CHECK_THROW_MSG( info.valueType == EValueType::Float2 or info.valueType == EValueType::Float4,
					GetMsgTemplate() << "has gesture 'ScaleRotate2D' which requires 'Float2/4' type" );

				// TODO
				//CHECK_THROW_MSG( _self->IsVec2D( type ),
				//	GetMsgTemplate() << "has gesture 'ScaleRotate2D' which requires MultiTouch type" );

				CHECK_THROW_MSG( _mode->actions.insert_or_assign( _Pack( type, info.gesture, EGestureState::Update ), info ).second,
					GetMsgTemplate() << "with gesture '" << Base::ToString(info.gesture) << "' override 'Begin / Update / End' events." );
				break;
			}

			case EGestureType::_Count :
			case EGestureType::Unknown :
			default :
				CHECK_THROW_MSG( false,
					GetMsgTemplate() << "has unsupported gesture type" );
		}
		switch_end
	}

/*
=================================================
	ScriptBindingsModeBase::_Inherit
=================================================
*/
	void  SerializableInputActions::ScriptBindingsModeBase::_Inherit (const String &name) __Th___
	{
		CHECK_THROW_MSG( _self != null and _mode != null );

		auto	it = _self->_modeMap.find( InputModeName{ name });
		CHECK_THROW_MSG( it != _self->_modeMap.end(),
						 "BindingsMode '"s << name << "' is not defined" );

		_mode->lockAndHideCursor = it->second.lockAndHideCursor;

		for (auto& [key, val] : it->second.actions) {
			CHECK_THROW_MSG( _mode->actions.insert_or_assign( key, val ).second );
		}
	}

} // AE::App
#endif // AE_ENABLE_SCRIPTING
