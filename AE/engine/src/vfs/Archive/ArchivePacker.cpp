// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Archive/ArchivePacker.h"

namespace AE::VFS
{

/*
=================================================
	constructor
=================================================
*/
	ArchivePacker::ArchivePacker ()
	{}

/*
=================================================
	destructor
=================================================
*/
	ArchivePacker::~ArchivePacker ()
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _map.empty() );
		CHECK( _archive == null );
	}

/*
=================================================
	Create
=================================================
*/
	bool  ArchivePacker::Create (const Path &tempFile)
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _archive == null );

		_archive = MakeRC<FileWStream>( tempFile, FileWStream::EMode::OpenRewrite );
		CHECK_ERR( _archive->IsOpen() );

		_map.clear();
		_tempFile = FileSystem::ToAbsolute( tempFile );

		return true;
	}

/*
=================================================
	IsCreated
=================================================
*/
	bool  ArchivePacker::IsCreated () const
	{
		DRC_EXLOCK( _drCheck );
		return _archive != null;
	}

/*
=================================================
	TempFilePath
=================================================
*/
	Path  ArchivePacker::TempFilePath () const
	{
		DRC_EXLOCK( _drCheck );
		return _tempFile;
	}

/*
=================================================
	Store
=================================================
*/
	bool  ArchivePacker::Store (WStream &dstStream)
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _archive );
		CHECK_ERR( dstStream.IsOpen() );

		_archive->Flush();

		const Bytes	archive_size = _archive->Position();

		// close write-only file and open for reading
		_archive = null;

		CHECK_ERR( _Store( dstStream, archive_size ));

		// reopen
		/*_archive = MakeRC<FileWStream>( _tempFile, FileWStream::EMode::Append );
		CHECK_ERR( _archive->IsOpen() );
		CHECK_ERR( archive_size == _archive->Size() );
		*/
		return true;
	}

	bool  ArchivePacker::Store (const Path &filename)
	{
		Path	folder = filename.parent_path();
		if ( not folder.empty() )
			FileSystem::CreateDirectories( folder );

		FileWStream		file{ filename };
		return Store( file );
	}

	bool  ArchivePacker::_Store (WStream &dstStream, Bytes archiveSize)
	{
		FileRStream		src_file {_tempFile};
		CHECK_ERR( src_file.IsOpen() );

		CHECK_ERR( not _hashCollisionCheck.HasCollisions() );

		const Bytes	fhdr_size {_map.size() * sizeof(FileHeader)};
		CHECK_ERR( fhdr_size == uint(fhdr_size) );

		ArchiveHeader	hdr;
		hdr.name			= Name;
		hdr.version			= Version;
		hdr.fileHeadersSize	= uint(fhdr_size);

		CHECK_ERR( dstStream.Write( hdr ));

		const Bytes	base_offset {sizeof(hdr) + hdr.fileHeadersSize};
		archiveSize += base_offset;

		for (auto& [name, info] : _map)
		{
			auto	temp	= info;
			Bytes	offset	= info.Offset() + base_offset;

			temp.offset	= ulong{offset};

			CHECK_ERR( temp.size > 0 );
			CHECK_ERR( temp.Offset() + temp.size <= archiveSize );

			CHECK_ERR( dstStream.Write( name ) and dstStream.Write( temp ));
		}

		CHECK_ERR( DataSourceUtils::BufferedCopy( dstStream, src_file ) == src_file.Size() );

		return true;
	}

/*
=================================================
	Add
=================================================
*/
	bool  ArchivePacker::Add (const FileName::WithString_t &name, RStream &stream, const Bytes size, const EFileType type)
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _archive );
		CHECK_ERR( stream.IsOpen() );
		CHECK_ERR( size > 0_b );

		_hashCollisionCheck.Add( name );
		CHECK_ERR( not _map.contains( name ));

		const Bytes	start_pos = stream.Position();

		FileInfo	info;
		const Bytes	offset = _archive->Position();

		info.size	= 0;
		info.offset = ulong{offset};
		info.type	= type;

		const auto	PutFile = [&] () -> bool
		{{
			const Bytes	copied = DataSourceUtils::BufferedCopy( *_archive, stream, size );
			CHECK_ERR( copied == (stream.Position() - start_pos) );

			info.size = uint(copied);

			return _AddFile( FileName::Optimized_t{name}, info );
		}};

		switch ( type )
		{
			case EFileType::InMemory :
			case EFileType::BrotliInMemory :
				CHECK( size <= _MaxInMemoryFileSize );
				break;
		}

		switch_enum( type )
		{
			// copy without compression
			case EFileType::Raw :
			case EFileType::InMemory :
				return PutFile();

			// use brotli compression
			case EFileType::Brotli :
			case EFileType::BrotliInMemory :
			{
				switch ( _BrotliCompression( stream, name, info, start_pos, size ))
				{
					case 0 :	return false;	// error
					case 1 :	return true;	// ok
				}

				// fallback to non-compressed
				info.type = EFileType::Raw;
				return PutFile();
			}

			// use zstd compression
			case EFileType::ZStd :
			case EFileType::ZStdInMemory :
			{
				switch ( _ZStdCompression( stream, name, info, start_pos, size ))
				{
					case 0 :	return false;	// error
					case 1 :	return true;	// ok
				}

				// fallback to non-compressed
				info.type = EFileType::Raw;
				return PutFile();
			}

			case EFileType::Unknown :
			case EFileType::_Last :
			case EFileType::All :
				break;
		}
		switch_end
		RETURN_ERR( "unknown file type" );
	}

	bool  ArchivePacker::Add (const FileName::WithString_t &name, RStream &stream, EFileType type)
	{
		return Add( name, stream, stream.RemainingSize(), type );
	}

	bool  ArchivePacker::Add (const FileName::WithString_t &name, const Path &filename, EFileType type)
	{
		FileRStream	file {filename};
		return Add( name, file, file.RemainingSize(), type );
	}

/*
=================================================
	_BrotliCompression
=================================================
*/
	uint  ArchivePacker::_BrotliCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size)
	{
	#ifdef AE_ENABLE_BROTLI
		BrotliWStream::Config	cfg;
		cfg.inBlockSize	= 1.0f;
		cfg.quality		= 1.0f;
		cfg.windowBits	= 1.0f;

		ASSERT( AllBits( info.type, EFileType::Brotli ));
		return _Compression<BrotliWStream>( stream, name, info, startPos, size, cfg );
	#else

		Unused( stream, name, info, startPos, size );
		return 2;	// not supported
	#endif
	}

/*
=================================================
	_ZStdCompression
=================================================
*/
	uint  ArchivePacker::_ZStdCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size)
	{
	#ifdef AE_ENABLE_ZSTD
		ZStdWStream::Config	cfg;
		cfg.level	= 1.0f;

		ASSERT( AllBits( info.type, EFileType::ZStd ));
		return _Compression<ZStdWStream>( stream, name, info, startPos, size, cfg );
	#else

		Unused( stream, name, info, startPos, size );
		return 2;	// not supported
	#endif
	}

/*
=================================================
	_Compression
=================================================
*/
	template <typename StreamType, typename CfgType>
	uint  ArchivePacker::_Compression (RStream &stream, const FileName::WithString_t &name, FileInfo &info,
										Bytes startPos, Bytes size, const CfgType &cfg)
	{
		auto	mem = MakeRC<ArrayWStream>();
		Bytes	uncompressed_size;
		{
			StreamType	compressed { mem, cfg };
			CHECK_ERR( compressed.IsOpen() );

			uncompressed_size = DataSourceUtils::BufferedCopy( compressed, stream, size );
			CHECK_ERR( uncompressed_size == (stream.Position() - startPos) );
		}

		Bytes	compressed_size = mem->Position();
		CHECK_ERR( compressed_size > 0 );

		const double	compression_ratio = double(ulong(compressed_size)) / ulong(uncompressed_size);

		// some data con not be compressed
		if_likely( compression_ratio < 0.9 )
		{
			info.size = uint(compressed_size);

			auto	rmem = MakeRC<MemRefRStream>( mem->GetData() );
			CHECK_ERR( DataSourceUtils::BufferedCopy( *_archive, *rmem ) == compressed_size );

			CHECK_ERR( _AddFile( FileName::Optimized_t{name}, info ));
			return 1;	// ok
		}
		else
		{
			AE_LOGI( "File with name '"s << name.GetName() << "' has low compression ratio" );
			return 2;	// low compression
		}
	}

/*
=================================================
	_AddFile
=================================================
*/
	bool  ArchivePacker::_AddFile (FileName::Optimized_t name, const FileInfo &info)
	{
		CHECK_ERR( info.Offset() + info.size <= _archive->Position() );
		CHECK_ERR( info.size > 0 );

		_map.emplace( name, info );
		return true;
	}

/*
=================================================
	AddArchive
=================================================
*/
	bool  ArchivePacker::AddArchive (const Path &filename)
	{
		ArchiveStaticStorage	tmp_archive;
		CHECK_ERR( tmp_archive._Create( filename ));
		return _AddArchive( tmp_archive );
	}

	bool  ArchivePacker::AddArchive (RC<RDataSource> archiveDS)
	{
		ArchiveStaticStorage	tmp_archive;
		CHECK_ERR( tmp_archive._Create( archiveDS ));
		return _AddArchive( tmp_archive );
	}

	bool  ArchivePacker::_AddArchive (ArchiveStaticStorage &storage)
	{
		using ArchiveStream_t = RDataSourceAsStream< RC<RDataSource> >;

		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _archive );

		for (auto& [name, src_info] : storage._map)
		{
			CHECK_ERR( not _map.contains( name ));

			auto		stream		= MakeRC<ArchiveStream_t>( storage._archive, src_info.Offset(), src_info.Size() );
			const Bytes	start_pos	= stream->Position();
			const Bytes	offset		= _archive->Position();

			FileInfo	info;
			info.size	= 0;
			info.offset = ulong{offset};
			info.type	= src_info.type;

			const Bytes	copied = DataSourceUtils::BufferedCopy( *_archive, *stream, src_info.Size() );
			CHECK_ERR( copied == (stream->Position() - start_pos) );
			CHECK_ERR( copied == src_info.Size() );

			info.size	= uint(copied);

			CHECK_ERR( _AddFile( name, info ));
		}

		return true;
	}

/*
=================================================
	Exists
=================================================
*/
	bool  ArchivePacker::Exists (FileName::Ref name) const
	{
		DRC_EXLOCK( _drCheck );
		return _map.contains( name );
	}


} // AE::VFS
