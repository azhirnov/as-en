// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/FileSystemAndroid.h"

#ifdef AE_PLATFORM_ANDROID

namespace AE::App
{
namespace
{
	struct AssetDir
	{
		AAssetDir*	dir	= null;

		AssetDir () __NE___ {}

		~AssetDir () __NE___
		{
			if ( dir != null )
				AAssetDir_close( dir );
		}

		ND_ bool  Open (AAssetManager* mngr, NtStringView path) __NE___
		{
			CHECK_ERR( dir == null );
			dir = AAssetManager_openDir( mngr, path.c_str() );
			return dir != null;
		}

		ND_ const char*  Next () __NE___
		{
			CHECK_ERR( dir != null );
			return AAssetDir_getNextFileName( dir );
		}
	};

/*
=================================================
	Convert
=================================================
*/
	template <typename Alloc>
	ND_ static bool  Convert (OUT const char* &result, StringView filename, Alloc &allocator) __NE___
	{
		CHECK_ERR( filename.length() <= FileName::MaxStringLength() );

		char*	str = allocator.template Allocate<char>( filename.length()+1 );
		CHECK_ERR( str != null );

		std::memcpy( OUT str, filename.data(), filename.length() );
		str[filename.length()] = 0;

		result = str;
		return true;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	AndroidRStream::AndroidRStream (AAsset* asset, const char* name) __NE___ :
		_asset{ asset },
		_size{ asset != null ? CheckCast<ulong>( AAsset_getLength( asset )) : 0 }
		DEBUG_ONLY(, _name{ name })
	{}

/*
=================================================
	PositionAndSize
=================================================
*/
	RStream::PosAndSize  AndroidRStream::PositionAndSize ()	C_NE___
	{
		ASSERT( IsOpen() );

		Bytes	remain {CheckCast<ulong>( AAsset_getRemainingLength( _asset ))};
		return { _size - remain, _size };
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  AndroidRStream::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |
				ESourceType::FixedSize			| ESourceType::ReadAccess;
	}

/*
=================================================
	SeekSet / SeekFwd
=================================================
*/
	bool  AndroidRStream::SeekSet (Bytes newPos) __NE___
	{
		ASSERT( IsOpen() );
		return AAsset_seek( _asset, usize(newPos), SEEK_SET ) != 0;
	}

	bool  AndroidRStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );
		return AAsset_seek( _asset, usize(offset), SEEK_CUR ) != 0;
	}

/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  AndroidRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		return Bytes{ CheckCast<ulong>( AAsset_read( _asset, OUT buffer, usize(size) ))};
	}

/*
=================================================
	AsRDataSource
=================================================
*
	RC<RDataSource>  AndroidRStream::AsRDataSource ()
	{
		ASSERT( IsOpen() );
		return MakeRC<AndroidRDataSource>( _asset, "" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	AndroidRDataSource::AndroidRDataSource (AAsset* asset, const char* name) __NE___ :
		_asset{ asset },
		_size{ asset != null ? CheckCast<ulong>( AAsset_getLength( asset )) : 0 }
		DEBUG_ONLY(, _name{ name })
	{}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  AndroidRDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |
				ESourceType::FixedSize			| ESourceType::ReadAccess;
	}

/*
=================================================
	ReadBlock
=================================================
*/
	Bytes  AndroidRDataSource::ReadBlock (const Bytes pos, OUT void* buffer, const Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		if ( _pos != pos )
		{
			CHECK_ERR( AAsset_seek( _asset, usize(pos), SEEK_SET ) != 0 );
			_pos = pos;
		}
		return Bytes{ CheckCast<ulong>( AAsset_read( _asset, OUT buffer, usize(size) ))};
	}

/*
=================================================
	AsRStream
=================================================
*
	RC<RStream>  AndroidRDataSource::AsRStream ()
	{
		ASSERT( IsOpen() );
		return MakeRC<AndroidRStream>( _asset, "" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	FileSystemAndroid::~FileSystemAndroid () __NE___
	{
		DRC_EXLOCK( _drCheck );
	}

/*
=================================================
	Create
=================================================
*/
	bool  FileSystemAndroid::Create (AAssetManager* mngr, StringView folder) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( mngr != null );
		CHECK_ERR( _assetMngr == null );

		_allocator.Discard();

		_map.clear();
		_map.reserve( 128 );

		_assetMngr = mngr;
		DEBUG_ONLY( _folder = folder );

		// build file map
		{
			AssetDir	dir;
			if ( dir.Open( _assetMngr, folder ))
			{
				for (;;)
				{
					auto	filename = dir.Next();
					if ( filename != null )
					{
						const char*	str;
						CHECK_ERR( Convert( OUT str, filename, _allocator ));
						DEBUG_ONLY( _hashCollisionCheck.Add( FileName{str} ));	// throw

						CHECK_ERR( _map.emplace( FileName::Optimized_t{str}, str ).second );
					}
					else
						break;
				}
			}
		}
		CHECK_ERR( not _map.empty() );
		ASSERT( not _hashCollisionCheck.HasCollisions() );

		return true;
	}

/*
=================================================
	Open
=================================================
*/
	bool  FileSystemAndroid::Open (OUT RC<RStream> &outStream, FileName::Ref name) C_NE___
	{
		return _Open<AndroidRStream>( OUT outStream, name );
	}

	bool  FileSystemAndroid::Open (OUT RC<RDataSource> &outDS, FileName::Ref name) C_NE___
	{
		return _Open<AndroidRDataSource>( OUT outDS, name );
	}

	bool  FileSystemAndroid::Open (OUT RC<AsyncRDataSource> &outDS, FileName::Ref name) C_NE___
	{
		Unused( outDS, name );
		return false;
	}

/*
=================================================
	_Open
=================================================
*/
	template <typename ImplType, typename ResultType>
	bool  FileSystemAndroid::_Open (OUT ResultType &result, FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		if_likely( iter != _map.end() )
		{
			AAsset*	asset = AAssetManager_open( _assetMngr, iter->second, AASSET_MODE_RANDOM );
			CHECK_ERR( asset != null );

			result = MakeRC<ImplType>( asset, iter->second );
			return true;
		}
		return false;
	}

/*
=================================================
	Exists
=================================================
*/
	bool  FileSystemAndroid::Exists (FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		return iter != _map.end();
	}

	bool  FileSystemAndroid::Exists (FileGroupName::Ref) C_NE___
	{
		// not supported
		return false;
	}

/*
=================================================
	_Append
=================================================
*/
	void  FileSystemAndroid::_Append (INOUT GlobalFileMap_t &map) C_Th___
	{
		DRC_SHAREDLOCK( _drCheck );

		for (auto& [name, file] : _map)
		{
			map.emplace( name, GlobalFileRef{ this, file });
		}
	}

/*
=================================================
	_OpenByIter
=================================================
*/
	bool  FileSystemAndroid::_OpenByIter (OUT RC<RStream> &stream, FileName::Ref name, const void* ref) C_NE___
	{
		return _OpenByIter2<AndroidRStream>( OUT stream, name, ref, AASSET_MODE_STREAMING );
	}

	bool  FileSystemAndroid::_OpenByIter (OUT RC<RDataSource> &ds, FileName::Ref name, const void* ref) C_NE___
	{
		return _OpenByIter2<AndroidRDataSource>( OUT ds, name, ref, AASSET_MODE_RANDOM );
	}

	bool  FileSystemAndroid::_OpenByIter (OUT RC<AsyncRDataSource> &ds, FileName::Ref name, const void* ref) C_NE___
	{
		Unused( ds, name, ref );
		return false;
	}

/*
=================================================
	_OpenByIter2
=================================================
*/
	template <typename ImplType, typename ResultType>
	bool  FileSystemAndroid::_OpenByIter2 (OUT ResultType &result, FileName::Ref name, const void* ref, int mode) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		DEBUG_ONLY(
			auto	iter = _map.find( FileName::Optimized_t{name} );
			CHECK_ERR( iter != _map.end() );
			CHECK_ERR( StringView{iter->second} == Cast<char>(ref) );
		)
		Unused( name );

		AAsset*	asset = AAssetManager_open( _assetMngr, Cast<char>(ref), mode );
		CHECK_ERR( asset != null );

		result = MakeRC<ImplType>( asset, Cast<char>(ref) );
		return result != null;
	}


} // AE::App

#endif // AE_PLATFORM_ANDROID
