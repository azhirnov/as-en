// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#ifndef AE_DISABLE_THREADS
namespace
{
	static void  LfLinearAllocator_Test1 ()
	{
		static constexpr uint	MaxThreads	= 4;
		static constexpr uint	ElemSize	= 8;

		StaticArray< StdThread, MaxThreads >	worker_thread;
		StaticArray< Array<void*>, MaxThreads >	thread_data;
		LfLinearAllocator< usize{2_Mb} >		alloc;

		for (uint i = 0; i < MaxThreads; ++i)
		{
			thread_data[i].reserve( 1024 );

			worker_thread[i] = StdThread{
				[&alloc, data = &thread_data[i]] ()
				{
					for (;;)
					{
						//ThreadUtils::Sleep_1us();

						void*	ptr = alloc.Allocate( SizeAndAlign{ Bytes{ElemSize}, 8_b });
						if ( ptr == null )
							return;

						std::memset( ptr, ubyte(data->size()), ElemSize );
						data->push_back( ptr );
					}
				}
			};
		}

		for (uint i = 0; i < MaxThreads; ++i) {
			worker_thread[i].join();
		}

		for (auto& items : thread_data)
		{
			for (usize i = 0; i < items.size(); ++i)
			{
				StaticArray< ubyte, ElemSize >	ref;
				ref.fill( ubyte(i) );

				TEST( MemEqual( ref.data(), items[i], Bytes{ElemSize} ));
			}
		}
	}
}


extern void UnitTest_LfLinearAllocator ()
{
	LfLinearAllocator_Test1();

	TEST_PASSED();
}

#else

extern void UnitTest_LfLinearAllocator ()
{}

#endif
