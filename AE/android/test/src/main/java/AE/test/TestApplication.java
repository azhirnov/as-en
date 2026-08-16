// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package ae.test;


public final class TestApplication
		extends AE.engine.BaseApplication
{
	static {
		System.loadLibrary( "TestLauncher" );
	}
}
