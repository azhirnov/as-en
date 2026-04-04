// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
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
		uint				maxLayers			= 0;
		uint				outTokens			= 0;
		uint				inTokens			= 0;
		bool 				hasErrors			= false;
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


	static void  ClearFileCache ()
	{
		#ifdef AE_PLATFORM_UNIX_BASED
			PlatformUtils::ClearFileCache();
		#endif
	}

	ND_ static String  PrintLine (const PerfTestInput &in, const PerfTestResult &out)
	{
		String	str;
		str << "\n| " << ToString( in.modelPath.stem() ) << " | "
			<< ToString( in.backend ) << " | "
			<< ToString( Min( in.ctxSize, out.maxCtxSize )) << " | "
			<< ToString( out.inTokens ) << " | "
			<< (out.hasErrors ? "(" : "") << ToString( out.outTokens ) << (out.hasErrors ? ")" : "") << " | "
			<< ToString( secondsd{out.modelLoading}.count(), 2 ) << " | "
			<< ToString( secondsd{out.ctxCreation}.count(), 2 ) << " | "
			<< ToString( secondsd{out.timeToFirstToken}.count(), 2 ) << " | "
			<< ToString( secondsd{out.responseGeneration}.count(), 2 ) << " | ";

		if ( in.backend != EBackend::CPU )
			str << ToString(Max( in.gpuLayers, out.maxLayers )) << " / " << ToString( out.maxLayers );
		else
			str << "0 / " << ToString( out.maxLayers );

		str << " | "
			<< ToString( double(out.outTokens) / secondsd{out.responseGeneration}.count(), 2 ) << " |";

		return str;
	}

	static void  PrintResults (ArrayView<PerfTestRequest> results)
	{
		String	str;

		str << "\n| model | backend | ctx size | prompt tokens | response tokens | load (s) | create ctx (s) | time to first token (s) | gen time (s) | GPU layers | tok/sec |"
			<< "\n|-------|---------|----------|---------------|-----------------|----------|----------------|-------------------------|--------------|------------|---------|";

		for (auto& [in, out] : results)
		{
			if ( out.outTokens == 0 )
				continue;

			str << PrintLine( in, out );
		}
		AE_LOGI( str );
	}


	static void  LLamaPerf_Test1 (const PerfTestInput &in, OUT PerfTestResult &result, U8String prompt)
	{
		AE_LOGI( "Try backend: "s << ToString(in.backend) << ", ctx: " << ToString(in.ctxSize) );

		AE_LOGI( "Load model" );

		RC<ILanguageModel>	model;
		{
			ClearFileCache();

			auto	begin = Clock_t::now();

			LLama::OpenParams	params;
			params.modelFile			= in.modelPath;
			params.enableLogger			= true;
			params.keepModelInMemory	= true;
			params.backend				= in.backend;
			params.gpuLayers			= in.gpuLayers;

			model = LMFactory::CreateLLama( params );
			CHECK_ERRV( model );

			result.modelLoading = Clock_t::now() - begin;

			auto	info = model->GetModelInfo();
			result.maxCtxSize	= info.maxContextSize;
			result.maxLayers	= info.layerCount;
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
			params.sampler.minP		= LLama::Sampler_MinP{};

			ctx = model->CreateContext( params );
			CHECK_ERRV( ctx );

			result.ctxCreation = Clock_t::now() - begin;
		}

		AE_LOGI( "Generate response" );
		{
			auto	begin = Clock_t::now();
			auto	listener = MakeRC<ResponseListener>();

			CHECK( ctx->Generate( RVRef(prompt), listener ));

			result.timeToFirstToken		= listener->firstToken - begin;
			result.responseGeneration	= Clock_t::now() - begin;
			result.inTokens				= listener->inTokens;
			result.outTokens			= listener->outTokens;
			result.hasErrors			= listener->hasErrors;

		/*	FileWStream		file { dstFolder / in.modelPath.filename().replace_extension(".md") };
			if ( file.IsOpen() )
			{
				Unused( file.Write( listener->response ));
			}*/
		}
		AE_LOGI( PrintLine( in, result ));
	}


	ND_ static U8String  GeneratePrompt (const Path &folder, Bytes maxSize)
	{
		CHECK_FATAL( FileSystem::IsDirectory( folder ));

		String	prompt	= "Generate clang format rules to match code style in files below.\n\n";
		uint	count	= 0;

		for (auto& entry : FileSystem::Enum( folder ))
		{
			if ( not entry.IsFile() )
				continue;

			const auto	ext = ToString( entry.Get().extension() );
			if ( ext != ".h" and ext != ".cpp" )
				continue;

			if ( prompt.size() > maxSize )
				break;

			FileRStream		file {entry.Get()};
			if ( file.IsOpen() )
			{
				Bytes	size = file.RemainingSize();

				if ( size + prompt.size() > maxSize )
					continue;

				String	temp;
				if ( file.Read( size, OUT temp ))
				{
				//	prompt << ToString( entry.Get().filename() ) << "\n";
					prompt << "```cpp\n";
					prompt << temp;
					prompt << "```\n\n";

					++count;
				}
			}
		}

		CHECK_FATAL( count > 0 );

		return U8String{ Cast<CharUtf8>(prompt.c_str()), prompt.size() };
	}
}


extern void Perf_LLamaContextSize ()
{
	const Path	model_1 = R"(path/to/llm)";

	PerfTestRequest		requests[] =
	{
		{PerfTestInput{ model_1,	8 << 10,	999,	EBackend::CUDA		}, PerfTestResult{}},
		{PerfTestInput{ model_1,	8 << 10,	999,	EBackend::Vulkan	}, PerfTestResult{}},
		{PerfTestInput{ model_1,	8 << 10,	0,		EBackend::CPU		}, PerfTestResult{}},
	};

	const U8String	prompt = GeneratePrompt( Path{TEST_SRC_FOLDER} / "base/Math", 128_KiB );

	for (auto& [in, out] : requests)
	{
		LLamaPerf_Test1( in, out, prompt );
	}

	PrintResults( requests );
}
