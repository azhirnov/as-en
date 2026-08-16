// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "lang_model/Public/LanguageModel.h"
#include "lang_model/Public/RemoteParams.h"
#include "lang_model/Remote/Messages.h"

namespace AE::LangModel
{

	//
	// Language Model Client
	//
	class LangModelClient final : public ILanguageModel
	{
		friend class LMFactory;
		friend class LangModelContextClient;

	// types
	private:
		static constexpr Bytes	c_BufferSize = 4_MiB;

		class Connection final : public Networking::TcpStream
		{
		public:
			ND_ bool  InitClient (Networking::IpAddress			  addr,
								  Ptr<Serializing::ObjectFactory> factory)	__NE___	{ return _InitClient( addr, c_BufferSize, factory ); }

				using TcpStream::Send;

			ND_ bool  Send (const Msg::BaseMsg &msg)						__NE___	{ return _Send( msg ); }
			ND_ auto  Encode ()												__NE___ -> RC<Msg::BaseMsg>		{ return _Encode<Msg::BaseMsg>(); }
		};


	// variables
	private:
		Connection					_channel;
		Serializing::ObjectFactory	_objFactory;
		RC<ILogListener>			_logger;
		RC<ILoadingListener>		_loadingListener;

		LangModel::Msg::ModelUID	_uid		= Default;
		EImplementation				_implType	= EImplementation::_Count;
		Optional<ModelInfo>			_modelInfo;


	// methods
	public:
		LangModelClient ()													__NE___ {}
		~LangModelClient ()													__NE_OV;

		// ILanguageModel //
		RC<ILanguageModelContext>  CreateContext (const ContextParams &)	__NE_OV;

		ModelInfo  GetModelInfo ()											__NE_OV;

		EImplementation  GetImplementationType ()							C_NE_OV	{ return _implType; }

	private:
		bool  _Open (Remote::OpenParams &);
		bool  _RegisterMessages ();

		template <typename M>
		ND_ bool  _Register (Serializing::SerializedID::Ref id);

		template <typename M>
		ND_ RC<M>  _WaitFor ()												__NE___;
	};



	//
	// Language Model Context Client
	//
	class LangModelContextClient final : public ILanguageModelContext
	{
		friend class LangModelClient;

	// variables
	private:
		RC<LangModelClient>				_modelRC;
		LangModel::Msg::ContextUID		_uid			= Default;
		uint							_usedCtxSize	= 0;

		Array<Pair<ERole, U8String>>	_history;


	// methods
	public:
		LangModelContextClient ()							__NE___	{}
		~LangModelContextClient ()							__NE_OV;

		// ILanguageModelContext //
		bool  Generate (U8String				prompt,
						RC<IResponseListener>	listener)	__NE_OV;

		bool  Append (ERole role, U8String content)			__NE_OV;

		Array<Pair<ERole, U8String>>  GetMessages ()		__NE_OV;

		uint  CurrentSize ()								__NE_OV;

		void  Clear ()										__NE_OV;

		RC<ILanguageModel>  GetModel ()						__NE_OV	{ return _modelRC; }

	private:
		ND_ bool  _IsValid ()								C_NE___	{ return _modelRC and _uid != Default; }
		ND_ bool  _Process (IResponseListener &)			__NE___;
	};


} // AE::LangModel
