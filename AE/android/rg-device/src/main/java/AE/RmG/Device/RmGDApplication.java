// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.RmG.Device;


import android.app.Application;

public final class RmGDApplication
		extends AE.engine.BaseApplication
{
	static {
		System.loadLibrary( "RemoteGraphicsDevice" );
	}
}
