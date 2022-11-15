// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/FileSystem.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Memory/LinearAllocator.h"

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

	//
	// Archive Dynamic Storage
	//

	class ArchiveDynamicStorage final : public IVirtualFileStorage
	{
	// types
	private:
		using FileMap_t		= FlatHashMap< FileName::Optimized_t, StringView >;
		using Allocator_t	= LinearAllocator<>;


	// variables
	private:

		DEBUG_ONLY(
		  NamedID_HashCollisionCheck	_hasCollisionCheck;
		)
		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		ArchiveDynamicStorage () {}
		~ArchiveDynamicStorage () override {}

		ND_ bool  Create (RC<RStream> archive);


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
