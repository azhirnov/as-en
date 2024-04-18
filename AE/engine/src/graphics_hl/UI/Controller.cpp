// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/Controller.h"
#include "graphics_hl/UI/Drawable.h"

namespace AE::UI
{
	using namespace AE::Serializing;

/*
=================================================
	TryToCaptureCursor
=================================================
*/
	IController::InputState::CursorData const*
		IController::InputState::TryToCaptureCursor (IController* current, const RectF &rect, bool hasActiveEvents) C_NE___
	{
		if ( (_cursor._focused == current) or (_cursor._focused == null) )
		{
			if ( rect.Intersects( _cursor._position ) or hasActiveEvents )
			{
				_cursor._resetFocus = not hasActiveEvents;
				_cursor._focused	= current;
				return &_cursor;
			}

			_cursor._resetFocus |= (_cursor._focused == current);
		}
		return null;
	}

/*
=================================================
	ReleaseCursor
=================================================
*/
	bool  IController::InputState::ReleaseCursor (IController* current) C_NE___
	{
		if ( _cursor._focused == current )
		{
			_cursor._resetFocus = true;
			return true;
		}
		return false;
	}

/*
=================================================
	_SetCursorState
=================================================
*/
	void  IController::InputState::_SetCursorState (const float2 &pos, const float2 &wheel, float longPressTime,
													const float4 &scaleRotate, EGestureBits bits) __NE___
	{
		_cursor._position		= Any( pos < -1.f ) ? _cursor._position : pos;
		_cursor._wheel			= wheel;
		_cursor._scaleRotate	= scaleRotate;
		_cursor._longPressTime	= longPressTime;
		_cursor._bits			= bits;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	bool  IController::ActionMapBuilder::Bind (ActionName::Ref act, Callback_t cb) __NE___
	{
		CHECK_ERR( act.IsDefined() );
		CHECK_ERR( cb );

		bool	inserted = _actions.insert_or_assign( act, RVRef(cb) ).second;
		ASSERT_MSG( inserted, "action binding was overriden" );

		return inserted;
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  IController::ActionMapBuilder::Deserialize (Serializing::Deserializer &des) __NE___
	{
		uint	count = 0;
		CHECK_ERR( des( OUT count ));

		_actions.reserve( count );

		bool	res = true;
		for (uint i = 0; (i < count) and res; ++i)
		{
			ActionName	act;
			res = des( OUT act );
			if ( res )
				res = _actions.insert_or_assign( act, Callback_t{} ).second;
		}
		return res;
	}

/*
=================================================
	IsAllBound
=================================================
*/
	bool  IController::ActionMapBuilder::IsAllBound () C_NE___
	{
		bool	res = true;
		for (auto& [act, cb] : _actions) {
			res &= bool{cb};
		}
		return res;
	}

/*
=================================================
	Merge
=================================================
*/
	bool  IController::ActionMapBuilder::Merge (ActionMapBuilder &from) __NE___
	{
		bool	res = true;

		for (auto& [act, new_cb] : from._actions)
		{
			auto&	old_cb = _actions[ act ];

			res &= (bool{old_cb} != bool{new_cb});
			ASSERT( res );

			if ( not old_cb )
				old_cb = RVRef(new_cb);
		}
		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Call
=================================================
*/
	forceinline
	void  IController::ActionMap::Call (ActionName::Ref				act,
										ILayout::LayoutState const&	state,
										AnyTypeRef					data) C_NE___
	{
		ASSERT( act.IsDefined() );

		auto	it = _actions.find( act );
		ASSERT( it != _actions.end() );

		if_likely( it != _actions.end() )
		{
			it->second( state, data );
		}
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  IController::ActionMap::Serialize (Serializing::Serializer &ser) C_NE___
	{
		bool	res = ser( uint(_actions.size()) );

		for (auto& [act, cb] : _actions)
		{
			res &= ser( act );
		}
		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Update
=================================================
*/
	void  ButtonController::Update (const UpdateParams &p) __NE___
	{
		using EGesture = InputState::EGesture;

		EStyleState		new_state = Default;

		if_unlikely( auto* data = p.input.TryToCaptureCursor( this, p.state.GlobalRect(), false ))
		{
			new_state = (data->IsTouchDown() ? EStyleState::TouchDown : EStyleState::MouseOver);

			if_unlikely( data->Is( EGesture::Click ) and _onClick.IsDefined() )
				p.actionMap.Call( _onClick, p.state );

			if_unlikely( data->Is( EGesture::DoubleClick ) and _onDoubleClick.IsDefined() )
				p.actionMap.Call( _onDoubleClick, p.state );

			if_unlikely( data->Is( EGesture::LongPress ) and _onLongPress.IsDefined() )
				p.actionMap.Call( _onLongPress, p.state );
		}

		p.state.SetStyle( new_state );
	}

/*
=================================================
	SetActions
=================================================
*/
	void  ButtonController::SetActions (ActionName::Ref	onClick,
										ActionName::Ref onDoubleClick,
										ActionName::Ref onLongPress)
	{
		_onClick		= onClick;
		_onDoubleClick	= onDoubleClick;
		_onLongPress	= onLongPress;
	}

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  ButtonController::Serialize (Serializer &ser) C_NE___
	{
		return ser( _GetControllerID(GetType()), _onClick, _onDoubleClick, _onLongPress );
	}

	bool  ButtonController::Deserialize (Deserializer &des) __NE___
	{
		return des( OUT _onClick, OUT _onDoubleClick, OUT _onLongPress );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Update
=================================================
*/
	void  SelectionController::Update (const UpdateParams &) __NE___
	{
		// TODO
	}

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  SelectionController::Serialize (Serializer &ser) C_NE___
	{
		return ser( _GetControllerID(GetType()), _onSelectionChanged );
	}

	bool  SelectionController::Deserialize (Deserializer &des) __NE___
	{
		return des( OUT _onSelectionChanged );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Update
=================================================
*/
	void  DraggableController::Update (const UpdateParams &) __NE___
	{
		// TODO
	}

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  DraggableController::Serialize (Serializer &) C_NE___
	{
		return true;
	}

	bool  DraggableController::Deserialize (Deserializer &) __NE___
	{
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Update
=================================================
*/
	void  ResizableController::Update (const UpdateParams &) __NE___
	{
		// TODO
	}

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  ResizableController::Serialize (Serializer &) C_NE___
	{
		return true;
	}

	bool  ResizableController::Deserialize (Deserializer &) __NE___
	{
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ControllerSerializer
=================================================
*/
namespace
{
	template <typename T>
	struct ControllerSerializer
	{
		static bool  Serialize (Serializer &, const void*) __NE___ { return false; }

		static bool  Deserialize (Deserializer &des, INOUT void* &ptr, Ptr<IAllocator> alloc) __NE___
		{
			CHECK_ERR( ptr == null and alloc != null );

			ptr = alloc->Allocate<T>();
			CHECK_ERR( ptr != null );

			return PlacementNew<T>( OUT ptr, alloc )->Deserialize( des );
		}
	};
}
/*
=================================================
	RegisterControllers
=================================================
*/
	bool  IController::RegisterControllers (Serializing::ObjectFactory &factory) __NE___
	{
		switch_enum( EType::Unknown )
		{
			case EType::Unknown :
			case EType::Selection :
			case EType::Draggable :
			case EType::Resizable :
			case EType::_Count :
			#define REG( _name_ )																\
				case EType::_name_ :															\
					CHECK_ERR( factory.Register< _name_##Controller >(							\
									SerializedID{ #_name_ "Controller" },						\
									&ControllerSerializer< _name_##Controller >::Serialize,		\
									&ControllerSerializer< _name_##Controller >::Deserialize ));
			REG( Button )
		//	REG( Selection )
		//	REG( Draggable )
		//	REG( Resizable )
			#undef REG
			default : break;
		}
		switch_end
		return true;
	}

/*
=================================================
	_GetControllerID
=================================================
*/
	SerializedID  IController::_GetControllerID (EType type) __NE___
	{
		switch_enum( type )
		{
			#define REG( _name_ )	case EType::_name_ : return SerializedID{ #_name_ "Controller" };
			REG( Button )
			REG( Selection )
			REG( Draggable )
			REG( Resizable )
			#undef REG
			case EType::_Count :
			case EType::Unknown :	break;
		}
		switch_end
		return Default;
	}


} // AE::UI
