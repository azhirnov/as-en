// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.CICD;

import java.io.File;
import java.lang.Thread;
import java.util.concurrent.atomic.AtomicBoolean;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.core.app.NotificationCompat;


public final class CICDService
				extends Service
{
	public static final String	TAG = "<<<< CICD >>>>";

	//-----------------------------------------------------
	private static native void  native_Start (String workDir, String name);
	private static native void  native_Update ();
	private static native void  native_Stop ();
	//-----------------------------------------------------


	private static final class MyThread
							extends Thread
	{
		private AtomicBoolean	_looping	= new AtomicBoolean(false);
		private String			_workDir	= "";

		@Override public void  run ()
		{
			native_Start( _workDir, Build.MANUFACTURER+" "+Build.MODEL );

			for (; _looping.get();) {
				native_Update();
			}
			native_Stop();
		}

		public void  Begin (Context ctx)
		{
			File internalCache = ctx.getCacheDir();
			_workDir = (internalCache == null ? "" : internalCache.getAbsolutePath());

			this.setName( "CICD" );
			_looping.set( true );
			this.start();
		}

		public void  End ()
		{
			_looping.set( false );
			try {
				this.join( 200 );
			} catch (Exception e)
			{}
		}
	}
	private MyThread  _backgroundThread = null;
	//-----------------------------------------------------


	@Override public void  onCreate ()
	{
		super.onCreate();
	}

	@Override public void  onDestroy ()
	{
		Log.i( TAG, "Service.onDestroy" );

		if ( _backgroundThread != null )
		{
			_backgroundThread.End();
			_backgroundThread = null;
		}
		super.onDestroy();
	}

	@Override public int  onStartCommand (Intent intent, int flags, int startId)
	{
		Log.i( TAG, "Service.onStartCommand" );

		if ( _backgroundThread != null )
		{
			_backgroundThread.End();
			_backgroundThread = null;
		}

		try {
			Notification	notification = CreateNotification();
			int				type		 = intent.getIntExtra( "serviceType", 0 );

			startForeground(
					/* id = */ 100, // Cannot be 0
					notification,
					type
				);

			_backgroundThread = new MyThread();
			_backgroundThread.Begin( this );
		}
		catch (Exception e)
		{
			Log.e( TAG, "Service.onStartCommand: failed" );
		}
		return START_NOT_STICKY;
	}

	@Override
	public IBinder  onBind (Intent intent) {
		return null;
	}


	private Notification  CreateNotification ()
	{
		String	channel_id;
		if ( Build.VERSION.SDK_INT >= 26 )
			channel_id = CreateChannel();
		else
			channel_id = "";

		NotificationCompat.Builder builder = new NotificationCompat.Builder( this, channel_id )
												.setSmallIcon( android.R.drawable.ic_menu_mylocation )
												.setContentTitle( "CICD" );

		if ( Build.VERSION.SDK_INT >= 31 ) {
			builder.setForegroundServiceBehavior( Notification.FOREGROUND_SERVICE_IMMEDIATE );
		}else{
			builder.setPriority( Notification.PRIORITY_DEFAULT );
		}

		return builder
				.setCategory( Notification.CATEGORY_SERVICE )
				.build();
	}

	private synchronized String  CreateChannel ()
	{
		NotificationManager notification_mngr = (NotificationManager) this.getSystemService( Context.NOTIFICATION_SERVICE );

		if (! notification_mngr.areNotificationsEnabled() )
			Log.e( TAG, "notifications are disabled" );

		String	channel_id	= "CICD.channel";
		int		importance	= NotificationManager.IMPORTANCE_DEFAULT;	// IMPORTANCE_LOW

		NotificationChannel channel = new NotificationChannel( channel_id, "name", importance );

		channel.enableLights( true );
		channel.setLightColor( Color.BLUE );

		channel.enableVibration( true );

		channel.setLockscreenVisibility( Notification.VISIBILITY_PUBLIC );
		channel.setShowBadge( true );

		channel.setDescription( "CICD desc" );

		if ( notification_mngr != null )
			notification_mngr.createNotificationChannel( channel );
		else
			stopSelf();

		return channel_id;
	}
}
