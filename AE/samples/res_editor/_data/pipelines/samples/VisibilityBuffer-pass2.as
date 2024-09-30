// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	define SH_RAY_GEN
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<RayTracingPipeline>	ppln = RayTracingPipeline( "templ" );
		ppln.SetLayout( "rt-model.pl" );

		// general
		{
			RC<Shader>	rg = Shader();
			rg.type		= EShader::RayGen;
			rg.LoadSelf();
			ppln.AddGeneralShader( "Main", rg );
		}

		// specialization
		{
			RC<RayTracingPipelineSpec>	spec = ppln.AddSpecialization( "spec" );

			spec.AddToRenderTech( "rtech", "main" );  // in SceneRayTracingPass

			// shader binding table
			{
				RC<RayTracingShaderBinding>		sbt = RayTracingShaderBinding( spec, "sbt" );

				sbt.BindRayGen( "Main" );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "GlobalIndex.glsl"
	#include "HWRayTracing.glsl"
	#include "ModelMaterial.glsl"

	#define PositionsRef	packed_float3_AEPtr
	#define NormalsRef		packed_float3_AEPtr
	#define TexcoordsRef	float2_AEPtr
	#define IndicesRef		uint_AEPtr

	struct MeshAndMaterial
	{
		float3			pos;			// world space
	//	float3			surfNormal;		// world space  // from triangle positions
		float3			smoothNormal;	// world space  // from vertex attributes
		float2			uv0;
		float2			uv0_dx;
		float2			uv0_dy;
		ModelMaterial	mtr;
	};


	uint3  UnpackID (uint id)
	{
		return uint3(
				id >> 31,				// instanceId
				(id >> 20) & 0x7FF,		// geometryId
				id & 0x000FFFFF );		// primitiveId
	}

	float3  LocalPosToWorldSpace (float4x4 mat, float3 pos) {
		return (mat * float4(pos, 1.0f)).xyz - un_PerPass.camera.pos;
	}

	float4  LocalPosToClipSpace (float4x4 mat, float3 pos) {
		return un_PerPass.camera.viewProj * float4(LocalPosToWorldSpace( mat, pos ), 1.0);
	}

	float4  WorldPosToClipSpace (float3 pos) {
		return un_PerPass.camera.viewProj * float4(pos, 1.0);
	}


	// from https://github.com/ConfettiFX/The-Forge/blob/master/Common_3/Renderer/VisibilityBuffer/Shaders/FSL/vb_shading_utilities.h.fsl
	// Apache-2.0 license
	//>>>>
	#if 1
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
	#endif
	//<<<<<


	MeshAndMaterial  UnpackTriangle (uint3 id)
	{
		ModelRTMesh_AERef	mesh		= un_RTInstances.meshesPerInstance[ id.x ][ id.y ];
		PositionsRef		pos_addr	= NormalsRef(   mesh.positions );
		NormalsRef			norm_addr	= NormalsRef(   mesh.normals );
		IndicesRef			idx_addr	= IndicesRef(   mesh.indices );
		TexcoordsRef		uv_addr		= TexcoordsRef( mesh.texcoords );
		const uint3			idx			= uint3( idx_addr.data[ id.z*3+0 ], idx_addr.data[ id.z*3+1 ], idx_addr.data[ id.z*3+2 ]);
		float4x4			model_mat	= un_RTInstances.modelMatPerInstance[ id.x ].data[ id.y ];
		float3x3			norm_mat	= un_RTInstances.normalMatPerInstance[ id.x ].data[ id.y ];
		MeshAndMaterial		result;

		float3				wpos0		= LocalPosToWorldSpace( model_mat, Cast( pos_addr.data[ idx.x ]) );
		float3				wpos1		= LocalPosToWorldSpace( model_mat, Cast( pos_addr.data[ idx.y ]) );
		float3				wpos2		= LocalPosToWorldSpace( model_mat, Cast( pos_addr.data[ idx.z ]) );

	# if 0
		BarycentricDeriv	deriv		= CalcRayBary(	wpos0, wpos1, wpos2,
														float3(GetGlobalCoordSNorm().xy, 0.0), float3(0.0),
														MatInverse(un_PerPass.camera.view), MatInverse(un_PerPass.camera.proj),
														2.0 / float2(GetGlobalSize().xy) );
	# else
		BarycentricDeriv	deriv		= CalcFullBary( WorldPosToClipSpace( wpos0 ),
														WorldPosToClipSpace( wpos1 ),
														WorldPosToClipSpace( wpos2 ),
														GetGlobalCoordSNorm().xy,
														2.0 / float2(GetGlobalSize().xy) );
	# endif
		GradientInterpolationResults uv_res = Interpolate2DWithDeriv( deriv, uv_addr.data[idx.x], uv_addr.data[idx.y], uv_addr.data[idx.z] );

		result.uv0			= uv_res.interp;
		result.uv0_dx		= uv_res.dx;
		result.uv0_dy		= uv_res.dy;

		result.smoothNormal	= Normalize( norm_mat *
										 BaryLerp(	Cast( norm_addr.data[ idx.x ]),
													Cast( norm_addr.data[ idx.y ]),
													Cast( norm_addr.data[ idx.z ]),
													deriv.m_lambda ));
		result.pos			= BaryLerp( wpos0, wpos1, wpos2, deriv.m_lambda );

		result.mtr			= un_Materials.elements[ un_RTInstances.materialsPerInstance[ id.x ].data[ id.y ]];

		return result;
	}


	void Main ()
	{
		const int2		coord	= GetGlobalCoord().xy;
		const uint		id		= gl.image.Load( un_IDBuffer, coord ).r;

		if ( id == ~0u )
		{
			gl.image.Store( un_ColorBuf, coord, float4(0.0f, 1.f, 1.f, 1.f) );
			return;
		}

		MeshAndMaterial	mm		= UnpackTriangle( UnpackID( id ));
		float4			albedo	= SampleGradAlbedo( mm.mtr, mm.uv0, mm.uv0_dx, mm.uv0_dy );

		albedo *= CalcLighting( mm.pos, mm.smoothNormal );

		gl.image.Store( un_ColorBuf, coord, albedo );
	}

#endif
//-----------------------------------------------------------------------------
