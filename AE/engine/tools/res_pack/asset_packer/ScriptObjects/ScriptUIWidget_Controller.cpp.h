// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::AssetPacker
{
	using EControllerType = UI::IController::EType;
	using UI::ELayoutAlign;
	using UI::ActionName;



	//
	// Base UI Controller
	//
	class ScriptUIController : public EnableScriptRC
	{
	// types
	public:
		using ActionMapBuilder = UI::IController::ActionMapBuilder;

	// variables
	protected:
		const EControllerType	_type	= Default;


	// methods
	public:
		ScriptUIController (EControllerType type)							: _type{type} {}

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

		ND_ virtual RC<UI::IController>  Convert (UI::Widget &,
												  INOUT ActionMapBuilder &)	__Th___ = 0;

	protected:
		template <typename B>
		static void  _BindBase (B &binder)									__Th___;

		static void  _DummyCb (UI::ILayout::LayoutState const&, AnyTypeRef)	{}
	};
	using ScriptUIControllerPtr = ScriptRC< ScriptUIController >;



	//
	// Button Controller
	//
	class ScriptButtonController final : public ScriptUIController
	{
	// variables
	private:
		ActionName		_onClick;
		ActionName		_onDoubleClick;
		ActionName		_onLongPress;


	// methods
	public:
		ScriptButtonController ()								__Th___ : ScriptUIController{ EControllerType::Button } {}

		void  SetOnClick (const String &name)					__Th___;
		void  SetOnDoubleClick (const String &name)				__Th___;
		void  SetOnLongPress (const String &name)				__Th___;

		static void  Bind (const ScriptEnginePtr &se)			__Th___;

		RC<UI::IController>  Convert (UI::Widget &,
									  INOUT ActionMapBuilder &)	__Th_OV;
	};
	using ScriptButtonControllerPtr = ScriptRC< ScriptButtonController >;
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptUIController_ToBase
=================================================
*/
	template <typename T>
	static ScriptUIController*  ScriptUIController_ToBase (T* ptr)
	{
		StaticAssert( IsBaseOf< ScriptUIController, T >);

		ScriptRC<ScriptUIController>	result{ ptr };
		return result.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIController::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptUIController>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
	}

	template <typename B>
	void  ScriptUIController::_BindBase (B &binder) __Th___
	{
		using T = typename B::Class_t;
		binder.Operators().ImplCast( &ScriptUIController_ToBase<T> );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptButtonController::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptButtonController>	binder{ se };
		binder.CreateRef();
		_BindBase( binder );
		binder.AddMethod( &ScriptButtonController::SetOnClick,			"OnClick",			{} );
		binder.AddMethod( &ScriptButtonController::SetOnDoubleClick,	"OnDoubleClick",	{} );
		binder.AddMethod( &ScriptButtonController::SetOnLongPress,		"OnLongPress",		{} );
	}

/*
=================================================
	Convert
=================================================
*/
	RC<UI::IController>  ScriptButtonController::Convert (UI::Widget &widget, INOUT ActionMapBuilder &actionMap) __Th___
	{
		auto	result = widget.Create< UI::ButtonController >();
		CHECK_THROW( result );

		result->SetActions( _onClick, _onDoubleClick, _onLongPress );

		if ( _onClick.IsDefined() )
			CHECK_THROW( actionMap.Bind( _onClick, _DummyCb ));

		if ( _onDoubleClick.IsDefined() )
			CHECK_THROW( actionMap.Bind( _onDoubleClick, _DummyCb ));

		if ( _onLongPress.IsDefined() )
			CHECK_THROW( actionMap.Bind( _onLongPress, _DummyCb ));

		return result;
	}

/*
=================================================
	SetOn*
=================================================
*/
	void  ScriptButtonController::SetOnClick (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<ActionName>( name );  // throw
		_onClick = ActionName{name};
	}

	void  ScriptButtonController::SetOnDoubleClick (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<ActionName>( name );  // throw
		_onDoubleClick = ActionName{name};
	}

	void  ScriptButtonController::SetOnLongPress (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<ActionName>( name );  // throw
		_onLongPress = ActionName{name};
	}


} // AE::AssetPacker
