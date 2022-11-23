// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

// for vulkan
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_CXX17
#define GLM_FORCE_VEC_EQUAL_OP	// special for AE
#define GLM_FORCE_EXPLICIT_CTOR
//#define GLM_FORCE_XYZW_ONLY	// will disable SIMD
//#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_INLINE
#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_INTRINSICS


// enable simd
//	WIndows, Linux, Mac with Intel, Android x64
#if defined(AE_SIMD_AVX) or defined(AE_SIMD_SSE)
# if AE_SIMD_AVX >= 2
#	define GLM_FORCE_AVX2	// float/double/int64
# elif AE_SIMD_AVX >= 1
#	define GLM_FORCE_AVX	// float/double
#endif
# if AE_SIMD_SSE >= 42
#	define GLM_FORCE_SSE42	// float
# elif AE_SIMD_SSE >= 41
#	define GLM_FORCE_SSE41	// float
# elif AE_SIMD_SSE >= 31
#	define GLM_FORCE_SSSE3	// float
# elif AE_SIMD_SSE >= 30
#	define GLM_FORCE_SSE3	// float
# elif AE_SIMD_SSE >= 20
#	define GLM_FORCE_SSE2	// float
# elif AE_SIMD_SSE >= 10
#	define GLM_FORCE_SSE	// float
# endif
#endif
//	Android, iOS or Mac M1...
#if defined(AE_SIMD_NEON)
#	define GLM_FORCE_NEON
#endif


#ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4201)
#	pragma warning (disable: 4127)
#endif

#include "glm.hpp"
#include "detail/type_half.hpp"

#include "gtx/easing.hpp"

#include "ext/matrix_double2x2.hpp"
#include "ext/matrix_double2x2_precision.hpp"
#include "ext/matrix_double2x3.hpp"
#include "ext/matrix_double2x3_precision.hpp"
#include "ext/matrix_double2x4.hpp"
#include "ext/matrix_double2x4_precision.hpp"
#include "ext/matrix_double3x2.hpp"
#include "ext/matrix_double3x2_precision.hpp"
#include "ext/matrix_double3x3.hpp"
#include "ext/matrix_double3x3_precision.hpp"
#include "ext/matrix_double3x4.hpp"
#include "ext/matrix_double3x4_precision.hpp"
#include "ext/matrix_double4x2.hpp"
#include "ext/matrix_double4x2_precision.hpp"
#include "ext/matrix_double4x3.hpp"
#include "ext/matrix_double4x3_precision.hpp"
#include "ext/matrix_double4x4.hpp"
#include "ext/matrix_double4x4_precision.hpp"

#include "ext/matrix_float2x2.hpp"
#include "ext/matrix_float2x2_precision.hpp"
#include "ext/matrix_float2x3.hpp"
#include "ext/matrix_float2x3_precision.hpp"
#include "ext/matrix_float2x4.hpp"
#include "ext/matrix_float2x4_precision.hpp"
#include "ext/matrix_float3x2.hpp"
#include "ext/matrix_float3x2_precision.hpp"
#include "ext/matrix_float3x3.hpp"
#include "ext/matrix_float3x3_precision.hpp"
#include "ext/matrix_float3x4.hpp"
#include "ext/matrix_float3x4_precision.hpp"
#include "ext/matrix_float4x2.hpp"
#include "ext/matrix_float4x2_precision.hpp"
#include "ext/matrix_float4x3.hpp"
#include "ext/matrix_float4x3_precision.hpp"
#include "ext/matrix_float4x4.hpp"
#include "ext/matrix_float4x4_precision.hpp"

#include "ext/matrix_relational.hpp"

#include "ext/quaternion_double.hpp"
#include "ext/quaternion_double_precision.hpp"
#include "ext/quaternion_float.hpp"
#include "ext/quaternion_float_precision.hpp"
#include "ext/quaternion_geometric.hpp"
#include "ext/quaternion_relational.hpp"

#include "ext/scalar_constants.hpp"
#include "ext/scalar_int_sized.hpp"
#include "ext/scalar_relational.hpp"

#include "ext/vector_bool1.hpp"
#include "ext/vector_bool1_precision.hpp"
#include "ext/vector_bool2.hpp"
#include "ext/vector_bool2_precision.hpp"
#include "ext/vector_bool3.hpp"
#include "ext/vector_bool3_precision.hpp"
#include "ext/vector_bool4.hpp"
#include "ext/vector_bool4_precision.hpp"

#include "ext/vector_double1.hpp"
#include "ext/vector_double1_precision.hpp"
#include "ext/vector_double2.hpp"
#include "ext/vector_double2_precision.hpp"
#include "ext/vector_double3.hpp"
#include "ext/vector_double3_precision.hpp"
#include "ext/vector_double4.hpp"
#include "ext/vector_double4_precision.hpp"

#include "ext/vector_float1.hpp"
#include "ext/vector_float1_precision.hpp"
#include "ext/vector_float2.hpp"
#include "ext/vector_float2_precision.hpp"
#include "ext/vector_float3.hpp"
#include "ext/vector_float3_precision.hpp"
#include "ext/vector_float4.hpp"
#include "ext/vector_float4_precision.hpp"

#include "ext/vector_int1.hpp"
#include "ext/vector_int1_precision.hpp"
#include "ext/vector_int2.hpp"
#include "ext/vector_int2_precision.hpp"
#include "ext/vector_int3.hpp"
#include "ext/vector_int3_precision.hpp"
#include "ext/vector_int4.hpp"
#include "ext/vector_int4_precision.hpp"

#include "ext/vector_relational.hpp"

#include "ext/vector_uint1.hpp"
#include "ext/vector_uint1_precision.hpp"
#include "ext/vector_uint2.hpp"
#include "ext/vector_uint2_precision.hpp"
#include "ext/vector_uint3.hpp"
#include "ext/vector_uint3_precision.hpp"
#include "ext/vector_uint4.hpp"
#include "ext/vector_uint4_precision.hpp"

#include "gtc/bitfield.hpp"
#include "gtc/color_space.hpp"
#include "gtc/constants.hpp"
#include "gtc/epsilon.hpp"
#include "gtc/integer.hpp"
#include "gtc/matrix_access.hpp"
#include "gtc/matrix_integer.hpp"
#include "gtc/matrix_inverse.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/noise.hpp"
#include "gtc/packing.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/random.hpp"
#include "gtc/reciprocal.hpp"
#include "gtc/round.hpp"
#include "gtc/type_precision.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/ulp.hpp"
#include "gtc/vec1.hpp"

#include "gtx/matrix_decompose.hpp"
#include "gtx/matrix_major_storage.hpp"
#include "gtx/norm.hpp"
#include "gtx/easing.hpp"

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif


namespace AE::Math
{
# if (GLM_ARCH & GLM_ARCH_SIMD_BIT)
	static constexpr auto	GLMQuialifier	= glm::qualifier::aligned_highp;
# else
	static constexpr auto	GLMQuialifier	= glm::qualifier::highp;
# endif
	
	template <typename T, int I, glm::qualifier Q>
	using TVec = glm::vec< I, T, Q >;

	template <typename T, int I>
	using Vec = TVec< T, I, GLMQuialifier >;
	
	template <typename T, int I>
	using PackedVec = TVec< T, I, glm::qualifier::packed_highp >;

	template <typename T>
	struct Quat;

	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TMatrix;
	
	template <typename T, uint Columns, uint Rows>
	using Matrix = TMatrix< T, Columns, Rows, GLMQuialifier >;
	
	template <typename T, uint Columns, uint Rows>
	using PackedMatrix = TMatrix< T, Columns, Rows, glm::qualifier::packed_highp >;

} // AE::Math


namespace AE::Base
{
	template <typename T, uint I, glm::qualifier Q>
	struct TMemCopyAvailable< TVec<T,I,Q> > { static constexpr bool  value = IsMemCopyAvailable<T>; };
	
	template <typename T, uint I, glm::qualifier Q>
	struct TZeroMemAvailable< TVec<T,I,Q> > { static constexpr bool  value = IsZeroMemAvailable<T>; };
	
	template <typename T, uint I, glm::qualifier Q>
	struct TTrivialySerializable< TVec<T,I,Q> > { static constexpr bool  value = IsTrivialySerializable<T>; };

	template <typename T>
	struct TMemCopyAvailable< Quat<T> > { static constexpr bool  value = IsMemCopyAvailable<T>; };
	
	template <typename T>
	struct TZeroMemAvailable< Quat<T> > { static constexpr bool  value = IsZeroMemAvailable<T>; };
	
	template <typename T>
	struct TTrivialySerializable< Quat<T> > { static constexpr bool  value = IsTrivialySerializable<T>; };

	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TMemCopyAvailable< TMatrix<T, Columns, Rows, Q> > { static constexpr bool  value = IsMemCopyAvailable<T>; };
	
	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TZeroMemAvailable< TMatrix<T, Columns, Rows, Q> > { static constexpr bool  value = IsZeroMemAvailable<T>; };
	
	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TTrivialySerializable< TMatrix<T, Columns, Rows, Q> > { static constexpr bool  value = IsTrivialySerializable<T>; };
	
} // AE::Base


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef GLM_FORCE_LEFT_HANDED
#	pragma detect_mismatch( "GLM_FORCE_LEFT_HANDED", "1" )
#  else
#	pragma detect_mismatch( "GLM_FORCE_LEFT_HANDED", "0" )
#  endif

#  ifdef GLM_FORCE_DEPTH_ZERO_TO_ONE
#	pragma detect_mismatch( "GLM_FORCE_DEPTH_ZERO_TO_ONE", "1" )
#  else
#	pragma detect_mismatch( "GLM_FORCE_DEPTH_ZERO_TO_ONE", "0" )
#  endif

#  ifdef GLM_FORCE_RADIANS
#	pragma detect_mismatch( "GLM_FORCE_RADIANS", "1" )
#  else
#	pragma detect_mismatch( "GLM_FORCE_RADIANS", "0" )
#  endif

#  ifdef GLM_FORCE_CTOR_INIT
#	pragma detect_mismatch( "GLM_FORCE_CTOR_INIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_FORCE_CTOR_INIT", "0" )
#  endif

#  if GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_LH_ZO
#	pragma detect_mismatch( "GLM_CONFIG_CLIP_CONTROL", "1" )
#  elif GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_RH_ZO
#	pragma detect_mismatch( "GLM_CONFIG_CLIP_CONTROL", "2" )
#  elif GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_LH_NO
#	pragma detect_mismatch( "GLM_CONFIG_CLIP_CONTROL", "3" )
#  elif GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_RH_NO
#	pragma detect_mismatch( "GLM_CONFIG_CLIP_CONTROL", "4" )
#  else
#	pragma detect_mismatch( "GLM_CONFIG_CLIP_CONTROL", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_SIMD_BIT)
#	pragma detect_mismatch( "GLM_ARCH_SIMD_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_SIMD_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_AVX2_BIT)
#	pragma detect_mismatch( "GLM_ARCH_AVX2_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_AVX2_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_AVX_BIT)
#	pragma detect_mismatch( "GLM_ARCH_AVX_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_AVX_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_SSE42_BIT)
#	pragma detect_mismatch( "GLM_ARCH_SSE42_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_SSE42_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_SSE41_BIT)
#	pragma detect_mismatch( "GLM_ARCH_SSE41_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_SSE41_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_NEON_BIT)
#	pragma detect_mismatch( "GLM_ARCH_NEON_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_NEON_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_MIPS_BIT)
#	pragma detect_mismatch( "GLM_ARCH_MIPS_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_MIPS_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_PPC_BIT)
#	pragma detect_mismatch( "GLM_ARCH_PPC_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_PPC_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_ARM_BIT)
#	pragma detect_mismatch( "GLM_ARCH_ARM_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_ARM_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_ARMV8_BIT)
#	pragma detect_mismatch( "GLM_ARCH_ARMV8_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_ARMV8_BIT", "0" )
#  endif

#  if (GLM_ARCH & GLM_ARCH_X86_BIT)
#	pragma detect_mismatch( "GLM_ARCH_X86_BIT", "1" )
#  else
#	pragma detect_mismatch( "GLM_ARCH_X86_BIT", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
