// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Matrix.h"
#include "base/Math/Frustum.h"
#include "base/Math/Transformation.h"
#include "base/Containers/ArrayView.h"
#include "platform/Public/Common.h"

namespace AE::App
{

	//
	// Rays Grid
	//

	class RaysGrid
	{
	// variables
	private:
		float4 *	_rays	= null;
		uint2		_dim	{0};
		// TODO: rays dencity ?


	// methods
	public:
		RaysGrid ()											__NE___	{}
		RaysGrid (float4* rays, const uint2 &dim)			__NE___	: _rays{rays}, _dim{dim} { ASSERT( _rays != null ); }

		ND_ uint2 const&  Dimension ()						C_NE___	{ return _dim; }

		ND_ float4&  operator () (const uint2 &pos)			__NE___	{ return (*this)( pos.x, pos.y ); }

		ND_ float4&  operator () (uint x, uint y)			__NE___
		{
			ASSERT( _rays != null );
			ASSERT( x < _dim.x and y < _dim.y );
			return _rays[ _dim.x * y + x ];
		}

		ND_ float4 const&  operator () (const uint2 &pos)	C_NE___	{ return const_cast< RaysGrid *>(this)->operator()( pos ); }
		ND_ float4 const&  operator () (uint x, uint y)		C_NE___	{ return const_cast< RaysGrid *>(this)->operator()( x, y ); }
	};



	//
	// Projection
	//

	class IProjection
	{
	// types
	public:
		template <typename T>
		struct TViewProj
		{
			Matrix< T, 4, 4 >	proj;
			Matrix< T, 4, 4 >	view;
		};

		enum class EPrecision : ubyte
		{
			Fp16,
			Fp32,
			Fp64,
		};


	// interface
	public:

		// Some devices (VR) requires you to use view/proj matrices which returned by API.
			virtual void  GetViewProj (OUT TViewProj<float> &)			C_NE___ = 0;
			virtual void  GetViewProj (OUT TViewProj<double> &)			C_NE___ = 0;

			virtual void  GetInvViewProj (OUT TViewProj<float> &)		C_NE___ = 0;
			virtual void  GetInvViewProj (OUT TViewProj<double> &)		C_NE___ = 0;

		// Same as view matrix.
			virtual void  GetTransform (OUT Transformation<float> &)	C_NE___ = 0;
			virtual void  GetTransform (OUT Transformation<double> &)	C_NE___ = 0;

		// Frustum calculated form view/proj matrix.
		//	virtual void  GetFrustum (OUT FrustumTempl<float> &)		C_NE___ = 0;
		//	virtual void  GetFrustum (OUT FrustumTempl<double> &)		C_NE___ = 0;

		// Generate low resolution ray grid to support specific screen types.
		// Should be used for VR, curved screens and other.
			virtual void  GenetateRays (RaysGrid &grid)					C_NE___ = 0;

		// Some devices may use low or high precission for matrices.
		ND_ virtual EPrecision  NativePrecision ()						C_NE___ = 0;

		// for shader builder
		//ND_ virtual String  GetSource ()								C_NE___ = 0;
	};



	//
	// Level of detail function
	//

	class LODFunction
	{
		// Function used to calculate LOD for each object in frustum.
	};


} // AE::App
