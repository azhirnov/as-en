// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Noncopyable
	//

	class Noncopyable
	{
	public:
		Noncopyable () = default;

		Noncopyable (const Noncopyable &) = delete;
		Noncopyable (Noncopyable &&) = delete;

		Noncopyable& operator = (const Noncopyable &) = delete;
		Noncopyable& operator = (Noncopyable &&) = delete;
	};


	//
	// Movable Only
	//

	class MovableOnly
	{
	public:
		MovableOnly () = default;

		MovableOnly (MovableOnly &&) = default;
		MovableOnly& operator = (MovableOnly &&) = default;

		MovableOnly (const MovableOnly &) = delete;
		MovableOnly& operator = (const MovableOnly &) = delete;
	};


}	// AE::Base
