// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

package AE.RemoteControl;

import java.util.List;
import java.lang.Thread;
import java.util.concurrent.atomic.AtomicBoolean;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;

import android.content.Intent;
import android.content.IntentFilter;
import android.content.Context;
import android.content.pm.ServiceInfo;
import android.content.BroadcastReceiver;

import android.os.Bundle;
import android.os.IBinder;
import android.os.Build;
import android.os.BatteryManager;

import android.util.Log;

import android.graphics.Color;
import android.graphics.Camera;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;

import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;


public final class RemoteControlService
				extends Service
{
	private static final String	TAG = "<<<< AE >>>>";

	//-----------------------------------------------------
	private static native void  native_Start (Object thread);
	private static native void  native_SendText (String txt);
	private static native void  native_SendSensor (int sensor, float[] values);
	private static native void  native_SendGNS (double lat, double lon, double alt, long time, float bearing, float speed,
												float horAcc, float vertAcc, float bearingAcc, float speedAcc);
	private static native void  native_SendBatteryStat1 (float current, float capacity, float energy);
	private static native void  native_SendBatteryStat2 (float level, float temperature, float voltage, boolean isCharging);
	private static native void  native_ProcessMessages ();
	private static native void  native_Stop ();
	//-----------------------------------------------------


	private final class MyThread
					extends		Thread
					implements	SensorEventListener,
								LocationListener
	{
		private AtomicBoolean	_looping		= new AtomicBoolean(false);
		private int				_serviceType	= 0;	// ServiceInfo.FOREGROUND_SERVICE_TYPE_*
		private Context			_context		= null;
		private CameraHelper	_camera			= null; //new CameraHelper();

		@Override public void  run ()
		{
			native_Start( this );
			if ( _camera != null )
				_camera.Start( _context );

			_context.registerReceiver( this._batInfoReceiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED) );

			_batteryManager	= (BatteryManager)_context.getSystemService( Context.BATTERY_SERVICE );
			_sensorMngr		= (SensorManager)_context.getSystemService( Context.SENSOR_SERVICE );
			_locationMngr	= (LocationManager)_context.getSystemService( Context.LOCATION_SERVICE );

			for (; _looping.get();)
			{
				if ( _camera != null )
					_camera.ProcessTasks();

				_UpdateBattery();
				native_ProcessMessages();

				try {
					sleep(100); // ms
				} catch (Exception e)
				{}
			}

			native_Stop();

			if ( _camera != null )
				_camera.Stop();

			_context.unregisterReceiver( this._batInfoReceiver );

			_context		= null;
			_batteryManager	= null;
			_sensorMngr		= null;
			_locationMngr	= null;
			_batInfoReceiver= null;
		}

		public void  Begin (Context ctx, int serviceType)
		{
			this.setName( "AE" );
			_context	 = ctx;
			_serviceType = serviceType;
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
		//-----------------------------------------------------


		// SensorEventListener //
		@Override public final void onAccuracyChanged (Sensor arg0, int arg1)
		{}

		@Override public final void onSensorChanged (SensorEvent ev)
		{
			native_SendSensor( ev.sensor.getType(), ev.values );
		}
		//-----------------------------------------------------


		// LocationListener //
		@Override public final void onLocationChanged (Location location)
		{
			native_SendGNS( location.getLatitude(), location.getLongitude(),
							(location.hasAltitude() ? location.getAltitude() : -1.0e-30),
							location.getElapsedRealtimeNanos(),
							(location.hasBearing() ? location.getBearing() : -1.0e-30f),
							(location.hasSpeed() ? location.getSpeed() : -1.0e-30f),

							(location.hasAccuracy() ? location.getAccuracy() : 0.f),
							(location.hasVerticalAccuracy() ? location.getVerticalAccuracyMeters() : 0.f),		// API 26
							(location.hasBearingAccuracy() ? location.getBearingAccuracyDegrees() : 0.f),
							(location.hasSpeedAccuracy() ? location.getSpeedAccuracyMetersPerSecond() : 0.f)	// API 26
						  );
		}

		@Override public final void onProviderDisabled (String provider)
		{}

		@Override public final void onProviderEnabled (String provider)
		{}

		@Override public final void onStatusChanged (String provider, int status, Bundle extras)
		{}
		//-----------------------------------------------------


		private BatteryManager	_batteryManager			= null;
		private SensorManager 	_sensorMngr 			= null;
		private LocationManager	_locationMngr			= null;
		private int				_enabledSensorBits		= 0;
		private int				_supportedSensorBits	= 0;
		private boolean			_sensorsEnumerated		= false;
		private final int		_gnsAsSensorType		= 3;
		private final String	_gnsProvider			= LocationManager.GPS_PROVIDER;		// or NETWORK_PROVIDER or PASSIVE_PROVIDER
		private final int		_sensorDelay			= SensorManager.SENSOR_DELAY_GAME;	// or SENSOR_DELAY_NORMAL

		private int  _ExtractBitLog2 (int bits)
		{
			int bit = bits & ~(bits - 1);
			return 31 - Integer.numberOfLeadingZeros( bit );	// int log2
		}

		private boolean  _SetSensorState (boolean enabled, int sensorType, int delay)
		{
			try {
				// GNS
				if ( sensorType == _gnsAsSensorType )
				{
					if ( enabled ) {
						Location last_loc = _locationMngr.getLastKnownLocation( _gnsProvider );
						if ( last_loc != null ) onLocationChanged( last_loc );
						_locationMngr.requestLocationUpdates( _gnsProvider, 0, 0, this );
					} else {
						_locationMngr.removeUpdates( this );
					}
					return true;
				}

				Sensor sensor = _sensorMngr.getDefaultSensor( sensorType );
				if ( enabled ) {
					return _sensorMngr.registerListener( this, sensor, delay, null );
				} else {
					_sensorMngr.unregisterListener( this, sensor );
					return true;
				}
			}
			catch (Exception e) {
				Log.i( TAG, "SetSensorState: " + e.toString() );
				return false;
			}
		}

		private int  _GetSupportedSensorsBits ()
		{
			try {
				List<Sensor> sensors = _sensorMngr.getSensorList( Sensor.TYPE_ALL );
				int bits = 0;

				for (int i = 0; i < sensors.size(); ++i) {
					int type = sensors.get(i).getType();
					if ( type < 31 )
						bits |= (1 << type);
				}

				if ( (_serviceType & ServiceInfo.FOREGROUND_SERVICE_TYPE_LOCATION) != 0 )
					bits |= (1 << _gnsAsSensorType);

				return bits;
			}
			catch (Exception e) {
				return 0;
			}
		}

		@SuppressWarnings("unused")
		public int  EnableSensors (int enableBits)
		{
			if ( enableBits == _enabledSensorBits )
				return _enabledSensorBits;

			if ( !_sensorsEnumerated )
			{
				_sensorsEnumerated   = true;
				_supportedSensorBits = _GetSupportedSensorsBits();
			}

			int	disable = (_enabledSensorBits & ~enableBits) & _supportedSensorBits;
			int	enable  = (~_enabledSensorBits & enableBits) & _supportedSensorBits;
			int enabled = 0;

			_enabledSensorBits &= ~disable;

			for (; disable != 0;)
			{
				int	idx = _ExtractBitLog2( disable );
				disable &= ~(1 << idx);
				_SetSensorState( false, idx, 0 );
			}

			for (; enable != 0;)
			{
				int	idx = _ExtractBitLog2( enable );
				enable &= ~(1 << idx);
				if ( _SetSensorState( true, idx, _sensorDelay ))
					_enabledSensorBits |= (1 << idx);
			}
			return _enabledSensorBits;
		}

		private BroadcastReceiver	_batInfoReceiver = new BroadcastReceiver()
		{
			@Override public void  onReceive (Context ctx, Intent intent)
			{
				boolean	is_present		= intent.getBooleanExtra( BatteryManager.EXTRA_PRESENT, false );
				if ( !is_present )
					return;

				int		ilevel			= intent.getIntExtra( "level", 0 );
				int		ilevel_scale	= intent.getIntExtra( BatteryManager.EXTRA_SCALE, 100 );
				float	level			= ilevel * 100.f / (float)ilevel_scale;
				int		status			= intent.getIntExtra( BatteryManager.EXTRA_STATUS, -1 );
				boolean is_charging		= status == BatteryManager.BATTERY_STATUS_CHARGING ||
										  status == BatteryManager.BATTERY_STATUS_FULL;
				float	temperature		= intent.getIntExtra( BatteryManager.EXTRA_TEMPERATURE, 0 ) * 0.1f;	// C
				float	voltage			= intent.getIntExtra( BatteryManager.EXTRA_VOLTAGE, 0 );			// V, mV, uV

				native_SendBatteryStat2( level, temperature, voltage, is_charging );
			}
		};

		private void  _UpdateBattery ()
		{
			if ( _batteryManager == null )
				return;

			float	current		= _batteryManager.getIntProperty( BatteryManager.BATTERY_PROPERTY_CURRENT_NOW );		// mA, uA
			float	capacity	= _batteryManager.getIntProperty( BatteryManager.BATTERY_PROPERTY_CHARGE_COUNTER );		// mAh, uAh
			float	energy		= _batteryManager.getLongProperty( BatteryManager.BATTERY_PROPERTY_ENERGY_COUNTER );	// nWh

			native_SendBatteryStat1( current, capacity, energy );
		}
	};
	private MyThread  _backgroundThread = null;
	//-----------------------------------------------------


	@Override public void onCreate ()
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

			if ( Build.VERSION.SDK_INT >= 29 )
			{
				startForeground(
					/* id = */ 100, // Cannot be 0
					notification,
					type
				);
			}
			else
			{
				startForeground(
					/* id = */ 100, // Cannot be 0
					notification
				);
			}
			_backgroundThread = new MyThread();
			_backgroundThread.Begin( this, type );
		}
		catch (Exception e)
		{
			Log.e( TAG, "Service.onStartCommand: failed" );
		}
		return START_NOT_STICKY;
	}

	@Override
	public IBinder onBind (Intent intent) {
		return null;
	}


	private Notification  CreateNotification ()
	{
		// TODO: media style?
		// https://developer.android.com/guide/topics/media/legacy/audio/mediabrowserservice#mediastyle-notifications

		String	channel_id;
		if ( Build.VERSION.SDK_INT >= 26 )
			channel_id = CreateChannel();
		else
			channel_id = "";

		NotificationCompat.Builder builder = new NotificationCompat.Builder( this, channel_id )
												.setSmallIcon( android.R.drawable.ic_menu_mylocation )
												.setContentTitle( "remote control" );

		if ( Build.VERSION.SDK_INT >= 31 ) {
			builder.setForegroundServiceBehavior( Notification.FOREGROUND_SERVICE_IMMEDIATE );
		}else{
			builder.setPriority( Notification.PRIORITY_DEFAULT );
		}

		Notification	notification = builder
				.setCategory( Notification.CATEGORY_SERVICE )
				.build();

		return notification;
	}

	private synchronized String  CreateChannel ()
	{
		NotificationManager notification_mngr = (NotificationManager) this.getSystemService( Context.NOTIFICATION_SERVICE );

		if (! notification_mngr.areNotificationsEnabled() )
			Log.e( TAG, "notifications are disabled" );

		String	channel_id	= "RemoteControl.channel";
		int		importance	= NotificationManager.IMPORTANCE_DEFAULT;	// IMPORTANCE_LOW

		NotificationChannel channel = new NotificationChannel( channel_id, "name", importance );

		channel.enableLights( true );
		channel.setLightColor( Color.BLUE );

		channel.enableVibration( true );

		channel.setLockscreenVisibility( Notification.VISIBILITY_PUBLIC );
		channel.setShowBadge( true );

		channel.setDescription( "remote control desc" );

		//channel.setVibrationPattern( new long[]{ 50, 50, 50, 50, 50, 100, 350, 25, 25, 25, 25, 200 });

		if ( notification_mngr != null )
			notification_mngr.createNotificationChannel( channel );
		else
			stopSelf();

		return channel_id;
	}
}
