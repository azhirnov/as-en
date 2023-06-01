// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/Queue.h"

namespace AE::Graphics
{

	//
	// Buffer description
	//

	struct BufferDesc
	{
	// variables
		Bytes			size;
		EBufferUsage	usage		= Default;
		EBufferOpt		options		= Default;
		EQueueMask		queues		= Default;
		EMemoryType		memType		= EMemoryType::DeviceLocal;

	// methods
		BufferDesc ()									__NE___ {}

		BufferDesc (Bytes			size,
					EBufferUsage	usage,
					EBufferOpt		options	= Default,
					EQueueMask		queues	= Default,
					EMemoryType		memType	= EMemoryType::DeviceLocal) __NE___ :
			size{size}, usage{usage}, options{options}, queues{queues}, memType{memType} {}
		
		// Will remove unsupported combinations
		void  Validate ()								__NE___;

		ND_ bool  operator == (const BufferDesc &rhs)	C_NE___;
		ND_ bool  IsExclusiveSharing ()					C_NE___	{ return queues == Default; }

		BufferDesc&  SetSize (Bytes value)				__NE___	{ size		= value;	return *this; }
		BufferDesc&  SetUsage (EBufferUsage value)		__NE___	{ usage		= value;	return *this; }
		BufferDesc&  SetOptions (EBufferOpt value)		__NE___	{ options	= value;	return *this; }
		BufferDesc&  SetQueues (EQueueMask value)		__NE___	{ queues	= value;	return *this; }
		BufferDesc&  SetMemory (EMemoryType value)		__NE___	{ memType	= value;	return *this; }
	};



	//
	// Buffer View description
	//

	struct BufferViewDesc
	{
	// variables
		Bytes				offset;
		Bytes				size	{ UMax };
		EPixelFormat		format	= Default;

	// methods
		BufferViewDesc ()									__NE___ {}

		BufferViewDesc (EPixelFormat	format,
						Bytes			offset,
						Bytes			size)				__NE___ :
			offset{offset}, size{size}, format{format} {}

		void  Validate (const BufferDesc &desc)				__NE___;
		
		ND_ bool  operator == (const BufferViewDesc &rhs)	C_NE___;
	};


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::BufferDesc >			{ static constexpr bool  value = true; };
	template <> struct TTriviallySerializable< AE::Graphics::BufferDesc >		{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::BufferViewDesc >		{ static constexpr bool  value = true; };
	template <> struct TTriviallySerializable< AE::Graphics::BufferViewDesc >	{ static constexpr bool  value = true; };

} // AE::Base
