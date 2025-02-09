// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	define SH_FRAG
#	include <aestyle.glsl.h>
#	define MODE		0
#	define TYPE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	string		defines;
	uint		mode_idx	= 1000;
	bool		high_perf	= true;
	const bool	use_cs		= false;


	void  CreatePipeline (string mode, string dataType, string feature = "")
	{
		string	suffix	= mode + "-" + dataType;
		string	rtech	= "InstBenchRT";
		string	name	= "InstBench";

		if ( high_perf ){
			rtech += ".High";
			name  += ".High.";
		}else{
			rtech += ".Low";
			name  += ".Low.";
		}

		if ( use_cs )
		{
			RC<ComputePipeline>		ppln = ComputePipeline( name+suffix );
			ppln.SetLayout( "InstBenchRT.pl" );

			if ( feature.length() > 0 )
			{
				ppln.AddFeatureSet( feature );
				rtech += "."+feature;
			}

			{
				RC<Shader>	cs = Shader();
				cs.LoadSelf();
				cs.Define( "MODE=MODE_"+mode );
				cs.Define( "TYPE=TYPE_"+dataType );
				cs.Define( defines );
				cs.ComputeLocalSize( 8, 8 );

				ppln.SetShader( cs );
			}

			// specialization
			{
				RC<ComputePipelineSpec>		spec = ppln.AddSpecialization( "InstBench."+suffix );
				spec.AddToRenderTech( rtech, "Compute" );
			}
		}
		else
		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( name+suffix );

			if ( feature.length() > 0 )
			{
				ppln.AddFeatureSet( feature );
				rtech += "."+feature;
			}

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	fs = Shader();
				fs.LoadSelf();
				fs.Define( "MODE=MODE_"+mode );
				fs.Define( "TYPE=TYPE_"+dataType );
				fs.Define( defines );
				ppln.SetFragmentShader( fs );
			}

			// specialization
			{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "InstBench."+suffix );
				spec.AddToRenderTech( rtech, "Graphics" );

				RenderState	rs;
				rs.inputAssembly.topology = EPrimitive::TriangleList;

				spec.SetRenderState( rs );
			}
		}
	}


	string  ToCpp (string name, const array<string> &arr)
	{
		string	str = "\tstatic const StringView  " + name + " [] = {\n\t\t";
		for (uint i = 0; i < arr.size(); ++i)
		{
			if ( i > 0 ) str += ", ";
			str += "\"" + arr[i] + "\"";
		}
		str += "\n\t};\n";
		return str;
	}


	string  CreatePipelines (const uint dim, const uint count)
	{
		const bool		is_apple	= IsMetal();
		const string	default_def	= "DIM="+dim+".0\n";
		const string	count_str	= "COUNT="+count+"\n";
		string			cpp_src;

		cpp_src += "\tstatic const uint  c_Dim = "+dim+";\n\n";
		cpp_src += "\tstatic const uint  c_IterCount = "+count+";\n\n";

		LogInfo( "float point 16-64" );
		{
			const array<string>	fp_mode_arr = {
				"fpModAdd", "fpAdd", "fpScalarAdd", "fpDualAdd", "fpDualMulAdd", "fpMul", "fpMulAdd",
				"fpFMA", "fpScalarMulAdd", "fpDivAdd", "fpFractAdd", "fpSqrtAdd", "fpSmoothstepAdd",
				"fpStepAdd", "fpSignAdd", "fpSignAdd_2", "fpClampUNormAdd", "fpClampSNormAdd", "fpLerpClamp",
				"fpClampAdd", "fpAbsAdd", "fpCeilAdd", "fpFloorAdd", "fpRoundAdd", "fpRoundEvenAdd",
				"fpTruncAdd", "fpCross3", "fpDot", "fpInvSqrtAdd", "fpLengthAdd", "fpNormalizeAdd",
				"fpDistanceAdd", "fpMinMaxAdd"
			};
			cpp_src += ToCpp( "c_F16F64Modes", fp_mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < fp_mode_arr.size(); ++i)
			{
				defines += "MODE_"+fp_mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < fp_mode_arr.size(); ++i)
			{
				CreatePipeline( fp_mode_arr[i], "FP32" );
				CreatePipeline( fp_mode_arr[i], "FP32_MED" );
				//CreatePipeline( fp_mode_arr[i], "FP32_LOW" );

				if ( not is_apple )
				{
					CreatePipeline( fp_mode_arr[i], "FP16", "ShaderFloat16" );
					CreatePipeline( fp_mode_arr[i], "FP64", "ShaderFloat64" );
				}
			}
		}

		LogInfo( "float point 16-32" );
		{
			array<string>	fp_mode_arr = {
				"fpExpAdd", "fpExp2Add", "fpLogAdd", "fpLog2Add", "fpCbrtPowAdd", "fpCbrtExpAdd",
				"fpPow8Add", "fpPow17Add", "fpPow33Add", "fpPow1ov7Add", "fpPow1ov17Add", "fpPow1ov33Add",
				"fpCosAdd", "fpCosAdd_2", "fpSinAdd", "fpSinAdd_2", "fpTanAdd", "fpTanAdd_2",
				"fpATanAdd", "fpATanAdd_2", "fpATanAdd_3", "fpACosAdd", "fpACosAdd_2", "fpACosAdd_3",
				"fpASinAdd", "fpASinAdd_2", "fpASinAdd_3", "fpSqrtAdd_2", "fpSqrtAdd_3",
				"fpSRGBCurve1Add", "fpSRGBCurve2Add", "fpSRGBCurve3Add",
				"fpQdrtPowAdd", "fpQdrtSqrtAdd", "fpQdrtInvsqrtAdd"

				// hang on PVR:
				, "fpCbrtAdd_3"

				// failed to compile on Adreno 505 with mediump:
				, "fpCbrtAdd_2"
			};
			if ( not is_apple ) {
				fp_mode_arr.push_back( "fpCosH" );
				fp_mode_arr.push_back( "fpSinH" );
				fp_mode_arr.push_back( "fpTanH" );
				fp_mode_arr.push_back( "fpACosH" );
				fp_mode_arr.push_back( "fpASinH" );
				fp_mode_arr.push_back( "fpATanH" );
			}
			cpp_src += ToCpp( "c_F16F32Modes", fp_mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < fp_mode_arr.size(); ++i)
			{
				defines += "MODE_"+fp_mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < fp_mode_arr.size(); ++i)
			{
				CreatePipeline( fp_mode_arr[i], "FP32" );
				CreatePipeline( fp_mode_arr[i], "FP32_MED" );
				//CreatePipeline( fp_mode_arr[i], "FP32_LOW" );

				if ( not is_apple )
					CreatePipeline( fp_mode_arr[i], "FP16", "ShaderFloat16" );
			}
		}

		LogInfo( "float point 16" );
		{
			const array<string>	mode_arr = {
				"f16ParFMA", "f16SeqFMA",
				"f16ParMulAdd", "f16SeqMulAdd"
			};
			cpp_src += ToCpp( "c_F16Modes", mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < mode_arr.size(); ++i)
			{
				defines += "MODE_"+mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < mode_arr.size(); ++i)
			{
				CreatePipeline( mode_arr[i], "FP16", "ShaderFloat16" );
			}
		}

		LogInfo( "signed integer 8-64" );
		{
			const array<string>	int_mode_arr = {
				"iAdd", "iMul", "iMulAdd", "iShift", "iShiftConst", "iAndAdd",
				"iOrAdd", "iXorAdd", "iAbsAdd", "iFindLSB", "iClampAdd", "iClampConstAdd", "iMinMaxAdd",
				"iSignAdd", "iSignAdd_2"

				// too slow on Intel
				, "iDivAdd", "iModAdd"
			};
			cpp_src += ToCpp( "c_I8I64Modes", int_mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				defines += "MODE_"+int_mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				CreatePipeline( int_mode_arr[i], "INT32" );
				CreatePipeline( int_mode_arr[i], "INT32_MED" );
				CreatePipeline( int_mode_arr[i], "INT32_LOW" );
				CreatePipeline( int_mode_arr[i], "INT16",	"ShaderInt16" );

				if ( not is_apple ) {
					CreatePipeline( int_mode_arr[i], "INT64",	"ShaderInt64" );
					CreatePipeline( int_mode_arr[i], "INT8",	"ShaderInt8" );
				}
			}
		}

		LogInfo( "unsigned integer 8-64" );
		{
			const array<string>	int_mode_arr = {
				"iAdd", "iMul", "iMulAdd", "iShift", "iShiftConst", "iAndAdd",
				"iOrAdd", "iXorAdd", "iFindLSB", "iClampAdd", "iClampConstAdd", "iMinMaxAdd"

				// crashed on Intel
				, "iDivAdd", "iModAdd"
			};
			cpp_src += ToCpp( "c_U8U64Modes", int_mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				defines += "MODE_"+int_mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				CreatePipeline( int_mode_arr[i], "UINT32" );
				CreatePipeline( int_mode_arr[i], "UINT32_MED" );
				CreatePipeline( int_mode_arr[i], "UINT32_LOW" );
				CreatePipeline( int_mode_arr[i], "UINT16",	"ShaderInt16" );

				if ( not is_apple ) {
					CreatePipeline( int_mode_arr[i], "UINT64",	"ShaderInt64" );
					CreatePipeline( int_mode_arr[i], "UINT8",	"ShaderInt8" );
				}
			}
		}

		LogInfo( "signed integer 32" );
		{
			const array<string>	int_mode_arr = {
				"iBitCount", "iFindMSB", "iMulExtended"
			};
			cpp_src += ToCpp( "c_I32Modes", int_mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				defines += "MODE_"+int_mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				CreatePipeline( int_mode_arr[i], "INT32" );
				CreatePipeline( int_mode_arr[i], "INT32_MED" );
				CreatePipeline( int_mode_arr[i], "INT32_LOW" );
			}
		}

		LogInfo( "unsigned integer 32" );
		{
			const array<string>	int_mode_arr = {
				"iBitCount", "iFindMSB", "uAddCarry", "uSubBorrow", "uMulExtended"
			};
			cpp_src += ToCpp( "c_U32Modes", int_mode_arr );
			defines = default_def + count_str;

			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				defines += "MODE_"+int_mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < int_mode_arr.size(); ++i)
			{
				CreatePipeline( int_mode_arr[i], "UINT32" );
				CreatePipeline( int_mode_arr[i], "UINT32_MED" );
				CreatePipeline( int_mode_arr[i], "UINT32_LOW" );
			}
		}

		LogInfo( "integer / float point 16-64" );
		{
			const array<string>	mode_arr = {
				"fiBitCast_Add", "fiIntToFloat_Add", "fiFloatToInt_Add",
				// parallel data path:
				"fiParAdd21", "fiParAdd11", "fiSeqAdd_BitCast", "fiParMul", "fiSeqMul_BitCast", "fiParFMulIAdd", "fiSeqFMulIAdd_BitCast"
			};
			cpp_src += ToCpp( "c_FUModes", mode_arr );
			defines = default_def + count_str + "FLOAT_INT_INTERACTION\n";

			for (uint i = 0; i < mode_arr.size(); ++i)
			{
				defines += "MODE_"+mode_arr[i]+"="+mode_idx+"\n";
				++mode_idx;
			}
			for (uint i = 0; i < mode_arr.size(); ++i)
			{
				CreatePipeline( mode_arr[i], "FP32" );
				CreatePipeline( mode_arr[i], "FP16",	"ShaderFloatInt16" );

				if ( not is_apple )
					CreatePipeline( mode_arr[i], "FP64",	"ShaderFloatInt64" );
			}
		}

		// bool
		// matrix
		// packing

		return cpp_src;
	}


	void  ASmain ()
	{
		string	cpp_src;

		cpp_src += "const bool isCS = " + use_cs + ";\n\n";

		high_perf = true;
		cpp_src += "namespace HighPerf\n{\n";
		cpp_src += CreatePipelines( 4<<10, 512 );
		cpp_src += "}\n\n";

		high_perf = false;
		cpp_src += "namespace LowPerf\n{\n";
		cpp_src += CreatePipelines( 2<<10, 128 );
		cpp_src += "}\n\n";

		// OpAdd is used to disable optimizations, but we need a clear measurements, so subtract the OpAdd time.
		{
			cpp_src += "static const Tuple<StringView, /*op1*/StringView, /*op0*/StringView, /*op1 scale*/double, /*op0 scale*/double>  op1_minus_op0 [] = {";
			// float
			cpp_src += "\n\t\tTuple{ \"fpDiv\",				\"fpDivAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpMod\",				\"fpModAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpFract\",			\"fpFractAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpInvSqrt\",			\"fpInvSqrtAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSqrt\",			\"fpSqrtAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSqrt_2\",			\"fpSqrtAdd_2\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSqrt_3\",			\"fpSqrtAdd_3\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSmoothstep\",		\"fpSmoothstepAdd\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpStep\",			\"fpStepAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSign\",			\"fpSignAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSign_2\",			\"fpSignAdd_2\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpClampUNorm\",		\"fpClampUNormAdd\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpClampSNorm\",		\"fpClampSNormAdd\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpClamp\",			\"fpClampAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpAbs\",				\"fpAbsAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCeil\",			\"fpCeilAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpFloor\",			\"fpFloorAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpRound\",			\"fpRoundAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpRoundEven\",		\"fpRoundEvenAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpTrunc\",			\"fpTruncAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpMinMax\",			\"fpMinMaxAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fiBitCast\",			\"fiBitCast_Add\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fiIntToFloat\",		\"fiIntToFloat_Add\",		\"fpAdd\",		1.0,	2.0 },";
			cpp_src += "\n\t\tTuple{ \"fiFloatToInt\",		\"fiFloatToInt_Add\",		\"fpAdd\",		1.0,	2.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCross\",			\"fpCross3\",				\"\",			4.0/3.0,1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpExp\",				\"fpExpAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpExp2\",			\"fpExp2Add\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCbrtExp\",			\"fpCbrtExpAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCbrtPow\",			\"fpCbrtPowAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCbrt_2\",			\"fpCbrtAdd_2\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCbrt_3\",			\"fpCbrtAdd_3\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpLog\",				\"fpLogAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpLog2\",			\"fpLog2Add\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpLength\",			\"fpLengthAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpNormalize\",		\"fpNormalizeAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpDistance\",		\"fpDistanceAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCos\",				\"fpCosAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpCos_2\",			\"fpCosAdd_2\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSin\",				\"fpSinAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSin_2\",			\"fpSinAdd_2\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpTan\",				\"fpTanAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpTan_2\",			\"fpTanAdd_2\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpACos\",			\"fpACosAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpACos_2\",			\"fpACosAdd_2\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpACos_3\",			\"fpACosAdd_3\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpASin\",			\"fpASinAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpASin_2\",			\"fpASinAdd_2\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpASin_3\",			\"fpASinAdd_3\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpATan\",			\"fpATanAdd\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpATan_2\",			\"fpATanAdd_2\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpATan_3\",			\"fpATanAdd_3\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpPow8\",			\"fpPow8Add\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpPow17\",			\"fpPow17Add\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpPow33\",			\"fpPow33Add\",				\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpPow1ov7\",			\"fpPow1ov7Add\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpPow1ov17\",		\"fpPow1ov17Add\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpPow1ov33\",		\"fpPow1ov33Add\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpQdrtPow\",			\"fpQdrtPowAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpQdrtSqrt\",		\"fpQdrtSqrtAdd\",			\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpQdrtInvsqrt\",		\"fpQdrtInvsqrtAdd\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSRGBCurve1\",		\"fpSRGBCurve1Add\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSRGBCurve2\",		\"fpSRGBCurve2Add\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpSRGBCurve3\",		\"fpSRGBCurve3Add\",		\"fpAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"fpLerp\",			\"fpLerpClamp\",			\"fpClampUNorm\",1.0,	1.0 },";
			// int
			cpp_src += "\n\t\tTuple{ \"iDiv\",				\"iDivAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iMod\",				\"iModAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iAnd\",				\"iAndAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iOr\",				\"iOrAdd\",					\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iXor\",				\"iXorAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iClamp\",			\"iClampAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iClampConst\",		\"iClampConstAdd\",			\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iMinMax\",			\"iMinMaxAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iAbs\",				\"iAbsAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iSign\",				\"iSignAdd\",				\"iAdd\",		1.0,	1.0 },";
			cpp_src += "\n\t\tTuple{ \"iSign_2\",			\"iSignAdd_2\",				\"iAdd\",		1.0,	1.0 },";
			// float/int
			cpp_src += "\n\t\tTuple{ \"fiSeqAdd\",			\"fiSeqAdd_BitCast\",		\"fiBitCast\",	1.0,	2.0 },";
			cpp_src += "\n\t\tTuple{ \"fiSeqMul\",			\"fiSeqMul_BitCast\",		\"fiBitCast\",	1.0,	2.0 },";
			cpp_src += "\n\t\tTuple{ \"fiSeqFMulIAdd\",		\"fiSeqFMulIAdd_BitCast\",	\"fiBitCast\",	1.0,	2.0 }";
			cpp_src += "\n};\n";
		}

		WriteFile( "cpp/InstructionBenchmark.cpp.h", cpp_src );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		gl.Position = FullscreenTrianglePos();
	}

#endif
//-----------------------------------------------------------------------------
#if defined(SH_FRAG) or defined(SH_COMPUTE)
	#define AE_LICENSE_MIT
	#define AE_ENABLE_UNKNOWN_LICENSE

	#include "Math.glsl"
	#include "FastMath.glsl"
	#include "ColorSpace.glsl"

	#define TYPE_FP32			20
	#define TYPE_FP32_MED		22
	#define TYPE_FP32_LOW		23
	#define TYPE_FP16			24
	#define TYPE_FP64			25

	#define TYPE_INT32			30
	#define TYPE_INT32_MED		31
	#define TYPE_INT32_LOW		32
	#define TYPE_INT16			33
	#define TYPE_INT8			34
	#define TYPE_INT64			35

	#define TYPE_UINT32			40
	#define TYPE_UINT32_MED		41
	#define TYPE_UINT32_LOW		42
	#define TYPE_UINT16			43
	#define TYPE_UINT8			44
	#define TYPE_UINT64			45

	#if TYPE == TYPE_FP32 or TYPE == TYPE_FP32_MED or TYPE == TYPE_FP32_LOW or TYPE == TYPE_FP16 or TYPE == TYPE_FP64
	# define FP_TYPE
	#elif TYPE == TYPE_INT32 or TYPE == TYPE_INT32_MED or TYPE == TYPE_INT32_LOW or TYPE == TYPE_INT16 or TYPE == TYPE_INT8 or TYPE == TYPE_INT64 or \
		  TYPE == TYPE_UINT32 or TYPE == TYPE_UINT32_MED or TYPE == TYPE_UINT32_LOW or TYPE == TYPE_UINT16 or TYPE == TYPE_UINT8 or TYPE == TYPE_UINT64
	# define INT_TYPE
	#endif

	#define UNROLL		[[unroll]]

	#ifdef FP_TYPE
		#if TYPE == TYPE_FP32
		#	define type			float32_t
		#	define type4		f32vec4
		#	define out_type4	float4
		#elif TYPE == TYPE_FP32_MED
		#	define type			float
		#	define type4		float4
		#	define out_type4	type4
			precision mediump float;
		#elif TYPE == TYPE_FP32_LOW
		#	define type			float
		#	define type4		float4
		#	define out_type4	type4
			precision lowp float;
		#elif TYPE == TYPE_FP16
		#	define type			half
		#	define type4		half4
		#	define out_type4	float4
		#elif TYPE == TYPE_FP64
		#	define type			double
		#	define type4		double4
		#	define out_type4	float4
		#endif
		#ifdef FLOAT_INT_INTERACTION
		# if TYPE == TYPE_FP32 or TYPE == TYPE_FP32_MED or TYPE == TYPE_FP32_LOW
		#	define utype		uint
		#	define utype4		uint4
		#	define CastU		uintBitsToFloat
		#	define CastF		floatBitsToUint
		# elif (TYPE == TYPE_FP16) && AE_ENABLE_SHORT_TYPE
		#	define utype		ushort
		#	define utype4		ushort4
		#	define CastU		uint16BitsToFloat16
		#	define CastF		float16BitsToUint16
		# elif (TYPE == TYPE_FP64) && AE_ENABLE_LONG_TYPE
		#	define utype		ulong
		#	define utype4		ulong4
		#	define CastU		uint64BitsToDouble
		#	define CastF		doubleBitsToUint64
		# endif
		#endif

		#define FOR()		UNROLL for (int i = 0; i < COUNT; ++i)
		#define FOR2()		UNROLL for (int i = 0; i < COUNT/2; ++i)
		#define FOR4()		UNROLL for (int i = 0; i < COUNT/4; ++i)

		#ifdef SH_COMPUTE
		#	define OUTPUT(x)	if ( AllLess( out_type4(x), out_type4(-1.e+20) )) ) gl.image.Store( un_Image, int2(gl.GlobalInvocationID.xy), out_type4(x) )
		#	define COORD		float2(gl.GlobalInvocationID.xy)
		#else
			layout(location=0) out  out_type4  out_Color;
		#	define OUTPUT(x)	out_Color = Saturate(out_type4(x)) * 0.001;
		#	define COORD		(gl.FragCoord)
		#endif

		#if TYPE != TYPE_FP64
			type4  ApplySRGBCurve1 (type4 v)
			{
				return pow( v, type4(1.0/2.2) );
			}

			type4  RemoveSRGBCurve1 (type4 v)
			{
				return pow( v, type4(2.2) );
			}

			// ApplySRGBCurve
			type4  ApplySRGBCurve2 (type4 v)
			{
				type4	b = step( v, type4(0.0031308) );
				return	(type(12.92) * v) * (type(1.0) - b) +
						(type(1.055) * pow( v, type4(1.0/2.4) ) - type(0.055)) * b;
			}

			// RemoveSRGBCurve
			type4  RemoveSRGBCurve2 (type4 v)
			{
				type4	b = step( v, type4(0.04045) );
				return	(v / type(12.92)) * (type(1.0) - b) +
						pow( (v + type(0.055)) / type(1.055), type4(2.4) ) * b;
			}

			// ApplySRGBCurve_Fast
			type4  ApplySRGBCurve3 (type4 v)
			{
				type4	b = step( v, type4(0.0031308) );
				return	(type(12.92) * v) * (type(1.0) - b) +
						(type(1.13005) * sqrt(v - type(0.00228)) - type(0.13448) * v + type(0.005719)) * b;
			}

			// RemoveSRGBCurve_Fast
			type4   RemoveSRGBCurve3 (type4 v)
			{
				type4	b = step( v, type4(0.04045) );
				return	(v / type(12.92)) * (type(1.0) - b) +
						(type(-7.43605) * v - type(31.24297) * sqrt(type(-0.53792) * v + type(1.279924)) + type(35.34864)) * b;
			}

			// from https://www.shadertoy.com/view/ssyyDh
			float4  Sqrt_v2 (float4 x)
			{
				float4 y = uintBitsToFloat( 532545536u + (floatBitsToUint(x) >> 1));
				y = 0.5*(y+x/y);
				y = 0.5*(y+x/y);
				return y;
			}

			float4  Sqrt_v3 (float4 x)
			{
				float4	y	= uintBitsToFloat( 532545536u + (floatBitsToUint(x) >> 1) );
				float4	y2	= y*y;
				float4	x2	= x*x;
				return (y2 * (y2 + 6.0 * x) + x2) / (4.0 * y * (y2 + x));
			}

			// from https://www.shadertoy.com/view/ssyyDh
			float4  Cbrt_v2 (float4 x)
			{
				float4 y = uintBitsToFloat( 709973695u + floatBitsToUint(x) / 3u);
				y = y*(2.0/3.0) + (1.0/3.0)*x/(y*y);
				y = y*(2.0/3.0) + (1.0/3.0)*x/(y*y);
				return y;
			}

			// https://www.shadertoy.com/view/wts3RX
			float4  Cbrt_v3 (float4 x)
			{
				#define NEWTON_ITER 1
				#define HALLEY_ITER 1

				float4 y = sign(x) * uintBitsToFloat( floatBitsToUint( abs(x) ) / 3u + 0x2a514067u );

				for (int i = 0; i < NEWTON_ITER; ++i)
					y = ( 2. * y + x / ( y * y ) ) * .333333333;

				for (int i = 0; i < HALLEY_ITER; ++i)
				{
					float4 y3 = y * y * y;
					y *= ( y3 + 2. * x ) / ( 2. * y3 + x );
				}
				return y;
			}

		# if TYPE == TYPE_FP16
			half4  Sqrt_v2 (half4 x)	{ return half4( Sqrt_v2( float4(x) )); }
			half4  Sqrt_v3 (half4 x)	{ return half4( Sqrt_v3( float4(x) )); }
			half4  Cbrt_v2 (half4 x)	{ return half4( Cbrt_v2( float4(x) )); }
			half4  Cbrt_v3 (half4 x)	{ return half4( Cbrt_v3( float4(x) )); }
		# endif
		#endif


		void  Main ()
		{
			const type4		p = type4(1.0 + COORD.xyyx / DIM * float4(0.5, 0.75, 0.4, 0.9) * 0.1);
			const type4		t = p.xwzy * type4(0.944, 1.209, 0.97, 1.08);

			#ifdef FLOAT_INT_INTERACTION
			const utype4	up = utype4(COORD.xyyx) << utype4(0,1,2,3);
			const utype4	ut = utype(1) + up / utype(DIM/4);
			#endif

			#if MODE == MODE_fpNone
				OUTPUT(p);

			#elif MODE == MODE_fpAdd

				type4	a = t;
				FOR4() {
					a += p;  a -= t;
					a += p;  a -= t;
				}
				OUTPUT(a);

			#elif MODE == MODE_fpScalarAdd

				type	a = t.x;
				FOR() {
					a += p.x;  a -= t.x;
					a += p.y;  a -= t.y;
				}
				OUTPUT(a);

			#elif MODE == MODE_fpDualAdd

				type4	a = t;
				type4	b = p;
				FOR2() {
					a += p;		b += t;
					a -= t;		b -= p;
				}
				OUTPUT(a + b);

			#elif MODE == MODE_fpDualMulAdd

				type4	a = t;
				type4	b = p;
				FOR2() {
					a += p;  a -= t;
					b *= p;  b *= t;
				}
				OUTPUT(a + b);

			#elif MODE == MODE_fpMul

				type4	a = type4(1.0);
				FOR4() {
					a *= p;  a *= a;
					a *= t;  a *= a;
				}
				OUTPUT(a);

			#elif MODE == MODE_fpMulAdd

				type4	a = type4(1.0);
				FOR2() {
					a = (a * p) + t;
					a = (a * p) + t;
				}
				OUTPUT(a);

			#elif MODE == MODE_fpScalarMulAdd

				type	a = type(1.0);
				FOR() {
					a = (a * p.x) + t.x;
					a = (a * p.y) + t.y;
					a = (a * p.z) + t.z;
					a = (a * p.w) + t.w;
				}
				OUTPUT(a);

			#elif MODE == MODE_fpFMA

				type4	a = type4(1.0);
				FOR() { a = fma( a, p, t ); }
				OUTPUT(a);

			#elif MODE == MODE_fpDivAdd

				type4	a = t * type(100.0);
				FOR() { a = (p / a) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpModAdd

				type4	a = p;
				FOR() { a = mod( t, a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpFractAdd

				type4	a = t;
				FOR() { a = fract( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpInvSqrtAdd

				type4	a = p;
				FOR() { a = InvSqrt( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSqrtAdd

				type4	a = p;
				FOR() { a = sqrt( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSqrtAdd_2

				type4	a = p;
				FOR() { a = Sqrt_v2( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSqrtAdd_3

				type4	a = p;
				FOR() { a = Sqrt_v3( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSmoothstepAdd

				type4	a = t;
				FOR() { a = smoothstep( type(0.0), type(1.0), a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpStepAdd

				type4	a = t;
				FOR() { a = step( type(0.5), a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpSignAdd

				type4	a = t;
				FOR2() { a = SignOrZero( a ) + p;  a = SignOrZero( a ) - t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSignAdd_2

				type4	a = t;
				FOR2() { a = Sign( a ) + p;  a = Sign( a ) - t; }
				OUTPUT(a);

			#elif MODE == MODE_fpLerpClamp

				type4		a = t;
				const type4	b = p + p + t;
				FOR() { a = Lerp( a, b, a );  a = Saturate(a); }
				OUTPUT(a);

			#elif MODE == MODE_fpClampUNormAdd

				type4	a = t;
				FOR() { a = clamp( a, type(0.0), type(1.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpClampSNormAdd

				type4	a = t;
				FOR() { a = clamp( a, type(-1.0), type(1.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpClampAdd

				type4	a = p;
				FOR() { a = clamp( t, a, t ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpAbsAdd

				type4	a = p;
				FOR2() { a = abs( a ) - p;  a = abs( a ) - t; }
				OUTPUT(a);

			#elif MODE == MODE_fpCeilAdd

				type4	a = p;
				FOR() { a = ceil( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpFloorAdd

				type4	a = p;
				FOR() { a = floor( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpRoundAdd

				type4	a = p;
				FOR() { a = round( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpRoundEvenAdd

				type4	a = p;
				FOR() { a = roundEven( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpTruncAdd

				type4	a = p;
				FOR() { a = trunc( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpCross3

				type4	a = p;
				FOR2() { a.xyz = cross( a.xyz, t.xyz );  a.xyz = cross( a.xyz, p.xyz ); }
				OUTPUT(a);

			#elif MODE == MODE_fpDot

				type4	a = p;
				FOR2() { a += dot( a, t );  a += dot( a, -p ); }
				OUTPUT(a);

			#elif MODE == MODE_fpExpAdd

				type4	a = p;
				FOR() { a = exp( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpExp2Add

				type4	a = p;
				FOR() { a = exp2( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpLogAdd

				type4	a = p;
				FOR() { a = log( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpLog2Add

				type4	a = p;
				FOR() { a = log2( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpLengthAdd

				type4	a = p;
				FOR() { a = length( a ) + p; }			// TODO: length() used once per float4, so its actual perf is 1/4
				OUTPUT(a);

			#elif MODE == MODE_fpNormalizeAdd

				type4	a = p;
				FOR() { a = normalize( a ) + p; }		// TODO: normalize() used once per float4, so its actual perf is 1/4
				OUTPUT(a);

			#elif MODE == MODE_fpDistanceAdd

				type4	a = p;
				FOR() { a = distance( a, t ) + p; }		// TODO: distance() used once per float4, so its actual perf is 1/4
				OUTPUT(a);

			#elif MODE == MODE_fpCosAdd

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = cos( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpCosAdd_2

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = FastCos( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpSinAdd

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = sin( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpSinAdd_2

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = FastSin( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpTanAdd

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = tan( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpTanAdd_2

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = FastTan( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpACosAdd

				const type4	d = p * type(0.317);
				type4		a = min( p, type4(1.0) );
				FOR() { a = acos( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpACosAdd_2

				const type4	d = p * type(0.317);
				type4		a = min( p, type4(1.0) );
				FOR() { a = FastACos( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpACosAdd_3

				const type4	d = p * type(0.317);
				type4		a = min( p, type4(1.0) );
				FOR() { a = FastACos2( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpASinAdd

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = asin( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpASinAdd_2

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = FastASin( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpASinAdd_3

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = FastASin2( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpATanAdd

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = atan( t, a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpATanAdd_2

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = FastATan( t, a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpATanAdd_3

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = FastATan2( t, a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpCosH

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = cosh( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpSinH

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = sinh( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpTanH

				const type4	d = p * type(3.14);
				type4		a = d;
				FOR() { a = tanh( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpACosH

				const type4	d = p * type(0.317);
				type4		a = min( p, type4(1.0) );
				FOR() { a = acosh( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpASinH

				const type4	d = p * type(0.632);
				type4		a = min( p, type4(1.0) );
				FOR() { a = asinh( a ) + d; }
				OUTPUT(a);

			#elif MODE == MODE_fpATanH

				type4	a = min( p, type4(1.0) );
				FOR() { a = atanh( a ); }
				OUTPUT(a);

			#elif MODE == MODE_fpCbrtPowAdd

				type4	a = p;
				FOR() { a = pow( a, type4(1.0/3.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpCbrtExpAdd

				type4	a = p;
				FOR() { a = exp( log(a) / type4(3.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpCbrtAdd_2

				type4	a = p;
				FOR() { a = Cbrt_v2( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpCbrtAdd_3

				type4	a = p;
				FOR() { a = Cbrt_v3( a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpPow8Add

				type4	a = p;
				FOR() { a = pow( a, type4(8.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpPow17Add

				type4	a = p;
				FOR() { a = pow( a, type4(17.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpPow33Add

				type4	a = p;
				FOR() { a = pow( a, type4(33.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpPow1ov7Add

				type4	a = p;
				FOR() { a = pow( a, type4(1.0/7.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpPow1ov17Add

				type4	a = p;
				FOR() { a = pow( a, type4(1.0/17.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpPow1ov33Add

				type4	a = p;
				FOR() { a = pow( a, type4(1.0/33.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpQdrtPowAdd

				type4	a = p;
				FOR() { a = pow( a, type4(1.0/4.0) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpQdrtSqrtAdd

				type4	a = p;
				FOR() { a = sqrt( sqrt( a )) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpQdrtInvsqrtAdd

				type4	a = p;
				FOR() { a = InvSqrt( InvSqrt( a )) + p; }
				OUTPUT(a);

			#elif MODE == MODE_fpSRGBCurve1Add

				type4	a = p;
				FOR2() { a = ApplySRGBCurve1( a ) + p;  a = RemoveSRGBCurve1( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSRGBCurve2Add

				type4	a = p;
				FOR2() { a = ApplySRGBCurve2( a ) + p;  a = RemoveSRGBCurve2( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpSRGBCurve3Add

				type4	a = p;
				FOR2() { a = ApplySRGBCurve3( a ) + p;  a = RemoveSRGBCurve3( a ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fpMinMaxAdd

				type4	a = p;
				FOR2() { a = min( a, t ) + p;  a = max( a, p ) + t; }
				OUTPUT(a);

			#elif MODE == MODE_fiBitCast_Add

				type4	a = p;
				FOR() { a = CastU( CastF( a + t )); }
				OUTPUT(a);

			#elif MODE == MODE_fiIntToFloat_Add

				type4	fa = p;
				utype4	ua = ut;

				FOR2()
				{
					ua += ut;	fa += type4(ua);
					ua += up;	fa += type4(ua);
				}
				OUTPUT(fa);

			#elif MODE == MODE_fiFloatToInt_Add

				type4	fa = p;
				utype4	ua = ut;

				FOR2()
				{
					fa += p;	ua += utype4(fa);
					fa -= t;	ua += utype4(fa);
				}
				OUTPUT(ua);

			#elif MODE == MODE_fiParAdd21 or MODE == MODE_fiParAdd11 or MODE == MODE_fiSeqAdd_BitCast

				type4	fa = p;
				utype4	ua = ut;

				#if MODE == MODE_fiParAdd21
					FOR4() {
						fa += p;	fa -= t;
						ua += ut;	ua += ua;
						fa += p;	fa -= t;
					}
				#elif MODE == MODE_fiParAdd11
					FOR2() {
						fa += p;	fa -= t;
						ua += ut;	ua += ua;
					}
				#else // MODE_fiSeqAdd_BitCast
					FOR4() {
						fa += p;	fa -= t;
						ua += ut;	fa -= CastU( ua );
						fa += p;	ua += CastF( fa );
					}
				#endif
				OUTPUT(out_type4(fa) * out_type4(ua));

			#elif MODE == MODE_fiParMul or MODE == MODE_fiSeqMul_BitCast

				type4	fa = type4(1.0);
				utype4	ua = up;

				#if MODE == MODE_fiParMul
					FOR4() {
						fa *= p;	fa *= t;
						ua *= up;	ua *= ut;
						fa *= p;	fa *= t;
					}
				#else // MODE_fiSeqMul_BitCast
					FOR4() {
						fa *= p;			fa *= t;
						ua *= CastF( fa );	ua *= ut;
						fa *= CastU( ua );	fa *= p;
					}
				#endif
				OUTPUT(out_type4(fa) * out_type4(ua));

			#elif MODE == MODE_fiParFMulIAdd or MODE == MODE_fiSeqFMulIAdd_BitCast

				type4	fa = type4(1.0);
				utype4	ua = up;

				#if MODE == MODE_fiParMul
					FOR4() {
						fa *= p;	fa *= t;
						ua += ut;	ua += ua;
						fa *= p;	fa *= t;
					}
				#else // MODE_fiSeqFMulIAdd_BitCast
					FOR4() {
						fa *= p;			fa *= t;
						ua += CastF( fa );	ua += ua;
						fa *= CastU( ua );	fa *= t;
					}
				#endif
				OUTPUT(out_type4(fa) * out_type4(ua));

			#elif MODE == MODE_f16ParFMA or MODE == MODE_f16SeqFMA or MODE == MODE_f16ParMulAdd or MODE == MODE_f16SeqMulAdd

				const float4	fp = (1.0 + COORD.xyyx / DIM * float4(0.5, 0.75, 0.4, 0.9) * 0.1);
				const float4	ft = fp.xwzy * float4(0.944, 1.209, 0.97, 1.08);
				type4			ha = type4(1.0);
				float4			fa = float4(1.0);

				#if MODE == MODE_f16ParFMA
					FOR2() {
						ha = fma( ha,  p,  t );
						fa = fma( fa, fp, ft );
					}
				#elif MODE == MODE_f16SeqFMA
					FOR2() {
						ha = fma( ha,  p,  t );
						fa = fma( fa, fp, float4(ha) );
					}
				#elif MODE == MODE_f16ParMulAdd
					FOR2() {
						ha = (ha *  p) +  t;
						fa = (fa * fp) + ft;
					}
				#elif MODE == MODE_f16SeqMulAdd
					FOR2() {
						ha = (ha * p)  + t;
						fa = (fa * fp) + float4(ha);
					}
				#endif
				OUTPUT(out_type4(ha) * fa);

			#else
			#	error Unsupported MODE
			#endif
		}

	#endif // FP_TYPE

	#ifdef INT_TYPE
		#if TYPE == TYPE_INT32
		#	define type		int32_t
		#	define type4	i32vec4
		#elif TYPE == TYPE_INT32_MED
		#	define type		int
		#	define type4	int4
			precision mediump int;
		#elif TYPE == TYPE_INT32_LOW
		#	define type		int
		#	define type4	int4
			precision lowp int;
		#elif TYPE == TYPE_INT16
		#	define type		sshort
		#	define type4	sshort4
		#elif TYPE == TYPE_INT8
		#	define type		sbyte
		#	define type4	sbyte4
		#elif TYPE == TYPE_INT64
		#	define type		slong
		#	define type4	slong4

		// unsigned
		#elif TYPE == TYPE_UINT32
		#	define type		uint
		#	define type4	u32vec4
		#elif TYPE == TYPE_UINT32_MED
		#	define type		uint
		#	define type4	uint4
			precision mediump int;
		#elif TYPE == TYPE_UINT32_LOW
		#	define type		uint
		#	define type4	uint4
			precision lowp int;
		#elif TYPE == TYPE_UINT16
		#	define type		ushort
		#	define type4	ushort4
		#elif TYPE == TYPE_UINT8
		#	define type		ubyte
		#	define type4	ubyte4
		#elif TYPE == TYPE_UINT64
		#	define type		ulong
		#	define type4	ulong4
		#endif

		#define FOR()		UNROLL for (int i = 0; i < COUNT; ++i)
		#define FOR2()		UNROLL for (int i = 0; i < COUNT/2; ++i)
		#define FOR4()		UNROLL for (int i = 0; i < COUNT/4; ++i)

		#ifdef SH_COMPUTE
		#	define OUTPUT(x)	if ( all(lessThan( float4(x), float4(-1.e+20) )) ) gl.image.Store( un_Image, int2(gl.GlobalInvocationID.xy), float4(x) )
		#	define COORD		float2(gl.GlobalInvocationID.xy)
		#else
			layout(location=0) out  float4  out_Color;
		#	define OUTPUT(x)	out_Color = clamp(float4(x), 0.0, 1.0) * 0.001;  //if ( all(greaterThan( float4(x), float4(-1.e+20) )) ) gl.Discard;
		#	define COORD		(gl.FragCoord)
		#endif

		void  Main ()
		{
			const type4		p = type4(COORD.xyyx) << type4(0,1,2,3);
			const type4		t = type(1) + p / type(DIM/4);

			#if MODE == MODE_iNone
				OUTPUT(p);

			#elif MODE == MODE_iAdd
				type4	a = type4(0);
				type4	b = t;
				FOR4() { a += b;  b += p;  a += t;  b += a; }
				OUTPUT(b);

			#elif MODE == MODE_iMul

				type4	a = p;
				FOR2() { a *= p;  a *= a; }
				OUTPUT(a);

			#elif MODE == MODE_iMulAdd

				type4	a = t;
				FOR() { a = (a * p) + t; }
				OUTPUT(a);

			#elif MODE == MODE_iDivAdd

				type4	a = t;
				FOR() { a = (p / a) + t; }
				OUTPUT(a);

			#elif MODE == MODE_iModAdd

				type4	a = p * type(8);
				FOR() { a = (p % a) + t; }
				OUTPUT(a);

			#elif MODE == MODE_iShift

				type4	a = p;
				FOR2() { a <<= (t >> a); }
				OUTPUT(a);

			#elif MODE == MODE_iShiftConst

				type4	a = p;
				FOR2() { a <<= (t >> type(i & 8)); }
				OUTPUT(a);

			#elif MODE == MODE_iAndAdd

				type4	a = p;
				FOR() { a += (t & a); }
				OUTPUT(a);

			#elif MODE == MODE_iOrAdd

				type4	a = p;
				FOR() { a += (t | a); }
				OUTPUT(a);

			#elif MODE == MODE_iXorAdd

				type4	a = p;
				FOR() { a += (t ^ a); }
				OUTPUT(a);

			#elif MODE == MODE_iAbsAdd

				type4	a = p;
				FOR2() { a = abs( a ) + t;  a = abs( a ) - p; }
				OUTPUT(a);

			#elif MODE == MODE_iSignAdd

				type4	a = p;
				FOR2() { a = SignOrZero( a ) + t;  a = SignOrZero( a ) - p; }
				OUTPUT(a);

			#elif MODE == MODE_iSignAdd_2

				type4	a = p;
				FOR2() { a = Sign( a ) + t;  a = Sign( a ) - p; }
				OUTPUT(a);

			#elif MODE == MODE_iBitCount

				type4	a = p * type(8);
				FOR() { a = bitCount( a ); }
				OUTPUT(a);

			#elif MODE == MODE_iFindLSB

				type4	a = p * type(8);
				FOR() { a = findLSB( a ); }
				OUTPUT(a);

			#elif MODE == MODE_iFindMSB

				type4	a = p * type(8);
				FOR() { a = findMSB( a ); }
				OUTPUT(a);

			#elif MODE == MODE_iMulExtended

				type4	a = p;
				type4	b, c;
				FOR() { imulExtended( a, p, b, c );  a = b | c; }
				OUTPUT(a);

			#elif MODE == MODE_iClampAdd

				type4	a = p;
				FOR() { a = clamp( p, t, a ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_iClampConstAdd

				type4	a = p;
				FOR() { a = clamp( a, type(0), type(0xFF) ) + p; }
				OUTPUT(a);

			#elif MODE == MODE_iMinMaxAdd

				type4	a = p;
				FOR2() { a = min( a, t );  a = max( a, p );  a += p; }
				OUTPUT(a);

			#elif MODE == MODE_uMulExtended

				type4	a = p;
				type4	b, c;
				FOR() { umulExtended( a, p, b, c );  a = b | c; }
				OUTPUT(a);

			#elif MODE == MODE_uAddCarry

				type4	a = p;
				type4	c;
				FOR() { a = uaddCarry( a, p, c );  a += c; }
				OUTPUT(a);

			#elif MODE == MODE_uSubBorrow

				type4	a = p;
				type4	c;
				FOR() { a = usubBorrow( a, p, c );  a += c; }
				OUTPUT(a);

			#else
			#	error Unsupported MODE
			#endif
		}

	#endif // INT_TYPE
#endif // SH_FRAG
//-----------------------------------------------------------------------------
