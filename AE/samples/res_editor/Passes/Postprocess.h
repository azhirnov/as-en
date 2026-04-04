// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Passes/IPass.h"
#include "Resources/Buffer.h"
#include "Resources/Image.h"

namespace AE::ResEditor
{

	//
	// Post Process
	//

	class Postprocess final : public IPass
	{
		friend class ScriptPostprocess;

	// types
	protected:
		using PipelineMap_t	= FixedMap< EDebugMode, GraphicsPipelineID, uint(EDebugMode::_Count) >;


	// variables
	protected:
		RTechInfo				_rtech;
		RenderPassDesc			_rpDesc;

		RC<DynamicDim>			_dynamicDim;

		PipelineMap_t			_pipelines;
		PerFrameDescSet_t		_descSets;
		DescSetBinding			_dsIndex;

		Strong<BufferID>		_ubuffer;

		ResourceArray			_resources;
		RenderTargets_t			_renderTargets;


	// methods
	public:
		Postprocess ()													__NE___	{}
		~Postprocess ();

	// IPass //
		EPassType	GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		bool		Execute (SyncPassData &)							__Th_OV;
		bool		Update (TransferCtx_t &, const UpdatePassData &)	__Th_OV;
		void		GetResourcesToResize (INOUT Array<RC<IResource>> &)	__NE_OV;

	private:
		ND_ uint2  _GetDimension ()										C_NE___;
	};


} // AE::ResEditor
