// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>
#include "style1.as"


void  Widget ()
{
	RC<StackLayout>		lt_1	= StackLayout();	// not implemented
	RC<PaddingLayout>	lt_2	= PaddingLayout( ELayoutType::PaddingLayoutPx );
	RC<PaddingLayout>	lt_3	= PaddingLayout( ELayoutType::PaddingLayoutPx );

	lt_1.AddChild( lt_2 );
	lt_1.AddChild( lt_3 );

	lt_1.Origin( EStackOrigin::Left );
	lt_2.Padding( 10.f );  // px
	lt_3.Padding( 10.f );  // px

	RC<UIWidget>	widget = UIWidget();
	widget.Initialize( lt_1 );
	widget.Store( "ui-editor.widget" );
}


void ASmain ()
{
	Style();
	Widget();
}
