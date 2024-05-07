// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.engine;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import java.util.List;


//
// Base Activity
//
public class BaseActivity
		extends		Activity
		implements	SurfaceHolder.Callback,
					View.OnKeyListener,
					View.OnTouchListener,
					SensorEventListener,
					LocationListener
{
	public static final String	TAG = "<<<< AE >>>>";

	private int					_wndID			= 0;
	private boolean				_inForeground	= false;

	private final Handler		_handler		= new Handler();
	private final Runnable		_nativeTick		= new Runnable()
	{
		@Override public final void  run ()
		{
			try {
				native_Update( _wndID );
				_handler.post( _nativeTick );
			}
			catch (Exception e) {
				Log.i( TAG, "Exception: " + e.toString() );
			}
		}
	};


//-----------------------------------------------------------------------------
// Activity

	@Override protected void  onCreate (android.os.Bundle savedInstance)
	{
		super.onCreate( savedInstance );

		_sensorMngr		= (SensorManager)getSystemService( Context.SENSOR_SERVICE );
		_locationMngr	= (LocationManager)getSystemService( Context.LOCATION_SERVICE );
		_supportedSensorBits = _GetSupportedSensorsBits();

		_wndID = native_OnCreate( this );
		_CreateSurface();
		_SetFullscreen();

		BaseApplication.SendDisplayInfo( this );
	}

	@Override protected void  onDestroy ()
	{
		super.onDestroy();
		native_OnDestroy( _wndID );

		_sensorMngr		= null;
		_locationMngr	= null;
	}

	@Override protected void  onPause ()
	{
		super.onPause();
		native_OnEnterBackground( _wndID );

		EnableSensors( 0 );
		_inForeground = false;

		// update before stop
		_nativeTick.run();
		_handler.removeCallbacks( _nativeTick );
	}

	@Override protected void  onResume ()
	{
		super.onResume();

		_inForeground = true;
		int	enable = _enabledSensorBits;
		_enabledSensorBits = 0;
		EnableSensors( enable );

		native_OnEnterForeground( _wndID );
		_nativeTick.run();
	}

	@Override protected void  onStart ()
	{
		super.onStart();
		native_OnStart( _wndID );
	}

	@Override protected void  onStop ()
	{
		super.onStop();
		native_OnStop( _wndID );
		_handler.removeCallbacks( _nativeTick );
	}

	@Override public void  onConfigurationChanged (Configuration newConfig)
	{
		super.onConfigurationChanged( newConfig );
		Display display = ((WindowManager)getSystemService( WINDOW_SERVICE )).getDefaultDisplay();
		native_OnOrientationChanged( _wndID, display.getRotation() );
	}

	private void  _SetFullscreen ()
	{
		Window wnd = getWindow();
		wnd.addFlags( WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON );

		// API 1..30
		wnd.addFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN );

		// API 19..30
		// <item name="android:windowTranslucentStatus">true</item>
		wnd.addFlags( WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS );

		// API 19..30
		// <item name="android:windowTranslucentNavigation">true</item>
		wnd.addFlags( WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION );

		if ( Build.VERSION.SDK_INT >= 28 )
		{
			// <item name="android:windowLayoutInDisplayCutoutMode">shortEdges</item>
			wnd.getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
		}

		// https://developer.android.com/develop/ui/views/layout/immersive
		if ( Build.VERSION.SDK_INT >= 30 )
		{
			WindowInsetsControllerCompat ctrl = WindowCompat.getInsetsController( wnd, _surfaceView );
			ctrl.setSystemBarsBehavior( WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE );
			ctrl.hide( WindowInsetsCompat.Type.systemBars() );
		}
	}


//-----------------------------------------------------------------------------
 // SurfaceHolder.Callback

	private SurfaceView		_surfaceView;

	@SuppressLint("ClickableViewAccessibility")
	private void  _CreateSurface ()
	{
		_surfaceView = new SurfaceView( this );
		setContentView( _surfaceView );

		SurfaceHolder holder = _surfaceView.getHolder();
		holder.addCallback( this );

		_surfaceView.setFocusable( true );
		_surfaceView.setFocusableInTouchMode( true );
		_surfaceView.requestFocus();
		_surfaceView.setOnKeyListener( this );
		_surfaceView.setOnTouchListener( this );
	}

	@Override public final void  surfaceChanged (@NonNull SurfaceHolder holder, int format, int w, int h) {
		native_SurfaceChanged( _wndID, holder.getSurface() );
	}

	@Override public final void  surfaceCreated (@NonNull SurfaceHolder holder) {
	}

	@Override public final void  surfaceDestroyed (@NonNull SurfaceHolder holder) {
		native_SurfaceDestroyed( _wndID );
	}


//-----------------------------------------------------------------------------
// View.OnKeyListener

	@Override public final boolean  onKey (View v, int keyCode, KeyEvent ev)
	{
		int action = ev.getAction();
		int count  = ev.getRepeatCount();

		if ( keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
			 keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
			 keyCode == KeyEvent.KEYCODE_VOLUME_MUTE ) {
			// android will change volume
			return false;
		}
		if ( keyCode == KeyEvent.KEYCODE_BACK )
			return false;

		if ( action == KeyEvent.ACTION_MULTIPLE )
			return false;

		native_OnKey( _wndID, keyCode, action, count );
		return true;
	}


//-----------------------------------------------------------------------------
// View.OnTouchListener

	private static final int	_maxTouches		= 8;
	private final float[]		_touchData		= new float[_maxTouches * 4]; // packed: {id, x, y, pressure}

	@Override public final boolean  onTouch (View vw, MotionEvent ev)
	{
		int num_pointers = Math.min( ev.getPointerCount(), _maxTouches );
		int action 		 = ev.getActionMasked();
		int index 		 = ev.getActionIndex();

		if ( action == MotionEvent.ACTION_UP )
			_surfaceView.performClick();

		for (int i = 0, j = 0; i < num_pointers; ++i)
		{
			_touchData[j++] = (float)ev.getPointerId( i );
			_touchData[j++] = ev.getX( i );
			_touchData[j++] = ev.getY( i );
			_touchData[j++] = ev.getPressure( i );
		}

		native_OnTouch( _wndID, action, index, num_pointers, _touchData );
		return true;
	}


//-----------------------------------------------------------------------------
// sensors + GNS

	private SensorManager 	_sensorMngr 			= null;
	private LocationManager	_locationMngr			= null;
	private int				_enabledSensorBits		= 0;
	private int				_supportedSensorBits	= 0;
	private boolean			_sensorsEnumerated		= false;
	private final int		_gnsAsSensorType		= 3;
	private final String	_gnsProvider			= LocationManager.GPS_PROVIDER;		// or NETWORK_PROVIDER or PASSIVE_PROVIDER

	private boolean  _SetSensorState (boolean enabled, int sensorType, int delay)
	{
		try {
			// GNS
			if ( sensorType == _gnsAsSensorType )
			{
				if ( enabled ) {
					onLocationChanged( _locationMngr.getLastKnownLocation( _gnsProvider ));
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

			if ( checkSelfPermission( Manifest.permission.ACCESS_FINE_LOCATION ) == PackageManager.PERMISSION_GRANTED )
				bits |= (1 << _gnsAsSensorType);

			return bits;
		}
		catch (Exception e) {
			return 0;
		}
	}


	// SensorEventListener //
	@Override public final void  onAccuracyChanged (Sensor sensor, int arg1)
	{}

	@Override public final void  onSensorChanged (SensorEvent ev)
	{
		native_UpdateSensor( _wndID, ev.sensor.getType(), ev.values );
	}


	// LocationListener //
	@Override public final void  onLocationChanged (Location location)
	{
		if ( location == null )
			return;

		float	vert_acc 	= 0.f;
		float	bearing_acc	= 0.f;
		float	speed_acc 	= 0.f;
		if ( Build.VERSION.SDK_INT >= 26 ) {
			vert_acc 	= (location.hasVerticalAccuracy() ? location.getVerticalAccuracyMeters() : 0.f);
			bearing_acc	= (location.hasBearingAccuracy() ? location.getBearingAccuracyDegrees() : 0.f);
			speed_acc	= (location.hasSpeedAccuracy() ? location.getSpeedAccuracyMetersPerSecond() : 0.f);
		}
		native_UpdateGNS(	location.getLatitude(), location.getLongitude(),
							(location.hasAltitude() ? location.getAltitude() : -1.0e-30),
							location.getElapsedRealtimeNanos(),
							(location.hasBearing() ? location.getBearing() : -1.0e-30f),
							(location.hasSpeed() ? location.getSpeed() : -1.0e-30f),

							(location.hasAccuracy() ? location.getAccuracy() : 0.f),
							vert_acc,
							bearing_acc,
							speed_acc
						);
	}

	@Override public final void  onProviderDisabled (String provider)
	{}

	@Override public final void  onProviderEnabled (String provider)
	{}

	@Override public final void  onStatusChanged (String provider, int status, Bundle extras)
	{}


//-----------------------------------------------------------------------------
// utils

	private static int  _ExtractBitLog2 (int bits)
	{
		int bit = bits & ~(bits - 1);
		return 31 - Integer.numberOfLeadingZeros( bit );	// int log2
	}


//-----------------------------------------------------------------------------
// called from native

	@SuppressWarnings("unused")
	public void  Close ()
	{
		this.finish();
	}

	@SuppressWarnings("unused")
	public void  EnableSensors (int enableBits)
	{
		if ( enableBits == _enabledSensorBits )
			return;

		if ( !_inForeground )
			return;

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
			if ( _SetSensorState( true, idx, SensorManager.SENSOR_DELAY_GAME ))
				_enabledSensorBits |= (1 << idx);
		}
	}


//-----------------------------------------------------------------------------
// native

	private static native int   native_OnCreate (Object wnd);
	private static native void  native_OnDestroy (int id);
	private static native void  native_OnStart (int id);
	private static native void  native_OnStop (int id);
	private static native void  native_OnEnterForeground (int id);
	private static native void  native_OnEnterBackground (int id);
	private static native void  native_SurfaceChanged (int id, Object surface);
	private static native void  native_SurfaceDestroyed (int id);
	private static native void  native_Update (int id);
	private static native void  native_OnKey (int id, int keycode, int action, int repeatCount);
	private static native void  native_OnTouch (int id, int action, int index, int count, float[] data);
	private static native void  native_OnOrientationChanged (int id, int newOrientation);
	private static native void  native_UpdateSensor (int id, int sensor, float[] values);
	private static native void  native_UpdateGNS (double lat, double lon, double alt, long time, float bearing, float speed,
												  float horAcc, float vertAcc, float bearingAcc, float speedAcc);
}
