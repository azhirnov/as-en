// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Private/SoftwareCmdBufBase.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Metal/Commands/MRenderTaskScheduler.h"
# include "graphics/Metal/Commands/MBarrierManager.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Metal software Command Buffer
	//

	class MSoftwareCmdBuf final : public SoftwareCmdBufBase
	{
	// types
	public:
		struct DebugMarkerCmd : BaseCmd
		{
			//char		text [];
		};
		
		struct PushDebugGroupCmd : BaseCmd
		{
			//char		text [];
		};
		
		struct PopDebugGroupCmd : BaseCmd
		{};
		
		//-------------------------------------------------
		// transfer commands
		
		struct BeginTransferCommandsCmd : BaseCmd
		{};

		//-------------------------------------------------
		// compute commands
		
		struct BeginComputeCommandsCmd : BaseCmd
		{};

		struct FillBufferCmd : BaseCmd
		{
			MetalBuffer		buffer;
			Bytes			offset;
			Bytes			size;
			uint			data;
		};

		struct CopyBufferCmd : BaseCmd
		{
			MetalBuffer		srcBuffer;
			MetalBuffer		dstBuffer;
			uint			rangesCount;
			//BufferCopy	ranges []
		};

		struct CopyImageCmd : BaseCmd
		{
			MetalImage		srcImage;
			MetalImage		dstImage;
			uint			rangesCount;
			//ImageCopy		ranges []
		};

		struct CopyBufferToImageCmd : BaseCmd
		{
			MetalBuffer			srcBuffer;
			MetalImage			dstImage;
			uint				rangesCount;
			//BufferImageCopy	ranges []
		};

		struct CopyImageToBufferCmd : BaseCmd
		{
			MetalImage			srcImage;
			MetalBuffer			dstBuffer;
			uint				rangesCount;
			//BufferImageCopy	ranges []
		};

		struct GenerateMipmapsCmd : BaseCmd
		{
			MetalImage	image;
		};

		//-------------------------------------------------
		// graphics commands
		

		//-------------------------------------------------
		// draw commands
		

		//-------------------------------------------------
		// acceleration structure build commands
		

		//-------------------------------------------------
		// ray tracing commands


	private:
		#define AE_BASE_IND_CTX_COMMANDS( _visitor_ )\
			/* shared commands */\
			_visitor_( DebugMarkerCmd )\
			_visitor_( PushDebugGroupCmd )\
			_visitor_( PopDebugGroupCmd )\
			/* transfer commands */\
			_visitor_( BeginTransferCommandsCmd )\
			_visitor_( FillBufferCmd )\
			_visitor_( CopyBufferCmd )\
			_visitor_( CopyBufferToImageCmd )\
			_visitor_( CopyImageToBufferCmd )\
			_visitor_( GenerateMipmapsCmd )\
			/* compute commands */\
			_visitor_( BeginComputeCommandsCmd )\
		
		using Commands_t	= TypeList<
				#define AE_BASE_IND_CTX_VISIT( _name_ )		_name_,
				AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
				#undef AE_BASE_IND_CTX_VISIT
				void
			>::PopBack::type;
			
		using CmdContent_t	= TypeList<
				// shared commands
				char,
				// transfer commands
				BufferCopy, ImageCopy, BufferImageCopy
				// compute commands
				// graphics commands
				// draw commands
				// acceleration structure build commands
				// ray tracing commands
			>;
		
		
	private:
		struct _CmdProcessor;
		struct _Encoders;
		
		
	// methods
	public:
		MSoftwareCmdBuf ()	{}
		
		ND_ MBakedCommands	Bake ();
		
		template <typename CmdType, typename ...DynamicTypes>
		ND_ CmdType&  CreateCmd (usize dynamicArraySize = 0)	{ return SoftwareCmdBufBase::_CreateCmd< Commands_t, CmdType, DynamicTypes... >( dynamicArraySize ); }
		
		void  DebugMarker (NtStringView text);
		void  PushDebugGroup (NtStringView text);
		void  PopDebugGroup ();
		void  CommitBarriers (EBarrierScope scope, ArrayView<MetalResource> resources);

		ND_ static bool  Execute (MetalCommandBuffer cmdbuf, void* root);
		
	private:
		ND_ bool  _Validate (const void* root) const	{ return SoftwareCmdBufBase::_Validate( root, Commands_t::Count ); }
	};
	
	using MSoftwareCmdBufPtr = Unique< MSoftwareCmdBuf >;
	
	
	
	//
	// Metal Indirect Context base class
	//

	class _MBaseIndirectContext
	{
	// types
	public:
		static constexpr bool	IsIndirectContext = true;
		
	protected:
		#define AE_BASE_IND_CTX_VISIT( _name_ )		using _name_ = MSoftwareCmdBuf::_name_;
		AE_BASE_IND_CTX_COMMANDS( AE_BASE_IND_CTX_VISIT )
		#undef AE_BASE_IND_CTX_VISIT


	// variables
	protected:
		MSoftwareCmdBufPtr	_cmdbuf;


	// methods
	public:
		virtual ~_MBaseIndirectContext ();

		ND_ bool				IsValid ()			const	{ return _cmdbuf and _cmdbuf->IsValid(); }
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		explicit _MBaseIndirectContext (MSoftwareCmdBufPtr cmdbuf) : _cmdbuf{RVRef(cmdbuf)} {}

		explicit _MBaseIndirectContext (NtStringView dbgName);
		_MBaseIndirectContext (NtStringView dbgName, MSoftwareCmdBufPtr cmdbuf);

		void  _DebugMarker (NtStringView text, RGBA8u)		{ _cmdbuf->DebugMarker( text ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u)	{ _cmdbuf->PushDebugGroup( text ); }
		void  _PopDebugGroup ()								{ _cmdbuf->PopDebugGroup(); }
	};



	//
	// Metal Indirect Context base class with barrier manager
	//

	class MBaseIndirectContext : public _MBaseIndirectContext
	{
	// variables
	protected:
		MBarrierManager		_mngr;
		

	// methods
	public:
		explicit MBaseIndirectContext (Ptr<MCommandBatch> batch);
		MBaseIndirectContext (Ptr<MCommandBatch> batch, MSoftwareCmdBufPtr cmdbuf);
		~MBaseIndirectContext () override;
		
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ()		{ ASSERT( _NoPendingBarriers() );  return _MBaseIndirectContext::ReleaseCommandBuffer(); }

	protected:
		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()			const	{ return _mngr.GetDevice().GetFeatures(); }
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
