// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.CICD;

import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import androidx.core.content.ContextCompat;
import androidx.appcompat.app.AppCompatActivity;


public final class CICDActivity
				extends AppCompatActivity
{
	public static final String	TAG = "<<<< CICD >>>>";

	Button 	btnStartService, btnStopService;


	@Override protected void  onCreate (Bundle icicle)
	{
		super.onCreate( icicle );

		setContentView( R.layout.activity_main );
		btnStartService	= findViewById( R.id.buttonStartService );
		btnStopService	= findViewById( R.id.buttonStopService );

		btnStartService.setOnClickListener( new View.OnClickListener()
		{
			@Override public void onClick (View v) {
				startService();
			}
		});
		btnStopService.setOnClickListener( new View.OnClickListener()
		{
			@Override public void onClick (View v) {
				stopService();
			}
		});
	}

	@Override protected void  onDestroy ()
	{
		//Log.i(TAG, "onDestroy");
		super.onDestroy();
	}

	@Override protected void  onPause ()
	{
		//Log.i(TAG, "onPause");
		super.onPause();
	}

	@Override protected void  onResume ()
	{
		//Log.i(TAG, "onResume");
		super.onResume();
	}

	@Override protected void  onStart ()
	{
		//Log.i(TAG, "onStart");
		super.onStart();
	}

	@Override protected void  onStop ()
	{
		//Log.i(TAG, "onStop");
		super.onStop();
	}

	@Override public void  onBackPressed ()
	{
		super.onBackPressed();
		this.finish();
	}


	private void  startService ()
	{
		Intent serviceIntent = new Intent( this, CICDService.class );
		if ( Build.VERSION.SDK_INT >= 30 )
		{
			serviceIntent.putExtra( "serviceType", ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC );
		}
		ContextCompat.startForegroundService( this, serviceIntent );
	}

	private void  stopService ()
	{
		Intent serviceIntent = new Intent( this, CICDService.class );
		stopService( serviceIntent );
	}
}
