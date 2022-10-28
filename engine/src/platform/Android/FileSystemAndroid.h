// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

#ifdef AE_PLATFORM_ANDROID

# include "base/Utils/FileSystem.h"
# include "base/Utils/NamedID_HashCollisionCheck.h"
# include "base/Memory/LinearAllocator.h"
# include "base/Stream/Stream.h"

# include "vfs/VirtualFileSystem.h"
# include "platform/Public/Common.h"
# include <android/asset_manager_jni.h>

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
		AAsset *	_asset = null;

		DEBUG_ONLY( const String  _name; )


	// methods
	public:
		AndroidRStream (AAsset* asset, const char* name) :
			_asset{asset} DEBUG_ONLY(, _name{name})
		{}

		~AndroidRStream () {  if ( _asset ) AAsset_close( _asset ); }
		
		bool		IsOpen ()			const override	{ return _asset != null; }
		Bytes		Position ()			const override	{ return Bytes{ CheckCast<ulong>( AAsset_getLength( _asset ) - AAsset_getRemainingLength( _asset ))}; }
		Bytes		Size ()				const override	{ return Bytes{ CheckCast<ulong>( AAsset_getLength( _asset ))}; }
		
		EStreamType	GetStreamType ()	const override	{ return EStreamType::SequentialAccess | EStreamType::RandomAccess | EStreamType::FixedSize; }
		
		bool		SeekSet (Bytes pos) override		{ return AAsset_seek( _asset, usize(pos), SEEK_SET ) != 0; }

		Bytes		ReadSeq (OUT void *buffer, Bytes size) override	{ return Bytes{ CheckCast<ulong>( AAsset_read( _asset, OUT buffer, usize(size) ))}; }
	};



	//
	// VFS Storage implementation for Android builtin storage
	//

	class FileSystemAndroid final : public IFileStorage
	{
	// types
	private:
		using FileMap_t		= FlatHashMap< FileName::Optimized_t, const char* >;
		using Allocator_t	= LinearAllocator<>;
		using AsyncRStream	= Threading::AsyncRStream;

		
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


	  // IFileStorage //
		RC<RStream>			Open (const FileName &name) const override;
		RC<AsyncRStream>	OpenAsync (const FileName &name) const override	{ return Default; }

		//Promise<void>		LoadAsync (const FileGroupName &name) const override;

		bool	Exists (const FileName &name) const override;
		bool	Exists (const FileGroupName &name) const override;

	private:
		void				_Append (INOUT GlobalFileMap_t &) const override;
		RC<RStream>			_OpenByIter (const FileName &name, const void* ref) const override;
		RC<AsyncRStream>	_OpenAsyncByIter (const FileName &name, const void* ref) const override;
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
