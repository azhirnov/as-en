// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Compiler/MetalCompiler.h"
#include "ScriptObjects/Common.inl.h"

#ifdef AE_ENABLE_GLSL_TRACE
# include "ShaderTrace.h"
#else
# include "Packer/ShaderTraceDummy.h"
#endif

namespace AE::PipelineCompiler
{
namespace
{
/*
=================================================
    HasShaderClock
=================================================
*/
    struct ShaderDebuggerFeatures
    {
        bool    shaderDeviceClock               = false;
        bool    shaderSubgroupClock             = false;
        bool    fragmentStoresAndAtomics        = false;
        bool    vertexPipelineStoresAndAtomics  = false;
    };
    ND_ static ShaderDebuggerFeatures  GetShaderDebuggerFeatures (ArrayView<ScriptFeatureSetPtr> features)
    {
        FeatureSetCounter       device;
        FeatureSetCounter       subgroup;
        FeatureSetCounter       vertex_atomic;
        FeatureSetCounter       frag_atomic;
        ShaderDebuggerFeatures  result;

        for (auto& ptr : features)
        {
            device          .Add( ptr->fs.shaderDeviceClock );
            subgroup        .Add( ptr->fs.shaderSubgroupClock );
            vertex_atomic   .Add( ptr->fs.vertexPipelineStoresAndAtomics );
            frag_atomic     .Add( ptr->fs.fragmentStoresAndAtomics );
        }

        result.shaderDeviceClock                = device.IsTrue();
        result.shaderSubgroupClock              = subgroup.IsTrue();
        result.fragmentStoresAndAtomics         = frag_atomic.IsTrue();
        result.vertexPipelineStoresAndAtomics   = vertex_atomic.IsTrue();

        return result;
    }
}

/*
=================================================
    _CompileShaderGLSL
=================================================
*/
    void  ObjectStorage::_CompileShaderGLSL (const ShaderSrcKey &info, ArrayView<ScriptFeatureSetPtr> features, const uint debugDSIndex, const PathAndLine &shaderPath,
                                             const String &entry, OUT CompiledShader &compiled) __Th___
    {
        CHECK_THROW_MSG( spirvCompiler );

        String  header;

        // add header
        {
            Version2    max_spv_ver;
            header = GetShaderExtensionsGLSL( INOUT max_spv_ver, EShaderStages::Unknown | info.type, features );

            CHECK_THROW_MSG( EShaderVersion_Ver2( info.version ) <= max_spv_ver );

            header << "\n#define " << ShaderToStr( info.type ) << " 1\n";
            header << "#define ND_\n";

            for (auto& def : info.defines) {
                header << def << '\n';
            }
            header << '\n';

            for (auto& inc : info.include) {
                header << "#include \"" << inc << "\"\n";
            }
            header << '\n';

            header << info.resources;
        }

        if ( debugDSIndex != UMax )
        {
            TestFeature_Min( features, &FeatureSet::maxDescriptorSets, ushort(debugDSIndex), "maxDescriptorSets", "Debug descriptor set index" );
        }

        const auto  dbg_feats = GetShaderDebuggerFeatures( features );
        if ( AnyBits( info.options, EShaderOpt::_ShaderTrace_Mask ))
        {
            switch ( info.type )
            {
                case EShader::Vertex :
                case EShader::TessControl :
                case EShader::TessEvaluation :
                case EShader::Geometry :
                    CHECK_THROW_MSG( dbg_feats.vertexPipelineStoresAndAtomics,
                        "Shader debugger requires 'vertexPipelineStoresAndAtomics' feature for "s << ToString(info.type) << " shader." );
                    break;

                case EShader::Fragment :
                    CHECK_THROW_MSG( dbg_feats.fragmentStoresAndAtomics,
                        "Shader debugger requires 'fragmentStoresAndAtomics' features for "s << ToString(info.type) << " shader." );
                    break;
            }
        }

        SpirvCompiler::Output   out;
        SpirvCompiler::Input    in;
        in.shaderType           = info.type;
        in.spirvVersion         = EShaderVersion_Ver2( info.version );
        in.entry                = entry.c_str();
        in.header               = header;
        in.source               = info.source;
        in.options              = info.options;
        in.dbgDescSetIdx        = debugDSIndex;
        in.fileLoc              = shaderPath;
        in.shaderDeviceClock    = dbg_feats.shaderDeviceClock;
        in.shaderSubgroupClock  = dbg_feats.shaderSubgroupClock;

        if_unlikely( not spirvCompiler->Compile( in, OUT out ))
        {
            AE_LOGI( "Shader source:\n"s << in.header << '\n' << in.source );
            CHECK_THROW_MSG( false, "Failed to compile shader:\n"s << out.log );
        }

        if ( not out.log.empty() )
        {
            AE_LOG_DBG( "Shader compiled with warnings:\n"s << out.log );
        }

        compiled.version    = info.version;
        compiled.type       = info.type;
        compiled.reflection = RVRef(out.reflection);

        CHECK_THROW_MSG( ((info.options & EShaderOpt::_ShaderTrace_Mask) != Default) == (out.trace != null) );

        if ( out.trace )
        {
            SpirvWithTrace  dbg_spv;
            dbg_spv.bytecode    = RVRef(out.spirv);
            dbg_spv.trace       = RVRef(out.trace);

            compiled.data = RVRef(dbg_spv);
        }
        else
        {
            compiled.data = RVRef(out.spirv);
        }
    }

/*
=================================================
    GetShaderExtensionsGLSL
=================================================
*/
    String  ObjectStorage::GetShaderExtensionsGLSL (INOUT Version2 &spirvVer, const EShaderStages stage, ArrayView<ScriptFeatureSetPtr> features) __Th___
    {
        String  def;
        String  ext =
            "#version 460 core\n"
            "#extension GL_ARB_separate_shader_objects                  : require\n"
            "#extension GL_ARB_shading_language_420pack                 : require\n"
            "#extension GL_GOOGLE_include_directive                     : require\n"
            "#extension GL_GOOGLE_cpp_style_line_directive              : require\n"
            "#extension GL_EXT_control_flow_attributes                  : require\n"
            "#extension GL_EXT_debug_printf                             : enable\n"
            "#extension GL_EXT_samplerless_texture_functions            : enable\n";

        // SPIRV version
        {
            const uint  ver = Max( spirvVer.To100(), GetMaxValueFromFeatures( features, &FeatureSet::maxShaderVersion ).spirv );
            spirvVer = Version2::From100( ver );
            CHECK_THROW_MSG(( spirvVer == Version2{0,0} or spirvVer >= Version2{1,0} ));
        }

        // render states
        /*{
            bool    independent_blend   = false;

            for (auto& ptr : features)
            {
                independent_blend   |= (ptr->fs.independentBlend == EFeature::RequireTrue);
            }

            if ( independent_blend )
                def << "#define AE_INDEPENDENT_BLEND 1\n";
        }*/

        // subgroup
        {
            FeatureSet::SubgroupOperationBits   ops;

            ESubgroupTypes          types           = Default;
            FeatureSetCounter       dynamic_id;
            FeatureSetCounter       un_control_flow;
            //uint                  min_size        = 0;
            //uint                  max_size        = UMax;

            for (auto& ptr : features)
            {
                if ( AllBits( ptr->fs.subgroupStages, stage ))
                {
                    ops             |= ptr->fs.subgroupOperations;
                    types           |= ptr->fs.subgroupTypes;
                    dynamic_id      .Add( ptr->fs.subgroupBroadcastDynamicId );
                    un_control_flow .Add( ptr->fs.shaderSubgroupUniformControlFlow );
                    //min_size      = Max( min_size, ptr->fs.minSubgroupSize );
                    //max_size      = Max( max_size, ptr->fs.maxSubgroupSize );
                    //CHECK( min_size <= max_size );
                }
            }

            if ( ops.Any() )
                CHECK_THROW_MSG(( spirvVer >= Version2{1,1} ));
            if ( ops.AnyInRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End ))
                ext << "#extension GL_KHR_shader_subgroup_basic                    : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_Vote_Begin, ESubgroupOperation::_Vote_End ))
                ext << "#extension GL_KHR_shader_subgroup_vote                     : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_Arithmetic_Begin, ESubgroupOperation::_Arithmetic_End ))
                ext << "#extension GL_KHR_shader_subgroup_arithmetic               : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_Ballot_Begin, ESubgroupOperation::_Ballot_End ))
                ext << "#extension GL_KHR_shader_subgroup_ballot                   : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_Shuffle_Begin, ESubgroupOperation::_Shuffle_End ))
                ext << "#extension GL_KHR_shader_subgroup_shuffle                  : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End ))
                ext << "#extension GL_KHR_shader_subgroup_shuffle_relative         : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_Clustered_Begin, ESubgroupOperation::_Clustered_End ))
                ext << "#extension GL_KHR_shader_subgroup_clustered                : require\n";
            if ( ops.AnyInRange( ESubgroupOperation::_Quad_Begin, ESubgroupOperation::_Quad_End ))
                ext << "#extension GL_KHR_shader_subgroup_quad                     : require\n";

            for (auto t : BitfieldIterate( types ))
            {
                switch_enum( t )
                {
                    case ESubgroupTypes::Float32 :
                    case ESubgroupTypes::Int32 :        break;  // skip
                    case ESubgroupTypes::Int8 :         ext << "#extension GL_EXT_shader_subgroup_extended_types_int8      : require\n";    break;
                    case ESubgroupTypes::Int16 :        ext << "#extension GL_EXT_shader_subgroup_extended_types_int16     : require\n";    break;
                    case ESubgroupTypes::Int64 :        ext << "#extension GL_EXT_shader_subgroup_extended_types_int64     : require\n";    break;
                    case ESubgroupTypes::Float16 :      ext << "#extension GL_EXT_shader_subgroup_extended_types_float16   : require\n";    break;
                    case ESubgroupTypes::Unknown :
                    case ESubgroupTypes::_Last :
                    case ESubgroupTypes::All :
                    default :                           CHECK_MSG( false, "unknown type" );
                }
                switch_end
            }

            if ( dynamic_id.IsTrue() )
                ext << "#extension GL_ARB_shader_ballot                            : require\n";

            if ( un_control_flow.IsTrue() and spirvVer >= Version2{1,3} )
                ext << "#ifdef GL_EXT_subgroupuniform_qualifier\n"
                    << "# extension GL_EXT_subgroupuniform_qualifier               : require\n"
                    << "#endif\n";

            //def << "#define AE_MIN_SUBGROUP_SIZE " << ToString( min_size ) << "\n"
            //  << "#define AE_MAX_SUBGROUP_SIZE " << ToString( max_size ) << "\n";
        }

        // shader types
        {
            FeatureSetCounter   shader_int8;
            FeatureSetCounter   shader_int16;
            FeatureSetCounter   shader_int64;
            FeatureSetCounter   shader_float16;
            FeatureSetCounter   shader_float64;
            FeatureSetCounter   scalar_layout;
            FeatureSetCounter   dev_addr_supported;

            for (auto& ptr : features)
            {
                shader_int8         .Add( ptr->fs.shaderInt8 );
                shader_int16        .Add( ptr->fs.shaderInt16 );
                shader_int64        .Add( ptr->fs.shaderInt64 );
                shader_float16      .Add( ptr->fs.shaderFloat16 );
                shader_float64      .Add( ptr->fs.shaderFloat64 );
                scalar_layout       .Add( ptr->fs.scalarBlockLayout );
                dev_addr_supported  .Add( ptr->fs.bufferDeviceAddress );
                // TODO: storageBuffer16BitAccess, ...
            }

            if ( shader_int8.IsTrue() ) {
                ext << "#extension GL_EXT_shader_8bit_storage                      : require\n"
                    << "#extension GL_EXT_shader_explicit_arithmetic_types_int8    : require\n"
                    << "#define AE_ENABLE_BYTE_TYPE 1\n";
            }
            if ( shader_int16.IsTrue() or shader_float16.IsTrue() ) {
                ext << "#extension GL_EXT_shader_16bit_storage                     : require\n";
            }
            if ( shader_int16.IsTrue() ) {
                ext << "#extension GL_EXT_shader_explicit_arithmetic_types_int16   : require\n"
                    << "#define AE_ENABLE_SHORT_TYPE 1\n";
            }
            if ( shader_float16.IsTrue() ) {
                ext << "#extension GL_EXT_shader_explicit_arithmetic_types_float16 : require\n"
                    << "#define AE_ENABLE_HALF_TYPE 1\n";
            }
            if ( shader_int64.IsTrue() ) {
                ext << "#extension GL_ARB_gpu_shader_int64                         : require\n"
                    << "#extension GL_EXT_shader_explicit_arithmetic_types_int64   : require\n"
                    << "#define AE_ENABLE_LONG_TYPE 1\n";
            }
            if ( shader_float64.IsTrue() ) {
                ext << "#extension GL_ARB_gpu_shader_fp64                          : require\n"
                    << "#extension GL_EXT_shader_explicit_arithmetic_types_float64 : require\n"
                    << "#define AE_ENABLE_DOUBLE_TYPE 1\n";
            }
            if ( scalar_layout.IsTrue() ) {
                ext << "#extension GL_EXT_scalar_block_layout                      : require\n";
            }
            if ( dev_addr_supported.IsTrue() )
            {
                ext << "#extension GL_EXT_buffer_reference                         : require\n";
                ext << "#extension GL_EXT_buffer_reference2                        : require\n";
                ext << "#extension GL_EXT_buffer_reference_uvec2                   : require\n";
            }
            ext << "#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable\n"
                << "#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable\n";
        }

        // atomic
        {
            FeatureSetCounter   has_atomics;
            FeatureSetCounter   storage_i64;
            FeatureSetCounter   image_i64;

            FeatureSetCounter   atomic_float;
            FeatureSetCounter   atomic_float2;

            for (auto& ptr : features)
            {
                if ( stage != Default and AllBits( EShaderStages::Fragment, stage ))
                    has_atomics.Add( ptr->fs.fragmentStoresAndAtomics );

                if ( stage != Default and
                     AllBits( EShaderStages::Vertex | EShaderStages::TessControl | EShaderStages::TessEvaluation | EShaderStages::Geometry | EShaderStages::MeshTask | EShaderStages::Mesh, stage ))
                    has_atomics.Add( ptr->fs.vertexPipelineStoresAndAtomics );

                storage_i64 .Add( ptr->fs.shaderBufferInt64Atomics );
                image_i64   .Add( ptr->fs.shaderImageInt64Atomics );

                atomic_float.Add( ptr->fs.shaderBufferFloat32Atomics );
                atomic_float.Add( ptr->fs.shaderBufferFloat32AtomicAdd );
                atomic_float.Add( ptr->fs.shaderBufferFloat64Atomics );
                atomic_float.Add( ptr->fs.shaderBufferFloat64AtomicAdd );
                atomic_float.Add( ptr->fs.shaderSharedFloat32Atomics );
                atomic_float.Add( ptr->fs.shaderSharedFloat32AtomicAdd );
                atomic_float.Add( ptr->fs.shaderSharedFloat64Atomics );
                atomic_float.Add( ptr->fs.shaderSharedFloat64AtomicAdd );
                atomic_float.Add( ptr->fs.shaderImageFloat32Atomics );
                atomic_float.Add( ptr->fs.shaderImageFloat32AtomicAdd );

                atomic_float2.Add( ptr->fs.shaderBufferFloat16Atomics );
                atomic_float2.Add( ptr->fs.shaderBufferFloat16AtomicAdd );
                atomic_float2.Add( ptr->fs.shaderBufferFloat16AtomicMinMax );
                atomic_float2.Add( ptr->fs.shaderBufferFloat32AtomicMinMax );
                atomic_float2.Add( ptr->fs.shaderBufferFloat64AtomicMinMax );
                atomic_float2.Add( ptr->fs.shaderSharedFloat16Atomics );
                atomic_float2.Add( ptr->fs.shaderSharedFloat16AtomicAdd );
                atomic_float2.Add( ptr->fs.shaderSharedFloat16AtomicMinMax );
                atomic_float2.Add( ptr->fs.shaderSharedFloat32AtomicMinMax );
                atomic_float2.Add( ptr->fs.shaderSharedFloat64AtomicMinMax );
                atomic_float2.Add( ptr->fs.shaderImageFloat32AtomicMinMax );
                atomic_float2.Add( ptr->fs.sparseImageFloat32AtomicMinMax );
            }

            if ( has_atomics.IsTrue() ) {
                def << "#define AE_HAS_ATOMICS 1\n";
            }
            if ( storage_i64.IsTrue() ) {
                ext << "#extension GL_EXT_shader_atomic_int64                      : require\n";
            }
            if ( image_i64.IsTrue() ) {
                ext << "#extension GL_EXT_shader_image_int64                       : require\n";
            }
            if ( atomic_float.IsTrue() ) {
                ext << "#extension GL_EXT_shader_atomic_float                      : require\n";
            }
            if ( atomic_float2.IsTrue() ) {
                ext << "#extension GL_EXT_shader_atomic_float2                     : require\n";
            }
        }

        // shader clock
        {
            FeatureSetCounter   device;
            FeatureSetCounter   subgroup;

            for (auto& ptr : features)
            {
                device  .Add( ptr->fs.shaderDeviceClock );
                subgroup.Add( ptr->fs.shaderSubgroupClock );
            }

            if ( device.IsTrue() ) {
                ext << "#extension GL_EXT_shader_realtime_clock                    : require\n";
            }
            if ( subgroup.IsTrue() ) {
                ext << "#extension GL_ARB_shader_clock                             : require\n";
            }
        }

        // mesh shader
        {
            FeatureSetCounter   ts_supported;
            FeatureSetCounter   ms_supported;
            for (auto& ptr : features) {
                ts_supported.Add( ptr->fs.taskShader );
                ms_supported.Add( ptr->fs.meshShader );
            }
            if ( (stage == EShaderStages::MeshTask  and ts_supported.IsTrue()) or
                 (stage == EShaderStages::Mesh      and ms_supported.IsTrue()) )
            {
                CHECK_THROW_MSG(( spirvVer >= Version2{1,4} ));
                ext << "#extension GL_EXT_mesh_shader                              : require\n";
            }
        }

        // ray tracing
        {
            FeatureSetCounter   rt_supported;
            FeatureSetCounter   prim_cull_supported;
            for (auto& ptr : features) {
                rt_supported        .Add( ptr->fs.rayTracingPipeline );
                prim_cull_supported .Add( ptr->fs.rayTraversalPrimitiveCulling );
            }
            if ( AnyBits( stage, EShaderStages::AllRayTracing ) and rt_supported.IsTrue() )
            {
                CHECK_THROW_MSG(( spirvVer >= Version2{1,4} ));
                ext << "#extension GL_EXT_ray_tracing                              : require\n";
                if ( prim_cull_supported.IsTrue() )
                    ext << "#extension GL_EXT_ray_flags_primitive_culling              : require\n";
            }
        }

        // ray query
        if ( spirvVer >= Version2{1,4} )
        {
            FeatureSetCounter   rq_supported;
            EShaderStages       rq_stages   = Default;

            for (auto& ptr : features)
            {
                rq_supported.Add( ptr->fs.rayQuery );
                rq_stages |= ptr->fs.rayQueryStages;
            }

            if ( rq_supported.IsTrue() and AnyBits( rq_stages, stage )) {
                ext << "#extension GL_EXT_ray_query                                : require\n";
                def << "#define AE_RAY_QUERY 1\n";
            }
        }

        // tile shader
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.tileShader );
            }
            if ( stage == EShaderStages::Tile and supported.IsTrue() ) {
                ext << "#extension GL_HUAWEI_subpass_shading                       : require\n";
            }
        }

        // shader output viewport / layer
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.shaderOutputViewportIndex );
                supported.Add( ptr->fs.shaderOutputLayer );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_ARB_shader_viewport_layer_array              : require\n";
            }
        }

        // sparse texture clamp
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.shaderResourceMinLod );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_ARB_sparse_texture_clamp                     : require\n";
            }
        }

        // NV shader SM
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.shaderSMBuiltinsNV );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_NV_shader_sm_builtins                        : require\n";
            }
        }

        // shader core builtins ARM
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.shaderCoreBuiltinsARM );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_ARM_shader_core_builtins                     : require\n";
            }
        }

        // nonuniform
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.shaderUniformBufferArrayNonUniformIndexing );
                supported.Add( ptr->fs.shaderSampledImageArrayNonUniformIndexing );
                supported.Add( ptr->fs.shaderStorageBufferArrayNonUniformIndexing );
                supported.Add( ptr->fs.shaderStorageImageArrayNonUniformIndexing );
                supported.Add( ptr->fs.shaderInputAttachmentArrayNonUniformIndexing );
                supported.Add( ptr->fs.shaderUniformTexelBufferArrayNonUniformIndexing );
                supported.Add( ptr->fs.shaderStorageTexelBufferArrayNonUniformIndexing );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_EXT_nonuniform_qualifier                     : require\n";
            }
        }

        // vulkan memory
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.vulkanMemoryModel );
                supported.Add( ptr->fs.vulkanMemoryModelDeviceScope );
                supported.Add( ptr->fs.vulkanMemoryModelAvailabilityVisibilityChains );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_KHR_memory_scope_semantics                   : require\n";
                def << "#define AE_MEM_SCOPE 1\n";
            }
        }

        // demote to helper invocation
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.shaderDemoteToHelperInvocation );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_EXT_demote_to_helper_invocation              : require\n";
            }
        }

        // fragment shader interlock
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.fragmentShaderSampleInterlock );
                supported.Add( ptr->fs.fragmentShaderPixelInterlock );
                supported.Add( ptr->fs.fragmentShaderShadingRateInterlock );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_ARB_fragment_shader_interlock                : require\n";
            }
        }

        // fragment shading rate
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.pipelineFragmentShadingRate );
                supported.Add( ptr->fs.primitiveFragmentShadingRate );
                supported.Add( ptr->fs.attachmentFragmentShadingRate );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_EXT_fragment_shading_rate                    : require\n";
            }
        }

        // fragment shading barycentric
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.fragmentShaderBarycentric );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_EXT_fragment_shader_barycentric              : require\n";
            }
        }

        // multiview
        {
            FeatureSetCounter   supported;
            for (auto& ptr : features) {
                supported.Add( ptr->fs.multiview );
            }
            if ( supported.IsTrue() ) {
                ext << "#extension GL_EXT_multiview                                : require\n";
            }
        }

        // cooperativeMatrix
        {
            FeatureSetCounter   supported;
            FeatureSetCounter   supported2;
            for (auto& ptr : features) {
                if ( AllBits( ptr->fs.cooperativeMatrixStages, stage ))
                    supported .Add( ptr->fs.cooperativeMatrix );
                supported2.Add( ptr->fs.vulkanMemoryModel );
            }
            if ( supported.IsTrue() ) {
                CHECK_THROW_MSG( supported2.IsTrue() ); // required
                ext << "#extension GL_KHR_cooperative_matrix                       : require\n";
                def << "#define AE_COOP_MATRIX 1\n";
            }
        }

        ext << '\n' << def << '\n';
        return ext;
    }

/*
=================================================
    CompileShaderGLSL
=================================================
*/
    void  ObjectStorage::CompileShaderGLSL (INOUT CompiledShaderPtr &outShader, const ScriptShaderPtr &inShader, EShaderVersion version,
                                            const String &defines, const String &resources, ArrayView<String> include, ArrayView<ScriptFeatureSetPtr> features,
                                            uint debugDSIndex, bool useMetalArgBuffer) __Th___
    {
        CHECK_THROW_MSG( not outShader );
        CHECK_THROW_MSG( inShader );

        // validate options
        for (auto opt : BitfieldIterate( inShader->options ))
        {
            switch_enum( opt )
            {
                case EShaderOpt::DebugInfo :
                case EShaderOpt::Trace :
                case EShaderOpt::FnProfiling :
                case EShaderOpt::TimeHeatMap :
                case EShaderOpt::Optimize :
                case EShaderOpt::OptimizeSize :
                case EShaderOpt::StrongOptimization :
                case EShaderOpt::WarnAsError :          break;  // ok
                case EShaderOpt::Unknown :
                case EShaderOpt::_ShaderTrace_Mask :
                case EShaderOpt::_Last :
                case EShaderOpt::All :
                default :                               CHECK_THROW_MSG( false, "unknown shader option" );
            }
            switch_end
        }

        ShaderSrcKey    key;
        key.source      = inShader->GetSource();
        key.resources   = resources;
        key.type        = inShader->type;
        key.version     = version;
        key.options     = inShader->options;
        key.include     = Array<String>{ include };
        _SetAndSortDefines( OUT key.defines, String{defines} << inShader->GetDefines() );

        // find in existing shader source
        {
            auto    iter = _shaderSrcMap.find( key );
            if ( iter != _shaderSrcMap.end() )
            {
                outShader = iter->second;
                return;
            }
        }

        // compile shader
        CompiledShader  compiled;
        _CompileShaderGLSL( key, features, debugDSIndex, inShader->GetPath(), inShader->GetEntry(), OUT compiled );

        const auto  ToMetalBytecode = [&] (ShaderUID (PipelineStorage::*fn) (MetalBytecode_t, const ShaderBytecode::SpecConstants_t &)) -> CompiledShaderPtr
        {{
            CHECK_THROW( metalCompiler );

            // convert SPIRV to Metal
            String  msl;
            {
                SpirvBytecode_t*    spirv_ptr = UnionGet<SpirvBytecode_t>( compiled.data );
                CHECK_THROW_MSG( spirv_ptr != null );

                SpirvBytecode_t     spirv = RVRef(*spirv_ptr);
                compiled.data = Default;

                MetalCompiler::SpirvToMslConfig config;
                config.shaderType   = inShader->type;
                config.version      = sprvToMslVersion;
                config.useArgBuffer = useMetalArgBuffer;

                CHECK_THROW_MSG( metalCompiler->SpirvToMsl( config, RVRef(spirv), INOUT compiled.reflection, OUT msl ));
            }

            CHECK( inShader->GetSpec().size() <= compiled.reflection.layout.specConstants.size() );

            // compile MSL to bytecode
            {
                MetalCompiler::Input    in;
                in.target               = target;
                in.options              = inShader->options;
                in.version              = sprvToMslVersion;
                in.shaderType           = inShader->type;
                in.source               = msl;
                in.enablePreprocessing  = false;

                MetalBytecode_t     mtbc;
                String              log;

                if ( not metalCompiler->Compile( in, OUT mtbc, OUT log ))
                {
                    CHECK_THROW_MSG( false, "Failed to compile shader:\n"s << log );
                }

                if ( not log.empty() )
                {
                    AE_LOG_DBG( "Shader compiled with warnings:\n"s << log );
                }

                compiled.data = RVRef(mtbc);
            }

            auto    cs_iter     = _compiledShaders.insert( RVRef(compiled) ).first;
            auto*   mtbc_ptr    = UnionGet<MetalBytecode_t>( cs_iter->data );
            CHECK_THROW_MSG( mtbc_ptr != null );

            cs_iter->uid = ((*pplnStorage).*fn)( *mtbc_ptr, cs_iter->reflection.layout.specConstants );

            auto [src_it, src_inserted] = _shaderSrcMap.emplace( RVRef(key), &(*cs_iter) );
            CHECK( src_inserted );

            return src_it->second;
        }};


        if ( target == ECompilationTarget::Vulkan )
        {
            auto    cs_iter = _compiledShaders.insert( RVRef(compiled) ).first;
            auto*   spirv   = UnionGet<SpirvBytecode_t>( cs_iter->data );
            auto*   dbg_spv = UnionGet<SpirvWithTrace>( cs_iter->data );

            CHECK_THROW_MSG( (spirv != null) != (dbg_spv != null) );

            if ( dbg_spv != null )
                cs_iter->uid = pplnStorage->AddSpirvShader( *dbg_spv, cs_iter->reflection.layout.specConstants );
            else
                cs_iter->uid = pplnStorage->AddSpirvShader( *spirv, cs_iter->reflection.layout.specConstants );

            auto [src_it, src_inserted] = _shaderSrcMap.emplace( RVRef(key), &(*cs_iter) );
            CHECK( src_inserted );

            outShader = src_it->second;
        }
        else
        if ( target == ECompilationTarget::Metal_iOS )
        {
            CHECK_THROW_MSG( AllBits( sprvToMslVersion, EShaderVersion::_Metal_iOS, EShaderVersion::_Mask ));

            outShader = ToMetalBytecode( &PipelineStorage::AddMsliOSShader );
        }
        else
        if ( target == ECompilationTarget::Metal_Mac )
        {
            CHECK_THROW_MSG( AllBits( sprvToMslVersion, EShaderVersion::_Metal_Mac, EShaderVersion::_Mask ));

            outShader = ToMetalBytecode( &PipelineStorage::AddMslMacShader );
        }
        else
        {
            CHECK_THROW_MSG( false, "unsupported compilation target" );
        }
    }

} // AE::PipelineCompiler
