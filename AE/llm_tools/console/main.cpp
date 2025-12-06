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

namespace
{
	using namespace AE;
	using namespace AE::LangModel;

	static uint		g_MaxCtxSize	= 0;

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

		ND_ bool			HasErrors ()	C_NE___	{ return hasErrors; }
		ND_ U8StringView	GetResponse ()	C_NE___	{ return response; }
	};


	static RC<ILanguageModelContext>  CreateContext (ILanguageModel &model, uint size)
	{
		LLama::ContextParams	params;
		params.contextSize		= size;
		params.threadCount		= UMax;
		params.offloadKQV		= true;
		params.opOffload		= true;

		#if 1
			params.sampler.minP			= LLama::Sampler_MinP{ 0.05f, 1 };
			params.sampler.temperature	= 0.8f;
			params.sampler.penalties	= LLama::Sampler_Penalties{ 64, 1.1f, 0.f, 0.f };
		#endif
		#if 0
			params.sampler.topK		= LLama::Sampler_TopK{ 40 };
			params.sampler.minP		= LLama::Sampler_MinP{ 0.05f, 1 };
			params.sampler.topP		= LLama::Sampler_TopP{ 0.95f, 1 };
			params.sampler.temperature = 0.6f;
		#endif

		return model.CreateContext( params );
	}


	static void  FixUnicode (INOUT U8String &str)
	{
		const CharUtf8	c0[] = { CharUtf8(226), CharUtf8(128), CharUtf8(145) };

		FindAndReplace( str, U8StringView{ c0, CountOf(c0) }, u8"-" );
	}


	static void  WrapThinkingBlock (INOUT U8String &str)
	{
		const U8StringView	block_begin	= u8"<|channel|>analysis<|message|>";
		const U8StringView	block_end	= u8"<|end|><|start|>assistant<|channel|>final<|message|>";

		{
			usize	pos = str.find( block_begin );
			if ( pos < str.size() )
				str.insert( pos, u8"\n<details><summary>Thinking</summary>\n\n" );
		}{
			usize	pos = str.find( block_end );
			if ( pos < str.size() )
				str.insert( pos, u8"\n\n</details>\n" );
		}
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

			FixUnicode( INOUT content );
			WrapThinkingBlock( INOUT content );

			Unused( file.Write( role_str ));
			Unused( file.Write( content ));
		}
		Unused( file.Write( "\n\n\n\n"sv ));

		String	full_path = ToString( FileSystem::ToAbsolute( fname ));

		std::cout << "\n> saved to file '" << full_path << "'\n";
		return full_path;
	}
//-----------------------------------------------------------------------------


	struct CommandCtx
	{
		ILanguageModel &			model;
		RC<ILanguageModelContext>	ctx;
		uint						ctxSize		= g_MaxCtxSize;
		bool						success		= true;
		StringView					prompt;

		explicit CommandCtx (ILanguageModel &model) __NE___ : model{model} {}
	};

	static void  CmdHelp (CommandCtx &);


	static void  CmdExit (CommandCtx &cmd)
	{
		cmd.success = false;
	}


	static void  CmdAddSystemMsg (CommandCtx &cmd)
	{
		U8String str{ Cast<CharUtf8>(cmd.prompt.data()), cmd.prompt.size() };
		CHECK( cmd.ctx->Append( ERole::System, RVRef(str) ));
	}


	static void  CmdStartNewContext (CommandCtx &cmd)
	{
		const StringView	c_CtxSize = "-size ";

		if ( StartsWith( cmd.prompt, c_CtxSize ))
		{
			cmd.prompt = cmd.prompt.substr( c_CtxSize.size() );
			cmd.ctxSize = StringToUInt( cmd.prompt );
		}

		cmd.ctx = null;
		cmd.ctx = CreateContext( cmd.model, cmd.ctxSize );
		cmd.success = bool{cmd.ctx};
	}


	static void  CmdSaveContext (CommandCtx &cmd)
	{
		SaveContext( *cmd.ctx );
	}


	static void  CmdSaveContextAndOpen (CommandCtx &cmd)
	{
	#ifdef AE_PLATFORM_WINDOWS
		String	cmdline = "start \"";
		cmdline << SaveContext( *cmd.ctx ) << '"';
		Unused( WindowsProcess::Execute( cmdline, WindowsProcess::EFlags::UsePowerShell | WindowsProcess::EFlags::NoWindow ));
	#endif
	#ifdef AE_PLATFORM_LINUX
		String	cmdline = "xdg-open \"";
		cmdline << SaveContext( *cmd.ctx ) << '"';
		Unused( UnixProcess::Execute( cmdline ));
	#endif
	}


	static void  CmdLoadContext (CommandCtx &)
	{
		// TODO
	}


	static void  CmdRefactorShader (CommandCtx &cmd)
	{
		LLama::ContextParams	params;
		params.contextSize		= cmd.ctxSize;
		params.threadCount		= UMax;
		params.offloadKQV		= true;
		params.opOffload		= true;

		params.sampler.temperature	= 0.3f;
		params.sampler.minP			= LLama::Sampler_MinP{ 0.05f, 1 };
		params.sampler.topP			= LLama::Sampler_TopP{ 0.9f, 1 };
		params.sampler.penalties	= LLama::Sampler_Penalties{ 64, 1.1f, 0.f, 0.f };

		cmd.ctx = null;
		cmd.ctx = cmd.model.CreateContext( params );
		cmd.success = bool{cmd.ctx};
		if ( not cmd.success )
			return;

		{
			U8String	str = u8"Reasoning: low";
			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}{
			U8String	str = u8R"(
You are a GLSL refactoring assistant. Improve readability of a disassembled shader while preserving exact behavior.

Strict rules:
* Do not omit or collapse any code. No ellipses. Return the entire rewritten shader.
* If the full output won’t fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* Put comments inline, not as separate paragraphs.
* If you think a block is dead or redundant, keep it and mark it with a comment rather than deleting it.
* Rename sampler, UBO, SSBO declarations. Do not change binding indices.
* Replace spvNMin to min, etc. Replace spirv intrinsics by GLSL functions.

Refactoring goals:
* Replace anonymous temps with descriptive names.
* Group related code into small helper functions without changing behavior.
* Add short, high-signal comments explaining each block’s purpose.
* Keep control flow structure; only clarify it (e.g., if/else formatting).
)";
			/*
* Keep vector/matrix math ordering as in original to avoid precision drift.
			*/

			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}

		for (bool first = true;; first = false)
		{
			RC<ResponseListener>	listener = MakeRC<ResponseListener>(cmd.ctxSize);
			U8String				str;

			if_unlikely( first )
			{
				String	ansi_str;
				CHECK( PlatformUtils::ClipboardExtract( OUT ansi_str ));
				CHECK( not ansi_str.empty() );

				ansi_str << "\n```";
				"Now refactor this shader:\n```\n" >> ansi_str;

				str = U8String{ Cast<CharUtf8>(ansi_str.data()), ansi_str.size() };
			}
			else
			{
				str = u8"CONTINUE";
			}

			CHECK_ERRV( cmd.ctx->Generate( RVRef(str), listener ));

			if ( listener->HasErrors() )
				break;

			bool	has_next_part = listener->GetResponse().contains( u8"CONTINUE" );
			has_next_part |= listener->GetResponse().contains( u8"next part" );

			if ( not has_next_part )
				break;
		}
	}


	static void  CmdRefactorDXIL (CommandCtx &cmd)
	{
		LLama::ContextParams	params;
		params.contextSize		= cmd.ctxSize;
		params.threadCount		= UMax;
		params.offloadKQV		= true;
		params.opOffload		= true;

		params.sampler.temperature	= 0.2f;
		params.sampler.minP			= LLama::Sampler_MinP{ 0.05f, 1 };
		params.sampler.topP			= LLama::Sampler_TopP{ 0.3f, 1 };
		params.sampler.penalties	= LLama::Sampler_Penalties{ 64, 1.1f, 0.f, 0.f };

		cmd.ctx = null;
		cmd.ctx = cmd.model.CreateContext( params );
		cmd.success = bool{cmd.ctx};
		if ( not cmd.success )
			return;

		{
			U8String	str = u8"Reasoning: low";
			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}{
			U8String	str = u8R"(
You are a decompiler specialized in converting DirectX DXIL disassembly into equivalent, readable HLSL with strict fidelity.
Your primary objectives are:
* Do not omit or elide any behavior. No “...” or “skipping for brevity”.
* Preserve semantics, constants, resource bindings, and control flow.
* Make code readable HLSL while annotating anything uncertain.
* If the full output won’t fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* If you cannot confidently reconstruct a piece, include a TODO comment and embed the original DXIL snippet as a comment right above the HLSL you derived from it.
* Keep resource bindings (t#, s#, u#, b#, space#) as in DXIL.
)";

			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}

		for (bool first = true;; first = false)
		{
			RC<ResponseListener>	listener = MakeRC<ResponseListener>(cmd.ctxSize);
			U8String				str;

			if_unlikely( first )
			{
				String	ansi_str;
				CHECK( PlatformUtils::ClipboardExtract( OUT ansi_str ));
				CHECK( not ansi_str.empty() );

				ansi_str << "\n```";
				"Now refactor this shader:\n```\n" >> ansi_str;

				str = U8String{ Cast<CharUtf8>(ansi_str.data()), ansi_str.size() };
			}
			else
			{
				str = u8"CONTINUE";
			}

			CHECK_ERRV( cmd.ctx->Generate( RVRef(str), listener ));

			if ( listener->HasErrors() )
				break;

			bool	has_next_part = listener->GetResponse().contains( u8"CONTINUE" );
			has_next_part |= listener->GetResponse().contains( u8"next part" );

			if ( not has_next_part )
				break;
		}
	}


	static void  CmdRefactorDXBC (CommandCtx &cmd)
	{
		LLama::ContextParams	params;
		params.contextSize		= cmd.ctxSize;
		params.threadCount		= UMax;
		params.offloadKQV		= true;
		params.opOffload		= true;

		params.sampler.temperature	= 0.2f;
		params.sampler.minP			= LLama::Sampler_MinP{ 0.05f, 1 };
		params.sampler.topP			= LLama::Sampler_TopP{ 0.3f, 1 };
		params.sampler.penalties	= LLama::Sampler_Penalties{ 64, 1.1f, 0.f, 0.f };

		cmd.ctx = null;
		cmd.ctx = cmd.model.CreateContext( params );
		cmd.success = bool{cmd.ctx};
		if ( not cmd.success )
			return;

		{
			U8String	str = u8"Reasoning: low";
			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}{
			U8String	str = u8R"(
You are a decompiler specialized in converting DirectX DXBC disassembly into equivalent, readable HLSL with strict fidelity.
Your primary objectives are:
* Do not omit or elide any behavior. No “...” or “skipping for brevity”.
* Preserve semantics, constants, resource bindings, and control flow.
* Make code readable HLSL while annotating anything uncertain.
* If the full output won’t fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* If you cannot confidently reconstruct a piece, include a TODO comment and embed the original DXBC snippet as a comment right above the HLSL you derived from it.
* Keep resource bindings (t#, s#, u#, b#) as in DXBC.
)";

			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}

		for (bool first = true;; first = false)
		{
			RC<ResponseListener>	listener = MakeRC<ResponseListener>(cmd.ctxSize);
			U8String				str;

			if_unlikely( first )
			{
				String	ansi_str;
				CHECK( PlatformUtils::ClipboardExtract( OUT ansi_str ));
				CHECK( not ansi_str.empty() );

				ansi_str << "\n```";
				"Now refactor this shader:\n```\n" >> ansi_str;

				str = U8String{ Cast<CharUtf8>(ansi_str.data()), ansi_str.size() };
			}
			else
			{
				str = u8"CONTINUE";
			}

			CHECK_ERRV( cmd.ctx->Generate( RVRef(str), listener ));

			if ( listener->HasErrors() )
				break;

			bool	has_next_part = listener->GetResponse().contains( u8"CONTINUE" );
			has_next_part |= listener->GetResponse().contains( u8"next part" );

			if ( not has_next_part )
				break;
		}
	}


	static void  CmdPasteFromClipboard (CommandCtx &cmd)
	{
		String	str;
		CHECK( PlatformUtils::ClipboardExtract( OUT str ));
		CHECK( not str.empty() );

		U8String ustr{ Cast<CharUtf8>(str.data()), str.size() };

		U8StringView{ Cast<CharUtf8>(cmd.prompt.data()), cmd.prompt.size() } >> ustr;
		CHECK( cmd.ctx->Append( ERole::User, RVRef(ustr) ));
	}


	static void  CmdPasteCodeFromClipboard (CommandCtx &cmd)
	{
		String	str;
		CHECK( PlatformUtils::ClipboardExtract( OUT str ));
		CHECK( not str.empty() );

		str << "\n```";
		"```\n" >> str;

		U8String ustr{ Cast<CharUtf8>(str.data()), str.size() };

		U8StringView{ Cast<CharUtf8>(cmd.prompt.data()), cmd.prompt.size() } >> ustr;
		CHECK( cmd.ctx->Append( ERole::User, RVRef(ustr) ));
	}


	static void  CmdCopyLastResponseToClipboard (CommandCtx &cmd)
	{
		auto	msgs = cmd.ctx->GetMessages();
		if ( msgs.empty() )
		{
			std::cout << "nothing to copy\n";
			return;
		}

		CHECK_ERRV( msgs.back().first == ERole::Assistant );

		FixUnicode( INOUT msgs.back().second );
		WrapThinkingBlock( INOUT msgs.back().second );

		CHECK_ERRV( PlatformUtils::ClipboardPut( NtStringView{ Cast<char>(msgs.back().second.c_str()),
																msgs.back().second.size() }));
		std::cout << "> copied to clipboard\n";
	}


	static void  CmdCopyAllMessagesToClipboard (CommandCtx &cmd)
	{
		auto	msgs = cmd.ctx->GetMessages();
		if ( msgs.empty() )
		{
			std::cout << "nothing to copy\n";
			return;
		}
		U8String	str;
		for (auto& [role, content] : msgs)
		{
			U8StringView	role_str;
			switch_enum( role )
			{
				case ERole::User :			role_str = u8"\n\n# User:\n";		break;
				case ERole::Assistant :		role_str = u8"\n# Assistant:\n";	break;
				case ERole::System :		role_str = u8"\n# System:\n";		break;
				case ERole::_Count :		break;
			}
			switch_end

			FixUnicode( INOUT content );
			WrapThinkingBlock( INOUT content );
			str << role_str << content;
		}

		CHECK_ERRV( PlatformUtils::ClipboardPut( NtStringView{ Cast<char>(str.c_str()), str.size() }));
		std::cout << "> copied to clipboard\n";
	}


	static FlatHashMap< StringView, void (*) (CommandCtx &) >		g_Commands = {
		{ "-exit",				&CmdExit },
		{ "-help",				&CmdHelp },
		{ "-system",			&CmdAddSystemMsg },
		{ "-new",				&CmdStartNewContext },
		{ "-save",				&CmdSaveContext },
		{ "-save-open",			&CmdSaveContextAndOpen },
		{ "-load",				&CmdLoadContext },
		{ "-refactor-shader",	&CmdRefactorShader },
		{ "-refactor-dxil",		&CmdRefactorDXIL },
		{ "-refactor-dxbc",		&CmdRefactorDXBC },
		{ "-paste",				&CmdPasteFromClipboard },
		{ "-code",				&CmdPasteCodeFromClipboard },
		{ "-copy",				&CmdCopyLastResponseToClipboard },
		{ "-copy-all",			&CmdCopyAllMessagesToClipboard },
		// TODO: save context/summary and continue with new
	};


	static void  CmdHelp (CommandCtx &)
	{
		String	msg = "> list of available commands:";

		for (auto& cmd : g_Commands) {
			msg << "\n    " << cmd.first;
		}
		std::cout << msg << '\n';
	}
//-----------------------------------------------------------------------------


	static void  UserInteraction (ILanguageModel &model)
	{
		CommandCtx	cmd_ctx{ model };

		cmd_ctx.ctx = CreateContext( cmd_ctx.model, cmd_ctx.ctxSize );
		CHECK_ERRV( cmd_ctx.ctx );

		String	cmd_buf;
		cmd_buf.reserve( 1 << 20 );

		for (; cmd_ctx.success;)
		{
			std::cout << "\n> ready:\n";

			String	temp;

			cmd_buf.clear();
			std::getline( std::cin, OUT temp );		// TODO: utf8

			if ( temp.empty() )
				continue;

			cmd_buf << temp << '\n';

			cmd_ctx.prompt = StringView{cmd_buf};

			if ( cmd_ctx.prompt[0] == '-' )
			{
				usize		end			= Min( cmd_ctx.prompt.find( ' ' ), cmd_ctx.prompt.find( '\n' ));
				StringView	cmd_name	= SubString( cmd_ctx.prompt, 0, end );

				auto	it = g_Commands.find( cmd_name );
				if ( it == g_Commands.end() )
				{
					std::cout << "> unknown command: '" << cmd_name << "'\n";
					continue;
				}

				cmd_ctx.prompt = SubString( cmd_ctx.prompt, end+1, UMax );
				it->second( cmd_ctx );
				continue;
			}

			std::cout << "\n> processing prompt...\n";

			U8String str{ Cast<CharUtf8>(cmd_ctx.prompt.data()), cmd_ctx.prompt.size() };
			Unused( cmd_ctx.ctx->Generate( RVRef(str), MakeRC<ResponseListener>(cmd_ctx.ctxSize) ));
		}
	}


	static void  SelectModelParams (StringView pcName, StringView userName, INOUT LLama::OpenParams &params)
	{

		CHECK_MSG( false, "no configuration for PC "s << pcName );
	}

} // namespace


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
		params.enableLogger			= true;
		params.keepModelInMemory	= true;
		params.useMMap				= true;

		SelectModelParams( PlatformUtils::GetComputerName(), PlatformUtils::GetUserName(), INOUT params );


		#if 1
			params.listener = MakeRC<LoadingListener>();

			model = LMFactory::CreateLLama( params );
			CHECK_ERR( model, -1 );
		#else
			// remote
			Remote::OpenParams	r_params;
			r_params.addr		= AE_LLM_IPv4;
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
