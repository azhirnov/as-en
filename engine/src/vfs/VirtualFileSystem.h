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

namespace AE { VFS::VirtualFileSystem&  GetVFS (); }

namespace AE::VFS
{

	//
	// Virtual File System Storage interface
	//

	class IVirtualFileStorage : public EnableRC<IVirtualFileStorage>
	{
		friend class VirtualFileSystem;

	// types
	protected:
		struct GlobalFileRef
		{
			IVirtualFileStorage const*	storage;
			void const*					ref;		// map iterator, only for static storage
		};

		using GlobalFileMap_t = FlatHashMap< FileName::Optimized_t, GlobalFileRef >;


	// interface
	public:
		ND_ virtual RC<RStream>		OpenAsStream (const FileName &name) const = 0;
		ND_ virtual RC<RDataSource>	OpenAsSource (const FileName &name) const = 0;

		ND_ virtual bool	Exists (const FileName &name) const = 0;
		ND_ virtual bool	Exists (const FileGroupName &name) const = 0;

	protected:
			virtual void			_Append (INOUT GlobalFileMap_t &) const = 0;
		ND_ virtual RC<RStream>		_OpenAsStreamByIter (const FileName &name, const void* ref) const = 0;
		ND_ virtual RC<RDataSource>	_OpenAsSourceByIter (const FileName &name, const void* ref) const = 0;
	};



	//
	// Virtual File System
	//

	class VirtualFileSystem final
	{
	// types
	private:
		using GlobalFileMap_t = IVirtualFileStorage::GlobalFileMap_t;


	// variables
	private:
		GlobalFileMap_t						_globalMap;
		Array< RC<IVirtualFileStorage> >	_storageArr;
		
		DRC_ONLY( RWDataRaceCheck			_drCheck;)


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();
		
		friend VirtualFileSystem&  AE::GetVFS ();

		ND_ bool  AddStorage (RC<IVirtualFileStorage> storage);
		
		ND_ RC<RStream>		OpenAsStream (const FileName &name) const;
		ND_ RC<RDataSource>	OpenAsSource (const FileName &name) const;

		ND_ bool	Exists (const FileName &name) const;
		ND_ bool	Exists (const FileGroupName &name) const;

	private:
		VirtualFileSystem () {}
		~VirtualFileSystem () {}

		ND_ static VirtualFileSystem*  _Instance ();
	};
	
} // AE::VFS


namespace AE
{
/*
=================================================
	GetVFS
=================================================
*/
	ND_ forceinline VFS::VirtualFileSystem&  GetVFS ()
	{
		return *VFS::VirtualFileSystem::_Instance();
	}

} // AE
