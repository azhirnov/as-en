// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"

namespace AE::App
{
    using AE::Threading::Promise;
    using AE::VFS::FileName;
    using AE::VFS::FileGroupName;


    //
    // Stream for Android
    //

    class AndroidRStream final : public RStream
    {
    // variables
    private:
        AAsset *        _asset = null;
        const Bytes     _size;

        DEBUG_ONLY( const String  _name; )


    // methods
    public:
        AndroidRStream (AAsset* asset, const char* name)    __NE___;
        ~AndroidRStream ()                                  __NE___ { AAsset_close( _asset ); }

        // RStream //
        bool        IsOpen ()                               C_NE_OV { return _asset != null; }
        PosAndSize  PositionAndSize ()                      C_NE_OV;
        ESourceType GetSourceType ()                        C_NE_OV;

        bool        SeekSet (Bytes newPos)                  __NE_OV;
        bool        SeekFwd (Bytes offset)                  __NE_OV;

        Bytes       ReadSeq (OUT void *buffer, Bytes size)  __NE_OV;
    };



    //
    // Android Asset Data Source
    //

    class AndroidRDataSource final : public RDataSource
    {
    // variables
    private:
        AAsset *        _asset = null;
        Bytes           _pos;
        const Bytes     _size;

        DEBUG_ONLY( const String  _name; )


    // methods
    public:
        AndroidRDataSource (AAsset*, const char*)       __NE___;
        ~AndroidRDataSource ()                          __NE___ { AAsset_close( _asset ); }

        // RDataSource //
        bool        IsOpen ()                           C_NE_OV { return _asset != null; }
        Bytes       Size ()                             C_NE_OV { return _size; }
        ESourceType GetSourceType ()                    C_NE_OV;

        Bytes       ReadBlock (Bytes, OUT void *, Bytes)__NE_OV;
    };



    //
    // VFS Storage implementation for Android builtin storage
    //

    class FileSystemAndroid final : public IVirtualFileStorage
    {
    // types
    private:
        using FileMap_t         = FlatHashMap< FileName::Optimized_t, const char* >;
        using Allocator_t       = LinearAllocator<>;
        using AsyncRDataSource  = Threading::AsyncRDataSource;


    // variables
    private:
        FileMap_t       _map;
        Allocator_t     _allocator;
        AAssetManager*  _assetMngr  = null;     // in specs: pointer may be shared across multiple threads.

        DEBUG_ONLY(
          String                        _folder;
          NamedID_HashCollisionCheck    _hashCollisionCheck;
        )
        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:
        FileSystemAndroid ()                                                                        __NE___ {}
        ~FileSystemAndroid ()                                                                       __NE_OV;

        ND_ bool  Create (AAssetManager* mngr, StringView folder);


      // IVirtualFileStorage //
        bool  Open (OUT RC<RStream> &stream, const FileName &name)                                  C_NE_OV;
        bool  Open (OUT RC<RDataSource> &ds, const FileName &name)                                  C_NE_OV;
        bool  Open (OUT RC<AsyncRDataSource> &ds, const FileName &name)                             C_NE_OV;

        bool  Exists (const FileName &name)                                                         C_NE_OV;
        bool  Exists (const FileGroupName &name)                                                    C_NE_OV;

    private:
        void  _Append (INOUT GlobalFileMap_t &)                                                     C_Th_OV;
        bool  _OpenByIter (OUT RC<RStream> &stream, const FileName &name, const void* ref)          C_NE_OV;
        bool  _OpenByIter (OUT RC<RDataSource> &ds, const FileName &name, const void* ref)          C_NE_OV;
        bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, const FileName &name, const void* ref)     C_NE_OV;

        template <typename ImplType, typename ResultType>
        ND_ bool  _Open (OUT ResultType &, const FileName &name)                                    C_NE___;

        template <typename ImplType, typename ResultType>
        ND_ bool  _OpenByIter2 (OUT ResultType &, const FileName &name, const void* ref, int mode)  C_NE___;
    };


} // AE::App

#endif // AE_PLATFORM_ANDROID
