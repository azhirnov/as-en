// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Array ();
extern void UnitTest_Class ();
extern void UnitTest_MathFunc ();
extern void UnitTest_String ();
extern void UnitTest_Exceptions ();
extern void UnitTest_Preprocessor ();
extern void UnitTest_Fn ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_Scripting ()
#else
int main ()
#endif
{
    AE::Base::StaticLogger::LoggerDbgScope log{};

    UnitTest_Array();
    UnitTest_Class();
    UnitTest_MathFunc();
    UnitTest_String();
    UnitTest_Exceptions();
    UnitTest_Preprocessor();
    UnitTest_Fn();

    AE_LOGI( "Tests.ScriptBinding finished" );
    return 0;
}
