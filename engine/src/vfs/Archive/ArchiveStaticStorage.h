// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/StringToID.h"
#include "base/Utils/FileSystem.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Memory/LinearAllocator.h"
#include "base/Math/PackedInt.h"

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

	//
	// Archive Static Storage
	//

	class ArchiveStaticStorage final : public IVirtualFileStorage
	{
		friend class ArchivePacker;

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
			Raw			= 1 << 0,		// SequentialAccess	| RandomAccess
			Brotli		= 1 << 1,		// SequentialAccess
			InMemory	= 1 << 2,		// RandomAccess | Buffered
		//	Encrypted	= 1 << 3,		// SequentialAccess
			Unknown		= Raw,

			BrotliInMemory			= Brotli | InMemory,
		//	BrotliEncrypted			= Brotli | Encrypted,
		//	BrotliEncryptedInMemory	= Brotli | Encrypted | InMemory,
		};

		struct FileInfo
		{
			uint			size		= 0;
			packed_ulong	offset;
			EFileType		type		= Default;

			ND_ Bytes  Size ()		const	{ return Bytes{size}; }
			ND_ Bytes  Offset ()	const	{ return Bytes{ulong{offset}}; }
		};
		
		struct FileHeader
		{
			FileName::Optimized_t	name;
			FileInfo				info;
		};
		STATIC_ASSERT( sizeof(FileHeader) == 20 );

		using FileMap_t = FlatHashMap< FileName::Optimized_t, FileInfo >;

		static constexpr uint	Name	= uint("VfsArh"_StringToID);
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
		ArchiveStaticStorage () {}
		~ArchiveStaticStorage () override {}

		ND_ bool  Create (RC<RDataSource> archive);
		ND_ bool  Create (const Path &filename);


	  // IVirtualFileStorage //
		RC<RStream>		OpenAsStream (const FileName &name) const override;
		RC<RDataSource>	OpenAsSource (const FileName &name) const override;

		bool	Exists (const FileName &name) const override;
		bool	Exists (const FileGroupName &name) const override;

	private:
		void			_Append (INOUT GlobalFileMap_t &) const override;
		RC<RStream>		_OpenAsStreamByIter (const FileName &name, const void* ref) const override;
		RC<RDataSource>	_OpenAsSourceByIter (const FileName &name, const void* ref) const override;

		ND_ bool  _ReadHeader (RDataSource &ds);
		
		ND_ RC<RStream>		_OpenAsStream (const FileInfo &) const;
		ND_ RC<RDataSource>	_OpenAsSource (const FileInfo &) const;
	};


} // AE::VFS
