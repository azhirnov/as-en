// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>
#include "style1.as"


void  Widget ()
{
	RC<PaddingLayout>	lt_0	= PaddingLayout( ELayoutType::PaddingLayoutPx );

	lt_0.Padding( 10.f );  // px
	lt_0.SetDrawable( RectangleDrawable( "col_red" ));


	RC<UIWidget>	widget = UIWidget();
	widget.Initialize( lt_0 );
	widget.Store( "ui-editor.widget" );
}


void ASmain ()
{
	Style();
	Widget();
}
