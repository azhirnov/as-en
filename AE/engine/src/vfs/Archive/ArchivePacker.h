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

		static constexpr Bytes	_MaxInMemoryFileSize {1_MiB};


	// variables
	private:
		FileMap_t		_map;
		Path			_tempFile;
		RC<WStream>		_archive;

		NamedID_HashCollisionCheck	_hashCollisionCheck;
		DRC_ONLY( DataRaceCheck		_drCheck;)


	// methods
	public:
		ArchivePacker ()																				__NE___;
		~ArchivePacker ()																				__NE___;

		ND_ bool  Create (const Path &tempFile)															__NE___;

		ND_ bool  Store (WStream &dstStream)															__NE___;
		ND_ bool  Store (const Path &filename)															__NE___;

		ND_ bool  Add (const FileName::WithString_t &name, RStream &stream, Bytes size, EFileType type)	__NE___;
		ND_ bool  Add (const FileName::WithString_t &name, RStream &stream, EFileType type)				__NE___;
		ND_ bool  Add (const FileName::WithString_t &name, const Path &filename, EFileType type)		__NE___;

		ND_ bool  AddArchive (const Path &filename)														__NE___;
		ND_ bool  AddArchive (RC<RDataSource> archive)													__NE___;

		ND_ bool  Exists (FileName::Ref	name)															C_NE___;
		ND_ bool  IsCreated ()																			C_NE___;
		ND_ Path  TempFilePath ()																		C_NE___;

	private:
		ND_ bool  _AddFile (FileName::Optimized_t name, const FileInfo &info)							__NE___;
		ND_ bool  _Store (WStream &dstStream, Bytes archiveSize)										__NE___;

		ND_ bool  _AddArchive (ArchiveStaticStorage &)													__NE___;

		template <typename StreamType, typename CfgType>
		ND_ uint  _Compression (RStream &stream, const FileName::WithString_t &name, FileInfo &info,
								Bytes startPos, Bytes size, const CfgType &cfg)							__NE___;
		ND_ uint  _ZStdCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size) __NE___;
		ND_ uint  _BrotliCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size) __NE___;
	};


} // AE::VFS
