// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ASBuildCtx -->  DirectASBuildCtx   --> BarrierMngr --> Metal device 
				\-> IndirectASBuildCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"
# include "graphics/Metal/Resources/MRTGeometry.h"
# include "graphics/Metal/Resources/MRTScene.h"

namespace AE::Graphics::_hidden_
{

	//
	// Metal Direct AS Build Context implementation
	//
	
	class _MDirectASBuildCtx : public MBaseDirectContext
	{
	// variables
	private:
		MetalAccelStructCommandEncoderRC	_encoder;


	// methods
	public:
		void  Copy (MetalAccelStruct src, MetalAccelStruct dst);
		void  CopyCompacted (MetalAccelStruct src, MetalAccelStruct dst);
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ();
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ();

	protected:
		explicit _MDirectASBuildCtx (const RenderTask &task);
		_MDirectASBuildCtx (const RenderTask &task, MCommandBuffer cmdbuf);
		
		ND_ MetalCommandEncoder  _BaseEncoder ()					{ return MetalCommandEncoder{ _encoder.Ptr() }; }
		
		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
		void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);

		void  _WriteCompactedSize (MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);
		
		void  _DebugMarker (NtStringView text, RGBA8u)				{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( _BaseEncoder(), text ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u)			{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( _BaseEncoder(), text ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup( _BaseEncoder() ); }
	};



	//
	// Metal Indirect AS Build Context implementation
	//
	
	class _MIndirectASBuildCtx : public MBaseIndirectContext
	{
	// methods
	public:
		void  Copy (MetalAccelStruct src, MetalAccelStruct dst);
		void  CopyCompacted (MetalAccelStruct src, MetalAccelStruct dst);
		
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		explicit _MIndirectASBuildCtx (const RenderTask &task);
		_MIndirectASBuildCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf);

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);
		
		void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
		void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);

		void  _WriteCompactedSize (MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);
	};


	
	//
	// Metal AS Build Context implementation
	//

	template <typename CtxImpl>
	class _MASBuildContextImpl : public CtxImpl, public IASBuildContext
	{
	// types
	public:
		static constexpr bool	IsASBuildContext		= true;
		static constexpr bool	IsMetalASBuildContext	= true;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumBarriers< _MASBuildContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _MASBuildContextImpl (const RenderTask &task) : RawCtx{ task } {}
		
		template <typename RawCmdBufType>
		_MASBuildContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_MASBuildContextImpl () = delete;
		_MASBuildContextImpl (const _MASBuildContextImpl &) = delete;

		using RawCtx::Copy;
		using RawCtx::CopyCompacted;
		
		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst) override final								{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) override final			{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) override final;
		
		void  Build  (const RTSceneBuild &cmd, RTSceneID dst) override final									{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) override final						{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) override final;

		void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size) override final;
		void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size) override final;

		void  WriteProperty (ERTASProperty property, MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);

		Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as) override final;
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as) override final;

		ND_ Promise<Bytes>  ReadProperty (ERTASProperty property, MetalAccelStruct as);

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectASBuildContext		= _hidden_::_MASBuildContextImpl< _hidden_::_MDirectASBuildCtx >;
	using MIndirectASBuildContext	= _hidden_::_MASBuildContextImpl< _hidden_::_MIndirectASBuildCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
	
/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _MASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode)
	{
		auto  [src_geom, dst_geom]  = _GetResourcesOrThrow( src, dst );
		
		BEGIN_ENUM_CHECKS();
		switch ( mode )
		{
			case ERTASCopyMode::Clone :		return Copy( src_geom.Handle(), dst_geom.Handle() );
			case ERTASCopyMode::Compaction:	return CopyCompacted( src_geom.Handle(), dst_geom.Handle() );
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown RT AS copy mode", void() );
	}
	
/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _MASBuildContextImpl<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode)
	{
		auto  [src_scene, dst_scene]  = _GetResourcesOrThrow( src, dst );
		
		BEGIN_ENUM_CHECKS();
		switch ( mode )
		{
			case ERTASCopyMode::Clone :		return Copy( src_scene.Handle(), dst_scene.Handle() );
			case ERTASCopyMode::Compaction:	return CopyCompacted( src_scene.Handle(), dst_scene.Handle() );
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown RT AS copy mode", void() );
	}

/*
=================================================
	WriteProperty
=================================================
*/
	template <typename C>
	void  _MASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );

		RawCtx::_WriteCompactedSize( src_as.Handle(), dst_buf.Handle(), offset, size );
	}
	
	template <typename C>
	void  _MASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );

		RawCtx::_WriteCompactedSize( src_as.Handle(), dst_buf.Handle(), offset, size );
	}
	
	template <typename C>
	void  _MASBuildContextImpl<C>::WriteProperty (ERTASProperty property, MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size)
	{
		CHECK_ERR( property == ERTASProperty::CompactedSize );

		return RawCtx::_WriteCompactedSize( as, dstBuffer, offset, size );
	}

/*
=================================================
	ReadProperty
=================================================
*/
	template <typename C>
	Promise<Bytes>  _MASBuildContextImpl<C>::ReadProperty (ERTASProperty property, RTGeometryID as)
	{
		auto&	src_as = _GetResourcesOrThrow( as );

		return ReadProperty( property, src_as.Handle() );
	}
	
	template <typename C>
	Promise<Bytes>  _MASBuildContextImpl<C>::ReadProperty (ERTASProperty property, RTSceneID as)
	{
		auto&	src_as = _GetResourcesOrThrow( as );

		return ReadProperty( property, src_as.Handle() );
	}
	
	template <typename C>
	Promise<Bytes>  _MASBuildContextImpl<C>::ReadProperty (ERTASProperty property, MetalAccelStruct as)
	{
		CHECK_ERR( property == ERTASProperty::CompactedSize );

		//  RawCtx::_WriteCompactedSize( as, staging_buffer, staging_buffer_offset, 4_b );

		Unused( as );
		// TODO
		return Default;
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
