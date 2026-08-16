// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <asset_packer.as>

void ASmain ()
{
	RC<UIStyleCollection>	collection = UIStyleCollection();

	collection.Resources( "ui_res_meta" );

	// red
	{
		const RGBA8u	col_disabled	= RGBA8u( 100,  0,   0, 255 );
		const RGBA8u	col_enabled		= RGBA8u( 200,  0,   0, 255 );
		const RGBA8u	col_mouse_over	= RGBA8u( 255,  0,  50, 255 );
		const RGBA8u	col_touch_down	= RGBA8u( 255,  0, 150, 255 );
		const RGBA8u	col_selected	= RGBA8u( 150, 50, 150, 255 );

		{
			RC<UIColorStyle>	style = collection.AddColorStyle( "col_red" );
			style.Pipeline	( "ui.draw1" );
			style.Disabled	( col_disabled );
			style.Enabled	( col_enabled );
			style.MouseOver	( col_mouse_over );
			style.TouchDown	( col_touch_down );
			style.Selected	( col_selected );
		}{
			RC<UIImageStyle>	style = collection.AddImageStyle( "button_red" );
			style.Pipeline	( "ui.draw1" );
			style.Image		( "ui_atlas", "Red" );
			style.Disabled	( col_disabled );
			style.Enabled	( col_enabled );
			style.MouseOver	( col_mouse_over );
			style.TouchDown	( col_touch_down );
			style.Selected	( col_selected );
		}/*{
			RC<UIFontStyle>		style = collection.AddFontStyle( "text_red" );
			style.Pipeline	( "ui.draw1" );
			style.Disabled	( col_disabled );
			style.Enabled	( col_enabled );
			style.MouseOver	( col_mouse_over );
			style.TouchDown	( col_touch_down );
			style.Selected	( col_selected );
			style.Font		( "ui_res_meta", "ui_font" );
		}*/
	}

	// green
	{
		const RGBA8u	col_disabled	= RGBA8u(  0, 100,   0, 255 );
		const RGBA8u	col_enabled		= RGBA8u(  0, 200,   0, 255 );
		const RGBA8u	col_mouse_over	= RGBA8u(  0, 255,  50, 255 );
		const RGBA8u	col_touch_down	= RGBA8u(  0, 255, 150, 255 );
		const RGBA8u	col_selected	= RGBA8u( 50, 150, 150, 255 );

		{
			RC<UIColorStyle>	style = collection.AddColorStyle( "col_green" );
			style.Pipeline	( "ui.draw1" );
			style.Disabled	( col_disabled );
			style.Enabled	( col_enabled );
			style.MouseOver	( col_mouse_over );
			style.TouchDown	( col_touch_down );
			style.Selected	( col_selected );
		}{
			RC<UIImageStyle>	style = collection.AddImageStyle( "button_green" );
			style.Pipeline	( "ui.draw1" );
			style.Image		( "ui_atlas", "Green" );
			style.Disabled	( col_disabled );
			style.Enabled	( col_enabled );
			style.MouseOver	( col_mouse_over );
			style.TouchDown	( col_touch_down );
			style.Selected	( col_selected );
		}/*{
			RC<UIFontStyle>		style = collection.AddFontStyle( "text_green" );
			style.Pipeline	( "ui.draw1" );
			style.Disabled	( col_disabled );
			style.Enabled	( col_enabled );
			style.MouseOver	( col_mouse_over );
			style.TouchDown	( col_touch_down );
			style.Selected	( col_selected );
			style.Font		( "ui_res_meta", "ui_font" );
		}*/
	}

	collection.Store( "ui-style" );
}
