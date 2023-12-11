// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

package AE.engine;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Environment;
import android.os.LocaleList;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;
import android.widget.Toast;

import java.io.File;

//
// Base Application
//
public class BaseApplication extends Application
{

    public void  onCreate ()
    {
        super.onCreate();
        native_OnCreate( this, getResources().getAssets() );

        _SendDirectories();
        _SendSystemInfo();
        _SendDisplayInfo();
    }



//-----------------------------------------------------------------------------
// utils

    private void _SendDirectories ()
    {
        File    internalPath    = getFilesDir();
        File    internalCache   = getCacheDir();
        File    externalPath    = getExternalFilesDir(null);
        File    externalCache   = getExternalCacheDir();

        native_SetDirectories(
            (internalPath    == null ? "" : internalPath.getAbsolutePath()   ),
            (internalCache   == null ? "" : internalCache.getAbsolutePath()  ),
            (externalPath    == null ? "" : externalPath.getAbsolutePath()   ),
            (externalCache   == null ? "" : externalCache.getAbsolutePath()  ));
    }

    private void  _SendSystemInfo ()
    {
        Context     ctx     = getApplicationContext();
        LocaleList  locales = ctx.getResources().getConfiguration().getLocales();
        String      locale0 = locales.size() > 0 ? locales.get(0).getLanguage() + "-" + locales.get(0).getCountry() : "";
        String      locale1 = locales.size() > 1 ? locales.get(1).getLanguage() + "-" + locales.get(1).getCountry() : "";

        native_SetSystemInfo( locale0, locale1 );
    }

    private void _SendDisplayInfo ()
    {
        Context         ctx     = getApplicationContext();
        WindowManager   wm      = (WindowManager) ctx.getSystemService( Context.WINDOW_SERVICE );
        DisplayMetrics  metrics = new DisplayMetrics();
        Display         display = wm.getDefaultDisplay();
        display.getMetrics( metrics );

        native_SetDisplayInfo(
                metrics.widthPixels,
                metrics.heightPixels,
                metrics.xdpi, metrics.ydpi,
                display.getRotation() );
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

    private static native void  native_OnCreate (Object app, Object assetMngr);
    private static native void  native_SetDirectories (String internal, String internalCache, String external, String externalCache);
    private static native void  native_SetSystemInfo (String iso3Lang0, String iso3Lang1);
    private static native void  native_SetDisplayInfo (int width, int height, float xdpi, float ydpi, int orientation);
}
