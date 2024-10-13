// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/TopDownCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	TopDownCamera::TopDownCamera (RC<DynamicDim> dim, const MovingScale &movingScale,
								  float rotationScale, float2 initialPos) __Th___ :
		_dynDim{ RVRef(dim) },			_movingScale{movingScale},
		_rotationScale{rotationScale},	_initialPos{initialPos}
	{
		CHECK_THROW( _dynDim );
		_Reset();
	}

/*
=================================================
	ProcessInput
=================================================
*/
	void  TopDownCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
	{
		constexpr auto&	IA		= InputActions::Controller_TopDown;
		constexpr auto&	BaseIA	= InputActions::SwitchInputMode;

		float2	move;
		float	angle	= 0.f;
		bool	reset	= false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			StaticAssert( (IA.actionCount - BaseIA.actionCount) == 5 );
			switch ( uint{hdr.name} )
			{
				case IA.Camera_Move :
					move += reader.DataCopy<float2>( hdr.offset );		break;

				case IA.Camera_Rotate :
					angle += reader.Data<float>( hdr.offset );			break;

				case IA.Camera_Reset :
					reset = true;										break;

				// UI_MousePos		- ignore
				// UI_MouseRBDown	- ignore
			}
		}


		EXLOCK( _guard );

		if_unlikely( reset )
			return _Reset();

		move = _movingScale.Apply( move ) * timeDelta.count();
		move = float2x2{_view} * move;

		_position	+= move;
		_angle		= (_angle + angle * _rotationScale).WrapToPi();

		_UpdateMatrix();
	}

/*
=================================================
	_Reset
=================================================
*/
	void  TopDownCamera::_Reset ()
	{
		_position	= _initialPos;

		_UpdateMatrix();
	}

/*
=================================================
	_UpdateMatrix
=================================================
*/
	void  TopDownCamera::_UpdateMatrix ()
	{
		_view = float4x4::RotateZ( _angle ).Translate( float3{ _position, 0.f });

		// TODO: proj
	}

/*
=================================================
	CopyTo
=================================================
*/
	void  TopDownCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
	{
		Frustum_t	frustum;
		frustum.Setup( GetViewProj() );

		_CopyToCameraData( OUT camera, frustum );
	}

/*
=================================================
	GetHelpText
=================================================
*/
	StringView  TopDownCamera::GetHelpText () C_NE___
	{
		return R"(
TopDownCamera controls:
  'W' 'S'           - move forward/backward
  'A' 'D' - - - - - - move left/right
  'Q' 'E'           - rotation
  'Right mouse btn' - pass mouse position to shader
  'R'               - reset position, rotation, zoom
)";
	}


} // AE::ResEditor
