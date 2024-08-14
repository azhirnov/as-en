// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"

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

		struct DynamicData
		{
			uint	frame		= 0;
			uint	prevFrame	= UMax;
		};


	// variables
	protected:
		RTechInfo				_rtech;
		RenderPassDesc			_rpDesc;
		float2					_depthRange		{0.f, 1.f};

		PipelineMap_t			_pipelines;
		PerFrameDescSet_t		_descSets;
		DescSetBinding			_dsIndex;

		Strong<BufferID>		_ubuffer;
		mutable DynamicData		_dynData;		// used only in 'Upload()'

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
	};


} // AE::ResEditor
