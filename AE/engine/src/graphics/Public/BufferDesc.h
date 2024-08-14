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



	//
	// Buffer Device Address (helper)
	//
	enum class DeviceAddress : ulong { Unknown = 0 };

	ND_ inline constexpr DeviceAddress  operator + (DeviceAddress addr, Bytes offset) __NE___ {
		return DeviceAddress( ulong(addr) + ulong(offset) );
	}


	//
	// Buffer Device Address (helper to use declare which type is used on the GPU side)
	//
	template <typename T>
	struct TDeviceAddress
	{
	public:
		using Value_t = T;

	private:
		DeviceAddress		_addr	= Default;

	public:
		constexpr TDeviceAddress ()						__NE___ = default;
		constexpr TDeviceAddress (DeviceAddress addr)	__NE___ : _addr{addr} {}
	};


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::BufferDesc >			: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::BufferDesc >		: CT_True {};

	template <> struct TMemCopyAvailable< AE::Graphics::BufferViewDesc >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::BufferViewDesc >	: CT_True {};

} // AE::Base
