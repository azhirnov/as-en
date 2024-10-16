// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct CB1_TestData
	{
		GAutorelease<BufferID>		buf_1;
		GAutorelease<BufferID>		buf_2;
		const Bytes					buf_size	= 128_b;
		Array<ubyte>				buffer_data;
		AsyncTask					result;
		CommandBatchPtr				batch;
		bool						isOK		= false;
		GfxMemAllocatorPtr			gfxAlloc;
	};


	template <typename Ctx>
	class CB1_CopyBufferTask final : public RenderTask
	{
	public:
		CB1_TestData&	t;

		CB1_CopyBufferTask (CB1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			Ctx		ctx{ *this };

			CHECK_TE( ctx.UpdateHostBuffer( t.buf_1, 0_b, t.buffer_data ));

			ctx.CopyBuffer( t.buf_1, t.buf_2, {BufferCopy{ 0_b, 0_b, t.buf_size }});

			ctx.AccumBarriers()
				.BufferBarrier( t.buf_2, EResourceState::CopyDst, EResourceState::CopySrc );

			auto	read_res = ctx.ReadbackBuffer( t.buf_2, ReadbackBufferDesc{}.DataSize( t.buf_size ));
			CHECK_TE( read_res.IsCompleted() );

			t.result = AsyncTask{ read_res.Then( [p = &t] (const BufferMemView &view)
								{
									p->isOK = (view == ArrayView<ubyte>{ p->buffer_data });
								})};

			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			Execute( ctx );
		}
	};


	template <typename Ctx>
	static bool  CopyBuffer1Test ()
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		EMemoryType		host_mem	= Default;
		CB1_TestData	t;

		t.gfxAlloc = res_mngr.CreateLinearGfxMemAllocator();

		if ( res_mngr.IsSupported( EMemoryType::HostCached ))
			host_mem = EMemoryType::HostCached;
		else
		if ( res_mngr.IsSupported( EMemoryType::HostCoherent ))
			host_mem = EMemoryType::HostCoherent;
		else
		if ( res_mngr.IsSupported( EMemoryType::Unified ))
			host_mem = EMemoryType::Unified;

		t.buf_1 = res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::TransferSrc }.SetMemory( host_mem                 ), "buf_1", t.gfxAlloc );
		t.buf_2 = res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer    }.SetMemory( EMemoryType::DeviceLocal ), "buf_2", t.gfxAlloc );
		CHECK_ERR( t.buf_1 and t.buf_2 );

		t.buffer_data.resize( uint(t.buf_size) );
		for (usize i = 0; i < t.buffer_data.size(); ++i) {
			t.buffer_data[i] = ubyte(i);
		}


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"CopyBuffer1"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run< CB1_CopyBufferTask<Ctx> >( Tuple{ArgRef(t)}, Tuple{}, True{"Last"}, {"Copy buffer task"} );
		AsyncTask	end		= rts.EndFrame( Tuple{task1} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_CopyBuffer1 ()
{
	bool	result = true;

	RG_CHECK( CopyBuffer1Test< DirectCtx::Transfer   >());
	RG_CHECK( CopyBuffer1Test< IndirectCtx::Transfer >());

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
