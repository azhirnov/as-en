// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/ResourceManager.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Memory Allocator
	//

	class RGfxMemAllocator final : public IGfxMemAllocator
	{
	// variables
	private:
		const RmGfxMemAllocatorID	_gfxAllocId;
		const Bytes					_minAlign;
		const Bytes					_maxSize;


	// methods
	public:
		RGfxMemAllocator ()															__NE___ {}
		RGfxMemAllocator (RmGfxMemAllocatorID id, Bytes minAlign, Bytes maxSize)	__NE___ : _gfxAllocId{id}, _minAlign{minAlign}, _maxSize{maxSize} {}
		~RGfxMemAllocator ()														__NE_OV;

		ND_ RmGfxMemAllocatorID  Handle ()											C_NE___	{ return _gfxAllocId; }

		// IGfxMemAllocator //
		bool	GetInfo (const Storage_t &, OUT NativeMemObjInfo_t &)				C_NE_OV	{ return false; }

		Bytes	MinAlignment ()														C_NE_OV	{ return _minAlign; }
		Bytes	MaxAllocationSize ()												C_NE_OV	{ return _maxSize; }
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
