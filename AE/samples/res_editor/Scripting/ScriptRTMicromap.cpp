// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptRTMicromap*  ScriptRTMicromap_Ctor1 (EMicromapType type) {
		return ScriptRTMicromapPtr{ new ScriptRTMicromap{ type }}.Detach();
	}

} // namespace

/*
=================================================
	constructor
=================================================
*/
	ScriptRTMicromap::ScriptRTMicromap (EMicromapType type) __Th___ :
		_type{ type }
	{
		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		switch_enum( _type )
		{
			case EMicromapType::Opacity :
				CHECK_THROW_MSG( fs.opacityMicromap == FeatureSet::EFeature::RequireTrue, "OpacityMicromap is not supported" );
				break;

			case EMicromapType::Displacement :
				CHECK_THROW_MSG( fs.displacementMicromap == FeatureSet::EFeature::RequireTrue, "DisplacementMicromap is not supported" );
				break;

			case EMicromapType::_Count :
			default :
				CHECK_THROW_MSG( false, "unknown EMicromapType" );
		}
		switch_end
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptRTMicromap::~ScriptRTMicromap ()
	{
		if ( not _resource )
			AE_LOGW( "Unused RTGeometry '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptRTMicromap::Name (const String &name) __Th___
	{
		_MutableResource();

		_dbgName = name;
	}

/*
=================================================
	AllowUpdate
=================================================
*/
	void  ScriptRTMicromap::AllowUpdate () __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );

		_allowUpdate = true;
	}

/*
=================================================
	AddTriangleType
=================================================
*/
	uint  ScriptRTMicromap::AddTriangleType (uint triangleCount, uint subdivisionLevel, EOpacityMicromapFormat format) __Th___
	{
		_MutableResource();
		CHECK_THROW_MSG( _type == EMicromapType::Opacity );
		CHECK_THROW_MSG( triangleCount > 0 );

		auto&	fs = GraphicsScheduler().GetFeatureSet();

		ulong	bit_count = 0;
		switch_enum( format )
		{
			case EOpacityMicromapFormat::TwoState :
				CHECK_THROW_MSG( subdivisionLevel <= fs.maxOpacity2StateSubdivisionLevel,
					"subdivisionLevel ("s << ToString( subdivisionLevel ) << ") must be <= than maxOpacity2StateSubdivisionLevel (" <<
					ToString( fs.maxOpacity2StateSubdivisionLevel ) << ")." );
				bit_count = 1;
				break;

			case EOpacityMicromapFormat::FourState :
				CHECK_THROW_MSG( subdivisionLevel <= fs.maxOpacity4StateSubdivisionLevel,
					"subdivisionLevel ("s << ToString( subdivisionLevel ) << ") must be <= than maxOpacity4StateSubdivisionLevel (" <<
					ToString( fs.maxOpacity4StateSubdivisionLevel ) << ")." );
				bit_count = 2;
				break;

			case EOpacityMicromapFormat::Unknown :
			case EOpacityMicromapFormat::_Count :
			default :
				CHECK_THROW_MSG( false, "unknown opacity format" );
		}
		switch_end

		bit_count *= triangleCount;
		bit_count *= IPow( 4u, subdivisionLevel );

		auto&	dst = _usage.emplace_back();
		dst.triangleCount		= triangleCount;
		dst.subdivisionLevel	= subdivisionLevel;
		dst.format.opacity		= format;

		return uint(DivCeil( bit_count, 8 ));
	}

/*
=================================================
	SetData
=================================================
*/
	void  ScriptRTMicromap::SetData1 (const ScriptBufferPtr &buf) __Th___
	{
		SetData2( buf, 0 );
	}

	void  ScriptRTMicromap::SetData2 (const ScriptBufferPtr &buf, uint offset) __Th___
	{
		CHECK_THROW_MSG( not _dataBuffer, "already defined" );
		CHECK_THROW_MSG( buf );

		buf->AddUsage( EResourceUsage::MMBuild );

		_dataBuffer			= buf;
		_dataBufferOffset	= Bytes{offset};
	}

/*
=================================================
	SetTriangles
=================================================
*/
	void  ScriptRTMicromap::SetTriangles1 (const ScriptBufferPtr &buf) __Th___
	{
		SetTriangles2( buf, 0 );
	}

	void  ScriptRTMicromap::SetTriangles2 (const ScriptBufferPtr &buf, uint offset) __Th___
	{
		CHECK_THROW_MSG( not _triangleArray, "already defined" );
		CHECK_THROW_MSG( buf );

		buf->AddUsage( EResourceUsage::MMBuild );

		_triangleArray			= buf;
		_triangleArrayOffset	= Bytes{offset};
	}

/*
=================================================
	_MutableResource
=================================================
*/
	void  ScriptRTMicromap::_MutableResource () C_Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );
	//	CHECK_THROW_MSG( not _immutableGeom,
	//		"RTMicromap is immutable" );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRTMicromap::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptRTMicromap>	binder{ se };
		binder.CreateRef( 0, False{"no factory"} );
		binder.AddFactoryCtor( &ScriptRTMicromap_Ctor1,		{"type"} );

		binder.Comment( "Set resource name. It is used for debugging." );
		AS_METHOD( binder, ScriptRTMicromap::Name,					"Name",					{} );

		binder.Comment( "Add opacity triangle type.\n"
						"Returns required opacity data size in bytes." );
		AS_METHOD( binder, ScriptRTMicromap::AddTriangleType,		"AddTriangleType",		{"totalTriangleCount", "subdivisionLevel", "format"} );

		binder.Comment( "Set opacity/displacement data buffer.\n"
						"Required data size is sum of sizes returned by 'AddTriangleType()'." );
		AS_METHOD( binder, ScriptRTMicromap::SetData1,				"SetData",				{"buffer"} );
		AS_METHOD( binder, ScriptRTMicromap::SetData2,				"SetData",				{"buffer", "offset"} );

		binder.Comment( "Set triangle array buffer.\n"
						"Must be array of 'MicromapTriangle' which is defined in 'IndirectCmd.glsl'." );
		AS_METHOD( binder, ScriptRTMicromap::SetTriangles1,			"SetTriangles",			{"buffer"} );
		AS_METHOD( binder, ScriptRTMicromap::SetTriangles2,			"SetTriangles",			{"buffer", "offset"} );
	}

/*
=================================================
	ToResource
=================================================
*/
	RC<RTMicromap>  ScriptRTMicromap::ToResource () __Th___
	{
		if ( _resource )
			return _resource;

		CHECK_THROW_MSG( _dataBuffer, "data buffer is not defined, see 'SetData()'." );
		CHECK_THROW_MSG( _triangleArray, "triangle array buffer is not defined, see 'SetTriangles()'." );

		Renderer&	renderer = ScriptExe::ScriptResourceApi::GetRenderer(); // throw

		// TODO: dummy

		RTMicromap::BuildData	desc;
		desc.type				= _type;
		desc.buildFlags			= EBuildMicromapFlags::PreferFastBuild;
		desc.usage				= _usage;
		desc.data				= _dataBuffer->ToResource();  // throw
		desc.dataOffset			= _dataBufferOffset;
		desc.triangleArray		= _triangleArray->ToResource();  // throw
		desc.triangleArrayOffset= _triangleArrayOffset;

		_resource = RTMicromap::Create( RVRef(desc), renderer, _dbgName, Bool{_allowUpdate} );  // throw
		return _resource;
	}

} // AE::ResEditor
