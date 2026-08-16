// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package AE.ShaderEditor;


import android.app.Application;

public final class SEApplication
		extends AE.engine.BaseApplication
{
	static {
		System.loadLibrary( "ResourceEditor" );
	}
}
