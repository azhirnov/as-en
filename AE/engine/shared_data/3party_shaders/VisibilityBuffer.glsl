// from [The Forge](https://github.com/ConfettiFX/The-Forge)
// Apache-2.0 license
// file [vb_shading_utilities.h.fsl](https://github.com/ConfettiFX/The-Forge/blob/c1665696d509bc9faed5432ccd14d1ff25975b66/Common_3/Renderer/VisibilityBuffer/Shaders/FSL/vb_shading_utilities.h.fsl)

#ifdef AE_LICENSE_APACHE_2

	#define rcp(VALUE)			(1.0f / (VALUE))
	#define mul(x,y)			((x) * (y))

	struct GradientInterpolationResults
	{
		float2 interp;
		float2 dx;
		float2 dy;
	};

	struct BarycentricDeriv
	{
		float3 m_lambda;
		float3 m_ddx;
		float3 m_ddy;
	};

	struct DerivativesOutput
	{
		float3 db_dx;
		float3 db_dy;
	};


	float3  rayTriangleIntersection (float3 p0, float3 p1, float3 p2, float3 o, float3 d)
	{
		float3	v0v1	= p1-p0;
		float3	v0v2	= p2-p0;
		float3	pvec	= cross(d,v0v2);
		float	det		= dot(v0v1,pvec);
		float	invDet	= 1/det;
		float3	tvec	= o - p0;
		float	u		= dot(tvec,pvec) * invDet;
		float3	qvec	= cross(tvec,v0v1);
		float	v		= dot(d,qvec) *invDet;
		float	w		= 1.0f - v - u;
		return float3(w,u,v);
	}

	BarycentricDeriv  CalcRayBary (float3 pt0, float3 pt1, float3 pt2, float3 pixelNdc, float3 rayOrigin, float4x4 viewInv, float4x4 projInv, float2 twoOverScreenSize)
	{
		BarycentricDeriv ret;

		// On the near plane, calculate the NDC of two nearby pixels in X and Y directions
		float3 ndcPos = pixelNdc;
		float3 ndcDx = pixelNdc + float3(twoOverScreenSize.x, 0, 0);
		float3 ndcDy = pixelNdc - float3(0, twoOverScreenSize.y, 0);

		// Inverse projection transform into view space
		float4 viewPos = mul(projInv, float4(ndcPos, 1.0));
		float4 viewDx = mul(projInv, float4(ndcDx, 1.0));
		float4 viewDy = mul(projInv, float4(ndcDy, 1.0));

		// Inverse view transform into world space
		// By setting homogeneous coordinate W to 0, this directly generates ray directions
		float3 rayDir = normalize(mul(viewInv, float4(viewPos.xyz, 0)).xyz);
		float3 rayDirDx = normalize(mul(viewInv, float4(viewDx.xyz, 0)).xyz);
		float3 rayDirDy = normalize(mul(viewInv, float4(viewDy.xyz, 0)).xyz);

		// Ray-triangle intersection for barycentric coordinates
		float3 lambda = rayTriangleIntersection(pt0, pt1, pt2, rayOrigin, rayDir);
		float3 lambdaDx = rayTriangleIntersection(pt0, pt1, pt2, rayOrigin, rayDirDx);
		float3 lambdaDy = rayTriangleIntersection(pt0, pt1, pt2, rayOrigin, rayDirDy);

		// Derivatives
		ret.m_lambda = lambda;
		ret.m_ddx = lambdaDx - lambda;
		ret.m_ddy = lambdaDy - lambda;
		return ret;
	}

	BarycentricDeriv  CalcFullBary (float4 pt0, float4 pt1, float4 pt2, float2 pixelNdc, float2 two_over_windowsize)
	{
		BarycentricDeriv ret;
		float3 invW =  rcp(float3(pt0.w, pt1.w, pt2.w));
		//Project points on screen to calculate post projection positions in 2D
		float2 ndc0 = pt0.xy * invW.x;
		float2 ndc1 = pt1.xy * invW.y;
		float2 ndc2 = pt2.xy * invW.z;

		// Computing partial derivatives and prospective correct attribute interpolation with barycentric coordinates
		// Equation for calculation taken from Appendix A of DAIS paper:
		// https://cg.ivd.kit.edu/publications/2015/dais/DAIS.pdf

		// Calculating inverse of determinant(rcp of area of triangle).
		float invDet = rcp(determinant(float2x2(ndc2 - ndc1, ndc0 - ndc1)));

		//determining the partial derivatives
		// ddx[i] = (y[i+1] - y[i-1])/Determinant
		ret.m_ddx = float3(ndc1.y - ndc2.y, ndc2.y - ndc0.y, ndc0.y - ndc1.y) * invDet * invW;
		ret.m_ddy = float3(ndc2.x - ndc1.x, ndc0.x - ndc2.x, ndc1.x - ndc0.x) * invDet * invW;
		// sum of partial derivatives.
		float ddxSum = dot(ret.m_ddx, float3(1,1,1));
		float ddySum = dot(ret.m_ddy, float3(1,1,1));

		// Delta vector from pixel's screen position to vertex 0 of the triangle.
		float2 deltaVec = pixelNdc - ndc0;

		// Calculating interpolated W at point.
		float interpInvW = invW.x + deltaVec.x*ddxSum + deltaVec.y*ddySum;
		float interpW = rcp(interpInvW);
		// The barycentric co-ordinate (m_lambda) is determined by perspective-correct interpolation.
		// Equation taken from DAIS paper.
		ret.m_lambda.x = interpW * (invW[0] + deltaVec.x*ret.m_ddx.x + deltaVec.y*ret.m_ddy.x);
		ret.m_lambda.y = interpW * (0.0f    + deltaVec.x*ret.m_ddx.y + deltaVec.y*ret.m_ddy.y);
		ret.m_lambda.z = interpW * (0.0f    + deltaVec.x*ret.m_ddx.z + deltaVec.y*ret.m_ddy.z);

		//Scaling from NDC to pixel units
		ret.m_ddx *= two_over_windowsize.x;
		ret.m_ddy *= two_over_windowsize.y;
		ddxSum    *= two_over_windowsize.x;
		ddySum    *= two_over_windowsize.y;

		ret.m_ddy *= -1.0f;
		ddySum *= -1.0f;

		// This part fixes the derivatives error happening for the projected triangles.
		// Instead of calculating the derivatives constantly across the 2D triangle we use a projected version
		// of the gradients, this is more accurate and closely matches GPU raster behavior.
		// Final gradient equation: ddx = (((lambda/w) + ddx) / (w+|ddx|)) - lambda

		// Calculating interpW at partial derivatives position sum.
		float interpW_ddx = 1.0f / (interpInvW + ddxSum);
		float interpW_ddy = 1.0f / (interpInvW + ddySum);

		// Calculating perspective projected derivatives.
		ret.m_ddx = interpW_ddx*(ret.m_lambda*interpInvW + ret.m_ddx) - ret.m_lambda;
		ret.m_ddy = interpW_ddy*(ret.m_lambda*interpInvW + ret.m_ddy) - ret.m_lambda;

		return ret;
	}

	GradientInterpolationResults  Interpolate2DWithDeriv (BarycentricDeriv deriv, float2 uv0, float2 uv1, float2 uv2)
	{
		float3 attr0 = float3(uv0.x, uv1.x, uv2.x); // u
		float3 attr1 = float3(uv0.y, uv1.y, uv2.y); // v

		GradientInterpolationResults result;
		// independently interpolate x and y attributes.
		result.interp.x = dot(deriv.m_lambda, attr0);
		result.interp.y = dot(deriv.m_lambda, attr1);

		// Calculate attributes' dx and dy (for texture sampling).
		result.dx.x = dot(attr0, deriv.m_ddx);
		result.dx.y = dot(attr1, deriv.m_ddx);
		result.dy.x = dot(attr0, deriv.m_ddy);
		result.dy.y = dot(attr1, deriv.m_ddy);
		return result;
	}

	// generates dx and dy for for 3x3 attributes
	DerivativesOutput  Cal3DDeriv (BarycentricDeriv deriv, float3x3 attributes)
	{
		DerivativesOutput result;
		result.db_dx = mul(attributes, deriv.m_ddx);
		result.db_dy = mul(attributes, deriv.m_ddy);
		return result;
	}

	DerivativesOutput  Cal3DDeriv (BarycentricDeriv deriv, float3 v0, float3 v1, float3 v2)
	{
		return Cal3DDeriv( deriv, float3x3( v0, v1, v2 ));
	}

	#undef rcp
	#undef mul

#endif // AE_LICENSE_APACHE_2
