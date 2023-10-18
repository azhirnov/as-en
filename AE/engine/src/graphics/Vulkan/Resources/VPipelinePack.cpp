// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics/Vulkan/Resources/VPipelinePack.h"
# include "graphics/Private/PipelinePack.cpp.h"

#ifdef AE_ENABLE_GLSL_TRACE
# include "ShaderTrace.h"
#endif

namespace AE::Graphics
{
/*
=================================================
    Create
=================================================
*/
    bool  VPipelinePack::Create (VResourceManager &resMngr, const PipelinePackDesc &desc) __NE___
    {
        DRC_EXLOCK( _drCheck );

        CATCH_ERR(
            _sbtAllocator = desc.sbtAllocator;

            return _Create( resMngr, desc );
        )
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VPipelinePack::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        {
            auto&   dev = resMngr.GetDevice();
            auto*   ptr = _shaders.Get<1>();

            for (uint i = 0, cnt = _shaders.Get<0>(); i < cnt; ++i)
            {
                DeferExLock lock{ ptr[i].guard };
                CHECK( lock.try_lock() );

                if ( ptr[i].module != Default )
                    dev.vkDestroyShaderModule( dev.GetVkDevice(), ptr[i].module, null );

                ptr[i].dbgTrace.reset();
            }
        }

        _Destroy( resMngr );
    }

/*
=================================================
    _LoadDepthStencilStates
=================================================
*/
    bool  VPipelinePack::_LoadDepthStencilStates (VResourceManager &, Serializing::Deserializer &des) __NE___
    {
        return _LoadDepthStencilStatesImpl( des );
    }

/*
=================================================
    _LoadRenderPasses
=================================================
*/
    bool  VPipelinePack::_LoadRenderPasses (VResourceManager &resMngr, Bytes offset, Bytes size) __Th___
    {
        CHECK_ERR( _file->SeekSet( offset ));

        auto    mem_stream = MakeRC<MemRStream>();
        CHECK_ERR( mem_stream->LoadRemaining( *_file, size ));

        Serializing::Deserializer   des{ RVRef(mem_stream) };
        CHECK_ERR( not des.stream.Empty() );

        uint    hdr_name    = 0;
        uint    version     = 0;
        CHECK_ERR( des( OUT hdr_name, OUT version ));
        CHECK_ERR( hdr_name == RenderPassPack_Name and version == RenderPassPack_Version );

        uint    compat_rp_count = 0;
        CHECK_ERR( des( OUT compat_rp_count ));
        CHECK_ERR( compat_rp_count <= SerializableRenderPassInfo::MaxCompatCount );

        for (uint i = 0; i < compat_rp_count; ++i)
        {
            SerializableRenderPassInfo  rp_info;
            CHECK_ERR( rp_info.Deserialize( des ));

            bool    supported = true;
            for (auto& fs_name : rp_info.features)
            {
                supported &= (_unsupportedFS->count( fs_name ) == 0);
                ASSERT( _allFeatureSets->count( fs_name ));
            }

            uint    vk_ver      = 0;
            uint    spec_count  = 0;
            CHECK_ERR( des( vk_ver, spec_count ));
            CHECK_ERR( vk_ver == RenderPassPack_VkRpBlock );
            CHECK_ERR( spec_count > 0 );
            CHECK_ERR( spec_count <= SerializableRenderPassInfo::MaxSpecCount );

            SerializableVkRenderPass    vk_compat;
            CHECK_ERR( vk_compat.Deserialize( _surfaceFormat, des ));

            RenderPassID    compat_id;
            if ( supported )
            {
                compat_id = resMngr.CreateRenderPass( rp_info, vk_compat, Default DEBUG_ONLY(, resMngr.HashToName( rp_info.name ))).Release();
                CHECK_ERR( compat_id );
                CHECK( _renderPassRefs->compatMap.emplace( rp_info.name, compat_id ).second );  // throw
            }
            else{
                AE_LOG_DBG( "Render pass '"s << resMngr.HashToName( rp_info.name ) << "' is NOT supported" );
            }

            for (uint j = 0; j < spec_count; ++j)
            {
                SerializableVkRenderPass    vk_rp;
                CHECK_ERR( vk_rp.Deserialize( _surfaceFormat, des ));

                if ( supported )
                {
                    RenderPassID    rp_id = resMngr.CreateRenderPass( rp_info, vk_rp, compat_id
                                                                      DEBUG_ONLY(, resMngr.HashToName( vk_rp.Name() ))).Release();
                    CHECK_ERR( rp_id );
                    CHECK( _renderPassRefs->specMap.emplace( vk_rp.Name(), rp_id ).second );    // throw
                }
            }
        }

        CHECK( des.IsEnd() );
        return true;
    }

/*
=================================================
    _CreateShader
=================================================
*/
    bool  VPipelinePack::_CreateShader (const VDevice &dev, INOUT const ShaderModule &shader) C_NE___
    {
        ShaderBytecode      code;
        code.offset     = _shaderOffset + Bytes{shader.offset};
        code.dataSize   = Bytes{shader.dataSize};
        code.typeIdx    = shader.shaderTypeIdx;

        {
            EXLOCK( _fileGuard );
            CHECK_ERR( code.ReadData( *_file ));
        }

        auto*   spirv_ptr   = UnionGet<SpirvBytecode_t>( code.code );
        auto*   dbg_ptr     = UnionGet<SpirvWithTrace>( code.code );
        CHECK_ERR( (spirv_ptr != null) != (dbg_ptr != null) );

        if ( dbg_ptr != null )
        {
            spirv_ptr       = &dbg_ptr->bytecode;
            shader.dbgTrace = RVRef(dbg_ptr->trace);
        }

        CHECK_ERR( spirv_ptr != null );

        VkShaderModuleCreateInfo    info = {};
        info.sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pCode      = spirv_ptr->data();
        info.codeSize   = usize(ArraySizeOf( *spirv_ptr ));

        VK_CHECK_ERR( dev.vkCreateShaderModule( dev.GetVkDevice(), &info, null, OUT &shader.module ));
        return true;
    }

/*
=================================================
    _GetShader
=================================================
*/
    VPipelinePack::ShaderModuleRef  VPipelinePack::_GetShader (const VResourceManager &resMngr, ShaderUID uid, EShader shaderType) C_NE___
    {
        CHECK_ERR( (uid & ShaderUID::_Mask) == ShaderUID::SPIRV );

        const uint  idx = uint(uid) & ~uint(ShaderUID::_Mask);
        CHECK_ERR( idx < _shaders.Get<0>() );

        auto&   shader = _shaders.Get<1>()[ idx ];
        shader.guard.lock_shared();

        if_likely( shader.module != Default )
        {
            ShaderModuleRef     res;
            res.module          = shader.module;
            res.shaderConstants = &shader.constants;
            res.stage           = (shaderType != Default ? VEnumCast( shaderType ) : Zero);
            res.dbgTrace        = shader.dbgTrace.get();

            shader.guard.unlock_shared();
            return res;
        }

        // load shader
        shader.guard.unlock_shared();
        EXLOCK( shader.guard );

        // shader may be already loaded in another thread
        if ( shader.module == Default )
        {
            // TODO: task ?
            if_unlikely( not _CreateShader( resMngr.GetDevice(), INOUT shader ))
                return {};
        }

        ShaderModuleRef     res;
        res.module          = shader.module;
        res.shaderConstants = &shader.constants;
        res.stage           = (shaderType != Default ? VEnumCast( shaderType ) : Zero);
        res.dbgTrace        = shader.dbgTrace.get();
        return res;
    }

/*
=================================================
    _CreateSampler
=================================================
*/
    SamplerID  VPipelinePack::_CreateSampler (VResourceManager &resMngr, const SamplerDesc &desc,
                                              const Optional<SamplerYcbcrConversionDesc> &ycbcrDesc, StringView dbgName) __NE___
    {
        VkSamplerCreateInfo     vk_desc;
        VSampler::ConvertSampler( desc, OUT vk_desc );

        VkSamplerYcbcrConversionCreateInfo  vk_ycbcr_desc;
        if ( ycbcrDesc.has_value() )
            VSampler::ConvertSampler( *ycbcrDesc, OUT vk_ycbcr_desc );

        return resMngr.CreateSampler( vk_desc, ycbcrDesc.has_value() ? &vk_ycbcr_desc : null, dbgName ).Release();
    }

/*
=================================================
    _CreateDescriptorSetLayout
=================================================
*/
    Strong<DescriptorSetLayoutID>  VPipelinePack::_CreateDescriptorSetLayout (VResourceManager          &resMngr,
                                                                              const Uniforms_t          &uniforms,
                                                                              ArrayView<SamplerID>       samplers,
                                                                              const UniformOffsets_t    &offsets,
                                                                              EDescSetUsage              usage,
                                                                              EShaderStages              stages,
                                                                              StackAllocator_t          &stackAlloc) __NE___
    {
        Unused( stages );

        VkSampler *     vk_samplers = null;

        if ( not samplers.empty() )
        {
            vk_samplers = stackAlloc.Allocate< VkSampler >( samplers.size() );
            CHECK_ERR( vk_samplers != null );

            for (usize i = 0; i < samplers.size(); ++i)
            {
                const VSampler*     samp = resMngr.GetResource( samplers[i] );
                CHECK_ERR( samp != null );

                vk_samplers[i] = samp->Handle();
            }
        }

        VDescriptorSetLayout::CreateInfo    ci;
        ci.uniforms         = uniforms;
        ci.samplerStorage   = ArrayView{ vk_samplers, samplers.size() };
        ci.unOffsets        = offsets;
        ci.usage            = usage;
        ci.dbgName          = Default;

        return resMngr.CreateDescriptorSetLayout( ci );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    _CreateRTShaderBinding
=================================================
*/
    RTShaderBindingID  VPipelinePack::RenderTech::_CreateRTShaderBinding (ResMngr_t                                                 &resMngr,
                                                                          const PipelineCompiler::SerializableRTShaderBindingTable  &desc,
                                                                          StringView                                                 dbgName) __NE___
    {
        auto    ppln_id = GetRayTracingPipeline( PipelineName{desc.pplnName} );
        CHECK_ERR( ppln_id );

        auto*   rt_ppln = resMngr.GetResource( ppln_id );
        CHECK_ERR( rt_ppln != null );

        auto&           dev             = resMngr.GetDevice();
        const auto&     rt_props        = dev.GetVProperties().rayTracingPipelineProps;
        const Bytes     handle_size     {rt_props.shaderGroupHandleSize};
        const Bytes     handle_stride   {AlignUp( rt_props.shaderGroupHandleSize, rt_props.shaderGroupHandleAlignment )};
        const Bytes     offset_align    {rt_props.shaderGroupBaseAlignment};
        Bytes           sbt_size;

        sbt_size = AlignUp( sbt_size +                        handle_stride, offset_align );
        sbt_size = AlignUp( sbt_size + desc.miss.size()     * handle_stride, offset_align );
        sbt_size = AlignUp( sbt_size + desc.hit.size()      * handle_stride, offset_align );
        sbt_size = AlignUp( sbt_size + desc.callable.size() * handle_stride, offset_align );
        CHECK_ERR( sbt_size > 0 );

        GAutorelease    buf_id = resMngr.CreateBuffer( BufferDesc{ sbt_size, EBufferUsage::ShaderBindingTable | EBufferUsage::Transfer }.SetMemory( EMemoryType::Unified ),
                                                        "SBT buffer", _pack._sbtAllocator );
        CHECK_ERR( buf_id );

        VulkanMemoryObjInfo     mem_info;
        CHECK_ERR( resMngr.GetMemoryInfo( buf_id.Get(), OUT mem_info ));
        CHECK_ERR( mem_info.mappedPtr != null );

        RTShaderBindingTable    sbt;
        Bytes                   offset;
        DeviceAddress           addr    = resMngr.GetResource( buf_id.Get() )->GetDeviceAddress();

        CHECK_ERR( addr != Default );
        CHECK_ERR( rt_ppln->CopyHandle( dev, desc.raygen.index, OUT mem_info.mappedPtr + offset, handle_size ));

        sbt.raygen.deviceAddress    = BitCast<VkDeviceAddress>( addr + offset );
        sbt.raygen.size             = VkDeviceSize(handle_stride);
        sbt.raygen.stride           = VkDeviceSize(handle_stride);
        offset                      = AlignUp( offset + handle_stride, offset_align );

        for (usize i = 0; i < desc.miss.size(); ++i) {
            rt_ppln->CopyHandle( dev, desc.miss[i].index, OUT mem_info.mappedPtr + offset + handle_stride*i, handle_size );
        }

        sbt.miss.deviceAddress      = desc.miss.empty() ? 0 : BitCast<VkDeviceAddress>( addr + offset );
        sbt.miss.size               = VkDeviceSize(handle_stride * desc.miss.size());
        sbt.miss.stride             = VkDeviceSize(handle_stride);
        offset                      = AlignUp( offset + desc.miss.size() * handle_stride, offset_align );

        for (usize i = 0; i < desc.hit.size(); ++i) {
            rt_ppln->CopyHandle( dev, desc.hit[i].index, OUT mem_info.mappedPtr + offset + handle_stride*i, handle_size );
        }

        sbt.hit.deviceAddress       = desc.hit.empty() ? 0 : BitCast<VkDeviceAddress>( addr + offset );
        sbt.hit.size                = VkDeviceSize(handle_stride * desc.hit.size());
        sbt.hit.stride              = VkDeviceSize(handle_stride);
        offset                      = AlignUp( offset + desc.hit.size() * handle_stride, offset_align );

        for (usize i = 0; i < desc.callable.size(); ++i) {
            rt_ppln->CopyHandle( dev, desc.callable[i].index, OUT mem_info.mappedPtr + offset + handle_stride*i, handle_size );
        }

        sbt.callable.deviceAddress  = desc.callable.empty() ? 0 : BitCast<VkDeviceAddress>( addr + offset );
        sbt.callable.size           = VkDeviceSize(handle_stride * desc.callable.size());
        sbt.callable.stride         = VkDeviceSize(handle_stride);
        offset                      = AlignUp( offset + desc.callable.size() * handle_stride, offset_align );

        ASSERT( IsMultipleOf( sbt.raygen.deviceAddress, offset_align ));
        ASSERT( IsMultipleOf( sbt.miss.deviceAddress, offset_align ));
        ASSERT( IsMultipleOf( sbt.hit.deviceAddress, offset_align ));
        ASSERT( IsMultipleOf( sbt.callable.deviceAddress, offset_align ));
        CHECK_ERR( offset == sbt_size );

        RTShaderBindingDesc     sbt_desc;
        sbt_desc.maxRayTypes    = Max( desc.numRayTypes, desc.miss.size() );
        sbt_desc.maxInstances   = uint(SafeDiv( desc.hit.size(), desc.numRayTypes ));
        sbt_desc.maxCallable    = desc.callable.size();

        ASSERT( desc.hit.empty() or IsMultipleOf( desc.hit.size(), desc.numRayTypes ));
        ASSERT( sbt_desc.maxRayTypes == Max( desc.numRayTypes, desc.miss.size() )); // overflow
        ASSERT( sbt_desc.maxCallable == desc.callable.size() );                     // overflow
        ASSERT( sbt_desc.maxInstances * desc.numRayTypes == desc.hit.size() );      // overflow

        auto    id = resMngr.CreateRTShaderBinding( VRTShaderBindingTable::CreateInfo{
                                                        sbt, buf_id, ppln_id, sbt_desc, dbgName
                                                    }).Release();
        return id;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
