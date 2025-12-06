// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Executor.h"

#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"


/*
=================================================
	Initialize
=================================================
*/
bool  Executor::Initialize ()
{
	using namespace Threading;
	using namespace Graphics;

	TaskScheduler::InstanceCtor::Create();

	TaskScheduler::Config	cfg;
	CHECK_FATAL( Scheduler().Setup( cfg ));

	VDeviceInitializer::InstanceCreateInfo	inst_ci;
	inst_ci.appName			= "CoopMatTest";
	inst_ci.instanceLayers	= vulkan.GetRecommendedInstanceLayers();

	CHECK_ERR( vulkan.CreateInstance( inst_ci ));

	vulkan.CreateDebugCallback( VDeviceInitializer::c_DefaultDebugMessageSeverity,
                                VDeviceInitializer::c_DefaultDebugMessageTypes,
								[] (const VDeviceInitializer::DebugReport &rep) { AE_LOGW(rep.message);  CHECK(not rep.isError); });

	CHECK_ERR( vulkan.ChooseHighPerformanceDevice() );
	CHECK_ERR( vulkan.CreateDefaultQueue() );
	CHECK_ERR( vulkan.CreateLogicalDevice() );

	CHECK_ERR( vulkan.IsInitialized() );
	CHECK_ERR( vulkan.CheckConstantLimits() );
	CHECK_ERR( vulkan.CheckExtensions() );

	RenderTaskScheduler::InstanceCtor::Create( vulkan );
	CHECK_ERR( GraphicsScheduler().Initialize( Default ));

	return true;
}

/*
=================================================
	Deinitialize
=================================================
*/
void  Executor::Deinitialize ()
{
	using namespace Threading;
	using namespace Graphics;

	RenderTaskScheduler::InstanceCtor::Destroy();

	CHECK_ERRV( vulkan.DestroyLogicalDevice() );
	CHECK_ERRV( vulkan.DestroyInstance() );
	CHECK_ERRV( not vulkan.IsInitialized() );

	Scheduler().Release();
	TaskScheduler::InstanceCtor::Destroy();
}

/*
=================================================
	Run
=================================================
*/
bool  Executor::Run (StringView source, ByteBuffer inputA, ByteBuffer inputB, ByteBuffer inputC, ByteBuffer output, uint elementSize)
{
	Graphics::GAutorelease<Graphics::PipelinePackID>	pack_id;
	Graphics::RenderTechPipelinesPtr					rtech;

	try {
		_Compile( source, elementSize, OUT pack_id, OUT rtech );
	}
	catch(...)
	{
		PipelineCompiler::ObjectStorage::SetInstance( null );
		return false;
	}

	CHECK_ERR( _RunPipe( inputA, inputB, inputC, output, rtech ));
	return true;
}

/*
=================================================
	_RunPipe
=================================================
*/
bool  Executor::_RunPipe (ByteBuffer inputA, ByteBuffer inputB, ByteBuffer inputC, ByteBuffer output, Graphics::RenderTechPipelinesPtr rtech)
{
	using namespace Threading;
	using namespace Graphics;

	const auto	timeout = seconds{10};

	auto&	rts			= GraphicsScheduler();
	auto&	res_mngr	= rts.GetResourceManager();

	auto	pipe_id	= rtech->GetComputePipeline( PipelineName{"comp"} );
	CHECK_ERR( pipe_id );

	GAutorelease	buf_a	= res_mngr.CreateBuffer( BufferDesc{ Bytes{inputA.size()}, EBufferUsage::Storage | EBufferUsage::Transfer }, "InputA" );
	GAutorelease	buf_b	= res_mngr.CreateBuffer( BufferDesc{ Bytes{inputB.size()}, EBufferUsage::Storage | EBufferUsage::Transfer }, "InputB" );
	GAutorelease	buf_c	= res_mngr.CreateBuffer( BufferDesc{ Bytes{inputC.size()}, EBufferUsage::Storage | EBufferUsage::Transfer }, "InputC" );
	GAutorelease	buf_out	= res_mngr.CreateBuffer( BufferDesc{ Bytes{output.size()}, EBufferUsage::Storage | EBufferUsage::Transfer }, "Output" );
	CHECK_ERR( buf_a and buf_b and buf_c and buf_out );

	GAutorelease	ds = res_mngr.CreateDescriptorSet( pipe_id, DescriptorSetName{"ds0"} ).Get<0>();
	CHECK_ERR( ds );

	{
		DescriptorUpdater	updater;
		updater.Set( ds, EDescUpdateMode::Partialy );
		updater.BindBuffer( UniformName{"un_InputA"}, buf_a );
		updater.BindBuffer( UniformName{"un_InputB"}, buf_b );
		updater.BindBuffer( UniformName{"un_InputC"}, buf_c );
		updater.BindBuffer( UniformName{"un_Output"}, buf_out );
		updater.Flush();
	}

	const EThreadArray	threads { EThread::Main, EThread::Renderer, EThread::PerFrame };
	CHECK_ERR( rts.WaitNextFrame( threads, timeout ));
	CHECK_ERR( rts.BeginFrame() );

	AsyncTask read_op;

	auto	batch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );

	auto	task = batch->Run(
					[&] () -> RenderCoro
					{
						DirectCtx::CommandBuffer	cmdbuf;
						{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							CHECK_CE( ctx.UploadBuffer( buf_a,   0_b, ArraySizeOf(inputA), inputA.data() ));
							CHECK_CE( ctx.UploadBuffer( buf_b,   0_b, ArraySizeOf(inputB), inputB.data() ));
							CHECK_CE( ctx.UploadBuffer( buf_c,   0_b, ArraySizeOf(inputC), inputC.data() ));
							CHECK_CE( ctx.UploadBuffer( buf_out, 0_b, ArraySizeOf(output), output.data() ));

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Compute	ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							ctx.AccumBarriers()
								.MemoryBarrier( EResourceState::CopyDst, EResourceState::ShaderAddress_RW | EResourceState::CoopVecConvertStage );

							ctx.BindPipeline( pipe_id );
							ctx.BindDescriptorSet( DescSetBinding{0}, ds );
							ctx.Dispatch( 1 );

							ctx.AccumBarriers()
								.MemoryBarrier( EResourceState::ShaderAddress_RW | EResourceState::CoopVecConvertStage, EResourceState::CopySrc );

							cmdbuf = ctx.ReleaseCommandBuffer();
						}{
							DirectCtx::Transfer  ctx { RenderCoro_Get(), RVRef(cmdbuf) };

							read_op = ctx.ReadbackBuffer( buf_out, Default ).Then(
										output,
										[] (Promise<BufferMemView> readRes, CoSafe<ByteBuffer&> output) -> InlineCoro<>
										{
											auto  view = co_await readRes;
											Bytes size = view->CopyTo( OUT ByteBuffer{output} );
											CHECK_Eq( size, output->size() );
										});

							RenderCoro_Execute( ctx );
						}
					}(),
					Tuple{},
					True{"Last"},
					{"Compute task"}
				);

	auto	end = rts.EndFrame( Tuple{task} );


	// wait for submit
	CHECK_ERR( Scheduler().Wait( {end}, threads, timeout ));

	// wait for complete on GPU
	CHECK_ERR( rts.WaitAll( threads, timeout ));

	// wait for result
	CHECK_ERR( Scheduler().Wait( {read_op}, threads, timeout ));

	return true;
}

/*
=================================================
	_Compile
=================================================
*/
void  Executor::_Compile (StringView source, uint elementSize,
						  OUT Graphics::GAutorelease<Graphics::PipelinePackID> &outPackId,
						  OUT Graphics::RenderTechPipelinesPtr &outRTech) __Th___
{
	using namespace AE::PipelineCompiler;

	// init pipeline compiler
	ObjectStorage	obj_storage;
	PipelineStorage	ppln_storage;
	{
		obj_storage.pplnStorage			= &ppln_storage;
		obj_storage.defaultFeatureSet	= "DefaultFS";

		obj_storage.spirvCompiler		= MakeUnique<SpirvCompiler>( ArrayView<Path>{} );
		obj_storage.spirvCompiler->SetDefaultResourceLimits();

		ObjectStorage::SetInstance( &obj_storage );

		ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ obj_storage.defaultFeatureSet }};
		fs->fs = GraphicsScheduler().GetFeatureSet();

		PipelineCompiler::ScriptConfig	cfg;
		cfg.SetShaderVersion( EShaderVersion(Version2::From100( fs->fs.maxShaderVersion.spirv ).ToHex()) |
							  EShaderVersion::_GLSL_SPIRV );

		cfg.SetDefaultLayout( EStructLayout::Std140 );
		cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );

		cfg.SetTarget( ECompilationTarget::Vulkan );

		cfg.SetPipelineOptions( EPipelineOpt::Optimize );
		cfg.SetShaderOptions( EShaderOpt::Optimize );
	}
	{
		RenderTechniquePtr	rtech{ new RenderTechnique{ "rtech" }};
		{
			RTComputePassPtr	pass = rtech->AddComputePass2( "Compute" );
			Unused( pass );
		}

		const auto				stage	= EShaderStages::Compute;
		DescriptorSetLayoutPtr	ds_layout{ new DescriptorSetLayout{ "dsl.0" }};
		{
			ShaderStructTypePtr	st{ new ShaderStructType{"CoopMatData"}};

			switch ( elementSize )
			{
				case 1 :	st->Set( EStructLayout::Std430, R"#( ubyte  data[]; )#");	break;
				case 2 :	st->Set( EStructLayout::Std430, R"#( ushort data[]; )#");	break;
				case 4 :	st->Set( EStructLayout::Std430, R"#( uint   data[]; )#");	break;
				case 8 :	st->Set( EStructLayout::Std430, R"#( ulong  data[]; )#");	break;
				default :	CHECK_THROW( false );
			}

			ds_layout->AddStorageBuffer( stage, "un_InputA", ArraySize{1}, "CoopMatData", EAccessType::Coherent, EResourceState::ShaderStorage_Read, False{} );
			ds_layout->AddStorageBuffer( stage, "un_InputB", ArraySize{1}, "CoopMatData", EAccessType::Coherent, EResourceState::ShaderStorage_Read, False{} );
			ds_layout->AddStorageBuffer( stage, "un_InputC", ArraySize{1}, "CoopMatData", EAccessType::Coherent, EResourceState::ShaderStorage_Read, False{} );
			ds_layout->AddStorageBuffer( stage, "un_Output", ArraySize{1}, "CoopMatData", EAccessType::Coherent, EResourceState::ShaderStorage_Write, False{} );
		}

		PipelineLayoutPtr		ppln_layout{ new PipelineLayout{ "comp.pl" }};
		ppln_layout->AddDSLayout2( "ds0", 0, "dsl.0" );

		ComputePipelinePtr		ppln_templ{ new ComputePipelineScriptBinding{ "comp" }};
		ppln_templ->Disable();
		ppln_templ->SetLayout2( ppln_layout );

		{
			const uint	subgroup_size = GraphicsScheduler().GetDevice().GetDeviceProperties().compute.subgroupSize;

			ScriptShaderPtr		sh{ new ScriptShader{}};
			sh->SetSource( EShader::Compute, String{source} );
			sh->options = EShaderOpt::Optimize;
			sh->SetComputeLocalSize3( subgroup_size, 1, 1 );

			ppln_templ->SetShader( sh );
		}
		{
			ComputePipelineSpecPtr	ppln_spec = ppln_templ->AddSpecialization2( "comp" );
			ppln_spec->Disable();
			ppln_spec->AddToRenderTech( "rtech", "Compute" );
			ppln_spec->SetOptions( EPipelineOpt::Optimize );

			// if successfully compiled
			ppln_spec->Enable();
		}
	}

	RC<RStream>	stream;
	{
		CHECK_THROW( obj_storage.Build() );
		CHECK_THROW( obj_storage.BuildRenderTechniques() );

		auto	mem = MakeRC<ArrayWStream>();

		PipelinePackOffsets		offsets	= {};
		CHECK_THROW( obj_storage.SavePack( *mem, true, OUT offsets ));

		auto	mem2 = MakeRC<ArrayWDataSource>( mem->ReleaseData() );
				mem  = null;

		CHECK_THROW( mem2->Write( Sizeof(PackOffsets_Name), offsets ));

		obj_storage.Clear();

		stream = MakeRC<ArrayRStream>( mem2->ReleaseData() );
	}

	// load from stream
	{
		using namespace Graphics;
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		PipelinePackDesc	desc;
		desc.stream = stream;

		outPackId = res_mngr.LoadPipelinePack( desc );
		CHECK_THROW( outPackId );

		outRTech = res_mngr.LoadRenderTech( outPackId, RenderTechName{"rtech"} );
		CHECK_THROW( outRTech );
	}

	ObjectStorage::SetInstance( null );
}

/*
=================================================
	Supports*
=================================================
*/
bool  Executor::SupportsCoopMatrix () const
{
	return GraphicsScheduler().GetFeatureSet().cooperativeMatrix == Graphics::FeatureSet::EFeature::RequireTrue;
}

bool  Executor::SupportsCoopVector () const
{
	return GraphicsScheduler().GetFeatureSet().cooperativeVector == Graphics::FeatureSet::EFeature::RequireTrue;
}

bool  Executor::SupportsCoopVecTraining () const
{
	return GraphicsScheduler().GetFeatureSet().cooperativeVectorTraining == Graphics::FeatureSet::EFeature::RequireTrue;
}

bool  Executor::SupportsIntDotProduct () const
{
	return GraphicsScheduler().GetDevice().GetVExtensions().shaderIntegerDotProduct;
}

