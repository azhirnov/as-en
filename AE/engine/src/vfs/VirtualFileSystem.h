// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	File - any data source with fixed size.

	FileGroup - list of file which can be preloaded or preload file headers.

	Storage - collection of files and file groups.
----

	RStream, RDataSource, AsyncRDataSource
		Thread-safe:	depends on implementation, use 'IsThreadSafe()' method.

	WStream, WDataSource, AsyncWDataSource
		Thread-safe:	depends on implementation, use 'IsThreadSafe()' method.

	IVirtualFileStorage
		Thread-safe:	yes (interface methods only)

	VirtualFileSystem
		Thread-safe:	yes (const methods only)
----

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/VirtualFileSystem-ru.md)
*/

#pragma once

#include "vfs/Common.h"

namespace AE { VFS::VirtualFileSystem&  GetVFS () __NE___; }

namespace AE::VFS
{

	//
	// Virtual File System Storage interface
	//
	class IVirtualFileStorage : public EnableRC<IVirtualFileStorage>
	{
	// types
	public:
		struct GlobalFileRef
		{
			IVirtualFileStorage const*	storage;
			void const*					ref;		// map iterator, only for static storage
		};

		using GlobalFileMap_t	= FlatHashMap< FileName::Optimized_t, GlobalFileRef >;


	// interface
	public:
		ND_ virtual bool  Open (OUT RC<RStream> &stream, FileName::Ref name)								C_NE___ = 0;
		ND_ virtual bool  Open (OUT RC<RDataSource> &ds, FileName::Ref name)								C_NE___ = 0;
		ND_ virtual bool  Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name)							C_NE___ = 0;

		ND_ virtual bool  Open (OUT RC<WStream> &stream, FileName::Ref name)								C_NE___ { Unused( stream, name );	return false; }
		ND_ virtual bool  Open (OUT RC<WDataSource> &ds, FileName::Ref name)								C_NE___ { Unused( ds, name );		return false; }
		ND_ virtual bool  Open (OUT RC<AsyncWDataSource> &ds, FileName::Ref name)							C_NE___ { Unused( ds, name );		return false; }

		ND_ virtual bool  CreateFile (OUT FileName &name, const Path &path)									C_NE___ { Unused( name, path );		return false; }
		ND_ virtual bool  CreateUniqueFile (OUT FileName &name, INOUT Path &path)							C_NE___ { Unused( name, path );		return false; }

		ND_ virtual bool  Exists (FileName::Ref name)														C_NE___ = 0;
		ND_ virtual bool  Exists (FileGroupName::Ref name)													C_NE___ = 0;


	// for VirtualFileSystem
			virtual void  _Append (INOUT GlobalFileMap_t &)													C_Th___ = 0;

		ND_ virtual bool  _OpenByIter (OUT RC<RStream> &stream, FileName::Ref name, const void* ref)		C_NE___ = 0;
		ND_ virtual bool  _OpenByIter (OUT RC<RDataSource> &ds, FileName::Ref name, const void* ref)		C_NE___ = 0;
		ND_ virtual bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, FileName::Ref name, const void* ref)	C_NE___ = 0;

		ND_ virtual bool  _OpenByIter (OUT RC<WStream> &stream, FileName::Ref name, const void* ref)		C_NE___ { Unused( stream, name, ref );	return false; }
		ND_ virtual bool  _OpenByIter (OUT RC<WDataSource> &ds, FileName::Ref name, const void* ref)		C_NE___ { Unused( ds, name, ref );		return false; }
		ND_ virtual bool  _OpenByIter (OUT RC<AsyncWDataSource> &ds, FileName::Ref name, const void* ref)	C_NE___ { Unused( ds, name, ref );		return false; }
	};



	//
	// Virtual File System
	//
	class VirtualFileSystem final
	{
		friend struct InPlace<VirtualFileSystem>;

	// types
	private:
		using GlobalFileMap_t	= IVirtualFileStorage::GlobalFileMap_t;
		using StorageArray_t	= FixedMap< StorageName::Optimized_t, RC<IVirtualFileStorage>, 8 >;

	public:
		class InstanceCtor {
		public:
			static void  Create ()	__NE___;
			static void  Destroy ()	__NE___;
		};


	// variables
	private:
		Atomic<bool>				_isImmutable	{false};

		GlobalFileMap_t				_globalMap;		// \__ immutable if '_isImmutable' is 'true'.
		StorageArray_t				_storageMap;	// /

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:

		// Add file storage to the VFS.
		// All static files will be added to the global map.
		// FileName hash must be unique for all files in the VFS.
		//	Thread-safe:  no
		//
		ND_ bool  AddStorage (StorageName::Ref, RC<IVirtualFileStorage> storage)				__NE___;
		ND_ bool  AddStorage (RC<IVirtualFileStorage> storage)									__NE___;


		// VFS has 2 stages:
		//	1 - mutable: you can add storage, but can not open files.
		//	2 - immutable: you can open files for read/write, but can not add new storages.
		//	Thread-safe:  no
		//
			bool  MakeImmutable ()																__NE___;


		ND_ bool  Open (OUT RC<RStream> &stream, FileName::Ref name)							C_NE___;
		ND_ bool  Open (OUT RC<RDataSource> &ds, FileName::Ref name)							C_NE___;
		ND_ bool  Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name)						C_NE___;
		ND_ bool  Open (OUT RC<AsyncRStream> &stream, FileName::Ref name)						C_NE___;

		ND_ bool  Open (OUT RC<WStream> &stream, FileName::Ref name)							C_NE___;
		ND_ bool  Open (OUT RC<WDataSource> &ds, FileName::Ref name)							C_NE___;
		ND_ bool  Open (OUT RC<AsyncWDataSource> &ds, FileName::Ref name)						C_NE___;
		ND_ bool  Open (OUT RC<AsyncWStream> &stream, FileName::Ref name)						C_NE___;

		template <typename T>
		ND_ RC<T>  Open (FileName::Ref name)													C_NE___;

		ND_ bool  CreateFile (OUT FileName		&name,
							  const Path		&path,
							  StorageName::Ref	storage)										C_NE___;

		ND_ bool  CreateUniqueFile (OUT FileName		&name,
									INOUT Path			&path,
									StorageName::Ref	storage)								C_NE___;

		ND_ bool  Exists (FileName::Ref name)													C_NE___;
		ND_ bool  Exists (FileGroupName::Ref name)												C_NE___;


	private:
		VirtualFileSystem ()																	__NE___	{}
		~VirtualFileSystem ()																	__NE___	{}

		ND_ bool  _AddStorage (StorageName::Optimized_t name, RC<IVirtualFileStorage>)			__NE___;

		template <typename ResultType>
		ND_ bool  _OpenForRead (OUT ResultType &, FileName::Ref name)							C_NE___;

		template <typename ResultType>
		ND_ bool  _OpenForWrite (OUT ResultType &, FileName::Ref name)							C_NE___;

		friend VirtualFileSystem&		AE::GetVFS ()											__NE___;
		ND_ static VirtualFileSystem&	_Instance ()											__NE___;
	};



	//
	// Virtual File Storage Factory
	//
	class VirtualFileStorageFactory : public Noninstanceable
	{
	public:
		ND_ static RC<IVirtualFileStorage>  CreateStaticArchive (RC<RDataSource> archive)							__NE___;
		ND_ static RC<IVirtualFileStorage>  CreateStaticArchive (const Path &filename)								__NE___;

		ND_ static RC<IVirtualFileStorage>  CreateStaticFolder (const Path &folder, StringView prefix = Default)	__NE___;
		ND_ static RC<IVirtualFileStorage>  CreateDynamicFolder (const Path &folder, StringView prefix = Default,
																 Bool createFolder = False{})						__NE___;

		ND_ static RC<IVirtualFileStorage>  CreateNetworkStorage (Networking::ClientServerBase &,
																  StringView prefix = Default)						__NE___;
	};


/*
=================================================
	Open
=================================================
*/
	template <typename T>
	RC<T>  VirtualFileSystem::Open (FileName::Ref name) C_NE___
	{
		RC<T>	result;
		return Open( OUT result, name ) ? RVRef(result) : null;
	}


} // AE::VFS


namespace AE
{
/*
=================================================
	GetVFS
=================================================
*/
	ND_ inline VFS::VirtualFileSystem&  GetVFS () __NE___
	{
		return VFS::VirtualFileSystem::_Instance();
	}

} // AE
