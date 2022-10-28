// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Projection.h"

namespace AE::App
{

	//
	// Projection implementation
	//

	template <typename T>
	class ProjectionImpl final : public IProjection
	{
	// variables
	public:
		float4x4	proj;
		float4x4	view;
		float4x4	invProj;
		float4x4	invView;


	// methods
	public:
		void  GetViewProj (OUT TViewProj<float> &) const override;
		void  GetViewProj (OUT TViewProj<double> &) const override;

		void  GetInvViewProj (OUT TViewProj<float> &) const override;
		void  GetInvViewProj (OUT TViewProj<double> &) const override;

		void  GetTransform (OUT Transformation<float> &) const override;
		void  GetTransform (OUT Transformation<double> &) const override;

		void  GenetateRays (RaysGrid &grid) const override;

		EPrecision  NativePrecision () const override	{ return EPrecision::Fp32; }
	};

	
	
/*
=================================================
	GetViewProj
=================================================
*/
	template <typename T>
	void  ProjectionImpl<T>::GetViewProj (OUT TViewProj<float> &result) const
	{
		result.proj	= proj;
		result.view	= view;
	}
	
	template <typename T>
	void  ProjectionImpl<T>::GetViewProj (OUT TViewProj<double> &result) const
	{
		result.proj	= double4x4{proj};
		result.view	= double4x4{view};
	}
	
/*
=================================================
	GetInvViewProj
=================================================
*/
	template <typename T>
	void  ProjectionImpl<T>::GetInvViewProj (OUT TViewProj<float> &result) const
	{
		result.proj	= invProj;
		result.view	= invView;
	}
	
	template <typename T>
	void  ProjectionImpl<T>::GetInvViewProj (OUT TViewProj<double> &result) const
	{
		result.proj	= double4x4{invProj};
		result.view	= double4x4{invView};
	}
	
/*
=================================================
	GetTransform
=================================================
*/
	template <typename T>
	void  ProjectionImpl<T>::GetTransform (OUT Transformation<float> &result) const
	{
		result = Transformation<float>{view};
	}
	
	template <typename T>
	void  ProjectionImpl<T>::GetTransform (OUT Transformation<double> &result) const
	{
		result = Transformation<double>{ double4x4{ view }};
	}
	
/*
=================================================
	GenetateRays
=================================================
*/
	template <typename T>
	void  ProjectionImpl<T>::GenetateRays (RaysGrid &grid) const
	{
		/*ASSERT( All( grid.Dimension() >= 2 ));
		
		const float4x4	mvp = proj * view * rotation;

		pos   = mvp * float4(0.0f, 0.0f, 0.0f, 1.0f);
		pos.w = 0.0f;

		for (uint y = 0; y < grid.Dimension().y; ++y)
		{
			for (uint x = 0; x < grid.Dimension().x; ++x)
			{
				float4	ray{	(float(x) / (grid.Dimension().x - 1)) * 2.0f - 1.0f,
								(float(y) / (grid.Dimension().y - 1)) * 2.0f - 1.0f,
								-1.0f,
								0.0f };

				ray = float4{ Normalize( float3{ mvp * ray }), 0.0f };

				grid(x, y) = ray;
			}
		}*/
	}


} // AE::App
