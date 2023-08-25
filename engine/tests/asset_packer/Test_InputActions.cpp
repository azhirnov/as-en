// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"

#include "platform/Private/SerializableInputActions.h"
#include "platform/Android/SerializableInputActionsAndroid.h"
#include "platform/GLFW/SerializableInputActionsGLFW.h"
#include "platform/WinAPI/SerializableInputActionsWinAPI.h"
#include "platform/OpenVR/SerializableInputActionsOpenVR.h"

namespace
{
    using namespace AE::App;

    const bool                      force_update            = true;
    decltype(&ConvertInputActions)  convert_input_actions   = null;


    static void  InputActions_Test1 ()
    {
        const CharType*     files[]         = { TXT("glfw.as"), TXT("android.as"), TXT("winapi.as") };
        const Path          output_script   = TXT( AE_SHARED_DATA "/scripts/input_actions.as" );
        const Path          ref_dump_fname  = FileSystem::ToAbsolute( "input_actions_ref.txt" );
        const Path          output_folder   = TXT("_output");
        const Path          output          = FileSystem::ToAbsolute( output_folder / "input_actions.bin" );
        const Path          output_cpp      = FileSystem::ToAbsolute( output_folder / "../names.h" );

        FileSystem::Remove( output );

        InputActionsInfo    info = {};
        info.inFiles            = files;
        info.inFileCount        = CountOf( files );
        info.outputPackName     = Cast<CharType>(output.c_str());
        info.outputScriptFile   = Cast<CharType>(output_script.c_str());
        info.outputCppFile      = Cast<CharType>(output_cpp.c_str());

        TEST( convert_input_actions( &info ));


        auto    file = MakeRC<FileRStream>( output );
        TEST( file->IsOpen() );

        auto    mem_stream = MakeRC<MemRStream>();
        {
            TEST( mem_stream->LoadRemaining( *file ));

            uint    name;
            TEST( mem_stream->Read( OUT name ));
            TEST_Eq( name, InputActions_Name );
        }

        Array<uint>     hashes;
        Array<uint>     offsets;

        {
            uint    count;
            TEST( mem_stream->Read( OUT count ));

            TEST( mem_stream->Read( count, hashes ));
            TEST( mem_stream->Read( count, offsets ));

            offsets.push_back( uint(mem_stream->Size()) );
        }

        TEST_Eq( hashes.size(), 3 );

        String  ser_str;

        SerializableInputActions::Reflection    refl;

        for (usize i = 0; i < hashes.size(); ++i)
        {
            TEST_Lt( offsets[i], offsets[i+1] );

            const uint  name = hashes[i];

            AE::Serializing::Deserializer   des{ mem_stream->ToSubStream( Bytes{offsets[i]}, Bytes{offsets[i+1] - offsets[i]} )};

            if ( name == InputActionsAndroid_Name )
            {
                SerializableInputActionsAndroid     temp;
                TEST( temp.Deserialize( des ));

                ser_str << temp.ToString( refl );
            }
            else
            if ( name == InputActionsGLFW_Name )
            {
                SerializableInputActionsGLFW    temp;
                TEST( temp.Deserialize( des ));

                ser_str << temp.ToString( refl );
            }
            else
            if ( name == InputActionsWinAPI_Name )
            {
                SerializableInputActionsWinAPI  temp;
                TEST( temp.Deserialize( des ));

                ser_str << temp.ToString( refl );
            }
            else
            if ( name == InputActionsOpenVR_Name )
            {
                SerializableInputActionsOpenVR  temp;
                TEST( temp.Deserialize( des ));

                ser_str << temp.ToString( refl );
            }
            else
                TEST( false );

            TEST( des.IsEnd() );
        }

        #if not AE_OPTIMIZE_IDS
            TEST( CompareWithDump( ser_str, ref_dump_fname, force_update ));
        #endif
    }
}


extern void Test_InputActions ()
{
    {
        Path    dll_path{ AE_INPUT_ACTIONS_BINDING_LIBRARY };

        #ifdef AE_COMPILER_MSVC
            dll_path.append( CMAKE_INTDIR "/InputActionsBinding-shared.dll" );
        #else
            dll_path.append( "InputActionsBinding-shared.so" );
        #endif

        Library     lib;
        TEST( lib.Load( dll_path ));
        TEST( lib.GetProcAddr( "ConvertInputActions", OUT convert_input_actions ));

        TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/input_actions_test" ));

        InputActions_Test1();
    }
    TEST_PASSED();
}
