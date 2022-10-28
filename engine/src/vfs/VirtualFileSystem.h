// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	File - any data source with fixed size.

	FileGroup - list of file which can be preloaded or preload file headers.

	Storage - collection of files and file groups.

TODO:
	- block allocator for RStream
*/

#pragma once

#include "vfs/Common.h"

namespace AE::VFS
{

	//
	// Virtual File System Storage interface
	//

	class IFileStorage : public EnableRC<IFileStorage>
	{
		friend class VirtualFileSystem;

	// types
	protected:
		struct GlobalFileRef
		{
			IFileStorage const*	storage;
			void const*			ref;		// map iterator, only for static storage
		};

		using GlobalFileMap_t = FlatHashMap< FileName::Optimized_t, GlobalFileRef >;


	// interface
	public:
		ND_ virtual RC<RStream>			Open (const FileName &name) const = 0;
		ND_ virtual RC<AsyncRStream>	OpenAsync (const FileName &name) const = 0;

		//ND_ virtual Promise<void>		LoadAsync (const FileGroupName &name) const = 0;

		ND_ virtual bool	Exists (const FileName &name) const = 0;
		ND_ virtual bool	Exists (const FileGroupName &name) const = 0;

	protected:
			virtual void				_Append (INOUT GlobalFileMap_t &) const = 0;
		ND_ virtual RC<RStream>			_OpenByIter (const FileName &name, const void* ref) const = 0;
		ND_ virtual RC<AsyncRStream>	_OpenAsyncByIter (const FileName &name, const void* ref) const = 0;
	};



	//
	// Virtual File System
	//

	class VirtualFileSystem final
	{
	// types
	private:
		using GlobalFileMap_t = IFileStorage::GlobalFileMap_t;


	// variables
	private:
		GlobalFileMap_t				_globalMap;
		Array< RC<IFileStorage> >	_storageArr;
		
		DRC_ONLY(
			RWDataRaceCheck			_drCheck;
		)


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();
		
		friend VirtualFileSystem&  GetVFS ();

		ND_ bool  AddStorage (RC<IFileStorage> storage);

		ND_ RC<RStream>			Open (const FileName &name) const;
		ND_ RC<AsyncRStream>	OpenAsync (const FileName &name) const;

		//ND_ Promise<void>		LoadAsync (const FileGroupName &name) const;

		ND_ bool	Exists (const FileName &name) const;
		ND_ bool	Exists (const FileGroupName &name) const;

	private:
		VirtualFileSystem () {}
		~VirtualFileSystem () {}

		ND_ static VirtualFileSystem*  _Instance ();
	};

	
/*
=================================================
	GetVFS
=================================================
*/
	ND_ forceinline VirtualFileSystem&  GetVFS ()
	{
		return *VirtualFileSystem::_Instance();
	}

} // AE::VFS
