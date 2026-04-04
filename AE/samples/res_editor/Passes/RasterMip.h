// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Passes/IPass.h"
#include "Resources/IResource.h"

namespace AE::ResEditor
{

	//
	// Raster Mip
	//

	class RasterMip final : public IPass
	{
		friend class ScriptRasterMip;

	// types
	private:
		static constexpr uint	_MaxVariables	= 8;

		using PipelineMap_t		= FixedMap< EDebugMode, GraphicsPipelineID, uint(EDebugMode::_Count) >;
		using MipChainDS_t		= Array< Strong<DescriptorSetID> >;
		using MipChainGroups_t	= Array< uint2 >;

		struct Variable
		{
			RC<Image>					image;
			MipmapLevel					baseMipmap;
			Array<Strong<ImageViewID>>	inViews;
			Array<Strong<ImageViewID>>	outViews;
			UniformName					inName;
			AttachmentName				outName;
		};
		using Variables_t	= FixedArray< Variable, _MaxVariables >;


	// variables
	private:
		RTechInfo				_rtech;
		RenderPassDesc			_rpDesc;

		Variables_t				_variables;
		uint2					_lastDim;

		PipelineMap_t			_pipelines;
		PerFrameDescSet_t		_descSets;
		MipChainDS_t			_mipChainDS;
		DescSetBinding			_ds0Index;
		DescSetBinding			_ds1Index;		// mip chain
		PushConstantIndex		_pcIndex;

		Strong<BufferID>		_ubuffer;

		ResourceArray			_resources;


	// methods
	public:
		RasterMip ()													__NE___ {}
		~RasterMip ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV;
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;

	private:
		ND_ bool	_CreateMipChain ();
			void	_DestroyMipChain ();
	};


} // AE::ResEditor
