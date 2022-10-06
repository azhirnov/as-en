// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TODO Optimization:
		* add extrnal lock
		* if locked then:
			* create temporary cache
			* compile pipeline with cache
			* merge temporary cache with main cache
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Pipeline Cache
	//

	class VPipelineCache final
	{
	// variables
	private:
		VkPipelineCache				_cache	= Default;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)

	
	// methods
	public:
		VPipelineCache () {}
		~VPipelineCache ();

		ND_ bool  Create (const VResourceManager& resMngr, StringView dbgName);
		ND_ bool  Create (const VResourceManager& resMngr, StringView dbgName, RC<RStream> stream);
			void  Destroy (VResourceManager &);

		ND_ bool  GetData (const VDevice &dev, OUT Array<char> &data) const;

		ND_ VkPipelineCache  Handle ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _cache; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

	private:
		bool  _Create (const VResourceManager& resMngr, StringView dbgName, ArrayView<char> initialData);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
