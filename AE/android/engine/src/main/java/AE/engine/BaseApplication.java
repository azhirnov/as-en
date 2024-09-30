// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.engine;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.graphics.Point;
import android.graphics.Rect;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Environment;
import android.os.LocaleList;
import android.os.Build;
import android.os.Debug;
import android.util.Log;
import android.view.Display;
import android.util.DisplayMetrics;
import android.view.DisplayCutout;
import android.view.WindowManager;
import android.view.WindowMetrics;
import android.widget.Toast;

import java.io.File;


//
// Base Application
//
public class BaseApplication
			extends Application
{
	public static final String	TAG = "<<<< AE >>>>";


	public void  onCreate ()
	{
		super.onCreate();
		native_OnCreate( this, getResources().getAssets(), Debug.isDebuggerConnected() );

		_SendDirectories();
		_SendSystemInfo();
	}



//-----------------------------------------------------------------------------
// utils

	private void _SendDirectories ()
	{
		File 	internalPath	= getFilesDir();
		File	internalCache	= getCacheDir();
		File	externalPath	= getExternalFilesDir(null);
		File	externalCache	= getExternalCacheDir();

		native_SetDirectories(
			(internalPath	== null ? "" : internalPath .getAbsolutePath()),
			(internalCache	== null ? "" : internalCache.getAbsolutePath()),
			(externalPath	== null ? "" : externalPath .getAbsolutePath()),
			(externalCache	== null ? "" : externalCache.getAbsolutePath()) );
	}

	private void  _SendSystemInfo ()
	{
		Context		ctx 	= getApplicationContext();
		LocaleList 	locales = ctx.getResources().getConfiguration().getLocales();
		String		locale0	= locales.size() > 0 ? locales.get(0).getLanguage() + "-" + locales.get(0).getCountry() : "";
		String		locale1	= locales.size() > 1 ? locales.get(1).getLanguage() + "-" + locales.get(1).getCountry() : "";

		native_SetSystemInfo( locale0, locale1 );
	}

	public static void SendDisplayInfo (Context ctx)
	{
		WindowManager 	wm 		= (WindowManager) ctx.getSystemService( Context.WINDOW_SERVICE );
		Display 		display	= wm.getDefaultDisplay();

		int				min_w, min_h;
		int				max_w, max_h;
		float			dpi;

		if ( Build.VERSION.SDK_INT >= 30 ) {
			WindowMetrics	wmetrics = wm.getMaximumWindowMetrics();
			DisplayMetrics 	dmetrics = new DisplayMetrics();
			display.getMetrics( dmetrics );
			min_w = dmetrics.widthPixels;
			min_h = dmetrics.heightPixels;
			max_w = wmetrics.getBounds().width();
			max_h = wmetrics.getBounds().height();
			dpi   = (dmetrics.xdpi + dmetrics.ydpi) * 0.5f;
		}else{
			DisplayMetrics 	metrics	= new DisplayMetrics();
			display.getMetrics( metrics );
			max_w = min_w = metrics.widthPixels;
			max_h = min_h = metrics.heightPixels;
			dpi   = (metrics.xdpi + metrics.ydpi) * 0.5f;
		}

		Display.HdrCapabilities	hdr = display.getHdrCapabilities();		// api 24

		int[]	cutout_rects = new int[4*4];
		int		c			 = 0;	// count

		if ( Build.VERSION.SDK_INT >= 29 ) {
			DisplayCutout cutout = display.getCutout();
			if (cutout != null) {
				Rect b = cutout.getBoundingRectBottom();
				if (! b.isEmpty()) {
					cutout_rects[c]   = b.left;
					cutout_rects[c+1] = b.top;
					cutout_rects[c+2] = b.right;
					cutout_rects[c+3] = b.bottom;
					c += 4;
					Log.i( TAG, "Bottom: " + b.toString() );
				}
				Rect l = cutout.getBoundingRectLeft();
				if (! l.isEmpty()) {
					cutout_rects[c]   = l.left;
					cutout_rects[c+1] = l.top;
					cutout_rects[c+2] = l.right;
					cutout_rects[c+3] = l.bottom;
					c += 4;
					Log.i( TAG, "Left: " + l.toString() );
				}
				Rect r = cutout.getBoundingRectRight();
				if (! r.isEmpty()) {
					cutout_rects[c]   = r.left;
					cutout_rects[c+1] = r.top;
					cutout_rects[c+2] = r.right;
					cutout_rects[c+3] = r.bottom;
					c += 4;
					Log.i( TAG, "Right: " + r.toString() );
				}
				Rect t = cutout.getBoundingRectTop();
				if (! t.isEmpty()) {
					cutout_rects[c]   = t.left;
					cutout_rects[c+1] = t.top;
					cutout_rects[c+2] = t.right;
					cutout_rects[c+3] = t.bottom;
					c += 4;
					Log.i( TAG, "Top: " + t.toString() );
				}
			}
		}

		native_SetDisplayInfo(
			min_w, min_h,
			max_w, max_h,
			dpi,
			display.getRotation(),
			hdr.getDesiredMaxAverageLuminance(), hdr.getDesiredMaxLuminance(), hdr.getDesiredMinLuminance(),
			cutout_rects, c
		);

		// TODO: https://developer.android.com/reference/android/view/Window#setPreferMinimalPostProcessing(boolean)
	}


//-----------------------------------------------------------------------------
// called from native

	@SuppressWarnings("unused")
	public final void  ShowToast (String msg, boolean longTime)
	{
		int duration = longTime ? Toast.LENGTH_LONG : Toast.LENGTH_SHORT;
		Toast toast = Toast.makeText( this, msg, duration );
		toast.show();
	}

	@SuppressWarnings("unused")
	public final boolean  IsNetworkConnected ()
	{
		try {
			ConnectivityManager cm = (ConnectivityManager)getSystemService( Context.CONNECTIVITY_SERVICE );
			NetworkInfo ni = cm.getActiveNetworkInfo();
			if ( ni != null ) {
				NetworkInfo.DetailedState state = ni.getDetailedState();
				if ( state == NetworkInfo.DetailedState.CONNECTED )
					return true;
			}
		} catch (Exception e) {
			//Log.e( TAG, "exception: " + e.toString() );
		}
		return false;
	}

//-----------------------------------------------------------------------------
// native

	private static native void  native_OnCreate (Object app, Object assetMngr, boolean isUnderDebugger);
	private static native void  native_SetDirectories (String internal, String internalCache, String external, String externalCache);
	private static native void  native_SetSystemInfo (String iso3Lang0, String iso3Lang1);
	private static native void  native_SetDisplayInfo (int minWidth, int minHeight, int maxWidth, int maxHeight,
													   float dpi, int orientation,
													   float avrLum, float maxLum, float minLum,
													   int[] cutoutRects, int cutoutRectCount);
	public  static native void  native_EnableCamera ();  // requires camera permission
}
