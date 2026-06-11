// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Archive/ArchiveStaticStorage.h"
#include "threading/DataSource/FileAsyncDataSource.h"

namespace AE::VFS
{
	using ArchiveDataSource_t	= RDataSourceRange< RC<RDataSource> >;
	using ArchiveStream_t		= RDataSourceAsStream< RC<RDataSource> >;

/*
=================================================
	_Create
=================================================
*/
	bool  ArchiveStaticStorage::_Create (RC<AsyncRDataSource> asyncFile, RC<RDataSource> syncFile) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( asyncFile and not asyncFile->IsOpen() ){
			asyncFile = null;
			AE_LOG_DBG( "AsyncFile is not open" );
		}

		if ( syncFile  and not syncFile->IsOpen() ){
			syncFile = null;
			AE_LOG_DBG( "File is not open" );
		}

		CHECK_ERR( asyncFile or syncFile );
		CHECK_ERR( (not _asyncFile) and (not _syncFile) );

		if ( syncFile and not syncFile->IsThreadSafe() )
		{
			using TSRDataSource_t = Threading::TsRDataSource< RC<RDataSource> >;

			syncFile = MakeRC<TSRDataSource_t>( RVRef(syncFile) );
			AE_LOG_DBG( "Performance warning: used thread-safe wrapper" );
		}

		if ( not syncFile )
		{
			syncFile = MakeRC< Threading::SyncRDataSource >( asyncFile );
			AE_LOG_DBG( "Performance warning: used 'SyncRDataSource' on top of 'AsyncRDataSource'" );
		}

		CHECK_ERR( syncFile and syncFile->IsOpen() );

		CHECK_ERR( _ReadHeader( *syncFile ));

		_asyncFile = RVRef(asyncFile);
		_syncFile  = RVRef(syncFile);
		return true;
	}

/*
=================================================
	_Create
=================================================
*/
	bool  ArchiveStaticStorage::_Create (const Path &filename) __NE___
	{
		using EMode = Threading::FileAsyncRDataSource::EMode;
		const auto	mode = EMode::RandomAccess;		// TODO: Direct

		auto	async_file	= MakeRC< Threading::FileAsyncRDataSource >( filename, mode );
		auto	sync_file	= MakeRC< Base::FileRDataSource >( filename, mode );

		CHECK_ERR( async_file );
		CHECK_ERR( sync_file );

		return _Create( RVRef(async_file), RVRef(sync_file) );
	}

/*
=================================================
	_ReadHeader
=================================================
*/
	bool  ArchiveStaticStorage::_ReadHeader (RDataSource &inDS) __NE___
	{
		TRY{
			ArchiveHeader	hdr;
			CHECK_ERR( inDS.ReadBlock( 0_b, OUT &hdr, Sizeof(hdr) ) == Sizeof(hdr) );

			CHECK_ERR(	hdr.name == Name			and
						hdr.version == Version		and
						hdr.fileHeadersSize > 0		and
						IsMultipleOf( hdr.fileHeadersSize, sizeof(FileHeader) ));

			const uint	file_count = hdr.fileHeadersSize / sizeof(FileHeader);
			_map.reserve( file_count );  // throw

			auto	mem = MakeRC<ArrayRStream>();
			CHECK_ERR( mem->LoadFrom( inDS, Sizeof(hdr), Bytes{hdr.fileHeadersSize} ));

			DEBUG_ONLY( const Bytes  ds_size = inDS.Size();)

			FastRStream	stream {mem};
			for (uint i = 0; i < file_count; ++i)
			{
				FileHeader	fhdr;
				CHECK_ERR( stream.Read( OUT fhdr ));
				CHECK_ERR( _map.emplace( fhdr.name, fhdr.info ).second );  // throw

				ASSERT( fhdr.info.Offset() < ds_size );
				ASSERT( (fhdr.info.Offset() + fhdr.info.size) <= ds_size );
			}
			return true;
		}
		CATCH_ALL(
			return false;
		)
	}

/*
=================================================
	Open (RStream)
=================================================
*/
	bool  ArchiveStaticStorage::Open (OUT RC<RStream> &outStream, FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		if_likely( iter != _map.end() )
			return _Open2( OUT outStream, iter->second );

		return false;
	}

	bool  ArchiveStaticStorage::_Open2 (OUT RC<RStream> &outStream, const FileInfo &info) C_NE___
	{
		auto	substream = MakeRC<ArchiveStream_t>( _syncFile, info.Offset(), info.Size() );

		switch_enum( info.type )
		{
			case EFileType::Raw :
			{
				outStream = RVRef(substream);
				return true;
			}

			case EFileType::InMemory :
			{
				auto	result = MakeRC<ArrayRStream>();
				CHECK_ERR( result->LoadAllFrom( *substream ));

				outStream = RVRef(result);
				return true;
			}

			// Brotli //
		  #ifdef AE_ENABLE_BROTLI
			case EFileType::Brotli :
			{
				outStream = MakeRC<BrotliRStream>( substream );
				return true;
			}

			case EFileType::BrotliInMemory :
			{
				BrotliRStream	brotli	{ substream };
				auto			result	= MakeRC<ArrayRStream>();

				CHECK_ERR( result->DecompressFrom( brotli ));
				outStream = RVRef(result);
				return true;
			}
		  #else

			case EFileType::Brotli :
			case EFileType::BrotliInMemory :
				break;
		  #endif

			// ZStd //
		  #ifdef AE_ENABLE_ZSTD
			case EFileType::ZStd :
			{
				outStream = MakeRC<ZStdRStream>( substream );
				return true;
			}

			case EFileType::ZStdInMemory :
			{
				ZStdRStream		zstd	{ substream };
				auto			result	= MakeRC<ArrayRStream>();

				CHECK_ERR( result->DecompressFrom( zstd ));
				outStream = RVRef(result);
				return true;
			}
		  #else

			case EFileType::ZStd :
			case EFileType::ZStdInMemory :
				break;
		  #endif

			case EFileType::Unknown :
			case EFileType::_Last :
			case EFileType::All :
				break;
		}
		switch_end

		return false;
	}

/*
=================================================
	Open (RDataSource)
=================================================
*/
	bool  ArchiveStaticStorage::Open (OUT RC<RDataSource> &outDS, FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		if_likely( iter != _map.end() )
			return _Open2( OUT outDS, iter->second );

		return false;
	}

	bool  ArchiveStaticStorage::_Open2 (OUT RC<RDataSource> &outDS, const FileInfo &info) C_NE___
	{
		auto	ds = MakeRC<ArchiveDataSource_t>( _syncFile, info.Offset(), info.Size() );

		switch_enum( info.type )
		{
			case EFileType::Raw :
			{
				outDS = RVRef(ds);
				return true;
			}

			case EFileType::InMemory :
			{
				auto	result = MakeRC<ArrayRDataSource>();
				CHECK_ERR( result->LoadAllFrom( *ds ));

				outDS = RVRef(result);
				return true;
			}

			// Brotli //
		  #ifdef AE_ENABLE_BROTLI
			case EFileType::Brotli :
				break;	// not supported

			case EFileType::BrotliInMemory :
			{
				auto			stream	= MakeRC<ArchiveStream_t>( _syncFile, info.Offset(), info.Size() );
				BrotliRStream	brotli	{ stream };
				auto			result	= MakeRC<ArrayRDataSource>();

				CHECK_ERR( result->DecompressFrom( brotli ));
				outDS = RVRef(result);
				return true;
			}
		  #else

			case EFileType::Brotli :
			case EFileType::BrotliInMemory :
				break;
		  #endif

			// ZStd //
		  #ifdef AE_ENABLE_ZSTD
			case EFileType::ZStd :
				break;	// not supported

			case EFileType::ZStdInMemory :
			{
				auto			stream	= MakeRC<ArchiveStream_t>( _syncFile, info.Offset(), info.Size() );
				ZStdRStream		zstd	{ stream };
				auto			result	= MakeRC<ArrayRDataSource>();

				CHECK_ERR( result->DecompressFrom( zstd ));
				outDS = RVRef(result);
				return true;
			}
		  #else

			case EFileType::ZStd :
			case EFileType::ZStdInMemory :
				break;
		  #endif

			case EFileType::Unknown :
			case EFileType::_Last :
			case EFileType::All :
				break;
		}
		switch_end

		return false;
	}

/*
=================================================
	Open (AsyncRDataSource)
=================================================
*/
	bool  ArchiveStaticStorage::Open (OUT RC<AsyncRDataSource> &outDS, FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( _asyncFile );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		if_likely( iter != _map.end() )
			return _Open2( OUT outDS, iter->second );

		return false;
	}

	bool  ArchiveStaticStorage::_Open2 (OUT RC<AsyncRDataSource> &outDS, const FileInfo &info) C_NE___
	{
		auto	ds = MakeRC< Threading::AsyncRDataSourceSubRange >( _asyncFile, info.Offset(), info.End() );

		switch ( info.type )
		{
			case EFileType::Raw :
			{
				outDS = RVRef(ds);
				return true;
			}
		}
		return false;
	}

/*
=================================================
	Open (AsyncRStream)
=================================================
*/
	bool  ArchiveStaticStorage::Open (OUT RC<AsyncRStream> &outStream, FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( _asyncFile );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		if_likely( iter != _map.end() )
			return _Open2( OUT outStream, iter->second );

		return false;
	}

	bool  ArchiveStaticStorage::_Open2 (OUT RC<AsyncRStream> &outStream, const FileInfo &info) C_NE___
	{
		auto	stream = MakeRC< Threading::AsyncRStreamSubRange >( _asyncFile, info.Offset(), info.End() );

		switch ( info.type )
		{
			case EFileType::Raw :
			{
				outStream = RVRef(stream);
				return true;
			}
		}
		return false;
	}

/*
=================================================
	Exists
=================================================
*/
	bool  ArchiveStaticStorage::Exists (FileName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	iter = _map.find( FileName::Optimized_t{name} );
		return iter != _map.end();
	}

/*
=================================================
	Exists
=================================================
*/
	bool  ArchiveStaticStorage::Exists (FileGroupName::Ref) C_NE___
	{
		// not supported
		return false;
	}

/*
=================================================
	_Append
=================================================
*/
	void  ArchiveStaticStorage::_Append (INOUT GlobalFileMap_t &map) C_Th___
	{
		DRC_SHAREDLOCK( _drCheck );

		for (auto& [name, info] : _map)
		{
			map.emplace( name, GlobalFileRef{ this, &info });	// throw
		}
	}

/*
=================================================
	_OpenByIter (RStream)
=================================================
*/
	bool  ArchiveStaticStorage::_OpenByIter (OUT RC<RStream> &outStream, FileName::Ref name, const void* ref) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		DEBUG_ONLY(
			auto	iter = _map.find( FileName::Optimized_t{name} );
			CHECK_ERR( iter != _map.end() );
			CHECK_ERR( &iter->second == ref );
		)
		Unused( name );

		return _Open2( OUT outStream, *Cast<FileInfo>( ref ));
	}

/*
=================================================
	_OpenByIter (RDataSource)
=================================================
*/
	bool  ArchiveStaticStorage::_OpenByIter (OUT RC<RDataSource> &outDS, FileName::Ref name, const void* ref) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		DEBUG_ONLY(
			auto	iter = _map.find( FileName::Optimized_t{name} );
			CHECK_ERR( iter != _map.end() );
			CHECK_ERR( &iter->second == ref );
		)
		Unused( name );

		return _Open2( OUT outDS, *Cast<FileInfo>( ref ));
	}

/*
=================================================
	_OpenByIter (AsyncRStream)
=================================================
*/
	bool  ArchiveStaticStorage::_OpenByIter (OUT RC<AsyncRStream> &outStream, FileName::Ref name, const void* ref) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( _asyncFile );

		DEBUG_ONLY(
			auto	iter = _map.find( FileName::Optimized_t{name} );
			CHECK_ERR( iter != _map.end() );
			CHECK_ERR( &iter->second == ref );
		)
		Unused( name );

		return _Open2( OUT outStream, *Cast<FileInfo>( ref ));
	}

/*
=================================================
	_OpenByIter (AsyncRDataSource)
=================================================
*/
	bool  ArchiveStaticStorage::_OpenByIter (OUT RC<AsyncRDataSource> &outDS, FileName::Ref name, const void* ref) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( _asyncFile );

		DEBUG_ONLY(
			auto	iter = _map.find( FileName::Optimized_t{name} );
			CHECK_ERR( iter != _map.end() );
			CHECK_ERR( &iter->second == ref );
		)
		Unused( name );

		return _Open2( OUT outDS, *Cast<FileInfo>( ref ));
	}
//-----------------------------------------------------------------------------


/*
=================================================
	CreateStaticArchive
=================================================
*/
	RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateStaticArchive (RC<RDataSource> archive) __NE___
	{
		auto	result = RC<ArchiveStaticStorage>{ new ArchiveStaticStorage{}};
		CHECK_ERR( result->_Create( null, RVRef(archive) ));
		return result;
	}

	RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateStaticArchive (RC<AsyncRDataSource> asyncFile, RC<RDataSource> syncFile) __NE___
	{
		auto	result = RC<ArchiveStaticStorage>{ new ArchiveStaticStorage{}};
		CHECK_ERR( result->_Create( RVRef(asyncFile), RVRef(syncFile) ));
		return result;
	}

	RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateStaticArchive (const Path &filename) __NE___
	{
		AE_LOG_DBG( "Mount static archive '"s << ToString(filename) << "'" );

		auto	result = RC<ArchiveStaticStorage>{ new ArchiveStaticStorage{}};
		CHECK_ERR( result->_Create( filename ));
		return result;
	}


} // AE::VFS
