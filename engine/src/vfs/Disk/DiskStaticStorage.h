// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/FileSystem.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Memory/LinearAllocator.h"

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

	//
	// Disk Static Storage
	//

	class DiskStaticStorage final : public IVirtualFileStorage
	{
	// types
	private:
		using FileMap_t		= FlatHashMap< FileName::Optimized_t, StringView >;
		using Allocator_t	= LinearAllocator<>;


	// variables
	private:
		FileMap_t		_map;
		Path			_folder;
		Allocator_t		_allocator;

		DEBUG_ONLY(
		  NamedID_HashCollisionCheck	_hasCollisionCheck;
		)
		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		DiskStaticStorage () {}
		~DiskStaticStorage () override {}

		ND_ bool  Create (const Path &folder);


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


} // AE::VFS
