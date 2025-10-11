// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"
#include "base/DataSource/Lz4Stream.h"

namespace
{
	static void  ReadNonCompressed (OUT Array<char> &buffer)
	{
	#ifdef AE_PLATFORM_ANDROID
		FileRStream		file {"text.dds"};	// put in 'Android/AE.Test/cache'
	#else
		FileRStream		file {R"(path/to/file)"};
	#endif
		TEST( file.IsOpen() );
		TEST( file.Read( file.RemainingSize(), OUT buffer ));
	}


	static void  CompressionTest (StringView coreTypeName)
	{
		Array<char>		buffer;
		ReadNonCompressed( OUT buffer );

		AE_LOGI( "Decompressed size: "s << ToString( ArraySizeOf( buffer )) );

		Array<char>		compressed;
		compressed.resize( buffer.size() );

		const uint		max_iter = 10;
		Array<float>	comp_arr;

		IntervalProfiler	profiler{ "CompressionTest on "s << coreTypeName, IntervalProfiler::EFlags::ExcludeDelta };
		
	  #ifdef AE_ENABLE_BROTLI
		for (float wnd = 0.6f; wnd < 1.1f; wnd += 0.1f)
		for (float qual = 0.2f; qual < 0.9f; qual += 0.2f)
		{
			BrotliWStream::Config	cfg;
			cfg.quality		= qual;
			cfg.windowBits	= wnd;

			String	name = "Brotli, q="s << ToString( qual, 1 ) << ", wnd=" << ToString( wnd, 1 );
			Bytes	comp_size;
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				comp_size = ArraySizeOf(compressed);
				TEST( BrotliUtils::Compress( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));

				profiler.EndIteration();
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
	  #endif
		
	  #ifdef AE_ENABLE_LZ4
		compressed.resize( usize{Lz4Utils::MaxCompressedSize( Bytes{buffer.size()} )} );

		for (float hc = -0.2f; hc < 1.0f; hc += 0.2f)
		{
			Lz4WStream::Config	cfg;
			cfg.hc		= hc >= 0.0f;
			cfg.level	= hc;

			String	name = "LZ4, hc="s << ToString( hc, 1 );
			Bytes	comp_size;
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				comp_size = ArraySizeOf(compressed);
				TEST( Lz4Utils::Compress( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));

				profiler.EndIteration();
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
		
		for (float lvl = 0.0f; lvl < 1.0f; lvl += 0.2f)
		{
			Lz4WStream::Config	cfg;
			cfg.level	= lvl;

			String	name = "LZ4 frame, lvl="s << ToString( lvl, 1 );
			Bytes	comp_size;
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				comp_size = ArraySizeOf(compressed);
				TEST( Lz4Utils::CompressFrame( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));

				profiler.EndIteration();
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
	  #endif
		
	  #ifdef AE_ENABLE_ZSTD
		for (float lvl = 0.f; lvl < 1.0f; lvl += 0.1f)
		{
			ZStdWStream::Config		cfg;
			cfg.level = lvl;

			String	name = "ZStd, lvl="s << ToString( lvl, 1 );
			Bytes	comp_size;
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				comp_size = ArraySizeOf(compressed);
				TEST( ZStdUtils::Compress( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));

				profiler.EndIteration();
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
	  #endif
	}

	
	static void  DecompressionTest (StringView coreTypeName)
	{
		Array<char>		buffer;
		ReadNonCompressed( OUT buffer );

		AE_LOGI( "Decompressed size: "s << ToString( ArraySizeOf( buffer )) );

		Array<char>		compressed;
		compressed.resize( buffer.size() );
		
		Array<char>		decompressed;
		decompressed.resize( buffer.size() );

		const uint		max_iter = 10;
		Array<float>	comp_arr;

		IntervalProfiler	profiler{ "DecompressionTest on "s << coreTypeName, IntervalProfiler::EFlags::ExcludeDelta };
		
	  #ifdef AE_ENABLE_BROTLI
		for (float wnd = 0.6f; wnd < 1.1f; wnd += 0.1f)
		for (float qual = 0.2f; qual < 0.9f; qual += 0.2f)
		{
			BrotliWStream::Config	cfg;
			cfg.quality		= qual;
			cfg.windowBits	= wnd;

			String	name = "Brotli, q="s << ToString( qual, 1 ) << ", wnd=" << ToString( wnd, 1 );

			Bytes	comp_size = ArraySizeOf(compressed);
			TEST( BrotliUtils::Compress( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();

				Bytes	decomp_size = ArraySizeOf(decompressed);
				TEST( BrotliUtils::Decompress( OUT decompressed.data(), INOUT decomp_size, compressed.data(), comp_size ));

				profiler.EndIteration();
				TEST( decomp_size == ArraySizeOf(buffer) );
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
	  #endif
		
	  #ifdef AE_ENABLE_LZ4
		compressed.resize( usize{Lz4Utils::MaxCompressedSize( Bytes{buffer.size()} )} );

		for (float hc = -0.2f; hc < 1.0f; hc += 0.2f)
		{
			Lz4WStream::Config	cfg;
			cfg.hc		= hc >= 0.0f;
			cfg.level	= hc;

			String	name = "LZ4, hc="s << ToString( hc, 1 );
			Bytes	comp_size = ArraySizeOf(compressed);
			TEST( Lz4Utils::Compress( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				Bytes	decomp_size = ArraySizeOf(decompressed);
				TEST( Lz4Utils::Decompress( OUT decompressed.data(), INOUT decomp_size, compressed.data(), comp_size ));

				profiler.EndIteration();
				TEST( decomp_size == ArraySizeOf(buffer) );
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
		
		for (float lvl = 0.0f; lvl < 1.0f; lvl += 0.2f)
		{
			Lz4WStream::Config	cfg;
			cfg.level	= lvl;

			String	name = "LZ4 frame, lvl="s << ToString( lvl, 1 );
			Bytes	comp_size = ArraySizeOf(compressed);
			TEST( Lz4Utils::CompressFrame( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				Bytes	decomp_size = ArraySizeOf(decompressed);
				TEST( Lz4Utils::DecompressFrame( OUT decompressed.data(), INOUT decomp_size, compressed.data(), comp_size ));

				profiler.EndIteration();
				TEST( decomp_size == ArraySizeOf(buffer) );
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
	  #endif
		
	  #ifdef AE_ENABLE_ZSTD
		for (float lvl = 0.f; lvl < 1.0f; lvl += 0.1f)
		{
			ZStdWStream::Config		cfg;
			cfg.level = lvl;

			String	name = "ZStd, lvl="s << ToString( lvl, 1 );
			Bytes	comp_size = ArraySizeOf(compressed);
			TEST( ZStdUtils::Compress( OUT compressed.data(), INOUT comp_size, buffer.data(), ArraySizeOf(buffer), cfg ));
			
			profiler.BeginTest( name, [&comp_arr, i = comp_arr.size()](nanoseconds) { return ToString(comp_arr[i], 1) << '%'; } );
			for (uint i = 0; i < max_iter; ++i)
			{
				profiler.BeginIteration();
				
				Bytes	decomp_size = ArraySizeOf(decompressed);
				TEST( ZStdUtils::Decompress( OUT decompressed.data(), INOUT decomp_size, compressed.data(), comp_size ));

				profiler.EndIteration();
				TEST( decomp_size == ArraySizeOf(buffer) );
			}
			profiler.EndTest();
			comp_arr.push_back(float( 100.0 * double(ulong{comp_size}) / double(buffer.size()) ));
			AE_LOGI( name );
		}
	  #endif
	}
}

extern void PerfTest_Compression ()
{
	ForEachCoreType(
		[&] (auto& core, Function<void()> setAffinity)
		{
			setAffinity();

			StringView core_type = ToString( core.type );

			CompressionTest( core_type );
			DecompressionTest( core_type );
		});

	TEST_PASSED();
}
