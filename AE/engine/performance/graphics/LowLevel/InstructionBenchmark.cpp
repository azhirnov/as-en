// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "LowLevelPerfCore.h"

namespace AE::Graphics
{
namespace
{
	#include "performance/graphics/Resources/cpp/InstructionBenchmark.cpp.h"

	using namespace AE::Threading;

	static constexpr seconds	c_MaxTimeout	{30*60};
	static const EThreadArray	c_ThreadArr		{EThread::PerFrame, EThread::Renderer};

/*
=================================================
	IB_ProcessResults
=================================================
*/
	static String  IB_ProcessResults (const LowLevelPerfCore::ResultsPerType_t &map)
	{
		String		str = "\n\n## Notes";

		const auto	Get = [&map] (StringView type, StringView mode, OUT double &dt) -> bool
		{{
			dt = MaxValue<double>();

			auto	it = map.find( type );
			if ( it == map.end() )
				return false;

			for (auto& res : it->second)
			{
				if ( res.name == mode )
				{
					dt = res.dtAvr;
					return true;
				}
			}

			DBG_WARNING( "Can't find "s << type << " " << mode );
			return false;
		}};

		using NameAndDT = Pair< StringView, double >;
		const auto	SortNameAndDT = [] (auto &arr)
		{{
			std::sort(	std::begin(arr), std::end(arr),
						[](auto& lhs, auto& rhs)
						{
							return	Equal( lhs.second, rhs.second, 5_pct ) ?
										StringLessThan( lhs.first, rhs.first ) :
										lhs.second < rhs.second;
						}
					);
		}};

		// FMA
		{
			str << "\n\nFMA:\n";

			if ( double	fma, muladd, mul;
				 Get( "FP32", "fpFMA", OUT fma ) and Get( "FP32", "fpMulAdd", OUT muladd ) and Get( "FP32", "fpMul", OUT mul ))
			{
				muladd /= fma;
				mul /= fma;

				if ( muladd > 0.9 or mul > 0.9 )
					str << "\t* fp32 FMA is preferred than single FMul or separate FMulAdd\n";
				else
					str << "\t* fp32 FMA is x" << ToString( muladd, 1 ) << " SLOWER than single FMul or separate FMulAdd\n";
			}

			if ( double vfp16, sfp16, sfp32;
				 Get( "FP16", "fpAdd", OUT vfp16 ) and Get( "FP16", "fpScalarAdd", OUT sfp16 ) and Get( "FP32", "fpScalarAdd", OUT sfp32 ))
			{
				vfp16 *= 4.0 / sfp16;
				sfp16 /= sfp32;

				if ( vfp16 > 1.5 and sfp16 > 0.9 and sfp16 < 1.1 )
					str << "\t* HFMA2 is used, scalar FMA doesn't have x2 performance\n";
				else
					str << "\t* scalar fp16 FMA is used, vector FMA doesn't increase performance\n";
			}

			if ( double h, f;
				 Get( "FP16", "fpFMA", OUT h ) and Get( "FP32", "fpFMA", OUT f ))
			{
				f /= h;
				if ( f > 1.4 )
					str << "\t* fp16 FMA is x" << ToString( f, 1 ) << " faster than fp32 FMA - has HFMA2\n";
			}

			if ( double muladd, fma;
				 Get( "FP16", "fpFMA", OUT fma ) and Get( "FP16", "fpMulAdd", OUT muladd ))
			{
				muladd /= fma;
				if ( muladd > 1.6 )
					str << "\t* fp16 FMA is x" << ToString( muladd, 1 ) << " faster than MulAdd, HFMA2 is used only with FMA function\n";
			}

			// TODO: check
			if ( double h, f, par, seq;
				 Get( "FP16", "fpFMA", OUT h ) and Get( "FP32", "fpFMA", OUT f ) and Get( "FP32", "f16ParFMA", OUT par ) and Get( "FP32", "f16SeqFMA", OUT seq ))
			{
				seq /= par;
				h /= par;
				f /= par;
				if ( f < 0.9 and seq > 1.1 )
					str << "\t* has parallel datapath for fp32 FMA and fp16 FMA\n";
			}

			if ( double f, h, m;
				 Get( "FP32", "fpFMA", OUT f ) and Any( Get( "FP32_MED", "fpFMA", OUT m ), Get( "FP16", "fpFMA", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest FMA in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")";

				if ( h/f > 0.9 and m/f > 0.9 )
					str << ", all types may use the same fp32 FMA with increased precision";
				str << '\n';
			}

			if ( double f, h, m;
				 Get( "FP32", "fpAdd", OUT f ) and Any( Get( "FP32_MED", "fpAdd", OUT m ), Get( "FP16", "fpAdd", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest FAdd in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double f, h, m;
				 Get( "FP32", "fpMul", OUT f ) and Any( Get( "FP32_MED", "fpMul", OUT m ), Get( "FP16", "fpMul", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest FMul in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double f, h, m;
				 Get( "FP32", "fpMulAdd", OUT f ) and Any( Get( "FP32_MED", "fpMulAdd", OUT m ), Get( "FP16", "fpMulAdd", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest FMulAdd in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}
		}

		// TODO: check
		/*if ( double da, a;
			 Get( "FP32", "fpDualAdd", OUT da ) and Get( "FP32", "fpAdd", OUT a ))
		{
			da /= a;
			if ( da < 1.1 )
				str << "\t* fp32 supports dual issue for commands\n";
		}*/

		// vector length
		{
			str << "\n\nVector op\n";

			if ( double f, h, m;
				 Get( "FP32", "fpLength", OUT f ) and Any( Get( "FP32_MED", "fpLength", OUT m ), Get( "FP16", "fpLength", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest Length in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double l, d, n;
				 Get( "FP32", "fpLength", OUT l ) and Get( "FP32", "fpDistance", OUT d ) and Get( "FP32", "fpNormalize", OUT n ))
			{
				NameAndDT	arr [] = {
					{"Length"sv, l}, {"Distance"sv, d}, {"Normalize"sv, n}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}
		}

		// clamp
		{
			str << "\n\nClamp op\n";

			if ( double v1, v2, v3;
				 Get( "FP32", "fpClampUNorm", OUT v1 ) and Get( "FP32", "fpClampSNorm", OUT v2 ) and Get( "FP32", "fpClamp", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"ClampUNorm"sv, v1}, {"ClampSNorm"sv, v2}, {"Clamp"sv, v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}
		}

		// integer
		{
			str << "\n\nInt op:\n";

			if ( double msb, lsb;
				 Get( "INT32", "iFindMSB", OUT msb ) and Get( "INT32", "iFindLSB", OUT lsb ))
			{
				lsb /= msb;
				if ( lsb > 1.2 )
					str << "\t* i32 FindMSB is x" << ToString( lsb, 1 ) << " faster than FindLSB\n";
				if ( lsb < 0.8 )
					str << "\t* i32 FindMSB is x" << ToString( 1.0/lsb, 1 ) << " SLOWER than FindLSB\n";
			}

			if ( double m, h;
				 Get( "INT32_MED", "IAdd", OUT m ) and Get( "INT32", "IAdd", OUT h ))
			{
				h /= m;
				if ( h > 1.2 )
					str << "\t* medium precision FAdd is x" << ToString( h, 1 ) << " faster than high precision FAdd\n";
				if ( h < 0.8 )
					str << "\t* medium precision FAdd is x" << ToString( 1.0/h, 1 ) << " SLOWER than high precision FAdd\n";
			}

			if ( double m, h;
				 Get( "INT32_MED", "IMul", OUT m ) and Get( "INT32", "IMul", OUT h ))
			{
				h /= m;
				if ( h > 1.2 )
					str << "\t* medium precision IMul is x" << ToString( h, 1 ) << " faster than high precision IMul\n";
				if ( h < 0.8 )
					str << "\t* medium precision IMul is x" << ToString( 1.0/h, 1 ) << " SLOWER than high precision IMul\n";
			}
		}

		// sRGB
		{
			str << "\n\nsRGB conversion:\n";

			if ( double v1, v2, v3;
				 Get( "FP32", "fpSRGBCurve1", OUT v1 ) and Get( "FP32", "fpSRGBCurve2", OUT v2 ) and Get( "FP32", "fpSRGBCurve3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"Pow(2.2)", v1}, {"Pow(2.4)", v2}, {"Sqrt", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest sRGB curve: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP16", "fpSRGBCurve1", OUT v1 ) and Get( "FP16", "fpSRGBCurve2", OUT v2 ) and Get( "FP16", "fpSRGBCurve3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"v1", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp16 has fastest sRGB curve: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}
		}

		// trigonometry
		{
			str << "\n\nTrigonometry:\n";

			if ( double v1, v2, v3;
				 Get( "FP32", "fpATan", OUT v1 ) and Get( "FP32", "fpATan_2", OUT v2 ) and Get( "FP32", "fpATan_3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"native", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest ATan: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP32", "fpACos", OUT v1 ) and Get( "FP32", "fpACos_2", OUT v2 ) and Get( "FP32", "fpACos_3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"native", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest ACos: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP32", "fpASin", OUT v1 ) and Get( "FP32", "fpASin_2", OUT v2 ) and Get( "FP32", "fpASin_3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"native", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest ASin: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP16", "fpATan", OUT v1 ) and Get( "FP16", "fpATan_2", OUT v2 ) and Get( "FP16", "fpATan_3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"native", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp16 has fastest ATan: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP16", "fpACos", OUT v1 ) and Get( "FP16", "fpACos_2", OUT v2 ) and Get( "FP16", "fpACos_2", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"native", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp16 has fastest ACos: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP16", "fpASin", OUT v1 ) and Get( "FP16", "fpASin_2", OUT v2 ) and Get( "FP16", "fpASin_3", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"native", v1}, {"v2", v2}, {"v3", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp16 has fastest ASin: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}
		}

		// exponent
		{
			str << "\n\nExponent:\n";

			if ( double is, s;
				 Get( "FP32", "fpInvSqrt", OUT is ) and Get( "FP32", "fpSqrt", OUT s ))
			{
				s /= is;
				if ( s > 1.1 )
					str << "\t* fp32 InvSqrt is x" << ToString( s, 1 ) << " faster than Sqrt\n";
			}

			if ( double f, h, m;
				 Get( "FP32", "fpSqrt", OUT f ) and Any( Get( "FP32_MED", "fpSqrt", OUT m ), Get( "FP16", "fpSqrt", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest Sqrt in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double f, h, m;
				 Get( "FP32", "fpInvSqrt", OUT f ) and Any( Get( "FP32_MED", "fpInvSqrt", OUT m ), Get( "FP16", "fpInvSqrt", OUT h )))
			{
				NameAndDT	arr [] = {
					{"FP32", f}, {"FP16", h}, {"MediumP", m}
				};
				SortNameAndDT( arr );

				str << "\t* fastest InvSqrt in  ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3, v4;
				 Get( "FP32", "fpInvSqrt", OUT v1 ) and Get( "FP32", "fpSqrt", OUT v2 ) and Get( "FP32", "fpSqrt_2", OUT v3 ) and Get( "FP32", "fpSqrt_3", OUT v4 ))
			{
				NameAndDT	arr [] = {
					{"InvSqrt", v1}, {"Sqrt", v2}, {"Software2", v3}, {"Software3", v4}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest square root: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << "),  ";
				str << arr[3].first << " (x" << ToString( arr[3].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3, v4;
				 Get( "FP32", "fpCbrtPow", OUT v1 ) and Get( "FP32", "fpCbrtExp", OUT v2 ) and Any( Get( "FP32", "fpCbrt_2", OUT v3 ), Get( "FP32", "fpCbrt_3", OUT v4 )))
			{
				NameAndDT	arr [] = {
					{"Pow", v1}, {"ExpLog", v2}, {"Software2", v3}, {"Software3", v4}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest cube root: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "),  ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << "),  ";
				str << arr[3].first << " (x" << ToString( arr[3].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double v1, v2, v3;
				 Get( "FP32", "fpQdrtPow", OUT v1 ) and Get( "FP32", "fpQdrtSqrt", OUT v2 ) and Get( "FP32", "fpQdrtInvsqrt", OUT v3 ))
			{
				NameAndDT	arr [] = {
					{"Pow", v1}, {"Sqrt", v2}, {"InvSqrt", v3}
				};
				SortNameAndDT( arr );

				str << "\t* fp32 has fastest quad root: ";
				str << arr[0].first << ",  ";
				str << arr[1].first << " (x" << ToString( arr[1].second / arr[0].second, 1 ) << "), ";
				str << arr[2].first << " (x" << ToString( arr[2].second / arr[0].second, 1 ) << ")\n";
			}

			if ( double a, b;
				 Get( "FP32", "fpPow17", OUT a ) and Get( "FP32", "fpPow8", OUT b ))
			{
				a /= b;
				if ( a > 0.9 and a < 1.1 )
					str << "\t* fp32 Pow17 equal to Pow8 - native function used instead of MUL loop\n";
				else
					str << "\t* fp32 Pow uses MUL loop - performance depends on power\n";
			}

			if ( double a, b;
				 Get( "FP32", "fpPow1ov17", OUT a ) and Get( "FP32", "fpPow1ov7", OUT b ))
			{
				a /= b;
				if ( a > 0.9 and a < 1.1 )
					str << "\t* fp32 Pow1/17 equal to Pow1/7 - native function used\n";
			}
		}

		// other
		{
			str << "\n\nOther:\n";

			if ( double s2, s;
				 Get( "FP32", "fpSign_2", OUT s2 ) and Get( "FP32", "fpSign", OUT s ))
			{
				s /= s2;
				if ( s > 1.2 )
					str << "\t* fp32 Sign is x" << ToString( s, 1 ) << " faster than SignOrZero\n";
				if ( s < 0.8 )
					str << "\t* fp32 SignOrZero is x" << ToString( 1.0/s, 1 ) << " faster than FastSign\n";
			}

			if ( double pa, a;
				 Get( "FP32", "fiParAdd11", OUT pa ) and Get( "FP32", "fpAdd", OUT a ))
			{
				pa /= a;
				if ( pa < 1.2 )
					str << "\t* fp32 & i32 datapaths can execute in parallel in 1:1 rate\n";
				else
				if ( Get( "FP32", "fiParAdd21", OUT pa ) and Get( "FP32", "fpAdd", OUT a ))
				{
					pa /= a;
					if ( pa < 1.2 )
						str << "\t* fp32 & i32 datapaths can execute in parallel in 2:1 rate\n";
				}
			}

			if ( double vec, scalar;
				 Get( "FP32", "fpAdd", OUT vec ) and Get( "FP32", "fpScalarAdd", OUT scalar ))
			{
				scalar /= vec;
				if ( scalar < 2.0 )
					str << "\t* fp32 uses scalar instructions\n";
				else
					str << "\t* fp32 uses vector instructions\n";
			}

			if ( double vec, scalar;
				 Get( "FP32", "fpMulAdd", OUT vec ) and Get( "FP32", "fpScalarMulAdd", OUT scalar ))
			{
				scalar /= vec;
				if ( scalar < 2.0 )
					str << "\t* fp32 uses scalar instructions\n";
				else
					str << "\t* fp32 uses vector instructions\n";
			}
		}

		// errors
		{
			str << "\n\nErrors:\n";

			if ( double s, s2;
				 Get( "FP32", "fpSqrt", OUT s ) and Get( "FP32", "fpQdrtSqrt", OUT s2 ))
			{
				s2 /= s;
				if ( not (s2 > 1.8 and s2 < 3.0))
					str << "\t!!! fp32 Sqrt(Sqrt(x)) must be x2 slower than Sqrt(x)\n";
			}

			if ( double add, add2;
				 Get( "FP32", "fpAdd", OUT add ) and Get( "FP32", "fpDualAdd", OUT add2 ))
			{
				add2 /= add;
				if ( not (add2 > 1.8 and add2 < 2.1) )
					str << "\t!!! fp32 FAddx2 must be x2 slower than FAddx1, time measurements is not correct\n";
			}
		}

		return str;
	}
}

/*
=================================================
	_InstructionBenchmark
=================================================
*/
	void  LowLevelPerfCore::_InstructionBenchmark ()
	{
		const auto&		dim				= _isHighPerf ? HighPerf::c_Dim						: LowPerf::c_Dim;
		const auto		f16f64_modes	= _isHighPerf ? ArrayView{HighPerf::c_F16F64Modes}	: ArrayView{LowPerf::c_F16F64Modes};
		const auto		f16_modes		= _isHighPerf ? ArrayView{HighPerf::c_F16Modes}		: ArrayView{LowPerf::c_F16Modes};
		const auto		f16f32_modes	= _isHighPerf ? ArrayView{HighPerf::c_F16F32Modes}	: ArrayView{LowPerf::c_F16F32Modes};
		const auto		i8i64_modes		= _isHighPerf ? ArrayView{HighPerf::c_I8I64Modes}	: ArrayView{LowPerf::c_I8I64Modes};
		const auto		i32_modes		= _isHighPerf ? ArrayView{HighPerf::c_I32Modes}		: ArrayView{LowPerf::c_I32Modes};
		const auto		u8u64_modes		= _isHighPerf ? ArrayView{HighPerf::c_U8U64Modes}	: ArrayView{LowPerf::c_U8U64Modes};
		const auto		u32_modes		= _isHighPerf ? ArrayView{HighPerf::c_U32Modes}		: ArrayView{LowPerf::c_U32Modes};
		const auto		fu_modes		= _isHighPerf ? ArrayView{HighPerf::c_FUModes}		: ArrayView{LowPerf::c_FUModes};
		const auto&		count			= _isHighPerf ? HighPerf::c_IterCount				: LowPerf::c_IterCount;

		CHECK( All( _res.dim == uint2(dim) ));

		const String	rtech = "InstBenchRT."s << (_isHighPerf ? "High" : "Low");

	  #if 1
		// fast bench
		_IBenchmark( rtech,						f16f64_modes,	f16f32_modes,	count,	List<StringView>{ "FP32" });
	//	_IBenchmark( rtech,						f16f64_modes,	f16f32_modes,	count,	List<StringView>{ "FP32_MED" });
	//	_IBenchmark( rtech,						i8i64_modes,	i32_modes,		count,	List<StringView>{ "INT32" });
	//	_IBenchmark( rtech,						u8u64_modes,	u32_modes,		count,	List<StringView>{ "UINT32" });
	//	_IBenchmark( rtech,						fu_modes,		Default,		count,	List<StringView>{ "FP32" });
		_IBenchmark( rtech+".ShaderFloat16",	f16f64_modes,	f16f32_modes,	count,	List<StringView>{ "FP16" });
	//	_IBenchmark( rtech+".ShaderFloat16",	f16_modes,		Default,		count,	List<StringView>{ "FP16" });

	  #elif 0
		_IBenchmark( rtech+".ShaderFloat64",	f16f64_modes,	Default,		count,	List<StringView>{ "FP64" });
		_IBenchmark( rtech+".ShaderInt64",		i8i64_modes,	Default,		count,	List<StringView>{ "INT64" });
		_IBenchmark( rtech+".ShaderInt64",		u8u64_modes,	Default,		count,	List<StringView>{ "UINT64" });
		_IBenchmark( rtech+".ShaderFloatInt64",	fu_modes,		Default,		count,	List<StringView>{ "FP64" });

	  #elif 0
		static const StringView  c_Modes [] = { "fpAdd", "fpMul", "fpFMA", "fpDivAdd", "fpSqrtAdd" };

		_IBenchmark( rtech,						c_Modes,		Default,		count,	List<StringView>{ "FP32" });
		_IBenchmark( rtech+".ShaderFloat16",	c_Modes,		Default,		count,	List<StringView>{ "FP16" });

	  #else

		// full benchmark
		_IBenchmark( rtech,						f16f64_modes,	f16f32_modes,	count,	List<StringView>{ "FP32", "FP32_MED" }); // "FP32_LOW"
		_IBenchmark( rtech+".ShaderFloat64",	f16f64_modes,	Default,		count,	List<StringView>{ "FP64" });

		_IBenchmark( rtech,						i8i64_modes,	i32_modes,		count,	List<StringView>{ "INT32", "INT32_MED", "INT32_LOW" });
		_IBenchmark( rtech+".ShaderInt16",		i8i64_modes,	Default,		count,	List<StringView>{ "INT16" });

		_IBenchmark( rtech,						u8u64_modes,	u32_modes,		count,	List<StringView>{ "UINT32", "UINT32_MED", "UINT32_LOW" });
		_IBenchmark( rtech+".ShaderInt16",		u8u64_modes,	Default,		count,	List<StringView>{ "UINT16" });

		_IBenchmark( rtech,						fu_modes,		Default,		count,	List<StringView>{ "FP32" });
		_IBenchmark( rtech+".ShaderFloatInt64",	fu_modes,		Default,		count,	List<StringView>{ "FP64" });

		_IBenchmark( rtech+".ShaderInt8",		i8i64_modes,	Default,		count,	List<StringView>{ "INT8"  });
		_IBenchmark( rtech+".ShaderInt8",		u8u64_modes,	Default,		count,	List<StringView>{ "UINT8"  });

		_IBenchmark( rtech+".ShaderFloat16",	f16_modes,		Default,		count,	List<StringView>{ "FP16" });
		_IBenchmark( rtech+".ShaderFloat16",	f16f64_modes,	f16f32_modes,	count,	List<StringView>{ "FP16" });
		_IBenchmark( rtech+".ShaderFloatInt16",	fu_modes,		Default,		count,	List<StringView>{ "FP16" });

		_IBenchmark( rtech+".ShaderInt64",		i8i64_modes,	Default,		count,	List<StringView>{ "INT64" });
		_IBenchmark( rtech+".ShaderInt64",		u8u64_modes,	Default,		count,	List<StringView>{ "UINT64" });
	  #endif

		_complex.clear();
		_complex.reserve( CountOf( op1_minus_op0 ));

		for (auto& t : op1_minus_op0)
			_complex.push_back( t );

		_SortResults();
		_PrintResults( "InstBench", &IB_ProcessResults );
		_Reset();
	}

/*
=================================================
	_IBenchmark
=================================================
*/
	void  LowLevelPerfCore::_IBenchmark (StringView rtechName, ArrayView<StringView> modeArr1, ArrayView<StringView> modeArr2, uint iterCount, ArrayView<StringView> types)
	{
		if ( isCS )
			return _IBenchmarkCS( rtechName, modeArr1, modeArr2, iterCount, types );
		else
			return _IBenchmarkFS( rtechName, modeArr1, modeArr2, iterCount, types );
	}

/*
=================================================
	_IBenchmarkFS
=================================================
*/
	void  LowLevelPerfCore::_IBenchmarkFS (StringView rtechName, ArrayView<StringView> modeArr1, ArrayView<StringView> modeArr2, uint iterCount, ArrayView<StringView> types)
	{
		auto&	rts			= GraphicsScheduler();
		auto&	res_mngr	= rts.GetResourceManager();

		auto	pipelines	= res_mngr.LoadRenderTech( Default, RenderTechName{rtechName}, Default, _res.pplnCache );
		if ( not pipelines )
		{
			AE_LOGW( "Skip render technique '"s << rtechName << "'" );
			return;
		}

		const auto	Run = [this, &pipelines, iterCount] (StringView type, StringView mode, INOUT AllResults_t &results)
		{{
			String	name = "InstBench."s << mode << '-' << type;

			GraphicsPipelineID	ppln = pipelines->GetGraphicsPipeline( PipelineName{name} );
			if ( not ppln )
			{
				AE_LOGW( "Skip pipeline '"s << name << "'" );
				return;
			}

			AE_LOGI( "    - "s << mode );

			BenchResult	res;
			CHECK_ERRV( _IBenchmarkFrame( ppln, DescriptorSetID{}, mode, iterCount, OUT res ));

			results.push_back( RVRef(res) );
		}};

		for (auto type : types)
		{
			auto&	type_res = _results[ type ];

			AE_LOGI( "Benchmark '"s << type << "' type" );

			for (auto mode : modeArr1)
				Run( type, mode, INOUT type_res );

			for (auto mode : modeArr2)
				Run( type, mode, INOUT type_res );
		}
	}

/*
=================================================
	_IBenchmarkCS
=================================================
*/
	void  LowLevelPerfCore::_IBenchmarkCS (StringView rtechName, ArrayView<StringView> modeArr1, ArrayView<StringView> modeArr2, uint iterCount, ArrayView<StringView> types)
	{
		auto&	rts			= GraphicsScheduler();
		auto&	res_mngr	= rts.GetResourceManager();

		auto	pipelines	= res_mngr.LoadRenderTech( Default, RenderTechName{rtechName}, Default, _res.pplnCache );
		if ( not pipelines )
		{
			AE_LOGW( "Skip render technique '"s << rtechName << "'" );
			return;
		}

		Strong<DescriptorSetID>	ds;

		const auto	Run = [this, &pipelines, iterCount, &ds, &res_mngr] (StringView type, StringView mode, INOUT AllResults_t &results)
		{{
			String	name = "InstBench."s << mode << '-' << type;

			ComputePipelineID	ppln = pipelines->GetComputePipeline( PipelineName{name} );
			if ( not ppln )
			{
				AE_LOGW( "Skip pipeline '"s << name << "'" );
				return;
			}

			if ( not ds )
			{
				ds = res_mngr.CreateDescriptorSet( ppln, DescriptorSetName{"InstBenchRT.ds"} ).Get<0>();
				CHECK_ERRV( ds );

				DescriptorUpdater	upd;
				CHECK_ERRV( upd.Set( ds, EDescUpdateMode::Partialy ));
				CHECK_ERRV( upd.BindImage( UniformName{"un_Image"}, _res.imgView ));
				CHECK_ERRV( upd.Flush() );
			}

			AE_LOGI( "    - "s << mode );

			BenchResult	res;
			CHECK_ERRV( _IBenchmarkFrame( ppln, ds, mode, iterCount, OUT res ));

			results.push_back( RVRef(res) );
		}};

		for (auto type : types)
		{
			auto&	type_res = _results[ type ];

			AE_LOGI( "Benchmark '"s << type << "' type" );

			for (auto mode : modeArr1)
				Run( type, mode, INOUT type_res );

			for (auto mode : modeArr2)
				Run( type, mode, INOUT type_res );
		}

		res_mngr.ReleaseResource( ds );
	}

/*
=================================================
	_IBenchmarkTask
=================================================
*/
	RenderTaskCoro  LowLevelPerfCore::_IBenchmarkTask (GraphicsPipelineID ppln, DescriptorSetID, Query& q, uint qIndex, bool isLast) const
	{
		RenderTask&		self = co_await RenderTask_GetRef;

		DirectCtx::Graphics	ctx{ self };

		ctx.AccumBarriers()
			.ImageBarrier( _res.rt, EResourceState::Unknown, EResourceState::ColorAttachment );

		if ( q )
			ctx.WriteTimestamp( q, qIndex*3+0, EPipelineScope::All );

		// render pass
		{
			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"InstBench.RP"}, _res.dim }
													.AddTarget( AttachmentName{"Color"}, _res.rtView, RGBA32f{0.f} )
													.DefaultViewport() );

			dctx.BindPipeline( ppln );
			dctx.Draw( 3 );

			if ( q )
				dctx.WriteTimestamp( q, qIndex*3+1, EPipelineScope::All );

			ctx.EndRenderPass( dctx );
		}

		if ( q )
			ctx.WriteTimestamp( q, qIndex*3+2, EPipelineScope::All );

		if ( isLast )
		{
			DirectCtx::Transfer	tctx { self, ctx.ReleaseCommandBuffer() };
			BufferImageCopy		copy;

			copy.bufferRowLength		= 1;
			copy.bufferImageHeight		= 1;
			copy.imageSubres.aspectMask	= EImageAspect::Color;
			copy.imageOffset			= uint3{ _res.dim - 1u, 0 };
			copy.imageExtent			= uint3{1};

			tctx.AccumBarriers()
				.ImageBarrier( _res.rt, EResourceState::ColorAttachment, EResourceState::CopySrc );

			tctx.CopyImageToBuffer( _res.rt, _res.hostBuf, {copy} );

			tctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			co_await RenderTask_Execute( tctx );
		}
		else
		{
			co_await RenderTask_Execute( ctx );
		}

		co_return;
	}

/*
=================================================
	_IBenchmarkTask
=================================================
*/
	RenderTaskCoro  LowLevelPerfCore::_IBenchmarkTask (ComputePipelineID ppln, DescriptorSetID ds, Query& q, uint qIndex, bool isLast) const
	{
		RenderTask&		self	= co_await RenderTask_GetRef;
		const auto		state	= EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;

		DirectCtx::Compute	ctx{ self };

		uint2	wg_size = uint2{ctx.GetResourceManager().GetResource( ppln )->LocalSize()};

		ctx.AccumBarriers()
			.ImageBarrier( _res.img, EResourceState::Unknown, state );

		if ( q )
			ctx.WriteTimestamp( q, qIndex*3+0, EPipelineScope::All );

		ctx.BindPipeline( ppln );
		ctx.BindDescriptorSet( DescSetBinding{0}, ds );
		ctx.Dispatch( _res.dim / wg_size );

		if ( q )
			ctx.WriteTimestamp( q, qIndex*3+1, EPipelineScope::All );

		ctx.AccumBarriers()
			.ExecutionBarrier( EPipelineScope::Compute, EPipelineScope::Compute );

		if ( q )
			ctx.WriteTimestamp( q, qIndex*3+2, EPipelineScope::All );

		if ( isLast )
		{
			DirectCtx::Transfer	tctx { self, ctx.ReleaseCommandBuffer() };
			BufferImageCopy		copy;

			copy.bufferRowLength		= 1;
			copy.bufferImageHeight		= 1;
			copy.imageSubres.aspectMask	= EImageAspect::Color;
			copy.imageOffset			= uint3{ _res.dim - 1u, 0 };
			copy.imageExtent			= uint3{1};

			tctx.AccumBarriers()
				.ImageBarrier( _res.img, state, EResourceState::CopySrc );

			tctx.CopyImageToBuffer( _res.img, _res.hostBuf, {copy} );

			tctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			co_await RenderTask_Execute( tctx );
		}
		else
		{
			co_await RenderTask_Execute( ctx );
		}

		co_return;
	}

/*
=================================================
	_IBenchmarkFrame
=================================================
*/
	template <typename PplnType>
	bool  LowLevelPerfCore::_IBenchmarkFrame (PplnType ppln, DescriptorSetID ds, StringView mode, uint iterCount, OUT BenchResult &result) const
	{
		const uint	q_count = 10;

		result = Default;

		auto&	rts = GraphicsScheduler();
		auto&	qm	= rts.GetQueryManager();

		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		_ResetProfilers();

		Query	query	= qm.AllocQuery( EQueueType::Graphics, EQueryType::Timestamp, q_count*3 );

		auto	batch	= rts.BeginCmdBatch( CmdBatchDesc{ EQueueType::Graphics, 0, Default, null, CmdBatchDesc::EFlags::ResetQuery });
		CHECK_ERR( batch );

		StaticArray< AsyncTask, q_count >	rtasks;

		for (uint i = 0; i < q_count; ++i)
			rtasks[i] = batch->Run( _IBenchmarkTask( ppln, ds, query, i, (i+1 == q_count) ));

		auto	task	= batch->SubmitAsTask( Tuple{ArrayView{ rtasks }});
		auto	end		= rts.EndFrame( Tuple{ task });

		CHECK_ERR( Scheduler().Wait( {end}, c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

	  #if 0
		IResourceManager::NativeMemObjInfo_t	mem_info;
		CHECK_ERR( rts.GetResourceManager().GetMemoryInfo( _res.hostBuf, OUT mem_info ));

		const RGBA8u	col = *Cast<RGBA8u>( mem_info.mappedPtr );
		if ( col != RGBA8u{0} )
			AE_LOGW( "NaN or Inf in result with mode '"s << mode << "'" );
	  #endif

		StaticArray< nanosecondsd, q_count >	time_delta = {};

		if ( query )
		{
			StaticArray< nanosecondsd, q_count*3 >	tmp_time = {};
			qm.GetTimestamp( query, OUT tmp_time.data(), Sizeof(tmp_time) );

			for (usize i = 0; i < q_count; ++i)
				time_delta[i] = Max( tmp_time[i*3+1], tmp_time[i*3+2] ) - tmp_time[i*3];
		}

		{
			StaticArray< nanosecondsd, q_count*2 >	tmp_time = {};
			_ReadProfilers( OUT tmp_time.data(), CountOf(tmp_time) );

			if ( not query ) {
				for (usize i = 0; i < q_count; ++i)
					time_delta[i] = tmp_time[i*2+1] - tmp_time[i*2];
			}
		}

		std::sort( time_delta.begin(), time_delta.end() );

		// skip invalid results
		uint		first			= q_count-1;
		const auto	approx_min_dt	= (time_delta[q_count-2] + time_delta[q_count-1]) * 0.25;

		for (uint i = 0; i < q_count; ++i)
		{
			if ( time_delta[i] < approx_min_dt )
				continue;	// ~0 may happens on Apple with Vulkan emulation

			first = i;
			break;
		}

		// skip first valid result
		first = first+1 < q_count ? first+1 : first;

		// skip largest result if possible
		uint	last		= Min( Max( first+1, q_count-2 ), q_count-1 );
		uint	res_count	= last+1 - first;

		if ( res_count < 3 )
			AE_LOGI( "Valid results "s << ToString(res_count) << " for '" << mode << "' is less than 3" );

		result.dtMin = time_delta[ first ];
		result.dtMax = time_delta[ last ];

		for (uint i = first; i <= last; ++i)
			result.dtAvr += time_delta[i].count();

		double	scale = double(res_count);		// result count
		scale *= double(Area( _res.dim ));		// per pixel
		scale *= double(iterCount);				// instruction count
		scale *= 4.0;							// scalar count
		result.dtAvr /= scale;					// ns / op

		result.name = mode;
		return true;
	}


} // AE::Graphics
