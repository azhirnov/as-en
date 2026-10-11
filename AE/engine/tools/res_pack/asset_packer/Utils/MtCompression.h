// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

/*
=================================================
	MtBlockCompression
=================================================
*/
	template <typename InitThreadStateFn, typename BlockFn>
	ND_ bool  MtBlockCompression (const InitThreadStateFn &initFn, const BlockFn &fn, const uint2 blockCount, const uint threadCount)
	{
		// multithreading
		if ( threadCount > 1 )
		{
			Atomic<uint>		block_y		{0};
			Atomic<uint>		error_cnt	{0};
			Array<StdThread>	thread_arr;
			thread_arr.resize( threadCount - 1 );

			const auto	thread_fn = [&initFn, &fn, &error_cnt, &block_y, blockCount] ()
			{{
				auto	state = initFn();
				for (;;)
				{
					uint	y = block_y.fetch_add( 1 );
					if ( y >= blockCount.y )
						return;

					bool	ok = true;
					for (uint x = 0; x < blockCount.x and ok; ++x) {
						ok = fn( state, x, y );
					}
					if ( not ok )
					{
						++error_cnt;
						return;
					}
				}
			}};

			// submit
			for (auto& t : thread_arr) {
				t = StdThread{ thread_fn };
			}

			thread_fn();

			// wait
			for (auto& t : thread_arr) {
				t.join();
			}
			return error_cnt.load() == 0;
		}
		else
		// single thread
		{
			auto	state	= initFn();
			bool	ok		= true;

			for (uint y = 0; y < blockCount.y; ++y)
			for (uint x = 0; x < blockCount.x; ++x)
			{
				ok &= fn( state, x, y );
			}
			return ok;
		}
	}

/*
=================================================
	MtPartCompression
=================================================
*/
	template <typename Fn>
	ND_ bool  MtPartCompression (const Fn &fn, const uint threadCount)
	{
		// multithreading
		if ( threadCount > 0 )
		{
			Atomic<uint>		error_cnt	{0};
			Array<StdThread>	thread_arr;
			thread_arr.resize( threadCount );

			// submit
			for (usize i = 0; i < thread_arr.size(); ++i)
			{
				thread_arr[i] = StdThread{ [&fn, &error_cnt, j = i] ()
								{
									if ( not fn( uint(j) ))
										++error_cnt;
								}};
			}

			if ( not fn( uint(thread_arr.size()) ))
				++error_cnt;

			// wait
			for (auto& t : thread_arr) {
				t.join();
			}
			return error_cnt.load() == 0;
		}
		else
		// single thread
		{
			return fn( 0u );
		}
	}
