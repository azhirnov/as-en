// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.RmG.Device;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.view.View;

public final class RmGDActivity
		extends AE.engine.BaseActivity
{
	@Override protected void  onCreate (Bundle icicle)
	{
		super.onCreate(icicle);
	}

	@Override protected void  onDestroy () {
		Log.i(TAG, "onDestroy");
		super.onDestroy();
	}

	@Override protected void  onPause () {
		Log.i(TAG, "onPause");
		super.onPause();
	}

	@Override protected void  onResume () {
		Log.i(TAG, "onResume");
		super.onResume();
	}

	@Override protected void  onStart () {
		Log.i(TAG, "onStart");
		super.onStart();
	}

	@Override protected void  onStop () {
		Log.i(TAG, "onStop");
		super.onStop();
	}

	@Override public void  onBackPressed () {
		super.onBackPressed();
		this.finish();
		System.exit(0);
	}
}
