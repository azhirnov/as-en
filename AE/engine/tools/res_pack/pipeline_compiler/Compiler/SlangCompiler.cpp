// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_SLANG

# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wcast-qual"
# endif

# include "slang.h"
# include "slang-com-ptr.h"

# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic pop
# endif

# include "SlangCompiler.h"
# include "ScriptObjects/ObjectStorage.h"

# define SLANG_CHECK( _fn_ )		CHECK( SLANG_SUCCEEDED(_fn_) )
# define SLANG_CHECK_ERR( _fn_ )	CHECK_ERR( SLANG_SUCCEEDED(_fn_) )


namespace AE::PipelineCompiler
{
namespace
{
	class StringBlob final : public ISlangBlob
	{
	private:
		String	_storage;
		int		_refCount	= 0;

	public:
		explicit StringBlob (String str)			__NE___	: _storage{RVRef(str)} {}
		
		// ISlangUnknown //
		SlangResult SLANG_MCALL	queryInterface (SlangUUID const& uuid, void** outObject) __NE_OV
		{
			if ( uuid == ISlangBlob::getTypeGuid() )
			{
				++_refCount;
				*outObject = this;
				return SLANG_OK;
			}
			return SLANG_E_NO_INTERFACE;
		}

		uint SLANG_MCALL		addRef ()			__NE_OV	{ return ++_refCount; }
		uint SLANG_MCALL		release ()			__NE_OV
		{
			ASSERT( _refCount != 0 );
			const uint	cnt = --_refCount;
			if ( cnt == 0 )
				delete this;
			return cnt;
		}

		// ISlangBlob //
		void const* SLANG_MCALL	getBufferPointer ()	__NE_OV	{ return _storage.c_str(); }
		size_t SLANG_MCALL		getBufferSize ()	__NE_OV { return _storage.size(); }
	};

} // namespace
	
	using Slang::ComPtr;

/*
=================================================
	constructor
=================================================
*/
	SLangCompiler::SLangCompiler (ArrayView<Path> includeDirs) __NE___
	{
		bool	res = _Initialize( includeDirs );
		if ( not res )
			_Deinitialize();
	}

/*
=================================================
	destructor
=================================================
*/
	SLangCompiler::~SLangCompiler ()
	{
		_Deinitialize();
	}
	
/*
=================================================
	_Initialize
=================================================
*/
	bool  SLangCompiler::_Initialize (ArrayView<Path> includeDirs)
	{
		CHECK_ERR( not _lib );
		CHECK_ERR( _globalSession == null );
		CHECK_ERR( _session == null );

		#ifdef AE_PLATFORM_WINDOWS
		# ifdef AE_CFG_DEBUG
			Unused( _lib.Load( "slangd.dll" ));
		# else
			Unused( _lib.Load( "slang.dll" ));
		# endif
		#endif

		#ifdef AE_PLATFORM_LINUX
		# ifdef AE_CFG_DEBUG
			Unused( _lib.Load( "slangd.so" ));
		# else
			Unused( _lib.Load( "slang.so" ));
		# endif
		#endif

		#ifdef AE_PLATFORM_APPLE
		# ifdef AE_CFG_DEBUG
			Unused( _lib.Load( "slangd.dylib" ));
		# else
			Unused( _lib.Load( "slang.dylib" ));
		# endif
		#endif

		if ( not _lib )
			return false;

		decltype(&slang_createGlobalSession2)	fnCreateGlobalSession = null;
		CHECK_ERR( _lib.GetProcAddr( "slang_createGlobalSession2", OUT fnCreateGlobalSession ));
		
		decltype(&slang_shutdown)	fnShutdown = null;
		CHECK_ERR( _lib.GetProcAddr( "slang_shutdown", OUT fnShutdown ));

		_shutdown = fnShutdown;
		
		SlangGlobalSessionDesc	global_desc = {};
		SLANG_CHECK_ERR( fnCreateGlobalSession( &global_desc, OUT &_globalSession ));

		slang::TargetDesc	target_desc [1] = {};
		target_desc[0].format	= SLANG_SPIRV;
		target_desc[0].profile	= _globalSession->findProfile( "spirv_1_5" );	// TODO

		// TODO: compilerOptionEntries
		//target_desc[1].format	= SLANG_METAL_LIB;
		//target_desc[2].format	= SLANG_WGSL_SPIRV;

		Array<const char*>	include_dirs;
		Array<String>		include_dirs_str;
		include_dirs.resize( includeDirs.size() );
		include_dirs_str.resize( includeDirs.size() );

		for (usize i = 0; i < includeDirs.size(); ++i)
		{
			include_dirs_str[i]	= ToString( includeDirs[i] );
			include_dirs[i]		= include_dirs_str[i].c_str();
		}

		slang::SessionDesc	session_desc = {};
		session_desc.defaultMatrixLayoutMode	= SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
		session_desc.targetCount				= CountOf(target_desc);
		session_desc.targets					= target_desc;

		session_desc.searchPathCount			= include_dirs.size();
		session_desc.searchPaths				= include_dirs.data();

		SLANG_CHECK_ERR( _globalSession->createSession( session_desc, OUT &_session ));

		return true;
	}
	
/*
=================================================
	_Deinitialize
=================================================
*/
	void  SLangCompiler::_Deinitialize ()
	{
		if ( _session != null )
		{
			_session->release();
			_session = null;
		}

		if ( _globalSession != null )
		{
			_globalSession->release();
			_globalSession = null;
		}

		if ( _shutdown != null )
		{
			_shutdown();
			_shutdown = null;
		}

		_lib.Unload();
	}

/*
=================================================
	Compile
=================================================
*/
	bool  SLangCompiler::Compile (const Input &in, OUT Output &out)
	{
		CHECK_ERR( _session != null );
		CHECK_ERR( IsNullTerminated( in.entry ));

		ComPtr<slang::IModule>	src_module;
		{
			ComPtr<slang::IBlob>	diagnostic_blob;
			ComPtr<slang::IBlob>	source_blob		{new StringBlob{ String{in.header} << '\n' << in.source }};

			src_module.attach( _session->loadModuleFromSource( "name", ToString( in.fileLoc.path ).c_str(), source_blob, OUT diagnostic_blob.writeRef() ));

			if ( diagnostic_blob )
				out.log << Cast<char>(diagnostic_blob->getBufferPointer()) << "\n\n";

			if ( not src_module )
				return false;
		}
		
		ComPtr<slang::IEntryPoint>		entry_point;
		SLANG_CHECK_ERR( src_module->findEntryPointByName( in.entry.data(), OUT entry_point.writeRef() ));

		Array<slang::IComponentType*>	component_types;
		component_types.push_back( src_module );
		component_types.push_back( entry_point );
		
		ComPtr<slang::IComponentType>	composed_program;
		{
			ComPtr<slang::IBlob>	diagnostic_blob;
			SlangResult				result = _session->createCompositeComponentType(
												component_types.data(),
												component_types.size(),
												OUT composed_program.writeRef(),
												OUT diagnostic_blob.writeRef() );
			if ( diagnostic_blob )
				out.log << Cast<char>(diagnostic_blob->getBufferPointer()) << "\n\n";

			SLANG_CHECK_ERR( result );
		}
		
		ComPtr<slang::IBlob>	spirv_code;
		{
			ComPtr<slang::IBlob>	diagnostic_blob;
			SlangResult				result = composed_program->getEntryPointCode(
												0,
												0,
												OUT spirv_code.writeRef(),
												OUT diagnostic_blob.writeRef() );
			if ( diagnostic_blob )
				out.log << Cast<char>(diagnostic_blob->getBufferPointer()) << "\n\n";

			SLANG_CHECK_ERR( result );
		}

		NOTHROW_ERR( out.spirv.assign( Cast<ubyte>(spirv_code->getBufferPointer()), Cast<ubyte>(spirv_code->getBufferPointer()) + spirv_code->getBufferSize() ));

		return true;
	}


} // AE::PipelineCompiler

#else

# include "SlangCompiler.h"

namespace AE::PipelineCompiler
{
	SLangCompiler::SLangCompiler (ArrayView<Path>)				__NE___	{}
	SLangCompiler::~SLangCompiler ()									{}
	bool  SLangCompiler::Compile (const Input &, OUT Output &)			{ return false; }

} // AE::PipelineCompiler
#endif // AE_ENABLE_SLANG
