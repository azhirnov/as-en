// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RASBuildContext.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RASBuildContext::RASBuildContext (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), dbg, ECtxType::ASBuild }
	{}

/*
=================================================
	Build
=================================================
*/
	void  RASBuildContext::Build (const RTGeometryBuild &build, RTGeometryID dstId) __Th___
	{
		auto&	dst = _GetResourcesOrThrow( dstId );

		VALIDATE_GCTX( Build( dst.Description(), _GetResourcesOrThrow(build.scratch.id).Description(), build.scratch.offset ));

		RTempLinearAllocator						alloc;
		Msg::CmdBuf_Bake::ASBuild_BuildGeometryCmd	cmd;
		cmd.build	= build;
		cmd.dstId	= dst.Handle();
		CHECK_THROW( RRTGeometry::ConvertBuildInfo( _mngr.GetResourceManager(), INOUT cmd.build, alloc ));
		_cmdbuf->AddCommand( cmd );
	}

	void  RASBuildContext::Build (const RTSceneBuild &build, RTSceneID dstId) __Th___
	{
		auto&	dst = _GetResourcesOrThrow( dstId );

		VALIDATE_GCTX( Build( dst.Description(),
							  _GetResourcesOrThrow(build.scratch.id).Description(), build.scratch.offset,
							  _GetResourcesOrThrow(build.instanceData.id).Description(), build.instanceData.offset ));

		Msg::CmdBuf_Bake::ASBuild_BuildSceneCmd  cmd;
		cmd.build	= build;
		cmd.dstId	= dst.Handle();
		CHECK_THROW( RRTScene::ConvertBuildInfo( _mngr.GetResourceManager(), INOUT cmd.build ));
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	Update
=================================================
*/
	void  RASBuildContext::Update (const RTGeometryBuild &build, RTGeometryID srcId, RTGeometryID dstId) __Th___
	{
		auto	[src, dst] = _GetResourcesOrThrow( srcId, dstId );

		VALIDATE_GCTX( Update(	src.Description(), dst.Description(),
								_GetResourcesOrThrow(build.scratch.id).Description(), build.scratch.offset ));

		RTempLinearAllocator						alloc;
		Msg::CmdBuf_Bake::ASBuild_UpdateGeometryCmd	cmd;
		cmd.build	= build;
		cmd.srcId	= src.Handle();
		cmd.dstId	= dst.Handle();
		CHECK_THROW( RRTGeometry::ConvertBuildInfo( _mngr.GetResourceManager(), INOUT cmd.build, alloc ));
		_cmdbuf->AddCommand( cmd );
	}

	void  RASBuildContext::Update (const RTSceneBuild &build, RTSceneID srcId, RTSceneID dstId) __Th___
	{
		auto	[src, dst] = _GetResourcesOrThrow( srcId, dstId );

		VALIDATE_GCTX( Update(	src.Description(), dst.Description(),
								_GetResourcesOrThrow(build.scratch.id).Description(), build.scratch.offset,
								_GetResourcesOrThrow(build.instanceData.id).Description(), build.instanceData.offset ));

		Msg::CmdBuf_Bake::ASBuild_UpdateSceneCmd  cmd;
		cmd.build	= build;
		cmd.srcId	= src.Handle();
		cmd.dstId	= dst.Handle();
		CHECK_THROW( RRTScene::ConvertBuildInfo( _mngr.GetResourceManager(), INOUT cmd.build ));
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	Copy
=================================================
*/
	void  RASBuildContext::Copy (RTGeometryID srcId, RTGeometryID dstId, ERTASCopyMode mode) __Th___
	{
		auto	[src, dst] = _GetResourcesOrThrow( srcId, dstId );

		VALIDATE_GCTX( Copy( src.Description(), dst.Description(), mode ));

		Msg::CmdBuf_Bake::ASBuild_CopyGeometryCmd  cmd;
		cmd.srcId	= src.Handle();
		cmd.dstId	= dst.Handle();
		cmd.mode	= mode;
		_cmdbuf->AddCommand( cmd );
	}

	void  RASBuildContext::Copy (RTSceneID srcId, RTSceneID dstId, ERTASCopyMode mode) __Th___
	{
		auto	[src, dst] = _GetResourcesOrThrow( srcId, dstId );

		VALIDATE_GCTX( Copy( src.Description(), dst.Description(), mode ));

		Msg::CmdBuf_Bake::ASBuild_CopySceneCmd  cmd;
		cmd.srcId	= src.Handle();
		cmd.dstId	= dst.Handle();
		cmd.mode	= mode;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	WriteProperty
=================================================
*/
	void  RASBuildContext::WriteProperty (ERTASProperty property, RTGeometryID asId, BufferID dstBufferId, Bytes offset, Bytes size) __Th___
	{
		auto	[as, buf] = _GetResourcesOrThrow( asId, dstBufferId );

		//VALIDATE_GCTX( WriteProperty( property, buf.Description(), offset, size ));	// TODO

		return WriteProperty( property, as.Handle(), buf.Handle(), offset, size );
	}

	void  RASBuildContext::WriteProperty (ERTASProperty property, RmRTGeometryID as, RmBufferID dstBuffer, Bytes offset, Bytes size) __Th___
	{
		Msg::CmdBuf_Bake::ASBuild_WriteGeometryPropertyCmd  cmd;
		cmd.property	= property;
		cmd.as			= as;
		cmd.dstBuffer	= dstBuffer;
		cmd.offset		= offset;
		cmd.size		= size;
		_cmdbuf->AddCommand( cmd );
	}

	void  RASBuildContext::WriteProperty (ERTASProperty property, RTSceneID asId, BufferID dstBufferId, Bytes offset, Bytes size) __Th___
	{
		auto	[as, buf] = _GetResourcesOrThrow( asId, dstBufferId );

		//VALIDATE_GCTX( WriteProperty( property, buf.Description(), offset, size ));	// TODO

		return WriteProperty( property, as.Handle(), buf.Handle(), offset, size );
	}

	void  RASBuildContext::WriteProperty (ERTASProperty property, RmRTSceneID as, RmBufferID dstBuffer, Bytes offset, Bytes size) __Th___
	{
		Msg::CmdBuf_Bake::ASBuild_WriteScenePropertyCmd  cmd;
		cmd.property	= property;
		cmd.as			= as;
		cmd.dstBuffer	= dstBuffer;
		cmd.offset		= offset;
		cmd.size		= size;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BuildIndirect
=================================================
*/
	void  RASBuildContext::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer, Bytes indirectStride) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::BuildIndirect ));
		UNTESTED;
	}

	void  RASBuildContext::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, BufferID indirectBuffer, Bytes indirectBufferOffset, Bytes indirectStride) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::BuildIndirect ));
		UNTESTED;
	}

	void  RASBuildContext::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::BuildIndirect ));
		UNTESTED;
	}

	void  RASBuildContext::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::BuildIndirect ));
		UNTESTED;
	}

/*
=================================================
	SerializeToMemory
=================================================
*/
	void  RASBuildContext::SerializeToMemory (RTGeometryID src, DeviceAddress dst) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::SerializeToMemory ));
		UNTESTED;
	}

	void  RASBuildContext::SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::SerializeToMemory ));
		UNTESTED;
	}

	void  RASBuildContext::SerializeToMemory (RTSceneID src, DeviceAddress dst) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::SerializeToMemory ));
		UNTESTED;
	}

	void  RASBuildContext::SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::SerializeToMemory ));
		UNTESTED;
	}

/*
=================================================
	DeserializeFromMemory
=================================================
*/
	void  RASBuildContext::DeserializeFromMemory (DeviceAddress src, RTGeometryID dst) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::DeserializeFromMemory ));
		UNTESTED;
	}

	void  RASBuildContext::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::DeserializeFromMemory ));
		UNTESTED;
	}

	void  RASBuildContext::DeserializeFromMemory (DeviceAddress src, RTSceneID dst) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::DeserializeFromMemory ));
		UNTESTED;
	}

	void  RASBuildContext::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::DeserializeFromMemory ));
		UNTESTED;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
