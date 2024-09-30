// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::AssetPacker
{
	using EDrawableType = UI::IDrawable::EType;



	//
	// Base UI Drawable
	//
	class ScriptUIDrawable : public EnableScriptRC
	{
	// variables
	protected:
		const EDrawableType		_type		= Default;
		String					_styleName;


	// methods
	public:
		ScriptUIDrawable (EDrawableType type, const String &styleName)	: _type{type}, _styleName{styleName} {}

		static void  Bind (const ScriptEnginePtr &se)			__Th___;

		ND_ virtual RC<UI::IDrawable>  Convert (UI::Widget &)	__Th___ = 0;

	protected:
		template <typename B>
		static void  _BindBase (B &binder)						__Th___;
	};
	using ScriptUIDrawablePtr = ScriptRC< ScriptUIDrawable >;



	//
	// Rectangle Drawable
	//
	class ScriptRectangleDrawable final : public ScriptUIDrawable
	{
	// variables
	private:


	// methods
	public:
		ScriptRectangleDrawable (const String &styleName)	__Th___ : ScriptUIDrawable{ EDrawableType::Rectangle, styleName } {}

		static void  Bind (const ScriptEnginePtr &se)		__Th___;

		RC<UI::IDrawable>  Convert (UI::Widget &)			__Th_OV;
	};
	using ScriptRectangleDrawablePtr = ScriptRC< ScriptRectangleDrawable >;
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptUIDrawable_ToBase
=================================================
*/
	template <typename T>
	static ScriptUIDrawable*  ScriptUIDrawable_ToBase (T* ptr)
	{
		StaticAssert( IsBaseOf< ScriptUIDrawable, T >);

		ScriptRC<ScriptUIDrawable>	result{ ptr };
		return result.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIDrawable::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptUIDrawable>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
	}

	template <typename B>
	void  ScriptUIDrawable::_BindBase (B &binder) __Th___
	{
		using T = typename B::Class_t;
		binder.Operators().ImplCast( &ScriptUIDrawable_ToBase<T> );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptRectangleDrawable::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptRectangleDrawable>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		binder.AddFactoryCtor< String >();
		_BindBase( binder );
	}

/*
=================================================
	Bind
=================================================
*/
	RC<UI::IDrawable>  ScriptRectangleDrawable::Convert (UI::Widget &widget) __Th___
	{
		auto	result = widget.Create< UI::RectangleDrawable >();
		CHECK_THROW( result );

		Unused( result->SetStyle( UI::StyleName{_styleName} ));
		return result;
	}


} // AE::AssetPacker
