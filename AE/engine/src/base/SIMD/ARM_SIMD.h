// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if AE_SIMD_NEON or AE_SIMD_SVE

# if 0
#	include <arm_acle.h>			// crc32				__ARM_ACLE
#	include <arm_fp16.h>			// scalar fp16			__ARM_FEATURE_FP16_SCALAR_ARITHMETIC
#	include <arm_bf16.h>			//						__ARM_FEATURE_BF16
#	include <arm_neon.h>			//						__ARM_NEON
#	include <arm_sve.h>				//						__ARM_FEATURE_SVE
#	include <arm_neon_sve_bridge.h>	// Neon to SVE			__ARM_NEON_SVE_BRIDGE
#	include <arm_mve.h>				// mix Neon and scalar	__ARM_FEATURE_MVE
#	include <arm_sme.h>				// SVE streaming mode	__ARM_FEATURE_SME
# endif

# include "base/SIMD/Neon.h"
# include "base/SIMD/SVE.h"
# include "base/SIMD/ARMCrypto.h"

#endif // AE_SIMD_NEON or AE_SIMD_SVE
