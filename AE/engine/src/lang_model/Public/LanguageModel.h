// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread safe: no
*/

#pragma once

#include "lang_model/Public/Types.h"

namespace AE::LangModel
{

	//
	// (Large) Language Model interface
	//
	class ILanguageModel : public EnableRC<ILanguageModel>
	{
	// interface
	public:

		// Create LLM context.
		// Params type must be same as returned by 'GetImplementationType()'.
		//
		ND_ virtual RC<ILanguageModelContext>  CreateContext (const ContextParams &)	__NE___ = 0;


		ND_ virtual ModelInfo  GetModelInfo ()											__NE___ = 0;


		// Can be used to choose which context params pass to 'CreateContext()'.
		//
		ND_ virtual EImplementation  GetImplementationType ()							C_NE___ = 0;
	};



	//
	// (Large) Language Model Context interface
	//
	class ILanguageModelContext : public EnableRC<ILanguageModelContext>
	{
	// interface
	public:

		// Process prompt and generate response.
		//
		ND_ virtual bool  Generate (U8String				prompt,
									RC<IResponseListener>	listener)			__NE___	= 0;


		// Append message to chat without generating response.
		// Message may contains user questions, LLM responses, system setup.
		// Some models supports setting reasoning level in 'ERole::System' with 'Reasoning: high/medium/low'.
		//
		ND_ virtual bool  Append (ERole role, U8String content)					__NE___ = 0;


		// Returns all messages with role.
		// Can be used to create chat branch.
		//
		ND_ virtual Array<Pair<ERole, U8String>>  GetMessages ()				__NE___ = 0;


		// Returns number of tokens which is used in current context.
		// Generation will stop when context size reached to it maximal size,
		// which was defined in 'ContextParams'.
		//
		ND_ virtual uint  CurrentSize ()										__NE___ = 0;

		// Clear message history.
		//
			virtual void  Clear ()												__NE___ = 0;


		ND_ virtual RC<ILanguageModel>  GetModel ()								__NE___ = 0;
	};


} // AE::LangModel
