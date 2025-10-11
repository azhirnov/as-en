// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
		Direct transform
	Convert viewport space to surface coordinate space.
	Examples: ToMatrix, Transform.

		Inverse transform
	Convert sensor world space to surface coordinate space,
	Convert viewport space to surface with pre-transform coordinate space.
	Examples: ToInvMatrix, InvTransform.
*/

#pragma once

namespace AE::Graphics
{
	using namespace AE::Base;


	//
	// Surface Transform
	//

	enum class ESurfaceTransform : ubyte
	{
		Deg_0,
		Deg_90,
		Deg_180,
		Deg_270,

		HorizontalMirror_0,
		HorizontalMirror_90,
		HorizontalMirror_180,
		HorizontalMirror_270,

		_Count,
		Identity	= Deg_0,
		Unknown		= _Count,
	};


	//
	// Surface Transform Utils
	//

	struct SurfaceTransformUtils final : Noninstanceable
	{
		ND_ static float2x2		ToMatrix (ESurfaceTransform)		__NE___;
		ND_ static float2x2		ToInvMatrix (ESurfaceTransform)		__NE___;
		ND_ static float3x3		ToInvMatrix3 (ESurfaceTransform)	__NE___;

		ND_ static Quat			ToQuat (ESurfaceTransform)			__NE___;
		ND_ static Quat			ToInvQuat (ESurfaceTransform)		__NE___;

		ND_ static bool			IsAny90deg (ESurfaceTransform)		__NE___;

		template <typename T>		static void			Swap (ESurfaceTransform, INOUT T &x, INOUT T &y)									__NE___;

		template <typename T>	ND_ static Vec<T,2>		Transform (ESurfaceTransform, const Vec<T,2> &size)									__NE___;
		template <typename T>	ND_ static Vec<T,2>		Transform (ESurfaceTransform, const Vec<T,2> &pos, const Vec<T,2> &size)			__NE___;
		template <typename T>	ND_ static Rectangle<T>	Transform (ESurfaceTransform, const Rectangle<T> &rect, const Vec<T,2> &size)		__NE___;

		template <typename T>	ND_ static Vec<T,2>		InvTransform (ESurfaceTransform, const Vec<T,2> &size)								__NE___;
		template <typename T>	ND_ static Vec<T,2>		InvTransform (ESurfaceTransform, const Vec<T,2> &pos, const Vec<T,2> &size)			__NE___;
		template <typename T>	ND_ static Rectangle<T>	InvTransform (ESurfaceTransform, const Rectangle<T> &rect, const Vec<T,2> &size)	__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	ToMatrix
=================================================
*/
	inline float2x2  SurfaceTransformUtils::ToMatrix (ESurfaceTransform orient) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :			return float2x2{ { 1.f,  0.f}, { 0.f,  1.f} };
			case_likely ESurfaceTransform::Deg_90 :			return float2x2{ { 0.f, -1.f}, { 1.f,  0.f} };
			case_likely ESurfaceTransform::Deg_180 :		return float2x2{ {-1.f,  0.f}, { 0.f, -1.f} };
			case_likely ESurfaceTransform::Deg_270 :		return float2x2{ { 0.f,  1.f}, {-1.f,  0.f} };

			case ESurfaceTransform::HorizontalMirror_0 :	return float2x2{ {-1.f,  0.f}, { 0.f,  1.f} };
			case ESurfaceTransform::HorizontalMirror_90 :	return float2x2{ { 0.f, -1.f}, {-1.f,  0.f} };
			case ESurfaceTransform::HorizontalMirror_180 :	return float2x2{ { 1.f,  0.f}, { 0.f, -1.f} };
			case ESurfaceTransform::HorizontalMirror_270 :	return float2x2{ { 0.f,  1.f}, { 1.f,  0.f} };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

/*
=================================================
	ToInvMatrix
=================================================
*/
	inline float2x2  SurfaceTransformUtils::ToInvMatrix (ESurfaceTransform orient) __NE___
	{
		return ToMatrix( orient ).Transpose();
	}

	inline float3x3  SurfaceTransformUtils::ToInvMatrix3 (ESurfaceTransform orient) __NE___
	{
		return float3x3{ ToMatrix( orient ).Transpose() };
	}

/*
=================================================
	ToQuat
=================================================
*/
	inline Quat  SurfaceTransformUtils::ToQuat (ESurfaceTransform orient) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :
			case ESurfaceTransform::HorizontalMirror_0 :	return Quat{ 1.0f, 0.f, 0.f, 0.f };						// RotateZ( 0_deg )

			case_likely ESurfaceTransform::Deg_90 :
			case ESurfaceTransform::HorizontalMirror_90 :	return Quat{ -0.707106769f, 0.f, 0.f, 0.707106769f };	// RotateZ( 270_deg )

			case_likely ESurfaceTransform::Deg_180 :
			case ESurfaceTransform::HorizontalMirror_180 :	return Quat{ 0.f, 0.f, 0.f, 1.0f };						// RotateZ( 180_deg )

			case_likely ESurfaceTransform::Deg_270 :
			case ESurfaceTransform::HorizontalMirror_270 :	return Quat{ 0.707106769f, 0.f, 0.f, 0.707106769f };	// RotateZ( 90_deg )

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

/*
=================================================
	ToInvQuat
=================================================
*/
	inline Quat  SurfaceTransformUtils::ToInvQuat (ESurfaceTransform orient) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :
			case ESurfaceTransform::HorizontalMirror_0 :	return Quat{ 1.0f, 0.f, 0.f, 0.f };						// RotateZ( 0_deg )

			case_likely ESurfaceTransform::Deg_90 :
			case ESurfaceTransform::HorizontalMirror_90 :	return Quat{ 0.707106769f, 0.f, 0.f, 0.707106769f };	// RotateZ( 90_deg )

			case_likely ESurfaceTransform::Deg_180 :
			case ESurfaceTransform::HorizontalMirror_180 :	return Quat{ 0.f, 0.f, 0.f, 1.0f };						// RotateZ( 180_deg )

			case_likely ESurfaceTransform::Deg_270 :
			case ESurfaceTransform::HorizontalMirror_270 :	return Quat{ -0.707106769f, 0.f, 0.f, 0.707106769f };	// RotateZ( 270_deg )

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

/*
=================================================
	IsAny90deg
=================================================
*/
	inline bool  SurfaceTransformUtils::IsAny90deg (ESurfaceTransform orient) __NE___
	{
		const bool	res = !!(uint(orient) & 1);
		#ifdef AE_DEBUG
			switch_enum( orient )
			{
				case ESurfaceTransform::Deg_0 :
				case ESurfaceTransform::Deg_180 :
				case ESurfaceTransform::HorizontalMirror_0 :
				case ESurfaceTransform::HorizontalMirror_180 :	ASSERT( not res );	break;

				case ESurfaceTransform::Deg_90 :
				case ESurfaceTransform::Deg_270 :
				case ESurfaceTransform::HorizontalMirror_90 :
				case ESurfaceTransform::HorizontalMirror_270 :	ASSERT( res );  break;

				case ESurfaceTransform::Unknown :
				default :										DBG_WARNING( "unknown surface transform" );  break;
			}
			switch_end
		#endif
		return res;
	}

/*
=================================================
	Swap
=================================================
*/
	template <typename T>
	void  SurfaceTransformUtils::Swap (ESurfaceTransform orient, INOUT T &x, INOUT T &y) __NE___
	{
		if ( IsAny90deg( orient ))
			std::swap( x, y );
	}

/*
=================================================
	Transform
----
	rotate from 0 to 'orient'.
	'pos' and 'size' must be in same orientation (0).
=================================================
*/
	template <typename T>
	Vec<T,2>  SurfaceTransformUtils::Transform (ESurfaceTransform orient, const Vec<T,2> &size) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :
			case_likely ESurfaceTransform::Deg_180 :
			case ESurfaceTransform::HorizontalMirror_0 :
			case ESurfaceTransform::HorizontalMirror_180 :	return size;

			case_likely ESurfaceTransform::Deg_90 :
			case_likely ESurfaceTransform::Deg_270 :
			case ESurfaceTransform::HorizontalMirror_90 :
			case ESurfaceTransform::HorizontalMirror_270 :	return Vec<T,2>{ size.y, size.x };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

	template <typename T>
	Vec<T,2>  SurfaceTransformUtils::Transform (ESurfaceTransform orient, const Vec<T,2> &pos, const Vec<T,2> &size) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :			return pos;
			case_likely ESurfaceTransform::Deg_90 :			return Vec<T,2>{ pos.y,				size.x - pos.x };
			case_likely ESurfaceTransform::Deg_180 :		return Vec<T,2>{ size.x - pos.x,	size.y - pos.y };
			case_likely ESurfaceTransform::Deg_270 :		return Vec<T,2>{ size.y - pos.y,	pos.x };

			case ESurfaceTransform::HorizontalMirror_0 :	return Vec<T,2>{ size.x - pos.x,	pos.y };
			case ESurfaceTransform::HorizontalMirror_90 :	return Vec<T,2>{ pos.y,				pos.x };
			case ESurfaceTransform::HorizontalMirror_180 :	return Vec<T,2>{ pos.x,				size.y - pos.y };
			case ESurfaceTransform::HorizontalMirror_270 :	return Vec<T,2>{ size.y - pos.y,	size.x - pos.x };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

	template <typename T>
	Rectangle<T>  SurfaceTransformUtils::Transform (ESurfaceTransform orient, const Rectangle<T> &pos, const Vec<T,2> &size) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :			return pos;
			case_likely ESurfaceTransform::Deg_90 :			return Rectangle<T>{ pos.top,             size.x - pos.right,  pos.bottom,         size.x - pos.left };
			case_likely ESurfaceTransform::Deg_180 :		return Rectangle<T>{ size.x - pos.right,  size.y - pos.bottom, size.x - pos.left,  size.y - pos.top  };
			case_likely ESurfaceTransform::Deg_270 :		return Rectangle<T>{ size.y - pos.bottom, pos.left,            size.y - pos.top,   pos.right         };

			case ESurfaceTransform::HorizontalMirror_0 :	return Rectangle<T>{ size.x - pos.right,  pos.top,             size.x - pos.left,  pos.bottom        };
			case ESurfaceTransform::HorizontalMirror_90 :	return Rectangle<T>{ size.y - pos.bottom, size.x - pos.right,  size.y - pos.top,   size.x - pos.left };
			case ESurfaceTransform::HorizontalMirror_180 :	return Rectangle<T>{ pos.left,            size.y - pos.bottom, pos.right,          size.y - pos.top  };
			case ESurfaceTransform::HorizontalMirror_270 :	return Rectangle<T>{ pos.top,             pos.left,            pos.bottom,         pos.right         };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

/*
=================================================
	InvTransform
=================================================
*/
	template <typename T>
	Vec<T,2>  SurfaceTransformUtils::InvTransform (ESurfaceTransform orient, const Vec<T,2> &size) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :
			case_likely ESurfaceTransform::Deg_180 :
			case ESurfaceTransform::HorizontalMirror_0 :
			case ESurfaceTransform::HorizontalMirror_180 :	return size;

			case_likely ESurfaceTransform::Deg_90 :
			case_likely ESurfaceTransform::Deg_270 :
			case ESurfaceTransform::HorizontalMirror_90 :
			case ESurfaceTransform::HorizontalMirror_270 :	return Vec<T,2>{ size.y, size.x };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

	template <typename T>
	Vec<T,2>  SurfaceTransformUtils::InvTransform (ESurfaceTransform orient, const Vec<T,2> &pos, const Vec<T,2> &size) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :			return pos;
			case_likely ESurfaceTransform::Deg_90 :			return Vec<T,2>{ size.y - pos.y,	pos.x };
			case_likely ESurfaceTransform::Deg_180 :		return Vec<T,2>{ size.x - pos.x,	size.y - pos.y };
			case_likely ESurfaceTransform::Deg_270 :		return Vec<T,2>{ pos.y,				size.x - pos.x };

			case ESurfaceTransform::HorizontalMirror_0 :	return Vec<T,2>{ size.x - pos.x,	pos.y };
			case ESurfaceTransform::HorizontalMirror_90 :	return Vec<T,2>{ pos.y,				pos.x };
			case ESurfaceTransform::HorizontalMirror_180 :	return Vec<T,2>{ pos.x,				size.y - pos.y };
			case ESurfaceTransform::HorizontalMirror_270 :	return Vec<T,2>{ size.y - pos.y,	size.x - pos.x };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

	template <typename T>
	Rectangle<T>  SurfaceTransformUtils::InvTransform (ESurfaceTransform orient, const Rectangle<T> &pos, const Vec<T,2> &size) __NE___
	{
		switch_enum( orient )
		{
			case_likely ESurfaceTransform::Deg_0 :			return pos;
			case_likely ESurfaceTransform::Deg_90 :			return Rectangle<T>{ size.y - pos.bottom, pos.left,            size.y - pos.top,   pos.right         };
			case_likely ESurfaceTransform::Deg_180 :		return Rectangle<T>{ size.x - pos.right,  size.y - pos.bottom, size.x - pos.left,  size.y - pos.top  };
			case_likely ESurfaceTransform::Deg_270 :		return Rectangle<T>{ pos.top,             size.x - pos.right,  pos.bottom,         size.x - pos.left };

			case ESurfaceTransform::HorizontalMirror_0 :	return Rectangle<T>{ size.x - pos.right,  pos.top,             size.x - pos.left,  pos.bottom        };
			case ESurfaceTransform::HorizontalMirror_270 :	return Rectangle<T>{ pos.top,             pos.left,            pos.bottom,         pos.right         };
			case ESurfaceTransform::HorizontalMirror_180 :	return Rectangle<T>{ pos.left,            size.y - pos.bottom, pos.right,          size.y - pos.top  };
			case ESurfaceTransform::HorizontalMirror_90 :	return Rectangle<T>{ size.y - pos.bottom, size.x - pos.right,  size.y - pos.top,   size.x - pos.left };

			case ESurfaceTransform::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}


} // AE::Graphics
