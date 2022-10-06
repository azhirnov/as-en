// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"

using namespace AE;
using namespace AE::Base;

#define TEST	CHECK_FATAL

#define TEST_PASSED()	AE_LOGI( String{AE_FUNCTION_NAME} + " - passed" );
