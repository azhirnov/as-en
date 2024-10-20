// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/Archive/ArchiveStaticStorage.h"

namespace AE::VFS
{

	//
	// Archive Packer
	//

	class ArchivePacker
	{
	// types
	public:
		using EFileType			= ArchiveStaticStorage::EFileType;

	private:
		using ArchiveHeader		= ArchiveStaticStorage::ArchiveHeader;
		using FileInfo			= ArchiveStaticStorage::FileInfo;
		using FileHeader		= ArchiveStaticStorage::FileHeader;
		using FileMap_t			= ArchiveStaticStorage::FileMap_t;

		static constexpr uint	Name	= ArchiveStaticStorage::Name;
		static constexpr uint	Version	= ArchiveStaticStorage::Version;

		static constexpr Bytes	_MaxInMemoryFileSize {1_Mb};


	// variables
	private:
		FileMap_t		_map;
		Path			_tempFile;
		RC<WStream>		_archive;

		NamedID_HashCollisionCheck	_hashCollisionCheck;
		DRC_ONLY( DataRaceCheck		_drCheck;)


	// methods
	public:
		ArchivePacker ();
		~ArchivePacker ();

		ND_ bool  Create (const Path &tempFile);

		ND_ bool  Store (WStream &dstStream);
		ND_ bool  Store (const Path &filename);

		ND_ bool  Add (const FileName::WithString_t &name, RStream &stream, Bytes size, EFileType type);
		ND_ bool  Add (const FileName::WithString_t &name, RStream &stream, EFileType type);
		ND_ bool  Add (const FileName::WithString_t &name, const Path &filename, EFileType type);

		ND_ bool  AddArchive (const Path &filename);
		ND_ bool  AddArchive (RC<RDataSource> archive);

		ND_ bool  Exists (FileName::Ref	name)	const;
		ND_ bool  IsCreated ()					const;
		ND_ Path  TempFilePath ()				const;

	private:
		ND_ bool  _AddFile (FileName::Optimized_t name, const FileInfo &info);
		ND_ bool  _Store (WStream &dstStream, Bytes archiveSize);

		template <typename StreamType, typename CfgType>
		ND_ uint  _Compression (RStream &stream, const FileName::WithString_t &name, FileInfo &info,
								Bytes startPos, Bytes size, const CfgType &cfg);
		ND_ uint  _ZStdCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size);
		ND_ uint  _BrotliCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size);

		ND_ bool  _AddArchive (ArchiveStaticStorage &);
	};


} // AE::VFS
