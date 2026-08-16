// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package AE.RemoteControl;

import java.util.HashMap;

import android.content.Intent;
import android.content.Context;
import android.content.pm.ServiceInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Build;
import android.util.Log;
import android.app.Activity;
import android.view.View;
import android.widget.Button;
import android.Manifest;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.appcompat.app.AppCompatActivity;


public final class RemoteControlActivity
				extends AppCompatActivity
{
	public static final String	TAG = "<<<< AE >>>>";

	public static final int		REQUEST_CAMERA			= ServiceInfo.FOREGROUND_SERVICE_TYPE_CAMERA;
	public static final int		REQUEST_LOCATION		= ServiceInfo.FOREGROUND_SERVICE_TYPE_LOCATION;
	public static final int		REQUEST_MICROPHONE		= ServiceInfo.FOREGROUND_SERVICE_TYPE_MICROPHONE;
	public static final int		REQUEST_NOTIFICATION	= 0;

	HashMap<Integer, Boolean>	permissionMap		= new HashMap<>();

	Button	btnStartService, btnStopService;
	//-----------------------------------------------------


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

		// no runtime permission needed
		permissionMap.put( ServiceInfo.FOREGROUND_SERVICE_TYPE_MEDIA_PLAYBACK,	true );
		permissionMap.put( ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC,		true );
	}

	@Override protected void  onDestroy ()
	{
		//Log.i( TAG, "onDestroy" );
		super.onDestroy();
	}

	@Override protected void  onPause ()
	{
		//Log.i( TAG, "onPause" );
		super.onPause();
	}

	@Override protected void  onResume ()
	{
		//Log.i( TAG, "onResume" );
		super.onResume();
		RequestPermissions();
	}

	@Override protected void  onStart ()
	{
		//Log.i( TAG, "onStart" );
		super.onStart();
	}

	@Override protected void  onStop ()
	{
		//Log.i( TAG, "onStop" );
		super.onStop();
	}

	@Override public void  onBackPressed ()
	{
		super.onBackPressed();
		this.finish();
	}


	private void  startService ()
	{
		Intent serviceIntent = new Intent( this, RemoteControlService.class );
		if ( Build.VERSION.SDK_INT >= 30 )
		{
			int	type = 0;
			for (HashMap.Entry<Integer, Boolean> entry : permissionMap.entrySet())
			{
				if ( entry.getValue() )
					type |= entry.getKey();
			}
			serviceIntent.putExtra( "serviceType", type );
		}
		ContextCompat.startForegroundService( this, serviceIntent );
	}

	private void  stopService ()
	{
		Intent serviceIntent = new Intent( this, RemoteControlService.class );
		stopService( serviceIntent );
	}
	//-----------------------------------------------------


	private boolean  CheckForPermission (String permission, int request)
	{
		if ( ContextCompat.checkSelfPermission( this, permission ) == PackageManager.PERMISSION_GRANTED ) {
			permissionMap.put( request, true );
			return true;
		}

		permissionMap.put( request, false );

		if ( ActivityCompat.shouldShowRequestPermissionRationale( this, permission ))
			return false;

		ActivityCompat.requestPermissions( this, new String[]{ permission }, request );
		return false;
	}

	private void  RequestPermissions ()
	{
		// show 1 request per call
		if (! CheckForPermission( Manifest.permission.CAMERA,				REQUEST_CAMERA ))		return;
		if (! CheckForPermission( Manifest.permission.ACCESS_FINE_LOCATION,	REQUEST_LOCATION ))		return;
		if (! CheckForPermission( Manifest.permission.RECORD_AUDIO,			REQUEST_MICROPHONE ))	return;
		if (! CheckForPermission( Manifest.permission.POST_NOTIFICATIONS,	REQUEST_NOTIFICATION ))	return;
	}

	@Override public void  onRequestPermissionsResult (int requestCode, String[] permissions, int[] grantResults)
	{
		if ( grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED )
		{
			Log.i( TAG, "PERMISSION_GRANTED: " + permissions[0] );
			if ( requestCode > 0 )
				permissionMap.put( requestCode, true );

			// request next permission
			RequestPermissions();
		}
	}
}
