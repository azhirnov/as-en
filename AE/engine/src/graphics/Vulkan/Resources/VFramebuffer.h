// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ImageDesc.h"
# include "graphics/Public/RenderPassDesc.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Framebuffer
	//

	class VFramebuffer final
	{
	// types
	public:
		using Images_t		= FixedArray< ImageID, GraphicsConfig::MaxAttachments >;
		using Attachments_t	= FixedArray< ImageViewID, GraphicsConfig::MaxAttachments >;

		struct Key
		{
		// variables
			uint3			dimension;
			RenderPassID	renderPassId;
			Attachments_t	attachments;

		// methods
			Key (StructView<ImageViewID>, RenderPassID, const uint3 &dim) __NE___;

			ND_ bool	operator == (const Key &)	C_NE___;
			ND_ HashVal	CalcHash ()					C_NE___;
		};

		struct KeyHash {
			ND_ usize  operator () (const Key &key) C_NE___ {
				return usize(key.CalcHash());
			}
		};

		using FramebufferCache_t	= HashMap< VFramebuffer::Key, VFramebufferID, KeyHash >;
		using CachePtr_t			= FramebufferCache_t::iterator;


	// variables
	private:
		VkFramebuffer				_framebuffer	= Default;
		Strong<RenderPassID>		_renderPassId;

		uint2						_dimension;
		ImageLayer					_layers;
		Images_t					_images;
		Attachments_t				_attachments;	// weak references
		CachePtr_t					_cachePtr		= Default;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VFramebuffer ()									__NE___	{}
		~VFramebuffer ()								__NE___;

		ND_ bool  Create (VResourceManager &, const RenderPassDesc &desc, RenderPassID rpId, StringView dbgName = Default)	__NE___;
			void  Destroy (VResourceManager &)																				__NE___;
		ND_ bool  SetCachePtr (CachePtr_t ptr)																				__NE___;

		ND_ bool  IsAllResourcesAlive (const VResourceManager &) C_NE___;

		ND_ VkFramebuffer			Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _framebuffer; }
		ND_ RenderPassID			RenderPass ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _renderPassId; }
		ND_ uint2 const&			Dimension ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dimension; }
		ND_ uint					LayerCount ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layers.Get(); }
		ND_ ArrayView<ImageViewID>	Attachments ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _attachments; }
		ND_ ArrayView<ImageID>		Images ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _images; }

		DEBUG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};



	inline VFramebuffer::Key::Key (StructView<ImageViewID> inAttachments, RenderPassID rpId, const uint3 &dim) __NE___ :
		dimension{ dim },
		renderPassId{ rpId }
	{
		for (auto id : inAttachments)
			attachments.push_back( id );
	}

	inline bool  VFramebuffer::Key::operator == (const Key &rhs) C_NE___
	{
		return	All( dimension	== rhs.dimension )		and
				(renderPassId	== rhs.renderPassId)	and
				(attachments	== rhs.attachments);
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
