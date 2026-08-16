// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/ShaderDebugger.h"
#include "Core/RenderGraph.h"
#include "Controllers/IController.h"
#include "Dynamic/DynamicVec.h"
#include "Resources/ResourceArray.h"

namespace AE::ResEditor
{

	//
	// Pass interface
	//

	class IPass : public EnableRC< IPass >
	{
		friend class ScriptBasePass;

	// types
	public:
		using TransferCtx_t = DirectCtx::Transfer;
		using CustomKeys_t	= StaticArray< float, 2 >;

		enum class EPassType : uint
		{
			Sync				= 1 << 0,
			Async				= 1 << 1,	// for heavy tasks
			Present				= 1 << 2,
			//SeparateBatch		= 1 << 3,	// for async compute
			Update				= 1 << 16,
			Export				= 1 << 17,	// will pause rendering
			Unknown				= 0,
			_BITOPS_			= 0
		};

		enum class EDebugMode : uint
		{
			Unknown			= 0,
			Trace,
			FnProfiling,		// function profiling
			TimeHeatMap,		// profiling for the whole pass
			Asserts,
			_Count
		};

		enum class ECompare : ubyte
		{
			Unknown,
			Equal,
			Less,
			Greater,
			AnyBit,
		};

		static inline void const* const		c_DebugAllTargets = reinterpret_cast<void const*>(usize(0x1));

		struct Debugger
		{
			void const*					target		= null;
			EDebugMode					mode		= Default;
			EShaderStages				stage		= Default;
			float2						coord;		// relative to render target dimension or dispatch size
			Optional<uint3>				exactCoord;
			Ptr<ShaderDebugger>			debugger;

			ND_ bool  IsEnabled ()					const	{ return debugger and mode != Default; }
			ND_ bool  IsEnabled (const IPass* pass)	const;
		};

		struct SyncPassData
		{
			RenderTaskApi				rtask;
			DirectCtx::CommandBuffer	cmdbuf;
			Debugger					dbg;

			explicit SyncPassData (RenderTaskApi rtask) : rtask{rtask} {}
		};

		struct AsyncPassData
		{
			RG::CommandBatchPtr			batch;
			Debugger					dbg;
			ActionQueueReader			reader;
			Array<AsyncTask>			_deps;

			ND_ ArrayView<AsyncTask>  DepsRef ()	const { return _deps; }
		};

		struct PresentPassData
		{
			RG::CommandBatchPtr			batch;
			Ptr<IOutputSurface>			surface;
			Array<AsyncTask>			_deps;

			ND_ ArrayView<AsyncTask>  DepsRef ()	const { return _deps; }
		};

		struct UpdatePassData
		{
			secondsf		totalTime;
			secondsf		frameTime;
			uint			frameId			= 0;
			uint			seed			= 0;

			float2			unormCursorPos;
			float			pixPerMm		= 1.f;		// for current screen (pix / mm)
			float			mmPerPix		= 1.f;		// for current screen (mm / pix)
			bool			pressed			= false;	// mouse down or touch pressed
			CustomKeys_t	customKeys		= {};

			EColorSpace		swapchainColorSpace	= Default;
		};


	protected:
		using PerFrameDescSet_t		= StaticArray< Strong<DescriptorSetID>, GraphicsConfig::MaxFrames >;
		using PerFrameTimeQuery_t	= StaticArray< QueryManager::Query, GraphicsConfig::MaxFrames >;

		using ClearValue_t		= RenderPassDesc::ClearValue_t;

		struct RenderTarget
		{
			AttachmentName	name;
			RC<Image>		image;
			ClearValue_t	clear;

			ND_ bool  HasClearValue ()	C_NE___	{ return not IsNullUnion( clear ); }
		};

		using RenderTargets_t	= Array< RenderTarget >;

		struct DynamicData
		{
			uint	frame		= 0;
			uint	prevFrame	= UMax;
		};


	public:
		using DynamicFloatTypes_t	= TypeList< DynamicFloat, DynamicFloat2, DynamicFloat3, DynamicFloat4 >;
		using DynamicIntTypes_t		= TypeList< DynamicInt, DynamicInt2, DynamicInt3, DynamicInt4,
												DynamicUInt, DynamicUInt2, DynamicUInt3, DynamicUInt4,
												DynamicDim >;

		struct Constants
		{
			static constexpr uint	MaxCount = 8;

			StaticArray< RC<>, MaxCount >	f;
			StaticArray< RC<>, MaxCount >	i;
		};


	// variables
	protected:
		RC<IController>			_controller;
		Constants				_shConst;

		RC<DynamicUInt>			_repeatCount;

		RC<DynamicFloat>		_passTime;
		PerFrameTimeQuery_t		_timeQuery;

		mutable DynamicData		_dynData;		// used only in 'Upload()'

		String					_dbgName;
		RGBA8u					_dbgColor;

		struct {
			RC<DynamicUInt>		dynamic;
			uint				ref			= 0;
			ECompare			op			= Default;
		}					_enablePass;


	// interface
	public:
		~IPass ()																			__NE_OV;


		ND_ StringView			GetName ()													C_NE___ { return _dbgName; }
		ND_ RC<IController>		GetController ()											C_NE___	{ return _controller; }

		ND_ virtual EPassType	GetType ()													C_NE___ = 0;

		// EPassType::Update	- returns 'false' on error
		// EPassType::Export	- returns 'false' if not complete
		ND_ virtual bool		Update (TransferCtx_t &, const UpdatePassData &)			__Th___	{ DBG_WARNING("Update");  return false; }

		// EPassType::Sync
		ND_ virtual bool		Execute (SyncPassData &)									__Th___	{ DBG_WARNING("Execute");  return false; }

		// EPassType::Async
		ND_ virtual AsyncTask	ExecuteAsync (const AsyncPassData &)						__Th___	{ DBG_WARNING("ExecuteAsync");  return null; }

		// EPassType::Present
		ND_ virtual AsyncTask	PresentAsync (const PresentPassData &)						__Th___	{ DBG_WARNING("PresentAsync");  return null; }

		// EPassType::SeparateBatch
		//ND_ virtual Tuple< CommandBatchPtr, AsyncTask >  SeparateBatch (const PassData &)	{ DBG_WARNING("SeparateBatch");  return Default; }

			virtual void		GetResourcesToResize (INOUT Array<RC<IResource>> &)			__NE___ = 0;


	protected:
		IPass ();
		explicit IPass (StringView dbgName, RGBA8u dbgColor = Default);

		void  _CopySliders (OUT StaticArray<float4,8> &, OUT StaticArray<int4,8> &,
							OUT StaticArray<float4,8> &)									const;
		void  _CopyConstants (const Constants &, OUT StaticArray<float4,8> &,
							  OUT StaticArray<int4,8> &)									const;

		template <typename CtxType>
		void  _BeginTimeQuery (CtxType &ctx);

		template <typename CtxType>
		void  _EndTimeQuery (CtxType &ctx);

		void  _ReadTimeQuery (FrameUID);

		ND_ bool  _IsEnabled ()																const;
		ND_ uint  _GetRepeatCount ()														const;

		void  _UpdateComputeUB (INOUT AnyTypeRef, const UpdatePassData &pd)					C_NE___;
	};



/*
=================================================
	_BeginTimeQuery
=================================================
*/
	template <typename CtxType>
	void  IPass::_BeginTimeQuery (CtxType &ctx)
	{
		if ( not _passTime )
			return;

	  #if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)

		auto&		qm			= GraphicsScheduler().GetQueryManager();
		FrameUID	frame_id	= ctx.GetFrameId();
		auto&		query		= _timeQuery[ frame_id.Index() ];

		query = qm.AllocQuery( frame_id, EQueueType::Graphics, EQueryType::Timestamp, 2 );
		if ( not query )
			return;

		ctx.WriteTimestamp( query, 0, EPipelineScope::All );
	  #endif
	}

/*
=================================================
	_EndTimeQuery
=================================================
*/
	template <typename CtxType>
	void  IPass::_EndTimeQuery (CtxType &ctx)
	{
		if ( not _passTime )
			return;

	  #if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)

		FrameUID	frame_id	= ctx.GetFrameId();
		auto&		query		= _timeQuery[ frame_id.Index() ];

		if ( not query )
			return;

		ctx.WriteTimestamp( query, 1, EPipelineScope::All );
	  #endif
	}


} // AE::ResEditor
