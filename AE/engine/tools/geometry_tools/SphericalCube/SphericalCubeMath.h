// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	based on
	"Cube-to-sphere Projections for Procedural Texturing and Beyond"
	https://www.jcgt.org/published/0007/02/01/paper.pdf
*/
#pragma once

#include "pch/Base.h"

namespace AE::GeometryTools
{
	using namespace AE::Base;

	enum class ECubeFace : uint
	{
		XPos	= 0,	// right
		XNeg	= 1,	// left
		YPos	= 2,	// down
		YNeg	= 3,	// up
		ZPos	= 4,	// front
		ZNeg	= 5,	// back
	};

	struct SCProj1_Identity;
	struct SCProj1_Tangential;
	struct SCProj1_Everitt;
	struct SCProj1_5thPoly;
	struct SCProj1_COBE;
	struct SCProj1_Arvo;

	template <typename TProj>	struct SCProj2_Cube;
	template <typename TProj>	struct SCProj2_Spherical;
	template <typename TProj>	struct SCProj2_Texture;
	template <typename TProj>	struct SCProj2_TextureYFlip;



	//
	// Spherical Cube Projection
	//

	template <typename PosProj, typename TexProj>
	class SphericalCubeProjection
	{
	// types
	public:
		using Base_t				= SphericalCubeProjection< PosProj, TexProj >;
		using VertexProjection_t	= PosProj;
		using TextureProjection_t	= TexProj;


	// methods
	public:
		// position projection
		ND_ static double3  ForwardProjection (const double2 &snormCoord, ECubeFace face)		__NE___;
		ND_ static float3   ForwardProjection (const float2 &snormCoord, ECubeFace face)		__NE___;

		ND_ static Pair<double2, ECubeFace>  InverseProjection (const double3 &coord)			__NE___;
		ND_ static Pair<float2, ECubeFace>   InverseProjection (const float3 &coord)			__NE___;

		// texture coordinate projection
		ND_ static double3  ForwardTexProjection (const double2 &snormCoord, ECubeFace face)	__NE___;
		ND_ static float3   ForwardTexProjection (const float2 &snormCoord, ECubeFace face)		__NE___;

		ND_ static Pair<double2, ECubeFace>  InverseTexProjection (const double3 &coord)		__NE___;
		ND_ static Pair<float2, ECubeFace>   InverseTexProjection (const float3 &coord)			__NE___;
	};


/*
=================================================
	RotateVec
----
	project 2D coord to cube face
=================================================
*/
	ND_ inline double3  RotateVec (const double3 &c, ECubeFace face) __NE___
	{
		return	double3( c.z, -c.y, -c.x) * double(face == ECubeFace::XPos) +	// X+
				double3(-c.z, -c.y,  c.x) * double(face == ECubeFace::XNeg) +	// X-
				double3( c.x,  c.z,  c.y) * double(face == ECubeFace::YPos) +	// Y+
				double3( c.x, -c.z, -c.y) * double(face == ECubeFace::YNeg) +	// Y-
				double3( c.x, -c.y,  c.z) * double(face == ECubeFace::ZPos) +	// Z+
				double3(-c.x, -c.y, -c.z) * double(face == ECubeFace::ZNeg);	// Z-
	}

/*
=================================================
	InverseRotation
----
	project 3D coord on cube/sphere to face 2D coord
=================================================
*/
	ND_ inline Tuple<double2, double, ECubeFace>  InverseRotation (double3 c) __NE___
	{
		c.y = -c.y;

		// front (xy space)
		if ( (Abs(c.x) <= c.z) and (c.z > 0.0) and (Abs(c.y) <= c.z) )
			return Tuple{ double2{c.x, c.y}, c.z, ECubeFace::ZPos };

		// right (zy space)
		if ( (Abs(c.z) <= c.x) and (c.x > 0.0) and (Abs(c.y) <= c.x) )
			return Tuple{ double2{-c.z, c.y}, c.x, ECubeFace::XPos };

		// back (xy space)
		if ( (Abs(c.x) <= -c.z) and (c.z < 0.0) and (Abs(c.y) <= -c.z) )
			return Tuple{ double2{-c.x, c.y}, -c.z, ECubeFace::ZNeg };

		// left (zy space)
		if ( (Abs(c.z) <= -c.x) and (c.x < 0.0) and (Abs(c.y) <= -c.x) )
			return Tuple{ double2{c.z, c.y}, -c.x, ECubeFace::XNeg };

		// up (xz space)
		if ( c.y > 0.0 )
			return Tuple{ double2{c.x, -c.z}, c.y, ECubeFace::YNeg };

		// down (xz space)
		return Tuple{ double2{c.x, c.z}, -c.y, ECubeFace::YPos };
	}
//-----------------------------------------------------------------------------


// First level projection //

/*
=================================================
	SCProj1_Identity
=================================================
*/
	struct SCProj1_Identity
	{
		// 2D regular grid on cube face --> 2D regular grid on cube face
		ND_ static double2  Forward (const double2 &snormCoord) __NE___	{ return snormCoord; }
		ND_ static double2  Inverse (const double2 &snormCoord) __NE___	{ return snormCoord; }
	};

/*
=================================================
	SCProj1_Tangential
=================================================
*/
	struct SCProj1_Tangential
	{
	private:
	  #if 1
		// Angle from "Cube-to-sphere Projections for Procedural Texturing and Beyond", is better than Pi/4
		// ~49.8 deg
		static constexpr double  warp_theta		= 0.868734829276;
		static constexpr double  tan_warp_theta	= 1.182286685546; //tan( warp_theta );
	  #else
		// 45 deg
		static constexpr double  warp_theta		= double{TRadian<double>::Pi()} / 4.0;
		static constexpr double  tan_warp_theta	= 1.0; //tan( warp_theta );
	  #endif

	public:
		// 2D regular grid on cube face --> 2D irregular grid on cube face
		ND_ static double2  Forward (const double2 &snormCoord) __NE___
		{
			return glm::tan( warp_theta * snormCoord ) / tan_warp_theta;
		}

		// 2D irregular grid on cube face --> 2D regular grid on cube face
		ND_ static double2  Inverse (const double2 &snormCoord) __NE___
		{
			return glm::atan( snormCoord * tan_warp_theta ) / warp_theta;
		}
	};

/*
=================================================
	SCProj1_Everitt
=================================================
*/
	struct SCProj1_Everitt
	{
	private:
	  #if 1
		// Parameter from "Cube-to-sphere Projections for Procedural Texturing and Beyond"
		static constexpr double  e	= 1.4511;
	  #else
	//	static constexpr double  e	= 1.375;
	  #endif

	public:
		// 2D regular grid on cube face --> 2D irregular grid on cube face
		ND_ static double2  Forward (const double2 &snormCoord) __NE___
		{
			return Sign( snormCoord ) * (e - Sqrt( e*e - 4.0 * (e - 1.0) * Abs(snormCoord) )) / (2.0 * (e - 1.0));
		}

		// 2D irregular grid on cube face --> 2D regular grid on cube face
		ND_ static double2  Inverse (const double2 &snormCoord) __NE___
		{
			return snormCoord * (e + (1.0 - e) * Abs(snormCoord));
		}
	};

/*
=================================================
	SCProj1_5thPoly
=================================================
*/
	struct SCProj1_5thPoly
	{
		// 2D regular grid on cube face --> 2D irregular grid on cube face
		ND_ static double2  Forward (const double2 &snormCoord) __NE___
		{
			double2	sq = snormCoord * snormCoord;
			return (0.745558715593 + (0.130546850193 + 0.123894434214 * sq) * sq) * snormCoord;
		}

		// 2D irregular grid on cube face --> 2D regular grid on cube face
		ND_ static double2  Inverse (const double2 &snormCoord) __NE___
		{
			double2	sq = snormCoord * snormCoord;
			return (1.34318229552 + (-0.486514066449 + 0.143331770927 * sq) * sq) * snormCoord;
		}
	};

/*
=================================================
	SCProj1_COBE
=================================================
*/
	struct SCProj1_COBE
	{
		// 2D regular grid on cube face --> 2D irregular grid on cube face
		ND_ static double2  Forward (const double2 &snormCoord) __NE___
		{
			double2	sq1	= snormCoord * snormCoord;
			double2	sq2	{sq1.y, sq1.x};
			double2	sum	= ((-0.0941180085824 + 0.0409125981187 * sq2 - 0.0623272690881 * sq1) * sq1 + (0.0275922480902 + 0.0342217026979 * sq2) * sq2);
			return (0.723951234952 + 0.276048765048 * sq1 + (1.0 - sq1) * sum) * snormCoord;
		}

		// 2D irregular grid on cube face --> 2D regular grid on cube face
		ND_ static double2  Inverse (const double2 &snormCoord) __NE___
		{
			double2	sq1	= snormCoord * snormCoord;
			double2	sq2	{sq1.y, sq1.x};
			double2	sum	= ((-0.212853382041 + 0.0941259684877 * sq2 + 0.0693532685333 * sq1) * sq1 + (-0.117847692949 + 0.0107989197181 * sq2) * sq2);
			return (1.37738198385 - 0.377381983848 * sq1 + (1.0 - sq1) * sum) * snormCoord;
		}
	};

/*
=================================================
	SCProj1_Arvo
=================================================
*/
	struct SCProj1_Arvo
	{
		// 2D regular grid on cube face --> 2D irregular grid on cube face
		ND_ static double2  Forward (const double2 &snormCoord) __NE___
		{
			double	tan_a_term	= glm::tan( snormCoord.x * 0.523598775598 );
			double	cos_a_term	= glm::cos( snormCoord.x * 1.0471975512 );
			return double2{ 1.41421356237 * tan_a_term / Sqrt( 1.0 - tan_a_term * tan_a_term ),
							snormCoord.y / Sqrt( 1.0 + (1.0 - snormCoord.y * snormCoord.y) * cos_a_term) };
		}

		// 2D irregular grid on cube face --> 2D regular grid on cube face
		ND_ static double2  Inverse (const double2 &snormCoord) __NE___
		{
			double	ss2 = Sqrt( snormCoord.x * snormCoord.x + 2.0 );
			return double2{ glm::atan( snormCoord.x / ss2 ) * 1.9098593171,
							snormCoord.y * ss2 / Sqrt( Dot( snormCoord, snormCoord ) + 1.0 )};
		}
	};
//-----------------------------------------------------------------------------


// Second level projection //

/*
=================================================
	SCProj2_Cube
=================================================
*/
	template <typename TProj>
	struct SCProj2_Cube : TProj
	{
		using TProj::Forward;
		using TProj::Inverse;

		// 2D regular grid on cube face + face --> 3D on sphere
		ND_ static double3  Forward (const double2 &snormCoord, ECubeFace face) __NE___
		{
			return Forward2( double3{Forward( snormCoord ), 1.0}, face );
		}

		// 3D on cube face --> 3D on sphere
		ND_ static double3  Forward2 (const double3 &projected, ECubeFace face) __NE___
		{
			return RotateVec( projected, face );
		}

		// 3D on sphere --> 2D regular grid on cube face + face
		ND_ static Pair<double2, ECubeFace>  Inverse (const double3 &coord) __NE___
		{
			auto [c, z, face] = InverseRotation( coord );
			return { c, face };
		}
	};

/*
=================================================
	SCProj2_Spherical
=================================================
*/
	template <typename TProj>
	struct SCProj2_Spherical : TProj
	{
		using TProj::Forward;
		using TProj::Inverse;

		// 2D regular grid on cube face + face --> 3D on sphere
		ND_ static double3  Forward (const double2 &snormCoord, ECubeFace face) __NE___
		{
			return Forward2( double3{Forward( snormCoord ), 1.0}, face );
		}

		// 3D on cube face --> 3D on sphere
		ND_ static double3  Forward2 (const double3 &projected, ECubeFace face) __NE___
		{
			return Normalize( RotateVec( projected, face ));
		}

		// 3D on sphere --> 2D regular grid on cube face + face
		ND_ static Pair<double2, ECubeFace>  Inverse (const double3 &coord) __NE___
		{
			auto [c, z, face] = InverseRotation( coord );
			c /= z;
			return { Inverse(c), face };
		}
	};

/*
=================================================
	SCProj2_Texture
=================================================
*/
	template <typename TProj>
	struct SCProj2_Texture
	{
		using Proj_t = SCProj2_Spherical< TProj >;

		ND_ static double3  Forward (const double2 &snormCoord, ECubeFace face) __NE___
		{
			return Proj_t::Forward( snormCoord, face );
		}

		ND_ static Pair<double2, ECubeFace>  Inverse (const double3 &coord) __NE___
		{
			return Proj_t::Inverse( coord );
		}
	};

/*
=================================================
	SCProj2_TextureYFlip
=================================================
*/
	template <typename TProj>
	struct SCProj2_TextureYFlip
	{
		using Proj_t = SCProj2_Spherical< TProj >;

		ND_ static double3  Forward (const double2 &snormCoord, ECubeFace face) __NE___
		{
			double3 c = Proj_t::Forward( snormCoord, face );
			c.y = -c.y;
			return c;
		}

		ND_ static Pair<double2, ECubeFace>  Inverse (const double3 &coord) __NE___
		{
			double3 c = coord;
			c.y = -c.y;
			return Proj_t::Inverse( c );
		}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	ForwardProjection
=================================================
*/
	template <typename PP, typename TP>
	double3  SphericalCubeProjection<PP,TP>::ForwardProjection (const double2 &snormCoord, ECubeFace face) __NE___
	{
		return VertexProjection_t::Forward( snormCoord, face );
	}

	template <typename PP, typename TP>
	float3  SphericalCubeProjection<PP,TP>::ForwardProjection (const float2 &snormCoord, ECubeFace face) __NE___
	{
		return float3(VertexProjection_t::Forward( double2{snormCoord}, face ));
	}

/*
=================================================
	InverseProjection
=================================================
*/
	template <typename PP, typename TP>
	Pair<double2, ECubeFace>  SphericalCubeProjection<PP,TP>::InverseProjection (const double3 &coord) __NE___
	{
		auto [c, face] = VertexProjection_t::Inverse( coord );
		return { c, face };
	}

	template <typename PP, typename TP>
	Pair<float2, ECubeFace>  SphericalCubeProjection<PP,TP>::InverseProjection (const float3 &coord) __NE___
	{
		auto [c, face] = VertexProjection_t::Inverse( double3(coord) );
		return { float2(c), face };
	}

/*
=================================================
	ForwardTexProjection
=================================================
*/
	template <typename PP, typename TP>
	double3  SphericalCubeProjection<PP,TP>::ForwardTexProjection (const double2 &snormCoord, ECubeFace face) __NE___
	{
		return TextureProjection_t::Forward( snormCoord, face );
	}

	template <typename PP, typename TP>
	float3  SphericalCubeProjection<PP,TP>::ForwardTexProjection (const float2 &snormCoord, ECubeFace face) __NE___
	{
		return float3(TextureProjection_t::Forward( double2{snormCoord}, face ));
	}

/*
=================================================
	InverseTexProjection
=================================================
*/
	template <typename PP, typename TP>
	Pair<double2, ECubeFace>  SphericalCubeProjection<PP,TP>::InverseTexProjection (const double3 &coord) __NE___
	{
		auto [c, face] = TextureProjection_t::Inverse( coord );
		return { c, face };
	}

	template <typename PP, typename TP>
	Pair<float2, ECubeFace>  SphericalCubeProjection<PP,TP>::InverseTexProjection (const float3 &coord) __NE___
	{
		auto [c, face] = TextureProjection_t::Inverse( double3(coord) );
		return { float2(c), face };
	}


} // AE::GeometryTools
