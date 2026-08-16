// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <asset_packer.as>
#include "style1.as"


void  Widget ()
{
	RC<AlignedLayout>	lt_0	= AlignedLayout( ELayoutType::AlignedLayoutRel );
	RC<AlignedLayout>	lt_1	= AlignedLayout( ELayoutType::AlignedLayoutMm );
	RC<AlignedLayout>	lt_2	= AlignedLayout( ELayoutType::AlignedLayoutRel );

	lt_0.AddChild( lt_1 );
	lt_0.AddChild( lt_2 );

	lt_0.Align( ELayoutAlign::Fill );

	lt_1.Size( 50.f, 100.f );	// mm
	lt_1.Align( ELayoutAlign::Center );
	lt_1.SetDrawable( ImageDrawable( "btn_red" ));

	lt_2.Size( 0.1f, 0.2f );	// %
	lt_2.Align( ELayoutAlign::Left | ELayoutAlign::CenterY );
	lt_2.SetDrawable( ImageDrawable( "btn_green" ));

	{
		RC<ButtonController>	c = ButtonController();
		c.OnClick( "btn1_click" );
		lt_1.SetController( c );
	}{
		RC<ButtonController>	c = ButtonController();
		c.OnClick( "btn2_click" );
		lt_2.SetController( c );
	}

	RC<UIWidget>	widget = UIWidget();
	widget.Initialize( lt_0 );
	widget.Store( "ui-editor.widget" );
}


void ASmain ()
{
	Style();
	Widget();
}
