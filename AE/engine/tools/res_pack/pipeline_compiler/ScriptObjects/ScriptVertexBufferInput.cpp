// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptVertexBufferInput.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
	static ScriptVertexBufferInput*  ScriptVertexBufferInput_Ctor (const String &name) {
		return VertexBufferInputPtr{ new ScriptVertexBufferInput{ name }}.Detach();
	}

	static void  VB_SameAttribs (const String &lhs, const String &rhs) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		auto	lhs_it	= storage.vbInputMap.find( lhs );
		auto	rhs_it	= storage.vbInputMap.find( rhs );

		CHECK_THROW_MSG( lhs_it != storage.vbInputMap.end(), "Vertex buffer with name '"s << lhs << "' is not exists" );
		CHECK_THROW_MSG( rhs_it != storage.vbInputMap.end(), "Vertex buffer with name '"s << rhs << "' is not exists" );

		CHECK_THROW_MSG( lhs_it->second->IsSameAttribs( *rhs_it->second ),
			"Vertex buffer layouts are not equal ('"s << lhs << "' != '" << rhs << "')" );
	}

	static void  VertexDivisor_Ctor (OUT void* mem, uint value) {
		PlacementNew<VertexDivisor>( OUT mem, value );
	}

} // namespace


/*
=================================================
	constructor
=================================================
*/
	ScriptVertexBufferInput::ScriptVertexBufferInput (const String &name) :
		_features{ ObjectStorage::Instance()->GetDefaultFeatureSets() },
		_name{ name }
	{
		auto&	map = ObjectStorage::Instance()->vbInputMap;
		CHECK_THROW_MSG( map.emplace( name, VertexBufferInputPtr{this} ).second );
	}

/*
=================================================
	AddFeatureSet
=================================================
*/
	void  ScriptVertexBufferInput::AddFeatureSet (const String &name) __Th___
	{
		auto&	storage = *ObjectStorage::Instance();
		auto	fs_it	= storage.featureSets.find( FeatureSetName{name} );
		CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
			"FeatureSet with name '"s << name << "' is not found" );

		_features.push_back( fs_it->second );
	}

/*
=================================================
	Add
=================================================
*/
	void  ScriptVertexBufferInput::Add1 (const String &bufferName, const String &typeName, uint stride) __Th___
	{
		const auto&	struct_types = ObjectStorage::Instance()->structTypes;

		auto	it = struct_types.find( typeName );
		CHECK_THROW_MSG( it != struct_types.end(),
			"Typename '"s << typeName << "' is not defined" );

		return Add2( bufferName, it->second, stride );
	}

	void  ScriptVertexBufferInput::Add2 (const String &bufferName, const ShaderStructTypePtr &ptr, uint stride) __Th___
	{
		return _Add( bufferName, ptr, stride, NullOptional );
	}

	void  ScriptVertexBufferInput::Add3 (const String &bufferName, const String &typeName) __Th___
	{
		return Add1( bufferName, typeName, 0 );
	}

	void  ScriptVertexBufferInput::Add4 (const String &bufferName, const ShaderStructTypePtr &ptr) __Th___
	{
		return Add2( bufferName, ptr, 0 );
	}

	void  ScriptVertexBufferInput::Add5 (const String &bufferName, const String &typeName, const Align &align) __Th___
	{
		const auto&	struct_types = ObjectStorage::Instance()->structTypes;

		auto	it = struct_types.find( typeName );
		CHECK_THROW_MSG( it != struct_types.end(),
			"Typename '"s << typeName << "' is not defined" );

		return Add6( bufferName, it->second, align );
	}

	void  ScriptVertexBufferInput::Add6 (const String &bufferName, const ShaderStructTypePtr &ptr, const Align &align) __Th___
	{
		CHECK_THROW_MSG( align.value != 0 );
		CHECK_THROW_MSG( IsPowerOfTwo( align.value ));

		uint	stride = AlignUp( uint(ptr->StaticSize()), align.value );

		return Add2( bufferName, ptr, stride );
	}

/*
=================================================
	Add (with divisor)
=================================================
*/
	void  ScriptVertexBufferInput::AddD1 (const String &bufferName, const String &typeName, uint stride, const VertexDivisor &divisor) __Th___
	{
		const auto&	struct_types = ObjectStorage::Instance()->structTypes;

		auto	it = struct_types.find( typeName );
		CHECK_THROW_MSG( it != struct_types.end(),
			"Typename '"s << typeName << "' is not defined" );

		return AddD2( bufferName, it->second, stride, divisor );
	}

	void  ScriptVertexBufferInput::AddD2 (const String &bufferName, const ShaderStructTypePtr &ptr, uint stride, const VertexDivisor &divisor) __Th___
	{
		return _Add( bufferName, ptr, stride, divisor );
	}

	void  ScriptVertexBufferInput::AddD3 (const String &bufferName, const String &typeName, const VertexDivisor &divisor) __Th___
	{
		return AddD1( bufferName, typeName, 0, divisor );
	}

	void  ScriptVertexBufferInput::AddD4 (const String &bufferName, const ShaderStructTypePtr &ptr, const VertexDivisor &divisor) __Th___
	{
		return AddD2( bufferName, ptr, 0, divisor );
	}

	void  ScriptVertexBufferInput::AddD5 (const String &bufferName, const String &typeName, const Align &align, const VertexDivisor &divisor) __Th___
	{
		const auto&	struct_types = ObjectStorage::Instance()->structTypes;

		auto	it = struct_types.find( typeName );
		CHECK_THROW_MSG( it != struct_types.end(),
			"Typename '"s << typeName << "' is not defined" );

		return AddD6( bufferName, it->second, align, divisor );
	}

	void  ScriptVertexBufferInput::AddD6 (const String &bufferName, const ShaderStructTypePtr &ptr, const Align &align, const VertexDivisor &divisor) __Th___
	{
		CHECK_THROW_MSG( align.value != 0 );
		CHECK_THROW_MSG( IsPowerOfTwo( align.value ));

		uint	stride = AlignUp( uint(ptr->StaticSize()), align.value );

		return AddD2( bufferName, ptr, stride, divisor );
	}

/*
=================================================
	_Add
=================================================
*/
	void  ScriptVertexBufferInput::_Add (const String &bufferName, const ShaderStructTypePtr &ptr, uint stride, Optional<VertexDivisor> divisor) __Th___
	{
		ObjectStorage::Instance()->AddName<VertexBufferName>( bufferName );

		CHECK_THROW_MSG( ptr );
		CHECK_THROW_MSG( not ptr->HasDynamicArray() );
		CHECK_THROW_MSG( _buffers.size() < GraphicsConfig::MaxVertexBuffers, "vertex buffer overflow" );
		CHECK_THROW_MSG( stride == 0 or ptr->StaticSize() <= stride );

		Buffer	buf;
		buf.ptr			= ptr;
		buf.glslLoc		= _glslLoc;
		buf.mslIndex	= _mslIndex;
		buf.index		= CheckCast<ubyte>(_buffers.size());
		buf.stride		= Max( ptr->StaticSize(), Bytes{stride} );
		buf.rate		= (divisor.has_value() ? EVertexInputRate::Instance : EVertexInputRate::Vertex);
		buf.divisor		= (divisor.has_value() ? divisor->value : 0);

		CHECK_THROW_MSG( _buffers.emplace( bufferName, buf ).second,
			"Vertex buffer with name '"s << bufferName << "' already exists" );

		_glsl << "  // " << bufferName << "\n"
			  << ptr->VertexInputToGLSL( "in", INOUT _glslLoc );
		ASSERT( buf.glslLoc < _glslLoc );

		_msl << "  // " << bufferName << "\n"
			 << ptr->VertexInputToMSL( "", INOUT _mslIndex );
		ASSERT( buf.mslIndex < _mslIndex );

		for (auto& fs : ptr->Features()) {
			_features.push_back( fs );
		}
		ScriptFeatureSet::Minimize( INOUT _features );

		// validate
		const uint	attribs = Max( _glslLoc, _mslIndex );

		TestFeature_Min( _features, &FeatureSet::maxVertexAttributes, attribs, "maxVertexAttributes", "attribs" );
		TestFeature_Min( _features, &FeatureSet::maxVertexAttributes, uint(_buffers.size()), "maxVertexBuffers", "buffers" );

		if ( divisor.has_value() )
		{
			CHECK_THROW_MSG( divisor->value != 0 );	// not supported yet
			TEST_FEATURE( _features, vertexDivisor );
		}
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptVertexBufferInput::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			ClassBinder<VertexDivisor>	binder{ se };
			binder.CreateClassValue();
			binder.AddConstructor( &VertexDivisor_Ctor, {} );
		}
		{
			ClassBinder<ScriptVertexBufferInput>	binder{ se };
			binder.CreateRef();

			binder.Comment( "Create vertex buffer.\n"
							"Name is used as typename for vertex data." );
			binder.AddFactoryCtor( &ScriptVertexBufferInput_Ctor, {"name"} );

			binder.Comment( "Add vertex attribute." );
			binder.AddMethod( &ScriptVertexBufferInput::Add1,			"Add",	{"bufferName", "vertexTypeName", "stride"} );
			binder.AddMethod( &ScriptVertexBufferInput::Add2,			"Add",	{"bufferName", "vertexType", "stride"} );
			binder.AddMethod( &ScriptVertexBufferInput::Add3,			"Add",	{"bufferName", "vertexTypeName"} );
			binder.AddMethod( &ScriptVertexBufferInput::Add4,			"Add",	{"bufferName", "vertexType"} );
			binder.AddMethod( &ScriptVertexBufferInput::Add5,			"Add",	{"bufferName", "vertexTypeName", "align"} );
			binder.AddMethod( &ScriptVertexBufferInput::Add6,			"Add",	{"bufferName", "vertexType", "align"} );

			binder.Comment( "Add vertex attribute with divisor." );
			binder.AddMethod( &ScriptVertexBufferInput::AddD1,			"Add",	{"bufferName", "vertexTypeName", "stride", "divisor"} );
			binder.AddMethod( &ScriptVertexBufferInput::AddD2,			"Add",	{"bufferName", "vertexType", "stride", "divisor"} );
			binder.AddMethod( &ScriptVertexBufferInput::AddD3,			"Add",	{"bufferName", "vertexTypeName", "divisor"} );
			binder.AddMethod( &ScriptVertexBufferInput::AddD4,			"Add",	{"bufferName", "vertexType", "divisor"});
			binder.AddMethod( &ScriptVertexBufferInput::AddD5,			"Add",	{"bufferName", "vertexTypeName", "align", "divisor"});
			binder.AddMethod( &ScriptVertexBufferInput::AddD6,			"Add",	{"bufferName", "vertexType", "align", "divisor"});

			binder.Comment( "Add FeatureSet to the vertex buffer." );
			binder.AddMethod( &ScriptVertexBufferInput::AddFeatureSet,	"AddFeatureSet", {"fsName"} );
		}

		se->AddFunction( &VB_SameAttribs, "SameAttribs",	{"lhs", "rhs"}, "Check if two vertex buffers have the same attributes." );
	}

/*
=================================================
	ToGLSL
=================================================
*/
	String  ScriptVertexBufferInput::ToGLSL () const
	{
		return String{_glsl} << "\n";
	}

/*
=================================================
	ToMSL
=================================================
*/
	String  ScriptVertexBufferInput::ToMSL () const
	{
		String	str;
		str << "struct VertexInput\n{\n"
			<< _msl
			<< "};\n\n";
		return str;
	}

/*
=================================================
	Get
=================================================
*/
	void  ScriptVertexBufferInput::_Get (OUT Array<VertexAttrib> &attribs) const
	{
		Array<VertexInput>	temp_vi;

		for (auto& [name, vb] : _buffers)
		{
			uint	loc = vb.glslLoc;
			vb.ptr->GetVertexInput( loc, INOUT temp_vi );
		}

		attribs.resize( temp_vi.size() );

		for (usize i = 0; i < temp_vi.size(); ++i)
		{
			const auto&	src	= temp_vi[i];
			auto&		dst	= attribs[i];

			dst.type	= EVertexType_ToAttribType( src.type );
			dst.index	= src.index;
		}

		std::sort( attribs.begin(), attribs.end(), [](auto& lhs, auto &rhs) { return lhs.index < rhs.index; });
	}

	void  ScriptVertexBufferInput::Get (OUT ArrayView<VertexAttrib> &attribs) C_Th___
	{
		Array<VertexAttrib>	temp;
		_Get( OUT temp );

		auto&	storage = *ObjectStorage::Instance();
		auto*	va_ptr	= storage.allocator.Allocate<VertexAttrib>( temp.size() );

		MemCopy( OUT va_ptr, temp.data(), ArraySizeOf(temp) );
		attribs = ArrayView<VertexAttrib> { va_ptr, temp.size() };
	}

/*
=================================================
	Get
=================================================
*/
	bool  ScriptVertexBufferInput::Get (OUT ArrayView<VertexInput> &vertexInput, OUT ArrayView<VertexBuffer> &vertexBuffers) const
	{
		Array<VertexInput>	temp_vi;
		Array<VertexBuffer>	temp_vb;

		temp_vb.resize( _buffers.size() );

		for (auto& [name, vb] : _buffers)
		{
			uint	loc = vb.glslLoc;
			vb.ptr->GetVertexInput( loc, INOUT temp_vi );
			vb.ptr->AddUsage( ShaderStructType::EUsage::VertexLayout );

			for (auto& vi : temp_vi)
			{
				if ( vi.bufferBinding == UMax )
					vi.bufferBinding = vb.index;
			}

			auto&	dst		= temp_vb[ vb.index ];
			dst.index		= vb.index;
			dst.name		= VertexBufferName::Optimized_t{ name };
			dst.typeName	= ShaderStructName{ vb.ptr->Typename() };
			dst.rate		= vb.rate;
			dst.stride		= vb.stride;
			dst.divisor		= vb.divisor;
		}

		CHECK_ERR( not temp_vi.empty() );
		CHECK_ERR( not temp_vb.empty() );

		auto&	storage = *ObjectStorage::Instance();

		auto*	vi_ptr	= storage.allocator.Allocate<VertexInput>( temp_vi.size() );
		auto*	vb_ptr	= storage.allocator.Allocate<VertexBuffer>( temp_vb.size() );
		CHECK_ERR( vi_ptr != null and vb_ptr != null );

		MemCopy( OUT vi_ptr, temp_vi.data(), ArraySizeOf(temp_vi) );
		MemCopy( OUT vb_ptr, temp_vb.data(), ArraySizeOf(temp_vb) );

		vertexInput		= ArrayView<VertexInput> { vi_ptr, temp_vi.size() };
		vertexBuffers	= ArrayView<VertexBuffer>{ vb_ptr, temp_vb.size() };
		return true;
	}

/*
=================================================
	IsSameAttribs
=================================================
*/
	bool  ScriptVertexBufferInput::IsSameAttribs (const ScriptVertexBufferInput &rhs) const
	{
		Array<VertexAttrib>		lhs_attribs;
		Array<VertexAttrib>		rhs_attribs;
		_Get( OUT lhs_attribs );
		rhs._Get( OUT rhs_attribs );

		return lhs_attribs == rhs_attribs;
	}


} // AE::PipelineCompiler
