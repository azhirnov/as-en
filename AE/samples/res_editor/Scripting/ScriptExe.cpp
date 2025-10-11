// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptExe.h"
#include "Passes/BuildRTAS.h"
#include "Passes/Export.h"
#include "Passes/ImageCompression.h"

#include "Core/EditorCore.h"
#include "Core/EditorUI.h"
#include "Scripting/PipelineCompiler.inl.h"


AE_DECL_SCRIPT_OBJ( AE::ResEditor::ScriptExe::EnableLabel,  "EnableLabel" );


namespace AE::ResEditor
{
namespace {
	static ScriptExe*  s_scriptExe = null;
}

	using namespace AE::Scripting;
	using namespace AE::PipelineCompiler;

	#include "Scripting/ScriptExe_Passes.inl.h"


	//
	// Temporary Data
	//
	struct ScriptExe::TempData
	{
	// types
		using Labels_t	= UIInteraction::Labels_t;

	// variables
		ScriptConfig		cfg;
		RC<Renderer>		renderer;
		ScriptPassGroupPtr	passGroup;
		Array< Path >		currPath;
		Array< Path >		dependencies;
		uint				dbgViewCounter		= 0;
		int					passGroupDepth		= 0;

		bool				hasPresent			= false;

		Sliders_t			sliders;
		UniqueSliderNames_t	uniqueSliderNames;
		SliderCounter_t		sliderCounter		{};

		Labels_t			labels;
	};
//-----------------------------------------------------------------------------



	struct ScriptExe::SamplerConsts
	{
		const String	NearestClamp			{"NearestClamp"};
		const String	NearestRepeat			{"NearestRepeat"};
		const String	NearestMirrorRepeat		{"NearestMirrorRepeat"};
		const String	LinearClamp				{"LinearClamp"};
		const String	LinearRepeat			{"LinearRepeat"};
		const String	LinearMirrorRepeat		{"LinearMirrorRepeat"};
		const String	LinearMipmapClamp		{"LinearMipmapClamp"};
		const String	LinearMipmapRepeat		{"LinearMipmapRepeat"};
		const String	LinearMipmapMirrorRepeat{"LinearMipmapMirrorRepeat"};
		const String	LinearMipmapMirrorClamp	{"LinearMipmapMirrorClamp"};
		const String	Anisotropy8Repeat		{"Anisotropy8Repeat"};
		const String	Anisotropy8MirrorRepeat	{"Anisotropy8MirrorRepeat"};
		const String	Anisotropy8Clamp		{"Anisotropy8Clamp"};
		const String	Anisotropy16Repeat		{"Anisotropy16Repeat"};
		const String	Anisotropy16MirrorRepeat{"Anisotropy16MirrorRepeat"};
		const String	Anisotropy16Clamp		{"Anisotropy16Clamp"};
		const String	NearestClampSubsampled	{"NearestClampSubsampled"};
		const String	MaxLinearClamp			{"MaxLinearClamp"};
		const String	MinLinearClamp			{"MinLinearClamp"};
	};

/*
=================================================
	constructor
=================================================
*/
	ScriptExe::ScriptExe (Config cfg) :
		_sampConsts{ new SamplerConsts{} },
		_config{ RVRef(cfg) }
	{
		EXLOCK( _guard );

		s_scriptExe = this;

		try{
			// bind RE scripts
			_engine = MakeRC<ScriptEngine>();
			CHECK_THROW( _engine->Create( True{"gen cpp header"} ));

			// init pipeline compiler
			ObjectStorage	obj_storage;
			{
				ObjectStorage::SetInstance( &obj_storage );

				ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ "InitialFS" }};
				fs->fs = ScriptResourceApi::GetFeatureSet();

				obj_storage.target				= ECompilationTarget::Vulkan;
				obj_storage.shaderVersion		= EShaderVersion(Version2::From100( fs->fs.maxShaderVersion.spirv ).ToHex()) | EShaderVersion::_GLSL_SPIRV;
				obj_storage.defaultFeatureSet	= fs->Name();
				obj_storage.defaultLayout		= EStructLayout::Compatible_Std140;

				obj_storage.spirvCompiler		= MakeUnique<SpirvCompiler>( Array<Path>{} );
				obj_storage.spirvCompiler->SetDefaultResourceLimits();
			}

			// bind pipeline compiler scripts
			_engine2 = MakeRC<ScriptEngine>();
			ObjectStorage::Bind( _engine2 );
			_Bind_Constants( _engine2 );

			ObjectStorage::Bind_EImageType( _engine );

			// save RE shader types as C++ types
			if ( not _config.cppTypesFolder.empty() )
			{
				FileSystem::CreateDirectory( _config.cppTypesFolder );
				_SaveCppStructs( _config.cppTypesFolder / "vk_types.h" );	// TODO: metal?
			}

			if ( FileSystem::IsDirectory( _config.scriptHeaderOutFolder ))
				CHECK( _engine2->SaveCppHeader( _config.scriptHeaderOutFolder / "pipeline_compiler.as" ));

			ObjectStorage::SetInstance( null );

			CoreBindings::BindString( _engine );
			_Bind_Constants( _engine );
			_Bind_Enums( _engine );
			_Bind( _engine, _config );
		}
		catch(...)
		{
			_engine  = null;
			_engine2 = null;

			AE_LOGE( "Failed to initialize script executor" );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptExe::~ScriptExe ()
	{
		EXLOCK( _guard );
		s_scriptExe = null;
	}

/*
=================================================
	Run
=================================================
*/
	RC<Renderer>  ScriptExe::Run (const Path &filePath, const ScriptConfig &cfg) __NE___
	{
		AE_LOGI( "------------------------------------------" );

		ASSERT( _guard.is_unlocked() );
		EXLOCK( _guard );
		CHECK_ERR( not _tempData );

		_tempData.reset( new TempData{} );
		{
			uint	seed = _rand.Uniform( 0u, 0xFFFF'FFFFu );
			if ( not cfg.enableRandomizer )
				seed = 0;

			_tempData->renderer = MakeRCTh<Renderer>( seed );  // throw
		}
		_tempData->currPath.push_back( FileSystem::ToAbsolute( filePath ));
		_tempData->dependencies.push_back( _tempData->currPath.front() );
		_tempData->cfg = cfg;

		for (auto& p : _tempData->cfg.shaderDirs) {
			p = FileSystem::ToAbsolute( p );
		}

		RC<Renderer>	result;

		if ( _Run2( filePath ))
		{
			result = _tempData->renderer;
			//result->SetDependencies( RVRef(_tempData->dependencies) );

			_AddSlidersToUIInteraction( *_tempData, result.get() );
		}

		// in VS: click in console to open script
	  #ifdef AE_COMPILER_MSVC
		if ( result and PlatformUtils::IsUnderDebugger() )
			AE_LOGI( "<<<<< Loaded script >>>>>", SourceLoc( ToString(filePath).c_str(), 1 ));
	  #endif

		_tempData.reset();
		return result;
	}

/*
=================================================
	_AddSlidersToUIInteraction
=================================================
*/
	void  ScriptExe::_AddSlidersToUIInteraction (TempData &data, Renderer* renderer) __NE___
	{
		if ( not data.labels.empty() )
		{
			UIInteraction::Instance().AddLabels( renderer, RVRef(data.labels) );
			data.labels.clear();
		}

		if ( data.sliders.empty() )
			return;

		UIInteraction::PerPassSlidersInfo	info;
		SliderCounter_t						slider_idx {};
		Renderer::Sliders_t					dyn_sliders;

		for (const auto& slider : data.sliders)
		{
			const uint	idx = slider_idx[ uint(slider.type) ]++;
			switch_enum( slider.type )
			{
				case ESlider::Int :
					info.intRange [idx][0]							= slider.intRange[0];
					info.intRange [idx][1]							= slider.intRange[1];
					info.intRange [idx][2]							= slider.intRange[2];
					info.intVecSize [idx]							= slider.count;
					info.names [idx][UIInteraction::IntSliderIdx]	= slider.name;
					dyn_sliders.push_back( slider.dyn );
					break;

				case ESlider::Float :
					info.floatRange [idx][0]						= slider.floatRange[0];
					info.floatRange [idx][1]						= slider.floatRange[1];
					info.floatRange [idx][2]						= slider.floatRange[2];
					info.floatVecSize [idx]							= slider.count;
					info.names [idx][UIInteraction::FloatSliderIdx]	= slider.name;
					dyn_sliders.push_back( slider.dyn );
					break;

				case ESlider::Color :
				case ESlider::_Count :
					break;
			}
			switch_end
		}

		info.passName = "Global";
		UIInteraction::Instance().AddSliders( renderer, RVRef(info) );

		renderer->SetSliders( RVRef(dyn_sliders) );
	}

/*
=================================================
	_RunScript*
=================================================
*/
	ScriptBasePass*  ScriptExe::_RunScript1 (const String &filePath, const ScriptCollectionPtr &collection) __Th___
	{
		return _RunScript2( filePath, Default, collection );
	}

	ScriptBasePass*  ScriptExe::_RunScript2 (const String &filePath, PassGroup::EFlags flags, const ScriptCollectionPtr &collection) __Th___
	{
		CHECK_THROW_MSG( collection );

		auto&		data	 = _GetTempData();
		const auto	path	 = Path{data.cfg.scriptDir} / filePath;

		CHECK_THROW_MSG( data.passGroup );
		CHECK_THROW_MSG( FileSystem::IsFile( path ),
			"script '"s << filePath << "' is not exists" );

		ScriptPassGroupPtr	pg {new ScriptPassGroup{ flags, data.renderer }};
		ScriptPassGroupPtr	prev = data.passGroup;
		data.passGroup = pg;

		data.currPath.push_back( FileSystem::ToAbsolute( path ));

		CHECK_THROW_MSG( s_scriptExe->_Run( path, collection ),
			"Failed to run script '"s << filePath << "'" );

		data.currPath.pop_back();
		data.passGroup = prev;
		data.passGroup->Add( pg );

		return pg.Detach();
	}

/*
=================================================
	_Run2
=================================================
*/
	bool  ScriptExe::_Run2 (const Path &filePath) __NE___
	{
		try
		{
			CHECK_ERR( _tempData->passGroup == null );

			ScriptPassGroupPtr	pg {new ScriptPassGroup{ PassGroup::EFlags::Unknown, _tempData->renderer }};
			_tempData->passGroup = pg;

			if ( not _Run( filePath, null ))
				return false;

			for (auto& script_pass : pg->GetPasses())
			{
				auto	pass = script_pass->ToPass();	// throw
				CHECK_ERR( pass );

				if ( auto cont = pass->GetController())
					_tempData->renderer->SetController( RVRef(cont) );	// throw

				_tempData->renderer->AddPass( pass );	// throw
			}

			return true;
		}
		catch(...)
		{}
		return false;
	}

/*
=================================================
	_Run
=================================================
*/
	bool  ScriptExe::_Run (const Path &filePath, const ScriptCollectionPtr &collection) __NE___
	{
		CHECK_ERR( _engine );

		_tempData->passGroupDepth++;

		const String				ansi_path = ToString( filePath );
		ScriptEngine::ModuleSource	src;
		{
			FileRStream		file {filePath};

			if ( not file.IsOpen() )
				RETURN_ERR( "Failed to open script file: '"s << ansi_path << "'" );

			src.name = ToString( filePath.stem() );

			if ( not file.Read( file.RemainingSize(), OUT src.script ))
				RETURN_ERR( "Failed to read script file: '"s << ansi_path << "'" );
		}

		src.dbgLocation		= SourceLoc{ ansi_path.c_str(), 0 };
		src.usePreprocessor	= true;

		ScriptModulePtr		module = _engine->CreateModule( {src}, {"SCRIPT"}, _config.scriptIncludeDirs );
		if ( not module )
			RETURN_ERR( "Failed to parse script file: '"s << ansi_path << "'" );

		if ( collection		or
			 module->HasFunction< void (ScriptCollectionPtr) >( "ASmain" ))	// callable script can be used too
		{
			auto	fn = _engine->CreateScript< void (ScriptCollectionPtr) >( "ASmain", module );
			if ( not fn )
				RETURN_ERR( "Failed to create script context for file: '"s << ansi_path << "'" );

			if ( not fn->Run( collection ))
				RETURN_ERR( "Failed to run script '"s << ansi_path << "'" );
		}
		else
		{
			auto	fn = _engine->CreateScript< void () >( "ASmain", module );
			if ( not fn )
				RETURN_ERR( "Failed to create script context for file: '"s << ansi_path << "'" );

			if ( not fn->Run() )
				RETURN_ERR( "Failed to run script '"s << ansi_path << "'" );
		}

		_tempData->passGroupDepth--;
		_tempData->passGroup = null;
		return true;
	}

/*
=================================================
	_ConvertAndLoad
=================================================
*/
	RTechInfo  ScriptExe::_ConvertAndLoad () __Th___
	{
		auto	mem3	 = _ConvertAndLoad2();
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		PipelinePackDesc	desc;
		desc.stream			= mem3;

		GAutorelease	pack_id	= res_mngr.LoadPipelinePack( desc );
		CHECK_THROW( pack_id );

		auto			rtech	= res_mngr.LoadRenderTech( pack_id, RenderTechName{"rtech"} );
		CHECK_THROW( rtech );

		return RTechInfo{ pack_id.Release(), rtech };
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_GetTempData
=================================================
*/
	ScriptExe::TempData&  ScriptExe::_GetTempData () __Th___
	{
		CHECK_THROW_MSG( s_scriptExe != null );
		CHECK_THROW_MSG( s_scriptExe->_tempData != null );
		return *s_scriptExe->_tempData;
	}

/*
=================================================
	_SurfaceSize
=================================================
*/
	ScriptDynamicDim*  ScriptExe::_SurfaceSize () __Th___
	{
		ScriptDynamicDimPtr	result{new ScriptDynamicDim{ _GetTempData().cfg.dynSize }};
		return result.Detach();
	}

/*
=================================================
	_Present*
=================================================
*/
	void  ScriptExe::_Present1 (const ScriptImagePtr &rt) __Th___
	{
		return _Present6( rt, ImageLayer{}, MipmapLevel{}, Default );
	}

	void  ScriptExe::_Present2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap) __Th___
	{
		return _Present6( rt, ImageLayer{}, mipmap, Default );
	}

	void  ScriptExe::_Present3 (const ScriptImagePtr &rt, const ImageLayer &layer) __Th___
	{
		return _Present6( rt, layer, MipmapLevel{}, Default );
	}

	void  ScriptExe::_Present4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap) __Th___
	{
		return _Present6( rt, layer, mipmap, Default );
	}

	void  ScriptExe::_Present5 (const ScriptImagePtr &rt, EColorSpace cs) __Th___
	{
		return _Present6( rt, ImageLayer{}, MipmapLevel{}, cs );
	}

	void  ScriptExe::_Present6 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, EColorSpace cs) __Th___
	{
		CHECK_THROW_MSG( rt );

		rt->AddUsage( EResourceUsage::Present );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroupDepth == 1, "'Present()' must be used in main script" );
		CHECK_THROW_MSG( not data.hasPresent, "'Present()' must be used once" );

		data.hasPresent = true;
		data.renderer->SetSurfaceFormat( cs != Default ? ESurfaceFormat_Cast( rt->PixelFormat(), cs ) : Default );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptPresent{ rt, layer, mipmap, data.cfg.dynSize }});
	}

/*
=================================================
	_GenMipmaps
=================================================
*/
	void  ScriptExe::_GenMipmaps (const ScriptImagePtr &rt) __Th___
	{
		CHECK_THROW_MSG( rt );
		CHECK_THROW_MSG( rt->MipmapCount() > 1 or AllBits( rt->Flags(), Image::EImageFlags::AllMipmaps ));

		rt->AddUsage( EResourceUsage::GenMipmaps );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptGenMipmaps{ rt }});
	}

/*
=================================================
	_CompressImage
=================================================
*/
	void  ScriptExe::_CompressImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) __Th___
	{
		CHECK_THROW_MSG( dst );
		_CompressImage2( src, dst, dst->PixelFormat() );
	}

	void  ScriptExe::_CompressImage2 (const ScriptImagePtr &src, const ScriptImagePtr &dst, EPixelFormat dstFormat) __Th___
	{
		CHECK_THROW_MSG( src );
		CHECK_THROW_MSG( dst );

		auto&	src_fmt	= EPixelFormat_GetInfo( src->PixelFormat() );
		auto&	dst_fmt	= EPixelFormat_GetInfo( dstFormat );

		CHECK_THROW_MSG( not src_fmt.IsCompressed() and dst_fmt.IsCompressed() );
		CHECK_THROW_MSG( src_fmt.IsColor() and dst_fmt.IsColor() );

		CHECK_THROW_MSG( src->ArrayLayers() == dst->ArrayLayers() );
		CHECK_THROW_MSG( src->MipmapCount() == dst->MipmapCount() );

		if ( src->IsMutableDimension() and dst->IsMutableDimension() )
		{
			CHECK_THROW_MSG( src->DimensionRC() == dst->DimensionRC() );
		}
		else
		{
			CHECK_THROW_MSG( not src->IsMutableDimension() );
			CHECK_THROW_MSG( not dst->IsMutableDimension() );
			CHECK_THROW_MSG( All( src->Dimension3() == dst->Dimension3() ));

			CHECK_THROW_MSG( All( IsMultipleOf( uint2{dst->Dimension2()}, dst_fmt.TexBlockDim() )));
			CHECK_THROW_MSG( dst->PixelFormat() == src->PixelFormat() or dstFormat == dst->PixelFormat() );
		}

		src->AddUsage( EResourceUsage::WillReadback );
		dst->AddUsage( EResourceUsage::UploadedData );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptCompressImage{ src, dst, dstFormat }});
	}

/*
=================================================
	_CopyImage
=================================================
*/
	void  ScriptExe::_CopyImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) __Th___
	{
		CHECK_THROW_MSG( src and dst );
		src->AddUsage( EResourceUsage::Transfer );
		dst->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptCopyImage{ src, dst }});
	}
	
/*
=================================================
	_CopyImage2
=================================================
*/
	void  ScriptExe::_CopyImage2 (const ScriptImagePtr &src, const ScriptImagePtr &dst) __Th___
	{
		CHECK_THROW_MSG( src and dst );
		src->AddUsage( EResourceUsage::Sampled );
		dst->AddUsage( EResourceUsage::ComputeWrite );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptCopyImage2{ src, dst }});
	}

/*
=================================================
	_BlitImage
=================================================
*/
	void  ScriptExe::_BlitImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) __Th___
	{
		CHECK_THROW_MSG( src and dst );
		src->AddUsage( EResourceUsage::Transfer );
		dst->AddUsage( EResourceUsage::GenMipmaps );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptBlitImage{ src, dst }});
	}

/*
=================================================
	_ResolveImage
=================================================
*/
	void  ScriptExe::_ResolveImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) __Th___
	{
		CHECK_THROW_MSG( src and dst );
		src->AddUsage( EResourceUsage::Transfer );
		dst->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptResolveImage{ src, dst }});
	}

/*
=================================================
	_ClearImage*
=================================================
*/
	void  ScriptExe::_ClearImage1 (const ScriptImagePtr &image, const RGBA32f &value) __Th___
	{
		CHECK_THROW_MSG( image );
		image->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearImage{ image, value }});
	}

	void  ScriptExe::_ClearImage2 (const ScriptImagePtr &image, const RGBA32u &value) __Th___
	{
		CHECK_THROW_MSG( image );
		image->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearImage{ image, value }});
	}

	void  ScriptExe::_ClearImage3 (const ScriptImagePtr &image, const RGBA32i &value) __Th___
	{
		CHECK_THROW_MSG( image );
		image->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearImage{ image, value }});
	}

/*
=================================================
	_ClearBuffer*
=================================================
*/
	void  ScriptExe::_ClearBuffer1 (const ScriptBufferPtr &buffer, uint value) __Th___
	{
		CHECK_THROW_MSG( buffer );
		buffer->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearBuffer{ buffer, value }});
	}

	void  ScriptExe::_ClearBuffer2 (const ScriptBufferPtr &buffer, ulong offset, ulong size, uint value) __Th___
	{
		CHECK_THROW_MSG( buffer );
		buffer->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearBuffer{ buffer, Bytes{offset}, Bytes{size}, value }});
	}

/*
=================================================
	_ExportImage
=================================================
*/
	void  ScriptExe::_ExportImage (const ScriptImagePtr &image, const String &prefix) __Th___
	{
		CHECK_THROW_MSG( image );
		image->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptExportImage{ image, prefix }});
	}

/*
=================================================
	_DbgExportBuffer
=================================================
*/
	void  ScriptExe::_DbgExportBuffer (const ScriptBufferPtr &buffer, const String &prefix) __Th___
	{
		CHECK_THROW_MSG( buffer );
		buffer->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptExportBuffer{ buffer, prefix, ScriptExportBuffer::EMode::Structured }});
	}

/*
=================================================
	_ExportBuffer
=================================================
*/
	void  ScriptExe::_ExportBuffer (const ScriptBufferPtr &buffer, const String &prefix) __Th___
	{
		CHECK_THROW_MSG( buffer );
		buffer->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptExportBuffer{ buffer, prefix, ScriptExportBuffer::EMode::Binary }});
	}

/*
=================================================
	_ExportGeometry
=================================================
*/
	void  ScriptExe::_ExportGeometry (const ScriptGeomSourcePtr &, const String &) __Th___
	{
		// TODO
	}

/*
=================================================
	_BuildRTGeometry*
=================================================
*/
	void  ScriptExe::_BuildRTGeometry (const ScriptRTGeometryPtr &geom) __Th___
	{
		CHECK_THROW_MSG( geom );
		geom->AllowUpdate();

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTGeometry{ geom, false }});
	}

	void  ScriptExe::_BuildRTGeometryIndirect (const ScriptRTGeometryPtr &geom) __Th___
	{
		CHECK_THROW_MSG( geom );
		CHECK_THROW_MSG( geom->HasIndirectBuffer(),
			"'IndirectBuffer()' is never used, indirect buffer must be initialized before it is used to build RTGeometry" );
		geom->AllowUpdate();

		if ( ScriptResourceApi::GetFeatureSet().accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
		{
			CHECK_THROW_MSG( geom->WithHistory() );
		}

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTGeometry{ geom, true }});
	}

/*
=================================================
	_BuildRTScene*
=================================================
*/
	void  ScriptExe::_BuildRTScene (const ScriptRTScenePtr &scene) __Th___
	{
		CHECK_THROW_MSG( scene );
		scene->AllowUpdate();

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTScene{ scene, false }});
	}

	void  ScriptExe::_BuildRTSceneIndirect (const ScriptRTScenePtr &scene) __Th___
	{
		CHECK_THROW_MSG( scene );
		CHECK_THROW_MSG( scene->HasIndirectBuffer(),
			"'IndirectBuffer()' is never used, indirect buffer must be initialized before it is used to build RTScene" );
		scene->AllowUpdate();

		if ( ScriptResourceApi::GetFeatureSet().accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
		{
			CHECK_THROW_MSG( scene->WithHistory() );
		}

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTScene{ scene, true }});
	}

/*
=================================================
	_DbgView*
=================================================
*/
	void  ScriptExe::_DbgView1 (const ScriptImagePtr &rt, DebugView::EFlags flags) __Th___
	{
		return _DbgView4( rt, ImageLayer{}, MipmapLevel{}, flags );
	}

	void  ScriptExe::_DbgView2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap, DebugView::EFlags flags) __Th___
	{
		return _DbgView4( rt, ImageLayer{}, mipmap, flags );
	}

	void  ScriptExe::_DbgView3 (const ScriptImagePtr &rt, const ImageLayer &layer, DebugView::EFlags flags) __Th___
	{
		return _DbgView4( rt, layer, MipmapLevel{}, flags );
	}

	void  ScriptExe::_DbgView4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags) __Th___
	{
	#if RmG_UI_ON_HOST
		// UI on host can not draw debug view
		Unused( rt, layer, mipmap, flags );

	#else
		CHECK_THROW_MSG( rt );

		auto&	data	= _GetTempData();
		auto	idx		= data.dbgViewCounter++;

		CHECK_THROW_MSG( idx < UIInteraction::MaxDebugViews );

		switch_enum( flags )
		{
			case DebugView::EFlags::Copy :			rt->AddUsage( EResourceUsage::Transfer );		break;
			case DebugView::EFlags::NoCopy :		rt->AddUsage( EResourceUsage::Sampled );		break;
			case DebugView::EFlags::Histogram :		rt->AddUsage( EResourceUsage::Sampled );		break;
			case DebugView::EFlags::LinearDepth :	rt->AddUsage( EResourceUsage::Sampled );		break;
			case DebugView::EFlags::Stencil :		rt->AddUsage( EResourceUsage::DepthStencil );	break;
			case DebugView::EFlags::_Count :
			default :								CHECK_THROW_MSG( false, "unsupported flags" );
		}
		switch_end

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptDbgView{ rt, layer, mipmap, flags, idx }});
	#endif
	}

/*
=================================================
	_Slider
=================================================
*/
	template <typename D, typename T>
	void  ScriptExe::_Slider (const D &dyn, const String &name, const T &min, const T &max, const T &initial, ESlider type) __Th___
	{
		auto&	data = _GetTempData();

		CHECK_THROW_MSG( dyn and dyn->Get() );

		CHECK_THROW_MSG( data.uniqueSliderNames.insert( name ).second,
			"Slider '"s << name << "' is already exists" );

		uint	idx = data.sliderCounter[ uint(type) ]++;
		CHECK_THROW_MSG( idx < UIInteraction::MaxSlidersPerType,
			"Slider count "s << ToString(idx) << " must be less than " << ToString(UIInteraction::MaxSlidersPerType) );

		auto&	dst = data.sliders.emplace_back();

		dst.name	= name;
		dst.index	= idx;
		dst.count	= sizeof(T) / sizeof(int);
		dst.type	= type;
		dst.dyn		= dyn->Get();

		MemCopy( OUT &dst.intRange[0], &min, Sizeof(min) );
		MemCopy( OUT &dst.intRange[1], &max, Sizeof(max) );
		MemCopy( OUT &dst.intRange[2], &initial, Sizeof(initial) );
	}

/*
=================================================
	_SliderI*
=================================================
*/
	void ScriptExe:: _SliderI0 (const ScriptDynamicIntPtr &dyn, const String &name) __Th___
	{
		int	min = 0, max = 1024;
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderI1 (const ScriptDynamicIntPtr &dyn,const String &name, int min, int max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderI2 (const ScriptDynamicInt2Ptr &dyn,const String &name, const packed_int2 &min, const packed_int2 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderI3 (const ScriptDynamicInt3Ptr &dyn,const String &name, const packed_int3 &min, const packed_int3 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderI4 (const ScriptDynamicInt4Ptr &dyn,const String &name, const packed_int4 &min, const packed_int4 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderI1a (const ScriptDynamicIntPtr &dyn,const String &name, int min, int max, int val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

	void  ScriptExe::_SliderI2a (const ScriptDynamicInt2Ptr &dyn,const String &name, const packed_int2 &min, const packed_int2 &max, const packed_int2 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

	void  ScriptExe::_SliderI3a (const ScriptDynamicInt3Ptr &dyn,const String &name, const packed_int3 &min, const packed_int3 &max, const packed_int3 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

	void  ScriptExe::_SliderI4a (const ScriptDynamicInt4Ptr &dyn,const String &name, const packed_int4 &min, const packed_int4 &max, const packed_int4 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

/*
=================================================
	_SliderU*
=================================================
*/
	void  ScriptExe::_SliderU0 (const ScriptDynamicUIntPtr &dyn, const String &name) __Th___
	{
		uint	min = 0, max = 1024;
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderU1 (const ScriptDynamicUIntPtr &dyn,const String &name, uint min, uint max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderU2 (const ScriptDynamicUInt2Ptr &dyn,const String &name, const packed_uint2 &min, const packed_uint2 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderU3 (const ScriptDynamicUInt3Ptr &dyn,const String &name, const packed_uint3 &min, const packed_uint3 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderU4 (const ScriptDynamicUInt4Ptr &dyn,const String &name, const packed_uint4 &min, const packed_uint4 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Int );
	}

	void  ScriptExe::_SliderU1a (const ScriptDynamicUIntPtr &dyn,const String &name, uint min, uint max, uint val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

	void  ScriptExe::_SliderU2a (const ScriptDynamicUInt2Ptr &dyn,const String &name, const packed_uint2 &min, const packed_uint2 &max, const packed_uint2 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

	void  ScriptExe::_SliderU3a (const ScriptDynamicUInt3Ptr &dyn,const String &name, const packed_uint3 &min, const packed_uint3 &max, const packed_uint3 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

	void  ScriptExe::_SliderU4a (const ScriptDynamicUInt4Ptr &dyn,const String &name, const packed_uint4 &min, const packed_uint4 &max, const packed_uint4 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Int );
	}

/*
=================================================
	_SliderF*
=================================================
*/
	void  ScriptExe::_SliderF0 (const ScriptDynamicFloatPtr &dyn, const String &name) __Th___
	{
		float min = 0.f, max = 1.f;
		return _Slider( dyn, name, min, max, min, ESlider::Float );
	}

	void  ScriptExe::_SliderF1 (const ScriptDynamicFloatPtr &dyn, const String &name, float min, float max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Float );
	}

	void  ScriptExe::_SliderF2 (const ScriptDynamicFloat2Ptr &dyn, const String &name, const packed_float2 &min, const packed_float2 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Float );
	}

	void  ScriptExe::_SliderF3 (const ScriptDynamicFloat3Ptr &dyn, const String &name, const packed_float3 &min, const packed_float3 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Float );
	}

	void  ScriptExe::_SliderF4 (const ScriptDynamicFloat4Ptr &dyn, const String &name, const packed_float4 &min, const packed_float4 &max) __Th___
	{
		return _Slider( dyn, name, min, max, min, ESlider::Float );
	}

	void  ScriptExe::_SliderF1a (const ScriptDynamicFloatPtr &dyn, const String &name, float min, float max, float val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Float );
	}

	void  ScriptExe::_SliderF2a (const ScriptDynamicFloat2Ptr &dyn, const String &name, const packed_float2 &min, const packed_float2 &max, const packed_float2 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Float );
	}

	void  ScriptExe::_SliderF3a (const ScriptDynamicFloat3Ptr &dyn, const String &name, const packed_float3 &min, const packed_float3 &max, const packed_float3 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Float );
	}

	void  ScriptExe::_SliderF4a (const ScriptDynamicFloat4Ptr &dyn, const String &name, const packed_float4 &min, const packed_float4 &max, const packed_float4 &val) __Th___
	{
		return _Slider( dyn, name, min, max, val, ESlider::Float );
	}

/*
=================================================
	_Label*
=================================================
*/
	void  ScriptExe::_LabelI1 (const ScriptDynamicIntPtr  &dyn, const String &name) __Th___		{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelI2 (const ScriptDynamicInt2Ptr &dyn, const String &name) __Th___		{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelI3 (const ScriptDynamicInt3Ptr &dyn, const String &name) __Th___		{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelI4 (const ScriptDynamicInt4Ptr &dyn, const String &name) __Th___		{ _Label( dyn, name, {} ); }

	void  ScriptExe::_LabelU1 (const ScriptDynamicUIntPtr  &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelU2 (const ScriptDynamicUInt2Ptr &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelU3 (const ScriptDynamicUInt3Ptr &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelU4 (const ScriptDynamicUInt4Ptr &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }

	void  ScriptExe::_LabelF1 (const ScriptDynamicFloatPtr  &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelF2 (const ScriptDynamicFloat2Ptr &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelF3 (const ScriptDynamicFloat3Ptr &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }
	void  ScriptExe::_LabelF4 (const ScriptDynamicFloat4Ptr &dyn, const String &name) __Th___	{ _Label( dyn, name, {} ); }

	void  ScriptExe::_LabelI1a (const ScriptDynamicIntPtr  &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelI2a (const ScriptDynamicInt2Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelI3a (const ScriptDynamicInt3Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelI4a (const ScriptDynamicInt4Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }

	void  ScriptExe::_LabelU1a (const ScriptDynamicUIntPtr  &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelU2a (const ScriptDynamicUInt2Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelU3a (const ScriptDynamicUInt3Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelU4a (const ScriptDynamicUInt4Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }

	void  ScriptExe::_LabelF1a (const ScriptDynamicFloatPtr  &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelF2a (const ScriptDynamicFloat2Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelF3a (const ScriptDynamicFloat3Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }
	void  ScriptExe::_LabelF4a (const ScriptDynamicFloat4Ptr &dyn, const String &name, const EnableLabel &enableIf)	__Th___	{ _Label( dyn, name, enableIf ); }

	template <typename D>
	void  ScriptExe::_Label (const D &dyn, const String &name, const EnableLabel &enableIf) __Th___
	{
		CHECK_THROW_MSG( dyn and dyn->Get() );

		auto&	data	= _GetTempData();
		auto&	dst		= data.labels.emplace_back();

		dst.dyn		= dyn->Get();
		dst.label	= name;

		if ( enableIf.dyn and enableIf.op != Default )
		{
			dst.ifDyn	= enableIf.dyn->Get();
			dst.op		= enableIf.op;
			dst.ref		= enableIf.ref;
		}
	}

/*
=================================================
	_EnableIf*
=================================================
*/
	ScriptExe::EnableLabel  ScriptExe::_EnableIfEqual   (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___	{ return EnableLabel{ dyn, ref, IPass::ECompare::Equal	 }; }
	ScriptExe::EnableLabel  ScriptExe::_EnableIfGreater (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___	{ return EnableLabel{ dyn, ref, IPass::ECompare::Greater }; }
	ScriptExe::EnableLabel  ScriptExe::_EnableIfLess    (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___	{ return EnableLabel{ dyn, ref, IPass::ECompare::Less	 }; }
	ScriptExe::EnableLabel  ScriptExe::_EnableIfAnyBit  (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___	{ return EnableLabel{ dyn, ref, IPass::ECompare::AnyBit	 }; }

/*
=================================================
	_ReadBuffer
=================================================
*/
	void  ScriptExe::_ReadBufferI1 (const ScriptDynamicIntPtr  &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Int32, 1 ); }
	void  ScriptExe::_ReadBufferI2 (const ScriptDynamicInt2Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Int32, 2 ); }
	void  ScriptExe::_ReadBufferI3 (const ScriptDynamicInt3Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Int32, 3 ); }
	void  ScriptExe::_ReadBufferI4 (const ScriptDynamicInt4Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Int32, 4 ); }

	void  ScriptExe::_ReadBufferU1 (const ScriptDynamicUIntPtr  &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::UInt32, 1 ); }
	void  ScriptExe::_ReadBufferU2 (const ScriptDynamicUInt2Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::UInt32, 2 ); }
	void  ScriptExe::_ReadBufferU3 (const ScriptDynamicUInt3Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::UInt32, 3 ); }
	void  ScriptExe::_ReadBufferU4 (const ScriptDynamicUInt4Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::UInt32, 4 ); }

	void  ScriptExe::_ReadBufferF1 (const ScriptDynamicFloatPtr  &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Float32, 1 ); }
	void  ScriptExe::_ReadBufferF2 (const ScriptDynamicFloat2Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Float32, 2 ); }
	void  ScriptExe::_ReadBufferF3 (const ScriptDynamicFloat3Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Float32, 3 ); }
	void  ScriptExe::_ReadBufferF4 (const ScriptDynamicFloat4Ptr &dyn, const ScriptBufferPtr &buf, const String &field) __Th___ { _ReadBuffer( dyn, buf, field, PipelineCompiler::EValueType::Float32, 4 ); }

	template <typename D, typename T>
	void  ScriptExe::_ReadBuffer (const D &dyn, const ScriptBufferPtr &buf, const String &fieldName, T type, uint count) __Th___
	{
		CHECK_THROW_MSG( buf );
		CHECK_THROW_MSG( dyn and dyn->Get() );

		buf->AddUsage( EResourceUsage::Transfer );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptReadBufferValue{ buf, fieldName, dyn->Get(), type, count }});
	}

/*
=================================================
	_NormalizeSpectrum
=================================================
*/
	void  ScriptExe::_NormalizeSpectrum (INOUT ScriptArray<packed_float4> &wlToRGB) __Th___
	{
		float4	scale = float4(0.f);
		for (uint i = 0; i < wlToRGB.size(); ++i) {
			scale += wlToRGB[i];
		}

		scale = float4(1.0) / scale;
		scale.x = 1.0;

		for (uint i = 0; i < wlToRGB.size(); ++i) {
			wlToRGB[i] *= scale;
		}
	}

/*
=================================================
	_NormalizeSpectrum
=================================================
*/
	void  ScriptExe::_SpectrumToLinear (INOUT ScriptArray<packed_float4> &wlToRGB) __Th___
	{
		for (uint i = 0; i < wlToRGB.size(); ++i)
		{
			wlToRGB[i].y = RemoveSRGBCurve( wlToRGB[i].y );
			wlToRGB[i].z = RemoveSRGBCurve( wlToRGB[i].z );
			wlToRGB[i].w = RemoveSRGBCurve( wlToRGB[i].w );
		}
	}

/*
=================================================
	_WhiteColorSpectrum3
=================================================
*/
	void  ScriptExe::_WhiteColorSpectrum3 (OUT ScriptArray<packed_float4> &wlToRGB) __Th___
	{
		wlToRGB.clear();
		wlToRGB.push_back( float4( 445.f,  0.f, 0.f, 1.f ));
		wlToRGB.push_back( float4( 535.f,  0.f, 1.f, 0.f ));
		wlToRGB.push_back( float4( 632.f,  1.f, 0.f, 0.f ));
	}

	void  ScriptExe::_WhiteColorSpectrum7 (OUT ScriptArray<packed_float4> &wlToRGB, bool normalized) __Th___
	{
		wlToRGB.clear();
		wlToRGB.push_back( float4( 390.f,  0.5f, 0.0f, 1.0f ));
		wlToRGB.push_back( float4( 445.f,  0.0f, 0.0f, 1.0f ));
		wlToRGB.push_back( float4( 490.f,  0.0f, 1.0f, 1.0f ));
		wlToRGB.push_back( float4( 535.f,  0.0f, 1.0f, 0.0f ));
		wlToRGB.push_back( float4( 583.f,  1.0f, 1.0f, 0.0f ));
		wlToRGB.push_back( float4( 635.f,  1.0f, 0.0f, 0.0f ));
		wlToRGB.push_back( float4( 720.f,  0.5f, 0.0f, 0.0f ));

		_SpectrumToLinear( INOUT wlToRGB );
		if ( normalized )
			_NormalizeSpectrum( INOUT wlToRGB );
	}

/*
=================================================
	_WhiteColorSpectrumStep50nm
=================================================
*/
	void  ScriptExe::_WhiteColorSpectrumStep50nm (OUT ScriptArray<packed_float4> &wlToRGB, bool normalized) __Th___
	{
		wlToRGB.clear();
		wlToRGB.push_back( float4( 400.f,  0.4f, 0.0f, 0.84f ));
		wlToRGB.push_back( float4( 450.f,  0.0f, 0.0f, 1.00f ));
		wlToRGB.push_back( float4( 500.f,  0.0f, 1.0f, 0.75f ));
		wlToRGB.push_back( float4( 550.f,  0.0f, 1.0f, 0.01f ));
		wlToRGB.push_back( float4( 600.f,  1.0f, 0.6f, 0.00f ));
		wlToRGB.push_back( float4( 650.f,  0.9f, 0.0f, 0.00f ));
		wlToRGB.push_back( float4( 700.f,  0.3f, 0.0f, 0.00f ));

		_SpectrumToLinear( INOUT wlToRGB );
		if ( normalized )
			_NormalizeSpectrum( INOUT wlToRGB );
	}

/*
=================================================
	_WhiteColorSpectrumStep100nm
=================================================
*/
	void  ScriptExe::_WhiteColorSpectrumStep100nm (OUT ScriptArray<packed_float4> &wlToRGB, bool normalized) __Th___
	{
		wlToRGB.clear();
	  #if 1
		wlToRGB.push_back( float4( 400.f,  0.30f, 0.0f, 1.0f ));
		wlToRGB.push_back( float4( 500.f,  0.00f, 1.0f, 0.8f ));
		wlToRGB.push_back( float4( 600.f,  0.90f, 0.8f, 0.0f ));
		wlToRGB.push_back( float4( 700.f,  0.60f, 0.0f, 0.0f ));
	  #else
		wlToRGB.push_back( float4( 450.f,  0.00f, 0.13f, 1.00f ));
		wlToRGB.push_back( float4( 550.f,  0.30f, 1.00f, 0.13f ));
		wlToRGB.push_back( float4( 650.f,  0.83f, 0.00f, 0.00f ));
	  #endif

		_SpectrumToLinear( INOUT wlToRGB );
		if ( normalized )
			_NormalizeSpectrum( INOUT wlToRGB );
	}

/*
=================================================
	_Supports_***
=================================================
*/
namespace {
	static bool  _IsDiscreteGPU ()
	{
		return GraphicsScheduler().GetDevice().AdapterType() == EGraphicsAdapterType::Discrete;
	}

	static EGPUVendor  _GetGPUVendor ()
	{
		return GraphicsScheduler().GetFeatureSet().vendorIds.include.First();
	}

	static bool  _IsRemoteGPU ()
	{
	#ifdef AE_ENABLE_REMOTE_GRAPHICS
		return true;
	#else
		return false;
	#endif
	}

	static bool  _Supports_GeometryShader ()
	{
		return GraphicsScheduler().GetFeatureSet().geometryShader == FeatureSet::EFeature::RequireTrue;
	}

	static bool  _Supports_MeshShader ()
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		return	fs.meshShader == FeatureSet::EFeature::RequireTrue	and
				fs.taskShader == FeatureSet::EFeature::RequireTrue;
	}

	static bool  _Supports_TessellationShader ()
	{
		return GraphicsScheduler().GetFeatureSet().tessellationShader == FeatureSet::EFeature::RequireTrue;
	}

	static bool  _Supports_SamplerAnisotropy ()
	{
		return GraphicsScheduler().GetFeatureSet().samplerAnisotropy == FeatureSet::EFeature::RequireTrue;
	}

	static bool  _Supports_AttachmentFormat (const EPixelFormat fmt)
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		return fs.attachmentFormats.contains( fmt );
	}
	
	static bool  _Supports_AttachmentBlendFormat (const EPixelFormat fmt)
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		return fs.attachmentBlendFormats.contains( fmt );
	}
	
	static bool  _Supports_LinearSampledFormat (const EPixelFormat fmt)
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		return fs.linearSampledFormats.contains( fmt );
	}
	
	static bool  _Supports_StorageImageFormat (const EPixelFormat fmt)
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		return fs.storageImageFormats.contains( fmt );
	}
	
	static bool  _Supports_StorageImageAtomicFormat (const EPixelFormat fmt)
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		return fs.storageImageAtomicFormats.contains( fmt );
	}

	static uint _GetSubgroupSize ()
	{
		return GraphicsScheduler().GetDevice().GetDeviceProperties().compute.subgroupSize;	
	}

	static ScriptFeatureSet*  _GetFeatureSet ()
	{
		ScriptFeatureSetPtr	tmp{ new ScriptFeatureSet{} };
		tmp->fs = GraphicsScheduler().GetFeatureSet();
		return tmp.Detach();
	}
}

/*
=================================================
	_Bind
=================================================
*/
	void  ScriptExe::_Bind (const ScriptEnginePtr &se, const Config &cfg) __Th___
	{
		using namespace AE::Graphics;

		CoreBindings::BindStdTypes( se );
		CoreBindings::BindScalarMath( se );
		CoreBindings::BindVectorMath( se );
		CoreBindings::BindQuaternion( se );
		CoreBindings::BindRect( se );
		CoreBindings::BindMatrixMath( se );
		CoreBindings::BindColor( se );
		CoreBindings::BindArray( se );
		CoreBindings::BindLog( se );
		CoreBindings::BindRandom( se );
		CoreBindings::BindToString( se, true, true, true, true );

		GraphicsBindings::BindEnums( se );
		GraphicsBindings::BindTypes( se );

		{
			EnumBinder<EColorSpace>	binder {se};
			binder.Create();
			binder.AddValue( "sRGB_nonlinear",			EColorSpace::sRGB_nonlinear );
			binder.AddValue( "BT709_nonlinear",			EColorSpace::BT709_nonlinear );
			binder.AddValue( "Extended_sRGB_linear",	EColorSpace::Extended_sRGB_linear );
			binder.AddValue( "HDR10_ST2084",			EColorSpace::HDR10_ST2084 );
			binder.AddValue( "BT2020_linear",			EColorSpace::BT2020_linear );
			StaticAssert( uint(EColorSpace::_Count) == 15 );
			StaticAssert( uint(ESurfaceFormat::_Count) == 11 );

			// not compatible with ESurfaceFormat
			//binder.AddValue( "Display_P3_nonlinear",		EColorSpace::Display_P3_nonlinear );
			//binder.AddValue( "Display_P3_linear",			EColorSpace::Display_P3_linear );
			//binder.AddValue( "DCI_P3_nonlinear",			EColorSpace::DCI_P3_nonlinear );
			//binder.AddValue( "BT709_linear",				EColorSpace::BT709_linear );
			//binder.AddValue( "DolbyVision",				EColorSpace::DolbyVision );
			//binder.AddValue( "HDR10_HLG",					EColorSpace::HDR10_HLG );
			//binder.AddValue( "AdobeRGB_linear",			EColorSpace::AdobeRGB_linear );
			//binder.AddValue( "AdobeRGB_nonlinear",		EColorSpace::AdobeRGB_nonlinear );
			//binder.AddValue( "PassThrough",				EColorSpace::PassThrough );
			//binder.AddValue( "Extended_sRGB_nonlinear",	EColorSpace::Extended_sRGB_nonlinear );
		}{
			EnumBinder<ERenderLayer>	binder{ se };
			binder.Create();
			binder.AddValue( "Opaque",		ERenderLayer::Opaque );
			binder.AddValue( "Translucent",	ERenderLayer::Translucent );
			binder.AddValue( "PostProcess",	ERenderLayer::PostProcess );
			StaticAssert( uint(ERenderLayer::_Count) == 3 );
		}

		_Bind_DbgViewFlags( se );
		_Bind_PassGroupFlags( se );
		ScriptDynamicUInt::Bind( se );
		ScriptDynamicUInt2::Bind( se );
		ScriptDynamicUInt3::Bind( se );
		ScriptDynamicUInt4::Bind( se );
		ScriptDynamicInt::Bind( se );
		ScriptDynamicInt2::Bind( se );
		ScriptDynamicInt3::Bind( se );
		ScriptDynamicInt4::Bind( se );
		ScriptDynamicFloat::Bind( se );
		ScriptDynamicFloat2::Bind( se );
		ScriptDynamicFloat3::Bind( se );
		ScriptDynamicFloat4::Bind( se );
		ScriptDynamicULong::Bind( se );
		ScriptDynamicDim::Bind( se );

		ScriptImage::Bind( se );
		ScriptVideoImage::Bind( se );
		ScriptBuffer::Bind( se );
		ScriptRTGeometry::Bind( se );
		ScriptRTScene::Bind( se );

		ScriptBaseController::Bind( se );
		ScriptControllerScaleBias::Bind( se );
		ScriptControllerTopDown::Bind( se );
		ScriptControllerOrbitalCamera::Bind( se );
		ScriptControllerFlightCamera::Bind( se );
		ScriptControllerFPVCamera::Bind( se );
		ScriptControllerRemoteCamera::Bind( se );
		ScriptControllerFreeCamera::Bind( se );

		ScriptBasePass::Bind( se );
		ScriptGeomSource::Bind( se );
		ScriptSphericalCube::Bind( se );
		ScriptUniGeometry::Bind( se );
		ScriptModelGeometrySrc::Bind( se );

		ScriptCollection::Bind( se );

		// don't forget to update '_SaveCppStructs()'
		ScriptPostprocess::Bind( se );
		ScriptComputePass::Bind( se );
		ScriptComputeMip::Bind( se );
		ScriptRasterMip::Bind( se );
		ScriptRayTracingPass::Bind( se );
		ScriptSceneGraphicsPass::Bind( se );
		ScriptSceneRayTracingPass::Bind( se );
		ScriptScene::Bind( se );

		PipelineCompiler::ScriptFeatureSet::Bind( se );

		{
			Scripting::ClassBinder<EnableLabel>	binder{ se };
			binder.CreateClassValue();
		}

		AS_GLOBAL_FN( se, ScriptExe::_SurfaceSize,				"SurfaceSize",				{},		"Returns dynamic dimensions of the screen surface."	);

		AS_GLOBAL_FN( se, ScriptExe::_Present1,					"Present",					{},		"Present image to the screen." );
		AS_GLOBAL_FN( se, ScriptExe::_Present2,					"Present",					{} );
		AS_GLOBAL_FN( se, ScriptExe::_Present3,					"Present",					{} );
		AS_GLOBAL_FN( se, ScriptExe::_Present4,					"Present",					{} );
		AS_GLOBAL_FN( se, ScriptExe::_Present5,					"Present",					{} );
		AS_GLOBAL_FN( se, ScriptExe::_Present6,					"Present",					{} );

		AS_GLOBAL_FN( se, ScriptExe::_DbgView1,					"DbgView",					{},		"Draw image in child window for debugging." );
		AS_GLOBAL_FN( se, ScriptExe::_DbgView2,					"DbgView",					{} );
		AS_GLOBAL_FN( se, ScriptExe::_DbgView3,					"DbgView",					{} );
		AS_GLOBAL_FN( se, ScriptExe::_DbgView4,					"DbgView",					{} );

		AS_GLOBAL_FN( se, ScriptExe::_GenMipmaps,				"GenMipmaps",				{},		"Pass which generates mipmaps for image." );

		AS_GLOBAL_FN( se, ScriptExe::_CopyImage,				"CopyImage",				{"src", "dst"},	"Pass which copy image content to another image." );
		AS_GLOBAL_FN( se, ScriptExe::_CopyImage2,				"CopyImage2",				{"src", "dst"},	"Pass which copy image content to another image without format restrictions." );
		AS_GLOBAL_FN( se, ScriptExe::_BlitImage,				"BlitImage",				{"src", "dst"},	"Pass which blits image to another image." );
		AS_GLOBAL_FN( se, ScriptExe::_ResolveImage,				"ResolveImage",				{"src", "dst"},	"Pass which resolve multisample image to another single-sampled image." );
		AS_GLOBAL_FN( se, ScriptExe::_CompressImage,			"CompressImage",			{"src", "dst"},	"Pass which compress image on CPU or GPU." );
		AS_GLOBAL_FN( se, ScriptExe::_CompressImage2,			"CompressImage",			{"src", "dst", "dstFormat"}, "Pass which compress image on CPU or GPU.\n'dstFormat' may not be supported by current GPU, but may be used for software decoding.\n'dst' image must be compatible with 'dstFormat'." );

		AS_GLOBAL_FN( se, ScriptExe::_ClearImage1,				"ClearImage",				{},		"Pass to clear float-color image." );
		AS_GLOBAL_FN( se, ScriptExe::_ClearImage2,				"ClearImage",				{},		"Pass to clear uint-color image." );
		AS_GLOBAL_FN( se, ScriptExe::_ClearImage3,				"ClearImage",				{},		"Pass to clear int-color image." );
		AS_GLOBAL_FN( se, ScriptExe::_ClearBuffer1,				"ClearBuffer",				{},		"Pass to clear buffer." );
		AS_GLOBAL_FN( se, ScriptExe::_ClearBuffer2,				"ClearBuffer",				{"buffer", "offset", "size", "value"} );

		AS_GLOBAL_FN( se, ScriptExe::_ExportImage,				"Export",					{"image", "prefix"},	"Readback the image and save it to a file in DDS format. Rendering will be paused until the readback is completed." );
		AS_GLOBAL_FN( se, ScriptExe::_DbgExportBuffer,			"DbgExport",				{"buffer", "prefix"},	"Readback the buffer and save it to a file in structured format. Rendering will be paused until the readback is completed." );
		AS_GLOBAL_FN( se, ScriptExe::_ExportBuffer,				"Export",					{"buffer", "prefix"},	"Readback the buffer and save it to a file in binary format. Rendering will be paused until the readback is completed." );
	//	AS_GLOBAL_FN( se, ScriptExe::_ExportGeometry,			"Export",					{"geometry", "prefix"},	"Readback the geometry data (images, buffers, etc) and save it to a file in glTF format. Rendering will be paused until the readback is completed." );

		AS_GLOBAL_FN( se, ScriptExe::_BuildRTGeometry,			"BuildRTGeometry",			{},		"Pass to build RTGeometry, executed every frame."			);
		AS_GLOBAL_FN( se, ScriptExe::_BuildRTGeometryIndirect,	"BuildRTGeometryIndirect",	{},		"Pass to indirect build RTGeometry, executed every frame."	);

		AS_GLOBAL_FN( se, ScriptExe::_BuildRTScene,				"BuildRTScene",				{},		"Pass to build RTScene, executed every frame."				);
		AS_GLOBAL_FN( se, ScriptExe::_BuildRTSceneIndirect,		"BuildRTSceneIndirect",		{},		"Pass to indirect build RTScene, executed every frame."		);

		AS_GLOBAL_FN( se, ScriptExe::_GetCube2,					"GetCube",					{"positions", "normals", "indices"} );
		AS_GLOBAL_FN( se, ScriptExe::_GetCube3,					"GetCube",					{"positions", "normals", "tangents", "bitangents", "texcoords2d", "indices"} );
		AS_GLOBAL_FN( se, ScriptExe::_GetCube4,					"GetCube",					{"positions", "normals", "tangents", "bitangents", "cubemapTexcoords", "indices"} );
		AS_GLOBAL_FN( se, ScriptExe::_GetGrid1,					"GetGrid",					{"size", "unorm2Positions", "indices"},					"Returns (size * size) grid" );
		AS_GLOBAL_FN( se, ScriptExe::_GetGrid2,					"GetGrid",					{"size", "unorm3Positions", "indices"},					"Returns (size * size) grid in XY space." );
		AS_GLOBAL_FN( se, ScriptExe::_GetSphere1,				"GetSphere",				{"lod", "positions", "indices"},						"Returns spherical cube" );
		AS_GLOBAL_FN( se, ScriptExe::_GetSphere5,				"GetSphere",				{"lod", "positions", "texcoords2d", "indices"},			"Returns spherical cube with 2D UV" );
		AS_GLOBAL_FN( se, ScriptExe::_GetSphere2,				"GetSphere",				{"lod", "positions", "cubemapTexcoords", "indices"},	"Returns spherical cube" );
		AS_GLOBAL_FN( se, ScriptExe::_GetSphere3,				"GetSphere",				{"lod", "positions", "normals", "tangents", "bitangents", "cubemapTexcoords", "indices"},	"Returns spherical cube with tangential projection for cubemap." );
		AS_GLOBAL_FN( se, ScriptExe::_GetSphere4,				"GetSphere",				{"lod", "positions", "normals", "tangents", "bitangents", "texcoords2d", "indices"},		"Returns spherical cube" );
		AS_GLOBAL_FN( se, ScriptExe::_GetCylinder1,				"GetCylinder",				{"segmentCount", "isInner", "positions", "texcoords", "indices"},			"Returns cylinder" );
		AS_GLOBAL_FN( se, ScriptExe::_GetCylinder2,				"GetCylinder",				{"segmentCount", "isInner", "positions", "normals", "tangents", "bitangents", "texcoords", "indices"},	"Returns cylinder" );

		AS_GLOBAL_FN( se, ScriptExe::_GetSphericalCube1,		"GetSphericalCube",			{"lod", "positions", "indices"},						"Returns spherical cube without projection and face rotation.\nIn 'positions': xy - pos on face, z - face index." );

		AS_GLOBAL_FN( se, ScriptExe::_IndicesToPrimitives,		"IndicesToPrimitives",		{"indices", "primitives"},		"Helper function to convert array of indices to array of uint3 indices per triangle" );
		AS_GLOBAL_FN( se, ScriptExe::_GetFrustumPlanes,			"GetFrustumPlanes",			{"viewProj", "outPlanes"},		"Helper function to convert matrix to 6 planes of the frustum." );
		AS_GLOBAL_FN( se, ScriptExe::_MergeMesh,				"MergeMesh",				{"srcIndices", "srcVertexCount", "indicesToAdd"} );

		#ifdef AE_ENABLE_CDT
		AS_GLOBAL_FN( se, ScriptExe::_ExtrudeAndMerge,			"Extrude",					{"lineStrip", "height", "positions", "indices"},					"Output is a TriangleList, front face: CCW" );
		AS_GLOBAL_FN( se, ScriptExe::_TriangulateAndMerge1,		"Triangulate",				{"lineStrip", "yCoord", "positions", "indices"},					"Output is a TriangleList, front face: CCW" );
		AS_GLOBAL_FN( se, ScriptExe::_TriangulateAndMerge2,		"Triangulate",				{"vertices", "lineListIndices", "yCoord", "positions", "indices"},	"Output is a TriangleList, front face: CCW" );
		AS_GLOBAL_FN( se, ScriptExe::_TriangulateExtrudeAndMerge1,"TriangulateAndExtrude",	{"lineStrip", "height", "positions", "indices"},					"Output is a TriangleList, front face: CCW" );
		AS_GLOBAL_FN( se, ScriptExe::_TriangulateExtrudeAndMerge2,"TriangulateAndExtrude",	{"vertices", "lineListIndices", "height", "positions", "indices"},	"Output is a TriangleList, front face: CCW" );
		#endif

		AS_GLOBAL_FN( se, ScriptExe::_RunScript1,				"RunScript",				{"filePath", "collection"},		"Run script, path to script must be added to 'res_editor_cfg.as' as 'SecondaryScriptDir()'" );
		AS_GLOBAL_FN( se, ScriptExe::_RunScript2,				"RunScript",				{"filePath", "flags", "collection"} );

		AS_GLOBAL_FN( se, ScriptExe::_SliderI0,					"Slider",					{"dyn", "name"},				"Add slider to UI." );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI1,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI2,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI3,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI4,					"Slider",					{"dyn", "name", "min", "max"} );

		AS_GLOBAL_FN( se, ScriptExe::_SliderI1a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI2a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI3a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderI4a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );

		AS_GLOBAL_FN( se, ScriptExe::_SliderU0,					"Slider",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU1,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU2,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU3,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU4,					"Slider",					{"dyn", "name", "min", "max"} );

		AS_GLOBAL_FN( se, ScriptExe::_SliderU1a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU2a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU3a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderU4a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );

		AS_GLOBAL_FN( se, ScriptExe::_SliderF0,					"Slider",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF1,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF2,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF3,					"Slider",					{"dyn", "name", "min", "max"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF4,					"Slider",					{"dyn", "name", "min", "max"} );

		AS_GLOBAL_FN( se, ScriptExe::_SliderF1a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF2a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF3a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		AS_GLOBAL_FN( se, ScriptExe::_SliderF4a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );

		AS_GLOBAL_FN( se, ScriptExe::_LabelI1,					"Label",					{"dyn", "name"},	"Add label to UI." );
		AS_GLOBAL_FN( se, ScriptExe::_LabelI2,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelI3,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelI4,					"Label",					{"dyn", "name"} );

		AS_GLOBAL_FN( se, ScriptExe::_LabelU1,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelU2,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelU3,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelU4,					"Label",					{"dyn", "name"} );

		AS_GLOBAL_FN( se, ScriptExe::_LabelF1,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelF2,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelF3,					"Label",					{"dyn", "name"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelF4,					"Label",					{"dyn", "name"} );

		AS_GLOBAL_FN( se, ScriptExe::_LabelI1a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelI2a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelI3a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelI4a,					"Label",					{"dyn", "name", "enableIf"} );

		AS_GLOBAL_FN( se, ScriptExe::_LabelU1a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelU2a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelU3a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelU4a,					"Label",					{"dyn", "name", "enableIf"} );

		AS_GLOBAL_FN( se, ScriptExe::_LabelF1a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelF2a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelF3a,					"Label",					{"dyn", "name", "enableIf"} );
		AS_GLOBAL_FN( se, ScriptExe::_LabelF4a,					"Label",					{"dyn", "name", "enableIf"} );

		AS_GLOBAL_FN( se, ScriptExe::_EnableIfEqual,			"EnableIfEqual",			{"dyn", "ref"} );
		AS_GLOBAL_FN( se, ScriptExe::_EnableIfGreater,			"EnableIfGreater",			{"dyn", "ref"} );
		AS_GLOBAL_FN( se, ScriptExe::_EnableIfLess,				"EnableIfLess",				{"dyn", "ref"} );
		AS_GLOBAL_FN( se, ScriptExe::_EnableIfAnyBit,			"EnableIfAnyBit",			{"dyn", "ref"} );

		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferI1,				"ReadBuffer",				{"dyn", "buffer", "field"},		"Read field from buffer and copy to dynamic variable." );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferI2,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferI3,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferI4,				"ReadBuffer",				{"dyn", "buffer", "field"} );

		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferU1,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferU2,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferU3,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferU4,				"ReadBuffer",				{"dyn", "buffer", "field"} );

		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferF1,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferF2,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferF3,				"ReadBuffer",				{"dyn", "buffer", "field"} );
		AS_GLOBAL_FN( se, ScriptExe::_ReadBufferF4,				"ReadBuffer",				{"dyn", "buffer", "field"} );

		AS_GLOBAL_FN( se, ScriptExe::_WhiteColorSpectrum3,			"WhiteColorSpectrum3",			{"wavelengthToRGB"},				"Returns array with 3 elements, where x - wavelength in nm, yzw - RGB color in linear space." );
		AS_GLOBAL_FN( se, ScriptExe::_WhiteColorSpectrum7,			"WhiteColorSpectrum7",			{"wavelengthToRGB", "normalized"},	"Returns array with 7 elements, where x - wavelength in nm, yzw - RGB color in linear space.\nnormalized - sum of colors will be 1." );
		AS_GLOBAL_FN( se, ScriptExe::_WhiteColorSpectrumStep100nm,	"WhiteColorSpectrumStep100nm",	{"wavelengthToRGB", "normalized"},	"Returns array 4 elements with visible light spectrum with step 100nm, where x - wavelength in nm, yzw - RGB color in linear space.\nnormalized - sum of colors will be 1." );
		AS_GLOBAL_FN( se, ScriptExe::_WhiteColorSpectrumStep50nm,	"WhiteColorSpectrumStep50nm",	{"wavelengthToRGB", "normalized"},	"Returns array 7 elements with visible light spectrum with step 50nm, where x - wavelength in nm, yzw - RGB color in linear space.\nnormalized - sum of colors will be 1." );

		AS_GLOBAL_FN( se, ScriptExe::_CM_CubeSC_Forward,		"CM_CubeSC_Forward",		{"snormCoord_cubeFace"},	"Convert 2D regular grid on cube face to 3D position on cube." );
		AS_GLOBAL_FN( se, ScriptExe::_CM_IdentitySC_Forward,	"CM_IdentitySC_Forward",	{"snormCoord_cubeFace"},	"Convert 2D regular grid on cube face to 3D position on sphere using identity projection (normalization)." );
		AS_GLOBAL_FN( se, ScriptExe::_CM_TangentialSC_Forward,	"CM_TangentialSC_Forward",	{"snormCoord_cubeFace"},	"Convert 2D regular grid on cube face to 3D position on sphere using tangential projection." );
		
		AS_GLOBAL_FN( se, _GetGPUVendor,									"GPUVendor",						{} );
		AS_GLOBAL_FN( se, _IsDiscreteGPU,									"IsDiscreteGPU",					{} );
		AS_GLOBAL_FN( se, _IsRemoteGPU,										"IsRemoteGPU",						{} );
		AS_GLOBAL_FN( se, _Supports_GeometryShader,							"Supports_GeometryShader",			{} );
		AS_GLOBAL_FN( se, _Supports_MeshShader,								"Supports_MeshShader",				{} );
		AS_GLOBAL_FN( se, _Supports_TessellationShader,						"Supports_TessellationShader",		{} );
		AS_GLOBAL_FN( se, _Supports_SamplerAnisotropy,						"Supports_SamplerAnisotropy",		{} );
		AS_GLOBAL_FN( se, ScriptResourceApi::Supported_DepthFormat,			"Supported_DepthFormat",			{} );
		AS_GLOBAL_FN( se, ScriptResourceApi::Supported_DepthStencilFormat,	"Supported_DepthStencilFormat",		{} );
		AS_GLOBAL_FN( se, _Supports_AttachmentFormat,						"Supports_AttachmentFormat",		{} );
		AS_GLOBAL_FN( se, _Supports_AttachmentBlendFormat,					"Supports_AttachmentBlendFormat",	{} );
		AS_GLOBAL_FN( se, _Supports_LinearSampledFormat,					"Supports_LinearSampledFormat",		{} );
		AS_GLOBAL_FN( se, _Supports_StorageImageFormat,						"Supports_StorageImageFormat",		{} );
		AS_GLOBAL_FN( se, _Supports_StorageImageAtomicFormat,				"Supports_StorageImageAtomicFormat",{} );
		AS_GLOBAL_FN( se, _GetSubgroupSize,									"GetSubgroupSize",					{} );
		AS_GLOBAL_FN( se, _GetFeatureSet,									"GetFeatureSet",					{} );

		// TODO:
		//	PresentVR( left, left_layer, left_mipmap,  right, right_layer, right_mipmap )

		if ( FileSystem::IsDirectory( cfg.scriptHeaderOutFolder ))
		{
			se->AddCppHeader( "", "#define SCRIPT\n\n", 0 );

			CHECK( se->SaveCppHeader( cfg.scriptHeaderOutFolder / "res_editor.as" ));
		}
	}

/*
=================================================
	_Bind_DbgViewFlags
=================================================
*/
	void  ScriptExe::_Bind_DbgViewFlags (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<DebugView::EFlags>	binder {se};
		binder.Create();
		switch_enum( DebugView::EFlags::_Count )
		{
			case DebugView::EFlags::_Count :
			#define CASE( _name_ )	case DebugView::EFlags::_name_ : binder.AddValue( #_name_, DebugView::EFlags::_name_ );
			CASE( NoCopy )
			CASE( Copy )
			CASE( Histogram )
			CASE( LinearDepth )
			CASE( Stencil )
			#undef CASE
			default : break;
		}
		switch_end
	}

/*
=================================================
	_Bind_PassGroupFlags
=================================================
*/
	void  ScriptExe::_Bind_PassGroupFlags (const ScriptEnginePtr &se) __Th___
	{
		EnumBinder<PassGroup::EFlags>	binder {se};
		binder.Create();
		binder.AddValue( "RunOnce",					PassGroup::EFlags::RunOnce );
		binder.AddValue( "OnRequest",				PassGroup::EFlags::OnRequest );
		binder.AddValue( "RunOnce_AfterLoading",	PassGroup::EFlags::RunOnce_AfterLoading );
		StaticAssert( uint(PassGroup::EFlags::_Count) == 4 );
	}

/*
=================================================
	_Bind_Constants
=================================================
*/
	void  ScriptExe::_Bind_Constants (const ScriptEnginePtr &se) __Th___
	{
		se->AddConstProperty( _sampConsts->NearestClamp,				"Sampler_" + _sampConsts->NearestClamp );
		se->AddConstProperty( _sampConsts->NearestRepeat,				"Sampler_" + _sampConsts->NearestRepeat );
		se->AddConstProperty( _sampConsts->NearestMirrorRepeat,			"Sampler_" + _sampConsts->NearestMirrorRepeat );
		se->AddConstProperty( _sampConsts->LinearClamp,					"Sampler_" + _sampConsts->LinearClamp );
		se->AddConstProperty( _sampConsts->LinearRepeat,				"Sampler_" + _sampConsts->LinearRepeat );
		se->AddConstProperty( _sampConsts->LinearMirrorRepeat,			"Sampler_" + _sampConsts->LinearMirrorRepeat );
		se->AddConstProperty( _sampConsts->LinearMipmapClamp,			"Sampler_" + _sampConsts->LinearMipmapClamp );
		se->AddConstProperty( _sampConsts->LinearMipmapRepeat,			"Sampler_" + _sampConsts->LinearMipmapRepeat );
		se->AddConstProperty( _sampConsts->LinearMipmapMirrorRepeat,	"Sampler_" + _sampConsts->LinearMipmapMirrorRepeat );
		se->AddConstProperty( _sampConsts->LinearMipmapMirrorClamp,		"Sampler_" + _sampConsts->LinearMipmapMirrorClamp );
		se->AddConstProperty( _sampConsts->Anisotropy8Repeat,			"Sampler_" + _sampConsts->Anisotropy8Repeat );
		se->AddConstProperty( _sampConsts->Anisotropy8MirrorRepeat,		"Sampler_" + _sampConsts->Anisotropy8MirrorRepeat );
		se->AddConstProperty( _sampConsts->Anisotropy8Clamp,			"Sampler_" + _sampConsts->Anisotropy8Clamp );
		se->AddConstProperty( _sampConsts->Anisotropy16Repeat,			"Sampler_" + _sampConsts->Anisotropy16Repeat );
		se->AddConstProperty( _sampConsts->Anisotropy16MirrorRepeat,	"Sampler_" + _sampConsts->Anisotropy16MirrorRepeat );
		se->AddConstProperty( _sampConsts->Anisotropy16Clamp,			"Sampler_" + _sampConsts->Anisotropy16Clamp );
		se->AddConstProperty( _sampConsts->NearestClampSubsampled,		"Sampler_" + _sampConsts->NearestClampSubsampled );
		se->AddConstProperty( _sampConsts->MaxLinearClamp,				"Sampler_" + _sampConsts->MaxLinearClamp );
		se->AddConstProperty( _sampConsts->MinLinearClamp,				"Sampler_" + _sampConsts->MinLinearClamp );

		StaticAssert( (sizeof(SamplerConsts) / sizeof(String)) == 19 );
	}

/*
=================================================
	_Bind_Enums
=================================================
*/
	void  ScriptExe::_Bind_Enums (const ScriptEnginePtr &se) __Th___
	{
		{
			EnumBinder<EAttachmentLoadOp>	binder{ se };
			binder.Create();
			binder.AddValue( "Invalidate",	EAttachmentLoadOp::Invalidate );
			binder.AddValue( "Load",		EAttachmentLoadOp::Load );
			binder.AddValue( "Clear",		EAttachmentLoadOp::Clear );
			binder.AddValue( "None",		EAttachmentLoadOp::None );
			StaticAssert( uint(EAttachmentLoadOp::_Count) == 4 );
		}{
			EnumBinder<EAttachmentStoreOp>	binder{ se };
			binder.Create();
			binder.AddValue( "Invalidate",	EAttachmentStoreOp::Invalidate );
			binder.AddValue( "Store",		EAttachmentStoreOp::Store );
			binder.AddValue( "None",		EAttachmentStoreOp::None );
			StaticAssert( uint(EAttachmentStoreOp::_Count) == 4 );
		}
	}

/*
=================================================
	_SaveCppStructs
=================================================
*/
	void  ScriptExe::_SaveCppStructs (const Path &fname) __Th___
	{
		ScriptBasePass::CppStructsFromShaders	data;

		_GetSharedShaderTypes( INOUT data );

		// don't forget to update '_Bind()'
		ScriptPostprocess::GetShaderTypes( INOUT data );
		ScriptComputePass::GetShaderTypes( INOUT data );
		ScriptComputeMip::GetShaderTypes( INOUT data );
		ScriptRasterMip::GetShaderTypes( INOUT data );
		ScriptRayTracingPass::GetShaderTypes( INOUT data );
		ScriptSceneGraphicsPass::GetShaderTypes( INOUT data );
		ScriptSceneRayTracingPass::GetShaderTypes( INOUT data );

		ScriptSphericalCube::GetShaderTypes( INOUT data );
		ScriptUniGeometry::GetShaderTypes( INOUT data );
		ScriptModelGeometrySrc::GetShaderTypes( INOUT data );

		if ( data.cpp.empty() )
			return;

		HashVal32	prev_hash;

		if ( FileSystem::IsFile( fname ))
		{
			FileRStream	file {fname};
			if ( file.IsOpen() )
			{
				char	hash_str [2+8+1] = {};
				if ( file.Read( OUT hash_str, Sizeof(hash_str) ))
				{
					ASSERT( hash_str[0] == '/' );
					ASSERT( hash_str[1] == '/' );
					ASSERT( hash_str[10] == '\n' );
					prev_hash = HashVal32{StringToUInt( StringView{hash_str}.substr( 2 ), 16 )};
				}
			}
		}

		const HashVal32	hash = CT_Hash( data.cpp.data(), data.cpp.length(), 0 );
		if ( hash != prev_hash )
		{
			FileWStream		file {fname};
			CHECK_ERRV( file.IsOpen() );
			CHECK_ERRV( file.Write( "//"s << ToString<16>(uint{hash}) << "\n" ));
			CHECK_ERRV( file.Write( data.cpp ));
		}
	}

/*
=================================================
	_GetRenderer
=================================================
*/
	Renderer*  ScriptExe::_GetRenderer () __Th___
	{
		return _GetTempData().renderer.get();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ConvertAndLoad
=================================================
*/
	RTechInfo  ScriptExe::ScriptPassApi::ConvertAndLoad (Function<void (ScriptEnginePtr)> fn, ScriptBasePass::EFlags passFlags) __Th___
	{
		CHECK_THROW( s_scriptExe != null );

		const Bool	use_slang = Bool{AllBits( passFlags, ScriptBasePass::EFlags::UseSLang )};
		
	  #ifdef AE_METAL_TOOLS
		try{
			const auto	flags = UIInteraction::Instance().graphics->shaderFlags;
			if ( flags.contains( UIInteraction::EShaderFlags::CompileMSL ))
			{
				s_scriptExe->_RunWithPipelineCompiler(
					[&] ()
					{
						ScriptBasePass::CppStructsFromShaders	data;
						ScriptSphericalCube::GetShaderTypes( INOUT data );
						ScriptUniGeometry::GetShaderTypes( INOUT data );
						ScriptModelGeometrySrc::GetShaderTypes( INOUT data );

						fn( s_scriptExe->_engine2 );
					},
					True{"compileMSL"},
					use_slang );
			}
		}catch(...){
			AE_LOGE( "failed to compile for Metal API" );
		}
	  #endif

		RTechInfo	result;
		s_scriptExe->_RunWithPipelineCompiler(
			[&] ()
			{
				ScriptBasePass::CppStructsFromShaders	data;
				ScriptSphericalCube::GetShaderTypes( INOUT data );
				ScriptUniGeometry::GetShaderTypes( INOUT data );
				ScriptModelGeometrySrc::GetShaderTypes( INOUT data );

				fn( s_scriptExe->_engine2 );
				result = s_scriptExe->_ConvertAndLoad();
			},
			False{"don't compile MSL"},
			use_slang );

		return result;
	}

/*
=================================================
	AddPass
=================================================
*/
	void  ScriptExe::ScriptPassApi::AddPass (ScriptBasePassPtr pass) __Th___
	{
		CHECK_THROW( pass );

		auto&	data = _GetTempData();
		CHECK_THROW( data.passGroup );

		data.passGroup->Add( pass );
	}

/*
=================================================
	GetCurrentFile
=================================================
*/
	Path  ScriptExe::ScriptPassApi::GetCurrentFile () __Th___
	{
		auto&	data = _GetTempData();
		CHECK_THROW( not data.currPath.empty() );

		return data.currPath.back();
	}

	Path  ScriptExe::ScriptPassApi::ToAbsolute (const Path &path) __Th___
	{
		auto	curr = GetCurrentFile().parent_path();
		curr /= path;

		if ( FileSystem::IsFile( curr ))
			return curr;

		curr = FileSystem::CurrentPath();
		curr /= path;

		if ( FileSystem::IsFile( curr ))
			return curr;

		CHECK_THROW_MSG( false, "can't find file '"s << ToString(path) << "'");
	}

/*
=================================================
	ToShaderPath
=================================================
*/
	Path  ScriptExe::ScriptPassApi::ToShaderPath (const Path &path) __Th___
	{
		auto&	data = _GetTempData();

		// shader source in the same file
		if ( path.empty() )
		{
			CHECK_THROW( not data.currPath.empty() );
			return data.currPath.back();
		}

		for (const auto& p : data.cfg.shaderDirs)
		{
			Path	pp = p / path;

			if ( FileSystem::IsFile( pp ))
			{
				data.dependencies.push_back( pp );
				return pp;
			}
		}

		if ( FileSystem::IsFile( path ))
		{
			Path	pp = ToAbsolute( path );
			data.dependencies.push_back( pp );
			return RVRef(pp);
		}

		return {};
	}

/*
=================================================
	ToPipelinePath
=================================================
*/
	Path  ScriptExe::ScriptPassApi::ToPipelinePath (const Path &path) __Th___
	{
		CHECK_THROW( not path.empty() );

		auto&	data = _GetTempData();
		for (const auto& p : data.cfg.pipelineDirs)
		{
			Path	pp = p / path;

			if ( FileSystem::IsFile( pp ))
				return pp;
		}
		CHECK_THROW_MSG( false,
			"Can't find pipeline '"s << ToString(path) << "'" );
	}

/*
=================================================
	ToPipelineFolder
=================================================
*/
	Path  ScriptExe::ScriptPassApi::ToPipelineFolder (const Path &path) __Th___
	{
		CHECK_THROW( not path.empty() );

		auto&	data = _GetTempData();
		for (const auto& p : data.cfg.pipelineDirs)
		{
			Path	pp = p / path;

			if ( FileSystem::IsDirectory( pp ))
				return pp;
		}
		CHECK_THROW( false );
	}

/*
=================================================
	GetRenderer
=================================================
*/
	Renderer&  ScriptExe::ScriptPassApi::GetRenderer () __Th___
	{
		CHECK_THROW( s_scriptExe != null );
		auto*	result = s_scriptExe->_GetRenderer();
		CHECK_THROW( result != null );
		return *result;
	}

/*
=================================================
	GetPipelineIncludeDirs
=================================================
*/
	ArrayView<Path>  ScriptExe::ScriptPassApi::GetPipelineIncludeDirs () __NE___
	{
		return ResEditorAppConfig::Get().pipelineIncludeDirs;
	}

/*
=================================================
	GetMonitor
=================================================
*/
	App::Monitor const&  ScriptExe::ScriptPassApi::GetMonitor () __Th___
	{
		CHECK_THROW( s_scriptExe != null );
		CHECK_THROW( s_scriptExe->_tempData );
		return s_scriptExe->_tempData->cfg.monitor;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetRenderer
=================================================
*/
	Renderer&  ScriptExe::ScriptResourceApi::GetRenderer () __Th___
	{
		CHECK_THROW( s_scriptExe != null );
		auto*	result = s_scriptExe->_GetRenderer();
		CHECK_THROW( result != null );
		return *result;
	}

/*
=================================================
	IsPassGroup
=================================================
*/
	bool  ScriptExe::ScriptResourceApi::IsPassGroup (const ScriptBasePassPtr &pass) __NE___
	{
		return DynCast<ScriptPassGroup>( pass.Get() ) != null;
	}

/*
=================================================
	ToAbsolute
=================================================
*/
	Path  ScriptExe::ScriptResourceApi::ToAbsolute (const String &inPath) __Th___
	{
		CHECK_THROW( s_scriptExe != null );

		auto&	cfg	= s_scriptExe->_config;

		for (auto& [folder, prefix] : cfg.vfsPaths)
		{
			if ( StartsWith( inPath, prefix ))
			{
				Path	path = folder / inPath.substr( prefix.size() );
				if ( FileSystem::IsFile( path ))
					return FileSystem::ToAbsolute( path );
			}
		}

		CHECK_THROW_MSG( false,
			"File '"s << inPath << "' is not exists" );
	}

/*
=================================================
	GetFeatureSet
=================================================
*/
	Graphics::FeatureSet const&  ScriptExe::ScriptResourceApi::GetFeatureSet () __NE___
	{
		return GraphicsScheduler().GetFeatureSet();
	}

/*
=================================================
	GetFeatureSet
=================================================
*/
	EPixelFormat  ScriptExe::ScriptResourceApi::Supported_DepthFormat ()
	{
		auto&	fs = GetFeatureSet();
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth32F ))	return EPixelFormat::Depth32F;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth24 ))	return EPixelFormat::Depth24;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth16 ))	return EPixelFormat::Depth16;
		return Default;
	}

/*
=================================================
	GetFeatureSet
=================================================
*/
	EPixelFormat  ScriptExe::ScriptResourceApi::Supported_DepthStencilFormat ()
	{
		auto&	fs = GetFeatureSet();
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth32F_Stencil8 ))	return EPixelFormat::Depth32F_Stencil8;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth24_Stencil8 ))	return EPixelFormat::Depth24_Stencil8;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth16_Stencil8 ))	return EPixelFormat::Depth16_Stencil8;
		return Default;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_RunWithPipelineCompiler
=================================================
*/
	void  ScriptExe::_RunWithPipelineCompiler (Function<void ()> fn, const Bool compileMSL, const Bool useSlang) __Th___
	{
		try
		{
			// init pipeline compiler
			ObjectStorage	obj_storage;
			PipelineStorage	ppln_storage;
			{
				obj_storage.pplnStorage			= &ppln_storage;
				obj_storage.shaderFolders		= _GetTempData().cfg.shaderDirs;
				obj_storage.defaultFeatureSet	= "DefaultFS";
				obj_storage.defaultShaderDefines = "\n"
					"AE_LICENSE_MIT\n"
					"AE_LICENSE_BSD2\n"
					"AE_LICENSE_BSD3\n"
					"AE_LICENSE_APACHE_2\n"
					"AE_LICENSE_UNLICENSE\n"
					"AE_LICENSE_CC0\n"
					"AE_LICENSE_CC_BY_NC_SA_3\n"
					"AE_ENABLE_UNKNOWN_LICENSE\n";

				obj_storage.spirvCompiler	= MakeUnique<SpirvCompiler>( _GetTempData().cfg.includeDirs );
				obj_storage.spirvCompiler->SetDefaultResourceLimits();
				
			  #ifdef AE_METAL_TOOLS
				if ( compileMSL )
				{
					obj_storage.metalCompiler		= MakeUnique<MetalCompiler>( _GetTempData().cfg.includeDirs );
					obj_storage.defaultDescSetUsage	= EDescSetUsage::ArgumentBuffer;
				}
			  #endif
			  #ifdef AE_ENABLE_SLANG
				{
					obj_storage.slangCompiler = MakeUnique<SLangCompiler>( _GetTempData().cfg.includeDirs );
				}
			  #endif

				ObjectStorage::SetInstance( &obj_storage );

				ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ obj_storage.defaultFeatureSet }};
				fs->fs = ScriptResourceApi::GetFeatureSet();

				PipelineCompiler::ScriptConfig	cfg;
				cfg.SetShaderVersion( EShaderVersion(Version2::From100( fs->fs.maxShaderVersion.spirv ).ToHex()) |
									  (useSlang ? EShaderVersion::_Slang_SPIRV : EShaderVersion::_GLSL_SPIRV) );

				cfg.SetDefaultLayout( EStructLayout::Compatible_Std140 );
				cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );
				
				const auto		flags	 = UIInteraction::Instance().graphics->shaderFlags;
				EShaderOpt		sh_opt	 = Default;
				EPipelineOpt	ppln_opt = Default;

				if ( flags.contains( UIInteraction::EShaderFlags::DebugInfo ))
				{
					sh_opt = EShaderOpt::DebugInfo;
				}
				else
				if ( flags.contains( UIInteraction::EShaderFlags::Optimize ))
				{
					sh_opt   = EShaderOpt::Optimize;
					ppln_opt |= EPipelineOpt::Optimize;
				}
		
				if ( flags.contains( UIInteraction::EShaderFlags::CaptureStatistics ))
					ppln_opt |= EPipelineOpt::CaptureStatistics;
		
				if ( flags.contains( UIInteraction::EShaderFlags::CaptureInternalRepresentation ))
					ppln_opt |= EPipelineOpt::CaptureInternalRepresentation;

				if ( compileMSL )
				{
					cfg.SetTarget( ECompilationTarget::Metal_Mac );
					cfg.SetSpirvToMslVersion( EShaderVersion::Metal_Mac_3_0 );	// TODO
				}
				else
				{
					cfg.SetTarget( ECompilationTarget::Vulkan );
				}

				StaticAssert( uint(UIInteraction::EShaderFlags::_Count) == 5 );

				cfg.SetPipelineOptions( ppln_opt );
				cfg.SetShaderOptions( sh_opt );
			}

			_LoadSamplers();				// throw
			_RegisterSharedShaderTypes();	// throw

			fn();

			ObjectStorage::SetInstance( null );
		}
		catch (...)
		{
			ObjectStorage::SetInstance( null );
			throw;
		}
	}

/*
=================================================
	_CompilePipeline
=================================================
*/
	bool  ScriptExe::_CompilePipeline (const Path &pplnPath)
	{
		CHECK_ERR( _engine2 );

		auto	obj_storage = ObjectStorage::Instance();
		CHECK_ERR( obj_storage != null );

		CHECK_ERR( obj_storage->CompilePipeline( _engine2, pplnPath, _config.pipelineIncludeDirs ));
		return true;
	}

/*
=================================================
	_CompilePipelineFromSource
=================================================
*/
	bool  ScriptExe::_CompilePipelineFromSource (const Path &pplnPath, StringView source)
	{
		CHECK_ERR( _engine2 );

		auto	obj_storage = ObjectStorage::Instance();
		CHECK_ERR( obj_storage != null );

		CHECK_ERR( obj_storage->CompilePipelineFromSource( _engine2, pplnPath, source, Default ));
		return true;
	}

/*
=================================================
	_ConvertAndLoad2
=================================================
*/
	RC<RStream>  ScriptExe::_ConvertAndLoad2 ()
	{
		auto	obj_storage = ObjectStorage::Instance();
		CHECK_THROW( obj_storage != null );

		CHECK_THROW( obj_storage->Build() );
		CHECK_THROW( obj_storage->BuildRenderTechniques() );

		auto	mem = MakeRC<ArrayWStream>();

		PipelinePackOffsets		offsets	= {};
		CHECK_THROW( obj_storage->SavePack( *mem, true, OUT offsets ));

		auto	mem2 = MakeRC<ArrayWDataSource>( mem->ReleaseData() );
				mem  = null;

		CHECK_THROW( mem2->Write( Sizeof(PackOffsets_Name), offsets ));

		obj_storage->Clear();

		return MakeRC<ArrayRStream>( mem2->ReleaseData() );
	}

/*
=================================================
	_LoadSamplers
=================================================
*/
	void  ScriptExe::_LoadSamplers () __Th___
	{
		auto	obj_storage = ObjectStorage::Instance();
		CHECK_THROW( obj_storage != null );

		auto&	fs = GraphicsScheduler().GetFeatureSet();

		{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->NearestClamp}};
			samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->NearestRepeat}};
			samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->NearestMirrorRepeat}};
			samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearClamp}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearRepeat}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearMirrorRepeat}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearMipmapClamp}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
			samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearMipmapRepeat}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
			samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		}{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearMipmapMirrorRepeat}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
			samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
		}

		if ( fs.samplerMirrorClampToEdge == FeatureSet::EFeature::RequireTrue )
		{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->LinearMipmapMirrorClamp}};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
			samp->SetAddressMode( EAddressMode::MirrorClampToEdge, EAddressMode::MirrorClampToEdge, EAddressMode::MirrorClampToEdge );
		}

		if ( fs.samplerAnisotropy == FeatureSet::EFeature::RequireTrue )
		{
			if ( fs.maxSamplerAnisotropy >= 8.0f )
			{
				{
					ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->Anisotropy8Repeat}};
					samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
					samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
					samp->SetAnisotropy( 8.0f );
				}{
					ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->Anisotropy8MirrorRepeat}};
					samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
					samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
					samp->SetAnisotropy( 8.0f );
				}{
					ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->Anisotropy8Clamp}};
					samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
					samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
					samp->SetAnisotropy( 8.0f );
				}
			}
			if ( fs.maxSamplerAnisotropy >= 8.0f )
			{
				{
					ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->Anisotropy16Repeat}};
					samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
					samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
					samp->SetAnisotropy( 16.0f );
				}{
					ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->Anisotropy16MirrorRepeat}};
					samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
					samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
					samp->SetAnisotropy( 16.0f );
				}{
					ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->Anisotropy16Clamp}};
					samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
					samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
					samp->SetAnisotropy( 16.0f );
				}
			}
		}

		if ( fs.fragmentDensityMap == FeatureSet::EFeature::RequireTrue )
		{
			ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->NearestClampSubsampled}};
			samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
			samp->SetOptions( ESamplerOpt::Subsampled );
			samp->SetLodRange( 0.f, 0.f );
		}

		if ( fs.samplerFilterMinmax == FeatureSet::EFeature::RequireTrue )
		{
			{
				ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->MaxLinearClamp}};
				samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
				samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
				samp->SetReductionMode( EReductionMode::Max );
			}{
				ScriptSamplerPtr	samp{new ScriptSampler{_sampConsts->MinLinearClamp}};
				samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
				samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
				samp->SetReductionMode( EReductionMode::Min );
			}
		}

		StaticAssert( (sizeof(SamplerConsts) / sizeof(String)) == 19 );
		CHECK_THROW( obj_storage->Build() );
	}

/*
=================================================
	_RegisterSharedShaderTypes
=================================================
*/
	void  ScriptExe::_RegisterSharedShaderTypes () __Th___
	{
		auto&	obj_storage = *ObjectStorage::Instance();

		if ( not obj_storage.structTypes.contains( "CameraData" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"CameraData"}};
			st->Set( EStructLayout::Compatible_Std140, R"#(
					float4x4	viewProj;
					float4x4	invViewProj;
					float4x4	proj;
					float4x4	view;
					float3		pos;
					float2		clipPlanes;
					float2		fov;
					float		zoom;
					float4		frustum [6];	// world space
				)#");
		}

		/*if ( not obj_storage.structTypes.contains( "CameraSet" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"CameraSet"}};
			st->Set( EStructLayout::Compatible_Std140, R"#(
					float		ipd;		// for VR video
					float3		globalPos;	// actual position: 'globalPos + data[0].localPos'
					uint		count;
					CameraData	data [4];
				)#");
		}*/

		if ( not obj_storage.structTypes.contains( "AccelStructInstance" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"AccelStructInstance"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					float3x4	transform;							// 3x4 row-major
					uint		instanceCustomIndex24_mask8;
					uint		instanceSBTOffset24_flags8;			// flags: gl::GeometryInstanceFlags
					uint2		accelerationStructureReference;		// gl::DeviceAddress
				)#");
			CHECK( st->StaticSize() == 64_b );
		}

		if ( not obj_storage.structTypes.contains( "ASBuildIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"ASBuildIndirectCommand"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					uint		primitiveCount;
					uint		primitiveOffset;
					uint		firstVertex;
					uint		transformOffset;
				)#");
			CHECK( st->StaticSize() == SizeOf<ASBuildIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "TraceRayIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"TraceRayIndirectCommand"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					packed_uint3	dim;
				)#");
			CHECK( st->StaticSize() == SizeOf<TraceRayIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "DispatchIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"DispatchIndirectCommand"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					packed_uint3	groupCount;
				)#");
			CHECK( st->StaticSize() == SizeOf<DispatchIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "DrawIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"DrawIndirectCommand"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					uint	vertexCount;
					uint	instanceCount;
					uint	firstVertex;
					uint	firstInstance;
				)#");
			CHECK( st->StaticSize() == SizeOf<DrawIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "DrawIndexedIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"DrawIndexedIndirectCommand"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					uint	indexCount;
					uint	instanceCount;
					uint	firstIndex;
					int		vertexOffset;
					uint	firstInstance;
				)#");
			CHECK( st->StaticSize() == SizeOf<DrawIndexedIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "DrawMeshTasksIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"DrawMeshTasksIndirectCommand"}};
			st->Set( EStructLayout::Compatible_Std430, R"#(
					packed_uint3	taskCount;
				)#");
			CHECK( st->StaticSize() == SizeOf<DrawMeshTasksIndirectCommand> );
		}
	}

/*
=================================================
	_GetSharedShaderTypes
=================================================
*/
	void  ScriptExe::_GetSharedShaderTypes (ScriptBasePass::CppStructsFromShaders &data) __Th___
	{
		_RegisterSharedShaderTypes();	// throw

		auto&	obj_storage = *ObjectStorage::Instance();

		{
			auto	it = obj_storage.structTypes.find( "CameraData" );
			CHECK_THROW( it != obj_storage.structTypes.end() );
			CHECK_THROW( it->second->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CompareImageTypes
=================================================
*/
	bool  CompareImageTypes (const Graphics::ImageDesc &lhs, const ResLoader::IntermImage &rhs)
	{
		const auto	[lhs_t0, lhs_t1]	= GetDescriptorImageTypeRelaxed( lhs );
		const auto	rhs_t0				= GetDescriptorImageTypeRelaxed( rhs.PixelFormat(), rhs.GetType(), False{"non-MS"}, False{"non-CubeMap"} );
		const auto	rhs_t1				= GetDescriptorImageTypeRelaxed( rhs.PixelFormat(), rhs.GetType(), False{"non-MS"}, True{"CubeMap"} );

		bool		is_equal			= lhs_t0 == rhs_t0 or lhs_t1 == rhs_t0 or
										  lhs_t0 == rhs_t1 or lhs_t1 == rhs_t1;
		if ( is_equal )
			return true;

		AE_LOGE( "Image description supports types: "s << PipelineCompiler::EImageType_ToString(lhs_t0) << ", " << PipelineCompiler::EImageType_ToString(lhs_t1) << ".\n"
				 "Intermediate image supports types: " << PipelineCompiler::EImageType_ToString(rhs_t0) << ", " << PipelineCompiler::EImageType_ToString(rhs_t1) << ".\n"
				 "But non of them are match." );
		return false;
	}

} // AE::ResEditor
