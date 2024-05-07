// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/IResource.h"
#include "res_editor/GeomSource/IGeomSource.h"
#include "res_editor/Resources/DefaultResources.h"

namespace AE::ResEditor
{

	//
	// Renderer
	//

	class Renderer final : public DefaultResources
	{
	// types
	public:
		using DynSlider_t	= Union< RC<DynamicInt>, RC<DynamicInt2>, RC<DynamicInt3>, RC<DynamicInt4>,
									 RC<DynamicUInt>, RC<DynamicUInt2>, RC<DynamicUInt3>, RC<DynamicUInt4>,
									 RC<DynamicFloat>, RC<DynamicFloat2>, RC<DynamicFloat3>, RC<DynamicFloat4> >;
		using Sliders_t		= Array< DynSlider_t >;


	private:
		using PassArr_t		= Array< RC<IPass> >;
		using CustomKeys_t	= StaticArray< float, 1 >;

		struct ScriptFile
		{
			Path				path;
			FileSystem::Time_t	time;

			explicit ScriptFile (const Path &p) : path{p} {}
		};
		using ScriptFiles_t	= Array< ScriptFile >;


		struct InputData
		{
			float2			cursorPos;					// unorm
			bool			pressed			= false;	// mouse down or touch pressed
			bool			pauseRendering	= false;
			CustomKeys_t	customKeys		= {};
		};
		using InputDataSync = Synchronized< RWSpinLock, InputData >;

		enum class EExportState : ubyte
		{
			Completed	= 0,
			Started		= 1,
			InProgress	= 2,
		};


	// variables
	private:
		PassArr_t				_passes;
		RC<IController>			_controller;

		Unique<ShaderDebugger>	_shaderDebugger;

		microseconds			_totalTime			{};
		Clock					_frameClock;
		uint					_frameCounter		= 0;

		InputDataSync			_input;
		Sliders_t				_sliders;
		const uint				_seed;
		ESurfaceFormat			_reqSurfFormat		= Default;

		/*struct {
			RWSpinLock				guard;
			ScriptFiles_t			files;
			TimePoint_t				lastCheck;
			const secondsf			updateInterval {1.f};
		}						_scriptFile;		// TODO: use Synchronized*/

		Atomic<bool>			_freeze					{false};
		Atomic<bool>			_uploadInProgress		{true};
		Atomic<bool>			_readbackInProgress		{true};
		Atomic<EExportState>	_resExport				{EExportState::Completed};

		static constexpr uint	_minFramesWithoutWork	= 2;


	// methods
	public:
		explicit Renderer (uint seed)											__Th___;
		~Renderer ();

			void			ProcessInput (ActionQueueReader reader, OUT bool &switchMode);
		ND_ InputModeName	GetInputMode ()										const;

		ND_ AsyncTask		Execute (ArrayView<AsyncTask> deps);

		//ND_ bool			IsFileChanged ();

		ND_ String			GetHelpText ()										C_Th___;

		ND_ bool			IsUploadComplete ()									C_NE___	{ return not _uploadInProgress.load(); }

		ND_ bool			FreezeTime ()										C_NE___	{ return _freeze.load(); }
			void			SetFreezeTime (bool freeze)							__NE___	{ _freeze.store( freeze ); }


	// api for ScriptExe
	public:
			void			AddPass (RC<IPass> pass)							__Th___;
			void			SetController (RC<IController> cont)				__Th___;
		//	void			SetDependencies (Array<Path> deps)					__Th___;
			void			SetSliders (Sliders_t value)						__NE___	{ _sliders = RVRef(value); }
			void			SetSurfaceFormat (ESurfaceFormat value)				__Th___	{ _reqSurfFormat = value; }

		ND_ ESurfaceFormat	GetSurfaceFormat ()									C_NE___	{ return _reqSurfFormat; }


	private:
		ND_ static RenderTaskCoro	_SyncPasses (PassArr_t updatePasses, PassArr_t passes, IPass::Debugger, IPass::UpdatePassData) __Th___;
		ND_ static RenderTaskCoro	_ResizeRes (Array<RC<IResource>>)			__Th___;
		ND_ RenderTaskCoro			_ReadShaderTrace ()							__Th___;

		ND_ AsyncTask				_Export (ArrayView<AsyncTask> deps);
		ND_ static RenderTaskCoro	_ExportPasses (PassArr_t, RC<Renderer>, IPass::UpdatePassData)		__Th___;

		void  _PrintDbgTrace (const Array<String> &) const;
		void  _UpdateDynSliders ();
	};



	inline DataTransferQueue&  IResource::_DtTrQueue () const {
		return _renderer.GetDataTransferQueue();
	}

	inline DataTransferQueue&  IGeomSource::_DtTrQueue () const {
		return _renderer.GetDataTransferQueue();
	}


} // AE::ResEditor
