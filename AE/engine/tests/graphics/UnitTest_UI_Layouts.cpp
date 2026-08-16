// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "pch/Graphics.h"
#include "../tests/shared/UnitTest_Shared.h"
using namespace AE::UI;

namespace
{
	static const float2						surf_size	{100.f};
	static const float						mm_to_px	{1.0f};
	static const IController::InputState	input;


	ND_ static bool  SerializeAndDeserialize (const Widget &src, OUT Widget &dst)
	{
		Serializing::ObjectFactory	factory;
		CHECK_ERR( ILayout::RegisterLayouts( factory ));
		CHECK_ERR( IDrawable::RegisterDrawables( factory ));
		CHECK_ERR( IController::RegisterControllers( factory ));

		auto	wmem = MakeRC<ArrayWStream>();
		{
			Serializing::Serializer		ser {wmem};
			ser.factory	= &factory;
			CHECK_ERR( src.Serialize( ser ));
		}

		auto	rmem = MakeRC<MemRefRStream>( wmem->GetData() );
		{
			Serializing::Deserializer	des {rmem};
			des.factory	= &factory;
			CHECK_ERR( dst.Deserialize( des ));
		}
		return true;
	}


	template <typename T>
	ND_ static bool  CompareLayouts (const ILayout* lhs, const RC<T> &rhs)
	{
		StaticAssert( IsBaseOf< ILayout, T >);
		CHECK_ERR( lhs != null and rhs != null );
		CHECK_ERR( lhs->Type() == rhs->Type() );
		CHECK_ERR( lhs->GetChilds().size() == rhs->GetChilds().size() );

		auto*	t_lhs = Cast<T>( lhs );

		if constexpr( IsSame< T, FixedLayoutPx > or
					  IsSame< T, FixedLayoutMm >)
		{
			CHECK_ERR( All( t_lhs->Region() == rhs->Region() ));
			return true;
		}

		if constexpr( IsSame< T, PaddingLayoutPx > or
					  IsSame< T, PaddingLayoutMm > or
					  IsSame< T, PaddingLayoutRel >)
		{
			CHECK_ERR( All( t_lhs->PaddingX() == rhs->PaddingX() ));
			CHECK_ERR( All( t_lhs->PaddingY() == rhs->PaddingY() ));
			return true;
		}

		if constexpr( IsSame< T, AlignedLayoutPx > or
					  IsSame< T, AlignedLayoutMm > or
					  IsSame< T, AlignedLayoutRel >)
		{
			CHECK_ERR( All( t_lhs->Size() == rhs->Size() ));
			CHECK_ERR( t_lhs->Align() == rhs->Align() );
			return true;
		}

		if constexpr( IsSame< T, FillStackLayout >)
		{
			CHECK_ERR( t_lhs->Origin() == rhs->Origin() );
			return true;
		}
	}
	//-----------------------------------------------------



	static void  Test_FixedLayout1 ()
	{
		Widget::Allocator_t		alloc;
		Widget::TempAllocator_t	alloc2;
		RC<Widget>				w		= Widget::New( alloc, alloc2 );
		RC<FixedLayoutPx>		lt_0	= w->Create<FixedLayoutPx>();
		RC<FixedLayoutPx>		lt_1	= w->Create<FixedLayoutPx>();
		RC<FixedLayoutPx>		lt_2	= w->Create<FixedLayoutPx>();

		lt_1->AddChild( lt_2 );
		lt_0->AddChild( lt_1 );

		TEST( w->Initialize( lt_0 ));

		auto	action_map = IController::ActionMapBuilder{}.Build();
		w->SetActionBindingsRef( action_map );

		const RectF	region_0 {  0.f,  0.f, 100.f, 100.f };
		const RectF	region_1 {  5.f,  8.f,  25.f,  28.f };
		const RectF	region_2 { 10.f, 20.f,  31.f,  42.f };

		lt_0->SetRegion( region_0 );
		lt_1->SetRegion( region_1 );
		lt_2->SetRegion( region_2 );

		w->Update( surf_size, mm_to_px, input );

		TEST( All( lt_0->GlobalRect() == region_0 ));
		TEST( All( lt_1->GlobalRect() == region_1 + region_0.LeftTop() ));
		TEST( All( lt_2->GlobalRect() == region_2 + region_1.LeftTop() + region_0.LeftTop() ));

		{
			RC<Widget>		w2 = Widget::New( alloc, alloc2 );
			TEST( SerializeAndDeserialize( *w, OUT *w2 ));

			auto*	lt2_0 = w2->GetRoot();
			TEST( CompareLayouts( lt2_0, lt_0 ));

			auto*	lt2_1 = lt2_0->GetChilds()[0].get();
			TEST( CompareLayouts( lt2_1, lt_1 ));

			auto*	lt2_2 = lt2_1->GetChilds()[0].get();
			TEST( CompareLayouts( lt2_2, lt_2 ));
		}
	}


	static void  Test_AlignedLayout1 ()
	{
		Widget::Allocator_t		alloc;
		Widget::TempAllocator_t	alloc2;
		RC<Widget>				w		= Widget::New( alloc, alloc2 );
		RC<FixedLayoutPx>		lt_0	= w->Create<FixedLayoutPx>();
		RC<AlignedLayoutPx>		lt_1	= w->Create<AlignedLayoutPx>();

		lt_0->AddChild( lt_1 );

		TEST( w->Initialize( lt_0 ));

		auto	action_map = IController::ActionMapBuilder{}.Build();
		w->SetActionBindingsRef( action_map );

		const RectF	base_region { 5.f, 9.f, 100.f, 100.f };

		lt_0->SetRegion( base_region );
		lt_1->SetSize({ 20.f, 20.f });	lt_1->SetAlign( ELayoutAlign::Left | ELayoutAlign::Bottom );

		w->Update( surf_size, mm_to_px, input );

		TEST( All( lt_0->GlobalRect() == base_region ));
		TEST( All( lt_1->GlobalRect() == RectF{ 5.f, 9.f, 25.f, 29.f } ));

		{
			RC<Widget>		w2 = Widget::New( alloc, alloc2 );
			TEST( SerializeAndDeserialize( *w, OUT *w2 ));

			auto*	lt2_0 = w2->GetRoot();
			TEST( CompareLayouts( lt2_0, lt_0 ));

			auto*	lt2_1 = lt2_0->GetChilds()[0].get();
			TEST( CompareLayouts( lt2_1, lt_1 ));
		}
	}


	static void  Test_AlignedLayout2 ()
	{
		Widget::Allocator_t		alloc;
		Widget::TempAllocator_t	alloc2;
		RC<Widget>				w		= Widget::New( alloc, alloc2 );
		RC<FixedLayoutPx>		lt_0	= w->Create<FixedLayoutPx>();
		RC<AlignedLayoutPx>		lt_1	= w->Create<AlignedLayoutPx>();

		lt_0->AddChild( lt_1 );

		TEST( w->Initialize( lt_0 ));

		auto	action_map = IController::ActionMapBuilder{}.Build();
		w->SetActionBindingsRef( action_map );

		const RectF	base_region { 0.f, 0.f, 100.f, 100.f };

		lt_0->SetRegion( base_region );
		lt_1->SetSize({ 20.f, 30.f });	lt_1->SetAlign( ELayoutAlign::Right | ELayoutAlign::CenterY );

		w->Update( surf_size, mm_to_px, input );

		TEST( All( lt_0->GlobalRect() == base_region ));
		TEST( All( lt_1->GlobalRect() == RectF{ 80.f, 35.f, 100.f, 65.f } ));

		{
			RC<Widget>		w2 = Widget::New( alloc, alloc2 );
			TEST( SerializeAndDeserialize( *w, OUT *w2 ));

			auto*	lt2_0 = w2->GetRoot();
			TEST( CompareLayouts( lt2_0, lt_0 ));

			auto*	lt2_1 = lt2_0->GetChilds()[0].get();
			TEST( CompareLayouts( lt2_1, lt_1 ));
		}
	}


	static void  Test_PaddingLayout1 ()
	{
		Widget::Allocator_t		alloc;
		Widget::TempAllocator_t	alloc2;
		RC<Widget>				w		= Widget::New( alloc, alloc2 );
		RC<FixedLayoutPx>		lt_0	= w->Create<FixedLayoutPx>();
		RC<PaddingLayoutPx>		lt_1	= w->Create<PaddingLayoutPx>();

		lt_0->AddChild( lt_1 );

		TEST( w->Initialize( lt_0 ));

		auto	action_map = IController::ActionMapBuilder{}.Build();
		w->SetActionBindingsRef( action_map );

		const RectF	base_region { 0.f, 0.f, 100.f, 100.f };

		lt_0->SetRegion( base_region );
		lt_1->SetPaddingX( 1.f, 2.f );
		lt_1->SetPaddingY( 4.f, 5.f );

		w->Update( surf_size, mm_to_px, input );

		TEST( All( lt_0->GlobalRect() == base_region ));
		TEST( All( lt_1->GlobalRect() == RectF{ 1.f, 4.f, 98.f, 95.f } ));

		{
			RC<Widget>		w2 = Widget::New( alloc, alloc2 );
			TEST( SerializeAndDeserialize( *w, OUT *w2 ));

			auto*	lt2_0 = w2->GetRoot();
			TEST( CompareLayouts( lt2_0, lt_0 ));

			auto*	lt2_1 = lt2_0->GetChilds()[0].get();
			TEST( CompareLayouts( lt2_1, lt_1 ));
		}
	}


	static void  Test_FillStackLayout1 ()
	{
		Widget::Allocator_t		alloc;
		Widget::TempAllocator_t	alloc2;
		RC<Widget>				w		= Widget::New( alloc, alloc2 );
		RC<FixedLayoutPx>		lt_0	= w->Create<FixedLayoutPx>();
		RC<FillStackLayout>		lt_1	= w->Create<FillStackLayout>();
		RC<PaddingLayoutPx>		lt_2	= w->Create<PaddingLayoutPx>();
		RC<PaddingLayoutPx>		lt_3	= w->Create<PaddingLayoutPx>();

		lt_1->AddChild( lt_2 );
		lt_1->AddChild( lt_3 );
		lt_0->AddChild( lt_1 );

		TEST( w->Initialize( lt_0 ));

		auto	action_map = IController::ActionMapBuilder{}.Build();
		w->SetActionBindingsRef( action_map );

		const RectF	base_region { 0.f, 0.f, 100.f, 100.f };

		lt_0->SetRegion( base_region );
		lt_1->SetOrigin( EStackOrigin::Left );
		lt_2->SetPadding( 1.f );
		lt_3->SetPadding( 2.f );

		w->Update( surf_size, mm_to_px, input );

		TEST( All( lt_0->GlobalRect() == base_region ));
		TEST( All( lt_1->GlobalRect() == base_region ));
		TEST( All( lt_2->GlobalRect() == RectF{ 1.f, 1.f, 49.f, 99.f } ));
		TEST( All( lt_3->GlobalRect() == RectF{ 52.f, 2.f, 98.f, 98.f } ));

		{
			RC<Widget>		w2 = Widget::New( alloc, alloc2 );
			TEST( SerializeAndDeserialize( *w, OUT *w2 ));

			auto*	lt2_0 = w2->GetRoot();
			TEST( CompareLayouts( lt2_0, lt_0 ));

			auto*	lt2_1 = lt2_0->GetChilds()[0].get();
			TEST( CompareLayouts( lt2_1, lt_1 ));

			// TODO: lt_2, lt_3
		}
	}
}


extern void UnitTest_UI_Layouts ()
{
	Test_FixedLayout1();
	Test_AlignedLayout1();
	Test_AlignedLayout2();
	Test_PaddingLayout1();
	Test_FillStackLayout1();

	TEST_PASSED();
}
