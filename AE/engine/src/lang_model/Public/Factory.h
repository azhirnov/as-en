// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "lang_model/Public/LanguageModel.h"
#include "lang_model/Public/DiffusionModel.h"

#include "lang_model/Public/LLamaParams.h"
#include "lang_model/Public/SDParams.h"
#include "lang_model/Public/RemoteParams.h"

namespace AE::LangModel
{

	//
	// Language Model Factory
	//

	class LMFactory : public Noninstanceable
	{
	// methods
	public:
		ND_ static RC<ILanguageModel>	CreateLLama (const LLama::OpenParams &)			__NE___;

		// connect to remote model
		ND_ static RC<ILanguageModel>	CreateRemote (Remote::OpenParams &)				__NE___;
	};



	//
	// Diffusion Model Factory
	//

	class DMFactory : public Noninstanceable
	{
	// methods
	public:
		ND_ static RC<IDiffusionModel>	CreateStableDiffusion (const SD::OpenParams &)	__NE___;

		// connect to remote model
		ND_ static RC<IDiffusionModel>	CreateRemote (Remote::OpenParams &)				__NE___;
	};


} // AE::LangModel
