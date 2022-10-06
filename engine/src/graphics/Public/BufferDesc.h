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
		BufferDesc () {}

		BufferDesc (Bytes			size,
					EBufferUsage	usage,
					EBufferOpt		options	= Default,
					EQueueMask		queues	= Default,
					EMemoryType		memType	= EMemoryType::DeviceLocal) :
			size{size}, usage{usage}, options{options}, queues{queues}, memType{memType} {}
		
		// Will remove unsupported combinations
		void  Validate ();

		ND_ bool  operator == (const BufferDesc &rhs) const;

		BufferDesc&  SetSize (Bytes value)			{ size		= value;	return *this; }
		BufferDesc&  SetUsage (EBufferUsage value)	{ usage		= value;	return *this; }
		BufferDesc&  SetOptions (EBufferOpt value)	{ options	= value;	return *this; }
		BufferDesc&  SetQueues (EQueueMask value)	{ queues	= value;	return *this; }
		BufferDesc&  SetMemory (EMemoryType value)	{ memType	= value;	return *this; }
	};



	//
	// Buffer View description
	//

	struct BufferViewDesc
	{
	// variables
		Bytes				offset;
		Bytes				size	{ ~0_b };
		EPixelFormat		format	= Default;

	// methods
		BufferViewDesc () {}

		BufferViewDesc (EPixelFormat	format,
						Bytes			offset,
						Bytes			size) :
			offset{offset}, size{size}, format{format} {}

		void  Validate (const BufferDesc &desc);
		
		ND_ bool  operator == (const BufferViewDesc &rhs) const;
	};


}	// AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::BufferDesc >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::BufferDesc >		{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::BufferViewDesc >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::BufferViewDesc >	{ static constexpr bool  value = true; };

}	// AE::Base

namespace std
{
	template <>
	struct hash< AE::Graphics::BufferViewDesc > {
		ND_ size_t  operator () (const AE::Graphics::BufferViewDesc &value) const;
	};

}	// std
