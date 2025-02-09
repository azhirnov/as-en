// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>
#include "style1.as"


void  Widget ()
{
	RC<FixedLayout>		lt_0	= FixedLayout( ELayoutType::FixedLayoutPx );

	lt_0.Region( RectF( 0.f, 0.f, 500.f, 500.f ));  // px
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
