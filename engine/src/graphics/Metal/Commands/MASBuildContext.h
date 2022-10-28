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
		void  WriteCompactedSize (MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);

	protected:
		explicit _MDirectASBuildCtx (Ptr<MCommandBatch> batch);
		_MDirectASBuildCtx (Ptr<MCommandBatch> batch, MCommandBuffer cmdbuf);
		
		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
		void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);
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
		void  WriteCompactedSize (MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);


	protected:
		explicit _MIndirectASBuildCtx (Ptr<MCommandBatch> batch) : MBaseIndirectContext{ batch } {}
		_MIndirectASBuildCtx (Ptr<MCommandBatch> batch, MSoftwareCmdBufPtr cmdbuf) : MBaseIndirectContext{ batch, RVRef(cmdbuf) } {}

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);
		
		void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
		void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);
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
		explicit _MASBuildContextImpl (Ptr<MCommandBatch> batch) : RawCtx{ batch } {}
		
		template <typename RawCmdBufType>
		_MASBuildContextImpl (Ptr<MCommandBatch> batch, RawCmdBufType cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_MASBuildContextImpl () = delete;
		_MASBuildContextImpl (const _MASBuildContextImpl &) = delete;

		using RawCtx::Copy;
		
		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst) override final								{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) override final			{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) override final;
		
		void  Build  (const RTSceneBuild &cmd, RTSceneID dst) override final									{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) override final						{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) override final;
		
		using RawCtx::WriteCompactedSize;

		void  WriteCompactedSize (RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size) override final;
		void  WriteCompactedSize (RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size) override final;

		Promise<Bytes>  ReadCompactedSize (RTGeometryID as) override final;
		Promise<Bytes>  ReadCompactedSize (RTSceneID as) override final;

		ND_ Promise<Bytes>  ReadCompactedSize (MetalAccelStruct as);

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }

		ND_ AccumBar  AccumBarriers ()							{ return AccumBar{ *this }; }

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
		auto*	src_geom = this->_mngr.Get( src );
		auto*	dst_geom = this->_mngr.Get( dst );

		CHECK_ERRV( src_geom != null and
					dst_geom != null );
		
		BEGIN_ENUM_CHECKS();
		switch ( mode )
		{
			case ERTASCopyMode::Clone :		return Copy( src_geom->Handle(), dst_geom->Handle() );
			case ERTASCopyMode::Compaction:	return CopyCompacted( src_geom->Handle(), dst_geom->Handle() );
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
		auto*	src_scene = this->_mngr.Get( src );
		auto*	dst_scene = this->_mngr.Get( dst );

		CHECK_ERRV( src_scene != null and
					dst_scene != null );
		
		BEGIN_ENUM_CHECKS();
		switch ( mode )
		{
			case ERTASCopyMode::Clone :		return Copy( src_scene->Handle(), dst_scene->Handle() );
			case ERTASCopyMode::Compaction:	return CopyCompacted( src_scene->Handle(), dst_scene->Handle() );
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown RT AS copy mode", void() );
	}

/*
=================================================
	WriteCompactedSize
=================================================
*/
	template <typename C>
	void  _MASBuildContextImpl<C>::WriteCompactedSize (RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto*	src_as	= this->_mngr.Get( as );
		auto*	dst_buf	= this->_mngr.Get( dstBuffer );

		CHECK_ERRV( src_as != null and dst_buf != null );

		RawCtx::WriteCompactedSize( src_as->Handle(), dst_buf->Handle(), offset, size );
	}
	
	template <typename C>
	void  _MASBuildContextImpl<C>::WriteCompactedSize (RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto*	src_as	= this->_mngr.Get( as );
		auto*	dst_buf	= this->_mngr.Get( dstBuffer );

		CHECK_ERRV( src_as != null and dst_buf != null );

		RawCtx::WriteCompactedSize( src_as->Handle(), dst_buf->Handle(), offset, size );
	}
	
/*
=================================================
	ReadCompactedSize
=================================================
*/
	template <typename C>
	Promise<Bytes>  _MASBuildContextImpl<C>::ReadCompactedSize (RTGeometryID as)
	{
		auto*	src_as	= this->_mngr.Get( as );
		CHECK_ERR( src_as != null );

		return ReadCompactedSize( src_as->Handle() );
	}
	
	template <typename C>
	Promise<Bytes>  _MASBuildContextImpl<C>::ReadCompactedSize (RTSceneID as)
	{
		auto*	src_as	= this->_mngr.Get( as );
		CHECK_ERR( src_as != null );

		return ReadCompactedSize( src_as->Handle() );
	}
	
	template <typename C>
	Promise<Bytes>  _MASBuildContextImpl<C>::ReadCompactedSize (MetalAccelStruct as)
	{
		// TODO
		return Default;
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
