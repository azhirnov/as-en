// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptUIWidget.h"

#include "graphics_hl/UI/Widget.h"

#include "ScriptObjects/ScriptUIWidget_Controller.cpp.h"
#include "ScriptObjects/ScriptUIWidget_Drawable.cpp.h"
#include "ScriptObjects/ScriptUIWidget_Layout.cpp.h"


AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIWidget,			"UIWidget"			);

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIDrawable,			"BaseUIDrawable"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptRectangleDrawable,	"RectangleDrawable"	);

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIController,		"BaseUIController"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptButtonController,	"ButtonController"	);

AE_DECL_SCRIPT_TYPE(	AE::AssetPacker::ELayoutType,				"ELayoutType"		);
AE_DECL_SCRIPT_TYPE(	AE::AssetPacker::ELayoutAlign,				"ELayoutAlign"		);
AE_DECL_SCRIPT_TYPE(	AE::AssetPacker::EStackOrigin,				"EStackOrigin"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptBaseLayout,			"BaseLayout"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptFixedLayout,			"FixedLayout"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptPaddingLayout,		"PaddingLayout"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptAlignedLayout,		"AlignedLayout"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptFillStackLayout,		"FillStackLayout"	);


namespace AE::AssetPacker
{

/*
=================================================
	constructor / destructor
=================================================
*/
	ScriptUIWidget::ScriptUIWidget ()
	{}

	ScriptUIWidget::~ScriptUIWidget ()
	{
		CHECK_MSG( _root == null,
			"call 'ScriptUIWidget::Store()' to store ui widget to archive" );
	}

/*
=================================================
	Initialize
=================================================
*/
	void  ScriptUIWidget::Initialize (const ScriptRC<ScriptBaseLayout> &root) __Th___
	{
		CHECK_THROW_MSG( root != null );
		CHECK_THROW_MSG( _root == null );

		_root = root;
	}

/*
=================================================
	Store
=================================================
*/
	void  ScriptUIWidget::Store (const String &nameInArchive) __Th___
	{
		CHECK_THROW_MSG( _root != null );

		UI::Widget::Allocator_t			alloc;
		UI::Widget::TempAllocator_t		temp_alloc;
		UI::IController::ActionMap		action_map;

		auto	widget	= UI::Widget::New( alloc, temp_alloc );
		CHECK_THROW( widget );

		ScriptBaseLayout::Convert( *widget, *_root, OUT action_map );  // throw

		{
			auto	wmem = MakeRC<MemWStream>();
			{
				Serializing::ObjectFactory	factory;
				CHECK_THROW( UI::ILayout::RegisterLayouts( factory ));
				CHECK_THROW( UI::IDrawable::RegisterDrawables( factory ));
				CHECK_THROW( UI::IController::RegisterControllers( factory ));

				Serializing::Serializer		ser {wmem};
				ser.factory = &factory;

				CHECK_THROW( widget->Serialize( ser ));
				CHECK_THROW( action_map.Serialize( ser ));
			}
			auto	rmem = wmem->ToRStream();
			ObjectStorage::Instance()->AddToArchive( nameInArchive, *rmem, EArchivePackerFileType::InMemory );  // throw
		}
		_root = null;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIWidget::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace AE::Scripting;
		{
			EnumBinder<ELayoutType>	binder{ se };
			binder.Create();
			switch_enum( ELayoutType::Unknown )
			{
				case ELayoutType::Unknown :
				case ELayoutType::FillStackLayout_Cell :
				case ELayoutType::_Count :
				case ELayoutType::_Begin_AutoSize :
				#define CASE( _name_ )		case ELayoutType::_name_ :  binder.AddValue( #_name_, ELayoutType::_name_ );
				CASE( FixedLayoutPx )
				CASE( FixedLayoutMm )
				CASE( PaddingLayoutPx )
				CASE( PaddingLayoutMm )
				CASE( PaddingLayoutRel )
				CASE( AlignedLayoutPx )
				CASE( AlignedLayoutMm )
				CASE( AlignedLayoutRel )
				CASE( StackLayoutL )
				CASE( StackLayoutR )
				CASE( StackLayoutB )
				CASE( StackLayoutT )
				CASE( FillStackLayout )
				#undef CASE
				default : break;
			}
			switch_end
		}{
			EnumBinder<ELayoutAlign>	binder{ se };
			binder.Create();
			switch_enum( ELayoutAlign::Unknown )
			{
				case ELayoutAlign::Unknown :
				#define CASE( _name_ )		case ELayoutAlign::_name_ :  binder.AddValue( #_name_, ELayoutAlign::_name_ );
				CASE( Left )
				CASE( Right )
				CASE( Bottom )
				CASE( Top )
				CASE( CenterX )
				CASE( CenterY )
				CASE( FillX )
				CASE( FillY )
				CASE( Center )
				CASE( Fill )
				#undef CASE
				default : break;
			}
			switch_end
		}{
			EnumBinder<EStackOrigin>	binder{ se };
			binder.Create();
			switch_enum( EStackOrigin::Unknown )
			{
				case EStackOrigin::Unknown :
				#define CASE( _name_ )		case EStackOrigin::_name_ :  binder.AddValue( #_name_, EStackOrigin::_name_ );
				CASE( Left )
				CASE( Right )
				CASE( Bottom )
				CASE( Top )
				#undef CASE
				default : break;
			}
			switch_end
		}

		ScriptUIDrawable::Bind( se );
		ScriptRectangleDrawable::Bind( se );

		ScriptUIController::Bind( se );
		ScriptButtonController::Bind( se );

		ScriptBaseLayout::Bind( se );
		ScriptFixedLayout::Bind( se );
		ScriptPaddingLayout::Bind( se );
		ScriptAlignedLayout::Bind( se );
		ScriptFillStackLayout::Bind( se );

		{
			ClassBinder<ScriptUIWidget>	binder{ se };
			binder.CreateRef();
			binder.AddMethod( &ScriptUIWidget::Initialize,	"Initialize"	);
			binder.AddMethod( &ScriptUIWidget::Store,		"Store"			);
		}
	}


} // AE::AssetPacker
