// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package AE.CICD;

import android.app.Application;

public final class CICDApplication
		extends Application
{
	static {
		System.loadLibrary( "CICD" );
	}
}
