// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Wrapper for std::filesystem that disable all exceptions except std::bad_alloc
*/

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_ENABLE_GFS
#  include "filesystem.hpp"
    namespace _ae_fs_ = ghc::filesystem;
#else
#   include <filesystem>
    namespace _ae_fs_ = std::filesystem;
#endif
#include "base/Defines/Undef.h"

#include "base/Math/Bytes.h"
#include "base/Utils/SourceLoc.h"

namespace AE::Base
{
    using Path = _ae_fs_::path;


    //
    // File System
    //

    class FileSystem final
    {
    private:
        FileSystem () = delete;
        ~FileSystem () = delete;

    // types
    public:
        using Time_t = _ae_fs_::file_time_type;

        struct DirectoryEntry;
        struct DirectoryIter;
        struct RecursiveDirectoryIter;


    // filesystem
    public:
        // remove file or empty directory.
        // returns 'true' if the file was deleted.
        static bool  Remove (const Path &p)                             __NE___;

        // remove directory and all subdirectories.
        static bool  RemoveAll (const Path &p)                          __NE___;

        // create directory, parent directory must be exists
        static bool  CreateDirectory (const Path &p)                    __NE___;

        // create all directories that is not exists
        static bool  CreateDirectories (const Path &p)                  __NE___;

        // set working directory
        static bool  SetCurrentPath (const Path &p)                     __NE___;

        // returns 'true' if file or directory is exists
        ND_ static bool  Exists (const Path &p)                         __NE___;

        // returns 'true' if path refers to a file
        ND_ static bool  IsFile (const Path &p)                         __NE___ { return Exists( p ) and not _IsDirectory( p ); }

        // returns 'true' if path refers to a directory
        ND_ static bool  IsDirectory (const Path &p)                    __NE___ { return Exists( p ) and _IsDirectory( p ); }

        // returns time of the last modification of file.
        ND_ static Time_t  LastWriteTime (const Path &p)                __NE___;

        // set time of the last modification of file.
            static bool  SetLastWriteTime (const Path &p, Time_t t)     __NE___;

        // returns current path
        ND_ static Path  CurrentPath ()                                 __Th___;

        // returns absolute path
        ND_ static Path  ToAbsolute (const Path &p)                     __Th___;

        // returns relative path
        ND_ static Path  ToRelative (const Path &p, const Path &base);

        // enumerate all files in directory
        ND_ static auto  Enum (const Path &p)                           __Th___;

        // enumerate all files in directory and its subdirectories
        ND_ static auto  EnumRecursive (const Path &p)                  __Th___;

        // 
        static bool  CopyFile (const Path &from, const Path &to)        __NE___;
        static bool  CopyDirectory (const Path &from, const Path &to)   __NE___;

        // moves or renames filesystem object
        static bool  Rename (const Path &oldName, const Path &newName)  __NE___;

        // writes file system capacity and available space
        static bool  GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available) __NE___;

        // replace unsupported symbols.
        // returns 'true' if name is not modified.
        template <typename T>
        static bool  ValidateFileName (INOUT BasicString<T> &name)      __NE___;

        // calculate hash of path
        ND_ static HashVal  Hash (const Path &p)                        __NE___ { ASSERT( p.is_absolute() );  return HashVal{ _ae_fs_::hash_value( p )}; }


    // utils
    public:
        // searches for a directory for which 'Exists( ref )' returns 'true'
        static bool  FindAndSetCurrent (const Path &ref, uint depth)                                                        __Th___;
        static bool  FindAndSetCurrent (const Path &base, const Path &ref, uint depth)                                      __Th___;

        static bool  SearchBackward (const Path &ref, uint depth, OUT Path &result)                                         __Th___;
        static bool  SearchBackward (const Path &base, const Path &ref, uint depth, OUT Path &result)                       __Th___;

        static bool  SearchForward (const Path &ref, uint depth, OUT Path &result)                                          __Th___;
        static bool  SearchForward (const Path &base, const Path &ref, uint depth, OUT Path &result)                        __Th___;

        static bool  Search (const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result)                      __Th___;
        static bool  Search (const Path &base, const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result)    __Th___;

        // 
        static void  FindUnusedFilename (const Function< void (OUT Path &, usize idx) > &   buildName,
                                         const Function< bool (const Path &) > &            consume)                        __Th___;


    // platform dependent
    public:
        #ifdef AE_PLATFORM_WINDOWS
        ND_ static Path  GetWindowsPath ()                              __Th___;
        #endif

    private:
        ND_ static bool  _IsDirectory (const Path &p)                   __NE___;
    };
//-----------------------------------------------------------------------------



    //
    // Directory iterator
    //
    struct FileSystem::DirectoryIter
    {
        friend class FileSystem;
    private:
        _ae_fs_::directory_iterator     _it;

        DirectoryIter (_ae_fs_::directory_iterator it)      __NE___ : _it{it} {}
    public:
        DirectoryIter ()                                    __NE___ = default;
        DirectoryIter (DirectoryIter &&)                    __NE___ = default;
        DirectoryIter (const DirectoryIter &)               __NE___ = default;
        ~DirectoryIter ()                                   __NE___ = default;

        DirectoryIter&  operator = (const DirectoryIter &)  __NE___ = default;
        DirectoryIter&  operator = (DirectoryIter &&)       __NE___ = default;

        ND_ DirectoryEntry const&   operator *  ()          C_NE___;
        ND_ DirectoryEntry const*   operator -> ()          C_NE___;

            DirectoryIter &         operator ++ ()          __NE___ { std::error_code ec;  _it.increment( OUT ec );  return *this; }    // throw 'std::bad_alloc'

        ND_ bool  operator == (const DirectoryIter &rhs)    C_NE___ { return _it == rhs._it; }
    };



    //
    // Recursive Directory iterator
    //
    struct FileSystem::RecursiveDirectoryIter
    {
        friend class FileSystem;
    private:
        _ae_fs_::recursive_directory_iterator       _it;

        RecursiveDirectoryIter (_ae_fs_::recursive_directory_iterator it)   __NE___ : _it{it} {}
    public:
        RecursiveDirectoryIter ()                                           __NE___ = default;
        RecursiveDirectoryIter (RecursiveDirectoryIter &&)                  __NE___ = default;
        RecursiveDirectoryIter (const RecursiveDirectoryIter &)             __NE___ = default;
        ~RecursiveDirectoryIter ()                                          __NE___ = default;

        RecursiveDirectoryIter&  operator = (const RecursiveDirectoryIter &)__NE___ = default;
        RecursiveDirectoryIter&  operator = (RecursiveDirectoryIter &&)     __NE___ = default;

        ND_ DirectoryEntry const&   operator *  ()                          C_NE___;
        ND_ DirectoryEntry const*   operator -> ()                          C_NE___;

            RecursiveDirectoryIter& operator ++ ()                          __NE___ { std::error_code ec;  _it.increment( OUT ec );  return *this; }    // throw 'std::bad_alloc'

        ND_ bool  operator == (const RecursiveDirectoryIter &rhs)           C_NE___ { return _it == rhs._it; }
    };



    //
    // Directory Entry
    //
    struct FileSystem::DirectoryEntry
    {
        friend struct FileSystem::DirectoryIter;
        friend struct FileSystem::RecursiveDirectoryIter;

    private:
        _ae_fs_::directory_entry    _entry;

        DirectoryEntry (_ae_fs_::directory_entry e)         __NE___ : _entry{e} {}
    public:
        DirectoryEntry ()                                   __NE___ = default;
        DirectoryEntry (const DirectoryEntry &)                     = default;
        DirectoryEntry (DirectoryEntry &&)                  __NE___ = default;

        DirectoryEntry&  operator = (const DirectoryEntry &)        = default;
        DirectoryEntry&  operator = (DirectoryEntry &&)     __NE___ = default;

        operator const Path & ()                            C_NE___ { return _entry.path(); }

        ND_ Path const&     Get ()                          C_NE___ { return _entry.path(); }
        ND_ bool            Exist ()                        C_NE___ { std::error_code ec;  return _entry.exists( OUT ec ); }
        ND_ bool            IsDirectory ()                  C_NE___ { std::error_code ec;  return _entry.is_directory( OUT ec ); }
        ND_ Bytes           FileSyze ()                     C_NE___ { std::error_code ec;  return Bytes{_entry.file_size( OUT ec )}; }
        ND_ Time_t          LastWriteTime ()                C_NE___ { std::error_code ec;  return _entry.last_write_time( OUT ec ); }
        ND_ auto            Status ()                       C_NE___ { std::error_code ec;  return _entry.status( OUT ec ); }

        ND_ bool  operator == (const DirectoryEntry &rhs)   C_NE___ { return _entry == rhs._entry; }
        ND_ bool  operator != (const DirectoryEntry &rhs)   C_NE___ { return _entry != rhs._entry; }
        ND_ bool  operator <  (const DirectoryEntry &rhs)   C_NE___ { return _entry <  rhs._entry; }
        ND_ bool  operator >  (const DirectoryEntry &rhs)   C_NE___ { return _entry >  rhs._entry; }
        ND_ bool  operator <= (const DirectoryEntry &rhs)   C_NE___ { return _entry <= rhs._entry; }
        ND_ bool  operator >= (const DirectoryEntry &rhs)   C_NE___ { return _entry >= rhs._entry; }
    };
//-----------------------------------------------------------------------------



    //
    // Path and Line
    //
    struct PathAndLine
    {
        Path        path;
        uint        line    = 0;

        PathAndLine ()                                  __NE___ {}
        explicit PathAndLine (Path path, uint line = 0) __Th___ : path{RVRef(path)}, line{line} {}
        explicit PathAndLine (const SourceLoc &loc)     __Th___ : path{loc.file}, line{loc.line} {}
        explicit PathAndLine (const SourceLoc2 &loc)    __Th___ : path{loc.file}, line{loc.line} {}

        PathAndLine (const PathAndLine &)               __Th___ = default;
        PathAndLine (PathAndLine &&)                    __NE___ = default;

        PathAndLine&  operator = (const PathAndLine &)  __Th___ = default;
        PathAndLine&  operator = (PathAndLine &&)       __NE___ = default;
    };
//-----------------------------------------------------------------------------


    //
    // Path Hasher
    //
    struct PathHasher
    {
        ND_ usize  operator () (const Path &p) C_NE___
        {
            return usize(FileSystem::Hash( p ));
        }
    };
//-----------------------------------------------------------------------------



    inline FileSystem::DirectoryEntry const&  FileSystem::DirectoryIter::operator *  ()         C_NE___ { return reinterpret_cast<DirectoryEntry const&>( _it.operator* () ); }
    inline FileSystem::DirectoryEntry const*  FileSystem::DirectoryIter::operator -> ()         C_NE___ { return reinterpret_cast<DirectoryEntry const*>( _it.operator->() ); }

    inline FileSystem::DirectoryIter  begin (FileSystem::DirectoryIter it)                      __NE___ { return it; }
    inline FileSystem::DirectoryIter  end   (FileSystem::DirectoryIter it)                      __NE___ { return {}; }


    inline FileSystem::DirectoryEntry const&  FileSystem::RecursiveDirectoryIter::operator * () C_NE___ { return reinterpret_cast<DirectoryEntry const&>( _it.operator* () ); }
    inline FileSystem::DirectoryEntry const*  FileSystem::RecursiveDirectoryIter::operator ->() C_NE___ { return reinterpret_cast<DirectoryEntry const*>( _it.operator->() ); }

    inline FileSystem::RecursiveDirectoryIter  begin (FileSystem::RecursiveDirectoryIter it)    __NE___ { return it; }
    inline FileSystem::RecursiveDirectoryIter  end   (FileSystem::RecursiveDirectoryIter it)    __NE___ { return {}; }


    inline bool  FileSystem::Remove (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::remove( p, OUT ec );
    }

    inline bool  FileSystem::RemoveAll (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::remove_all( p, OUT ec ) != UMax;
    }

    inline bool  FileSystem::CreateDirectory (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::create_directory( p, OUT ec );
    }

    inline bool  FileSystem::CreateDirectories (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::create_directories( p, OUT ec );
    }

    inline bool  FileSystem::SetCurrentPath (const Path &p) __NE___
    {
        std::error_code ec;
        _ae_fs_::current_path( p, OUT ec );
        return ec == Default;
    }

    inline bool  FileSystem::Exists (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::exists( p, OUT ec );
    }

    inline FileSystem::Time_t  FileSystem::LastWriteTime (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::last_write_time( p, OUT ec );
    }

    inline bool  FileSystem::SetLastWriteTime (const Path &p, Time_t t) __NE___
    {
        std::error_code ec;
        _ae_fs_::last_write_time( p, t, OUT ec );
        return not ec;
    }

    inline bool  FileSystem::_IsDirectory (const Path &p) __NE___
    {
        std::error_code ec;
        return _ae_fs_::is_directory( p, OUT ec );
    }

    inline Path  FileSystem::CurrentPath () __Th___
    {
        std::error_code ec;
        return _ae_fs_::current_path( OUT ec );
    }

    inline Path  FileSystem::ToAbsolute (const Path &p) __Th___
    {
        std::error_code ec;
        return _ae_fs_::absolute( p, OUT ec );
    }

    inline Path  FileSystem::ToRelative (const Path &p, const Path &base) __Th___
    {
        std::error_code ec;
        return _ae_fs_::relative( p, base, OUT ec );
    }

    inline auto  FileSystem::Enum (const Path &p) __Th___
    {
        std::error_code ec;
        return DirectoryIter{ _ae_fs_::directory_iterator{ p, OUT ec }};
    }

    inline auto  FileSystem::EnumRecursive (const Path &p) __Th___
    {
        std::error_code ec;
        return RecursiveDirectoryIter{ _ae_fs_::recursive_directory_iterator{ p, OUT ec }};
    }

    inline bool  FileSystem::CopyFile (const Path &from, const Path &to) __NE___
    {
        std::error_code ec;
        return _ae_fs_::copy_file( from, to, _ae_fs_::copy_options::overwrite_existing, OUT ec );
    }

    inline bool  FileSystem::CopyDirectory (const Path &from, const Path &to) __NE___
    {
        std::error_code ec;
        _ae_fs_::copy( from, to, _ae_fs_::copy_options::recursive, OUT ec );
        return not ec;
    }

    inline bool  FileSystem::Rename (const Path &oldName, const Path &newName) __NE___
    {
        std::error_code ec;
        _ae_fs_::rename( oldName, newName, OUT ec );
        return not ec;
    }

    inline bool  FileSystem::GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available) __NE___
    {
        std::error_code ec;
        auto    space = _ae_fs_::space( path, OUT ec );
        if ( not ec )
        {
            total       = Bytes{ space.capacity };
            available   = Bytes{ space.available };
            return true;
        }
        return false;
    }

    template <typename T>
    inline bool  FileSystem::ValidateFileName (INOUT BasicString<T> &name) __NE___
    {
        bool    res = true;
        for (usize i = 0; i < name.size(); ++i)
        {
            T&  c = name[i];

        #if defined(AE_PLATFORM_WINDOWS)
            if ( (c == T('/')) | (c == T('\\')) | (c == T('?')) | (c == T('%')) | (c == T('*')) |
                 (c == T('|')) | (c == T(':'))  | (c == T('"')) | (c == T('<')) | (c == T('>')) )
            {
                res = false;
                c = T('_');
            }
        #elif defined(AE_PLATFORM_UNIX_BASED)
            if ( (c == T('/')) )
            {
                res = false;
                c = T('_');
            }
        #else
            #error Not implemented
        #endif
        }
        return res;
    }


    ND_ inline bool  IsAnsiPath (const Path &path) __NE___
    {
        for (auto& c : path.native())
        {
            if ( ulong(c) > 127 )
                return false;
        }
        return true;
    }

} // AE::Base
