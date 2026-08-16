// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  CoopMatrixConfig_Test1 ()
	{
		for (uint i = 0; i < uint(ECoopMatrixCfg::_Count); ++i)
		{
			const auto	e0	= ECoopMatrixCfg(i);
			const auto	cfg	= CoopMatrixConfig{e0};
			const auto	e1	= cfg.ToECoopMatrixCfg();

			TEST( e0 == e1 );
		}
	}


	static void  CoopVectorConfig_Test1 ()
	{
		for (uint i = 0; i < uint(ECoopVecCfg::_Count); ++i)
		{
			const auto	e0	= ECoopVecCfg(i);
			const auto	cfg	= CoopVectorConfig{e0};
			const auto	e1	= cfg.ToECoopVecCfg();

			TEST( e0 == e1 );
		}
	}
}


extern void UnitTest_CoopMatrixConfig ()
{
	CoopMatrixConfig_Test1();

	CoopVectorConfig_Test1();

	TEST_PASSED();
}
