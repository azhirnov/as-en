// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/StructView.h"
#include "UnitTest_Common.h"


namespace
{
	static void  StructView_Test1 ()
	{
		const int		data[] = { 0, 1, 2, 3, 4, 5 };
		StructView<int>	view{ data };

		TEST( CountOf( data ) == view.size() );

		for (usize i = 0; i < view.size(); ++i) {
			TEST( data[i] == view[i] );
		}
	}


	static void  StructView_Test2 ()
	{
		#pragma pack (push, 1)
		struct Data {
			int		field1;
			double	field2;
			ubyte	field3;
		};
		#pragma pack (pop)

		const Data	data[] = {
			{ 1, 1.111, 11 },
			{ 2, 2.222, 22 },
			{ 3, 3.333, 33 }
		};

		StructView<double>	view{ ArrayView<Data>{data}, &Data::field2 };
	
		TEST( CountOf( data ) == view.size() );

		for (usize i = 0; i < view.size(); ++i) {
			TEST( data[i].field2 == view[i] );
		}
	}


	static void  StructView_Test3 ()
	{
		struct Data {
			int		field1;
			double	field2;
			ubyte	field3;
		};

		const Data	data[] = {
			{ 1, 1.111, 11 },
			{ 2, 2.222, 22 },
			{ 3, 3.333, 33 }
		};

		StructView<double>	view{ ArrayView<Data>{data}, &Data::field2 };
	
		TEST( CountOf( data ) == view.size() );

		for (usize i = 0; i < view.size(); ++i) {
			TEST( data[i].field2 == view[i] );
		}
	}


	static void  StructViewTransform_Test1 ()
	{
		struct Data {
			int		field1;
			double	field2;
			ubyte	field3;
		};
		const Data	data[] = {
			{ BitCast<int>(1.111f), 1.111, 11 },
			{ BitCast<int>(2.222f), 2.222, 22 },
			{ BitCast<int>(3.333f), 3.333, 33 }
		};

		StructView<int>		view1{ ArrayView<Data>{data}, &Data::field1 };
		TEST( CountOf( data ) == view1.size() );

		StructViewTransform< int, float, StructViewTransform_BitCastConverter<int, float>>	view2{ view1 };
		TEST( CountOf( data ) == view2.size() );

		for (usize i = 0; i < view2.size(); ++i)
		{
			double	d = double(view2[i]);
			TEST( Equals( data[i].field2, d, 0.0001 ));
		}
	}
}


extern void UnitTest_StructView ()
{
	StructView_Test1();
	
	#if not (defined(AE_PLATFORM_ANDROID) or defined(AE_PLATFORM_EMSCRIPTEN))
		StructView_Test2();
	#endif

	StructView_Test3();

	StructViewTransform_Test1();

	TEST_PASSED();
}
