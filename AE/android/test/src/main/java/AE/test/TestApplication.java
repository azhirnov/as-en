// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.test;


import android.app.Application;

public final class TestApplication
        extends AE.engine.BaseApplication
{
    static {
        System.loadLibrary("TestLauncher" );
    }
}
