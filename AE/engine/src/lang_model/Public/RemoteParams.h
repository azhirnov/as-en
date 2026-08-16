// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "lang_model/Public/LLamaParams.h"

namespace AE::LangModel::Remote
{

	//
	// Open Params
	//
	struct OpenParams : LangModel::OpenParams
	{
		Networking::IpAddress		addr;

		Unique<LLama::OpenParams>	llama;
	};



	//
	// Context Params
	//
	struct ContextParams : LangModel::ContextParams
	{
		Unique<LLama::ContextParams>	llama;
	};


} // AE::LangModel::Remote
