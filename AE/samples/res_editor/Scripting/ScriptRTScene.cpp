// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
namespace
{
	static void  RTInstanceCustomIndex_Ctor (OUT void* mem, uint value) {
		CHECK_THROW_MSG( value <= (1<<24), "Only first 24 bits are used" );
		PlacementNew<RTInstanceCustomIndex>( OUT mem, value );
	}

	static void  RTInstanceMask_Ctor (OUT void* mem, uint value) {
		CHECK_THROW_MSG( value < (1<<8), "Only first 8 bits are used" );
		PlacementNew<RTInstanceMask>( OUT mem, value );
	}

	static void  RTInstanceSBTOffset_Ctor (OUT void* mem, uint value) {
		CHECK_THROW_MSG( value <= (1<<24), "Only first 24 bits are used" );
		PlacementNew<RTInstanceSBTOffset>( OUT mem, value );
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	RTInstanceCustomIndex::Bind
=================================================
*/
	void  RTInstanceCustomIndex::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<RTInstanceCustomIndex>	binder{ se };
		binder.CreateClassValue();
		binder.Comment( "Pass custom index to the shader, used first 24 bits.\n"
						"Use 'gl.rayQuery.GetIntersectionInstanceCustomIndex()' or 'gl.InstanceCustomIndex' to get it in shader." );
		binder.AddConstructor( &RTInstanceCustomIndex_Ctor, {} );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	RTInstanceMask::Bind
=================================================
*/
	void  RTInstanceMask::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<RTInstanceMask>		binder{ se };
		binder.CreateClassValue();
		binder.Comment( "Set instance cull mask, used only first 8 bits.\n"
						"In trace ray call: 'if (cullMask_argument & instance_cullMask) != 0' then instance is visible." );
		binder.AddConstructor( &RTInstanceMask_Ctor, {} );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	RTInstanceSBTOffset::Bind
=================================================
*/
	void  RTInstanceSBTOffset::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<RTInstanceSBTOffset>	binder{ se };
		binder.CreateClassValue();
		binder.Comment( "Set shader binding table offset, used first 24 bits.\n"
						"By default SBTOffset is calculated as 'instanceIndex * MaxRayTypes()'." );
		binder.AddConstructor( &RTInstanceSBTOffset_Ctor, {} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptRTScene::ScriptRTScene () __Th___ :
		_instanceBuffer{ new ScriptBuffer{} },
		_dbgName{ "RTScene" }
	{
		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.accelerationStructure() == FeatureSet::EFeature::RequireTrue,
			"RTScene is not supported" );

		_instanceBuffer->Name( "RTScene-Instances" );
		_instanceBuffer->AddUsage( EResourceUsage::ASBuild );
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptRTScene::~ScriptRTScene ()
	{
		if ( not _resource )
			AE_LOGW( "Unused RTScene '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptRTScene::Name (const String &name) __Th___
	{
		_MutableResource();
		_dbgName = name;
	}

/*
=================================================
	EnableHistory
=================================================
*/
	void  ScriptRTScene::EnableHistory () __Th___
	{
		_MutableResource();
		GetInstanceBuffer()->EnableHistory();
		GetIndirectBuffer()->EnableHistory();
	}

/*
=================================================
	AllowUpdate
=================================================
*/
	void  ScriptRTScene::AllowUpdate () __Th___
	{
		_MutableResource();
		_allowUpdate = true;
	}

/*
=================================================
	WithHistory
=================================================
*/
	bool  ScriptRTScene::WithHistory () C_Th___
	{
		uint		with	= 0;
		uint		without	= 0;
		const auto	Test	= [&with, &without] (bool b)
		{{
			if ( b )	++with;
			else		++without;
		}};

		Test( _instanceBuffer->WithHistory() );
		Test( _indirectBuffer->WithHistory() );

		if ( with > 0 )
		{
			CHECK_THROW_MSG( without == 0 );
			return true;
		}
		return false;
	}

/*
=================================================
	_GetInstanceBuffer
=================================================
*/
	ScriptBufferPtr  ScriptRTScene::GetInstanceBuffer () __Th___
	{
		_MutableResource();
		_MakeInstancesImmutable();
		return _instanceBuffer;
	}

	ScriptBuffer*  ScriptRTScene::_GetInstanceBuffer () __Th___
	{
		return ScriptBufferPtr{GetInstanceBuffer()}.Detach();
	}

/*
=================================================
	GetInstanceCount
=================================================
*/
	uint  ScriptRTScene::GetInstanceCount () __Th___
	{
		_MakeInstancesImmutable();
		return uint(_instances.size());
	}

/*
=================================================
	_MakeInstancesImmutable
=================================================
*/
	void  ScriptRTScene::_MakeInstancesImmutable ()
	{
		if ( not _immutableInstances )
		{
			_MutableResource();

			_immutableInstances = true;
			_instanceBuffer->SetArrayLayout1( "AccelStructInstance", uint(_instances.size()) );
		}
	}

/*
=================================================
	_MutableResource
=================================================
*/
	void  ScriptRTScene::_MutableResource () C_Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );
	}

/*
=================================================
	_GetIndirectBuffer
=================================================
*/
	ScriptBufferPtr  ScriptRTScene::GetIndirectBuffer () __Th___
	{
		_MutableResource();

		if ( not _indirectBuffer )
		{
			_indirectBuffer.Set( new ScriptBuffer{} );
			_indirectBuffer->Name( "RTScene-Indirect" );
			_indirectBuffer->AddUsage( EResourceUsage::IndirectBuffer );
			_indirectBuffer->AddUsage( EResourceUsage::ShaderAddress );
			_indirectBuffer->SetLayout1( "ASBuildIndirectCommand" );

			if ( auto& fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
				 fs.accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
			{
				EnableHistory();
			}

			_MakeInstancesImmutable();
		}

		return _indirectBuffer;
	}

	ScriptBuffer*  ScriptRTScene::_GetIndirectBuffer () __Th___
	{
		return ScriptBufferPtr{GetIndirectBuffer()}.Detach();
	}

/*
=================================================
	_AddInstance
=================================================
*/
	void  ScriptRTScene::_AddInstance (Scripting::ScriptArgList args) __Th___
	{
		args.GetObject< ScriptRTScene >()->_AddInstance2( args );
	}

	void  ScriptRTScene::_AddInstance2 (Scripting::ScriptArgList args) __Th___
	{
		_MutableResource();
		CHECK_THROW_MSG( not _immutableInstances,
			"can not add instance when 'InstanceCount()' was used" );

		auto&	dst	= _instances.emplace_back();
		uint	idx	= 0;

		if ( args.IsArg< ScriptRTGeometryPtr const& >(idx) )
		{
			dst.geometry = args.Arg< ScriptRTGeometryPtr const& >(idx++);
			CHECK_THROW_MSG( dst.geometry );
		}

		if ( args.IsArg< ScriptTransform const& >(idx) )
		{
			auto&	tr = args.Arg< ScriptTransform const& >(idx++);

			dst.transform = tr.ToMatrix4x3();
		}
		else
		if ( args.IsArg< packed_float3 const& >(idx) )
		{
			auto&	pos = args.Arg< packed_float3 const& >(idx++);

			dst.transform = float4x3::Identity().SetTranslation( pos );
		}

		if ( args.IsArg< RTInstanceCustomIndex const& >(idx) )
			dst.instanceCustomIndex = args.Arg< RTInstanceCustomIndex const& >(idx++).value;
		else
			dst.instanceCustomIndex = uint(_instances.size()-1);

		if ( args.IsArg< RTInstanceMask const& >(idx) )
			dst.mask = args.Arg< RTInstanceMask const& >(idx++).value;
		else
			dst.mask = UMax;

		if ( args.IsArg< RTInstanceSBTOffset const& >(idx) )
			dst.instanceSBTOffset = args.Arg< RTInstanceSBTOffset const& >(idx++).value;
		else
			dst.instanceSBTOffset = uint(_instances.size()-1) * _maxRayTypes;

		if ( args.IsArg< ERTInstanceOpt >(idx) )
			dst.flags = args.Arg< ERTInstanceOpt >(idx++);

		CHECK_THROW_MSG( idx == args.ArgCount() );
	}

/*
=================================================
	AddInstance
=================================================
*/
	void  ScriptRTScene::AddInstance (const ScriptRTGeometryPtr &geom, const float4x3 &transform,
									  const RTInstanceCustomIndex &customIdx, const RTInstanceMask &mask,
									  const RTInstanceSBTOffset &sbtOffset, ERTInstanceOpt opt) __Th___
	{
		auto&	dst	= _instances.emplace_back();
		dst.geometry			= geom;
		dst.transform			= transform;
		dst.instanceCustomIndex	= customIdx.value;
		dst.mask				= mask.value;
		dst.instanceSBTOffset	= sbtOffset.value;
		dst.flags				= opt;
	}

/*
=================================================
	MaxRayTypes
=================================================
*/
	void  ScriptRTScene::MaxRayTypes (uint value) __Th___
	{
		CHECK_THROW_MSG( _instances.empty(),
			"MaxRayTypes() must be used before any AddInstance() call" );

		_maxRayTypes = value;
	}

/*
=================================================
	HasMicromaps
=================================================
*/
	bool  ScriptRTScene::HasMicromaps () C_NE___
	{
		for (auto& inst : _instances)
		{
			if ( inst.geometry->HasMicromaps() )
				return true;
		}
		return false;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRTScene::Bind (const ScriptEnginePtr &se) __Th___
	{
		RTInstanceCustomIndex::Bind( se );
		RTInstanceMask::Bind( se );
		RTInstanceSBTOffset::Bind( se );

		Scripting::ClassBinder<ScriptRTScene>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set resource name. It is used for debugging." );
		AS_METHOD( binder, ScriptRTScene::Name,		"Name",		{} );

		binder.Comment( "Add instance to the scene." );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &)																						>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceMask &)																>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "mask"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceSBTOffset &)														>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "sbtOffset"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &)														>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &)								>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "mask"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &)							>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "sbtOffset"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "mask", "sbtOffset"} );

		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, ERTInstanceOpt)																						>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceMask &, ERTInstanceOpt)																>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "mask", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceSBTOffset &, ERTInstanceOpt)														>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "sbtOffset", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, ERTInstanceOpt)														>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &, ERTInstanceOpt)								>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "mask", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &, ERTInstanceOpt)							>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "sbtOffset", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &, ERTInstanceOpt)	>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "customIndex", "mask", "sbtOffset", "options"} );

		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &)																						>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceMask &)																>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position", "mask"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceSBTOffset &)															>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position", "sbtOffset"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &)														>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position", "customIndex"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &)								>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position", "customIndex", "mask"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &)							>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position", "customIndex", "sbtOffset"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &)	>( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "position", "customIndex", "mask", "sbtOffset"} );

		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &)																					 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceMask &)															 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "mask"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceSBTOffset &)														 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "sbtOffset"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &)														 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &)								 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "mask"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &)						 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "sbtOffset"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "mask", "sbtOffset"} );

		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, ERTInstanceOpt)																					 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceMask &, ERTInstanceOpt)															 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "mask", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceSBTOffset &, ERTInstanceOpt)														 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "sbtOffset", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, ERTInstanceOpt)														 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, ERTInstanceOpt)								 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "mask", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &, ERTInstanceOpt)						 >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "sbtOffset", "options"} );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const ScriptTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {"rtGeometry", "transform", "customIndex", "mask", "sbtOffset", "options"} );

		binder.Comment( "Returns instance buffer, can be used to update instances in compute shader."
						"Layout: 'AccelStructInstance[]'" );
		AS_METHOD( binder, ScriptRTScene::_GetInstanceBuffer,	"InstanceBuffer",	{} );

		binder.Comment( "Returns number of instances." );
		AS_METHOD( binder, ScriptRTScene::GetInstanceCount,		"InstanceCount",	{} );

		binder.Comment( "Returns indirect buffer, only this buffer must be used for indirect build."
						"Layout: 'ASBuildIndirectCommand'" );
		AS_METHOD( binder, ScriptRTScene::_GetIndirectBuffer,	"IndirectBuffer",	{} );

		binder.Comment( "Set number of ray types. It is used to calculate SBTOffset for instances." );
		AS_METHOD( binder, ScriptRTScene::MaxRayTypes,			"MaxRayTypes",		{} );
	}

/*
=================================================
	ToResource
=================================================
*/
	RC<RTScene>  ScriptRTScene::ToResource () __Th___
	{
		if ( _resource )
			return _resource;

		CHECK_THROW_MSG( not _instances.empty() );
		CHECK_THROW_MSG( _instanceBuffer );

		_MakeInstancesImmutable();

		auto	inst_buf = _instanceBuffer->ToResource();
		CHECK_THROW( inst_buf );

		RC<Buffer>	ind_buf;
		if ( _indirectBuffer )
		{
			ind_buf = _indirectBuffer->ToResource();
			CHECK_THROW( ind_buf );
		}

		RTScene::Instances_t	instances;
		instances.reserve( _instances.size() );

		for (auto& src : _instances)
		{
			auto&	dst = instances.emplace_back();
			dst.geometry			= src.geometry->ToResource();
			dst.transform			= src.transform;
			dst.instanceCustomIndex	= src.instanceCustomIndex;
			dst.mask				= src.mask;
			dst.instanceSBTOffset	= src.instanceSBTOffset;
			dst.flags				= src.flags;
			CHECK_THROW( dst.geometry );
		}

		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer(); // throw
		auto		result		= RTScene::Create( RVRef(instances), inst_buf, ind_buf, renderer, _dbgName, Bool{_allowUpdate} );  // throw

		_resource = result;
		return _resource;
	}


} // AE::ResEditor
