// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include <iostream>

#ifdef AE_PLATFORM_WINDOWS
# 	include <conio.h>

	static bool  UserRequestStop ()
	{
		if ( ::_kbhit() )
		{
			int c = ::_getch();
			return c == '`';
		}
		return false;
	}
#endif

#ifdef AE_PLATFORM_LINUX
# 	include <signal.h> 

	std::atomic<bool> s_RequestInterrupt {false};

	static void  OnUserInterrupt (int sig)
	{
		s_RequestInterrupt.store( true );
	}

	static bool  UserRequestStop ()
	{
		return s_RequestInterrupt.load();
	}
#endif

#include "pch/LangModel.h"

using namespace AE;
using namespace AE::LangModel;

static const char	c_ModelPath [] 	= R"(path/to/model)";
static const auto	c_Backend 		= LLama::EGPUBackend::CUDA;
static const uint	c_GPULayers		= 999;

using TimePoint_t	= HighResClock::time_point;
using Duration_t	= HighResClock::duration;


class LoadingListener final : public ILoadingListener
{
private:
	int 	last = -1;
public:
	bool  Progress (Percent pct) __NE_OV
	{
		int val = int(pct.GetPercent());

		if ( last != val )
		{
			AE_LOGI( "Loading "s << ToString( val ) << '%' );
			last = val;
		}

		if ( UserRequestStop() )
			return false;

		return true;  // continue
	}
};


class ResponseListener final : public IResponseListener
{
private:
	U8String		response;
	uint			outTokens		= 0;
	uint			inTokens		= 0;
	uint			maxTokens		= 0;
	TimePoint_t		begin;
	TimePoint_t		firstToken;
	bool 			hasErrors		= false;

public:
	explicit ResponseListener (uint ctxSize) __NE___
	{
		maxTokens 	= ctxSize;
		begin 		= HighResClock::now();
	}

	bool  AppendResponse (U8StringView piece, uint count) __NE_OV
	{
		if ( outTokens == 0 ) {
			firstToken = HighResClock::now();
		}
		outTokens += count;
		std::cout << StringView{ Cast<char>(piece.data()), piece.size() };

		if ( UserRequestStop() )
			return false;

		return true;  // continue
	}
		
	void  OnComplete (U8StringView, uint tokenCount) __NE_OV
	{
		CHECK( outTokens == tokenCount );
		PrintStat();
	}

	void  OnError (ErrorCode code) __NE_OV
	{
		switch_enum( code )
		{
			case ErrorCode::None :						break;
			case ErrorCode::FailedToApplyChatTemplate :	std::cout << "\nFailedToApplyChatTemplate\n";	break;
			case ErrorCode::FailedToTokenizePrompt :	std::cout << "\nFailedToTokenizePrompt\n";		break;
			case ErrorCode::ContextSizeExceeded :		std::cout << "\nContextSizeExceeded\n";			break;
			case ErrorCode::UnknownError :				std::cout << "\nUnknownError\n";				break;
			case ErrorCode::InterruptedByUser :			std::cout << "\nInterruptedByUser\n";			break;
			case ErrorCode::OutOfMemory :				std::cout << "\nOutOfMemory\n";					break;
		}
		switch_end

		hasErrors = true;
		PrintStat();
	}

	void  RequiredPromptTokens (uint count) __NE_OV
	{
		inTokens = count;
	}

	void  PrintStat ()
	{
		auto	ppt  = secondsd{ firstToken - begin };
		auto	gent = secondsd{ HighResClock::now() - firstToken };
		double	ts   = double(outTokens) / gent.count();
		double	pp   = double(inTokens) / ppt.count();
		double	ctx	 = double(inTokens + outTokens) / double(maxTokens);

		std::cout << "\n> "
			<< "ttf: " << ToString( ppt )
			<< ", gen (t/s): " << ToString( ts, 2 )
			<< ", pp (t/s): " << ToString( pp, 2 )
			<< ", in: " << ToString( inTokens )
			<< ", out: " << ToString( outTokens )
			<< ", ctx: " << ToString( int(ctx * 100.0) )
			<< "%\n\n";
	}
};


static RC<ILanguageModelContext>  CreateContext (ILanguageModel &model, uint size)
{
	LLama::ContextParams	params;
	params.contextSize		= size;
	params.threadCount		= UMax;
	params.offloadKQV		= true;
	params.opOffload		= true;

	#if 1
		params.sampler.minP		= LLama::Sampler_MinP{ 0.05f, 1 };
		params.sampler.temperature	= 0.8f;
	#endif
	#if 0
		params.sampler.topK		= LLama::Sampler_TopK{ 40 };
		params.sampler.minP		= LLama::Sampler_MinP{ 0.05f, 1 };
		params.sampler.topP		= LLama::Sampler_TopP{ 0.95f, 1 };
		params.sampler.temperature = 0.6f;
	#endif

	return model.CreateContext( params );
}


static String  SaveContext (ILanguageModelContext &ctx)
{
	const String	fname = Date::Now().ToString( "hh.mi.ss" ) << ".md";

	FileWStream		file {fname};
	CHECK_ERR( file.IsOpen() );

	auto	msgs = ctx.GetMessages();
	for (auto& [role, content] : msgs)
	{
		StringView	role_str;
		switch_enum( role )
		{
			case ERole::User :			role_str = "\n\n# User:\n";		break;
			case ERole::Assistant :		role_str = "\n# Assistant:\n";	break;
			case ERole::System :		role_str = "\n# System:\n";		break;
			case ERole::_Count :		break;
		}
		switch_end

		Unused( file.Write( role_str ));
		Unused( file.Write( content ));
	}

	std::cout << "\n> saved to file '" << fname << "'\n";
	return fname;
}


static void  UserInteraction (ILanguageModel &model)
{
	uint	ctx_size = 16 << 10;

	RC<ILanguageModelContext>	ctx = CreateContext( model, ctx_size );
	CHECK_ERRV( ctx );

	String	cmd_buf;
	cmd_buf.reserve( 1 << 20 );

	const StringView	c_System	= "-system";
	const StringView	c_New		= "-new";
	const StringView	c_CtxSize	= " -size ";

	for (;;)
	{
		std::cout << "\n> ready:\n";

		String	temp;

		cmd_buf.clear();
		std::getline( std::cin, OUT temp );
		
		cmd_buf << temp << '\n';
		

		StringView	cmd {cmd_buf};
		
		if ( StartsWith( cmd, "-exit" ))
			break;

		if ( StartsWith( cmd, c_New ))
		{
			cmd = cmd.substr( c_New.size() );

			if ( StartsWith( cmd, c_CtxSize ))
			{
				cmd = cmd.substr( c_CtxSize.size() );
				ctx_size = StringToUInt( cmd );
			}

			ctx->Clear();

			std::cout << "\nready:\n";
			continue;
		}
		
		if ( StartsWith( cmd, "-save-open" ))
		{
			#ifdef AE_PLATFORM_WINDOWS
				String	cmdline = "open ";
				cmdline << SaveContext( *ctx );

				Unused( WindowsProcess::Execute( cmdline ));
			#endif
			#ifdef AE_PLATFORM_LINUX
				String	cmdline = "xdg-open ";
				cmdline << SaveContext( *ctx );
				
				Unused( LinuxProcess::Execute( cmdline ));
			#endif
			continue;
		}

		if ( StartsWith( cmd, "-save" ))
		{
			SaveContext( *ctx );
			continue;
		}

		if ( StartsWith( cmd, c_System ))
		{
			cmd = cmd.substr( c_System.size() );
			U8String str{ Cast<CharUtf8>(cmd.data()), cmd.size() };

			CHECK_ERRV( ctx->Append( ERole::System, RVRef(str) ));
			continue;
		}
		
		U8String str{ Cast<CharUtf8>(cmd.data()), cmd.size() };
		Unused( ctx->Generate( RVRef(str), MakeRC<ResponseListener>(ctx_size) ));
	}
}


int main (const int argc, char* argv[])
{
	std::filesystem::current_path( Path{argv[0]}.parent_path() );
	Unused( argc );

	StaticLogger::LoggerScope log{0};
	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
	StaticLogger::AddLogger( ILogger::CreateIDEOutput() );

	#ifdef AE_PLATFORM_LINUX
  		::signal( SIGINT, OnUserInterrupt ); 
	#endif

	RC<ILanguageModel>	model;
	{
		LLama::OpenParams	params;
		params.modelFile			= c_ModelPath;
		params.enableLogger			= true;
		params.keepModelInMemory	= true;
		params.useMMap				= true;
		params.gpuBackend			= c_Backend;
		params.gpuLayers			= c_GPULayers;

		#if 1
			params.listener = MakeRC<LoadingListener>();

			model = LMFactory::CreateLLama( params );
			CHECK_ERR( model, -1 );
		#else
			// remote
			Remote::OpenParams	r_params;
			r_params.addr		= Networking::IpAddress::FromInt( 192,168,0,100, 3000 );	// set your server address
			r_params.listener	= MakeRC<LoadingListener>();
			r_params.llama		= MakeUnique<LLama::OpenParams>( params );
			
			model = LMFactory::CreateRemote( r_params );
			CHECK_ERR( model, -1 );
		#endif

		Path	folder = Path{OUTPUT_FOLDER} / Date::Now().ToString( "yy.mm.dm" );

		FileSystem::CreateDirectories( folder );
		CHECK_ERR( FileSystem::SetCurrentPath( folder ), -1 );
	}

	UserInteraction( *model );

	return 0;
}
