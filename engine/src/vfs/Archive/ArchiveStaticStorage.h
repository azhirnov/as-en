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

	class ArchiveStaticStorage final : public IFileStorage
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
			Raw,
			Brotli,
			//Encrypted,
			Unknown	= Raw,
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
		FileMap_t		_map;
		RC<RStream>		_archive;

		DRC_ONLY(
			RWDataRaceCheck	_drCheck;
		)


	// methods
	public:
		ArchiveStaticStorage () {}
		~ArchiveStaticStorage () override {}

		ND_ bool  Create (RC<RStream> archive);
		ND_ bool  Create (const Path &filename);


	  // IFileStorage //
		RC<RStream>			Open (const FileName &name) const override;
		RC<AsyncRStream>	OpenAsync (const FileName &name) const override;

		//Promise<void>		LoadAsync (const FileGroupName &name) const override;

		bool	Exists (const FileName &name) const override;
		bool	Exists (const FileGroupName &name) const override;

	private:
		void			  _Append (INOUT GlobalFileMap_t &) const override;
		RC<RStream>		  _OpenByIter (const FileName &name, const void* ref) const override;
		RC<AsyncRStream>  _OpenAsyncByIter (const FileName &name, const void* ref) const override;

		ND_ bool  _ReadHeader (RStream &stream);

		ND_ RC<RStream>  _Open (const FileInfo &) const;
	};


} // AE::VFS
