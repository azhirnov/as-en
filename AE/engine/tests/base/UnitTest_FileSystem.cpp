// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  FileSystem_SearchForward_Test ()
    {
        Path    res;
        TEST( FileSystem::SearchForward( FileSystem::CurrentPath(), Path{"b1.txt"}, 3, OUT res ));

        auto    ref = FileSystem::ToAbsolute( Path{"b/b1.txt"} );
        TEST( res == ref );

        TEST( not FileSystem::SearchForward( Path{"b4.txt"}, 3, OUT res ));
        TEST( not FileSystem::SearchForward( FileSystem::CurrentPath().append("a"), Path{"b1.txt"}, 3, OUT res ));
    }


    static void  FileSystem_SearchBackward_Test ()
    {
        Path    res;
        TEST( FileSystem::SearchBackward( FileSystem::CurrentPath().append("a/aa"), Path{"b/b1.txt"}, 3, OUT res ));

        auto    ref = FileSystem::ToAbsolute( Path{"b/b1.txt"} );
        TEST( res == ref );

        TEST( not FileSystem::SearchBackward( FileSystem::CurrentPath().append("a/aa"), Path{"b1.txt"}, 3, OUT res ));
    }


    static void  FileSystem_Search_Test ()
    {
        Path    res;
        TEST( FileSystem::Search( FileSystem::CurrentPath(),                Path{"b1.txt"}, 3, 3, OUT res ));
        TEST( FileSystem::Search( FileSystem::CurrentPath().append("a"),    Path{"b1.txt"}, 3, 3, OUT res ));
        TEST( FileSystem::Search( FileSystem::CurrentPath().append("a/aa"), Path{"b1.txt"}, 3, 3, OUT res ));
    }
}


extern void UnitTest_FileSystem ()
{
#if not defined(AE_PLATFORM_EMSCRIPTEN) and\
    not defined(AE_PLATFORM_ANDROID)

    if ( not FileSystem::SetCurrentPath( AE_CURRENT_DIR "/fs_test" ))
        TEST( FileSystem::FindAndSetCurrent( "fs_test", 5 ));

    FileSystem_SearchForward_Test();
    FileSystem_SearchBackward_Test();
    FileSystem_Search_Test();

    TEST_PASSED();
#endif
}
