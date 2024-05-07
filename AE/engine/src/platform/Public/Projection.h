// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

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
		float4 *	_rays	= null;		// xyz - normal, w - density
		uint2		_dim	{0};


	// methods
	public:
		RaysGrid ()											__NE___	{}
		RaysGrid (float4* rays, const uint2 &dim)			__NE___	: _rays{rays}, _dim{dim} { NonNull( _rays ); }

		ND_ uint2 const&  Dimension ()						C_NE___	{ return _dim; }

		ND_ float4&  operator () (const uint2 &pos)			__NE___	{ return (*this)( pos.x, pos.y ); }

		ND_ float4&  operator () (uint x, uint y)			__NE___
		{
			NonNull( _rays );
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
	// variables
	public:
		float4x4	proj;
		float4x4	view;
	//	float4x4	invProj;
	//	float4x4	invView;


	// interface
	public:
		IProjection ()													__NE___ {}

		ND_ float4x4  ViewProj ()										C_NE___	{ return proj * view; }


		// Generate low resolution ray grid to support specific screen types.
		// Should be used for VR, curved screens and other.
		//
		// 'grid' - must contains preallocated memory for rays and non-zero dimension.
			virtual void  GenerateRays (RaysGrid &grid)					C_NE___ = 0;
	};



	//
	// Level of detail function
	//

	class LODFunction
	{
		// Function used to calculate LOD for each object in frustum.
	};


} // AE::App
