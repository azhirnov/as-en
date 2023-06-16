// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

	//
	// Pass Group
	//

	class PassGroup final : public IPass
	{
	// types
	public:
		enum class EFlags : uint
		{
			Unknown		= 0,
			RunOnce		= 1 << 0,
			OnRequest	= 1 << 1,

			_Last,
			All			= ((_Last - 1) << 1) - 1,
		};

	private:
		using PassArr_t		= Array< RC<IPass> >;
		

	// variables
	private:
		PassArr_t		_passes;
		const EFlags	_flags			= Default;
		Atomic<uint>	_count			{0};
		Atomic<bool>	_requestUpdate	{false};
		String			_dbgName;


	// methods
	public:
		explicit PassGroup (EFlags flags) : _flags{flags} {}
		~PassGroup () {}
		
		void  AddPass (RC<IPass> pass)										__Th___;
		void  RequestUpdate ()												__NE___	{ _requestUpdate.store( true ); }

	// IPass //
		EPassType		GetType ()											C_NE_OV	{ return EPassType::Sync | EPassType::Update; }
		RC<IController>	GetController ()									C_NE_OV	{ return null; }
		StringView		GetName ()											C_NE_OV	{ return _dbgName; }
		bool			Execute (SyncPassData &)							__NE_OV;
		bool			Update (TransferCtx_t &, const UpdatePassData &)	__NE_OV;
	};
	
	AE_BIT_OPERATORS( PassGroup::EFlags );


} // AE::ResEditor
