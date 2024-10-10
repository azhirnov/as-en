// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

// for vulkan
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL		// for gtx
#ifdef AE_CXX_20
# define GLM_FORCE_CXX20
#else
# define GLM_FORCE_CXX17
#endif
#define GLM_FORCE_EXPLICIT_CTOR
//#define GLM_FORCE_XYZW_ONLY		// will disable SIMD
#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_INLINE
#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
//#define GLM_FORCE_FMA
//#define GLM_FORCE_SIZE_T_LENGTH
//#define GLM_FORCE_MESSAGES		// for debugging
#define GLM_FORCE_UNRESTRICTED_GENTYPE

#define GLM_AE_VERSION


// enable simd
//	Windows, Linux, Mac with Intel, Android x64
#if AE_SIMD_AVX | AE_SIMD_SSE
# if AE_SIMD_AVX >= 2
#	define GLM_FORCE_AVX2	// float/double/int64
# elif AE_SIMD_AVX >= 1
#	define GLM_FORCE_AVX	// float/double
# elif AE_SIMD_SSE >= 42
#	define GLM_FORCE_SSE42	// float
# elif AE_SIMD_SSE >= 41
#	define GLM_FORCE_SSE41	// float
# elif AE_SIMD_SSE >= 31
#	define GLM_FORCE_SSSE3	// float
# elif AE_SIMD_SSE >= 30
#	define GLM_FORCE_SSE3	// float
# elif AE_SIMD_SSE >= 20
#	define GLM_FORCE_SSE2	// float
# endif

//	Android ARM, iOS or Mac M1, M2 ...
#elif AE_SIMD_NEON
#	define GLM_FORCE_NEON

// disable intrinsics
#else
#	define GLM_FORCE_ARCH_UNKNOWN
#endif


#include "glm.hpp"

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

#include "gtx/matrix_decompose.hpp"
#include "gtx/matrix_major_storage.hpp"
#include "gtx/norm.hpp"
#include "gtx/easing.hpp"
#include "gtx/rotate_vector.hpp"
#include "gtx/quaternion.hpp"
#include "gtx/dual_quaternion.hpp"
#include "gtx/intersect.hpp"
#include "gtx/fast_exponential.hpp"
#include "gtx/fast_square_root.hpp"
#include "gtx/fast_trigonometry.hpp"

#include "ext/scalar_constants.hpp"
#include "ext/scalar_int_sized.hpp"
#include "ext/scalar_relational.hpp"
#include "ext/quaternion_geometric.hpp"
#include "ext/quaternion_relational.hpp"
#include "ext/vector_relational.hpp"
#include "ext/matrix_relational.hpp"


#if GLM_CONFIG_ALIGNED_GENTYPES != GLM_ENABLE
#	error required GLM_CONFIG_ALIGNED_GENTYPES = GLM_ENABLE
#endif

namespace AE::Math
{
	static constexpr inline auto	GLMSimdQualifier	= glm::qualifier::aligned_highp;
	static constexpr inline auto	GLMPackedQualifier	= glm::qualifier::packed_highp;

	template <typename T, int I, glm::qualifier Q>
	using TVec = glm::vec< I, T, Q >;

	template <typename T, glm::qualifier Q = GLMSimdQualifier>
	struct TQuat;

	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TMatrix;

	template <typename T, int I>	using Vec		= TVec< T, I, GLMSimdQualifier >;
	template <typename T, int I>	using PackedVec	= TVec< T, I, GLMPackedQualifier >;

	template <typename T, uint Columns, uint Rows>	using Matrix		= TMatrix< T, Columns, Rows, GLMSimdQualifier >;
	template <typename T, uint Columns, uint Rows>	using PackedMatrix	= TMatrix< T, Columns, Rows, GLMPackedQualifier >;

} // AE::Math


namespace AE::Base
{
	template <typename T, int I, glm::qualifier Q>
	struct TMemCopyAvailable< TVec<T,I,Q> >		: CT_Bool< IsMemCopyAvailable<T> >{};

	template <typename T, int I, glm::qualifier Q>
	struct TZeroMemAvailable< TVec<T,I,Q> >		: CT_Bool< IsZeroMemAvailable<T> >{};

	// 'IsTriviallySerializable< TVec<> > = false' - because SIMD and packed types has different alignment

	template <typename T, int I, glm::qualifier Q>
	struct TNothrowCopyCtor< TVec<T,I,Q> >		: CT_True {};

	template <typename T, int I, glm::qualifier Q>
	struct TNothrowDefaultCtor< TVec<T,I,Q> >	: CT_True {};

	template <typename T, int I, glm::qualifier Q, typename ...Args>
	struct TNothrowCtor< TVec<T,I,Q>, Args... >	: CT_True {};


	template <typename T, glm::qualifier Q>
	struct TMemCopyAvailable< TQuat<T,Q> >		: CT_Bool< IsMemCopyAvailable<T> >{};

	template <typename T, glm::qualifier Q>
	struct TZeroMemAvailable< TQuat<T,Q> >		: CT_Bool< IsZeroMemAvailable<T> >{};

	// 'IsTriviallySerializable< TQuat<> > = false' - because SIMD and packed types has different alignment


	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TMemCopyAvailable< TMatrix<T, Columns, Rows, Q> > : CT_Bool< IsMemCopyAvailable<T> >{};

	template <typename T, uint Columns, uint Rows, glm::qualifier Q>
	struct TZeroMemAvailable< TMatrix<T, Columns, Rows, Q> > : CT_Bool< IsZeroMemAvailable<T> >{};

	// 'IsTriviallySerializable< TMatrix<> > = false' - because SIMD and packed types has different alignment

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
