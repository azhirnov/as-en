// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ecs-st/Core/EntityPool.h"
#include "UnitTest_Common.h"

namespace
{
	static void  EntityPool_Test1 ()
	{
		constexpr uint		count = 1024;
		EntityPool			pool;
		Array<EntityID>		ids;

		for (uint j = 0; j < 16; ++j)
		{
			for (usize i = 0; i < count; ++i)
			{
				EntityID	id;
				TEST( pool.Assign( OUT id ));
				TEST( pool.IsValid( id ));

				ids.push_back( id );
			}

			for (auto& id : ids)
			{
				TEST( pool.Unassign( id ));
			}

			ids.clear();
		}
	}
}


extern void UnitTest_EntityPool ()
{
	EntityPool_Test1();

	TEST_PASSED();
}
