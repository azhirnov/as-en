// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "res_pack/asset_packer/ScriptObjects/ScriptUIWidget.h"

#include "graphics/UI/Widget.h"

#include "res_pack/asset_packer/ScriptObjects/ScriptUIWidget_Controller.cpp.h"
#include "res_pack/asset_packer/ScriptObjects/ScriptUIWidget_Drawable.cpp.h"
#include "res_pack/asset_packer/ScriptObjects/ScriptUIWidget_Layout.cpp.h"


AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIWidget,			"UIWidget"			);

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIDrawable,			"BaseUIDrawable"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptRectangleDrawable,	"RectangleDrawable"	);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptImageDrawable,		"ImageDrawable"		);
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptTextDrawable,		"TextDrawable"		);

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
			auto	wmem = MakeRC<ArrayWStream>();
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
			MemRefRStream	rmem {wmem->GetData()};
			ObjectStorage::Instance()->AddToArchive( nameInArchive, rmem, EArchivePackerFileType::InMemory );  // throw
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
			binder.BindAll();
		}{
			EnumBinder<ELayoutAlign>	binder{ se };
			binder.Create();
			binder.BindAll();
			binder.AddValue( "FillX",	ELayoutAlign::FillX );
			binder.AddValue( "FillY",	ELayoutAlign::FillY );
			binder.AddValue( "Center",	ELayoutAlign::Center );
			binder.AddValue( "Fill",	ELayoutAlign::Fill );
		}{
			EnumBinder<EStackOrigin>	binder{ se };
			binder.Create();
			binder.BindAll();
		}

		ScriptUIDrawable::Bind( se );
		ScriptRectangleDrawable::Bind( se );
		ScriptImageDrawable::Bind( se );
		ScriptTextDrawable::Bind( se );

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
			AS_METHOD( binder, ScriptUIWidget::Initialize,	"Initialize"	);
			AS_METHOD( binder, ScriptUIWidget::Store,		"Store"			);
		}
	}


} // AE::AssetPacker
