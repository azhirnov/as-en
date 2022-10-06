// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Stream/StdStream.h"
#include "base/Stream/MemStream.h"
#include "base/Stream/BrotliStream.h"
#include "base/Stream/BufferedStream.h"
#include "base/Stream/FastStream.h"
#include "UnitTest_Common.h"


namespace
{
#ifdef AE_ENABLE_BROTLI
	static void  BrotliStream_Test1 ()
	{
		const String	str1 = "12i12ienmqpwom12euj1029podmksjhjbjcnalsmoiiwujkcmsalsc,posaasjncsalkmxaz";

		Array<ubyte>	file_data;

		// compress
		{
			auto			stream = MakeRC<MemWStream>();
			BrotliWStream	encoder{ stream };

			TEST( encoder.IsOpen() );
			TEST( encoder.Write( str1 ));
			encoder.Flush();

			file_data.assign( stream->GetData().begin(), stream->GetData().end() );
		}

		// uncompress
		{
			BrotliRStream	decoder{ MakeRC<MemRStream>( RVRef(file_data) )};
			String			str2, str3;

			TEST( decoder.IsOpen() );
			TEST( decoder.Read( str1.length() / 2, OUT str2 ));
			TEST( decoder.Read( str1.length() - str2.length(), OUT str3 ));

			TEST( str1 == (str2 + str3) );
		}
	}
#endif	// AE_ENABLE_BROTLI


	static void  StdStream_Test1 ()
	{
		String			src_str = "4324356uytsdgfh1243rttrasfdg";
		RC<RStream>		mem = MakeRC<MemRefRStream>( src_str );

		StreambufWrap<char>	streambuf{ mem };
		std::istream		stream{ &streambuf };

		String				dst_str;
		dst_str.resize( src_str.length() );

		stream.read( dst_str.data(), dst_str.length() );
		TEST( src_str == dst_str );

		// read again
		dst_str.clear();
		dst_str.resize( src_str.length() );

		stream.seekg( 0, std::ios_base::beg );
		stream.read( dst_str.data(), dst_str.length() );
		TEST( src_str == dst_str );
	}
	

	static void  BufferedStream_Test1 ()
	{
		const uint		count	= 10'000;
		RC<MemWStream>	stream1	= MakeRC<MemWStream>();
		RC<WStream>		stream2	= MakeRC<BufferedWStream>( stream1, 128_b );

		for (uint i = 0; i < count; ++i)
			stream2->Write( i );

		stream2->Flush();
		ArrayView<uint>	mem = stream1->GetData().Cast<uint>();

		for (uint i = 0; i < count; ++i)
			TEST( mem[i] == i );

		RC<MemRefRStream>	stream3 = MakeRC<MemRefRStream>( mem );
		RC<RStream>			stream4 = MakeRC<BufferedRStream>( stream3 );
		
		for (uint i = 0; i < count; ++i)
		{
			uint	j = 0;
			stream4->Read( OUT j );
			TEST( i == j );
		}
	}


	static void  FastStream_Test1 ()
	{
		const uint		count	= 10'000;
		RC<MemWStream>	stream1	= MakeRC<MemWStream>();
		
		for (uint i = 0; i < count; ++i)
			stream1->Write( i );
		
		RC<MemRefRStream>	stream2	= stream1->ToRStream();

		const void*	begin1	= null;
		const void*	end1	= null;
		stream2->UpdateFastStream( OUT begin1, OUT end1 );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 4_b * count) == end1 );

		for (uint i = 0; i < count; ++i)
			TEST( Cast<uint>(begin1)[i] == i );
		
		const void*	begin2	= begin1 + 8_b;
		const void*	end2	= end1;
		stream2->UpdateFastStream( OUT begin2, OUT end2 );
		TEST( begin2 != null and end2 != null );
		TEST( begin1 + 8_b == begin2 );
		TEST( end1 == end2 );
		TEST( (begin2 + 4_b * count - 8_b) == end2 );
		
		for (uint i = 0; i < count-2; ++i)
			TEST( Cast<uint>(begin2)[i] == i+2 );
	}
	

	static void  FastStream_Test2 ()
	{
		const uint		count	= 10'000;
		RC<MemWStream>	stream1	= MakeRC<MemWStream>();
		
		for (uint i = 0; i < count; ++i)
			stream1->Write( i );
		
		RC<MemRefRStream>	stream2	= stream1->ToRStream();
		RC<BufferedRStream>	stream3 = MakeRC<BufferedRStream>( stream2, 128_b );

		const void*	begin1	= null;
		const void*	end1	= null;
		stream3->UpdateFastStream( OUT begin1, OUT end1 );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 128_b) == end1 );
		
		for (uint i = 0; i < 128/4; ++i)
			TEST( Cast<uint>(begin1)[i] == i );

		const void*	begin2	= begin1 + 128_b;
		const void*	end2	= end1;
		stream3->UpdateFastStream( OUT begin2, OUT end2 );
		TEST( begin2 != null and end2 != null );
		TEST( (begin2 + 128_b) == end2 );
		TEST( begin1 == begin2 );
		TEST( end1 == end2 );
		
		for (uint i = 0; i < 128/4; ++i)
			TEST( Cast<uint>(begin2)[i] == (i + 128/4) );
	}
	

	static void  FastStream_Test3 ()
	{
		RC<MemWStream>	stream1	= MakeRC<MemWStream>();
		
		void*		begin1	= null;
		const void*	end1	= null;
		stream1->UpdateFastStream( OUT begin1, OUT end1, 128_b );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 128_b) == end1 );
		
		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin1)[i] = i;

		void*		begin2	= begin1 + 128_b;
		const void*	end2	= end1;
		stream1->UpdateFastStream( OUT begin2, OUT end2, 128_b );
		TEST( begin2 != null and end2 != null );
		TEST( (begin2 + 128_b) == end2 );
		TEST( end1 == begin2 );
		
		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin2)[i] = (i + 128/4);
		
		stream1->EndFastStream( begin2 + 128_b );

		ArrayView<uint>	arr = stream1->GetData().Cast<uint>();
		TEST( arr.size() == (128/4 * 2) );

		for (uint i = 0; i < uint(arr.size()); ++i)
			TEST( arr[i] == i );
	}
	

	static void  FastStream_Test4 ()
	{
		RC<MemWStream>		stream1	= MakeRC<MemWStream>();
		RC<BufferedWStream>	stream2 = MakeRC<BufferedWStream>( stream1, 256_b );
		
		void*		begin1	= null;
		const void*	end1	= null;
		stream2->UpdateFastStream( OUT begin1, OUT end1, 128_b );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 128_b) == end1 );
		
		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin1)[i] = i;

		void*		begin2	= begin1 + 128_b;
		const void*	end2	= end1;
		stream2->UpdateFastStream( OUT begin2, OUT end2, 128_b );
		TEST( begin2 != null and end2 != null );
		TEST( (begin2 + 128_b) == end2 );
		TEST( begin1 == begin2 );
		TEST( end1 == end2 );
		
		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin2)[i] = (i + 128/4);
		
		stream2->EndFastStream( begin2 + 128_b );
		stream2 = null;

		ArrayView<uint>	arr = stream1->GetData().Cast<uint>();
		TEST( arr.size() == (128/4 * 2) );

		for (uint i = 0; i < uint(arr.size()); ++i)
			TEST( arr[i] == i );
	}
}


extern void UnitTest_Stream ()
{
	#ifdef AE_ENABLE_BROTLI
	BrotliStream_Test1();
	#endif

	StdStream_Test1();
	BufferedStream_Test1();

	FastStream_Test1();
	FastStream_Test2();
	FastStream_Test3();
	FastStream_Test4();

	TEST_PASSED();
}
