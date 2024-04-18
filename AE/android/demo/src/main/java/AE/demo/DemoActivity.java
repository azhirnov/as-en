// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.demo;

import android.os.Bundle;
import android.util.Log;
import android.content.pm.PackageManager;
import android.Manifest;

public final class DemoActivity
		extends AE.engine.BaseActivity
{
	private final int	ReqCode = 0xAE;

	@Override protected void onCreate(Bundle icicle) {
		super.onCreate(icicle);
	}

	@Override protected void onDestroy() {
		Log.i(TAG, "onDestroy");
		super.onDestroy();
	}

	@Override protected void onPause() {
		Log.i(TAG, "onPause");
		super.onPause();
	}

	@Override protected void onResume() {
		Log.i(TAG, "onResume");
		super.onResume();

		if ( checkSelfPermission( Manifest.permission.CAMERA ) == PackageManager.PERMISSION_GRANTED ) {
			AE.engine.BaseApplication.native_EnableCamera();
		}else{
			//if ( shouldShowRequestPermissionRationale( Manifest.permission.CAMERA ))
			requestPermissions( new String[]{ Manifest.permission.CAMERA }, ReqCode );
		}
	}

	@Override protected void onStart() {
		Log.i(TAG, "onStart");
		super.onStart();
	}

	@Override protected void onStop() {
		Log.i(TAG, "onStop");
		super.onStop();
	}

	@Override public void onBackPressed() {
		super.onBackPressed();
		this.finish();
	}

	@Override public void  onRequestPermissionsResult (int requestCode, String[] permissions, int[] grantResults)
	{
		super.onRequestPermissionsResult(requestCode, permissions, grantResults);
		if ( grantResults.length > 0 &&
			 grantResults[0] == PackageManager.PERMISSION_GRANTED &&
			 permissions[0].equals( Manifest.permission.CAMERA ) &&
			 requestCode == ReqCode )
		{
			AE.engine.BaseApplication.native_EnableCamera();
		}
	}
}
