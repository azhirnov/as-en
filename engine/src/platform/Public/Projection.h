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
		RaysGrid () {}
		
		RaysGrid (float4* rays, const uint2 &dim) : _rays{rays}, _dim{dim}
		{
			ASSERT( _rays != null );
		}

		ND_ uint2 const&  Dimension () const			{ return _dim; }

		ND_ float4&  operator () (const uint2 &pos)		{ return (*this)( pos.x, pos.y ); }

		ND_ float4&  operator () (uint x, uint y)
		{
			ASSERT( _rays != null );
			ASSERT( x < _dim.x and y < _dim.y );
			return _rays[ _dim.x * y + x ];
		}

		ND_ float4 const&  operator () (const uint2 &pos)	const	{ return const_cast< RaysGrid *>(this)->operator()( pos ); }
		ND_ float4 const&  operator () (uint x, uint y)		const	{ return const_cast< RaysGrid *>(this)->operator()( x, y ); }
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
			virtual void  GetViewProj (OUT TViewProj<float> &) const = 0;
			virtual void  GetViewProj (OUT TViewProj<double> &) const = 0;

			virtual void  GetInvViewProj (OUT TViewProj<float> &) const = 0;
			virtual void  GetInvViewProj (OUT TViewProj<double> &) const = 0;

		// Same as view matrix.
			virtual void  GetTransform (OUT Transformation<float> &) const = 0;
			virtual void  GetTransform (OUT Transformation<double> &) const = 0;

		// Frustum calculated form view/proj matrix.
		//	virtual void  GetFrustum (OUT FrustumTempl<float> &) const = 0;
		//	virtual void  GetFrustum (OUT FrustumTempl<double> &) const = 0;

		// Generate low resolution ray grid to support specific screen types.
		// Should be used for VR, curved screens and other.
			virtual void  GenetateRays (RaysGrid &grid) const = 0;

		// Some devices may use low or high precission for matrices.
		ND_ virtual EPrecision  NativePrecision () const = 0;

		// for shader builder
		//ND_ virtual String  GetSource () const = 0;
	};



	//
	// Level of detail function
	//

	class LODFunction
	{
		// Function used to calculate LOD for each object in frustum.
	};


} // AE::App
