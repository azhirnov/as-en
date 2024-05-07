// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Packer/Enums.h"

namespace AE::PipelineCompiler
{

#if not defined(AE_ENABLE_VULKAN)
namespace
{
	enum VkAttachmentLoadOp
	{
		VK_ATTACHMENT_LOAD_OP_LOAD = 0,
		VK_ATTACHMENT_LOAD_OP_CLEAR = 1,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE = 2,
		VK_ATTACHMENT_LOAD_OP_NONE_KHR = 1000400000,
		VK_ATTACHMENT_LOAD_OP_NONE_EXT = VK_ATTACHMENT_LOAD_OP_NONE_KHR,
		VK_ATTACHMENT_LOAD_OP_MAX_ENUM = 0x7FFFFFFF
	};

	enum VkAttachmentStoreOp
	{
		VK_ATTACHMENT_STORE_OP_STORE = 0,
		VK_ATTACHMENT_STORE_OP_DONT_CARE = 1,
		VK_ATTACHMENT_STORE_OP_NONE = 1000301000,
		VK_ATTACHMENT_STORE_OP_NONE_KHR = VK_ATTACHMENT_STORE_OP_NONE,
		VK_ATTACHMENT_STORE_OP_NONE_QCOM = VK_ATTACHMENT_STORE_OP_NONE,
		VK_ATTACHMENT_STORE_OP_NONE_EXT = VK_ATTACHMENT_STORE_OP_NONE,
		VK_ATTACHMENT_STORE_OP_MAX_ENUM = 0x7FFFFFFF
	};
}
#endif

	using namespace AE::Base;
	using namespace AE::Graphics;

/*
=================================================
	VEnumCast (EAttachmentLoadOp)
=================================================
*/
	ND_ inline VkAttachmentLoadOp  VEnumCast (EAttachmentLoadOp value) __NE___
	{
		switch_enum( value )
		{
			case EAttachmentLoadOp::Invalidate :	return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			case EAttachmentLoadOp::Load :			return VK_ATTACHMENT_LOAD_OP_LOAD;
			case EAttachmentLoadOp::Clear :			return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case EAttachmentLoadOp::None :			return VK_ATTACHMENT_LOAD_OP_NONE_KHR;	// emulated if not supported
			case EAttachmentLoadOp::_Count :
			case EAttachmentLoadOp::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "invalid load op type", VK_ATTACHMENT_LOAD_OP_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EAttachmentStoreOp)
=================================================
*/
	ND_ inline VkAttachmentStoreOp  VEnumCast (EAttachmentStoreOp value) __NE___
	{
		switch_enum( value )
		{
			case EAttachmentStoreOp::Invalidate :	return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case EAttachmentStoreOp::StoreCustomSamplePositions :
			case EAttachmentStoreOp::Store :		return VK_ATTACHMENT_STORE_OP_STORE;
			case EAttachmentStoreOp::None :			return VK_ATTACHMENT_STORE_OP_NONE_KHR;	// emulated if not supported
			case EAttachmentStoreOp::_Count :
			case EAttachmentStoreOp::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "invalid store op type", VK_ATTACHMENT_STORE_OP_MAX_ENUM );
	}

} // AE::PipelineCompiler
//-----------------------------------------------------------------------------
