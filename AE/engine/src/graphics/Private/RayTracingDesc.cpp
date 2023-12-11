// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/RayTracingDesc.h"
#include "graphics/Public/GraphicsImpl.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
#endif
#ifdef AE_ENABLE_METAL
# include "graphics/Metal/MEnumCast.h"
#endif

namespace AE::Graphics
{

/*
=================================================
    RTSceneBuild::Instance::ctor
=================================================
*/
#ifdef AE_ENABLE_VULKAN
    RTSceneBuild::Instance::Instance () __NE___ :
        transform           { RTMatrixStorage::Identity() },
        instanceCustomIndex { 0 },
        mask                { 0xFF },
        instanceSBTOffset   { 0 },
        flags               { 0 },
        rtas                { Default }
    {}

#elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
    RTSceneBuild::Instance::Instance () __NE___ :
        transform           { RTMatrixStorage::Identity() },
        options             { 0 },
        mask                { UMax },
        instanceSBTOffset   { 0 },
        rtasIndex           { UMax }
    {}

#else
#   error not implemented
#endif

/*
=================================================
    RTSceneBuild::Instance::SetFlags
=================================================
*/
    RTSceneBuild::Instance&  RTSceneBuild::Instance::SetFlags (ERTInstanceOpt value) __NE___
    {
        #ifdef AE_ENABLE_VULKAN
            this->flags = VEnumCast( value );

        #elif defined(AE_ENABLE_METAL)
            this->options = uint(MEnumCast( value ));

        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            this->options = uint(value);

        #else
        #   error not implemented
        #endif
        return *this;
    }

/*
=================================================
    RTSceneBuild::SetGeometry
=================================================
*/
#ifdef AE_ENABLE_VULKAN
    bool  RTSceneBuild::SetGeometry (RTGeometryID id, INOUT Instance &inst) __NE___
    {
        ASSERT( id );
        ASSERT( inst.rtas == Default );

        auto*   geom = GraphicsScheduler().GetResourceManager().GetResource( id, False{"don't inc ref"}, True{"quiet"} );
        if_likely( geom != null )
        {
            CHECK( uniqueGeoms.insert( id ).first != null );

            inst.rtas = geom->GetDeviceAddress();
            return true;
        }
        return false;
    }

#elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
    bool  RTSceneBuild::SetGeometry (RTGeometryID id, INOUT Instance &inst) __NE___
    {
        ASSERT( id );
        ASSERT( inst.rtasIndex == UMax );

        if_likely( auto it = uniqueGeoms.insert( id ).first;  it != null )
        {
            inst.rtasIndex = uint(uniqueGeoms.IndexOf( it ));
            return true;
        }
        return false;
    }

#else
#   error not implemented
#endif

} // AE::Graphics
