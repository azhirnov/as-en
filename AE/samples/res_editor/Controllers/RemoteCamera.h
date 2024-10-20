// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Controllers/IController.h"

namespace AE::ResEditor
{

	//
	// Remote Camera
	//

	class RemoteCamera final : public IController
	{
	// variables
	private:
		mutable RWSpinLock		_guard;

		RC<DynamicDim>			_dynDim;

		float4x4				_view;				// cached
		float4x4				_viewProj;			// cached
		float4x4				_invViewProj;		// cached

		TFPVCamera<float>		_camera;
		const float2			_clipPlanes;
		const Rad				_fovY;
		const MovingScale		_movingScale;
		const float3			_initialPos;
		const float				_zoomSpeed		= 10.f;	// zoom/sec
		const bool				_reverseZ;

		float					_dimAspect		= 1.f;


	// methods
	public:
		RemoteCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY,
					  const MovingScale &, float3 initialPos, bool reverseZ) __Th___;

		// IController //
		void  ProcessInput (ActionQueueReader, secondsf)				__NE_OV;

		InputModeName	GetInputMode ()									C_NE_OV { return InputModeName{"Controller.RemoteCamera"}; }

		float3			GetPosition ()									C_NE_OV	{ SHAREDLOCK( _guard );  return _camera.Position(); }
		float4x4		GetViewProj ()									C_NE_OV { SHAREDLOCK( _guard );  return _viewProj; }
		float4x4		GetInvViewProj ()								C_NE_OV { SHAREDLOCK( _guard );  return _invViewProj; }
		float4x4		GetProj ()										C_NE_OV { SHAREDLOCK( _guard );  return _camera.Projection(); }
		float4x4		GetView ()										C_NE_OV { SHAREDLOCK( _guard );  return _view; }
		float			GetZoom ()										C_NE_OV	{ return 1.f; }
		float2			GetClipPlanes ()								C_NE_OV	{ return _clipPlanes; }
		StringView		GetHelpText ()									C_NE_OV;
	//	RaysGrid_t		GetRaysGrid ()									C_NE_OV;

		void			CopyTo (OUT AE::ShaderTypes::CameraData &)		C_NE_OV;

	private:
		void  _Reset ();
		void  _UpdateMatrix ();
	};


} // AE::ResEditor
