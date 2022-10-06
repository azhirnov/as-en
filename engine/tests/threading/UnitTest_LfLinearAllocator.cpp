// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Memory/LfLinearAllocator.h"
#include "base/Containers/FixedTupleArray.h"
#include "UnitTest_Common.h"

namespace
{
	static void  LfLinearAllocator_Test1 ()
	{
		static constexpr uint	MaxThreads	= 8;
		static constexpr uint	ElemSize	= 8;

		FixedArray< std::thread, MaxThreads >	worker_thread;
		StaticArray< Array<void*>, MaxThreads >	thread_data;
		LfLinearAllocator< 4<<10 >				alloc;

		for (uint i = 0; i < MaxThreads; ++i)
		{
			worker_thread.emplace_back( std::thread{
				[&alloc, data = &thread_data[i]] ()
				{
					for (;;)
					{
						ThreadUtils::Yield();

						void*	ptr = alloc.Allocate( Bytes{ElemSize}, 8_b );
						if ( ptr == null )
							return;

						std::memset( ptr, ubyte(data->size()), ElemSize );
						data->push_back( ptr );
					}
				}
			});
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

				TEST( std::memcmp( ref.data(), items[i], ElemSize ) == 0 );
			}
		}
	}
}


extern void UnitTest_LfLinearAllocator ()
{
	LfLinearAllocator_Test1();

	TEST_PASSED();
}
