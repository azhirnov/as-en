// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "lang_model/Remote/Messages.h"
#include "lang_model/Public/LanguageModel.h"

namespace AE::LangModel
{

	//
	// Language Model Server
	//

	class LangModelServer
	{
	// types
	private:
		static constexpr uint	c_MinCharsForAppend	= 64;


		class Connection final : public Networking::TcpStream
		{
		public:
			static constexpr Bytes	c_BufferSize = 4_MiB;
			
			ND_ bool  InitServer (ushort port,
								  Ptr<Serializing::ObjectFactory> factory)	__NE___	{ return _InitServer( port, c_BufferSize, factory ); }

				using TcpStream::Send;

			ND_ bool  Send (const Msg::BaseMsg &msg)						__NE___	{ return _Send( msg ); }
			ND_ auto  Encode ()												__NE___ -> RC<Msg::BaseMsg>		{ return _Encode<Msg::BaseMsg>(); }
		};


		class Logger final : public ILogger
		{
		private:
			LangModelServer &	_server;

		public:
			Logger (LangModelServer &ref)				__NE___	: _server{ref} {}

			EResult  Process (const MessageInfo &info)	__Th_OV;
		};


		class ResponseListener final : public IResponseListener
		{
		private:
			LangModelServer &		_server;
			const Msg::ContextUID	_uid;

			uint					_lastSentToken	= 0;
			uint					_tokenCount		= 0;
			const uint				_prevCtxSize;
			U8String				_responsePart;

			bool					_canceled		= false;

		public:
			ResponseListener (LangModelServer &ref, Msg::ContextUID uid, uint ctxSize) __NE___ : _server{ref}, _uid{uid}, _prevCtxSize{ctxSize} {}

			void  Reset ()									__NE___;
			void  Cancel ()									__NE___	{ _canceled = true; }

			// IResponseListener //
			bool  AppendResponse (U8StringView, uint)		__NE_OV;
			void  OnComplete (U8StringView, uint)			__NE_OV;
			void  RequiredPromptTokens (uint)				__NE_OV;
			void  OnError (ErrorCode)						__NE_OV;

		private:
			void  _Append ();
		};


		struct ContextData
		{
			RC<ILanguageModelContext>	ctx;
			RC<ResponseListener>		listener;
		};

		using MsgCallback_t		= void (LangModelServer::*) (const Msg::BaseMsg &) __Th___;
		using MsgCallbacks_t	= FlatHashMap< TypeId, MsgCallback_t >;
		using ContextPool_t		= FlatHashMap< Msg::ContextUID, ContextData >;


	// variables
	private:
		Connection					_channel;
		Serializing::ObjectFactory	_objFactory;

		MsgCallbacks_t				_callbacks;

		RC<ILanguageModel>			_curModel;
		Msg::ModelUID				_modelUID	= Default;
		Random						_rnd;

		ContextPool_t				_ctxPool;

		Atomic<bool>				_run;
		Atomic<bool>				_hasError;


	// methods
	public:
		LangModelServer ()										__NE___;
		~LangModelServer ()										__NE___ {}

		ND_ bool  Run (ushort port)								__NE___;
			void  Stop ()										__NE___;

		ND_	Unique<ILogger>  CreateLogOutput ()					__NE___;

	private:
			void  _PrintSelfIP (ushort port)					C_NE___;

		ND_ bool  _StartServer (ushort port)					__NE___;
			void  _StopServer ()								__NE___;

			void  _MessageLoop ()								__NE___;
			void  _ProcessMessage ()							__Th___;
			void  _ProcessMessageNothrow ()						__NE___;

		ND_ bool  _OpenLLama (const Msg::LangModelOpenLLama &)	__NE___;

		ND_ bool  _RegisterMessages ();
		
		template <typename M>
		ND_ bool  _Register (Serializing::SerializedID::Ref id);

		template <typename M>
		ND_ bool  _RegisterCB (Serializing::SerializedID::Ref id,
							   void (LangModelServer::*)(const M &) __Th___);

			void  _SendLog (ELogLevel, StringView)				__NE___;

	private:
		void  _Cb_LangModelInit (const Msg::LangModelInit &);
		void  _Cb_LangModelShutdown (const Msg::LangModelShutdown &);
		void  _Cb_LangModelOpenLLama (const Msg::LangModelOpenLLama &);
		void  _Cb_LangModelClose (const Msg::LangModelClose &);
		void  _Cb_LangModelGetInfo (const Msg::LangModelGetInfo &);
		void  _Cb_LangModelCreateContextLLama (const Msg::LangModelCreateContextLLama &);
		void  _Cb_LangModelContextGenerate (const Msg::LangModelContextGenerate &);
		void  _Cb_LangModelContextGenerate_Cancel (const Msg::LangModelContextGenerate_Cancel &);
		void  _Cb_LangModelContextAppend (const Msg::LangModelContextAppend &);
		void  _Cb_LangModelContextClear (const Msg::LangModelContextClear &);
		void  _Cb_LangModelContextClose (const Msg::LangModelContextClose &);
	};


} // AE::LangModel
