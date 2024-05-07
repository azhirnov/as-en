// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/Common.h"

namespace AE::Audio
{

	//
	// Audio Sample View
	//

	class AudioSampleView
	{
	// variables
	private:
		AudioDataDesc	_desc;
		void *			_data	= null;


	// methods
	public:
		AudioSampleView ()											__NE___	{}
		AudioSampleView (const AudioDataDesc &desc, void* data)		__NE___	: _desc{desc}, _data{data} {}

		AudioSampleView (const AudioSampleView &)					__NE___	= default;
		AudioSampleView (AudioSampleView &&)						__NE___	= default;
		AudioSampleView&  operator = (const AudioSampleView &)		__NE___	= default;
		AudioSampleView&  operator = (AudioSampleView &&)			__NE___	= default;

		ND_ AudioDataDesc const&	Desc ()							C_NE___	{ return _desc; }
		ND_ void *					Data ()							__NE___	{ return _data; }
		ND_ void const *			Data ()							C_NE___	{ return _data; }

		ND_ bool					Empty ()						C_NE___	{ return _data == null; }
	};


} // AE::Audio
