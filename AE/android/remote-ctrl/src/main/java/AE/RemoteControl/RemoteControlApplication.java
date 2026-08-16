// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package AE.RemoteControl;


import android.app.Application;

public final class RemoteControlApplication
				extends Application
{
	static {
		System.loadLibrary( "AndroidRemoteControl" );
	}
}
