// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../tests/shared/UnitTest_Shared.h"
#include "pch/LangModel.h"
#include "lang_model/Remote/Server.h"
using namespace AE::LangModel;

extern const char*  GetGGMLModelName();

namespace
{
	static constexpr ushort		c_ServerPort = 3000;


	class LoadingListener final : public ILoadingListener
	{
	public:
		bool  Progress (Percent pct) __NE_OV
		{
			AE_LOGI( "Loading "s << ToString( int(pct.GetPercent()) ) << '%' );
			return true;  // continue
		}
	};

	class ResponseListener final : public IResponseListener
	{
	public:
		U8String	response;
		uint		tokens		= 0;
		
		bool  AppendResponse (U8StringView piece, uint count) __NE_OV
		{
			TEST( count > 0 );
			tokens += count;
			response << piece;
			return true;  // continue
		}
		
		void  OnComplete (U8StringView completeResponse, uint count) __NE_OV
		{
			TEST( response == completeResponse );
			TEST( tokens == count );
		}

		void  OnError (ErrorCode) __NE_OV {}

		void  RequiredPromptTokens (uint) __NE_OV {}
	};


	struct RemoteLLama_Server
	{
		LangModelServer		_server;
		StdThread			_thread;

		RemoteLLama_Server ()
		{
			AE_LOGI( "Start server" );

			_thread = StdThread{ [this]()
								{
									Unused( _server.Run( c_ServerPort ));
								}};
		}

		~RemoteLLama_Server()
		{
			_server.Stop();
			_thread.join();
		}
	};


	static void  RemoteLLama_Test1 ()
	{
		RemoteLLama_Server	s;

		AE_LOGI( "Load model" );

		RC<ILanguageModel>	model;
		{
			Remote::OpenParams	params;
			params.addr					= Networking::IpAddress::FromLocalhostTCP( c_ServerPort );
			params.listener				= MakeRC<LoadingListener>();
			params.llama				= MakeUnique<LLama::OpenParams>();
			params.llama->modelFile		= GetGGMLModelName();
			params.llama->enableLogger	= false;

			model = LMFactory::CreateRemote( params );
			TEST( model );
		}

		AE_LOGI( "Create context" );

		RC<ILanguageModelContext>	ctx;
		{
			LLama::ContextParams	params;
			params.contextSize		= 8 << 10;
			params.sampler.minP		= LLama::Sampler_MinP{};

			ctx = model->CreateContext( params );
			TEST( ctx );
		}

		AE_LOGI( "Generate response" );
		{
			auto	listener = MakeRC<ResponseListener>();
			
			TEST( ctx->Generate( u8"What you can do?", listener ));

			TEST( not listener->response.empty() );

			AE_LOGI( "LLM response tokens: "s << ToString( listener->tokens ));
			AE_LOGI( "LLM response:\n"s << ToString( listener->response ));
		}
		TEST_PASSED();
	}


	static void  RemoteLLama_Test2 ()
	{
		RemoteLLama_Server	s;

		RC<ILanguageModel>	model;
		{
			Remote::OpenParams	params;
			params.addr					= Networking::IpAddress::FromLocalhostTCP( c_ServerPort );
			params.llama				= MakeUnique<LLama::OpenParams>();
			params.llama->modelFile		= GetGGMLModelName();
			params.llama->enableLogger	= false;

			model = LMFactory::CreateRemote( params );
			TEST( model );
		}

		RC<ILanguageModelContext>	ctx;
		{
			LLama::ContextParams	params;
			params.contextSize		= 8 << 10;
			params.sampler.minP		= LLama::Sampler_MinP{};

			ctx = model->CreateContext( params );
			TEST( ctx );
		}

		TEST( ctx->Append( ERole::System, u8"Answer as experienced programmer. By default use C++ and HLSL. Prefer snake_case style." ));

		for (uint i = 0; i < 4; ++i)
		{
			auto		listener = MakeRC<ResponseListener>();
			U8String	prompt;

			switch ( i )
			{
				case 0 :	prompt = u8"Write simple sky shader on GLSL.";		break;
				case 1 :	prompt = u8"Convert it to HLSL.";					break;
				case 2 :	prompt = u8"Use structured buffer for uniforms, calculate view direction from current pixel coordinates.";	break;
				case 3 :	prompt = u8"Add example how to use this shader.";	break;
				default :	TEST(false);
			}

			TEST( ctx->Generate( RVRef(prompt), listener ));

			TEST( not listener->response.empty() );

			AE_LOGI( "LLM response tokens: "s << ToString( listener->tokens ));
			AE_LOGI( "LLM response:\n"s << ToString( listener->response ) << "\n---------------------\n\n" );
		}
		TEST_PASSED();
	}
}


extern void  UnitTest_RemoteLLama ()
{
	CHECK_FATAL( Networking::SocketService::Instance().Initialize() );

	RemoteLLama_Test1();
	RemoteLLama_Test2();
	
	Networking::SocketService::Instance().Deinitialize();
	TEST_PASSED();
}
