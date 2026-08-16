// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package AE.demo;


import android.app.Application;

public final class DemoApplication
		extends AE.engine.BaseApplication
{
	static {
		System.loadLibrary( "SampleDemo" );
	}
}
