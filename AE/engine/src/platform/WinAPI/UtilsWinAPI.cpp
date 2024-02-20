// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "platform/WinAPI/UtilsWinAPI.h"

namespace AE::App
{

/*
=================================================
    SetMonitorNames
=================================================
*/
    void  UtilsWinAPI::SetMonitorNames (INOUT ApplicationBase::Monitors_t &monitors) __NE___
    {
        Array<DISPLAYCONFIG_PATH_INFO>  paths;
        Array<DISPLAYCONFIG_MODE_INFO>  modes;
        const UINT32                    flags   = QDC_ONLY_ACTIVE_PATHS;
        UINT32                          path_count, mode_count;

        CHECK_ERRV( ::GetDisplayConfigBufferSizes( flags, OUT &path_count, &mode_count ) == ERROR_SUCCESS );    // winvista
        CHECK_ERRV( monitors.size() == path_count );

        paths.resize( path_count );
        modes.resize( mode_count );

        CHECK_ERRV( ::QueryDisplayConfig( flags, INOUT &path_count, OUT paths.data(), INOUT &mode_count, OUT modes.data(), null ) == ERROR_SUCCESS );   // win7

        paths.resize( path_count );
        modes.resize( mode_count );

        for (usize i = 0 ; i < paths.size(); ++i)
        {
            const auto& src = paths[i].targetInfo;

            DISPLAYCONFIG_TARGET_DEVICE_NAME    target_name = {};
            target_name.header.adapterId    = src.adapterId;
            target_name.header.id           = src.id;
            target_name.header.type         = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
            target_name.header.size         = sizeof(target_name);

            CHECK_ERRV( ::DisplayConfigGetDeviceInfo( INOUT &target_name.header ) == ERROR_SUCCESS );   // winvista

            auto&   dst = monitors[i];

            dst.freq        = (src.refreshRate.Numerator + src.refreshRate.Denominator/2) / src.refreshRate.Denominator;
            dst.isExternal  = AnyBits( target_name.outputTechnology, DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL );

            if ( target_name.flags.friendlyNameFromEdid )
                dst.name = Monitor::Name_t{ToString( WStringView{target_name.monitorFriendlyDeviceName} )};
        }
    }


} // AE::App

#endif // AE_PLATFORM_WINDOWS
