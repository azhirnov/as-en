// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.engine;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.os.Handler;
import android.view.Window;
import android.view.WindowManager;

import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import java.util.List;

//
// Base Activity
//
public class BaseActivity
        extends     Activity
        implements  SurfaceHolder.Callback,
                    View.OnKeyListener,
                    View.OnTouchListener,
                    SensorEventListener
{
    public static final String  TAG = "<<<< AE >>>>";

    private int                 _wndID      = 0;

    private Handler             _handler    = new Handler();
    private final Runnable      _nativeTick = new Runnable()
    {
        @Override public final void run ()
        {
            try {
                int delay_ms = native_Update( _wndID );
                _handler.postDelayed(_nativeTick, delay_ms);
            }
            catch (Exception e) {
                Log.i( TAG,"Exception: " + e.toString() );
            }
        }
    };


//-----------------------------------------------------------------------------
// Activity

    @Override protected void onCreate (android.os.Bundle savedInstance)
    {
        super.onCreate( savedInstance );
        _wndID = native_OnCreate( this );
        _CreateSurface();
        _SetFullscreen();
        _sensorManager = (SensorManager)getSystemService( Context.SENSOR_SERVICE );
    }

    @Override protected void onDestroy ()
    {
        super.onDestroy();
        native_OnDestroy( _wndID );
        _sensorManager = null;
    }

    @Override protected void onPause ()
    {
        super.onPause();
        native_OnEnterBackground( _wndID );

        // update before stop
        _nativeTick.run();
        _handler.removeCallbacks( _nativeTick );
    }

    @Override protected void onResume ()
    {
        super.onResume();
        native_OnEnterForeground( _wndID );
        _nativeTick.run();
    }

    @Override protected void onStart ()
    {
        super.onStart();
        native_OnStart( _wndID );
    }

    @Override protected void onStop ()
    {
        super.onStop();
        native_OnStop( _wndID );
        _handler.removeCallbacks( _nativeTick );
    }

    @Override public void onConfigurationChanged (Configuration newConfig)
    {
        super.onConfigurationChanged( newConfig );
        Display display = ((WindowManager)getSystemService( WINDOW_SERVICE )).getDefaultDisplay();
        native_OnOrientationChanged( _wndID, display.getRotation() );
    }

    private void _SetFullscreen ()
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

        // API 28+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            // <item name="android:windowLayoutInDisplayCutoutMode">shortEdges</item>
            wnd.getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }

        // https://developer.android.com/develop/ui/views/layout/display-cutout
        // TODO

        // https://developer.android.com/develop/ui/views/layout/immersive
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            WindowInsetsControllerCompat ctrl = WindowCompat.getInsetsController( wnd, _surfaceView );
            ctrl.setSystemBarsBehavior( WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE );
            ctrl.hide( WindowInsetsCompat.Type.systemBars() );
        }
    }

//-----------------------------------------------------------------------------
 // SurfaceHolder.Callback

    private SurfaceView     _surfaceView;

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

    @Override public final void surfaceChanged (SurfaceHolder holder, int format, int w, int h) {
        native_SurfaceChanged( _wndID, holder.getSurface() );
    }

    @Override public final void surfaceCreated (SurfaceHolder holder) {
    }

    @Override public final void surfaceDestroyed (SurfaceHolder holder) {
        native_SurfaceDestroyed( _wndID );
    }


//-----------------------------------------------------------------------------
// View.OnKeyListener

    @Override public final boolean onKey (View v, int keyCode, KeyEvent ev)
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

    private static final int _maxTouches    = 8;
    private float[]         _touchData      = new float[_maxTouches * 4]; // packed: {id, x, y, pressure}

    @Override public final boolean onTouch (View vw, MotionEvent ev)
    {
        int num_pointers = Math.min( ev.getPointerCount(), _maxTouches );
        int action       = ev.getActionMasked();
        int index        = ev.getActionIndex();

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
// SensorEventListener

    private SensorManager   _sensorManager      = null;
    private int             _enabledSensorBits  = 0;
    private int             _savedSensorBits    = 0;

    private final boolean _SetSensorState (boolean enabled, int sensor, int delay)
    {
        try {
            if ( enabled ) {
                return _sensorManager.registerListener( this, _sensorManager.getDefaultSensor( sensor ), delay, null );
            } else {
                _sensorManager.unregisterListener( this, _sensorManager.getDefaultSensor( sensor ) );
                return true;
            }
        }
        catch (Exception e) {
            return false;
        }
    }

    public final void onAccuracyChanged (Sensor sensor, int arg1)
    {

    }

    public final void onSensorChanged (SensorEvent ev)
    {
        native_UpdateSensor( _wndID, ev.sensor.getType(), ev.values );
    }

    private int _GetSupportedSensorsBits ()
    {
        List<Sensor>    sensors = _sensorManager.getSensorList( Sensor.TYPE_ALL );
        int             bits    = 0;

        for (int i = 0; i < sensors.size(); ++i)
        {
            int type = sensors.get(i).getType();
            if ( type < 32 )
                bits |= 1 << type;
        }
        return bits;
    }

//-----------------------------------------------------------------------------
// called from native

    public void Close ()
    {
        this.finish();
    }

    public int GetSupportedSensorsBits () { return _GetSupportedSensorsBits(); }

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
    private static native int   native_Update (int id);
    private static native void  native_OnKey (int id, int keycode, int action, int repeatCount);
    private static native void  native_OnTouch (int id, int action, int index, int count, float[] data);
    private static native void  native_OnOrientationChanged (int id, int newOrientation);
    private static native void  native_UpdateSensor (int id, int sensor, float[] values);
}
