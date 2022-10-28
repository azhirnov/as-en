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
			VRenderPassID	renderPassId;
			Attachments_t	attachments;

		// methods
			Key (StructView<ImageViewID>, VRenderPassID, const uint3 &dim);

			ND_ bool	operator == (const Key &) const;
			ND_ HashVal	CalcHash () const;
		};
		
		struct KeyHash {
			ND_ usize  operator () (const Key &key) const {
				return usize(key.CalcHash());
			}
		};

		using FramebufferCache_t	= HashMap< VFramebuffer::Key, VFramebufferID, KeyHash >;
		using CachePtr_t			= FramebufferCache_t::iterator;

		
	// variables
	private:
		VkFramebuffer				_framebuffer	= Default;
		Strong<VRenderPassID>		_renderPassId;

		uint2						_dimension;
		ImageLayer					_layers;
		Images_t					_images;
		Attachments_t				_attachments;	// weak references
		CachePtr_t					_cachePtr		= Default;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)

		
	// methods
	public:
		VFramebuffer () {}
		~VFramebuffer ();
		
		ND_ bool  Create (VResourceManager &, const RenderPassDesc &desc, VRenderPassID rpId, StringView dbgName = Default);
			void  Destroy (VResourceManager &);
		ND_ bool  SetCachePtr (CachePtr_t ptr);

		ND_ bool  IsAllResourcesAlive (const VResourceManager &) const;

		ND_ VkFramebuffer			Handle ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _framebuffer; }
		ND_ VRenderPassID			RenderPassID ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _renderPassId; }
		ND_ uint2 const&			Dimension ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _dimension; }
		ND_ uint					Layers ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _layers.Get(); }
		ND_ ArrayView<ImageViewID>	Attachments ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _attachments; }
		ND_ ArrayView<ImageID>		Images ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _images; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

	private:
		ND_ bool  _Create (VResourceManager &, const RenderPassDesc &desc, StringView dbgName);
	};

	
	
	inline VFramebuffer::Key::Key (StructView<ImageViewID> inAttachments, VRenderPassID rpId, const uint3 &dim) :
		dimension{ dim },
		renderPassId{ rpId }
	{
		for (auto id : inAttachments)
			attachments.push_back( id );
	}

	inline bool  VFramebuffer::Key::operator == (const Key &rhs) const
	{
		return	All( dimension	== rhs.dimension )		&
				(renderPassId	== rhs.renderPassId)	&
				(attachments	== rhs.attachments);
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
