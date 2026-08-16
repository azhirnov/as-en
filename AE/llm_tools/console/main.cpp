// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
				str.insert( pos + block_end.size(), u8"\n\n</details>\n\n" );
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

		std::cout << "\n> closing....\n";
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


	static void  CmdRefactorImpl (CommandCtx &cmd, U8String systemMsg)
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
			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(systemMsg) ));
		}

		for (bool first = true;; first = false)
		{
			RC<ResponseListener>	listener = MakeRC<ResponseListener>(cmd.ctxSize);
			U8String				str;

			if_unlikely( first )
			{
				CHECK( PlatformUtils::ClipboardExtract( OUT str ));
				CHECK( not str.empty() );

				std::cout << "> used code from clipboard " << ToString( Bytes{str.size()} ) << " size\n";

				str << u8"\n```";
				u8"Now refactor this shader:\n```\n" >> str;
			}
			else
			{
				str = u8"CONTINUE";

				std::cout << "> paste CONTINUE\n";
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


	static void  CmdRefactorGLSL (CommandCtx &cmd)
	{
		U8String	str = u8R"(
You are a GLSL refactoring assistant. Improve readability of a disassembled shader while preserving exact behavior.

Strict rules:
* Do not omit or collapse any code. No ellipses. Return the entire rewritten shader.
* If the full output won't fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* Put comments inline, not as separate paragraphs.
* If you think a block is dead or redundant, keep it and mark it with a comment rather than deleting it.
* Rename sampler, UBO, SSBO declarations. Do not change binding indices.
* Replace spvNMin to min, etc. Replace spirv intrinsics by GLSL functions.

Refactoring goals:
* Replace anonymous temps with descriptive names.
* Group related code into small helper functions without changing behavior.
* Add short, high-signal comments explaining each block's purpose.
* Keep control flow structure; only clarify it (e.g., if/else formatting).
)";
		/*
* Keep vector/matrix math ordering as in original to avoid precision drift.
		*/

		CmdRefactorImpl( cmd, RVRef(str) );
	}


	static void  CmdRefactorHLSL (CommandCtx &cmd)
	{
		U8String	str = u8R"(
You are a HLSL refactoring assistant. Improve readability of a disassembled shader while preserving exact behavior.

Strict rules:
* Do not omit or collapse any code. No ellipses. Return the entire rewritten shader.
* If the full output won't fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* Put comments inline, not as separate paragraphs.
* If you think a block is dead or redundant, keep it and mark it with a comment rather than deleting it.
* Rename sampler, texture, all buffer types, UAV declarations. Do not change binding indices.

Refactoring goals:
* Replace anonymous temps with descriptive names.
* Group related code into small helper functions without changing behavior.
* Add short, high-signal comments explaining each block's purpose.
* Keep control flow structure; only clarify it (e.g., if/else formatting).
)";
		CmdRefactorImpl( cmd, RVRef(str) );
	}


	static void  CmdRefactorDXIL (CommandCtx &cmd)
	{
		U8String	str = u8R"(
You are a decompiler specialized in converting DirectX DXIL disassembly into equivalent, readable HLSL with strict fidelity.
Your primary objectives are:
* Do not omit or elide any behavior. No “...” or “skipping for brevity”.
* Preserve semantics, constants, resource bindings, and control flow.
* Make code readable HLSL while annotating anything uncertain.
* If the full output won't fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* If you cannot confidently reconstruct a piece, include a TODO comment and embed the original DXIL snippet as a comment right above the HLSL you derived from it.
* Keep resource bindings (t#, s#, u#, b#, space#) as in DXIL.
* Replace anonymous temps with descriptive names.
* Reason in at most 10 steps.
* If you cannot make further progress, just say: "I'm done." and stop.
* Never restate the same reasoning more than once.
)";
		CmdRefactorImpl( cmd, RVRef(str) );
	}


	static void  CmdRefactorDXBC (CommandCtx &cmd)
	{
			U8String	str = u8R"(
You are a decompiler specialized in converting DirectX DXBC disassembly into equivalent, readable HLSL with strict fidelity.
Your primary objectives are:
* Do not omit or elide any behavior. No “...” or “skipping for brevity”.
* Preserve semantics, constants, resource bindings, and control flow.
* Make code readable HLSL while annotating anything uncertain.
* If the full output won't fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* If you cannot confidently reconstruct a piece, include a TODO comment and embed the original DXBC snippet as a comment right above the HLSL you derived from it.
* Keep resource bindings (t#, s#, u#, b#) as in DXBC.
* Reason in at most 10 steps.
* If you cannot make further progress, just say: "I'm done." and stop.
* Never restate the same reasoning more than once.
)";
		CmdRefactorImpl( cmd, RVRef(str) );
	}


	static void  CmdRefactorLargeDXIL_part1 (CommandCtx &cmd)
	{
		/*
		Step A: Extract high-level interface
		“From this DXIL disasm, extract the HLSL interface: constant buffers, textures, samplers, and input/output structs. Don’t decompile the code yet; just write HLSL declarations that match the binding slots and semantics.”

		pre B:
		divide dxil on logical blocks for further decompilation.
		output same assemble with added comments `//` with begin and end of logical block.
		don't copy full block just some lines after BEGIN and before END of logical block.


		Step B: Reconstruct small code sections
		For each chunk of code (function or part of main), send only that part plus the interface and ask:
		“Here’s part of a DXIL disassembly and the HLSL interface we established. Please explain in English what this block does, and then propose equivalent HLSL for just this block. Use the resource/struct names from the interface.”


		Step C
		“Here is the reassembled HLSL from several decompiled pieces. Please:
		* Check for obvious logical errors/inconsistencies.
		* Suggest simplifications/cleanups without changing behavior.”
		*/

		// just text processing DXIL -> HLSL without refactoring
		// when complete, try to use '-refactor-hlsl'

		U8String	str = u8R"(
You will be given DXIL disassembly.

Goal:
- Convert the DXIL disassembly to valid HLSL that compiles to an equivalent shader.
- Do NOT refactor or “clean up” the logic. Preserve the structure and behavior as closely as possible.

Hard constraints:

1. Do NOT change control flow:
   - Preserve all branching, loops, early returns, and discards exactly.
   - Keep the same order of operations and computations.

2. Do NOT introduce new abstractions:
   - No new helper functions.
   - No restructuring of code into different functions, classes, or files.

3. Naming rules (important):
   - Any DXIL identifier that begins with `%` MUST be converted to an HLSL identifier that begins with `_`.
	 - Examples: `%0` → `_0`, `%tmp12` → `_tmp12`, `%myVar` → `_myVar`.
   - The rest of the identifier (after the `%`) must remain identical.
   - Do NOT invent “nicer” names; use only the mechanical `%` → `_` substitution.
   - If the DXIL name does not start with `%`, keep it as-is unless it is illegal in HLSL; only then fix it minimally.

4. Do NOT optimize:
   - Do not merge instructions.
   - Do not remove “redundant” operations, dead code, or constants.
   - Do not simplify algebraic expressions or constant-fold.

5. Do NOT change resource layout:
   - Keep the same binding slots, register spaces, and resource types (cbuffer, Texture2D, RWTexture2D, SamplerState, etc.).
   - If a specific name is not present, invent only minimal descriptive names like `cbuffer0`, `tex0`, `uav0`.

6. Preserve precision and types:
   - Respect `min16float`, `float`, `int`, `uint`, `bool` as implied by the DXIL.
   - Keep vector/scalar widths (float4, float3, etc.) as indicated by the operations.

7. Preserve semantics:
   - Preserve semantics such as SV_Position, SV_Target, SV_DispatchThreadID, etc., based on the DXIL signatures.
   - Maintain input/output parameter counts and semantics.

8. Logical block comments (important):
   - Divide the resulting HLSL into logical blocks using **only comments**.
   - Comments must NOT alter control flow or grouping of statements; they only annotate existing structure.
   - Use short, neutral labels such as:
	 - `// --- Input loads ---`
	 - `// --- Constants / cbuffers ---`
	 - `// --- Main computation block ---`
	 - `// --- Output writes ---`
	 - `// --- Loop body ---`
   - Place these comments at natural boundaries (e.g., before a group of loads, before a loop, before output stores).
   - Do NOT move, merge, or split HLSL statements just to fit comments; insert comments around the existing statement order only.

9. Other comments:
   - You may add brief comments showing which DXIL instructions each HLSL line came from **if helpful**, but keep them short.
   - Do not add high-level explanations, “cleanup” notes, or suggestions.

Output format:
- Output **only** the final HLSL shader code in a single code block.
- If there are multiple entry points, output each as a separate HLSL function in the same file.
- Do not include explanations or prose outside the code block unless I explicitly ask.

If any aspect of the DXIL is ambiguous:
- Make the minimal, most literal choice.
- Do NOT add behavior that is not clearly implied by the DXIL.
- If something is impossible to infer (e.g., exact original struct name), choose a generic placeholder and proceed.

If you are unsure whether to simplify or refactor something, choose the option that is **more literal** and **closer to the DXIL**.
Do not try to make the code “nicer” or “more idiomatic” HLSL.
)";
		CmdRefactorImpl( cmd, RVRef(str) );
	}


	static void  CmdRefactorLargeDXIL_part2 (CommandCtx &cmd)
	{
			U8String	str = u8R"(
You are a decompiler specialized in converting DirectX DXIL disassembly into equivalent, readable HLSL with strict fidelity.
Your primary objectives are:
* Do not omit or elide any behavior. No “...” or “skipping for brevity”.
* Preserve semantics, constants, resource bindings, and control flow.
* Make code readable HLSL while annotating anything uncertain.
* If the full output won't fit in one response, split it into numbered parts and stop mid-token only at safe boundaries. Begin with: PART 1/N — tell me to reply “CONTINUE” for the next part.
* If you cannot confidently reconstruct a piece, include a TODO comment and embed the original DXIL snippet as a comment right above the HLSL you derived from it.
* Keep resource bindings (t#, s#, u#, b#, space#) as in DXIL.
* When renaming add comment with origin name like: `// _500`
)";

			// avoid reasoning loops
			str << u8R"(
* Reason in at most 10 steps.
* If you cannot make further progress, just say: "I'm done." and stop.
* Never restate the same reasoning more than once.
)";
			// add rename table
			str << u8R"(
Additionally output a full, machine-readable rename map in this format:
```
RENAMES:
<kind>: <old_name> -> <new_name>
<kind>: <old_name> -> <new_name>
...
END RENAMES
```
)";
			// use rename table
			str << u8R"(
When you are given a rename table in this format:
```
RENAMES:
<kind>: <old_name> -> <new_name>
...
END RENAMES
```
you must:
1. Apply all renames consistently:
	* For every `<old_name>` in the table, replace it with `<new_name>`
	* In all code you are asked to modify.
	* In related references, imports, calls, and type annotations.

2. Do not invent or change the mapping
	* Do not introduce extra renames that are not in the table, unless the user explicitly asks for additional refactors.

3. Output requirements
	* Show the updated/refactored code as requested by the user.
	* If you also perform new renames beyond the table (only if explicitly asked), you must output an updated `RENAMES` block including both:
		* The original entries (unchanged).
		* Any additional renames you introduced.
	* If you only applied the existing table and added no new renames, you may omit a new `RENAMES` block unless the user asks for it.
)";
		str << u8R"(
Here is rename table from previous parts:
)";

		CmdRefactorImpl( cmd, RVRef(str) );
	}


	static void  CmdPasteFromClipboard (CommandCtx &cmd)
	{
		U8String	str;
		CHECK( PlatformUtils::ClipboardExtract( OUT str ));
		CHECK( not str.empty() );

		U8StringView{ Cast<CharUtf8>(cmd.prompt.data()), cmd.prompt.size() } >> str;
		CHECK( cmd.ctx->Append( ERole::User, RVRef(str) ));
	}


	static void  CmdPasteCodeFromClipboard (CommandCtx &cmd)
	{
		U8String	str;
		CHECK( PlatformUtils::ClipboardExtract( OUT str ));
		CHECK( not str.empty() );

		str << u8"\n```";
		u8"```\n" >> str;

		U8StringView{ Cast<CharUtf8>(cmd.prompt.data()), cmd.prompt.size() } >> str;
		CHECK( cmd.ctx->Append( ERole::User, RVRef(str) ));
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

		CHECK_ERRV( PlatformUtils::ClipboardPut( msgs.back().second ));
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

		CHECK_ERRV( PlatformUtils::ClipboardPut( str ));
		std::cout << "> copied to clipboard\n";
	}


	static void  CmdTranslateRuEn (CommandCtx &cmd)
	{
		// prefer to use DeepSeek-R1 / v3 or Qwen 2.5 7B / 14B / 32B or LLaMA 3.1

		// new context
		LLama::ContextParams	params;
		params.contextSize		= cmd.ctxSize;
		params.threadCount		= UMax;
		params.offloadKQV		= true;
		params.opOffload		= true;

		params.sampler.temperature	= 0.3f;
		params.sampler.minP			= LLama::Sampler_MinP{ 0.05f, 1 };
		params.sampler.topP			= LLama::Sampler_TopP{ 0.8f, 1 };

		cmd.ctx = null;
		cmd.ctx = cmd.model.CreateContext( params );
		cmd.success = bool{cmd.ctx};
		if ( not cmd.success )
			return;

		{
			U8String	str = u8R"(
Translate the following Russian text to clear, idiomatic English.

Requirements:
- Translate everything line by line.
- Do not add or remove sentences.
- If a word is unknown, copy it as-is in Latin letters.
- Preserve all C++ code, identifiers, function/class names, urls, and graphics API names (OpenGL, DirectX, Vulkan, shaders, etc.) exactly as in the original.
- Do NOT translate comments in code sections.
- Do NOT add explanations, comments, or extra text. Only output the translation.
)";
			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}

		U8String	str;
		{
			CHECK( PlatformUtils::ClipboardExtract( OUT str ));
			CHECK( not str.empty() );
			u8"Russian:\n" >> str;
		}

		RC<ResponseListener>	listener = MakeRC<ResponseListener>(cmd.ctxSize);
		CHECK_ERRV( cmd.ctx->Generate( RVRef(str), listener ));
	}


	static void  CmdTranslateEnRu (CommandCtx &cmd)
	{
		// new context
		cmd.ctx = null;
		cmd.ctx = CreateContext( cmd.model, cmd.ctxSize );
		cmd.success = bool{cmd.ctx};
		CHECK_ERRV( cmd.success );

		{
			U8String	str = u8R"(
Task: Translate English technical text to Russian.

Requirements:
- Translate everything line by line.
- Do not add or remove sentences.
- Preserve all C++ code, identifiers, function/class names, and graphics API names (OpenGL, DirectX, Vulkan, shaders, etc.) exactly as in the original.
- Translate text into natural, professional Russian used in technical documentation.
- Keep technical nuance and terminology accurate (e.g., "framebuffer", "render pass", "shader compilation").
- Do NOT translate comments in code sections.
- Do NOT add explanations, comments, or extra text. Only output the translation.
)";
			CHECK_ERRV( cmd.ctx->Append( ERole::System, RVRef(str) ));
		}

		U8String	str;
		{
			CHECK( PlatformUtils::ClipboardExtract( OUT str ));
			CHECK( not str.empty() );

			u8"English:\n" >> str;
		}

		RC<ResponseListener>	listener = MakeRC<ResponseListener>(cmd.ctxSize);
		CHECK_ERRV( cmd.ctx->Generate( RVRef(str), listener ));
	}


	static FlatHashMap< StringView, void (*) (CommandCtx &) >		g_Commands = {
		{ "-exit",					&CmdExit },
		{ "-help",					&CmdHelp },
		{ "-system",				&CmdAddSystemMsg },
		{ "-new",					&CmdStartNewContext },
		{ "-save",					&CmdSaveContext },
		{ "-save-open",				&CmdSaveContextAndOpen },
		{ "-load",					&CmdLoadContext },
		{ "-refactor-glsl",			&CmdRefactorGLSL },
		{ "-refactor-hlsl",			&CmdRefactorHLSL },
		{ "-refactor-dxil",			&CmdRefactorDXIL },
		{ "-refactor-large-dxil-1",	&CmdRefactorLargeDXIL_part1 },
		{ "-refactor-large-dxil-2",	&CmdRefactorLargeDXIL_part2 },
		{ "-refactor-dxbc",			&CmdRefactorDXBC },
		{ "-paste",					&CmdPasteFromClipboard },
		{ "-code",					&CmdPasteCodeFromClipboard },
		{ "-copy",					&CmdCopyLastResponseToClipboard },
		{ "-copy-all",				&CmdCopyAllMessagesToClipboard },
		{ "-translate-ru-en",		&CmdTranslateRuEn },
		{ "-translate-en-ru",		&CmdTranslateEnRu },
		// TODO:
		//	save context/summary and continue with new
		//	-refactor-names
		//	planing mode
		//	write formula as code with physical dimensions
	};


	static void  CmdHelp (CommandCtx &)
	{
		String	msg = "> list of available commands:";

		Array<StringView>	sorted;
		for (auto& cmd : g_Commands) {
			sorted.push_back( cmd.first );
		}

		std::sort( sorted.begin(), sorted.end() );

		for (auto& cmd : sorted) {
			msg << "\n    " << cmd;
		}
		msg << "\n> also press tilde ~ to stop generation";

		std::cout << msg << std::endl;
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


int main (const int argc, char const* argv[])
{
	std::filesystem::current_path( Path{argv[0]}.parent_path() );
	Unused( argc );

	StaticLogger::LoggerScope log{0};
	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
	StaticLogger::AddLogger( ILogger::CreateIDEOutput() );

	#ifdef AE_PLATFORM_LINUX
		::signal( SIGINT, OnUserInterrupt );
	#endif

	CHECK_ERR( Networking::SocketService::Instance().Initialize(), -1 );

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
			r_params.addr		= AE_LLM_SERVER_IPv4;
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

	Networking::SocketService::Instance().Deinitialize();
	return 0;
}
