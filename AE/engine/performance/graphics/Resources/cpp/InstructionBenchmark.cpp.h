const bool isCS = false;

namespace HighPerf
{
	static const uint  c_Dim = 4096;

	static const uint  c_IterCount = 512;

	static const StringView  c_F16F64Modes [] = {
		"fpModAdd", "fpAdd", "fpScalarAdd", "fpDualAdd", "fpDualMulAdd", "fpMul", "fpMulAdd", "fpFMA", "fpScalarMulAdd", "fpDivAdd", "fpFractAdd", "fpSqrtAdd", "fpSmoothstepAdd", "fpStepAdd", "fpSignAdd", "fpSignAdd_2", "fpClampUNormAdd", "fpClampSNormAdd", "fpLerpClamp", "fpClampAdd", "fpAbsAdd", "fpCeilAdd", "fpFloorAdd", "fpRoundAdd", "fpRoundEvenAdd", "fpTruncAdd", "fpCross3", "fpDot", "fpInvSqrtAdd", "fpLengthAdd", "fpNormalizeAdd", "fpDistanceAdd", "fpMinMaxAdd"
	};
	static const StringView  c_F16F32Modes [] = {
		"fpExpAdd", "fpExp2Add", "fpLogAdd", "fpLog2Add", "fpCbrtPowAdd", "fpCbrtExpAdd", "fpPow8Add", "fpPow17Add", "fpPow33Add", "fpPow1ov7Add", "fpPow1ov17Add", "fpPow1ov33Add", "fpCosAdd", "fpCosAdd_2", "fpSinAdd", "fpSinAdd_2", "fpTanAdd", "fpTanAdd_2", "fpATanAdd", "fpATanAdd_2", "fpATanAdd_3", "fpACosAdd", "fpACosAdd_2", "fpACosAdd_3", "fpASinAdd", "fpASinAdd_2", "fpASinAdd_3", "fpSqrtAdd_2", "fpSqrtAdd_3", "fpSRGBCurve1Add", "fpSRGBCurve2Add", "fpSRGBCurve3Add", "fpQdrtPowAdd", "fpQdrtSqrtAdd", "fpQdrtInvsqrtAdd", "fpCbrtAdd_3", "fpCbrtAdd_2", "fpCosH", "fpSinH", "fpTanH", "fpACosH", "fpASinH", "fpATanH"
	};
	static const StringView  c_F16Modes [] = {
		"f16ParFMA", "f16SeqFMA", "f16ParMulAdd", "f16SeqMulAdd"
	};
	static const StringView  c_I8I64Modes [] = {
		"iAdd", "iMul", "iMulAdd", "iShift", "iShiftConst", "iAndAdd", "iOrAdd", "iXorAdd", "iAbsAdd", "iFindLSB", "iClampAdd", "iClampConstAdd", "iMinMaxAdd", "iSignAdd", "iSignAdd_2", "iDivAdd", "iModAdd"
	};
	static const StringView  c_U8U64Modes [] = {
		"iAdd", "iMul", "iMulAdd", "iShift", "iShiftConst", "iAndAdd", "iOrAdd", "iXorAdd", "iFindLSB", "iClampAdd", "iClampConstAdd", "iMinMaxAdd", "iDivAdd", "iModAdd"
	};
	static const StringView  c_I32Modes [] = {
		"iBitCount", "iFindMSB", "iMulExtended"
	};
	static const StringView  c_U32Modes [] = {
		"iBitCount", "iFindMSB", "uAddCarry", "uSubBorrow", "uMulExtended"
	};
	static const StringView  c_FUModes [] = {
		"fiBitCast_Add", "fiIntToFloat_Add", "fiFloatToInt_Add", "fiParAdd21", "fiParAdd11", "fiSeqAdd_BitCast", "fiParMul", "fiSeqMul_BitCast", "fiParFMulIAdd", "fiSeqFMulIAdd_BitCast"
	};
}

namespace LowPerf
{
	static const uint  c_Dim = 2048;

	static const uint  c_IterCount = 128;

	static const StringView  c_F16F64Modes [] = {
		"fpModAdd", "fpAdd", "fpScalarAdd", "fpDualAdd", "fpDualMulAdd", "fpMul", "fpMulAdd", "fpFMA", "fpScalarMulAdd", "fpDivAdd", "fpFractAdd", "fpSqrtAdd", "fpSmoothstepAdd", "fpStepAdd", "fpSignAdd", "fpSignAdd_2", "fpClampUNormAdd", "fpClampSNormAdd", "fpLerpClamp", "fpClampAdd", "fpAbsAdd", "fpCeilAdd", "fpFloorAdd", "fpRoundAdd", "fpRoundEvenAdd", "fpTruncAdd", "fpCross3", "fpDot", "fpInvSqrtAdd", "fpLengthAdd", "fpNormalizeAdd", "fpDistanceAdd", "fpMinMaxAdd"
	};
	static const StringView  c_F16F32Modes [] = {
		"fpExpAdd", "fpExp2Add", "fpLogAdd", "fpLog2Add", "fpCbrtPowAdd", "fpCbrtExpAdd", "fpPow8Add", "fpPow17Add", "fpPow33Add", "fpPow1ov7Add", "fpPow1ov17Add", "fpPow1ov33Add", "fpCosAdd", "fpCosAdd_2", "fpSinAdd", "fpSinAdd_2", "fpTanAdd", "fpTanAdd_2", "fpATanAdd", "fpATanAdd_2", "fpATanAdd_3", "fpACosAdd", "fpACosAdd_2", "fpACosAdd_3", "fpASinAdd", "fpASinAdd_2", "fpASinAdd_3", "fpSqrtAdd_2", "fpSqrtAdd_3", "fpSRGBCurve1Add", "fpSRGBCurve2Add", "fpSRGBCurve3Add", "fpQdrtPowAdd", "fpQdrtSqrtAdd", "fpQdrtInvsqrtAdd", "fpCbrtAdd_3", "fpCbrtAdd_2", "fpCosH", "fpSinH", "fpTanH", "fpACosH", "fpASinH", "fpATanH"
	};
	static const StringView  c_F16Modes [] = {
		"f16ParFMA", "f16SeqFMA", "f16ParMulAdd", "f16SeqMulAdd"
	};
	static const StringView  c_I8I64Modes [] = {
		"iAdd", "iMul", "iMulAdd", "iShift", "iShiftConst", "iAndAdd", "iOrAdd", "iXorAdd", "iAbsAdd", "iFindLSB", "iClampAdd", "iClampConstAdd", "iMinMaxAdd", "iSignAdd", "iSignAdd_2", "iDivAdd", "iModAdd"
	};
	static const StringView  c_U8U64Modes [] = {
		"iAdd", "iMul", "iMulAdd", "iShift", "iShiftConst", "iAndAdd", "iOrAdd", "iXorAdd", "iFindLSB", "iClampAdd", "iClampConstAdd", "iMinMaxAdd", "iDivAdd", "iModAdd"
	};
	static const StringView  c_I32Modes [] = {
		"iBitCount", "iFindMSB", "iMulExtended"
	};
	static const StringView  c_U32Modes [] = {
		"iBitCount", "iFindMSB", "uAddCarry", "uSubBorrow", "uMulExtended"
	};
	static const StringView  c_FUModes [] = {
		"fiBitCast_Add", "fiIntToFloat_Add", "fiFloatToInt_Add", "fiParAdd21", "fiParAdd11", "fiSeqAdd_BitCast", "fiParMul", "fiSeqMul_BitCast", "fiParFMulIAdd", "fiSeqFMulIAdd_BitCast"
	};
}

static const Tuple<StringView, /*op1*/StringView, /*op0*/StringView, /*op1 scale*/double, /*op0 scale*/double>  op1_minus_op0 [] = {
		Tuple{ "fpDiv",				"fpDivAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpMod",				"fpModAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpFract",			"fpFractAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpInvSqrt",			"fpInvSqrtAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSqrt",			"fpSqrtAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSqrt_2",			"fpSqrtAdd_2",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSqrt_3",			"fpSqrtAdd_3",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSmoothstep",		"fpSmoothstepAdd",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpStep",			"fpStepAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSign",			"fpSignAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSign_2",			"fpSignAdd_2",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpClampUNorm",		"fpClampUNormAdd",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpClampSNorm",		"fpClampSNormAdd",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpClamp",			"fpClampAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpAbs",				"fpAbsAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCeil",			"fpCeilAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpFloor",			"fpFloorAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpRound",			"fpRoundAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpRoundEven",		"fpRoundEvenAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpTrunc",			"fpTruncAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpMinMax",			"fpMinMaxAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fiBitCast",			"fiBitCast_Add",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fiIntToFloat",		"fiIntToFloat_Add",		"fpAdd",		1.0,	2.0 },
		Tuple{ "fiFloatToInt",		"fiFloatToInt_Add",		"fpAdd",		1.0,	2.0 },
		Tuple{ "fpCross",			"fpCross3",				"",			4.0/3.0,1.0 },
		Tuple{ "fpExp",				"fpExpAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpExp2",			"fpExp2Add",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCbrtExp",			"fpCbrtExpAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCbrtPow",			"fpCbrtPowAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCbrt_2",			"fpCbrtAdd_2",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCbrt_3",			"fpCbrtAdd_3",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpLog",				"fpLogAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpLog2",			"fpLog2Add",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpLength",			"fpLengthAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpNormalize",		"fpNormalizeAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpDistance",		"fpDistanceAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCos",				"fpCosAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpCos_2",			"fpCosAdd_2",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSin",				"fpSinAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSin_2",			"fpSinAdd_2",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpTan",				"fpTanAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpTan_2",			"fpTanAdd_2",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpACos",			"fpACosAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpACos_2",			"fpACosAdd_2",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpACos_3",			"fpACosAdd_3",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpASin",			"fpASinAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpASin_2",			"fpASinAdd_2",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpASin_3",			"fpASinAdd_3",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpATan",			"fpATanAdd",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpATan_2",			"fpATanAdd_2",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpATan_3",			"fpATanAdd_3",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpPow8",			"fpPow8Add",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpPow17",			"fpPow17Add",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpPow33",			"fpPow33Add",				"fpAdd",		1.0,	1.0 },
		Tuple{ "fpPow1ov7",			"fpPow1ov7Add",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpPow1ov17",		"fpPow1ov17Add",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpPow1ov33",		"fpPow1ov33Add",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpQdrtPow",			"fpQdrtPowAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpQdrtSqrt",		"fpQdrtSqrtAdd",			"fpAdd",		1.0,	1.0 },
		Tuple{ "fpQdrtInvsqrt",		"fpQdrtInvsqrtAdd",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSRGBCurve1",		"fpSRGBCurve1Add",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSRGBCurve2",		"fpSRGBCurve2Add",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpSRGBCurve3",		"fpSRGBCurve3Add",		"fpAdd",		1.0,	1.0 },
		Tuple{ "fpLerp",			"fpLerpClamp",			"fpClampUNorm",1.0,	1.0 },
		Tuple{ "iDiv",				"iDivAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iMod",				"iModAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iAnd",				"iAndAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iOr",				"iOrAdd",					"iAdd",		1.0,	1.0 },
		Tuple{ "iXor",				"iXorAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iClamp",			"iClampAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iClampConst",		"iClampConstAdd",			"iAdd",		1.0,	1.0 },
		Tuple{ "iMinMax",			"iMinMaxAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iAbs",				"iAbsAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iSign",				"iSignAdd",				"iAdd",		1.0,	1.0 },
		Tuple{ "iSign_2",			"iSignAdd_2",				"iAdd",		1.0,	1.0 },
		Tuple{ "fiSeqAdd",			"fiSeqAdd_BitCast",		"fiBitCast",	1.0,	2.0 },
		Tuple{ "fiSeqMul",			"fiSeqMul_BitCast",		"fiBitCast",	1.0,	2.0 },
		Tuple{ "fiSeqFMulIAdd",		"fiSeqFMulIAdd_BitCast",	"fiBitCast",	1.0,	2.0 }
};
