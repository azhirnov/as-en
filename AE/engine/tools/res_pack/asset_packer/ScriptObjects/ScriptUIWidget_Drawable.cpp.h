// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
	// methods
	public:
		ScriptRectangleDrawable (const String &styleName)	__Th___ : ScriptUIDrawable{ EDrawableType::Rectangle, styleName } {}

		static void  Bind (const ScriptEnginePtr &se)		__Th___;

		RC<UI::IDrawable>  Convert (UI::Widget &)			__Th_OV;
	};
	using ScriptRectangleDrawablePtr = ScriptRC< ScriptRectangleDrawable >;



	//
	// Image Drawable
	//
	class ScriptImageDrawable final : public ScriptUIDrawable
	{
	// methods
	public:
		ScriptImageDrawable (const String &styleName)		__Th___ : ScriptUIDrawable{ EDrawableType::Image, styleName } {}

		static void  Bind (const ScriptEnginePtr &se)		__Th___;

		RC<UI::IDrawable>  Convert (UI::Widget &)			__Th_OV;
	};
	using ScriptImageDrawablePtr = ScriptRC< ScriptImageDrawable >;



	//
	// Text Drawable
	//
	class ScriptTextDrawable final : public ScriptUIDrawable
	{
	// variables
	protected:
		String		_text;

	// methods
	public:
		ScriptTextDrawable (const String &styleName)						__Th___ : ScriptUIDrawable{ EDrawableType::Text, styleName } {}
		ScriptTextDrawable (const String &styleName, const String &text)	__Th___ : ScriptTextDrawable{styleName} { _text = text; }

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

		RC<UI::IDrawable>  Convert (UI::Widget &)							__Th_OV;
	};
	using ScriptTextDrawablePtr = ScriptRC< ScriptTextDrawable >;
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
		AS_IMPL_CAST_T( binder, ScriptUIDrawable_ToBase<T> );
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
		binder.AddFactoryCtor< String >( {"styleName"} );
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

		result->SetStyleName( UI::StyleName{_styleName} );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptImageDrawable::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptImageDrawable>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		binder.AddFactoryCtor< String >( {"styleName"} );
		_BindBase( binder );
	}

/*
=================================================
	Bind
=================================================
*/
	RC<UI::IDrawable>  ScriptImageDrawable::Convert (UI::Widget &widget) __Th___
	{
		auto	result = widget.Create< UI::ImageDrawable >();
		CHECK_THROW( result );

		result->SetStyleName( UI::StyleName{_styleName} );
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptTextDrawable::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptTextDrawable>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		binder.AddFactoryCtor< String >( {"styleName"} );
		binder.AddFactoryCtor< String, String >( {"styleName", "text"} );
		_BindBase( binder );
	}

/*
=================================================
	Bind
=================================================
*/
	RC<UI::IDrawable>  ScriptTextDrawable::Convert (UI::Widget &widget) __Th___
	{
		auto	result = widget.Create< UI::TextDrawable >();
		CHECK_THROW( result );

		U8String	text;
		text.assign( _text.begin(), _text.end() );

		result->SetStyleName( UI::StyleName{_styleName} );
		result->SetText( RVRef(text) );
		return result;
	}


} // AE::AssetPacker
