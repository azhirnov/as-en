// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/IResource.h"
#include "res_editor/GeomSource/IGeomSource.h"
#include "res_editor/Resources/ResourceQueue.h"

namespace AE::ResEditor
{

	//
	// Renderer
	//

	class Renderer final : public EnableRC< Renderer >
	{
	// types
	public:
		using DynSlider_t	= Union< /*RC<DynamicInt>, RC<DynamicInt2>, RC<DynamicInt3>,*/ RC<DynamicInt4>,
									 RC<DynamicUInt>, /*RC<DynamicUInt2>,*/ RC<DynamicUInt3>, /*RC<DynamicUInt4>,*/
									 RC<DynamicFloat>, /*RC<DynamicFloat2>, RC<DynamicFloat3>,*/ RC<DynamicFloat4> >;
		using Sliders_t		= Array< DynSlider_t >;


	private:
		using PassArr_t		= Array< RC<IPass> >;
		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;

		struct ScriptFile
		{
			Path				path;
			FileSystem::Time_t	time;

			explicit ScriptFile (const Path &p) : path{p} {}
		};
		using ScriptFiles_t	= Array< ScriptFile >;


		struct InputData
		{
			float2		cursorPos;				// in pixels
			bool		pressed		= false;	// mouse down or touch pressed
		};
		using InputDataSync = Synchronized< RWSpinLock, InputData >;


	// variables
	private:
		PassArr_t				_passes;
		RC<IController>			_controller;

		Unique<ShaderDebugger>	_shaderDebugger;

		ResourceQueue			_resQueue;

		microseconds			_totalTime		{};
		TimePoint_t				_lastUpdateTime;
		uint					_frameCounter	= 0;

		GfxMemAllocatorPtr		_gfxAlloc;

		InputDataSync			_input;
		Sliders_t				_sliders;

		struct {
			RWSpinLock				guard;
			ScriptFiles_t			files;
			TimePoint_t				lastCheck;
			const secondsf			updateInterval {1.f};
		}						_scriptFile;		// TODO: use Synchronized

		struct {
			StrongImageAndViewID	image2D;
			StrongImageAndViewID	imageCube;

			Strong<RTGeometryID>	rtGeometry;
			Strong<RTSceneID>		rtScene;
		}						_dummyRes;


	// methods
	public:
		Renderer ();
		~Renderer ();
		
		ND_ bool			IsCompleted () const;

			void			ProcessInput (ActionQueueReader reader, OUT bool &switchMode);
		ND_ InputModeName	GetInputMode ()	const;

		ND_ AsyncTask		Execute (ArrayView<AsyncTask> deps);

		ND_ bool			IsFileChanged ();


	// api for ScriptExe
	public:
			void					AddPass (RC<IPass> pass)					__Th___;
			void					SetController (RC<IController> cont)		__Th___;
			void					SetDependencies (Array<Path> deps)			__Th___;
			void					SetSliders (Sliders_t value)				__Th___	{ _sliders = RVRef(value); }

		ND_ ResourceQueue&			GetResourceQueue ()							__NE___	{ return _resQueue; }
		ND_ GfxMemAllocatorPtr		GetAllocator ()								__NE___	{ return _gfxAlloc; }
		ND_ GfxMemAllocatorPtr		GetDynamicAllocator ()						__NE___	{ return null; }	// TODO

		ND_ StrongImageAndViewID	GetDummyImage (const ImageDesc &)			C_NE___;
		ND_ Strong<RTGeometryID>	GetDummyRTGeometry ()						C_NE___;
		ND_ Strong<RTSceneID>		GetDummyRTScene ()							C_NE___;


	private:
		ND_ static RenderTaskCoro	_SyncPasses (PassArr_t updatePasses, PassArr_t passes, IPass::Debugger, IPass::UpdatePassData);
		ND_ RenderTaskCoro			_ReadShaderTrace ();

		void  _PrintDbgTrace (const Array<String> &) const;
		void  _UpdateDynSliders ();

		static void  _CreateDummyImage2D (OUT StrongImageAndViewID &, GfxMemAllocatorPtr);
		static void  _CreateDummyImageCube (OUT StrongImageAndViewID &, GfxMemAllocatorPtr);
		static void  _CreateDummyRTGeometry (OUT Strong<RTGeometryID> &, GfxMemAllocatorPtr);
		static void  _CreateDummyRTScene (OUT Strong<RTSceneID> &, GfxMemAllocatorPtr);
	};
	


	inline ResourceQueue&  IResource::_ResQueue () const {
		return _renderer.GetResourceQueue();
	}

	inline GfxMemAllocatorPtr  IResource::_GfxAllocator () const {
		return _renderer.GetAllocator();
	}

	inline GfxMemAllocatorPtr  IResource::_GfxDynamicAllocator () const {
		return _renderer.GetDynamicAllocator();
	}
	
	
	inline ResourceQueue&  IGeomSource::_ResQueue () const {
		return _renderer.GetResourceQueue();
	}

	inline GfxMemAllocatorPtr  IGeomSource::_GfxAllocator () const {
		return _renderer.GetAllocator();
	}

} // AE::ResEditor
