// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Resources/IResource.h"

namespace AE::ResEditor
{

	//
	// Resource Array
	//

	class ResourceArray final
	{
	// types
	public:
		using AnyResource_t		= Union< NullUnion, RC<Buffer>, RC<BufferView>, RC<RTScene>, RC<Image>, RC<VideoImage>,
										 RC<VideoImage2>, Array<RC<Image>> >;

		struct Resource
		{
			UniformName		name;
			AnyResource_t	res;
			EResourceState	state;
			bool			clearContent;
		};
		using Resources_t	= Array< Resource >;


	// variables
	private:
		Resources_t		_resources;
		bool			_needClearContent	= false;


	// methods
	public:
		ResourceArray ();
		~ResourceArray ();

		ND_ bool				Empty ()											const	{ return _resources.empty(); }
		ND_ Resources_t const&	Get ()												const	{ return _resources; }

			void  Add (UniformName::Ref name, AnyResource_t res,
						EResourceState state, bool clearContent = false);

			void  SetStates (DirectCtx::Compute &, EResourceState shaderStages)		const;
			void  SetStates (DirectCtx::Graphics &, EResourceState shaderStages)	const;
			void  SetStates (DirectCtx::RayTracing &, EResourceState shaderStages)	const;

			void  GetResourcesToResize (INOUT Array<RC<IResource>> &)				const;

		ND_ bool  Bind (FrameUID, DescriptorUpdater &updater)						const;

	private:
		template <typename CtxType>
		void  _SetStates (CtxType &ctx, EResourceState shaderStages)				const;

		void  _ClearContent (DirectCtx::Transfer &ctx)								const;
	};


} // AE::ResEditor
