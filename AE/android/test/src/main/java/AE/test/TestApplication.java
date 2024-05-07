// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.test;


public final class TestApplication
		extends AE.engine.BaseApplication
{
	static {
		System.loadLibrary( "TestLauncher" );
	}
}
