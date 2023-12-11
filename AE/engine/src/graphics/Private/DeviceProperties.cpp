// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/DeviceProperties.h"
#include "graphics/Private/EnumToString.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VDevice.h"
#endif
#ifdef AE_ENABLE_METAL
# include "graphics/Metal/MDevice.h"
#endif

namespace AE::Graphics
{
namespace
{
/*
=================================================
    InitAppleShaderHWProperties
=================================================
*/
    static void  InitAppleShaderHWProperties (OUT DeviceProperties::ShaderHWProperties &shaderHW, StringView devName)
    {
        if ( HasSubString( devName, "M2 Max" ))
        {
            shaderHW.cores          = 30;   // or 38
            shaderHW.warpsPerCore   = 32;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "M2 Pro" ))
        {
            shaderHW.cores          = 16;   // or 19
            shaderHW.warpsPerCore   = 32;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "M2" ))
        {
            shaderHW.cores          = 8;    // or 10
            shaderHW.warpsPerCore   = 32;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "A16" ))
        {
            shaderHW.cores          = 5;
            shaderHW.warpsPerCore   = 32;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "A15" ))
        {
            shaderHW.cores          = 4;    // or 5
            shaderHW.warpsPerCore   = 32;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }
        //-----------------------------


        if ( HasSubString( devName, "M1 Ultra" ))
        {
            shaderHW.cores          = 48;   // or 64
            shaderHW.warpsPerCore   = 16;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "M1 Max" ))
        {
            shaderHW.cores          = 24;   // or 32
            shaderHW.warpsPerCore   = 16;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "M1 Pro" ))
        {
            shaderHW.cores          = 14;   // or 16
            shaderHW.warpsPerCore   = 16;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "M1" ))
        {
            shaderHW.cores          = 8;    // or 7
            shaderHW.warpsPerCore   = 16;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "A14" ))
        {
            shaderHW.cores          = 4;
            shaderHW.warpsPerCore   = 16;   // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }
        //-----------------------------


        if ( HasSubString( devName, "A13" ) or HasSubString( devName, "A12" ))
        {
            shaderHW.cores          = 4;
            shaderHW.warpsPerCore   = 8;    // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }

        if ( HasSubString( devName, "A11" ))
        {
            shaderHW.cores          = 3;
            shaderHW.warpsPerCore   = 8;    // EU
            shaderHW.threadsPerWarp = 8;    // ALU
            return;
        }
    }
}
//-----------------------------------------------------------------------------


/*
=================================================
    CompareWithConstant
=================================================
*/
    bool  DeviceProperties::CompareWithConstant (AnyTypeCRef vkExt_mtlFS) C_NE___
    {
        StaticAssert( sizeof(DeviceProperties) == 88 );

        const auto  CheckLimitLess = [] (auto curr, auto constant, const char* name)
        {{
            if ( curr <= constant )
                return true;

            AE_LOGI( "Current device limit for '"s << name << "' (" << ToString(curr) << ") must be <= to constant limit (" << ToString(constant) << ')' );
            return false;
        }};

        const auto  CheckLimitGreater = [] (auto curr, auto constant, const char* name)
        {{
            if ( curr >= constant )
                return true;

            AE_LOGI( "Current device limit for '"s << name << "' (" << ToString(curr) << ") must be >= to constant limit (" << ToString(constant) << ')' );
            return false;
        }};


        #ifdef AE_ENABLE_VULKAN
        CHECK_ERR( vkExt_mtlFS.Is< VDevice::VExtensions >() );
        const auto& vk_ext          = vkExt_mtlFS.As< VDevice::VExtensions >();
        const bool  accel_struct    = vk_ext.accelerationStructure;
        #endif

        #ifdef AE_ENABLE_METAL
        CHECK_ERR( vkExt_mtlFS.Is< MFeatureSet >() );
        const auto& mtl_feats       = vkExt_mtlFS.As< MFeatureSet >().features;
        const bool  accel_struct    = mtl_feats.accelerationStructure();
        #endif

        #ifdef AE_ENABLE_REMOTE_GRAPHICS
        Unused( vkExt_mtlFS );
        const bool  accel_struct    = true;
        #endif

        bool    result = true;

        StaticAssert( sizeof(res) == 24 );
        {
            #undef CMP_L
            #undef CMP_G
            #define CMP_L( _name_ ) result &= CheckLimitLess(    res._name_, DeviceLimits.res._name_, AE_TOSTRING(_name_) )
            #define CMP_G( _name_ ) result &= CheckLimitGreater( res._name_, DeviceLimits.res._name_, AE_TOSTRING(_name_) )

            CMP_L( minUniformBufferOffsetAlign );
            CMP_L( minStorageBufferOffsetAlign );
            CMP_L( minThreadgroupMemoryLengthAlign );
            CMP_G( maxVerticesPerRenderPass );
            CMP_L( minVertexBufferOffsetAlign );
            CMP_L( minVertexBufferElementsAlign );
            CMP_L( minUniformTexelBufferOffsetAlign );
            CMP_L( minStorageTexelBufferOffsetAlign );
            CMP_G( maxUniformBufferRange );
            CMP_G( maxBoundDescriptorSets );
            CMP_L( minMemoryMapAlign );
            CMP_L( minNonCoherentAtomSize );
            CMP_L( minBufferCopyOffsetAlign );
            CMP_L( minBufferCopyRowPitchAlign );
        }

        StaticAssert( sizeof(rayTracing) == 48 );
        if ( accel_struct )
        {
            #undef CMP_L
            #undef CMP_G
            #define CMP_L( _name_ ) result &= CheckLimitLess(    rayTracing._name_, DeviceLimits.rayTracing._name_, AE_TOSTRING(_name_) )
            #define CMP_G( _name_ ) result &= CheckLimitGreater( rayTracing._name_, DeviceLimits.rayTracing._name_, AE_TOSTRING(_name_) )

            CMP_L( vertexDataAlign );
            CMP_L( vertexStrideAlign );
            CMP_L( indexDataAlign );
            CMP_L( aabbDataAlign );
            CMP_L( aabbStrideAlign );
            CMP_L( transformDataAlign );
            CMP_L( instanceDataAlign );
            CMP_L( instanceStrideAlign );
            CMP_L( scratchBufferAlign );
            CMP_G( maxGeometries );
            CMP_G( maxInstances );
            CMP_G( maxPrimitives );
            CMP_G( maxRecursion );
        }
        // ignore shaderHW

        return result;
    }

/*
=================================================
    InitVulkan
=================================================
*/
#ifdef AE_ENABLE_VULKAN
    void  DeviceProperties::InitVulkan (AnyTypeCRef vkExt, AnyTypeCRef vkProps) __NE___
    {
        CHECK_ERRV( vkExt.Is< VDevice::VExtensions >() );
        CHECK_ERRV( vkProps.Is< VDevice::VProperties >() );

        const auto&     vk_ext      = vkExt.As< VDevice::VExtensions >();
        const auto&     vk_props    = vkProps.As< VDevice::VProperties >();

        // resource alignment
        {
            StaticAssert( sizeof(res) == 24 );

            const auto&     limits  = vk_props.properties.limits;

            res.minUniformBufferOffsetAlign         = POTBytes{ limits.minUniformBufferOffsetAlignment };
            res.minStorageBufferOffsetAlign         = POTBytes{ limits.minStorageBufferOffsetAlignment };
            res.minThreadgroupMemoryLengthAlign     = POTBytes{ 1_b };      // not supported
            res.minVertexBufferOffsetAlign          = POTBytes{ 1_b };      // not defined
            #ifdef AE_PLATFORM_ANDROID
                res.maxVerticesPerRenderPass        = POTValue( 2ull<<30 ); // no way to query
                res.minVertexBufferElementsAlign    = 4;
            #else
                res.maxVerticesPerRenderPass        = POTValue( UMax );     // not defined
                res.minVertexBufferElementsAlign    = 1;
            #endif
            res.minUniformTexelBufferOffsetAlign    = POTBytes{ limits.minTexelBufferOffsetAlignment };
            res.minStorageTexelBufferOffsetAlign    = POTBytes{ limits.minTexelBufferOffsetAlignment };
            res.maxUniformBufferRange               = Bytes{limits.maxUniformBufferRange};
            res.maxBoundDescriptorSets              = limits.maxBoundDescriptorSets;
            res.minMemoryMapAlign                   = POTBytes{ limits.minMemoryMapAlignment };
            res.minNonCoherentAtomSize              = POTBytes{ limits.nonCoherentAtomSize };
            res.minBufferCopyOffsetAlign            = POTBytes{ limits.optimalBufferCopyOffsetAlignment };
            res.minBufferCopyRowPitchAlign          = POTBytes{ limits.optimalBufferCopyRowPitchAlignment };

            if ( vk_ext.texelBufferAlignment and vk_props.texelBufferAlignmentFeats.texelBufferAlignment )
            {
                const auto& tba_props = vk_props.texelBufferAlignmentProps;
                res.minUniformTexelBufferOffsetAlign    = POTBytes{ tba_props.uniformTexelBufferOffsetAlignmentBytes };
                res.minStorageTexelBufferOffsetAlign    = POTBytes{ tba_props.storageTexelBufferOffsetAlignmentBytes };
            }
        }

        if ( vk_ext.accelerationStructure )
        {
            StaticAssert( sizeof(rayTracing) == 48 );

            const auto&     ac_props    = vk_props.accelerationStructureProps;
            const auto&     rt_props    = vk_props.rayTracingPipelineProps;

            rayTracing.vertexDataAlign          = POTBytes{ 1_b };
            rayTracing.vertexStrideAlign        = POTBytes{ 1_b };
            rayTracing.indexDataAlign           = POTBytes{ 1_b };
            rayTracing.aabbDataAlign            = POTBytes{ 8_b };  // from specs
            rayTracing.aabbStrideAlign          = POTBytes{ 1_b };
            rayTracing.transformDataAlign       = POTBytes{ 16_b }; // from specs
            rayTracing.instanceDataAlign        = POTBytes{ 16_b }; // from specs
            rayTracing.instanceStrideAlign      = POTBytes{ 1_b };
            rayTracing.scratchBufferAlign       = POTBytes{ ac_props.minAccelerationStructureScratchOffsetAlignment };

            rayTracing.maxGeometries            = ac_props.maxGeometryCount;
            rayTracing.maxInstances             = ac_props.maxInstanceCount;
            rayTracing.maxPrimitives            = ac_props.maxPrimitiveCount;

            rayTracing.maxRecursion             = rt_props.maxRayRecursionDepth;
            rayTracing.maxDispatchInvocations   = rt_props.maxRayDispatchInvocationCount;
        }

        // shader HW
        {
            StaticAssert( sizeof(shaderHW) == sizeof(uint)*3 );

            // AMD
            if ( vk_ext.shaderCorePropsAMD )
            {
                const auto& props       = vk_props.shaderCorePropsAMDProps;
                shaderHW.cores          = props.shaderEngineCount           * props.shaderArraysPerEngineCount  *
                                          props.computeUnitsPerShaderArray  * props.simdPerComputeUnit;
                shaderHW.warpsPerCore   = props.wavefrontsPerSimd;
                shaderHW.threadsPerWarp = props.wavefrontSize;
            }
            else
            // ARM
            if ( vk_ext.shaderCoreBuiltinsARM )
            {
                const auto& props       = vk_props.shaderCoreBuiltinsARMProps;
                shaderHW.cores          = props.shaderCoreCount;
                shaderHW.warpsPerCore   = props.shaderWarpsPerCore;
                shaderHW.threadsPerWarp = 16;
            }
            else
            // NV
            if ( vk_ext.shaderSMBuiltinsNV )
            {
                const auto& props       = vk_props.shaderSMBuiltinsNVProps;
                shaderHW.cores          = props.shaderSMCount;
                shaderHW.warpsPerCore   = props.shaderWarpsPerSM;
                shaderHW.threadsPerWarp = 32;   // TODO
            }
            else
            // Apple
            if ( vk_props.properties.vendorID == 0x0106B )
            {
                InitAppleShaderHWProperties( OUT shaderHW, vk_props.properties.deviceName );
            }
        }
    }
#endif // AE_ENABLE_VULKAN

/*
=================================================
    InitMetal
=================================================
*/
#ifdef AE_ENABLE_METAL
    void  DeviceProperties::InitMetal (AnyTypeCRef mtlFS, StringView devName) __NE___
    {
        CHECK_ERRV( mtlFS.Is< MFeatureSet >() );

        const auto&     mtl_feats = mtlFS.As< MFeatureSet >().features;
        const auto&     mtl_props = mtlFS.As< MFeatureSet >().properties;

        // resource alignment
        {
            StaticAssert( sizeof(res) == 24 );

            res.minUniformBufferOffsetAlign         = POTBytes{ mtl_props.minUniformBufferOffsetAlign };
            res.minStorageBufferOffsetAlign         = POTBytes{ mtl_props.minStorageBufferOffsetAlign };
            res.minThreadgroupMemoryLengthAlign     = POTBytes{ mtl_props.threadgroupMemoryLengthAlignment };
            res.minUniformTexelBufferOffsetAlign    = POTBytes{ 16_b };     // TODO
            res.minStorageTexelBufferOffsetAlign    = POTBytes{ 16_b };     // TODO
            res.maxVerticesPerRenderPass            = POTValue{ UMax };     // not defined
            res.minVertexBufferOffsetAlign          = POTBytes{ 4_b };
            res.minVertexBufferElementsAlign        = 1;                    // TODO
            res.maxUniformBufferRange               = LimitCast<Byte32u>(mtl_props.maxBufferSize);
            res.maxBoundDescriptorSets              = 31;                   // minus VBcount
            res.minMemoryMapAlign                   = POTBytes{ Max( mtl_props.minUniformBufferOffsetAlign, mtl_props.minStorageBufferOffsetAlign )};
            res.minNonCoherentAtomSize              = POTBytes{ res.minMemoryMapAlign };
            res.minBufferCopyOffsetAlign            = POTBytes{ 4_b };
            res.minBufferCopyRowPitchAlign          = POTBytes{ mtl_props.bufferAlignmentForCopyingExistingTextureToBuffer };
        }

        if ( mtl_feats.accelerationStructure() )
        {
            StaticAssert( sizeof(rayTracing) == 48 );

            const POTBytes      buf_align   {mtl_props.minStorageBufferOffsetAlign};

            // In specs:
            //   The vertex must be a multiple of the vertex stride and must be a multiple of 4 bytes.
            //   (Metal validation requires a 16 byte alignment)
            rayTracing.vertexDataAlign      = POTBytes{ 16_b };

            // In specs:
            //   The stride must be at least 12 bytes and must be a multiple of 4 bytes.
            //   (Metal validation requires a 16 byte alignment)
            rayTracing.vertexStrideAlign    = POTBytes{ 16_b };

            // In specs:
            //   Specify an offset that is a multiple of the index data type size and a multiple of the platform's buffer offset alignment.
            //   (Metal validation requires a 32 byte alignment)
            rayTracing.indexDataAlign       = POTBytes{ 32_b }; //Max( SizeOf<uint>, buf_align );

            // In specs:
            //   The stride must be at least 24 bytes, and must be a multiple of 4 bytes.
            rayTracing.aabbStrideAlign      = POTBytes{ 4_b };

            // In specs:
            //   The offset must be a multiple of boundingBoxStride, and must be aligned to the platform's buffer offset alignment.
            rayTracing.aabbDataAlign        = Max( rayTracing.aabbStrideAlign, buf_align );

            rayTracing.transformDataAlign   = POTBytes{ 1_b };  // not supported

            // In specs:
            //   Specify an offset that is a multiple of 64 bytes and a multiple of the platform's buffer offset alignment.
            rayTracing.instanceDataAlign    = Max( POTBytes{64_b}, buf_align );

            // In specs:
            //   The stride must be at least 64 bytes and must be a multiple of 4 bytes.
            rayTracing.instanceStrideAlign  = POTBytes{ 64_b };

            // No requirements in specs
            rayTracing.scratchBufferAlign   = buf_align;

            // https://developer.apple.com/documentation/metal/mtlaccelerationstructureusage/mtlaccelerationstructureusageextendedlimits
            rayTracing.maxGeometries        = 1u << 24;     // can be extended to 1<<30
            rayTracing.maxInstances         = 1u << 24;     // can be extended to 1<<30
            rayTracing.maxPrimitives        = 1u << 28;     // can be extended to 1<<30

            rayTracing.maxRecursion             = 0;    // not supported, yet
            rayTracing.maxDispatchInvocations   = 0;
        }

        // shader HW
        {
            StaticAssert( sizeof(shaderHW) == sizeof(uint)*3 );

            if ( HasSubStringIC( devName, "Apple" ))
                InitAppleShaderHWProperties( OUT shaderHW, devName );
        }
    }
#endif // AE_ENABLE_METAL


/*
=================================================
    Print
=================================================
*/
    void  DeviceProperties::Print () C_NE___
    {
    #ifdef AE_ENABLE_LOGS
        TRY{
            String  str = "\nDeviceProperties:";

            // resource alignment
            {
                StaticAssert( sizeof(res) == 24 );
                str << "\n  ResourceAlignment:"
                    << "\n    minUniformBufferOffsetAlign: . . . " << ToString( Bytes{ res.minUniformBufferOffsetAlign })
                    << "\n    minStorageBufferOffsetAlign:       " << ToString( Bytes{ res.minStorageBufferOffsetAlign })
                    << "\n    minThreadgroupMemoryLengthAlign: . " << ToString( Bytes{ res.minThreadgroupMemoryLengthAlign })
                    << "\n    minUniformTexelBufferOffsetAlign:  " << ToString( Bytes{ res.minUniformTexelBufferOffsetAlign })
                    << "\n    minStorageTexelBufferOffsetAlign:. " << ToString( Bytes{ res.minStorageTexelBufferOffsetAlign })
                    << "\n    maxVerticesPerRenderPass:          " << ToString( ulong(res.maxVerticesPerRenderPass) )
                    << "\n    minVertexBufferOffsetAlign:  . . . " << ToString( Bytes{ res.minVertexBufferOffsetAlign })
                    << "\n    minVertexBufferElementsAlign:      " << ToString( res.minVertexBufferElementsAlign )
                    << "\n    maxUniformBufferRange: . . . . . . " << ToString( res.maxUniformBufferRange )
                    << "\n    maxBoundDescriptorSets:            " << ToString( res.maxBoundDescriptorSets )
                    << "\n    minMemoryMapAlign: . . . . . . . . " << ToString( Bytes{ res.minMemoryMapAlign })
                    << "\n    minNonCoherentAtomSize:            " << ToString( Bytes{ res.minNonCoherentAtomSize })
                    << "\n    minBufferCopyOffsetAlign:  . . . . " << ToString( Bytes{ res.minBufferCopyOffsetAlign })
                    << "\n    minBufferCopyRowPitchAlign:        " << ToString( Bytes{ res.minBufferCopyRowPitchAlign })
                    << "\n  ----";
            }

            // ray tracing
            {
                StaticAssert( sizeof(rayTracing) == 48 );
                str << "\n  RayTracingProperties:"
                    << "\n    vertexDataAlign: . . . . . . . . . " << ToString( Bytes{ rayTracing.vertexDataAlign })
                    << "\n    vertexStrideAlign:                 " << ToString( Bytes{ rayTracing.vertexStrideAlign })
                    << "\n    indexDataAlign:  . . . . . . . . . " << ToString( Bytes{ rayTracing.indexDataAlign })
                    << "\n    aabbDataAlign:                     " << ToString( Bytes{ rayTracing.aabbDataAlign })
                    << "\n    aabbStrideAlign: . . . . . . . . . " << ToString( Bytes{ rayTracing.aabbStrideAlign })
                    << "\n    transformDataAlign:                " << ToString( Bytes{ rayTracing.transformDataAlign })
                    << "\n    instanceDataAlign: . . . . . . . . " << ToString( Bytes{ rayTracing.instanceDataAlign })
                    << "\n    instanceStrideAlign:               " << ToString( Bytes{ rayTracing.instanceStrideAlign })
                    << "\n    scratchBufferAlign:  . . . . . . . " << ToString( Bytes{ rayTracing.scratchBufferAlign })
                    << "\n    maxGeometries:                     " << ToString( rayTracing.maxGeometries )
                    << "\n    maxInstances:  . . . . . . . . . . " << ToString( rayTracing.maxInstances )
                    << "\n    maxPrimitives:                     " << ToString( rayTracing.maxPrimitives )
                    << "\n    maxRecursion:  . . . . . . . . . . " << ToString( rayTracing.maxRecursion )
                    << "\n    maxDispatchInvocations:            " << ToString( rayTracing.maxDispatchInvocations )
                    << "\n  ----";
            }

            // shader HW
            {
                StaticAssert( sizeof(shaderHW) == sizeof(uint)*3 );
                str << "\n  ShaderHWProperties:"
                    << "\n    cores: . . . . . . . . . . . . . . " << ToString( shaderHW.cores )
                    << "\n    warpsPerCore:                      " << ToString( shaderHW.warpsPerCore )
                    << "\n    threadsPerWarp:  . . . . . . . . . " << ToString( shaderHW.threadsPerWarp )
                    << "\n    totalWarps:                        " << ToString( shaderHW.TotalWarps() )
                    << "\n    totalThreads:  . . . . . . . . . . " << ToString( shaderHW.TotalThreads() )
                    << "\n  ----";
            }

            AE_LOGI( str );
        }
        CATCH_ALL()
    #endif
    }

/*
=================================================
    Print
=================================================
*/
    void  DeviceResourceFlags::Print () C_NE___
    {
    #ifdef AE_ENABLE_LOGS
        TRY{
            String  str;

            str << "memory types:";

            for (EMemoryType mem : memTypes) {
                str << "\n  " << ToString( mem );
            }

            AE_LOGI( str );
        }
        CATCH_ALL()
    #endif
    }


} // AE::Graphics
