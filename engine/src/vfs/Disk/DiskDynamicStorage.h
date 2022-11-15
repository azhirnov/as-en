// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/FileSystem.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Memory/LinearAllocator.h"

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

	//
	// Disk Dynamic Storage
	//

	class DiskDynamicStorage final : public IVirtualFileStorage
	{
	// types
	private:
		using FileMap_t		= FlatHashMap< FileName::Optimized_t, String >;
		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;

		static constexpr seconds	_UpdateInterval {10};


	// variables
	private:
		mutable SharedMutex		_mapGuard;
		mutable FileMap_t		_map;
		mutable TimePoint_t		_lastUpdate;
		Path					_folder;

		DEBUG_ONLY(
		  mutable NamedID_HashCollisionCheck	_hasCollisionCheck;
		)


	// methods
	public:
		DiskDynamicStorage () {}
		~DiskDynamicStorage () override {}

		ND_ bool  Create (const Path &folder);


	  // IVirtualFileStorage //
		RC<RStream>		OpenAsStream (const FileName &name) const override;
		RC<RDataSource>	OpenAsSource (const FileName &name) const override;

		bool	Exists (const FileName &name) const override;
		bool	Exists (const FileGroupName &name) const override;

	private:
		void				_Append (INOUT GlobalFileMap_t &) const override					{}
		RC<RStream>			_OpenAsStreamByIter (const FileName &, const void*) const override	{ DBG_WARNING("not supported");  return Default; }
		RC<RDataSource>		_OpenAsSourceByIter (const FileName &, const void*) const override	{ DBG_WARNING("not supported");  return Default; }
		
		ND_ RC<RStream>		_OpenAsStream (const FileName &name) const;
		ND_ RC<RDataSource>	_OpenAsSource (const FileName &name) const;
		ND_ bool			_Exists (const FileName &name) const;

		ND_ bool  _Update () const;
	};


} // AE::VFS
