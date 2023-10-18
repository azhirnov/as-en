// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ObjectStorage.h"

#include "platform/GLFW/SerializableInputActionsGLFW.h"
#include "platform/WinAPI/SerializableInputActionsWinAPI.h"
#include "platform/Android/SerializableInputActionsAndroid.h"
#include "platform/OpenVR/SerializableInputActionsOpenVR.h"
//#include "platform/OpenXR/SerializableInputActionsOpenXR.h"

namespace AE::InputActions
{
namespace
{
    using namespace AE::Scripting;
    using namespace AE::App;

    static constexpr uint   MaxIA = 16;

    using IABits    = BitSet<MaxIA>;

/*
=================================================
    TryLoadFromScript
=================================================
*/
    template <typename T>
    ND_ bool  TryLoadFromScript (INOUT Unique<SerializableInputActions> &actions,
                                 const ScriptEnginePtr &se, const String &script, const SourceLoc &loc,
                                 INOUT App::SerializableInputActions::Reflection &refl)
    {
        String  substr;
        ScriptTypeInfo< typename T::ScriptActionBindings >::Name( OUT substr );

        if ( script.find( substr ) != String::npos )
        {
            if ( not actions )  actions.reset( new T{} );

            T   temp;
            CHECK_ERR( temp.LoadFromScript( se, script, loc, INOUT refl ));

            CHECK_ERR( actions->Merge( temp ));
            return true;
        }
        return false;
    }

} // namespace


/*
=================================================
    constructor
=================================================
*/
    ObjectStorage::ObjectStorage ()
    {}

/*
=================================================
    destructor
=================================================
*/
    ObjectStorage::~ObjectStorage ()
    {}

/*
=================================================
    AddBindings
=================================================
*/
    bool  ObjectStorage::AddBindings (const ScriptEnginePtr &se, const Path &path)
    {
        const auto  ansi_path = ToString( path );
        String      script;
        {
            FileRStream     file {path};
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( file.Read( file.RemainingSize(), OUT script ));
        }

        if ( TryLoadFromScript< SerializableInputActionsGLFW >( INOUT _glfw, se, script, SourceLoc{ansi_path}, INOUT _refl ))
            return true;

        if ( TryLoadFromScript< SerializableInputActionsWinAPI >( INOUT _winapi, se, script, SourceLoc{ansi_path}, INOUT _refl ))
            return true;

        if ( TryLoadFromScript< SerializableInputActionsAndroid >( INOUT _android, se, script, SourceLoc{ansi_path}, INOUT _refl ))
            return true;

        if ( TryLoadFromScript< SerializableInputActionsOpenVR >( INOUT _openvr, se, script, SourceLoc{ansi_path}, INOUT _refl ))
            return true;

        //if ( TryLoadFromScript< SerializableInputActionsOpenXR >( INOUT _openxr, se, script, SourceLoc{ansi_path}, INOUT _refl ))
        //  return true;

        RETURN_ERR( "can't find suitable input actions type" );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ObjectStorage::Bind (const ScriptEnginePtr &se) __Th___
    {
        CHECK_THROW_MSG( se->Create( True{"gen cpp header"} ));

        SerializableInputActions::BindBase( se );

        SerializableInputActionsGLFW::Bind( se );
        SerializableInputActionsWinAPI::Bind( se );
        SerializableInputActionsAndroid::Bind( se );
        SerializableInputActionsOpenVR::Bind( se );
        //SerializableInputActionsOpenXR::Bind( se );
    }

/*
=================================================
    Save
=================================================
*/
    bool  ObjectStorage::Save (const Path &filename) const
    {
        using namespace AE::Serializing;

        FileSystem::CreateDirectories( filename.parent_path() );

        FixedArray< uint, MaxIA >                           hash_arr;
        FixedArray< uint, MaxIA >                           offset_arr;
        FixedArray< App::SerializableInputActions*, MaxIA > ia_arr;

        if ( _android ) { hash_arr.push_back( InputActionsAndroid_Name );   offset_arr.push_back( 0 );  ia_arr.push_back( _android.get() ); }
        if ( _glfw )    { hash_arr.push_back( InputActionsGLFW_Name );      offset_arr.push_back( 0 );  ia_arr.push_back( _glfw.get() );    }
        if ( _winapi )  { hash_arr.push_back( InputActionsWinAPI_Name );    offset_arr.push_back( 0 );  ia_arr.push_back( _winapi.get() );  }
        if ( _openvr )  { hash_arr.push_back( InputActionsOpenVR_Name );    offset_arr.push_back( 0 );  ia_arr.push_back( _openvr.get() );  }
    //  if ( _openxr )  { hash_arr.push_back( InputActionsOpenXR.Name );    offset_arr.push_back( 0 );  ia_arr.push_back( _openxr.get() );  }

        RC<WStream> file = MakeRC<FileWStream>( filename );

        CHECK_ERR( file->IsOpen() );
        CHECK_ERR( file->Write( InputActions_Name ));
        CHECK_ERR( file->Write( uint(hash_arr.size()) ));
        CHECK_ERR( file->Write( ArrayView<uint>{hash_arr} ));
        CHECK_ERR( file->Write( ArrayView<uint>{offset_arr} ));

        offset_arr.clear();

        for (auto& ia : ia_arr)
        {
            offset_arr.push_back( uint(file->Position()) );

            auto    mem = MakeRC<MemWStream>();
            {
                Serializer  ser{ mem };
                CHECK_ERR( ia->Serialize( ser ));
            }
            CHECK_ERR( mem->Store( *file ));
        }

        CHECK_ERR( hash_arr.size() == offset_arr.size() );
        CHECK_ERR( ia_arr.size() == offset_arr.size() );

        file = null;

        // update offsets
        {
            file = MakeRC<FileWStream>( filename, FileWStream::EMode::Update );
            CHECK_ERR( file->IsOpen() );

            CHECK_ERR( file->Write( InputActions_Name ));
            CHECK_ERR( file->Write( uint(hash_arr.size()) ));
            CHECK_ERR( file->Write( ArrayView<uint>{hash_arr} ));
            CHECK_ERR( file->Write( ArrayView<uint>{offset_arr} ));
        }

        AE_LOGI( "Store input actions pack: '"s << ToString(filename) << "'" );
        return true;
    }

/*
=================================================
    SaveCppTypes
=================================================
*/
    bool  ObjectStorage::SaveCppTypes (const Path &filename) const
    {
        const auto  ValidateName = [] (StringView name) -> String
        {{
            return Parser::ValidateVarName_CPP( name );
        }};

        FileSystem::CreateDirectories( filename.parent_path() );

        FixedArray< App::SerializableInputActions*, MaxIA >     ia_arr;

        IABits  desktop_bits;
        IABits  mobile_bits;
        IABits  vr_bits;

        if ( _android ) { mobile_bits.set( ia_arr.size() );     ia_arr.push_back( _android.get() ); }
        if ( _glfw )    { desktop_bits.set( ia_arr.size() );    ia_arr.push_back( _glfw.get() );    }
        if ( _winapi )  { desktop_bits.set( ia_arr.size() );    ia_arr.push_back( _winapi.get() );  }
        if ( _openvr )  { vr_bits.set( ia_arr.size() );         ia_arr.push_back( _openvr.get() );  }
    //  if ( _openxr )  { vr_bits.set( ia_arr.size() );         ia_arr.push_back( _openxr.get() );  }

        const IABits    all_bits = desktop_bits | mobile_bits | vr_bits;

        String  str;
        str << "namespace InputActions\n{\n"
            << "\tusing Hash_t          = AE::Base::HashVal32;\n"
            << "\tusing InputModeName_t = AE::App::InputModeName;\n\n";

        struct ShModeInfo
        {
            IABits                          bits;
            FlatHashMap< String, IABits >   actions;

            ShModeInfo () {}
        };

        FlatHashMap< String, ShModeInfo >   shared_modes;
        const uint                          shared_count = uint(ia_arr.size());

        for (usize i = 0; i < ia_arr.size(); ++i)
        {
            for (auto [mode_name, mode] : ia_arr[i]->GetModes())
            {
                auto&   dst = shared_modes[ _refl.Get( mode_name )];
                dst.bits.set( i );

                for (auto& [key, act] : mode.actions)
                {
                    const auto  act_name = _refl.Get( act.name );
                    dst.actions[act_name].set( i );
                }
            }
        }

        const auto  AddActions = [&all_bits, &ValidateName] (const ShModeInfo &mode, const IABits reqBits, StringView suffix)
        {{
            String          str;
            Array<String>   act_arr;

            for (auto& [act_name, cnt] : mode.actions)
            {
                if ( all_bits == reqBits )
                {
                    if ( cnt == all_bits )
                        act_arr.push_back( act_name );
                }else{
                    if ( cnt != all_bits and (cnt & reqBits) == reqBits )
                        act_arr.push_back( act_name );
                }
            }
            std::sort( act_arr.begin(), act_arr.end() );

            if ( act_arr.empty() )
                return str;

            str << suffix << "static constexpr uint  actionCount = " << ToString(act_arr.size()) << ";\n";

            for (const auto& act_name : act_arr) {
                str << suffix << "static constexpr uint  " << ValidateName(act_name) << "  = 0x"
                    << ToString<16>( uint{InputActionName{act_name}} ) << "u;  // InputActionName{\"" << act_name << "\"}\n";
            }
            return str;
        }};

        // shared modes
        {
            Array<String>   mode_arr;
            for (const auto& [name, info] : shared_modes)
            {
                if ( info.bits.count() >= shared_count )
                    mode_arr.push_back( name );
            }
            std::sort( mode_arr.begin(), mode_arr.end() );

            for (auto& mode_name : mode_arr)
            {
                str << "\tstatic constexpr struct _" << ValidateName(mode_name) << "\n\t{\n";
                str << "\t\tconstexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x"
                    << ToString<16>( uint{InputModeName{mode_name}} ) << "u}}; }  // '" << mode_name << "'\n\n";

                const auto& mode = shared_modes[mode_name];

                // shared
                str << AddActions( mode, all_bits, "\t\t" );

                // desktop
                if ( desktop_bits != all_bits )
                {
                    String  tmp = AddActions( mode, desktop_bits, "\t\t\t" );
                    if ( not tmp.empty() ) {
                        str << "\t\tstatic constexpr struct _Desktop {\n"
                            << tmp
                            << "\t\t} Desktop;\n";
                    }
                }

                // mobile
                if ( mobile_bits != all_bits )
                {
                    String  tmp = AddActions( mode, mobile_bits, "\t\t\t" );
                    if ( not tmp.empty() ) {
                        str << "\t\tstatic constexpr struct _Mobile {\n"
                            << tmp
                            << "\t\t} Mobile;\n";
                    }
                }

                // VR
                if ( vr_bits != all_bits )
                {
                    String  tmp = AddActions( mode, vr_bits, "\t\t\t" );
                    if ( not tmp.empty() ) {
                        str << "\t\tstatic constexpr struct _VR {\n"
                            << tmp
                            << "\t\t} VR;\n";
                    }
                }


                str << "\t} " << ValidateName(mode_name) << ";\n\n";
            }
        }


        // platform specific
    /*  for (auto& ia : ia_arr)
        {
            str << "\n\tstruct " << ia->GetApiName() << "\n\t{\n";

            Array<String>   mode_arr;
            for (auto [mode_name, mode] : ia->GetModes())
            {
                const auto  name = _refl.Get( mode_name );
                if ( shared_modes[name].bits.count() < shared_count )
                    mode_arr.push_back( name );
            }
            std::sort( mode_arr.begin(), mode_arr.end() );

            for (auto& mode_name : mode_arr)
            {
                String  tmp;
                tmp << "\t\tstatic constexpr struct _" << ValidateName(mode_name) << "\n\t\t{\n";
                tmp << "\t\t\tconstexpr operator InputModeName_t () const { return InputModeName_t{\"" << mode_name << "\"}; }\n\n";

                const auto  it = ia->GetModes().find( InputModeName{mode_name} );
                CHECK_ERR( it != ia->GetModes().end() );

                const auto&     mode    = it->second;
                const auto&     sh_mode = shared_modes[ mode_name ];

                Array<String>   act_arr;
                for (auto& [key, act_info] : mode.actions)
                {
                    const auto  act_name    = _refl.Get( act_info.name );
                    const auto  act_it      = sh_mode.actions.find( act_name );
                    const auto  act_bits    = act_it != sh_mode.actions.end() ? act_it->second : Default;

                    if ( act_bits.count() < shared_count )
                        act_arr.push_back( act_name );
                }
                std::sort( act_arr.begin(), act_arr.end() );
                RemoveDuplicates( act_arr );

                if ( act_arr.empty() )
                    continue;

                tmp << "\t\t\tstatic constexpr uint  actionCount = " << ToString(act_arr.size()) << ";\n\n";

                for (const auto& act_name : act_arr) {
                    tmp << "\t\t\tstatic constexpr InputActionName_t  " << ValidateName(act_name) << " {\"" << act_name << "\"};\n";
                }

                tmp << "\t\t} " << ValidateName(mode_name) << ";\n\n";
                str << tmp;
            }
            str.pop_back();
            str << "\t};\n\n";
        }*/

        str << "}\n";

      #if not AE_PRIVATE_USE_TABS
        str = Parser::TabsToSpaces( str );
      #endif

        auto    file = MakeRC<FileWStream>( filename );
        CHECK_ERR( file->IsOpen() );
        CHECK_ERR( file->Write( StringView{str} ));

        AE_LOGI( "Store input actions C++ types: '"s << ToString(filename) << "'" );
        return true;
    }

/*
=================================================
    Instance
=================================================
*/
    ObjectStorage*&  ShaderStorage_Instance ()
    {
        static thread_local ObjectStorage*  inst = null;
        return inst;
    }

    Ptr<ObjectStorage>  ObjectStorage::Instance ()
    {
        return ShaderStorage_Instance();
    }

    void  ObjectStorage::SetInstance (ObjectStorage* inst)
    {
        ShaderStorage_Instance() = inst;
    }


} // AE::InputActions
