// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct CB2_TestData
	{
		Strong<BufferID>			buf_1;
		Strong<BufferID>			buf_2;
		const Bytes					buf_size	= 128_b;
		Array<ubyte>				buffer_data;
		AsyncTask					result;
		RC<CommandBatch>			batch;
		bool						isOK		= false;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};


	template <typename Ctx>
	class CB2_CopyBufferTask final : public RenderTask
	{
	public:
		CB2_TestData&	t;

		CB2_CopyBufferTask (CB2_TestData& t, RC<CommandBatch> batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			Ctx		ctx{ GetBatchPtr() };

			CHECK_TE( ctx.IsValid() );
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc );

			CHECK_TE( ctx.UploadBuffer( t.buf_1, 0_b, t.buffer_data ) == ArraySizeOf(t.buffer_data) );

			ctx.AccumBarriers().BufferBarrier( t.buf_1, EResourceState::CopyDst, EResourceState::CopySrc );

			ctx.CopyBuffer( t.buf_1, t.buf_2, {BufferCopy{ 0_b, 0_b, t.buf_size }});

			t.result = AsyncTask{ ctx.ReadHostBuffer( t.buf_2, 0_b, t.buf_size )
						.Then( [p = &t] (const BufferMemView &view)
								{
									p->isOK = (view == ArrayView<ubyte>{ p->buffer_data });
								})};
			
			ctx.AccumBarriers().BufferBarrier( t.buf_2, EResourceState::CopyDst, EResourceState::Host_Read );
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};

	
	template <typename Ctx>
	static bool  CopyBuffer2Test ()
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		EMemoryType		host_mem	= Default;
		CB2_TestData	t;
		
		t.gfxAlloc = MakeRC<GfxLinearMemAllocator>();

		if ( res_mngr.IsSupported( EMemoryType::HostCached ))
			host_mem = EMemoryType::HostCached;
		else
		if ( res_mngr.IsSupported( EMemoryType::HostCocherent ))
			host_mem = EMemoryType::HostCocherent;
		else
		if ( res_mngr.IsSupported( EMemoryType::Unified ))
			host_mem = EMemoryType::Unified;

		t.buf_1 = res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer    }.SetMemory( EMemoryType::DeviceLocal ), "buf_1", t.gfxAlloc );
		t.buf_2 = res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::TransferDst }.SetMemory( host_mem                 ), "buf_2", t.gfxAlloc );
		CHECK_ERR( t.buf_1 and t.buf_2 );
	
		t.buffer_data.resize( uint(t.buf_size) );
		for (usize i = 0; i < t.buffer_data.size(); ++i) {
			t.buffer_data[i] = ubyte(i);
		}

		AsyncTask	begin	= rts.BeginFrame();
		CHECK_ERR( begin );

		auto	batch = rts.CreateBatch( EQueueType::Graphics, 0, "CopyBuffer2" );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< CB2_CopyBufferTask<Ctx> >( MakeTuple(ArgRef(t)), MakeTuple(begin), "Copy buffer task" );
		CHECK_ERR( task1 );

		AsyncTask	end		= rts.EndFrame( MakeTuple(task1) );
		CHECK_ERR( end );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.buf_1, t.buf_2 ));

		CHECK_ERR( t.isOK );
		return true;
	}

}	// namespace


bool RGTest::Test_CopyBuffer2 ()
{
	CHECK_ERR( CopyBuffer2Test< DirectCtx::Transfer   >());
	CHECK_ERR( CopyBuffer2Test< IndirectCtx::Transfer >());
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}