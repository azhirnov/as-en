// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package ae.test;


public final class TestApplication
		extends AE.engine.BaseApplication
{
	static {
		System.loadLibrary( "TestLauncher" );
	}
}
