// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_ANDROID
# include <dlfcn.h>
# include "base/Platforms/AndroidApi26.h"

namespace AE::Base
{

/*
=================================================
    constructor
=================================================
*/
    AndroidApi26::AndroidApi26 () __NE___
    {
        ASSERT( AndroidUtils::GetSDKVersion() >= 26 );

        void*   main_dl = ::dlopen( null, RTLD_NOW );
        auto    Load    = [main_dl, this] (OUT auto& outResult, const char* name)
        {{
            using FN = RemoveReference< decltype(outResult) >;
            outResult = BitCast<FN>( ::dlsym( main_dl, name ));
            _loaded &= (outResult != null);
        }};

        if ( main_dl != null )
        {
            _loaded = true;

            Load( OUT hwbuf.allocate,                   "AHardwareBuffer_allocate"                  );
            Load( OUT hwbuf.acquire,                    "AHardwareBuffer_acquire"                   );
            Load( OUT hwbuf.release,                    "AHardwareBuffer_release"                   );
            Load( OUT hwbuf.describe,                   "AHardwareBuffer_describe"                  );
        //  Load( OUT hwbuf.lock,                       "AHardwareBuffer_lock"                      );
        //  Load( OUT hwbuf.unlock,                     "AHardwareBuffer_unlock"                    );
        //  Load( OUT hwbuf.recvHandleFromUnixSocket,   "AHardwareBuffer_recvHandleFromUnixSocket"  );
        //  Load( OUT hwbuf.sendHandleToUnixSocket,     "AHardwareBuffer_sendHandleToUnixSocket"    );

            Load( OUT imgReader.newWithUsage,           "AImageReader_newWithUsage"                 );
            Load( OUT imgReader.acquireNextImageAsync,  "AImageReader_acquireNextImageAsync"        );
            Load( OUT imgReader.acquireLatestImageAsync,"AImageReader_acquireLatestImageAsync"      );

            Load( OUT img.getHardwareBuffer,            "AImage_getHardwareBuffer"                  );
            Load( OUT img.deleteAsync,                  "AImage_deleteAsync"                        );
        }
    }

/*
=================================================
    Instance
=================================================
*/
    AndroidApi26&  AndroidApi26::Instance () __NE___
    {
        static AndroidApi26     inst;
        return inst;
    }


} // AE::Base

#endif // AE_PLATFORM_ANDROID
