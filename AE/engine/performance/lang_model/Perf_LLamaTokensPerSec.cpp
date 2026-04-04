// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
#ifdef AE_PLATFORM_WINDOWS
	static Bytes	g_RAM		= 64_GiB;	// TODO: auto-detect
	static Bytes	g_VRAM		= 8_GiB;
#endif
#ifdef AE_PLATFORM_LINUX
	static Bytes	g_RAM		= 28_GiB;
	static Bytes	g_VRAM		= 14_GiB;
#endif

	using Clock_t		= std::chrono::high_resolution_clock;
	using TimePoint_t	= Clock_t::time_point;
	using Duration_t	= Clock_t::duration;

	struct PerfTestInput
	{
		Path				modelPath;
		uint				ctxSize;
		uint				gpuLayers;
		EBackend		backend;
	};

	struct PerfTestResult
	{
		Duration_t			modelLoading;
		Duration_t			ctxCreation;
		Duration_t			timeToFirstToken;
		Duration_t			responseGeneration;
		uint				maxCtxSize			= 0;
		uint				totalTokens			= 0;
		uint				maxLayers			= 0;
		bool 				hasErrors			= false;
		float				expectedWords		= -1.f;
		float				unexpectedWords		= -1.f;
		Bytes				modelSize;
		Bytes				usedRAM;
	};
	using PerfTestRequest = Pair< PerfTestInput, PerfTestResult >;


	class ResponseListener final : public IResponseListener
	{
	public:
		U8String	response;
		uint		outTokens		= 0;
		uint		inTokens		= 0;
		TimePoint_t	firstToken;
		bool 		hasErrors		= false;

		Bytes		maxRAM;

		ResponseListener () __NE___ {}

		bool  AppendResponse (U8StringView, uint count) __NE_OV
		{
			if ( outTokens == 0 ) {
				firstToken = Clock_t::now();
			}
			outTokens += count;
			return true;
		}

		void  OnComplete (U8StringView completeResponse, uint tokenCount) __NE_OV
		{
			response = completeResponse;
			CHECK( outTokens == tokenCount );

			PerformanceStat::MemoryCounters	mem;
			Unused( PerformanceStat::GetPerfCounters( null, null, OUT &mem ));
			maxRAM = mem.peakUsage;
		}

		void  OnError (ErrorCode) __NE_OV
		{
			hasErrors = true;
		}

		void  RequiredPromptTokens (uint count) __NE_OV
		{
			inTokens = count;
		}
	};


	ND_ static String  PrintLine (const PerfTestInput &in, const PerfTestResult &out)
	{
		String	str;
		str << "\n| " << ToString( in.modelPath.stem() ) << " | "
			<< ToString( in.backend ) << " | "
			<< ToString( out.modelSize ) << " | "
			<< ToString( Min( in.ctxSize, out.maxCtxSize )) << " | "
			<< (out.hasErrors ? "(" : "") << ToString( out.totalTokens ) << (out.hasErrors ? ")" : "") << " | "
			<< ToString( out.modelLoading ) << " | "
			<< ToString( out.ctxCreation ) << " | "
			<< ToString( out.timeToFirstToken ) << " | "
			<< ToString( out.responseGeneration ) << " | ";

		if ( in.backend != EBackend::CPU )
			str << ToString(Min( in.gpuLayers, out.maxLayers )) << " / " << ToString( out.maxLayers );
		else
			str << "0 / " << ToString( out.maxLayers );

		str	<< " | "
			<< ToString( double(out.totalTokens) / secondsd{out.responseGeneration}.count(), 2 ) << " | "
			<< " | "
			<< ToString( out.usedRAM );

		if ( out.expectedWords >= 0.f or out.unexpectedWords >= 0.f )
			str << '+' << ToString( out.expectedWords, 1 ) << " / -" << ToString( out.unexpectedWords, 1 );
		else
			str << "-";

		str << " |";
		return str;
	}


	static void  PrintResults (ArrayView<PerfTestRequest> results, const Path &dstFolder)
	{
		String	str;

		str << "\n| model | backend | model size |ctx size | tokens | load (s) | create ctx (s) | time to first token (s) | gen time (s) | GPU layers | tok/sec | max RAM | correct |"
			<< "\n|-------|---------|------------|---------|--------|----------|----------------|-------------------------|--------------|------------|---------|---------|---------|";

		for (auto& [in, out] : results)
		{
			if ( out.totalTokens == 0 )
				continue;

			str << PrintLine( in, out );
		}
		AE_LOGI( str );

		str << "\n\n";

		FileWStream		file { dstFolder / "result.md" };
		if ( file.IsOpen() )
		{
			Unused( file.Write( str ));
		}
	}


	static void  LLamaPerf_RunTest (const PerfTestInput &in, OUT PerfTestResult &result,
									U8String systemMsg, U8String prompt, const Path &dstFolder,
									ArrayView<U8String> expected, ArrayView<U8String> unexpected)
	{
		AE_LOGI( "Try backend: "s << ToString(in.backend) << ", ctx: " << ToString(in.ctxSize) << ", gpuLayers: " << ToString(in.gpuLayers) );

		AE_LOGI( "Load model" );

		result.hasErrors = true;

		RC<ILanguageModel>	model;
		{
			#ifdef AE_PLATFORM_UNIX_BASED
				PlatformUtils::ClearFileCache();
			#endif

			auto	begin = Clock_t::now();

			LLama::OpenParams	params;
			params.modelFile			= in.modelPath;
			params.enableLogger			= true;
			params.keepModelInMemory	= true;
			params.useMMap				= true;
			params.backend				= in.backend;
			params.gpuLayers			= in.gpuLayers;

			model = LMFactory::CreateLLama( params );
			CHECK_ERRV( model );

			result.modelLoading = Clock_t::now() - begin;

			auto	info = model->GetModelInfo();
			result.maxCtxSize	= info.maxContextSize;
			result.maxLayers	= info.layerCount;
			result.modelSize	= info.modelSize;
		}

		AE_LOGI( "Create context" );

		RC<ILanguageModelContext>	ctx;
		{
			auto	begin = Clock_t::now();

			LLama::ContextParams	params;
			params.contextSize		= in.ctxSize;
			params.threadCount		= UMax;
			params.offloadKQV		= true;
			params.opOffload		= true;

			#if 1 // better for seed-oss
				params.sampler.minP		= LLama::Sampler_MinP{ 0.05f, 1 };
				params.sampler.temperature	= 0.8f;
			#endif
			#if 0 // better for gpt-oss ?
				params.sampler.topK		= LLama::Sampler_TopK{ 40 };
				params.sampler.minP		= LLama::Sampler_MinP{ 0.05f, 1 };
				params.sampler.topP		= LLama::Sampler_TopP{ 0.90f, 1 };
				params.sampler.temperature = 0.6f;
			#endif

			ctx = model->CreateContext( params );
			CHECK_ERRV( ctx );

			result.ctxCreation = Clock_t::now() - begin;
		}

		AE_LOGI( "Generate response" );
		{
			auto	begin	 = Clock_t::now();
			auto	listener = MakeRC<ResponseListener>();

			if ( not systemMsg.empty() )
				CHECK_ERRV( ctx->Append( ERole::System, RVRef(systemMsg) ));

			bool ok = ctx->Generate( RVRef(prompt), listener );
			CHECK( ok );

			result.timeToFirstToken		= listener->firstToken - begin;
			result.responseGeneration	= Clock_t::now() - begin;
			result.totalTokens			= listener->outTokens;
			result.hasErrors			= listener->hasErrors;
			result.usedRAM				= listener->maxRAM;

			if ( ok								and
				 (not expected.empty()		or
				  not unexpected.empty())		and
				 not listener->response.empty() )
			{
				uint	n_expected		= 0;
				uint	n_unexpected	= 0;

				for (auto& exp : expected)
				{
					n_expected += uint{HasSubString( listener->response, exp )};
				}
				for (auto& exp : unexpected)
				{
					n_unexpected += uint{HasSubString( listener->response, exp )};
				}

				result.expectedWords	= float(n_expected) / float(expected.size());
				result.unexpectedWords	= float(n_unexpected) / float(unexpected.size());
			}

			if ( not listener->response.empty() )
			{
				String	fname = ToString( in.modelPath.stem() );
				fname << '-' << ToString(in.backend) << ".md";

				FileWStream		file { dstFolder / fname };
				if ( file.IsOpen() )
				{
					Unused( file.Write( listener->response ));
				}
			}
		}
		AE_LOGI( PrintLine( in, result ));
	}


	static void  LLamaPerf_RunTestWithOptimalGPULayers (INOUT PerfTestInput &in, OUT PerfTestResult &result,
														U8String systemMsg, U8String prompt, const Path &dstFolder,
														ArrayView<U8String> expected, ArrayView<U8String> unexpected)
	{
		Bytes	model_size	= FileSystem::FileSize( in.modelPath );

		if ( model_size > g_RAM )
			return;

		if ( in.backend == EBackend::CPU )
		{
			LLamaPerf_RunTest( in, OUT result, systemMsg, prompt, dstFolder, expected, unexpected );
			return;
		}

		if ( model_size > g_VRAM )
			return;

		LLamaPerf_RunTest( in, OUT result, systemMsg, prompt, dstFolder, expected, unexpected );
	}
//-----------------------------------------------------------------------------


	static void  LLamaPerf_Test1 ()
	{
		const Path	model_1 = R"(path/to/llm)";

		PerfTestRequest		requests[] =
		{
			{PerfTestInput{ model_1,	8 << 10,	0,		EBackend::CPU		}, PerfTestResult{}},
			{PerfTestInput{ model_1,	8 << 10,	999,	EBackend::CUDA		}, PerfTestResult{}},
			{PerfTestInput{ model_1,	8 << 10,	999,	EBackend::Vulkan	}, PerfTestResult{}},
		};
		const Path	dst_folder = Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME;

		FileSystem::DeleteDirectory( dst_folder );
		FileSystem::CreateDirectory( dst_folder );

		for (auto& [in, out] : requests)
		{
			LLamaPerf_RunTest(	in, out,
								u8"Answer as experienced programmer. By default use C++ and HLSL. Prefer snake_case style.",
								u8"Write complex sky shader on GLSL. Add clouds with ray marching. Add effects: light shafts, lens flares, rain particles.",
								dst_folder, Default, Default );
		}
		PrintResults( requests, dst_folder );
	}
//-----------------------------------------------------------------------------


	static const Path	c_Models [] = {
		R"(path/to/llm)",
	};


	static void  LLamaPerf_RunTest2 (const Path &dstFolder, const U8String prompt, ArrayView<U8String> expected = Default, ArrayView<U8String> unexpected = Default)
	{
		constexpr uint	c_ContextSize	= 16 << 10;
		constexpr uint	c_GPULayers		= 999;

		Array<PerfTestRequest>	requests;
		const PerfTestInput		request_types[] =
		{
		//	PerfTestInput{ "",	c_ContextSize,	0,				EBackend::CPU		},
		//	PerfTestInput{ "",	c_ContextSize,	c_GPULayers,	EBackend::CUDA	},
			PerfTestInput{ "",	c_ContextSize,	c_GPULayers,	EBackend::Vulkan	},
		};

		//FileSystem::DeleteDirectory( dstFolder );
		FileSystem::CreateDirectories( dstFolder );

		for (auto& model : c_Models)
		{
			for (auto& type : request_types)
			{
				if ( not FileSystem::IsFile( model ))
				{
					AE_LOGE( "Model file '"s << ToString(model) << "' is not exists" );
					continue;
				}

				auto& [in, out] = requests.emplace_back();

				in = type;
				in.modelPath = model;

				LLamaPerf_RunTestWithOptimalGPULayers( in, out, Default, prompt, dstFolder, expected, unexpected );
			}
		}
		PrintResults( requests, dstFolder );
	}
//-----------------------------------------------------------------------------


	static void  LLamaPerf_Test2 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8"How to compile single file from cmake project. Use MSVC compiler.",
			List<U8String>{ u8"/p:SelectedFiles=", u8"/p:SourceFile=" },	// 'msbuild', '.vcxproj'
			List<U8String>{ u8"add_library", u8"add_executable", u8"add_custom_target", u8"compile_command.json", u8"cl.exe" }
		);
	}


	static void  LLamaPerf_Test3 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8R"(
convert from GLSL to HLSL
```
uniform uimage2D un_OutImage;

void main()
{
	ivec2 size = gl_NumWorkGroups.xy * gl_WorkGroupSize.xy;
	vec2 uv = vec2(gl_GlobalInvocationID.xy) / vec2(size);

	imageStore( un_OutImage, gl_GlobalInvocationID.xy, vec4(uv, uv) );
}
```)",
			List<U8String>{ u8"SV_DispatchThreadID", u8"[numthreads(", u8"RWTexture2D<uint>", u8"float2" },
			List<U8String>{ u8"pixel shader", u8"TEXCOORD", u8"SV_Target" }
		);
	}


	static void  LLamaPerf_Test4 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8"How to write entity component system with AVX512 support? With example.",
			List<U8String>{ u8"_mm512_load", u8"_mm512_mul", u8"_mm512_add", u8"_mm512_fmadd" },
			List<U8String>{ u8"_mm256_", u8"```python" }
		);
	}


	static void  LLamaPerf_Test6 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8R"(
Rewrite this code using only AVX instructions.
```cpp
template <uint X, uint Y, uint Z, uint W>
SimdDouble4  SimdDouble4::Swizzle ()  C_NE___
{
	StaticAssert( X < 4 );
	StaticAssert( Y < 4 );
	StaticAssert( Z < 4  );
	StaticAssert( W < 4 );

	packed_double4	v {*this};
	packed_double4	s{ v[X], v[Y], v[Z], v[W] };
	return Self{ &s.x };
}
```)",
			{ u8"_mm256_permute4x64_pd" }		// low performance
		);
	}


	static void  LLamaPerf_Test7 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8R"(
what do this slang shader
```
module network;

import common;
import mlp_sw;

public struct MyNetwork
{
    public FeedForwardLayer<4, 16> layer1;
    public FeedForwardLayer<16, 4> layer2;

    [Differentiable]
    internal MLVec<4> encodeInput(NFloat x, NFloat y)
    {
        return MLVec<4>.fromArray({
                x,
                y,
                x*x,
                y*y,
            });
    }

    [Differentiable]
    internal MLVec<4> _eval(NFloat x, NFloat y)
    {
        let encoding = encodeInput(x, y);
        let layer1Output = layer1.eval(encoding);
        let leyer2Output = layer2.eval(layer1Output);
        return leyer2Output;
    }

    [Differentiable]
    public half4 eval(no_diff NFloat x, no_diff NFloat y)
    {
        let mlv = _eval(x, y);
        let arr = mlv.toArray();
        return half4(arr[0], arr[1], arr[2], arr[3]);
    }
}

[Differentiable]
public half loss(MyNetwork* network, no_diff half x, no_diff half y)
{
    let networkResult = network.eval(x, y);
    let gt = no_diff groundtruth(x, y);
    let diff = networkResult - gt;

    return dot(diff, diff);
}

public half4 groundtruth(half x, half y)
{
    return {
        (x + y) / (1 + y * y),
        2 * x + y,
        0.5 * x * x + 1.2 * y,
        x + 0.5 * y * y,
    };
}
```)",
			List<U8String>{ u8"SLang", u8"slang", u8"4D", u8"4-Dim", u8"[Differentiable]" }
		);
	}


	static void  LLamaPerf_Test8 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8"Which CPU and GPU instructions used to accelerate 8bit tensors",
			List<U8String>{ u8"VNNI", u8"AMX", u8"vmlal_s8", u8"ONNX", u8"SVE", u8"CUDA" }	// vmull_u8 ?
		);
	}


	static void  LLamaPerf_Test9 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8"Calculate theoretical bf16 performance of AMD Zen4 with 11 cores at 5.3GHz. Same for Zen2 with 5 cores at 4.1GHz.",
			List<U8String>{ u8"AVX512", u8"TFLOPS" }
		);

		// answer:
		// Zen4: AVX512, 32x fp16 per cy, 2x SIMD per core, but it is 256bit with dual issue, 11 * 5.3 * 2 * 32 = 3.73 TFLOPS
		// Zen2: AVX2, only fp16c, so fp16 -> fp32 -> fp16, 8x fp32 per cy, 2x SIMD per core, 5 * 4.1 * 8 * 2 = 328 GFLOPS (fp32)
	}


	static void  LLamaPerf_Test10 ()
	{
		U8String	str;
		{
			FileRStream		file{ Path{TEST_SRC_FOLDER} / "../../samples/res_editor/_data/scripts/neural-shader/MLPTraining.as" };
			CHECK_ERRV( file.IsOpen() );
			CHECK_ERRV( file.Read( file.RemainingSize(), OUT str ));

			const U8StringView	correct		= u8"ptr += input_count * 2 * sizeof_float;";
			const U8StringView	incorrect	= u8"ptr += input_count * sizeof_float;";

			CHECK_ERRV( FindAndReplace( INOUT str, correct, incorrect ) == 1 );

			u8"Problem in MLP training shader - training doesn't work. Get list of most likely errors.\n```" >> str;
			str << u8"\n```";
		}

		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			str,
			List<U8String>{ u8"input_count", u8"un_ArgBuf.inputs" }
		);
	}


	static void  LLamaPerf_Test11 ()
	{
		U8String	str;
		{
			FileRStream		file{ Path{TEST_SRC_FOLDER} / "../../papers/graphics/GeometryCulling-ru.md" };
			CHECK_ERRV( file.IsOpen() );
			CHECK_ERRV( file.Read( file.RemainingSize(), OUT str ));

			U8StringView	cut = u8"# Итоги тестов";

			usize	pos = str.find( cut );
			CHECK_ERRV( pos < str.size() );
			str.resize( pos );

			u8"In text below find culling method or combination of them which I should use to support all GPU types.\n```" >> str;
			str << u8"\n```";
		}

		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			str
		);
	}


	static void  LLamaPerf_Test12 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8"Write optimized radix sort on glsl for vulkan using only subgroup operations.\n"
			u8"Minimize access to shared memory.\n"
			u8"Don't use loops in single lane like `if (gl_SubgroupInvocationID == 0) { for each lane... }`.\n"
		);

		// answer must not contain loops and Ballot(radix == radix)
	}


	static void  LLamaPerf_Test13 ()
	{
		LLamaPerf_RunTest2(
			Path{OUTPUT_FOLDER} / AE_FUNCTION_NAME,
			u8R"(
This shader place boxes as buildings to create simple streets.
Write new shader which add only street lights:
* must be 2 lines per street
* put them near to buildings, not in the center of street
* must be spot lights with position, direction, angle, height, attenuation
* use DHash to randomize direction, angle
```glsl
		ObjectTransform		obj;
		const uint			idx				= GetGlobalIndex();
		const uint			street_cnt		= 4;
		const uint			street_idx		= idx % street_cnt;
		const uint			building_idx	= idx / street_cnt;
		const float2		uv				= float2( street_idx, building_idx ) / float2( street_cnt, 1 );

		obj.position.x	= (ToSNorm( uv.x ) + 0.25) * 20.0;
		obj.position.y	= GROUND_Y;
		obj.position.z	= uv.y - 5.0;

		obj.scale.x		= 0.5 + DHash12( uv.yx * 111.0 ) * 1.0;
		obj.scale.y		= 0.8 + DHash12( uv * 444.0 ) * 5.0;
		obj.scale.z		= 2.0;

		obj.position.y	-= obj.scale.y;
		obj.position.z	*= obj.scale.z * 2.0;

		obj.color		= packUnorm4x8( float4( DHash32( uv * 333.0 ), 1.0 ));

		un_Objects.elements[idx] = obj;
```
)"
		);
	}
//-----------------------------------------------------------------------------
}


extern void Perf_LLamaTokensPerSecond ()
{
	LLamaPerf_Test1();
	LLamaPerf_Test2();
	LLamaPerf_Test3();
	LLamaPerf_Test4();
//	LLamaPerf_Test5();
	LLamaPerf_Test6();
	LLamaPerf_Test7();
	LLamaPerf_Test8();
	LLamaPerf_Test9();
	LLamaPerf_Test10();
	LLamaPerf_Test11();
	LLamaPerf_Test12();
	LLamaPerf_Test13();
}
