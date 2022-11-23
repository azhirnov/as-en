// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <android/asset_manager_jni.h>

# include "base/Utils/FileSystem.h"
# include "base/Utils/NamedID_HashCollisionCheck.h"
# include "base/Memory/LinearAllocator.h"
# include "base/DataSource/Stream.h"

# include "vfs/VirtualFileSystem.h"
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
		AAsset *		_asset = null;
		const Bytes		_size;

		DEBUG_ONLY( const String  _name; )


	// methods
	public:
		AndroidRStream (AAsset* asset, const char* name)	__NE___;
		~AndroidRStream ()									__NE___	{ AAsset_close( _asset ); }
		
		// RStream //
		bool		IsOpen ()								C_NE_OV	{ return true; }
		PosAndSize	PositionAndSize ()						C_NE_OV;
		ESourceType	GetSourceType ()						C_NE_OV;
		
		bool		SeekSet (Bytes newPos)					__NE_OV;
		bool		SeekFwd (Bytes offset)					__NE_OV;

		Bytes		ReadSeq (OUT void *buffer, Bytes size)	__NE_OV;

		RC<RDataSource>	AsRDataSource ()					__Th_OV;
	};



	//
	// Android Asset Data Source
	//

	class AndroidRDataSource final : public RDataSource
	{
	// variables
	private:
		AAsset *		_asset = null;
		Bytes			_pos;
		const Bytes		_size;

		DEBUG_ONLY( const String  _name; )


	// methods
	public:
		AndroidRDataSource (AAsset*, const char*)		__NE___;
		~AndroidRDataSource ()							__NE___	{ AAsset_close( _asset ); }
		
		// RDataSource //
		bool		IsOpen ()							C_NE_OV	{ return true; }
		Bytes		Size ()								C_NE_OV	{ return _size; }
		ESourceType	GetSourceType ()					C_NE_OV;

		Bytes		ReadBlock (Bytes, OUT void *, Bytes)__NE_OV;

		RC<RStream>	AsRStream ()						__Th_OV;
	};



	//
	// VFS Storage implementation for Android builtin storage
	//

	class FileSystemAndroid final : public IVirtualFileStorage
	{
	// types
	private:
		using FileMap_t		= FlatHashMap< FileName::Optimized_t, const char* >;
		using Allocator_t	= LinearAllocator<>;
		//using AsyncRStream	= Threading::AsyncRStream;

		
	// variables
	private:
		FileMap_t		_map;
		Allocator_t		_allocator;
		AAssetManager*	_assetMngr	= null;		// in specs: pointer may be shared across multiple threads.

		DEBUG_ONLY(
		  String						_folder;
		  NamedID_HashCollisionCheck	_hasCollisionCheck;
		)
		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		FileSystemAndroid () {}
		~FileSystemAndroid () override {}

		ND_ bool  Create (AAssetManager* mngr, StringView folder);


	  // IVirtualFileStorage //
		RC<RStream>		OpenAsStream (const FileName &name) const override;
		RC<RDataSource>	OpenAsSource (const FileName &name) const override;

		bool	Exists (const FileName &name) const override;
		bool	Exists (const FileGroupName &name) const override;

	private:
		void			_Append (INOUT GlobalFileMap_t &) const override;
		RC<RStream>		_OpenAsStreamByIter (const FileName &name, const void* ref) const override;
		RC<RDataSource>	_OpenAsSourceByIter (const FileName &name, const void* ref) const override;
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
