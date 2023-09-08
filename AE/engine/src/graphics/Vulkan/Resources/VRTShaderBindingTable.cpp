// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VRTShaderBindingTable.h"
# include "graphics/Vulkan/VResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VRTShaderBindingTable::~VRTShaderBindingTable () __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( _bufferId == Default );
        ASSERT( _pipelineId == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VRTShaderBindingTable::Create (VResourceManager &resMngr, const CreateInfo &ci) __NE___
    {
        DRC_EXLOCK( _drCheck );

        CHECK_ERR( _bufferId == Default and _pipelineId == Default );
        CHECK_ERR( resMngr.GetFeatureSet().rayTracingPipeline == EFeature::RequireTrue );

        _sbt            = ci.sbt;
        _bufferId       = resMngr.AcquireResource( ci.bufferId );
        _pipelineId     = resMngr.AcquireResource( ci.pipelineId );
        _desc           = ci.desc;

        CHECK_ERR( _bufferId != Default and _pipelineId != Default );

        DEBUG_ONLY( _debugName = ci.dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VRTShaderBindingTable::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        resMngr.ImmediatelyRelease( INOUT _bufferId );
        resMngr.ImmediatelyRelease( INOUT _pipelineId );

        _sbt = Default;

        DEBUG_ONLY( _debugName.clear(); )
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
