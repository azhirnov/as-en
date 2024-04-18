// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/BufferMemView.h"
#include "UnitTest_Common.h"

namespace
{
	static void  BufferMemView_Test1 ()
	{
		BufferMemView	view_a;
		BufferMemView	view_b;
		Array<uint>		data_a;
		Array<uint>		data_b;

		const Bytes		es		= SizeOf<uint>;
		const uint		count	= 10'000;

		for (uint i = 0; i < count; ++i)
		{
			data_a.push_back(i);
			data_b.push_back(i);
		}

		view_a.PushBack( data_a.data(), ArraySizeOf(data_a) );
		TEST( view_a.DataSize() == ArraySizeOf(data_a) );

		view_b.PushBack( &data_b[count/2], es * count/2 );
		TEST( view_b.DataSize() == ArraySizeOf(data_b)/2 );

		Bytes	d1 = view_a.Compare( view_b );
		TEST( d1 == view_a.DataSize() - view_b.DataSize() );

		view_a.Clear();
		view_a.PushBack( &data_a[0],     es * 4'000 );
		view_a.PushBack( &data_a[4'000], es * (count - 4'000) );
		TEST( view_a.DataSize() == ArraySizeOf(data_a) );

		auto	view_c = view_a.Section( es * 3'000, es * 2'000 );
		TEST( view_c.DataSize() == (es * 2'000) );

		view_b.Clear();
		view_b.PushBack( &data_b[2'000], es * 1'000 );
		view_b.PushBack( &data_b[3'000], es * 2'000 );
		view_b.PushBack( &data_b[5'000], es * 3'000 );
		TEST( view_b.DataSize() == es * 6'000 );

		Bytes	d2 = view_a.Compare( view_b );
		TEST( d2 == view_a.DataSize() - view_b.DataSize() );

		view_a = view_a.Section( es * 2'000, es * 6'000 );
		TEST( view_a.DataSize() == view_b.DataSize() );

		Bytes	d3 = view_a.Compare( view_b );
		TEST( d3 == 0_b );
	}


	static void  BufferMemView_Test2 ()
	{
		Array<uint>		data_a;
		Array<uint>		data_b;
		const uint		count	= 10'000;

		for (uint i = 0; i < count; ++i)
		{
			data_a.push_back(0);
			data_b.push_back(i);
		}

		BufferMemView	view_a;
		BufferMemView	view_b;
		view_a.PushBack( data_a.data(), ArraySizeOf(data_a) );
		view_b.PushBack( data_b.data(), ArraySizeOf(data_b) );

		TEST( data_a != data_b );

		Bytes	c = view_a.CopyFrom( view_b );
		TEST( c == view_a.DataSize() );

		TEST( data_a == data_b );
	}


	static void  BufferMemView_Test3 ()
	{
		Array<uint>		data_a;
		const uint		count	= 10'000;

		for (uint i = 0; i < count; ++i) {
			data_a.push_back( i );
		}

		BufferMemView	view_a;
		view_a.PushBack( data_a.data(), ArraySizeOf(data_a) );

		auto	r0 = view_a.GetRange( 42'000_b, 16_b );
		TEST( r0.Empty() );

		auto	r1 = view_a.GetRange( 400_b, 4_b );
		TEST( r1.size == 4_b );
		TEST_Eq( *Cast<uint>(r1.ptr), 100 );
	}
}


extern void UnitTest_BufferMemView ()
{
	BufferMemView_Test1();
	BufferMemView_Test2();
	BufferMemView_Test3();

	TEST_PASSED();
}
