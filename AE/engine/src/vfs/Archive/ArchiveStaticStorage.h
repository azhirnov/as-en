// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

	//
	// Archive Static Storage
	//

	class ArchiveStaticStorage final : public IVirtualFileStorage
	{
		friend class ArchivePacker;
		friend class VirtualFileStorageFactory;

	// types
	private:
		struct ArchiveHeader
		{
			uint	name;
			uint	version;
			uint	fileHeadersSize;	// array of FileInfo
		};

		enum class EFileType : uint
		{
			Raw			= 1 << 0,		// SequentialAccess	or RandomAccess
			Brotli		= 1 << 1,		// SequentialAccess
			InMemory	= 1 << 2,		// RandomAccess | Buffered
		//	Encrypted	= 1 << 3,		// SequentialAccess
			ZStd		= 1 << 4,		// SequentialAccess
			_Last,
			All			= ((_Last - 1) << 1) - 1,
			Unknown		= 0,

			BrotliInMemory			= Brotli | InMemory,
		//	BrotliEncrypted			= Brotli | Encrypted,
		//	BrotliEncryptedInMemory	= Brotli | Encrypted | InMemory,
			ZStdInMemory			= ZStd | InMemory,
		//	ZStdEncrypted			= ZStd | Encrypted,
		//	ZStdEncryptedInMemory	= ZStd | Encrypted | InMemory,
		};

		struct FileInfo
		{
			uint			size		= 0;
			packed_ulong	offset;
			EFileType		type		= Default;

			ND_ Bytes  Size ()		C_NE___	{ return Bytes{size}; }
			ND_ Bytes  Offset ()	C_NE___	{ return Bytes{ulong{offset}}; }
		};

		struct FileHeader
		{
			FileName::Optimized_t	name;
			FileInfo				info;
		};
		StaticAssert( sizeof(FileHeader) == 20 );

		using FileMap_t = FlatHashMap< FileName::Optimized_t, FileInfo >;

		static constexpr uint	Name	= "VfsArch"_Hash;
		static constexpr uint	Version = (1 << 12) | (sizeof(FileHeader) & 0xFFF);


	// variables
	private:
		FileMap_t			_map;
		RC<RDataSource>		_archive;

		DRC_ONLY(
			RWDataRaceCheck	_drCheck;
		)


	// methods
	public:

	  // IVirtualFileStorage //
		bool  Open (OUT RC<RStream> &stream, FileName::Ref name)							C_NE_OV;
		bool  Open (OUT RC<RDataSource> &ds, FileName::Ref name)							C_NE_OV;
		bool  Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name)						C_NE_OV;

		using IVirtualFileStorage::Open;

		bool  Exists (FileName::Ref name)													C_NE_OV;
		bool  Exists (FileGroupName::Ref name)												C_NE_OV;


	private:
		void  _Append (INOUT GlobalFileMap_t &)												C_Th_OV;

		bool  _OpenByIter (OUT RC<RStream> &stream, FileName::Ref, const void* ref)			C_NE_OV;
		bool  _OpenByIter (OUT RC<RDataSource> &ds, FileName::Ref, const void* ref)			C_NE_OV;
		bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, FileName::Ref, const void* ref)	C_NE_OV;

		using IVirtualFileStorage::_OpenByIter;

		bool  _Open2 (OUT RC<RStream> &stream, const FileInfo &info)						C_NE___;
		bool  _Open2 (OUT RC<RDataSource> &ds, const FileInfo &info)						C_NE___;
	//	bool  _Open2 (OUT RC<AsyncRDataSource> &ds, const FileInfo &info)					C_NE___;

		ND_ bool  _ReadHeader (RDataSource &ds)												__NE___;


	private:
		ArchiveStaticStorage ()																__NE___	{}
		~ArchiveStaticStorage ()															__NE_OV {}

		ND_ bool  _Create (RC<RDataSource> archive)											__NE___;
		ND_ bool  _Create (const Path &filename)											__NE___;
	};


} // AE::VFS
