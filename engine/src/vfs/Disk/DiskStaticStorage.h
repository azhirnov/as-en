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
		DiskStaticStorage ()																	__NE___	{}
		~DiskStaticStorage ()																	__NE_OV {}

		ND_ bool  Create (const Path &folder, StringView prefix = Default)						__NE___;


	  // IVirtualFileStorage //
		bool  Open (OUT RC<RStream> &stream, const FileName &name)								C_NE_OV;
		bool  Open (OUT RC<RDataSource> &ds, const FileName &name)								C_NE_OV;
		bool  Open (OUT RC<AsyncRDataSource> &ds, const FileName &name)							C_NE_OV;

		bool  Exists (const FileName &name)														C_NE_OV;
		bool  Exists (const FileGroupName &name)												C_NE_OV;

	private:
		bool  _Create (const Path &folder, StringView prefix)									__Th___;

		void  _Append (INOUT GlobalFileMap_t &)													C_Th_OV;

		bool  _OpenByIter (OUT RC<RStream> &stream, const FileName &name, const void* ref)		C_NE_OV;
		bool  _OpenByIter (OUT RC<RDataSource> &ds, const FileName &name, const void* ref)		C_NE_OV;
		bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, const FileName &name, const void* ref)	C_NE_OV;
		
		template <typename ImplType, typename ResultType>
		ND_ bool  _Open (OUT ResultType &, const FileName &name)								C_NE___;

		template <typename ImplType, typename ResultType>
		ND_ bool  _OpenByIter2 (OUT ResultType &, const FileName &name, const void* ref)		C_NE___;
	};


} // AE::VFS
