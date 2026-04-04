// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Core/ScriptExe.h"
#include "vfs/Archive/ReloadableArchiveStorage.h"

#include "res_pack/pipeline_compiler/PipelineCompiler.h"
#include "res_pack/asset_packer/AssetPacker.h"

namespace AE::UIEditor
{
namespace
{
/*
=================================================
	ConvertString2
=================================================
*/
	template <typename SrcStr>
	ND_ static BasicString<CharType>  ConvertString2 (const SrcStr &src) __Th___
	{
		BasicString<CharType>	dst;
		CHECK_THROW( Base::ConvertString( OUT dst, BasicStringView{src} ));
		return dst;
	}

	ND_ static BasicString<CharType>  ConvertString2 (const Path &src) __Th___
	{
		BasicString<CharType>	dst;
		CHECK_THROW( Base::ConvertString( OUT dst, BasicStringView{src.native()} ));
		return dst;
	}

/*
=================================================
	ConvertArray
=================================================
*/
	struct PathParams2
	{
		BasicString<CharType>	path;
		uint					priority;
		uint					flags;

		PathParams2 (const Path &path, uint prio) __Th___ :
			path{ConvertString2(path)}, priority{prio}, flags{0}
		{}

		PathParams2 (const Path &path, uint prio, PipelineCompiler::EPathParamsFlags flags) __Th___ :
			path{ConvertString2(path)}, priority{prio}, flags{uint(flags)}
		{}

		PathParams2 (const Path &path, uint prio, AssetPacker::EPathParamsFlags flags) __Th___ :
			path{ConvertString2(path)}, priority{prio}, flags{uint(flags)}
		{}
	};

	template <typename R>
	ND_ static Array<R>  ConvertArray (Array<PathParams2> &src)
	{
		Array<R>	dst;
		dst.resize( src.size() );

		for (usize i = 0; i < src.size(); ++i)
		{
			dst[i].path		= src[i].path.c_str();
			dst[i].priority	= src[i].priority;
			dst[i].flags	= src[i].flags;
		}
		return dst;
	}

/*
=================================================
	ConvertArray
=================================================
*/
	ND_ static Array<const CharType *>  ConvertArray (Array< BasicString<CharType> > &src)
	{
		Array<const CharType *>		dst;
		dst.resize( src.size() );

		for (usize i = 0; i < src.size(); ++i) {
			dst[i] = src[i].c_str();
		}
		return dst;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	ScriptExe::ScriptExe () __NE___
	{
		#ifdef CMAKE_INTDIR
			CHECK( _pipelineCompilerLib.Load( Path{AE_LIBRARY_FOLDER} / CMAKE_INTDIR / "PipelineCompiler-shared.dll" ));
			CHECK( _assetPackerLib.Load( Path{AE_LIBRARY_FOLDER} / CMAKE_INTDIR / "AssetPacker-shared.dll" ));
		#else
			CHECK( _pipelineCompilerLib.Load( Path{AE_LIBRARY_FOLDER} / "PipelineCompiler-shared.so" ));
			CHECK( _assetPackerLib.Load( Path{AE_LIBRARY_FOLDER} / "AssetPacker-shared.so" ));
		#endif
		CHECK( _pipelineCompilerLib.GetProcAddr( "CompilePipelines", OUT _compilePipelines ));
		CHECK( _assetPackerLib.GetProcAddr( "PackAssets", OUT _packAssets ));

		FileSystem::DeleteDirectory( AE_RES_FOLDER "/../temp" );
	}

	ScriptExe::~ScriptExe () __NE___
	{
	}

/*
=================================================
	InitVFS
=================================================
*/
	bool  ScriptExe::InitVFS () __NE___
	{
		_vfStorage = MakeRC<VFS::ReloadableArchiveStorage>();
		CHECK_ERR( GetVFS().AddStorage( _vfStorage ));

		return true;
	}

/*
=================================================
	Run
=================================================
*/
	auto  ScriptExe::Run (const Path &scriptPath, IOutputSurface &output) __NE___ -> SharedPtr<UIScreen::Ctor>
	{
		CHECK_ERR( _compilePipelines != null and _packAssets != null );

		_vfStorage->Reset();

		const Path	ppln_fname	{AE_RES_FOLDER "/../temp/pipelines-"s + ToString(_counter) + ".bin"};
		const Path	res_fname	{AE_RES_FOLDER "/../temp/ui-res-"s + ToString(_counter) + ".bin"};
		++_counter;

		CHECK_ERR( _CompilePipelines( ppln_fname ));
		CHECK_ERR( _BuildResources( scriptPath, res_fname ));

		auto	screen	= MakeShared<UIScreen::Ctor>();
		screen->rtech	= _CompileResources( ppln_fname, output );

		CHECK_ERR( _vfStorage->Reload( res_fname ));
		CHECK_ERR( _vfStorage->Open( OUT screen->styleStream, VFS::FileName{"ui-editor.style"} ));
		CHECK_ERR( _vfStorage->Open( OUT screen->widgetStream, VFS::FileName{"ui-editor.widget"} ));

		AE_LOGI( "<<<<< Loaded script >>>>>", SourceLoc( ToString(scriptPath).c_str(), 1 ));
		return screen;
	}

/*
=================================================
	_CompilePipelines
=================================================
*/
	bool  ScriptExe::_CompilePipelines (const Path &outputPackName) C_NE___
	{
		using namespace AE::PipelineCompiler;

		const auto	output_pack_name		= ConvertString2( outputPackName );
		const auto	output_cpp_types_file	= ConvertString2( AE_UI_SCRIPT_FOLDER "/../cpp/vk_types.h"s );
		const auto	output_cpp_names_file	= ConvertString2( AE_UI_SCRIPT_FOLDER "/../cpp/vk_names.h"s );

		auto		tmp_pipelines			= Array<PathParams2>{ PathParams2{ AE_SHARED_DATA "/feature_set"s,				0u, EPathParamsFlags::Folder },
																  PathParams2{ AE_UI_SCRIPT_FOLDER "/../config_vk.as",		1u },
																  PathParams2{ AE_UI_SCRIPT_FOLDER "/../rtech/samplers.as",	2u },
																  PathParams2{ AE_UI_SCRIPT_FOLDER "/../rtech/ren_tech.as",	3u },
																  PathParams2{ AE_CANVAS_VERTS,								4u },
																  PathParams2{ AE_UI_SCRIPT_FOLDER "/../pipelines"s,		5u, EPathParamsFlags::Folder }};
		auto		tmp_shader_folders		= Array<BasicString<CharType>>{ ConvertString2( AE_UI_SCRIPT_FOLDER "/../shaders"s )};
		auto		tmp_shader_include_dirs	= Array<BasicString<CharType>>{ ConvertString2( AE_SHARED_DATA "/shaders"s )};

		const auto	pipelines				= ConvertArray<PipelineCompiler::PathParams>( tmp_pipelines );
		const auto	shader_folders			= ConvertArray( tmp_shader_folders );
		const auto	shader_include_dirs		= ConvertArray( tmp_shader_include_dirs );

		PipelinesInfo	info = {};

		// input pipelines (file / folders)
		info.inPipelines			= pipelines.data();
		info.inPipelineCount		= pipelines.size();

		// input shaders
		info.shaderFolders			= shader_folders.data();
		info.shaderFolderCount		= shader_folders.size();

		// shader include directories
		info.shaderIncludeDirs		= shader_include_dirs.data();
		info.shaderIncludeDirCount	= shader_include_dirs.size();

		// pipeline include directories
		info.pipelineIncludeDirs	= null;
		info.pipelineIncludeDirCount= 0;

		// output
		info.outputPackName			= output_pack_name.c_str();
		info.outputCppStructsFile	= output_cpp_types_file.c_str();
		info.outputCppNamesFile		= output_cpp_names_file.c_str();
		info.cppReflectionFlags		= EReflectionFlags::All;

		info.flags					= EPipelineCompilerFlags::AddNameMapping;

		return BitCast<CompilePipelinesFn_t>(_compilePipelines)( &info );
	}

/*
=================================================
	_BuildResources
=================================================
*/
	bool  ScriptExe::_BuildResources (const Path &scriptPath, const Path &assetPack) C_NE___
	{
		using namespace AE::AssetPacker;

		auto		tmp_files = Array<PathParams2>{
									PathParams2{ AE_UI_SCRIPT_FOLDER "/../ui_resources.as",	0u, EPathParamsFlags::File },
									PathParams2{ scriptPath,								1u, EPathParamsFlags::File }
								};
		auto		tmp_scr_include = Array<BasicString<CharType>>{
											ConvertString2( AE_UI_SCRIPT_FOLDER "/.."s ),
											ConvertString2( AE_UI_SCRIPT_FOLDER "/../ui_styles"s )
										};
		auto		tmp_res_folders	= Array<BasicString<CharType>>{
											ConvertString2( AE_DATA_FOLDER ""s )
										};

		const auto	scr_include	= ConvertArray( tmp_scr_include );
		const auto	res_folders	= ConvertArray( tmp_res_folders );
		const auto	output		= ConvertString2( assetPack );
		const auto	files		= ConvertArray<AssetPacker::PathParams>( tmp_files );
		const auto	temp_file	= ConvertString2( Path{assetPack}.replace_extension(".temp") );

		AssetInfo	info = {};

		// input scripts (files / folders)
		info.inFiles				= files.data();
		info.inFileCount			= files.size();

		// script include directories
		info.inIncludeFolders		= scr_include.data();
		info.inIncludeFolderCount	= scr_include.size();

		// resource folders
		info.inResourceFolders		= res_folders.data();
		info.inResourceFolderCount	= res_folders.size();

		// temp / output
		info.tempFile				= temp_file.c_str();
		info.outputArchive			= output.c_str();

		return BitCast<PackAssetsFn_t>(_packAssets)( &info );
	}

/*
=================================================
	_CompileResources
=================================================
*/
	RTechInfo  ScriptExe::_CompileResources (const Path &filePath, IOutputSurface &output) __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto	rp_info = output.GetRenderPassInfo();
		CHECK_ERR( rp_info.attachments.size() == 1 );
		CHECK_ERR( rp_info.attachments[0].samples == 1_samples );

		auto	file = MakeRCNe<FileRStream>( filePath );
		CHECK_ERR( file->IsOpen() );

		PipelinePackDesc	desc;
		desc.stream			= file;
		desc.options		= EPipelinePackOpt::Samplers | EPipelinePackOpt::RenderPasses | EPipelinePackOpt::Pipelines;
		desc.surfaceFormat	= rp_info.attachments[0].format;
		desc.dbgName		= "ui screen ppln pack";

		RTechInfo	res;

		res.packId = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( res.packId );

		res.rtech = res_mngr.LoadRenderTech( res.packId, RenderTechName{"UI.RTech"} );
		CHECK_ERR( res.rtech );

		return res;
	}


} // AE::UIEditor
