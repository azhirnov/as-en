// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "lang_model/Public/Common.h"

namespace AE::LangModel
{

	enum class EModelFormat : ubyte
	{
		GGUF,				// for LLama
		SafeTensors,		// for OpenVINO
		_Count,
		Unknown		= _Count
	};
	using EModelFormatSet = EnumSet< EModelFormat >;


	enum class EFlashAttention : ubyte
	{
		Auto,
		Disabled,
		Enabled,

		Unknown		= Auto,
	};


	enum class EImplementation : ubyte
	{
		LLama,
		_Count,
	};


	enum class ERole : ubyte
	{
		User,		// from user (prompt)
		Assistant,	// from LLM
		System,		// setup LLM
		_Count,

		Prompt		= User,
	};



	//
	// Language Model Open Params
	//
	struct OpenParams
	{
		RC<ILoadingListener>	listener;
		RC<ILogListener>		logger;
	};



	//
	// Language Model Context Params
	//
	struct ContextParams
	{
	protected:
		const EImplementation	_type;

		explicit ContextParams (EImplementation type)	__NE___ : _type{type} {}

	public:
		ND_ EImplementation  Type ()					C_NE___	{ return _type; }
	};



	//
	// Language Model Info
	//
	struct ModelInfo
	{
		uint				maxContextSize			= 0;		// context size the model was trained on
		uint				layerCount				= 0;
		float				RoPE_freqScaleTrain		= 0.f;
		ulong				paramsCount				= 0;
		Bytes				modelSize;
		bool				recurrentModel			= false;
		bool				diffusionModel			= false;
		EModelFormat		format					= Default;
	};



	//
	// Language Model Loading Listener
	//
	class ILoadingListener : public EnableRC<ILoadingListener>
	{
	// interface
	public:
		// return 'false' to stop loading
		virtual bool  Progress (Percent)	__NE___ = 0;
	};



	//
	// Language Model Log Listener
	//
	class ILogListener : public EnableRC<ILogListener>
	{
	// interface
	public:
		virtual void  Log (ELogLevel, StringView)	__NE___ = 0;
	};



	//
	// Language Model Response Listener
	//
	class IResponseListener : public EnableRC<IResponseListener>
	{
	// types
	public:
		enum class ErrorCode
		{
			None,
			FailedToApplyChatTemplate,
			FailedToTokenizePrompt,
			ContextSizeExceeded,
			UnknownError,
			InterruptedByUser,
			OutOfMemory,
		};

	// interface
	public:

		// Return 'false' to stop generation.
		// User may ignore input, complete response will be passed to 'OnComplete()'.
		//
		virtual bool  AppendResponse (U8StringView piece, uint tokens)	__NE___ = 0;


		// Called once when response is complete.
		// Pass full response in utf8 format and total token count.
		//
		virtual void  OnComplete (U8StringView response, uint tokens)	__NE___ = 0;

		// Called once before tokenizing prompt.
		//
		virtual void  RequiredPromptTokens (uint tokens)				__NE___ = 0;

		virtual void  OnError (ErrorCode)								__NE___ = 0;
	};


} // AE::LangModel
