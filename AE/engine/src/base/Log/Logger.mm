// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/ObjC/NS.h"

#undef null
#import <Cocoa/Cocoa.h>
#define null    nullptr

#include "base/Log/Logger.h"

#if defined(AE_PLATFORM_APPLE) and not defined(AE_CI_BUILD)

namespace AE::Base
{
namespace
{
/*
=================================================
    ShowAlertDialog
=================================================
*/
    static NSModalResponse  ShowAlertDialog (const String &caption, const String &msg)
    {
        @autoreleasepool
        {
            NSString*   ns_caption  = [NSString stringWithUTF8String : caption.c_str()];
            NSString*   ns_msg      = [NSString stringWithUTF8String : msg.c_str()];

            NSAlert *alert = [[[NSAlert alloc] init] autorelease];
            [alert setMessageText : ns_caption ];
            [alert setInformativeText : ns_msg ];
            [alert addButtonWithTitle : @"Ignore"];
            [alert addButtonWithTitle : @"Retry"];
            [alert addButtonWithTitle : @"Abort"];
            [alert setAlertStyle : NSAlertStyleWarning];

            return [alert runModal];
        }
    }
}

/*
=================================================
    DialogLogOutput
=================================================
*/
    ILogger::EResult  DialogLogOutput::_ProcessImpl (const String &caption, const String &msg, ELevel level) __Th___
    {
        if ( _mainThread != std::this_thread::get_id() )
        {
            // TODO
            // check SDL_ShowMessageBox
            /*dispatch_async( dispatch_get_main_queue(),
                            ^(){
                                ShowAlertDialog( caption, msg );
                            }
                          );*/
            return level >= ELevel::Error ? EResult::Break : EResult::Unknown;
        }

        NSModalResponse returnCode = ShowAlertDialog( caption, msg );

        switch ( returnCode )
        {
            case NSAlertFirstButtonReturn :     return EResult::Continue;
            case NSAlertSecondButtonReturn :    return EResult::Break;
            case NSAlertThirdButtonReturn :     return EResult::Abort;
        }
        return EResult::Unknown;
    }


} // AE::Base
#endif

