// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Passes/BuildRTAS.h"
#include "res_editor/Passes/Export.h"

#include "res_editor/Core/EditorCore.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
namespace {
	static ScriptExe*  s_scriptExe = null;
}

	using namespace AE::Scripting;
	using namespace AE::PipelineCompiler;


	//
	// Present Pass
	//
	class ScriptExe::ScriptPresent final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		rt;
		ImageLayer			layer;
		MipmapLevel			mipmap;
		RC<DynamicDim>		dynSize;

	public:
		ScriptPresent (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, RC<DynamicDim> dynSize) :
			rt{rt}, layer{layer}, mipmap{mipmap}, dynSize{dynSize} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV;
	};

/*
=================================================
	ScriptPresent::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptPresent::ToPass () C_Th___
	{
		Array< RC<Image> >	src;

		ImageViewDesc	desc;
		desc.baseLayer	= layer;
		desc.baseMipmap	= mipmap;

		RC<Image>	img = rt->ToResource();
		CHECK_THROW( img );

		img = img->CreateView( desc, "PresentSrc" );
		CHECK_THROW( img );

		src.push_back( img );

		auto	fmode = UIInteraction::Instance().GetFilterMode();

		return MakeRCTh<ResEditor::Present>( RVRef(src), "Present", dynSize, fmode );
	}
//-----------------------------------------------------------------------------



	//
	// Dbg View Pass
	//
	class ScriptExe::ScriptDbgView final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		rt;
		ImageLayer			layer;
		MipmapLevel			mipmap;
		DebugView::EFlags	flags;
		uint				index;

	public:
		ScriptDbgView (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags, uint idx) :
			rt{rt}, layer{layer}, mipmap{mipmap}, flags{flags}, index{idx} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV
		{
			return MakeRCTh<ResEditor::DebugView>( rt->ToResource(), index, flags, layer, mipmap,
												   s_scriptExe->_GetRenderer(), "DbgView" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Generate Mipmaps Pass
	//
	class ScriptExe::ScriptGenMipmaps final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		rt;

	public:
		ScriptGenMipmaps (const ScriptImagePtr &rt) : rt{rt} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV
		{
			return MakeRCTh<ResEditor::GenerateMipmapsPass>( rt->ToResource(), "GenMipmaps" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Copy Image Pass
	//
	class ScriptExe::ScriptCopyImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		src;
		ScriptImagePtr		dst;

	public:
		ScriptCopyImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) :
			src{src}, dst{dst} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV
		{
			return MakeRCTh<ResEditor::CopyImagePass>( src->ToResource(), dst->ToResource(), "CopyImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Clear Image Pass
	//
	class ScriptExe::ScriptClearImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr					image;
		ClearImagePass::ClearValue_t	value;

	public:
		ScriptClearImage (const ScriptImagePtr &image, ClearImagePass::ClearValue_t value) :
			image{image}, value{value} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV
		{
			return MakeRCTh<ResEditor::ClearImagePass>( image->ToResource(), value, "ClearImage" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Clear Buffer Pass
	//
	class ScriptExe::ScriptClearBuffer final : public ScriptBasePass
	{
	private:
		ScriptBufferPtr		buffer;
		Bytes				offset;
		Bytes				size	= UMax;
		uint				value;

	public:
		ScriptClearBuffer (const ScriptBufferPtr &buffer, uint value) :
			buffer{buffer}, value{value} {}

		ScriptClearBuffer (const ScriptBufferPtr &buffer, Bytes offset, Bytes size, uint value) :
			buffer{buffer}, offset{offset}, size{size}, value{value} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV
		{
			RC<Buffer>	buf;

			s_scriptExe->_RunWithPipelineCompiler(
				[&] () {
					buf = buffer->ToResource();
					CHECK_THROW( buf );
				});

			return MakeRCTh<ResEditor::ClearBufferPass>( buf, offset, size, value, "ClearBuffer" );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Export Image
	//
	class ScriptExe::ScriptExportImage final : public ScriptBasePass
	{
	private:
		ScriptImagePtr		image;
		String				prefix;

	public:
		ScriptExportImage (const ScriptImagePtr &image, const String &prefix) :
			image{image}, prefix{prefix} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV
		{
			return MakeRCTh<ResEditor::ExportImage>( image->ToResource(), RVRef(prefix) );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Export Buffer
	//
	class ScriptExe::ScriptExportBuffer final : public ScriptBasePass
	{
	public:
		enum class EMode {
			Structured,
			Binary,
		};

	private:
		ScriptBufferPtr		buffer;
		String				prefix;
		EMode				mode;

	public:
		ScriptExportBuffer (const ScriptBufferPtr &buffer, const String &prefix, EMode mode) :
			buffer{buffer}, prefix{prefix}, mode{mode} {}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV;

	private:
		struct _Utils;
	};

/*
=================================================
	ScriptExportBuffer::_Utils
=================================================
*/
	struct ScriptExe::ScriptExportBuffer::_Utils
	{
	private:
		ShaderStructType &		_bufferType;
		BufferMemView const&	_memView;

		String					str;

	public:
		explicit _Utils (ShaderStructType &stType, const BufferMemView &memView) :
			_bufferType{stType}, _memView{memView}
		{}

		void  Parse (WStream &stream);

	private:
		void  _AddTypeName (const ShaderStructType::Field &field);

		ND_ bool  _Parse (const ShaderStructType &stType, Bytes baseOffset, uint depth);

		template <typename T>
		ND_ bool  _ParseVal (const ShaderStructType::Field &field, Bytes baseOffset, ulong arraySize, uint depth);
	};


/*
=================================================
	ScriptExportBuffer::_Utils::Parse
=================================================
*/
	void  ScriptExe::ScriptExportBuffer::_Utils::Parse (WStream &stream)
	{
		str.reserve( 1024 );

		str << "buffer ";
		switch_enum( _bufferType.Layout() )
		{
			case EStructLayout::Compatible_Std140 :
			case EStructLayout::Std140 :			str << "(std140) ";	break;
			case EStructLayout::Compatible_Std430 :
			case EStructLayout::Std430 :			str << "(std430) ";	break;
			case EStructLayout::Metal :				str << "(metal) ";	break;
			case EStructLayout::InternalIO :		break;
			case EStructLayout::_Count :
			case EStructLayout::Unknown :			break;
		}
		switch_end
		str << _bufferType.Typename() << " {\n";

		CHECK( _Parse( _bufferType, 0_b, 1 ));

		str << "} // " << _bufferType.Typename() << "\n";

		CHECK( stream.Write( str ));
	}

/*
=================================================
	ScriptExportBuffer::_Utils::_AddTypeName
=================================================
*/
	void  ScriptExe::ScriptExportBuffer::_Utils::_AddTypeName (const ShaderStructType::Field &field)
	{
		if ( field.IsStruct() )
		{
			str << field.stType->Typename();
		}
		else
		{
			switch_enum( field.type )
			{
				case EValueType::Bool8 :		str << "bool";			break;
				case EValueType::Bool32 :		str << "bool";			break;
				case EValueType::Int8 :			str << "byte";			break;
				case EValueType::Int16 :		str << "short";			break;
				case EValueType::Int32 :		str << "int";			break;
				case EValueType::Int64 :		str << "long";			break;
				case EValueType::UInt8 :		str << "ubyte";			break;
				case EValueType::UInt16 :		str << "ushort";		break;
				case EValueType::UInt32 :		str << "uint";			break;
				case EValueType::DeviceAddress:	str << "DeviceAddress";	break;
				case EValueType::UInt64 :		str << "ulong";			break;
				case EValueType::Float16 :		str << "half";			break;
				case EValueType::Float32 :		str << "float";			break;
				case EValueType::Float64 :		str << "double";		break;
				case EValueType::Int8_Norm :
				case EValueType::Int16_Norm :
				case EValueType::UInt8_Norm :
				case EValueType::UInt16_Norm :
				case EValueType::Unknown :
				case EValueType::_Count :		str << "<unknown>";		break;
			}
			switch_end

			if ( field.IsVec() )	str << ToString( field.rows );
			if ( field.IsMat() )	str << ToString( field.cols ) << 'x' << ToString( field.rows );
		}
	}

/*
=================================================
	ScriptExportBuffer::_Utils::_Parse
=================================================
*/
	bool  ScriptExe::ScriptExportBuffer::_Utils::_Parse (const ShaderStructType &stType, const Bytes baseOffset, const uint depth)
	{
		for (auto& field : stType.Fields())
		{
			ulong	array_size	= field.IsStaticArray() ? field.arraySize : 0;

			if ( field.IsDynamicArray() )
			{
				Bytes	remain_size = _memView.DataSize() - baseOffset - field.offset;
				CHECK_ERR( IsMultipleOf( remain_size, field.size ));

				array_size	= ulong{remain_size / field.size};
			}

			AppendToString( INOUT str, depth*2, ' ' );
			_AddTypeName( field );
			str << "  " << field.name;

			if ( array_size > 0 )	str << " [" << ToString( array_size ) << "] = {\n";
			else					str << " = ";

			if ( field.IsStruct() )
			{
				const uint	depth2 = depth+1 + uint(array_size > 0);

				for (ulong i = 0, cnt = Max( array_size, 1u ); i < cnt; ++i)
				{
					if ( array_size > 0 ) {
						AppendToString( INOUT str, (depth+1)*2, ' ' );
						str << field.stType->Typename() << " { // [" << ToString(i) << "]\n";
					}else{
						str << field.stType->Typename() << " {\n";
					}

					Bytes	off = baseOffset + field.offset + AlignUp( field.size, field.align ) * i;
					ASSERT( off + field.size <= _memView.DataSize() );

					CHECK_ERR( _Parse( *field.stType, off, depth2 ));

					AppendToString( INOUT str, (depth+1)*2, ' ' );

					if ( array_size > 0 ) {
						str << "}" << (i+1 != cnt ? "," : " ") << " // " << field.stType->Typename() << "\n";
					}else{
						str << "} // " << field.stType->Typename();
					}
				}
			}
			else
			{
				switch_enum( field.type )
				{
					case EValueType::Bool8 :		CHECK_ERR( _ParseVal< bool   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Bool32 :		CHECK_ERR( _ParseVal< lbool  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int8 :			CHECK_ERR( _ParseVal< sbyte  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int16 :		CHECK_ERR( _ParseVal< sshort >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int32 :		CHECK_ERR( _ParseVal< sint   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int64 :		CHECK_ERR( _ParseVal< slong  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt8 :		CHECK_ERR( _ParseVal< ubyte  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt16 :		CHECK_ERR( _ParseVal< ushort >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt32 :		CHECK_ERR( _ParseVal< uint   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::DeviceAddress:	CHECK_ERR( _ParseVal<DeviceAddress>( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::UInt64 :		CHECK_ERR( _ParseVal< ulong  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Float16 :		CHECK_ERR( _ParseVal< half   >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Float32 :		CHECK_ERR( _ParseVal< float  >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Float64 :		CHECK_ERR( _ParseVal< double >( field, baseOffset, array_size, depth+1 ));	break;
					case EValueType::Int8_Norm :
					case EValueType::Int16_Norm :
					case EValueType::UInt8_Norm :
					case EValueType::UInt16_Norm :
					case EValueType::Unknown :
					case EValueType::_Count :
					default :						RETURN_ERR( "unsupported value type" );
				}
				switch_end
			}

			if ( array_size > 0 )	{ AppendToString( INOUT str, depth*2, ' ' );  str << "} // " << field.name << "[]\n"; }
			else					str << "\n";
		}

		return true;
	}

/*
=================================================
	ScriptExportBuffer::_Utils::_ParseVal
=================================================
*/
	template <typename T>
	bool  ScriptExe::ScriptExportBuffer::_Utils::_ParseVal (const ShaderStructType::Field &field, Bytes baseOffset,
															const ulong arraySize, const uint depth)
	{
		const bool	is_array	= arraySize > 0;
		const Bytes	elem_size	= field.size / Max( arraySize, 1u );

		baseOffset += field.offset;
		ASSERT( baseOffset + field.size <= _memView.DataSize() );
		ASSERT( SizeOf<T> * field.rows * field.cols == elem_size );

		for (ulong i = 0, cnt = Max( arraySize, 1u ); i < cnt; ++i)
		{
			Bytes	off  = baseOffset + elem_size * i;
			auto	data = _memView.GetRange( off, elem_size );

			CHECK_ERR( data.size == elem_size );

			if ( is_array )  AppendToString( INOUT str, depth*2, ' ' );

			if ( field.rows > 1 ) str << "{ ";

			for (uint r = 0, rows = field.rows; r < rows; ++r)
			{
				if constexpr( IsSameTypes< T, DeviceAddress >)
					str << "0x" << ToString<16>( Cast<T>(data.ptr)[r] );
				else
				if constexpr( IsSameTypes< T, half >)
					str << ToString( float{Cast<T>(data.ptr)[r]}, 5, True{"exp"} );
				else
				if constexpr( IsFloatPoint<T> )
					str << ToString( Cast<T>(data.ptr)[r], 5, True{"exp"} );
				else
					str << ToString( Cast<T>(data.ptr)[r] );

				if ( r+1 != rows )  str << ", ";
			}

			if ( field.rows > 1 ) str << " }";

			if ( is_array ) str << (i+1 != arraySize ? "," : " ") << "  // [" << ToString(i) << "]\n";
		}
		return true;
	}

/*
=================================================
	ScriptExportBuffer::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptExportBuffer::ToPass () C_Th___
	{
		ShaderStructTypePtr	st_type;
		RC<Buffer>			buf;

		s_scriptExe->_RunWithPipelineCompiler(
			[&] () {
				auto	storage = ObjectStorage::Instance();
				CHECK_THROW( storage );

				buffer->AddLayoutReflection();
				buf = buffer->ToResource();
				CHECK_THROW( buf );

				auto&	st_types	= storage->structTypes;
				auto	it			= st_types.find( buffer->GetTypeName() );

				CHECK_THROW( it != st_types.end() );
				st_type = it->second;
			});

		switch_enum( mode )
		{
			case EMode::Structured :
				return MakeRCTh<ResEditor::ExportBuffer>( buf, RVRef(prefix),
								[st_type] (const BufferMemView &memView, WStream &stream) {
									_Utils{ *st_type, memView }.Parse( stream );
								});

			case EMode::Binary :
				return MakeRCTh<ResEditor::ExportBuffer>( buf, RVRef(prefix), Default );

			default :
				CHECK_THROW_MSG( false, "unsupported buffer export mode" );
		}
		switch_end
	}
//-----------------------------------------------------------------------------



	//
	// Build Ray Tracing Geometry Pass
	//
	class ScriptExe::ScriptBuildRTGeometry final : public ScriptBasePass
	{
	private:
		ScriptRTGeometryPtr	_dstGeometry;
		const bool			_indirect;


	public:
		ScriptBuildRTGeometry (ScriptRTGeometryPtr	dstGeometry,
							   bool					indirect) :
			_dstGeometry{ dstGeometry },
			_indirect{ indirect }
		{}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV;
	};

/*
=================================================
	ScriptBuildRTGeometry::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptBuildRTGeometry::ToPass () C_Th___
	{
		CHECK_THROW( s_scriptExe != null );

		RC<RTGeometry>	dst_geom;
		s_scriptExe->_RunWithPipelineCompiler(
			[&] () {
				dst_geom = _dstGeometry->ToResource();
			});

		return MakeRCTh<ResEditor::BuildRTGeometry>(
					dst_geom, _indirect,
					"BuildRTGeometry" );
	}
//-----------------------------------------------------------------------------



	//
	// Build Ray Tracing Scene Pass
	//
	class ScriptExe::ScriptBuildRTScene final : public ScriptBasePass
	{
	private:
		ScriptRTScenePtr	_dstScene;
		const bool			_indirect;


	public:
		ScriptBuildRTScene (ScriptRTScenePtr	dstScene,
							bool				indirect) :
			_dstScene{ dstScene },
			_indirect{ indirect }
		{}

		void		_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th_OV {}

		RC<IPass>	ToPass () C_Th_OV;
	};

/*
=================================================
	ScriptBuildRTScene::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptBuildRTScene::ToPass () C_Th___
	{
		CHECK_THROW( s_scriptExe != null );

		RC<RTScene>		dst_scene;
		s_scriptExe->_RunWithPipelineCompiler(
			[&] () {
				dst_scene = _dstScene->ToResource();
			});

		return MakeRCTh<ResEditor::BuildRTScene>(
					dst_scene, _indirect,
					"BuildRTScene" );
	}
//-----------------------------------------------------------------------------



	//
	// Pass Group
	//
	class ScriptExe::ScriptPassGroup final : public ScriptBasePass
	{
	private:
		Array< ScriptBasePassPtr >	_passes;
		const PassGroup::EFlags		_flags;
		RC<Renderer>				_renderer;
		mutable RC<IPass>			_result;

	public:
		ScriptPassGroup (PassGroup::EFlags flags, RC<Renderer> renderer) :
			_flags{flags}, _renderer{RVRef(renderer)}
		{}

			void  Add (ScriptBasePassPtr pass)							{ _passes.push_back( RVRef(pass) ); }
			void  _OnAddArg (INOUT ScriptPassArgs::Argument &)	C_Th_OV	{}

		ND_ ArrayView<ScriptBasePassPtr>	GetPasses ()		const	{ return _passes; }
		ND_ RC<IPass>						ToPass ()			C_Th_OV;
	};

/*
=================================================
	ScriptPassGroup::ToPass
=================================================
*/
	RC<IPass>  ScriptExe::ScriptPassGroup::ToPass () C_Th___
	{
		if ( _result )
			return _result;

		CHECK_THROW( not _passes.empty() );

		RC<PassGroup>	pg = MakeRC<PassGroup>( _flags, _renderer->GetDataTransferQueue() );

		for (auto& script_pass : _passes) {
			pg->AddPass( script_pass->ToPass() );  // throw
		}

		_result = pg;
		return pg;
	}
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
				obj_storage.shaderVersion		= EShaderVersion(Version2::From100( fs->fs.maxShaderVersion.spirv ).ToHex()) | EShaderVersion::_SPIRV;
				obj_storage.defaultFeatureSet	= fs->Name();

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
			AE_LOGI( "<<<<< Loaded script >>>>>", ToString(filePath), 1 );
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

			src.name = ToString( filePath.filename().replace_extension("") );

			if ( not file.Read( file.RemainingSize(), OUT src.script ))
				RETURN_ERR( "Failed to read script file: '"s << ansi_path << "'" );
		}

		src.dbgLocation		= SourceLoc{ ansi_path, 0 };
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

		auto			rtech	= res_mngr.LoadRenderTech( pack_id, RenderTechName{"rtech"}, Default );
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
		data.renderer->SetSurfaceFormat( cs != Default ? ESurfaceFormat_Cast( rt->Description().format, cs ) : Default );

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
		rt->AddUsage( EResourceUsage::GenMipmaps );

		auto&	data = _GetTempData();
		CHECK_THROW_MSG( data.passGroup );

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptGenMipmaps{ rt }});
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
		CHECK_THROW_MSG( rt );

		auto&	data	= _GetTempData();
		auto	idx		= data.dbgViewCounter++;

		CHECK_THROW_MSG( idx < UIInteraction::MaxDebugViews );

		switch_enum( flags )
		{
			case DebugView::EFlags::Copy :			rt->AddUsage( EResourceUsage::Transfer );		break;
			case DebugView::EFlags::NoCopy :		rt->AddUsage( EResourceUsage::Present );		break;
			case DebugView::EFlags::Histogram :		rt->AddUsage( EResourceUsage::Sampled );		break;
			case DebugView::EFlags::LinearDepth :	rt->AddUsage( EResourceUsage::Sampled );		break;
			case DebugView::EFlags::Stencil :		rt->AddUsage( EResourceUsage::DepthStencil );	break;
			case DebugView::EFlags::_Count :
			default :								CHECK_THROW_MSG( false, "unsupported flags" );
		}
		switch_end

		data.passGroup->Add( ScriptBasePassPtr{ new ScriptDbgView{ rt, layer, mipmap, flags, idx }});
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

		std::memcpy( OUT &dst.intRange[0], &min, sizeof(min) );
		std::memcpy( OUT &dst.intRange[1], &max, sizeof(max) );
		std::memcpy( OUT &dst.intRange[2], &initial, sizeof(initial) );
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

		if ( normalized )
			_NormalizeSpectrum( INOUT wlToRGB );
	}

/*
=================================================
	_Supported_***
=================================================
*/
namespace {
	static bool  _Supported_GeometryShader ()
	{
		return GraphicsScheduler().GetFeatureSet().geometryShader == FeatureSet::EFeature::RequireTrue;
	}

	static bool  _Supported_TessellationShader ()
	{
		return GraphicsScheduler().GetFeatureSet().tessellationShader == FeatureSet::EFeature::RequireTrue;
	}

	static bool  _Supported_SamplerAnisotropy ()
	{
		return GraphicsScheduler().GetFeatureSet().samplerAnisotropy == FeatureSet::EFeature::RequireTrue;
	}

	static EPixelFormat  _Supported_DepthFormat ()
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth32F ))	return EPixelFormat::Depth32F;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth24 ))	return EPixelFormat::Depth24;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth16 ))	return EPixelFormat::Depth16;
		return Default;
	}

	static EPixelFormat  _Supported_DepthStencilFormat ()
	{
		auto&	fs = GraphicsScheduler().GetFeatureSet();
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth32F_Stencil8 ))	return EPixelFormat::Depth32F_Stencil8;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth24_Stencil8 ))	return EPixelFormat::Depth24_Stencil8;
		if ( fs.attachmentFormats.contains( EPixelFormat::Depth16_Stencil8 ))	return EPixelFormat::Depth16_Stencil8;
		return Default;
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
			StaticAssert( uint(ESurfaceFormat::_Count) == 10 );

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
		ScriptRayTracingPass::Bind( se );
		ScriptSceneGraphicsPass::Bind( se );
		ScriptSceneRayTracingPass::Bind( se );
		ScriptScene::Bind( se );

		se->AddFunction( &ScriptExe::_SurfaceSize,				"SurfaceSize",				{},		"Returns dynamic dimensions of the screen surface."	);

		se->AddFunction( &ScriptExe::_Present1,					"Present",					{},		"Present image to the screen." );
		se->AddFunction( &ScriptExe::_Present2,					"Present",					{} );
		se->AddFunction( &ScriptExe::_Present3,					"Present",					{} );
		se->AddFunction( &ScriptExe::_Present4,					"Present",					{} );
		se->AddFunction( &ScriptExe::_Present5,					"Present",					{} );
		se->AddFunction( &ScriptExe::_Present6,					"Present",					{} );

		se->AddFunction( &ScriptExe::_DbgView1,					"DbgView",					{},		"Draw image in child window for debugging." );
		se->AddFunction( &ScriptExe::_DbgView2,					"DbgView",					{} );
		se->AddFunction( &ScriptExe::_DbgView3,					"DbgView",					{} );
		se->AddFunction( &ScriptExe::_DbgView4,					"DbgView",					{} );

		se->AddFunction( &ScriptExe::_GenMipmaps,				"GenMipmaps",				{},		"Pass which generates mipmaps for image." );
		se->AddFunction( &ScriptExe::_CopyImage,				"CopyImage",				{},		"Pass which copy image content to another image." );

		se->AddFunction( &ScriptExe::_ClearImage1,				"ClearImage",				{},		"Pass to clear float-color image." );
		se->AddFunction( &ScriptExe::_ClearImage2,				"ClearImage",				{},		"Pass to clear uint-color image." );
		se->AddFunction( &ScriptExe::_ClearImage3,				"ClearImage",				{},		"Pass to clear int-color image." );
		se->AddFunction( &ScriptExe::_ClearBuffer1,				"ClearBuffer",				{},		"Pass to clear buffer." );
		se->AddFunction( &ScriptExe::_ClearBuffer2,				"ClearBuffer",				{"buffer", "offset", "size", "value"} );

		se->AddFunction( &ScriptExe::_ExportImage,				"Export",					{"image", "prefix"},	"Readback the image and save it to a file in DDS format. Rendering will be paused until the readback is completed." );
		se->AddFunction( &ScriptExe::_DbgExportBuffer,			"DbgExport",				{"buffer", "prefix"},	"Readback the buffer and save it to a file in structured format. Rendering will be paused until the readback is completed." );
		se->AddFunction( &ScriptExe::_ExportBuffer,				"Export",					{"buffer", "prefix"},	"Readback the buffer and save it to a file in binary format. Rendering will be paused until the readback is completed." );
	//	se->AddFunction( &ScriptExe::_ExportGeometry,			"Export",					{"geometry", "prefix"},	"Readback the geometry data (images, buffers, etc) and save it to a file in glTF format. Rendering will be paused until the readback is completed." );

		se->AddFunction( &ScriptExe::_BuildRTGeometry,			"BuildRTGeometry",			{},		"Pass to build RTGeometry, executed every frame."			);
		se->AddFunction( &ScriptExe::_BuildRTGeometryIndirect,	"BuildRTGeometryIndirect",	{},		"Pass to indirect build RTGeometry, executed every frame."	);

		se->AddFunction( &ScriptExe::_BuildRTScene,				"BuildRTScene",				{},		"Pass to build RTScene, executed every frame."				);
		se->AddFunction( &ScriptExe::_BuildRTSceneIndirect,		"BuildRTSceneIndirect",		{},		"Pass to indirect build RTScene, executed every frame."		);

		se->AddFunction( &ScriptExe::_GetCube2,					"GetCube",					{"positions", "normals", "indices"} );
		se->AddFunction( &ScriptExe::_GetCube3,					"GetCube",					{"positions", "normals", "tangents", "bitangents", "texcoords2d", "indices"} );
		se->AddFunction( &ScriptExe::_GetCube4,					"GetCube",					{"positions", "normals", "tangents", "bitangents", "cubemapTexcoords", "indices"} );
		se->AddFunction( &ScriptExe::_GetGrid1,					"GetGrid",					{"size", "unorm2Positions", "indices"},					"Returns (size * size) grid" );
		se->AddFunction( &ScriptExe::_GetGrid2,					"GetGrid",					{"size", "unorm3Positions", "indices"},					"Returns (size * size) grid in XY space." );
		se->AddFunction( &ScriptExe::_GetSphere1,				"GetSphere",				{"lod", "positions", "indices"},						"Returns spherical cube" );
		se->AddFunction( &ScriptExe::_GetSphere2,				"GetSphere",				{"lod", "positions", "cubemapTexcoords", "indices"},	"Returns spherical cube" );
		se->AddFunction( &ScriptExe::_GetSphere3,				"GetSphere",				{"lod", "positions", "normals", "tangents", "bitangents", "cubemapTexcoords", "indices"},	"Returns spherical cube with tangential projection for cubemap." );
		se->AddFunction( &ScriptExe::_GetSphere4,				"GetSphere",				{"lod", "positions", "normals", "tangents", "bitangents", "texcoords2d", "indices"},		"Returns spherical cube" );
		se->AddFunction( &ScriptExe::_GetCylinder1,				"GetCylinder",				{"segmentCount", "isInner", "positions", "texcoords", "indices"},			"Returns cylinder" );
		se->AddFunction( &ScriptExe::_GetCylinder2,				"GetCylinder",				{"segmentCount", "isInner", "positions", "normals", "tangents", "bitangents", "texcoords", "indices"},	"Returns cylinder" );

		se->AddFunction( &ScriptExe::_GetSphericalCube1,		"GetSphericalCube",			{"lod", "positions", "indices"},						"Returns spherical cube without projection and rotation" );

		se->AddFunction( &ScriptExe::_IndicesToPrimitives,		"IndicesToPrimitives",		{"indices", "primitives"},		"Helper function to convert array of indices to array of uint3 indices per triangle" );
		se->AddFunction( &ScriptExe::_GetFrustumPlanes,			"GetFrustumPlanes",			{"viewProj", "outPlanes"},		"Helper function to convert matrix to 6 planes of the frustum." );
		se->AddFunction( &ScriptExe::_MergeMesh,				"MergeMesh",				{"srcIndices", "srcVertexCount", "indicesToAdd"} );

		se->AddFunction( &ScriptExe::_ExtrudeAndMerge,			"Extrude",					{"lineStrip", "height", "positions", "indices"},					"Output is a TriangleList, front face: CCW" );
		se->AddFunction( &ScriptExe::_TriangulateAndMerge1,		"Triangulate",				{"lineStrip", "yCoord", "positions", "indices"},					"Output is a TriangleList, front face: CCW" );
		se->AddFunction( &ScriptExe::_TriangulateAndMerge2,		"Triangulate",				{"vertices", "lineListIndices", "yCoord", "positions", "indices"},	"Output is a TriangleList, front face: CCW" );
		se->AddFunction( &ScriptExe::_TriangulateExtrudeAndMerge1,"TriangulateAndExtrude",	{"lineStrip", "height", "positions", "indices"},					"Output is a TriangleList, front face: CCW" );
		se->AddFunction( &ScriptExe::_TriangulateExtrudeAndMerge2,"TriangulateAndExtrude",	{"vertices", "lineListIndices", "height", "positions", "indices"},	"Output is a TriangleList, front face: CCW" );

		se->AddFunction( &ScriptExe::_RunScript1,				"RunScript",				{"filePath", "collection"},		"Run script, path to script must be added to 'res_editor_cfg.as' as 'SecondaryScriptDir()'" );
		se->AddFunction( &ScriptExe::_RunScript2,				"RunScript",				{"filePath", "flags", "collection"} );

		se->AddFunction( &ScriptExe::_SliderI0,					"Slider",					{"dyn", "name"} );
		se->AddFunction( &ScriptExe::_SliderI1,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderI2,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderI3,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderI4,					"Slider",					{"dyn", "name", "min", "max"},		"Add slider to UI." );

		se->AddFunction( &ScriptExe::_SliderI1a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderI2a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderI3a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderI4a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );

		se->AddFunction( &ScriptExe::_SliderU0,					"Slider",					{"dyn", "name"} );
		se->AddFunction( &ScriptExe::_SliderU1,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderU2,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderU3,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderU4,					"Slider",					{"dyn", "name", "min", "max"} );

		se->AddFunction( &ScriptExe::_SliderU1a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderU2a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderU3a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderU4a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );

		se->AddFunction( &ScriptExe::_SliderF0,					"Slider",					{"dyn", "name"} );
		se->AddFunction( &ScriptExe::_SliderF1,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderF2,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderF3,					"Slider",					{"dyn", "name", "min", "max"} );
		se->AddFunction( &ScriptExe::_SliderF4,					"Slider",					{"dyn", "name", "min", "max"} );

		se->AddFunction( &ScriptExe::_SliderF1a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderF2a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderF3a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );
		se->AddFunction( &ScriptExe::_SliderF4a,				"Slider",					{"dyn", "name", "min", "max", "initial"} );

		se->AddFunction( &ScriptExe::_WhiteColorSpectrum3,			"WhiteColorSpectrum3",			{"wavelengthToRGB"} );
		se->AddFunction( &ScriptExe::_WhiteColorSpectrum7,			"WhiteColorSpectrum7",			{"wavelengthToRGB", "normalized"} );
		se->AddFunction( &ScriptExe::_WhiteColorSpectrumStep50nm,	"WhiteColorSpectrumStep50nm",	{"wavelengthToRGB", "normalized"} );
		se->AddFunction( &ScriptExe::_WhiteColorSpectrumStep100nm,	"WhiteColorSpectrumStep100nm",	{"wavelengthToRGB", "normalized"} );

		se->AddFunction( &ScriptExe::_CM_CubeSC_Forward,		"CM_CubeSC_Forward",		{} );
		se->AddFunction( &ScriptExe::_CM_IdentitySC_Forward,	"CM_IdentitySC_Forward",	{} );
		se->AddFunction( &ScriptExe::_CM_TangentialSC_Forward,	"CM_TangentialSC_Forward",	{} );

		se->AddFunction( &_Supported_GeometryShader,			"Supported_GeometryShader",		{} );
		se->AddFunction( &_Supported_TessellationShader,		"Supported_TessellationShader",	{} );
		se->AddFunction( &_Supported_SamplerAnisotropy,			"Supported_SamplerAnisotropy",	{} );
		se->AddFunction( &_Supported_DepthFormat,				"Supported_DepthFormat",		{} );
		se->AddFunction( &_Supported_DepthStencilFormat,		"Supported_DepthStencilFormat",	{} );

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

		StaticAssert( (sizeof(SamplerConsts) / sizeof(String)) == 16 );
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
	RTechInfo  ScriptExe::ScriptPassApi::ConvertAndLoad (Function<void (ScriptEnginePtr)> fn) __Th___
	{
		CHECK_THROW( s_scriptExe != null );

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
			});
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
//-----------------------------------------------------------------------------



/*
=================================================
	_RunWithPipelineCompiler
=================================================
*/
	void  ScriptExe::_RunWithPipelineCompiler (Function<void ()> fn) __Th___
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

				obj_storage.spirvCompiler		= MakeUnique<SpirvCompiler>( _GetTempData().cfg.includeDirs );
				obj_storage.spirvCompiler->SetDefaultResourceLimits();

				ObjectStorage::SetInstance( &obj_storage );

				ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ obj_storage.defaultFeatureSet }};
				fs->fs = ScriptResourceApi::GetFeatureSet();

				PipelineCompiler::ScriptConfig	cfg;
				cfg.SetTarget( ECompilationTarget::Vulkan );
				cfg.SetShaderVersion( EShaderVersion(Version2::From100( fs->fs.maxShaderVersion.spirv ).ToHex()) | EShaderVersion::_SPIRV );
				cfg.SetShaderOptions( EShaderOpt::Optimize );
				cfg.SetDefaultLayout( EStructLayout::Std140 );
				cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );

			  #if PIPELINE_STATISTICS
				cfg.SetPipelineOptions( EPipelineOpt::Optimize | EPipelineOpt::CaptureStatistics | EPipelineOpt::CaptureInternalRepresentation );
			  #else
				cfg.SetPipelineOptions( EPipelineOpt::Optimize );
			  #endif
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

		StaticAssert( (sizeof(SamplerConsts) / sizeof(String)) == 16 );
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
			st->Set( EStructLayout::Std140, R"#(
					float4x4	viewProj;
					float4x4	invViewProj;
					float4x4	proj;
					float4x4	view;
					float3		pos;
					float2		clipPlanes;
					float		zoom;
					float4		frustum [6];
				)#");
		}

		/*if ( not obj_storage.structTypes.contains( "CameraSet" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"CameraSet"}};
			st->Set( EStructLayout::Std140, R"#(
					float		ipd;		// for VR video
					float3		globalPos;	// actual position: 'globalPos + data[0].localPos'
					uint		count;
					CameraData	data [4];
				)#");
		}*/

		if ( not obj_storage.structTypes.contains( "AccelStructInstance" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"AccelStructInstance"}};
			st->Set( EStructLayout::Std430, R"#(
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
			st->Set( EStructLayout::Std430, R"#(
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
			st->Set( EStructLayout::Std430, R"#(
					packed_uint3	dim;
				)#");
			CHECK( st->StaticSize() == SizeOf<TraceRayIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "DispatchIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"DispatchIndirectCommand"}};
			st->Set( EStructLayout::Std430, R"#(
					packed_uint3	groupCount;
				)#");
			CHECK( st->StaticSize() == SizeOf<DispatchIndirectCommand> );
		}

		if ( not obj_storage.structTypes.contains( "DrawIndirectCommand" ))
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"DrawIndirectCommand"}};
			st->Set( EStructLayout::Std430, R"#(
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
			st->Set( EStructLayout::Std430, R"#(
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
			st->Set( EStructLayout::Std430, R"#(
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
		const auto	rhs_t				= GetDescriptorImageTypeRelaxed( rhs.PixelFormat(), rhs.GetType(), false );
		return lhs_t0 == rhs_t or lhs_t1 == rhs_t;
	}

} // AE::ResEditor
