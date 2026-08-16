// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe: yes,  except 'Load()'
*/

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "platform/Public/Monitor.h"
# include "graphics_rhi/Public/SwapchainDesc.h"

namespace AE::App
{
	using Graphics::EColorSpace;


	//
	// NV API Lib
	//

	class NvAPILib
	{
	// variables
	private:
		Atomic<bool>		_initialized	{false};


	// methods
	public:
		NvAPILib ()													__NE___ {}
		~NvAPILib ()												__NE___;

		ND_ bool  Load ()											__NE___;

		ND_ bool  SetHDRMode (const RectI	&region,
							  EColorSpace	colorSpace)				__NE___;
		ND_ bool  IsHDRMode (const RectI	&region)				C_NE___;

		ND_ bool  FillHDRConfig (MutableArrayView<Monitor>)			C_NE___;

	private:
		bool  _CheckNvAPIError (const int err, const char* fnCall, const char* func, const SourceLoc &loc) C_NE___;

		void  _Print () const;
	};


} // AE::App
#endif // AE_PLATFORM_WINDOWS
