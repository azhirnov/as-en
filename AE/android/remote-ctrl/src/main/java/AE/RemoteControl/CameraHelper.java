// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	based on:
	https://github.com/chromium/chromium/blob/main/media/capture/video/android/java/src/org/chromium/media/VideoCaptureCamera2.java
*/

package AE.RemoteControl;


import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Set;
import java.util.Map;
import java.util.HashMap;
import java.util.concurrent.Executor;
import java.util.Collections;

import android.util.Pair;
import android.util.Range;
import android.util.Size;
import android.util.Log;

import android.content.Context;

import android.graphics.Rect;
import android.graphics.ImageFormat;
import android.media.Image;
import android.media.ImageReader;
import android.view.Surface;

import android.hardware.HardwareBuffer;

import android.hardware.camera2.*;
import android.hardware.camera2.params.*;


public final class CameraHelper
				implements	Executor
{
	private static final String	TAG = "<<<< AE >>>>";


	private class ActiveCapture
	{
		public CameraCaptureSession		session		= null;
		public CaptureRequest.Builder	request		= null;

		public void  Stop () {
			try {
				request = null;
				if ( session != null )
					session.abortCaptures();
			}
			catch (Exception e) {
				Log.e( TAG, "Stop exception: " + e.toString() );
			}
		}

		public void  Request (Executor executor, ImageReader img) {
			try {
				// Calling this method will replace any earlier repeating request
				session.setSingleRepeatingRequest(
						request.build(),
						executor,
						new CameraCaptureSessionCaptureCallback( img ));
			}
			catch (Exception e) {
				Log.e( TAG, "Request exception: " + e.toString() );
			}
		}
	};
	//-----------------------------------------------------


	private static native void  native_CameraResult (int[] intData, float[] floatData);
	//-----------------------------------------------------


	private CameraManager				_cameraMngr			= null;
	private Map<String, ActiveCapture>	_activeCapture		= new HashMap<String, ActiveCapture>();

	private List<Runnable>				_taskList			= Collections.synchronizedList( new ArrayList<Runnable>() );
	//-----------------------------------------------------


	@Override public void  execute (Runnable command)
	{
		synchronized(_taskList) {
			_taskList.add( command );
		}
	}

	public void  ProcessTasks ()
	{
		while ( true )
		{
			Runnable	task = null;
			synchronized(_taskList) {
				if ( !_taskList.isEmpty() )
					task = _taskList.remove(0);
				else
					return;
			}
			task.run();
		}
	}


	public void  Start (Context ctx)
	{
		_cameraMngr = (CameraManager) ctx.getSystemService( Context.CAMERA_SERVICE );
		PrintCameras();
		//OpenCamera( "0", 640, 480, ImageFormat.YUV_420_888 );
		//OpenCamera( "0", 640, 480, ImageFormat.PRIVATE );
		OpenCamera( "0", 4064, 3048, ImageFormat.YV12 );
	}

	public void  Stop ()
	{
		for (Map.Entry<String, ActiveCapture> entry : _activeCapture.entrySet())
			entry.getValue().Stop();
		_activeCapture.clear();

		_cameraMngr = null;
	}


	private void  OpenCamera (String cameraId, int surfW, int surfH, int surfFmt)
	{
		try {
			_cameraMngr.openCamera( cameraId, this, new CameraDeviceStateCallback( surfW, surfH, surfFmt ));
		}
		catch (Exception e) {
			Log.e( TAG, "OpenCamera exception: " + e.toString() );
		}
	}

	private void  CloseCamera (String cameraId)
	{
		try {
			ActiveCapture	capture = _activeCapture.remove( cameraId );
			if ( capture != null ) {
				capture.Stop();
			}
		}
		catch (Exception e) {
			Log.e( TAG, "CloseCamera exception: " + e.toString() );
		}
	}
	//-----------------------------------------------------


	private class CameraCaptureSessionCaptureCallback extends CameraCaptureSession.CaptureCallback
	{
		private ImageReader		_imageReader	= null;

		CameraCaptureSessionCaptureCallback (ImageReader img) {
			this._imageReader = img;
		}

		@Override public void  onCaptureCompleted (CameraCaptureSession session, CaptureRequest request, TotalCaptureResult totalResult)
		{
			CaptureResult		partialResult	= totalResult.getPartialResults().get(0);
			Integer				flash_mode		= partialResult.get( CaptureResult.FLASH_MODE );						// int_data[0]
			Integer				flash_state		= partialResult.get( CaptureResult.FLASH_STATE );			// opt		// int_data[1]
			Integer				lens_state		= partialResult.get( CaptureResult.LENS_STATE );			// opt		// int_data[2]
			Float				lens_aperture	= partialResult.get( CaptureResult.LENS_APERTURE );			// opt		// float_data[0]
			Float				focal_length	= partialResult.get( CaptureResult.LENS_FOCAL_LENGTH );					// float_data[1]
			Float				focus_dist		= partialResult.get( CaptureResult.LENS_FOCUS_DISTANCE );	// opt		// float_data[2]
			Pair<Float, Float>	focus_range		= partialResult.get( CaptureResult.LENS_FOCUS_RANGE );		// opt		// float_data[3,4]
			Long				exposure_time	= partialResult.get( CaptureResult.SENSOR_EXPOSURE_TIME );	// opt		// int_data[3]
			Long				timestamp		= partialResult.get( CaptureResult.SENSOR_TIMESTAMP );					// int_data[4]
			// TODO
			//	LENS_DISTORTION
			//	LENS_FILTER_DENSITY
			//	LOGICAL_MULTI_CAMERA_ACTIVE_PHYSICAL_ID

			//int[]	int_data	= new int[32];
			//float[]	float_data	= new float[32];

			//native_CameraResult( _imageReader.getSurface(), int_data, float_data );
		}
	};
	//-----------------------------------------------------


	private class CameraCaptureSessionStateCallback extends CameraCaptureSession.StateCallback
	{
		private CameraDevice	_cameraDev		= null;
		private ImageReader		_imageReader	= null;

		CameraCaptureSessionStateCallback (CameraDevice dev, ImageReader img) {
			this._cameraDev		= dev;
			this._imageReader	= img;
		}

		@Override public void  onConfigureFailed (CameraCaptureSession session) {
			Log.e( TAG, "onConfigureFailed" );
		}

		@Override public void  onConfigured (CameraCaptureSession session) {
			try {
				ActiveCapture	capture = new ActiveCapture();
				capture.session = session;
				_activeCapture.put( _cameraDev.getId(), capture );

				capture.request = _cameraDev.createCaptureRequest( CameraDevice.TEMPLATE_PREVIEW );
				capture.request.addTarget( _imageReader.getSurface() );

				capture.Request( CameraHelper.this, _imageReader );
			}
			catch (Exception e) {
				Log.e( TAG, "onConfigured exception: " + e.toString() );
			}
		}
	};
	//-----------------------------------------------------


	private class CameraDeviceStateCallback extends CameraDevice.StateCallback
	{
		private int				_width;
		private int				_height;
		private int				_format;		// ImageFormat
		private final int		_maxImages		= 3;
		private ImageReader		_imageReader	= null;

		CameraDeviceStateCallback (int w, int h, int fmt) {
			this._width		= w;
			this._height	= h;
			this._format	= fmt;
		}

		@Override public void  onDisconnected (CameraDevice camera) {
			Log.e( TAG, "onDisconnected" );
		}

		@Override public void  onError (CameraDevice camera, int error) {
			Log.e( TAG, "onError" );
		}

		@Override public void  onOpened (CameraDevice camera) {
			try {
				_imageReader = ImageReader.newInstance( _width, _height, _format, _maxImages, HardwareBuffer.USAGE_GPU_SAMPLED_IMAGE );

				OutputConfiguration		out_config = new OutputConfiguration( _imageReader.getSurface() );

				/*{
					CameraCharacteristics 	ch 			= _cameraMngr.getCameraCharacteristics( camera.getId() );
					Set<String> 			phys_ids	= ch.getPhysicalCameraIds();
					Iterator<String> 		phys_cam_it = phys_ids.iterator();

					if ( phys_cam_it.hasNext() )
						out_config.setPhysicalCameraId( phys_cam_it.next() );
				}*/
				// setDynamicRangeProfile

				SessionConfiguration	config = new SessionConfiguration(
						SessionConfiguration.SESSION_REGULAR,
						Collections.singletonList( out_config ),
						CameraHelper.this,  // Executor
						new CameraCaptureSessionStateCallback( camera, _imageReader ));

				camera.createCaptureSession( config );	// API 28
			}
			catch (Exception e) {
				Log.e( TAG, "onOpened exception: " + e.toString() );
			}
		}
	};
	//-----------------------------------------------------


	private void  PrintCameras ()
	{
		try {
			String[] ids = _cameraMngr.getCameraIdList();
			for (String id : ids)
			{
				CameraCharacteristics ch = _cameraMngr.getCameraCharacteristics(id);
				String info = "Camera: " + id;
				if ( ch.get( CameraCharacteristics.FLASH_INFO_AVAILABLE ))
					info += "\n  flash:  yes";
				Range<Float> zoom_ratio_range = ch.get( CameraCharacteristics.CONTROL_ZOOM_RATIO_RANGE );
				info += "\n  zoom ratio range: " + zoom_ratio_range.toString();
				// JPEG_AVAILABLE_THUMBNAIL_SIZES
				// LENS_DISTORTION
				int lens_facing = ch.get( CameraCharacteristics.LENS_FACING );
				info += "\n  lens facing: " +  (lens_facing == CameraMetadata.LENS_FACING_FRONT ? "front" :
												lens_facing == CameraMetadata.LENS_FACING_BACK ? "back" : "unknown");
				// LENS_INFO_AVAILABLE_APERTURES
				float[] focal_lengths = ch.get( CameraCharacteristics.LENS_INFO_AVAILABLE_FOCAL_LENGTHS );
				info += "\n  focal lengths: " + focal_lengths.toString();
				Integer focus_dist_cal = ch.get( CameraCharacteristics.LENS_INFO_FOCUS_DISTANCE_CALIBRATION );
				Float hyperfocal_dist = ch.get( CameraCharacteristics.LENS_INFO_HYPERFOCAL_DISTANCE );
				if ( hyperfocal_dist != null && focus_dist_cal != null ) {
					info += "\n  hyperfocal dist: " + hyperfocal_dist + " (" +
							(focus_dist_cal == CameraMetadata.LENS_INFO_FOCUS_DISTANCE_CALIBRATION_CALIBRATED ? "CALIBRATED" :
							 focus_dist_cal == CameraMetadata.LENS_INFO_FOCUS_DISTANCE_CALIBRATION_UNCALIBRATED ? "UNCALIBRATED" :
							 focus_dist_cal == CameraMetadata.LENS_INFO_FOCUS_DISTANCE_CALIBRATION_APPROXIMATE ? "APPROXIMATE" : "") + ")";
				}
				Float min_focus_dist = ch.get( CameraCharacteristics.LENS_INFO_MINIMUM_FOCUS_DISTANCE );
				if ( min_focus_dist != null )
					info += "\n  min focus dist: " + min_focus_dist;
				// LENS_INTRINSIC_CALIBRATION
				// LENS_POSE_ROTATION
				// LENS_POSE_TRANSLATION
				// NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES
				int[] caps = ch.get( CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES );
				info += "\n  caps:";
				for (int cap : caps) {
					switch ( cap ) {
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE :			info += " BACKWARD_COMPATIBLE";			break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR :					info += " MANUAL_SENSOR";				break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_MANUAL_POST_PROCESSING :			info += " MANUAL_POST_PROCESSING";		break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_RAW :							info += " RAW";							break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_PRIVATE_REPROCESSING :			info += " PRIVATE_REPROCESSING";		break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS :			info += " READ_SENSOR_SETTINGS";		break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_BURST_CAPTURE :					info += " BURST_CAPTURE";				break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_YUV_REPROCESSING :				info += " YUV_REPROCESSING";			break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT :					info += " DEPTH_OUTPUT";				break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_CONSTRAINED_HIGH_SPEED_VIDEO :	info += " CONSTRAINED_HIGH_SPEED_VIDEO";break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_MOTION_TRACKING :				info += " MOTION_TRACKING";				break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_LOGICAL_MULTI_CAMERA :			info += " LOGICAL_MULTI_CAMERA";		break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_MONOCHROME :						info += " MONOCHROME";					break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_SECURE_IMAGE_DATA :				info += " SECURE_IMAGE_DATA";			break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_SYSTEM_CAMERA :					info += " SYSTEM_CAMERA";				break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_OFFLINE_PROCESSING :				info += " OFFLINE_PROCESSING";			break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_ULTRA_HIGH_RESOLUTION_SENSOR :	info += " ULTRA_HIGH_RESOLUTION_SENSOR";break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_REMOSAIC_REPROCESSING :			info += " REMOSAIC_REPROCESSING";		break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_DYNAMIC_RANGE_TEN_BIT :			info += " DYNAMIC_RANGE_TEN_BIT";		break;
						case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_STREAM_USE_CASE :				info += " STREAM_USE_CASE";				break;
					//	case CameraMetadata.REQUEST_AVAILABLE_CAPABILITIES_COLOR_SPACE_PROFILES :			info += " COLOR_SPACE_PROFILES";		break;
					}
				}
				// REQUEST_AVAILABLE_COLOR_SPACE_PROFILES - API 34
				// REQUEST_AVAILABLE_DYNAMIC_RANGE_PROFILES - API 33
				// REQUEST_MAX_NUM_INPUT_STREAMS
				// REQUEST_MAX_NUM_OUTPUT_PROC
				// REQUEST_MAX_NUM_OUTPUT_RAW
				// SCALER_AVAILABLE_MAX_DIGITAL_ZOOM
				StreamConfigurationMap	cfg_map = ch.get( CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP );
				{
					int[]	out_fmts = cfg_map.getOutputFormats();
					info += "\n  output formats:";
					for (int fmt : out_fmts) {
						boolean found = true;
						switch ( fmt ) {
							case ImageFormat.DEPTH16 :				info += "\n    DEPTH16";			break;
							case ImageFormat.DEPTH_JPEG :			info += "\n    DEPTH_JPEG";			break;
							case ImageFormat.DEPTH_POINT_CLOUD :	info += "\n    DEPTH_POINT_CLOUD";	break;
							case ImageFormat.JPEG :					info += "\n    JPEG";				break;
							case ImageFormat.NV16 :					info += "\n    NV16";				break;
							case ImageFormat.NV21 :					info += "\n    NV21";				break;
							case ImageFormat.PRIVATE :				info += "\n    PRIVATE";			break;
							case ImageFormat.RAW10 :				info += "\n    RAW10";				break;
							case ImageFormat.RAW12 :				info += "\n    RAW12";				break;
							case ImageFormat.RAW_PRIVATE :			info += "\n    RAW_PRIVATE";		break;
							case ImageFormat.RAW_SENSOR :			info += "\n    RAW_SENSOR";			break;
							case ImageFormat.YCBCR_P010 :			info += "\n    YCBCR_P010";			break;
							case ImageFormat.YUV_420_888 :			info += "\n    YUV_420_888";		break;
							case ImageFormat.YUV_422_888 :			info += "\n    YUV_422_888";		break;
							case ImageFormat.YUV_444_888 :			info += "\n    YUV_444_888";		break;
							case ImageFormat.YUY2 :					info += "\n    YUY2";				break;
							case ImageFormat.YV12 :					info += "\n    YV12";				break;
							default :								found = false;						break;
						}

						if ( found ) {
							Size[]	sizes = cfg_map.getOutputSizes( fmt );
							info += ":";
							for (Size sz : sizes) {
								info += " " + sz.toString() + "/" + (int)(1.0e+9f / cfg_map.getOutputMinFrameDuration( fmt, sz ) + 0.5f);
							}
						}
					}
				}

				//Rect array_size = ch.get( CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE );
				//info += "\n  array size: " + array_size.toString();
				Range<Long> exp_time_range = ch.get( CameraCharacteristics.SENSOR_INFO_EXPOSURE_TIME_RANGE );
				if ( exp_time_range != null )
					info += "\n  exp time range: " + exp_time_range.toString();
				// SENSOR_INFO_MAX_FRAME_DURATION
				// SENSOR_INFO_PHYSICAL_SIZE
				Size px_array_size = ch.get( CameraCharacteristics.SENSOR_INFO_PIXEL_ARRAY_SIZE );
				info += "\n  px array size: " + px_array_size.toString();
				// SENSOR_INFO_PRE_CORRECTION_ACTIVE_ARRAY_SIZE
				// SENSOR_INFO_SENSITIVITY_RANGE
				// SENSOR_INFO_TIMESTAMP_SOURCE
				// SENSOR_INFO_WHITE_LEVEL
				// SENSOR_MAX_ANALOG_SENSITIVITY
				// SENSOR_OPTICAL_BLACK_REGIONS
				// SENSOR_ORIENTATION
				Set<String> phys_ids = ch.getPhysicalCameraIds();
				info += "\n  physical ids: " + phys_ids.toString();

				Log.i( TAG, info );
			}
		}
		catch (Exception e) {
			Log.e( TAG, "PrintCameras exception: " + e.toString() );
		}
	}
}
