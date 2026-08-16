// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Large Language Model API
*/

#pragma once

#include "pch/Base.h"
#include "pch/Networking.h"

namespace AE::LangModel
{
	using namespace AE::Base;

	ImportBitOperators;


	class ILanguageModel;
	class ILanguageModelContext;

	class IDiffusionModel;
	class IDiffusionModelContext;

	class ILoadingListener;
	class IResponseListener;
	class ILogListener;

} // AE::LangModel
