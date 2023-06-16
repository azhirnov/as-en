// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "base/DataSource/FileStream.h"

#include "res_editor/Passes/Renderer.h"
#include "res_editor/EditorUI.h"
#include "res_editor/EditorCore.h"

namespace AE::ResEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	Renderer::Renderer () :
		_lastUpdateTime{ TimePoint_t::clock::now() },
		_gfxAlloc{ MakeRC<GfxLinearMemAllocator>() }
	{
		const auto&	shader_trace_folder	= ResEditorAppConfig::Get().shaderTraceFolder;

		if ( not shader_trace_folder.empty() )
		{
			CHECK_THROW( FileSystem::IsDirectory( shader_trace_folder ));
			_shaderDebugger.reset( new ShaderDebugger{});
		}

		_CreateDummyImage2D( OUT _dummyRes.image2D, _gfxAlloc );
		_CreateDummyImageCube( OUT _dummyRes.imageCube, _gfxAlloc );

		if ( RenderTaskScheduler().GetDevice().GetExtensions().accelerationStructure )
		{
			_CreateDummyRTGeometry( OUT _dummyRes.rtGeometry, _gfxAlloc );
			_CreateDummyRTScene( OUT _dummyRes.rtScene, _gfxAlloc );
		}

		_resQueue.EnqueueImageTransition( _dummyRes.image2D  .image );
		_resQueue.EnqueueImageTransition( _dummyRes.imageCube.image );
	}
	
/*
=================================================
	destructor
=================================================
*/
	Renderer::~Renderer ()
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();

		res_mngr.ReleaseResources( _dummyRes.image2D.image, _dummyRes.image2D.view );
		res_mngr.ReleaseResources( _dummyRes.imageCube.image, _dummyRes.imageCube.view );
		res_mngr.ReleaseResources( _dummyRes.rtGeometry, _dummyRes.rtScene );

		// remove global sliders
		UIInteraction::Instance().RemovePass( this );
	}
	
/*
=================================================
	GetDummyImage
=================================================
*/
	StrongImageAndViewID  Renderer::GetDummyImage (const ImageDesc &desc) C_NE___
	{
		ASSERT( desc.imageDim != Default );

		auto&		res_mngr	= RenderTaskScheduler().GetResourceManager();
		const bool	is_cube		= (desc.imageDim == EImageDim_2D and AllBits( desc.options, EImageOpt::CubeCompatible ));
		const bool	is_2darr	= (desc.imageDim == EImageDim_2D and desc.arrayLayers.Get() > 1);
		const bool	is_2d		= (desc.imageDim == EImageDim_2D and desc.arrayLayers.Get() == 1);

		StrongImageAndViewID	result;
		
		if ( is_cube or is_2darr )
		{
			result.image = res_mngr.AcquireResource( _dummyRes.imageCube.image.Get() );
			result.view  = res_mngr.AcquireResource( _dummyRes.imageCube.view.Get() );
		}
		else
		if ( is_2d )
		{
			result.image = res_mngr.AcquireResource( _dummyRes.image2D.image.Get() );
			result.view  = res_mngr.AcquireResource( _dummyRes.image2D.view.Get() );
		}

		return result;
	}
	
/*
=================================================
	GetDummyRTGeometry
=================================================
*/
	Strong<RTGeometryID>  Renderer::GetDummyRTGeometry () C_NE___
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
		
		return res_mngr.AcquireResource( _dummyRes.rtGeometry.Get() );
	}
	
/*
=================================================
	GetDummyRTScene
=================================================
*/
	Strong<RTSceneID>  Renderer::GetDummyRTScene () C_NE___
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
		
		return res_mngr.AcquireResource( _dummyRes.rtScene.Get() );
	}

/*
=================================================
	_CreateDummyImage2D
=================================================
*/
	void  Renderer::_CreateDummyImage2D (OUT StrongImageAndViewID &dst, GfxMemAllocatorPtr gfxAlloc)
	{
		auto&		res_mngr = RenderTaskScheduler().GetResourceManager();
		ImageDesc	desc;

		desc.SetFormat( EPixelFormat::RGBA8_UNorm );
		desc.SetDimension( uint2{ 2, 2 });
		desc.SetUsage( EImageUsage::Sampled | EImageUsage::TransferSrc );

		dst.image = res_mngr.CreateImage( desc, "dummy image 2d", gfxAlloc );
		CHECK_ERRV( dst.image );

		dst.view = res_mngr.CreateImageView( ImageViewDesc{desc}, dst.image, "dummy image 2d view" );
		CHECK_ERRV( dst.view );

		FrameGraph().GetStateTracker().AddResource( dst.image, Default, EResourceState::ShaderSample | EResourceState::AllShaders );
	}
	
/*
=================================================
	_CreateDummyImageCube
=================================================
*/
	void  Renderer::_CreateDummyImageCube (OUT StrongImageAndViewID &dst, GfxMemAllocatorPtr gfxAlloc)
	{
		auto&		res_mngr = RenderTaskScheduler().GetResourceManager();
		ImageDesc	desc;

		desc.SetFormat( EPixelFormat::RGBA8_UNorm );
		desc.SetDimension( uint2{ 2, 2 });
		desc.SetUsage( EImageUsage::Sampled | EImageUsage::TransferSrc );
		desc.SetArrayLayers( 6 );
		desc.SetOptions( EImageOpt::CubeCompatible );

		dst.image = res_mngr.CreateImage( desc, "dummy image cube", gfxAlloc );
		CHECK_ERRV( dst.image );

		dst.view = res_mngr.CreateImageView( ImageViewDesc{desc}, dst.image, "dummy image cube view" );
		CHECK_ERRV( dst.view );

		FrameGraph().GetStateTracker().AddResource( dst.image, Default, EResourceState::ShaderSample | EResourceState::AllShaders );
	}
	
/*
=================================================
	_CreateDummyRTGeometry
=================================================
*/
	void  Renderer::_CreateDummyRTGeometry (OUT Strong<RTGeometryID> &dst, GfxMemAllocatorPtr gfxAlloc)
	{
		auto&			res_mngr = RenderTaskScheduler().GetResourceManager();
		RTGeometryDesc	desc;

		desc.options	= Default;
		desc.size		= 16_b;

		dst = res_mngr.CreateRTGeometry( desc, "dummy RTGeometry", gfxAlloc );
		CHECK_ERRV( dst );
	}
	
/*
=================================================
	_CreateDummyRTScene
=================================================
*/
	void  Renderer::_CreateDummyRTScene (OUT Strong<RTSceneID> &dst, GfxMemAllocatorPtr gfxAlloc)
	{
		auto&		res_mngr = RenderTaskScheduler().GetResourceManager();
		RTSceneDesc	desc;

		desc.options	= Default;
		desc.size		= 16_b;

		dst = res_mngr.CreateRTScene( desc, "dummy RTScene", gfxAlloc );
		CHECK_ERRV( dst );
	}

/*
=================================================
	IsCompleted
=================================================
*/
	bool  Renderer::IsCompleted () const
	{
		// TODO
		return false;
	}
	
/*
=================================================
	GetInputMode
=================================================
*/
	InputModeName  Renderer::GetInputMode () const
	{
		return _controller ? _controller->GetInputMode() : Default;
	}

/*
=================================================
	ProcessInput
=================================================
*/
	void  Renderer::ProcessInput (ActionQueueReader reader, OUT bool &switchMode)
	{
		if ( _controller )
			_controller->ProcessInput( reader, RenderTaskScheduler().GetFrameTimeDelta() );

		auto	input = _input.WriteNoLock();
		EXLOCK( input );

		switchMode		= false;
		input->pressed	= false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			// for UI compatibility
			{
				if_unlikely( hdr.name == InputActionName{"UI.MousePos"} )
					input->cursorPos = reader.Data<packed_float2>( hdr.offset );
			
				if_unlikely( hdr.name == InputActionName{"UI.MouseRBDown"} )
					input->pressed = true;
			}

			if_unlikely( hdr.name == InputActionName{"SwitchInputMode"} )
				switchMode = true;
		}
	}

/*
=================================================
	Execute
----
	'deps' - must contains 
=================================================
*/
	AsyncTask  Renderer::Execute (ArrayView<AsyncTask> inDeps)
	{
		using EPass = IPass::EPassType;

		_UpdateDynSliders();

		auto&	rg		= FrameGraph();

		auto	batch	= rg.Render( "RenderPasses" );
		CHECK_ERR( batch );
		
		auto	ui_batch = rg.UI();
		CHECK_ERR( ui_batch );

		auto	surf_acquire = rg.BeginOnSurface( ui_batch );
		
		Array<AsyncTask>		out_deps;
		PassArr_t				update_passes;
		PassArr_t				sync_passes;
		PassArr_t				present;
		IPass::UpdatePassData	update_pd;
		IPass::Debugger			pass_debugger;

		// update timers
		{
			const auto	time	= TimePoint_t::clock::now();
			const auto	dt		= time - _lastUpdateTime;

			update_pd.frameTime	= secondsf{dt};
			update_pd.totalTime	= secondsf{_totalTime};
			update_pd.frameId	= _frameCounter;

			_totalTime		+= Cast<microseconds>( dt );
			_lastUpdateTime  = time;
			_frameCounter	 ++;
			
			auto	input = _input.ReadNoLock();
			SHAREDLOCK( input );

			update_pd.cursorPos	= input->cursorPos;
			update_pd.pressed	= input->pressed;
		}

		// setup shader debugger
		{
			auto	dbg = UIInteraction::Instance().debugger.WriteNoLock();
			EXLOCK( dbg );
			pass_debugger.debugger	= _shaderDebugger.get();
			pass_debugger.target	= dbg->target;
			pass_debugger.mode		= dbg->mode;
			pass_debugger.stage		= dbg->stage;
			pass_debugger.coord		= dbg->coord;
		}

		for (auto& pass : _passes)
		{
			if ( AllBits( pass->GetType(), EPass::Update ))
				update_passes.push_back( pass );
		}

		// upload resources
		{
			Unused( _resQueue.Upload( batch, inDeps ));
		}

		const auto	RunSyncPasses = [&] ()
		{{
			if ( update_passes.size() | sync_passes.size() )
			{
				Unused( batch.Task( _SyncPasses( RVRef(update_passes), RVRef(sync_passes), pass_debugger, update_pd ),
								    DebugLabel{ "SyncPasses", HtmlColor::Blue })
								.Run( Tuple{inDeps} ));
			}
		}};
		
		for (auto& pass : _passes)
		{
			if ( AnyBits( pass->GetType(), EPass::Present ))
				present.push_back( pass );
		}
		if ( not present.empty() and surf_acquire )
		{
			Array<AsyncTask>	surf_deps {inDeps};
			surf_deps.push_back( surf_acquire );

			IPass::PresentPassData	pd;
			pd.batch	= rg.UI();
			pd.surface	= rg.GetSurface();
			pd.deps		= surf_deps;

			CHECK_ERR( pd.surface );

			for (auto& pass : present) {
				out_deps.push_back( pass->PresentAsync( pd ));
			}
		}

		for (auto& pass : _passes)
		{
			for (auto types = pass->GetType(); types != Default;)
			{
				const auto	t = ExtractBit( INOUT types );
				
				BEGIN_ENUM_CHECKS();
				switch ( t )
				{
					case EPass::Sync :
					{
						sync_passes.push_back( pass );
						break;
					}
					
					/*{
						RunSyncPasses();

						IPass::AsyncPassData	pd;
						pd.batch	= batch;
						pd.debugger	= &_shaderDebugger;
						pd.deps		= deps;

						auto	task = pass->ExecuteAsync( pd );
						CHECK_ERR( task );

						deps.clear();
						deps.push_back( task );
						break;
					}*/

					case EPass::Update :
					{
						update_passes.push_back( pass );
						break;
					}

					case EPass::Present :	break;	// already processed
					case EPass::Async :
					case EPass::Unknown :
					default :
						RETURN_ERR( "unknown pass type" );
				}
				END_ENUM_CHECKS();
			}
		}
		
		RunSyncPasses();

		out_deps.push_back( batch.SubmitAsTask() );


		// read shader debugger output
		if ( _shaderDebugger and _shaderDebugger->HasPendingRequests() )
		{
			out_deps.push_back( rg.UI().Task(
				_ReadShaderTrace(),
				{"Read shader debugger output"} ).Run() );
		}

		return Scheduler().WaitAsync( ETaskQueue::Renderer, Tuple{ArrayView{out_deps}} );
	}
	
/*
=================================================
	_SyncPasses
=================================================
*/
	RenderTaskCoro  Renderer::_SyncPasses (PassArr_t updatePasses, PassArr_t passes, IPass::Debugger dbg, IPass::UpdatePassData updatePD)
	{
		auto&				rtask	= co_await RenderTask_GetRef;
		IPass::SyncPassData	pd		{ rtask };

		pd.dbg = dbg;

		// begin
		{
			DirectCtx::Transfer		ctx{ rtask };

			for (auto& pass : updatePasses) {
				CHECK_CE( pass->Update( ctx, updatePD ));
			}

			pd.cmdbuf = ctx.ReleaseCommandBuffer();
		}

		for (auto& pass : passes)
		{
			ASSERT( AllBits( pass->GetType(), IPass::EPassType::Sync ));
			CHECK_CE( pass->Execute( pd ));
		}

		// end
		{
			DirectCtx::Graphics		ctx{ rtask, RVRef(pd.cmdbuf) };
			co_await RenderTask_Execute{ ctx };
		}
	}

/*
=================================================
	AddPass
=================================================
*/
	void  Renderer::AddPass (RC<IPass> pass) __Th___
	{
		CHECK_THROW( pass );
		_passes.emplace_back( RVRef(pass) );
	}
	
/*
=================================================
	SetController
=================================================
*/
	void  Renderer::SetController (RC<IController> cont) __Th___
	{
		CHECK_THROW( cont );
		CHECK_THROW( not _controller );

		_controller = RVRef(cont);
	}
	
/*
=================================================
	SetDependencies
=================================================
*/
	void  Renderer::SetDependencies (Array<Path> dependencies) __Th___
	{
		EXLOCK( _scriptFile.guard );

		for (auto& dep : dependencies) {
			_scriptFile.files.push_back( ScriptFile{ dep });
		}
		//RemoveDuplicates( INOUT _scriptFile.files, [](auto& lhs, auto& rhs) { return lhs.path < rhs.path; });

		for (auto& f : _scriptFile.files) {
			f.time = FileSystem::LastWriteTime( f.path );
		}

		_scriptFile.lastCheck = TimePoint_t::clock::now();
	}

/*
=================================================
	IsFileChanged
=================================================
*/
	bool  Renderer::IsFileChanged ()
	{
		SHAREDLOCK( _scriptFile.guard );

		const auto	cur_time = TimePoint_t::clock::now();

		if ( secondsf{cur_time - _scriptFile.lastCheck} < _scriptFile.updateInterval )
			return false;
		
		_scriptFile.lastCheck = cur_time;

		for (auto& f : _scriptFile.files)
		{
			auto	t = FileSystem::LastWriteTime( f.path );

			if_unlikely( t != f.time )
				return true;
		}
		return false;
	}
	
/*
=================================================
	_ReadShaderTrace
=================================================
*/
	RenderTaskCoro  Renderer::_ReadShaderTrace ()
	{
		auto&				rtask	= co_await RenderTask_GetRef;
		DirectCtx::Transfer	ctx		{rtask};

		_shaderDebugger->ReadAll( ctx, ShaderDebugger::ELogFormat::VS )
			.Then( [this] (const Array<String> &output)
					{
						_PrintDbgTrace( output );
					});

		co_await RenderTask_Execute( ctx );
	}
	
/*
=================================================
	_PrintDbgTrace
=================================================
*/
	void  Renderer::_PrintDbgTrace (const Array<String> &output) const
	{
		if ( output.empty() )
			return;

		const auto&	shader_trace_folder	= ResEditorAppConfig::Get().shaderTraceFolder;

		const auto	BuildName = [this, &shader_trace_folder] (OUT Path &fname, usize index)
		{{
			fname = shader_trace_folder / ("trace_"s << ToString(index) << ".glsl_dbg");
		}};

		usize		trace_idx	= 0;
		const auto	WriteToFile = [&output, &trace_idx] (const Path &fname) -> bool
		{{
			StringView		str		= output[trace_idx];
			FileWStream		file	{fname};
			if ( file.IsOpen() and file.Write( str ))
				AE_LOGI( "trace saved", ToString(fname), 1u );

			++trace_idx;
			return not (trace_idx < output.size());
		}};
		
		FileSystem::FindUnusedFilename( BuildName, WriteToFile );
	}
	
/*
=================================================
	_UpdateDynSliders
=================================================
*/
	void  Renderer::_UpdateDynSliders ()
	{
		uint	i_idx	= 0;
		uint	f_idx	= 0;

		if ( auto p_sliders = UIInteraction::Instance().GetSliders( this ))
		{
			auto	sliders = p_sliders->ReadNoLock();
			SHAREDLOCK( sliders );

			for (auto& slider : _sliders)
			{
				Visit( slider,
					[&] (RC<DynamicInt4> &dst)	{ dst->Set( sliders->intSliders[i_idx] );			++i_idx; },
					[&] (RC<DynamicUInt> &dst)	{ dst->Set( uint(sliders->intSliders[i_idx].x) );	++i_idx; },
					[&] (RC<DynamicUInt3> &dst)	{ dst->Set( uint3(sliders->intSliders[i_idx]) );	++i_idx; },
					[&] (RC<DynamicFloat> &dst)	{ dst->Set( sliders->floatSliders[f_idx].x );		++f_idx; },
					[&] (RC<DynamicFloat4> &dst){ dst->Set( sliders->floatSliders[f_idx] );			++f_idx; }
				);
			}
		}
	}


} // AE::ResEditor
